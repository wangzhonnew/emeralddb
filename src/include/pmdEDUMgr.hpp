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
#ifndef PMDEDUMGR_HPP__
#define PMDEDUMGR_HPP__

#include "core.hpp"
#include "pmdEDU.hpp"
#include "ossLatch.hpp"
#include "ossUtil.hpp"

#define EDU_SYSTEM     0x01
#define EDU_USER       0x02
#define EDU_ALL        ( EDU_SYSTEM | EDU_USER )

class pmdEDUMgr
{
private :
   std::map<EDUID, pmdEDUCB*> _runQueue ;
   std::map<EDUID, pmdEDUCB*> _idleQueue ;
   std::map<unsigned int, EDUID> _tid_eduid_map ;

   ossSLatch _mutex ;
   // increamental-only EDU id
   // 64 bit is big enough for most
   EDUID _EDUID ;
   // list of system EDUs
   std::map<unsigned int, EDUID> _mapSystemEDUS ;
   // no new requests are allowed
   bool _isQuiesced ;
   bool _isDestroyed ;
public :
   pmdEDUMgr () :
   _EDUID(1),
   _isQuiesced(false),
   _isDestroyed(false)
   {
   }

   ~pmdEDUMgr ()
   {
      reset () ;
   }
   void reset ()
   {
      _destroyAll () ;
   }

   unsigned int size ()
   {
      unsigned int num = 0 ;
      _mutex.get_shared () ;
      num = ( unsigned int ) _runQueue.size() +
            ( unsigned int ) _idleQueue.size () ;
      _mutex.release_shared () ;
      return num ;
   }

   unsigned int sizeRun ()
   {
      unsigned int num = 0 ;
      _mutex.get_shared () ;
      num = ( unsigned int ) _runQueue.size () ;
      _mutex.release_shared () ;
      return num ;
   }

   unsigned int sizeIdle ()
   {
      unsigned int num = 0 ;
      _mutex.get_shared () ;
      num = ( unsigned int ) _idleQueue.size () ;
      _mutex.release_shared () ;
      return num ;
   }

   unsigned int sizeSystem ()
   {
      unsigned int num = 0 ;
      _mutex.get_shared () ;
      num = _mapSystemEDUS.size() ;
      _mutex.release_shared () ;
      return num ;
   }

   EDUID getSystemEDU ( EDU_TYPES edu )
   {
      EDUID eduID = PMD_INVALID_EDUID;
      _mutex.get_shared () ;
      std::map<unsigned int, EDUID>::iterator it = _mapSystemEDUS.find( edu ) ;
      if ( it != _mapSystemEDUS.end() )
      {
         eduID = it->second  ;
      }
      _mutex.release_shared () ;
      return eduID ;
   }

   bool isSystemEDU ( EDUID eduID )
   {
      bool isSys = false ;
      _mutex.get_shared () ;
      isSys = _isSystemEDU ( eduID ) ;
      _mutex.release_shared () ;
      return isSys ;
   }

   void regSystemEDU ( EDU_TYPES edu, EDUID eduid )
   {
      _mutex.get() ;
      _mapSystemEDUS[ edu ] = eduid ;
      _mutex.release () ;
   }

   bool isQuiesced ()
   {
      return _isQuiesced ;
   }
   void setQuiesced ( bool b )
   {
      _isQuiesced = b ;
   }
   bool isDestroyed ()
   {
      return _isDestroyed ;
   }
   static bool isPoolable ( EDU_TYPES type )
   {
      return ( EDU_TYPE_AGENT == type ) ;
   }

private :
   int _createNewEDU ( EDU_TYPES type, void *arg, EDUID *eduid ) ;
   int _destroyAll () ;
   int _forceEDUs ( int property = EDU_ALL ) ;
   unsigned int _getEDUCount ( int property = EDU_ALL ) ;
   void _setDestroyed ( bool b )
   {
      _isDestroyed = b ;
   }
   bool _isSystemEDU ( EDUID eduID )
   {
      std::map<unsigned int, EDUID>::iterator it = _mapSystemEDUS.begin() ;
      while ( it != _mapSystemEDUS.end() )
      {
         if ( eduID == it->second )
         {
            return true ;
         }
         ++it ;
      }
      return false ;
   }

   /*
    * This function must be called against a thread that either in
    * PMD_EDU_WAITING or PMD_EDU_IDLE or PMD_EDU_CREATING status
    * This function set the status to PMD_EDU_DESTROY and remove
    * the control block from manager
    * Parameter:
    *   EDU ID (UINt64)
    * Return:
    *   EDB_OK (success)
    *   EDB_SYS (the given eduid can't be found)
    *   EDB_EDU_INVAL_STATUS (EDU is found but not with expected status)
    */
   int _destroyEDU ( EDUID eduID ) ;

