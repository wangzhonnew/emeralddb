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
#ifndef OSSLATCH_HPP__
#define OSSLATCH_HPP__

#include <pthread.h>

enum OSS_LATCH_MODE
{
   SHARED ,
   EXCLUSIVE
} ;

class ossXLatch
{
private :
   pthread_mutex_t _lock ;
public :
   ossXLatch ()
   {
      pthread_mutex_init ( &_lock, 0 ) ;
   }
   ~ossXLatch ()
   {
      pthread_mutex_destroy ( &_lock ) ;
   }
   void get ()
   {
      pthread_mutex_lock ( &_lock ) ;
   }
   void release ()
   {
      pthread_mutex_unlock ( &_lock ) ;
   }
   bool try_get ()
   {
      return ( pthread_mutex_trylock ( &_lock ) == 0 ) ;
   }
} ;

class ossSLatch
{
private :
   pthread_rwlock_t _lock ;
public :
   ossSLatch ()
   {
      pthread_rwlock_init ( &_lock, 0 ) ;
   }

   ~ossSLatch ()
   {
      pthread_rwlock_destroy ( &_lock ) ;
   }

   void get ()
   {
      pthread_rwlock_wrlock ( &_lock ) ;
   }

   void release ()
   {
      pthread_rwlock_unlock ( &_lock ) ;
   }

   bool try_get ()
   {
      return ( pthread_rwlock_trywrlock ( &_lock ) == 0 ) ;
   }

   void get_shared ()
   {
      pthread_rwlock_rdlock ( &_lock ) ;
   }

   void release_shared ()
   {
      pthread_rwlock_unlock ( &_lock ) ;
   }

   bool try_get_shared ()
   {
      return ( pthread_rwlock_tryrdlock ( &_lock ) == 0 ) ;
   }
} ;

#endif

