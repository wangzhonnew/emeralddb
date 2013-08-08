/*******************************************************************************
   Copyright (C) 2013 SequoiaDB Software Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License, version 3,
   as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program. If not, see <http://www.gnu.org/license/>.
*******************************************************************************/
#include "ossSocket.hpp"
#include "pd.hpp"

// Create a listening socket
_ossSocket::_ossSocket ( unsigned int port, int timeout )
{
   _init = false ;
   _fd = 0 ;
   _timeout = timeout ;
   memset ( &_sockAddress, 0, sizeof(sockaddr_in) ) ;
   memset ( &_peerAddress, 0, sizeof(sockaddr_in) ) ;
   _peerAddressLen = sizeof(_peerAddress) ;
   _sockAddress.sin_family = AF_INET ;
   _sockAddress.sin_addr.s_addr = htonl ( INADDR_ANY ) ;
   _sockAddress.sin_port = htons ( port ) ;
   _addressLen = sizeof( _sockAddress ) ;
}

// Create a socket
_ossSocket::_ossSocket ()
{
   _init = false ;
   _fd = 0 ;
   _timeout = 0 ;
   memset ( &_sockAddress, 0, sizeof(sockaddr_in) ) ;
   memset ( &_peerAddress, 0, sizeof(sockaddr_in) ) ;
   _peerAddressLen = sizeof(_peerAddress) ;
   _addressLen = sizeof( _sockAddress ) ;
}

// Create a connecting socket
_ossSocket::_ossSocket ( const char *pHostname, unsigned int port, int timeout )
{
   struct hostent *hp ;
   _init = false ;
   _timeout = timeout ;
   _fd = 0 ;
   memset ( &_sockAddress, 0, sizeof(sockaddr_in) ) ;
   memset ( &_peerAddress, 0, sizeof(sockaddr_in) ) ;
   _peerAddressLen = sizeof(_peerAddress) ;
   _sockAddress.sin_family = AF_INET ;
   if ( (hp = gethostbyname ( pHostname ) ) )
      _sockAddress.sin_addr.s_addr = *((int*)hp->h_addr_list[0] ) ;
   else
      _sockAddress.sin_addr.s_addr = inet_addr ( pHostname ) ;
   _sockAddress.sin_port = htons ( port ) ;
   _addressLen = sizeof(_sockAddress ) ;
}

// Create from a existing socket
_ossSocket::_ossSocket ( SOCKET *sock, int timeout )
{
   int rc = EDB_OK ;
   _fd = *sock ;
   _init = true ;
   _timeout = timeout ;
   _addressLen = sizeof( _sockAddress ) ;
   memset ( &_peerAddress, 0, sizeof(sockaddr_in) ) ;
   _peerAddressLen = sizeof(_peerAddress) ;
   rc = getsockname ( _fd, (sockaddr*)&_sockAddress, &_addressLen ) ;
   if ( rc )
   {
      PD_LOG ( PDERROR, "Failed to get sock name, error = %d",
               SOCKET_GETLASTERROR ) ;
      _init = false ;
   }
   else
   {
      rc = getpeername ( _fd, (sockaddr*)&_peerAddress, &_peerAddressLen ) ;
      PD_RC_CHECK ( rc, PDERROR, "Failed to get peer name, error = %d",
                    SOCKET_GETLASTERROR ) ;
   }
done :
   return ;
error :
   goto done ;
}

int _ossSocket::initSocket ()
{
   int rc = EDB_OK ;
   if ( _init )
   {
      goto done ;
   }
   memset ( &_peerAddress, 0, sizeof(sockaddr_in) ) ;
   _peerAddressLen = sizeof(_peerAddress) ;
   _fd =socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ;
   if ( -1 == _fd )
   {
      PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                    "Failed to initialize socket, error = %d",
                    SOCKET_GETLASTERROR ) ;
   }
   _init =true ;
   // set timeout
   setTimeout ( _timeout ) ;
done :
   return rc ;
error :
   goto done ;
}