   /*
    * This function must be called against a thread that either in creating
    * or waiting status, it will return without any change if the agent is
    * already in pool
    * This function will change the status to PMD_EDU_IDLE and put to
    * idlequeue, representing the EDU is pooled (no longer associate with
    * any user activities)
    * deactivateEDU supposed only happened to AGENT EDUs
    * Any EDUs other than AGENT will be forwarded to destroyEDU and return
    * SDB_SYS
    * Parameter:
    *   EDU ID (UINt64)
    * Return:
    *   EDB_OK (success)
    *   EDB_SYS (the given eduid can't be found, or it's not AGENT)
    *           (note that deactivate an non-AGENT EDU will cause the EDU
    *            destroyed and SDB_SYS return)
    *   EDB_EDU_INVAL_STATUS (EDU is found but not with expected status)
    */
   int _deactivateEDU ( EDUID eduID ) ;
public :
   /*
    * EDU Status Transition Table
    * C: CREATING
    * R: RUNNING
    * W: WAITING
    * I: IDLE
    * D: DESTROY
    * c: createNewEDU
    * a: activateEDU
    * d: destroyEDU
    * w: waitEDU
    * t: deactivateEDU
    *   C   R   W   I   D  <--- from
    * C c
    * R a   -   a   a   -  <--- Creating/Idle/Waiting status can move to Running status
    * W -   w   -   -   -  <--- Running status move to Waiting
    * I t   -   t   -   -  <--- Creating/Waiting status move to Idle
    * D d   -   d   d   -  <--- Creating / Waiting / Idle can be destroyed
    * ^ To
    */
   /*
    * This function must be called against a thread that either in
    * creating/idle or waiting status
    * Threads in creating/waiting status should be sit in runqueue
    * Threads in idle status should be sit in idlequeue
    * This function set the status to PMD_END_RUNNING and bring
    * the control block to runqueue if it was idle status
    * Parameter:
    *   EDU ID (UINt64)
    * Return:
    *   SDB_OK (success)
    *   SDB_SYS (the given eduid can't be found)
    *   SDB_EDU_INVAL_STATUS (EDU is found but not with expected status)
    */
   int activateEDU ( EDUID eduID ) ;

   /*
    * This function must be called against a thread that in running
    * status
    * Threads in running status will be put in PMD_EDU_WAITING and
    * remain in runqueue
    * Parameter:
    *   EDU ID (UINt64)
    * Return:
    *   SDB_OK (success)
    *   SDB_SYS (the given eduid can't be found)
    *   SDB_EDU_INVAL_STATUS (EDU is found but not with expected status)
    */
   int waitEDU ( EDUID eduID ) ;

   /*
    * This function is called to get an EDU run the given function
    * Depends on if there's any idle EDU, manager may choose an existing
    * idle thread or creating a new threads to serve the request
    * Parmaeter:
    *   pmdEntryPoint ( void (*entryfunc) (pmdEDUCB*, void*) )
    *   type (EDU type, PMD_TYPE_AGENT for example )
    *   arg ( void*, pass to pmdEntryPoint )
    * Output:
    *   eduid ( UINt64, the edu id for the assigned EDU )
    * Return:
    *   SDB_OK (success)
    *   SDB_SYS (internal error (edu id is reused) or creating thread fail)
    *   SDB_OOM (failed to allocate memory)
    *   SDB_INVALIDARG (the type is not valid )
    */
   int startEDU ( EDU_TYPES type, void* arg, EDUID *eduid ) ;

   /*
    * This function should post a message to EDU
    * In each EDU control block there is a queue for message
    * Posting EDU means adding an element to the queue
    * If the EDU is doing some other activity at the moment, it may
    * not able to consume the event right away
    * There can be more than one event sitting in the queue
    * The order is first in first out
    * Parameter:
    *   EDU Id ( EDUID )
    *   enum pmdEDUEventTypes, in pmdEDUEvent.hpp
    *   pointer for data
    * Return:
    *   SDB_OK ( success )
    *   SDB_SYS ( given EDU ID can't be found )
    */
   int postEDUPost ( EDUID eduID, pmdEDUEventTypes type,
                     bool release = false, void *pData = NULL ) ;

   /*
    * This function should wait an event for EDU
    * If there are more than one event sitting in the queue
    * waitEDU function should pick up the earliest event
    * This function will wait forever if the input is less than 0
    * Parameter:
    *    EDU ID ( EDUID )
    *    millisecond for the period of waiting ( -1 by default )
    * Output:
    *    Reference for event
    * Return:
    *   SDB_OK ( success )
    *   SDB_SYS ( given EDU ID can't be found )
    *   SDB_TIMEOUT ( timeout )
    */
   int waitEDUPost ( EDUID eduID, pmdEDUEvent& event,
                     long long millsecond ) ;

   /*
    * This function should return an waiting/creating EDU to pool
    * (cannot be running)
    * Pool will decide whether to destroy it or pool the EDU
    * Any thread main function should detect the destroyed output
    * deactivateEDU supposed only happened to AGENT EDUs
    * Parameter:
    *   EDU ID ( EDUID )
    * Output:
    *   Pointer for whether the EDU is destroyed
    * Return:
    *   SDB_OK ( success )
    *   SDB_SYS ( given EDU ID can't be found )
    *   SDB_EDU_INVAL_STATUS (EDU is found but not with expected status)
    */
   int returnEDU ( EDUID eduID, bool force, bool* destroyed ) ;

   int forceUserEDU ( EDUID eduID ) ;

   pmdEDUCB *getEDU ( unsigned int tid ) ;
   pmdEDUCB *getEDU () ;
   pmdEDUCB *getEDUByID ( EDUID eduID ) ;
   void setEDU ( unsigned int tid, EDUID eduid ) ;

} ;

#endif
