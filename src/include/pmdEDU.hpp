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
#ifndef PMDEDU_HPP__
#define PMDEDU_HPP__

#include "core.hpp"
#include "pmdEDUEvent.hpp"
#include "ossQueue.hpp"
#include "ossSocket.hpp"

#define PMD_INVALID_EDUID 0
#define PMD_IS_EDU_CREATING(x)    ( PMD_EDU_CREATING == x )
#define PMD_IS_EDU_RUNNING(x)     ( PMD_EDU_RUNNING  == x )
#define PMD_IS_EDU_WAITING(x)     ( PMD_EDU_WAITING  == x )
#define PMD_IS_EDU_IDLE(x)        ( PMD_EDU_IDLE     == x )
#define PMD_IS_EDU_DESTROY(x)     ( PMD_EDU_DESTROY  == x )

typedef unsigned long long EDUID ;

enum EDU_TYPES
{
   // System EDU Type
   EDU_TYPE_TCPLISTENER = 0,
   // Agent EDU Type
   EDU_TYPE_AGENT,

   EDU_TYPE_UNKNOWN,
   EDU_TYPE_MAXIMUM = EDU_TYPE_UNKNOWN
} ;

enum EDU_STATUS
{
   PMD_EDU_CREATING = 0,
   PMD_EDU_RUNNING,
   PMD_EDU_WAITING,
   PMD_EDU_IDLE,
   PMD_EDU_DESTROY,
   PMD_EDU_UNKNOWN,
   PMD_EDU_STATUS_MAXIMUM = PMD_EDU_UNKNOWN
} ;

class pmdEDUMgr ;

class pmdEDUCB
{
public :
   pmdEDUCB ( pmdEDUMgr *mgr, EDU_TYPES type ) ;
   inline EDUID getID()
   {
      return _id ;
   }
   inline void postEvent ( pmdEDUEvent const &data )
   {
      _queue.push ( data ) ;
   }
   bool waitEvent ( pmdEDUEvent &data, long long millsec )
   {
      // if millsec is not 0, that means we want timeout
      bool waitMsg = false ;
      if ( PMD_EDU_IDLE != _status )
      {
         _status = PMD_EDU_WAITING ;
      }
      if ( 0 > millsec )
      {
         _queue.wait_and_pop ( data ) ;
         waitMsg = true ;
      }
      else
      {
         waitMsg = _queue.timed_wait_and_pop ( data, millsec ) ;
      }

      if ( waitMsg )
      {
         if ( data._eventType == PMD_EDU_EVENT_TERM )
         {
            _isDisconnected = true ;
         }
         else
         {
            _status = PMD_EDU_RUNNING ;
         }
      }
      return waitMsg ;
   }
   inline void force ()
   {
      _isForced = true ;
   }
   inline void disconnect ()
   {
      _isDisconnected = true ;
   }
   inline EDU_TYPES getType ()
   {
      return _type ;
   }
   inline EDU_STATUS getStatus ()
   {
      return _status ;
   }
   inline void setType ( EDU_TYPES type )
   {
      _type = type ;
   }
   inline void setID ( EDUID id )
   {
      _id = id ;
   }
   inline void setStatus ( EDU_STATUS status )
   {
      _status = status ;
   }
   inline bool isForced ()
   {
      return _isForced ;
   }
   inline pmdEDUMgr *getEDUMgr ()
   {
      return _mgr ;
   }
private :
   EDU_TYPES  _type ;
   pmdEDUMgr *_mgr ;
   EDU_STATUS _status ;
   EDUID      _id ;
   bool       _isForced ;
   bool       _isDisconnected ;
   ossQueue<pmdEDUEvent> _queue ;
} ;

typedef int (*pmdEntryPoint) ( pmdEDUCB *, void * ) ;
pmdEntryPoint getEntryFuncByType ( EDU_TYPES type ) ;

int pmdAgentEntryPoint ( pmdEDUCB *cb, void *arg ) ;
int pmdTcpListenerEntryPoint ( pmdEDUCB *cb, void *arg ) ;
int pmdEDUEntryPoint ( EDU_TYPES type, pmdEDUCB *cb, void *arg ) ;

int pmdRecv ( char *pBuffer, int recvSize,
              ossSocket *sock, pmdEDUCB *cb ) ;
int pmdSend ( const char *pBuffer, int sendSize,
              ossSocket *sock, pmdEDUCB *cb ) ;

#endif
