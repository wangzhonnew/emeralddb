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
#include "core.hpp"
#include "ossSocket.hpp"

#define PMD_TCPLISTENER_RETRY 5
#define OSS_MAX_SERVICENAME NI_MAXSERV

int pmdTcpListenerEntryPoint ()
{
   int rc = EDB_OK ;
   int port = 48127 ;
   ossSocket sock ( port ) ;
   rc = sock.initSocket () ;
   if ( rc )
   {
      printf ( "Failed to initailize socket, rc = %d", rc ) ;
      goto error ;
   }
   rc = sock.bind_listen () ;
   if ( rc )
   {
      printf ( "Failed to bind/listen socket, rc = %d", rc ) ;
      goto error ;
   }
   // master loop for tcp listener
   while ( true )
   {
      int s ;
      rc = sock.accept ( &s, NULL, NULL ) ;
      // if we don't get anything from period of time, let's loop
      if ( EDB_TIMEOUT == rc )
      {
         rc = EDB_OK ;
         continue ;
      }
      char buffer [1024] ;
      int size ;
      ossSocket sock1 ( &s ) ;
      sock1.disableNagle () ;
      do
      {
         rc = sock1.recv ( ( char*)&size, 4 ) ;
         if ( rc && rc != EDB_TIMEOUT )
         {
            printf ( "Failed to receive size, rc = %d", rc ) ;
            goto error ;
         }
      }
      while ( EDB_TIMEOUT == rc ) ;
      do
      {
         rc = sock1.recv ( &buffer[0], size-sizeof(int) ) ;
         if ( rc && rc != EDB_TIMEOUT )
         {
            printf ( "Failed to receive buffer, rc = %d", rc ) ;
            goto error ;
         }
      }
      while ( EDB_TIMEOUT == rc ) ;
      printf ( "%s\n", buffer ) ;
      sock1.close () ;
   }
done :
   return rc ;
error :
   switch ( rc )
   {
   case EDB_SYS :
      printf ( "SYstem error occured" ) ;
      break ;
   default :
      printf ( "Internal error" ) ;
   }
   goto done ;
}