int _ossSocket::setSocketLi ( int lOnOff, int linger )
{
   int rc = EDB_OK ;
   struct linger _linger ;
   _linger.l_onoff = lOnOff ;
   _linger.l_linger = linger ;
   rc = setsockopt ( _fd, SOL_SOCKET, SO_LINGER,
                     (const char*)&_linger, sizeof(_linger) ) ;
   return rc ;
}

void _ossSocket::setAddress(const char *pHostname, unsigned int port )
{
   struct hostent *hp ;
   memset ( &_sockAddress, 0, sizeof(sockaddr_in ) ) ;
   memset ( &_peerAddress, 0, sizeof(sockaddr_in) ) ;
   _peerAddressLen = sizeof(_peerAddress) ;
   _sockAddress.sin_family = AF_INET ;
   if ( ( hp = gethostbyname ( pHostname )))
      _sockAddress.sin_addr.s_addr = *((int*)hp->h_addr_list[0] ) ;
   else
      _sockAddress.sin_addr.s_addr = inet_addr ( pHostname ) ;

   _sockAddress.sin_port = htons ( port ) ;
   _addressLen = sizeof(_sockAddress ) ;
}

int _ossSocket::bind_listen ()
{
   int rc = EDB_OK ;
   int temp = 1 ;
   rc = setsockopt ( _fd, SOL_SOCKET,
                     SO_REUSEADDR, (char*)&temp, sizeof(int) ) ;
   if ( rc )
   {
      PD_LOG ( PDWARNING, "Failed to setsockopt SO_REUSEADDR, rc = %d",
               SOCKET_GETLASTERROR ) ;
   }
   rc = setSocketLi ( 1, 30 ) ;
   if ( rc )
   {
      PD_LOG ( PDWARNING, "Failed to setsockopt SO_LINGER, rc = %d",
               SOCKET_GETLASTERROR ) ;
   }
   rc = ::bind ( _fd, (struct sockaddr*)&_sockAddress, _addressLen ) ;
   if ( rc )
   {
      PD_RC_CHECK( EDB_NETWORK, PDERROR,
                   "Failed to bind socket, rc = %d", SOCKET_GETLASTERROR ) ;
   }
   rc = listen ( _fd, SOMAXCONN ) ;
   if ( rc )
   {
      PD_RC_CHECK( EDB_NETWORK, PDERROR,
                   "Failed to listen socket, rc = %d", SOCKET_GETLASTERROR ) ;
   }
done :
   return rc ;
error :
   close () ;
   goto done ;
}

int _ossSocket::send ( const char *pMsg, int len,
                      int timeout, int flags )
{
   int rc = EDB_OK ;
	SOCKET maxFD;
#ifdef _WINDOWS
	maxFD = 0;
#else
	maxFD = _fd ;
#endif
   struct timeval maxSelectTime ;
   fd_set fds ;

   maxSelectTime.tv_sec = timeout / 1000000 ;
   maxSelectTime.tv_usec = timeout % 1000000 ;
   // if len == 0, then let's just return
   if ( 0 == len )
   {
      return EDB_OK ;
   }
   // wait loop until socket is ready
   while ( true )
   {
      FD_ZERO ( &fds ) ;
      FD_SET ( _fd, &fds ) ;
      rc = select ( (int)(maxFD + 1), NULL, &fds, NULL,
                    timeout>=0?&maxSelectTime:NULL ) ;
      if ( 0 == rc )
      {
         // timeout
         rc = EDB_TIMEOUT ;
         goto done ;
      }
      // if < 0, something wrong
      if ( 0 > rc )
      {
         rc = SOCKET_GETLASTERROR ;
         // if we failed due to interrupt, let's continue
         if ( OSS_EINTR == rc )
         {
            continue ;
         }
         PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                       "Failed to select from socket, rc = %d", rc ) ;
      }
      if ( FD_ISSET ( _fd, &fds ) )
      {
         break ;
      }
   }
   while ( len > 0 )
   {
      // MSG_NOSIGNAL: Requests not to send SIGPIPE on errors on stream oriented sockets
      // when the other end breaks the connection. The EPIPE error is still returned
      rc = ::send ( _fd, pMsg, len, MSG_NOSIGNAL | flags ) ;
      if ( -1 == rc )
      {
         PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                       "Failed to send, rc = %d", SOCKET_GETLASTERROR ) ;
      }
      len -= rc ;
      pMsg += rc ;
   }
   rc = EDB_OK ;
