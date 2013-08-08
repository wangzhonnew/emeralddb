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
#include "pmdEDU.hpp"
#include "pmd.hpp"
#include "pmdEDUMgr.hpp"
#include "pd.hpp"


#define PMD_TCPLISTENER_RETRY 5
#define OSS_MAX_SERVICENAME NI_MAXSERV

int pmdTcpListenerEntryPoint ( pmdEDUCB *cb, void *arg )
{
   int          rc        = EDB_OK ;
   pmdEDUMgr *  eduMgr    = cb->getEDUMgr() ;
   EDUID        myEDUID   = cb->getID() ;
   unsigned int retry     = 0 ;
   EDUID        agentEDU  = PMD_INVALID_EDUID ;
   char         svcName[OSS_MAX_SERVICENAME+1] ;
   while ( retry <= PMD_TCPLISTENER_RETRY && !EDB_IS_DB_DOWN )
   {
      retry ++ ;

      strcpy( svcName, pmdGetKRCB()->getSvcName() ) ;
      PD_LOG ( PDEVENT, "Listening on port_test %s\n", svcName ) ;

      int port = 0 ;
      int len = strlen ( svcName ) ;
      for ( int i = 0; i<len; ++i )
      {
         if ( svcName[i] >= '0' && svcName[i] <= '9' )
         {
            port = port * 10 ;
            port += int( svcName[i] - '0' ) ;
         }
         else
         {
            PD_LOG ( PDERROR, "service name error!\n" ) ;
         }
      }

      ossSocket sock ( port ) ;
      rc = sock.initSocket () ;
      EDB_VALIDATE_GOTOERROR ( EDB_OK==rc, rc, "Failed initialize socket" )

      rc = sock.bind_listen () ;
      EDB_VALIDATE_GOTOERROR ( EDB_OK==rc, rc,
                               "Failed to bind/listen socket");
      // once bind is successful, let's set the state of EDU to RUNNING
      if ( EDB_OK != ( rc = eduMgr->activateEDU ( myEDUID )) )
      {
         goto error ;
      }
      // master loop for tcp listener
      while ( !EDB_IS_DB_DOWN )
      {
         int s ;
         rc = sock.accept ( (SOCKET*)&s, NULL, NULL ) ;
         // if we don't get anything for a period of time, let's loop
         if ( EDB_TIMEOUT == rc )
         {
            rc = EDB_OK ;
            continue ;
         }
         // if we receive error due to database down, we finish
         if ( rc && EDB_IS_DB_DOWN )
         {
            rc = EDB_OK ;
            goto done ;
         }
         else if ( rc )
         {
            // if we fail due to error, let's restart socket
            PD_LOG ( PDERROR, "Failed to accept socket in TcpListener" ) ;
            PD_LOG ( PDEVENT, "Restarting socket to listen" ) ;
            break ;
         }

         // assign the socket to the arg
         void *pData = NULL ;
         *((int *) &pData) = s ;

         rc = eduMgr->startEDU ( EDU_TYPE_AGENT, pData, &agentEDU ) ;
         if ( rc )
         {
            if ( rc == EDB_QUIESCED )
            {
               // we cannot start EDU due to quiesced
               PD_LOG ( PDWARNING, "Reject new connection due to quiesced database" ) ;
            }
            else
            {
               PD_LOG ( PDERROR, "Failed to start EDU agent" ) ;
            }
            // close remote connection if we can't create new thread
            ossSocket newsock ( (SOCKET*)&s ) ;
            newsock.close () ;
            continue ;
         }
      }
      if ( EDB_OK != ( rc = eduMgr->waitEDU ( myEDUID )) )
      {
         goto error ;
      }
   } // while ( retry <= PMD_TCPLISTENER_RETRY )
done :
   return rc;
error :
   switch ( rc )
   {
   case EDB_SYS :
      PD_LOG ( PDSEVERE, "System error occured" ) ;
      break ;
   default :
      PD_LOG ( PDSEVERE, "Internal error" ) ;
   }
   goto done ;
}
