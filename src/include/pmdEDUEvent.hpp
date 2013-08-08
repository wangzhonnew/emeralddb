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
#ifndef PMDEDUEVENT_HPP__
#define PMDEDUEVENT_HPP__

#include "core.hpp"
enum pmdEDUEventTypes
{
   PMD_EDU_EVENT_NONE = 0,
   PMD_EDU_EVENT_TERM,     // terminate EDU
   PMD_EDU_EVENT_RESUME,   // resume a EDU, the data is startEDU's argv
   PMD_EDU_EVENT_ACTIVE,
   PMD_EDU_EVENT_DEACTIVE,
   PMD_EDU_EVENT_MSG,
   PMD_EDU_EVENT_TIMEOUT,  // timeout
   PMD_EDU_EVENT_LOCKWAKEUP // transaction lock wake up
} ;

class pmdEDUEvent
{
public :
   pmdEDUEvent () :
   _eventType(PMD_EDU_EVENT_NONE),
   _release(false),
   _Data(NULL)
   {
   }

   pmdEDUEvent ( pmdEDUEventTypes type ) :
   _eventType(type),
   _release(false),
   _Data(NULL)
   {
   }

   pmdEDUEvent ( pmdEDUEventTypes type, bool release, void *data ) :
   _eventType(type),
   _release(release),
   _Data(data)
   {
   }

   void reset ()
   {
      _eventType = PMD_EDU_EVENT_NONE ;
      _release = false ;
      _Data = NULL ;
   }

   pmdEDUEventTypes _eventType ;
   bool             _release ;
   void            *_Data ;
} ;

#endif