done :
   return rc ;
error :
   goto done ;
}

bool _ossSocket::isConnected ()
{
   int rc = EDB_OK ;
   rc = ::send ( _fd, "", 0, MSG_NOSIGNAL ) ;
   if ( 0 > rc )
      return false ;
   return true ;
}

#define MAX_RECV_RETRIES 5
int _ossSocket::recv ( char *pMsg, int len,
                      int timeout, int flags )
{
   int rc = EDB_OK ;
   int retries = 0 ;
   int maxFD = (int)_fd ;
   struct timeval maxSelectTime ;
   fd_set fds ;

   if ( 0 ==len )
      return EDB_OK ;
   maxSelectTime.tv_sec = timeout / 1000000 ;
   maxSelectTime.tv_usec = timeout % 1000000 ;
   while ( true )
   {
      FD_ZERO ( &fds ) ;
      FD_SET ( _fd, &fds ) ;
      rc = select ( maxFD+1, &fds, NULL, NULL,
                    timeout>=0?&maxSelectTime:NULL ) ;
      // 0 means timeout
      if ( 0 == rc )
      {
         rc = EDB_TIMEOUT ;
         goto done ;
      }
      // if < 0 something wrong
      if ( 0 > rc )
      {
         rc = SOCKET_GETLASTERROR ;
         if ( OSS_EINTR==rc )
         {
            continue ;
         }
         PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                       "Failed to select from socket, rc = %d", rc ) ;
      }
      if ( FD_ISSET ( _fd, &fds ) )
      {
          break ;
      }
   }
   while ( len > 0 )
   {
      rc = ::recv ( _fd, pMsg, len, MSG_NOSIGNAL|flags ) ;
      if ( rc > 0 )
      {
         if ( flags & MSG_PEEK )
         {
             goto done ;
         }
         len -= rc ;
         pMsg += rc ;
      }
      else if ( rc == 0 )
      {
         PD_RC_CHECK ( EDB_NETWORK_CLOSE, PDWARNING,
                       "Peer unexpected shutdown" ) ;
      }
      else
      {
         rc = SOCKET_GETLASTERROR ;
         if ( ( OSS_EAGAIN == rc || EWOULDBLOCK == rc )  &&
              _timeout >  0 )
         {
            PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                          "Recv() timeout: rc = %d", rc ) ;
         }
         if ( ( OSS_EINTR == rc ) && (retries < MAX_RECV_RETRIES ) )
         {
            retries ++ ;
            continue ;
         }
         PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                       "Recv() Failed: rc = %d", rc ) ;
      }
   }
   rc = EDB_OK ;
done :
   return rc ;
error :
   goto done ;
}

