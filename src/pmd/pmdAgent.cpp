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
#include "pd.hpp"
#include "pmdEDUMgr.hpp"
#include "pmdEDU.hpp"
#include "ossSocket.hpp"
#include "../bson/src/bson.h"
#include "pmd.hpp"
using namespace bson;
using namespace std;

#define ossRoundUpToMultipleX(x,y) (((x)+((y)-1))-(((x)+((y)-1))%(y)))
#define PMD_AGENT_RECIEVE_BUFFER_SZ 4096
#define EDB_PAGE_SIZE               4096

static int pmdProcessAgentRequest ( char *pReceiveBuffer,
                                    int packetSize,
                                    char **ppResultBuffer,
                                    int *pResultBufferSize,
                                    bool *disconnect,
                                    pmdEDUCB *cb )
{
   EDB_ASSERT ( disconnect, "disconnect can't be NULL" )
   EDB_ASSERT ( pReceiveBuffer, "pReceivedBuffer is NULL" )
   PD_LOG ( PDEVENT, "Process Agent Request" ) ;
   int rc                           = EDB_OK ;
   unsigned int probe               = 0 ;
   **(int**)(ppResultBuffer)        = 4 ;
   *pResultBufferSize = 4 ;
   return rc ;
}

struct MsgReply
{
   int a ;
} ;

int pmdAgentEntryPoint ( pmdEDUCB *cb, void *arg )
{
   int rc                = EDB_OK ;
   unsigned int probe    = 0 ;
   bool disconnect       = false ;
   char *pReceiveBuffer  = NULL ;
   char *pResultBuffer   = NULL ;
   int receiveBufferSize = ossRoundUpToMultipleX (
                           PMD_AGENT_RECIEVE_BUFFER_SZ,
                           EDB_PAGE_SIZE ) ;

   int resultBufferSize  = sizeof( MsgReply ) ;
   int packetLength      = 0 ;
   EDUID myEDUID         = cb->getID () ;
   pmdEDUMgr *eduMgr     = cb->getEDUMgr() ;

   // receive socket from argument
   int s                 = *(( int *) &arg ) ;
   ossSocket sock ( (SOCKET*)&s ) ;
   sock.disableNagle () ;

   // allocate memory for receive buffer
   pReceiveBuffer = (char*)malloc( sizeof(char) *
                                   receiveBufferSize ) ;
   if ( !pReceiveBuffer )
   {
      rc = EDB_OOM ;
      probe = 10 ;
      goto error ;
   }

   // allocate memory for result memory
   pResultBuffer = (char*)malloc( sizeof(char) *
                                  resultBufferSize ) ;
   if ( !pResultBuffer )
   {
      rc = EDB_OOM ;
      probe = 20 ;
      goto error ;
   }

   while ( !disconnect )
   {
      // receive next packet
      rc = pmdRecv ( pReceiveBuffer, sizeof (int), &sock, cb ) ;
      if ( rc )
      {
        if ( EDB_APP_FORCED == rc )
        {
           disconnect = true ;
           continue ;
        }
        probe = 30 ;
        goto error ;
      }
      packetLength = *(int*)(pReceiveBuffer) ;
      PD_LOG ( PDDEBUG,
              "Received packet size = %d", packetLength ) ;
      if ( packetLength < (int)sizeof (int) )
      {
         probe = 40 ;
         rc = EDB_INVALIDARG ;
         goto error ;
      }
      // check if current receive buffer is large enough for the package
      if ( receiveBufferSize < packetLength+1 )
      {
         PD_LOG ( PDDEBUG,
                 "Receive buffer size is too small: %d vs %d, increasing...",
                  receiveBufferSize, packetLength ) ;
         int newSize = ossRoundUpToMultipleX ( packetLength+1, EDB_PAGE_SIZE ) ;
         if ( newSize < 0 )
         {
            probe = 50 ;
            rc = EDB_INVALIDARG ;
            goto error ;
         }
         free ( pReceiveBuffer ) ;
         pReceiveBuffer = (char*)malloc ( sizeof(char) * (newSize) ) ;
         if ( !pReceiveBuffer )
         {
            rc = EDB_OOM ;
            probe = 60 ;
            goto error ;
         }
         *(int*)(pReceiveBuffer) = packetLength ;
         receiveBufferSize = newSize ;
      }
      // receive body
      rc = pmdRecv ( &pReceiveBuffer[sizeof(int)],
                     packetLength-sizeof(int),
                     &sock, cb ) ;
      if ( rc )
      {
         if ( EDB_APP_FORCED == rc )
         {
            disconnect = true ;
            continue ;
         }
         probe = 70 ;
         goto error ;
      }
      // put 0 at end of the packet
      pReceiveBuffer[packetLength] = 0 ;
      if ( EDB_OK != ( rc = eduMgr->activateEDU ( myEDUID )) )
      {
         goto error ;
      }
      if( resultBufferSize >(int)sizeof( MsgReply ) )
      {
          resultBufferSize =  (int)sizeof( MsgReply ) ;
          free ( pResultBuffer ) ;
          pResultBuffer = (char*)malloc( sizeof(char) *
                                         resultBufferSize ) ;
          if ( !pResultBuffer )
          {
            rc = EDB_OOM ;
            probe = 20 ;
            goto error ;
          }
      }
      rc = pmdProcessAgentRequest ( pReceiveBuffer,
                                    packetLength,
                                    &pResultBuffer,
                                    &resultBufferSize,
                                    &disconnect,
                                    cb ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Error processing Agent request, rc=%d", rc ) ;
      }
      // send reply if it's not disconnected message
      if ( !disconnect )
      {
         rc = pmdSend ( pResultBuffer, *(int*)pResultBuffer, &sock, cb ) ;
         if ( rc )
         {
            if ( EDB_APP_FORCED == rc )
            {
            }
            probe = 80 ;
            goto error ;
         }
      }
      if ( EDB_OK != ( rc = eduMgr->waitEDU ( myEDUID )) )
      {
         goto error ;
      }
   }
done :
   if ( pReceiveBuffer )
      free ( pReceiveBuffer )  ;
   if ( pResultBuffer )
      free ( pResultBuffer )  ;
   sock.close () ;
   return rc;
error :
   switch ( rc )
   {
   case EDB_SYS :
      PD_LOG ( PDSEVERE,
              "EDU id %d cannot be found, probe %d", myEDUID, probe ) ;
      break ;
   case EDB_EDU_INVAL_STATUS :
      PD_LOG ( PDSEVERE,
              "EDU status is not valid, probe %d", probe ) ;
      break ;
   case EDB_INVALIDARG :
      PD_LOG ( PDSEVERE,
              "Invalid argument receieved by agent, probe %d", probe ) ;
      break ;
   case EDB_OOM :
      PD_LOG ( PDSEVERE,
              "Failed to allocate memory by agent, probe %d", probe ) ;
      break ;
   case EDB_NETWORK :
      PD_LOG ( PDSEVERE,
              "Network error occured, probe %d", probe ) ;
      break ;
   case EDB_NETWORK_CLOSE :
      PD_LOG ( PDDEBUG,
              "Remote connection closed" ) ;
      rc = EDB_OK ;
      break ;
   default :
      PD_LOG ( PDSEVERE,
              "Internal error, probe %d", probe ) ;
   }
   goto done ;
}