int _ossSocket::recvNF ( char *pMsg, int len,
                         int timeout )
{
   int rc = EDB_OK ;
   int retries = 0 ;
   int maxFD = (int)_fd ;
   struct timeval maxSelectTime ;
   fd_set fds ;

   // if we don't expect to receive anything, no need to continue
   if ( 0 == len )
      return EDB_OK ;

   maxSelectTime.tv_sec = timeout / 1000000 ;
   maxSelectTime.tv_usec = timeout % 1000000 ;
   // wait loop until either we timeout or get a message
   while ( true )
   {
      FD_ZERO ( &fds ) ;
      FD_SET ( _fd, &fds ) ;
      rc = select ( maxFD + 1, &fds, NULL, NULL,
                    timeout>=0?&maxSelectTime:NULL ) ;

      // 0 means timeout
      if ( 0 == rc )
      {
         rc = EDB_TIMEOUT ;
         goto done ;
      }
      // if < 0, means something wrong
      if ( 0 > rc )
      {
         rc = SOCKET_GETLASTERROR ;
         // if we failed due to interrupt, let's continue
         if ( OSS_EINTR == rc )
         {
            continue ;
         }
         PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                       "Failed to select from socket, rc = %d", rc ) ;
      }

      // if the socket we interested is not receiving anything, let's continue
      if ( FD_ISSET ( _fd, &fds ) )
      {
         break ;
      }
   }
   // MSG_NOSIGNAL : Requests not to send SIGPIPE on errors on stream
   // oriented sockets when the other end breaks the connection. The EPIPE
   // error is still returned.
   rc = ::recv ( _fd, pMsg, len, MSG_NOSIGNAL ) ;

   if ( rc > 0 )
   {
      len = rc ;
   }
   else if ( rc == 0 )
   {
      PD_RC_CHECK ( EDB_NETWORK_CLOSE, PDWARNING,
                    "Peer unexpected shutdown" ) ;
   }
   else
   {
      // if rc < 0
      rc = SOCKET_GETLASTERROR ;
      if ( (EAGAIN == rc || EWOULDBLOCK == rc) &&
           _timeout > 0 )
      {
         // if we timeout, it's partial message and we should restart
         PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                       "Recv() timeout: rc = %d", rc ) ;
      }
      if ( ( OSS_EINTR == rc ) && ( retries < MAX_RECV_RETRIES ) )
      {
         // less than max_recv_retries number, let's retry
         retries ++ ;
      }
      // something bad when get here
      PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                    "Recv() Failed: rc = %d", rc ) ;
   }
   // Everything is fine when get here
   rc = EDB_OK ;
done :
   return rc ;
error :
   goto done ;
}

int _ossSocket::connect ()
{
   int rc = EDB_OK ;
   rc = ::connect ( _fd, (struct sockaddr *)&_sockAddress, _addressLen ) ;
   if ( rc )
   {
      PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                    "Failed to connect, rc = %d", SOCKET_GETLASTERROR ) ;
   }
   rc = getsockname ( _fd, (sockaddr*)&_sockAddress, &_addressLen ) ;
   if ( rc )
   {
      PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                    "Failed to get local address, rc = %d", rc ) ;
   }
   // get peer address
   rc = getpeername ( _fd, (sockaddr*)&_peerAddress, &_peerAddressLen ) ;
   if ( rc ) 
   {
      PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                    "Failed to get peer address, rc = %d", rc ) ;
   }
done :
   return rc ;
error :
   goto done ;
}

void _ossSocket::close ()
{
   if ( _init )
   {
      int i = 0 ;
      i = ::closesocket ( _fd ) ;
      if ( i < 0 )
      {
         i = -1 ;
      }
      _init = false ;
   }
}

int _ossSocket::accept ( SOCKET *sock, struct sockaddr *addr, socklen_t *addrlen,
                        int timeout )
{
   int rc = EDB_OK ;
   int maxFD = (int)_fd ;
   struct timeval maxSelectTime ;

   fd_set fds ;
   maxSelectTime.tv_sec = timeout / 1000000 ;
   maxSelectTime.tv_usec = timeout % 1000000 ;
   while ( true )
   {
      FD_ZERO ( &fds ) ;
      FD_SET ( _fd, &fds ) ;
      rc = select ( maxFD + 1, &fds, NULL, NULL,
                    timeout>=0?&maxSelectTime:NULL ) ;

      // 0 means timeout
      if ( 0 == rc )
      {
         *sock = 0 ;
         rc = EDB_TIMEOUT ;
         goto done ;
      }
      // if < 0, means something wrong
      if ( 0 > rc )
      {
         rc = SOCKET_GETLASTERROR ;
         // if we failed due to interrupt, let's continue
         if ( OSS_EINTR == rc )
         {
            continue ;
         }
         PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                       "Failed to select from socket, rc = %d",
                       SOCKET_GETLASTERROR);
      }

      // if the socket we interested is not receiving anything, let's continue
      if ( FD_ISSET ( _fd, &fds ) )
      {
         break ;
      }
   }
   rc = EDB_OK ;
   *sock = ::accept ( _fd, addr, addrlen );
   if ( -1 == *sock )
   {
      PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                    "Failed to accept socket, rc = %d",
                    SOCKET_GETLASTERROR ) ;
   }
done :
   return rc ;
error :
   close () ;
   goto done ;
}

int _ossSocket::disableNagle ()
{
   int rc = EDB_OK ;
   int temp = 1 ;
   rc = setsockopt ( _fd, IPPROTO_TCP, TCP_NODELAY, (char *) &temp,
                     sizeof ( int ) ) ;
   if ( rc )
   {
      PD_LOG ( PDWARNING, "Failed to setsockopt, rc = %d", SOCKET_GETLASTERROR ) ;
   }

   rc = setsockopt ( _fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &temp,
                     sizeof ( int ) ) ;
   if ( rc )
   {
      PD_LOG ( PDWARNING, "Failed to setsockopt, rc = %d", SOCKET_GETLASTERROR ) ;
   }
   return rc ;
}

unsigned int _ossSocket::_getPort ( sockaddr_in *addr )
{
   return ntohs ( addr->sin_port ) ;
}

int _ossSocket::_getAddress ( sockaddr_in *addr, char *pAddress, unsigned int length )
{
   int rc = EDB_OK ;
   length = length < NI_MAXHOST ? length : NI_MAXHOST ;
   rc = getnameinfo ( (struct sockaddr *)addr, sizeof(sockaddr), pAddress,
                      length,
                      NULL, 0, NI_NUMERICHOST ) ;
   if ( rc )
   {
      PD_RC_CHECK ( EDB_NETWORK, PDERROR,
                    "Failed to getnameinfo, rc = %d", SOCKET_GETLASTERROR ) ;
   }
done :
   return rc ;
error :
   goto done ;
}

int _ossSocket::setAnsyn () 
{
#ifdef _WINDOWS
	u_long nonblock = 1;
	return ioctlsocket(_fd, FIONBIO, &nonblock);
#else
   return fcntl(_fd, F_SETFL, O_NONBLOCK | fcntl(_fd, F_GETFL, 0));
#endif
}

unsigned int _ossSocket::getLocalPort ()
{
   return _getPort ( &_sockAddress ) ;
}

unsigned int _ossSocket::getPeerPort ()
{
   return _getPort ( &_peerAddress ) ;
}

int _ossSocket::getLocalAddress ( char * pAddress, unsigned int length )
{
   return _getAddress ( &_sockAddress, pAddress, length ) ;
}

int _ossSocket::getPeerAddress ( char * pAddress, unsigned int length )
{
   return _getAddress ( &_peerAddress, pAddress, length ) ;
}

int _ossSocket::setTimeout ( int seconds )
{
   int rc = EDB_OK ;
   struct timeval tv ;
   tv.tv_sec = seconds ;
   tv.tv_usec = 0 ;
   // windows take milliseconds as parameter
   // but linux takes timeval as input

   rc = setsockopt ( _fd, SOL_SOCKET, SO_RCVTIMEO, ( char* ) &tv,
                     sizeof ( tv ) ) ;
   if ( rc )
   {
      PD_LOG ( PDWARNING, "Failed to setsockopt, rc = %d",
               SOCKET_GETLASTERROR ) ;
   }

   rc = setsockopt ( _fd, SOL_SOCKET, SO_SNDTIMEO, ( char* ) &tv,
                     sizeof ( tv ) ) ;
   if ( rc )
   {
      PD_LOG ( PDWARNING, "Failed to setsockopt, rc = %d",
               SOCKET_GETLASTERROR ) ;
   }

   return rc ;
}

int _ossSocket::getHostName ( char *pName, int nameLen )
{
   return gethostname ( pName, nameLen ) ;
}

int _ossSocket::getPort ( const char *pServiceName, unsigned short &port )
{
   int rc = EDB_OK ;
   struct servent *servinfo ;
   servinfo = getservbyname ( pServiceName, "tcp" ) ;
   if ( !servinfo )
      port = atoi ( pServiceName ) ;
   else
      port = (unsigned short)ntohs(servinfo->s_port) ;
   return rc ;
}

