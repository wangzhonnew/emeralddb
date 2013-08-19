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

#include "core.hpp"

#ifdef _WINDOWS
#define oss_mutex_t						CRITICAL_SECTION
#define oss_mutex_init(__lock, __attribute)		InitializeCriticalSection( (__lock) )
#define oss_mutex_destroy				DeleteCriticalSection
#define oss_mutex_lock					EnterCriticalSection
#define oss_mutex_trylock(__lock)		(TRUE == TryEnterCriticalSection( (__lock) ) )
#define oss_mutex_unlock				LeaveCriticalSection

#define oss_rwlock_t					SRWLOCK
#define oss_rwlock_init(__lock, __attribute)		InitializeSRWLock( (__lock) )
#define oss_rwlock_destroy(__lock)		(1)	
#define oss_rwlock_rdlock				AcquireSRWLockShared
#define oss_rwlock_rdunlock				ReleaseSRWLockShared
#define oss_rwlock_wrlock				AcquireSRWLockExclusive
#define oss_rwlock_wrunlock				ReleaseSRWLockExclusive
#define oss_rwlock_rdtrylock(__lock)	(false)
#define oss_rwlock_wrtrylock(__lock)	(false)

#else

#define oss_mutex_t						pthread_mutex_t
#define oss_mutex_init					pthread_mutex_init
#define oss_mutex_destroy				pthread_mutex_destroy
#define oss_mutex_lock					pthread_mutex_lock
#define oss_mutex_trylock				pthread_mutex_trylock
#define oss_mutex_unlock				pthread_mutex_unlock

#define oss_rwlock_t					pthread_rwlock_t
#define oss_rwlock_init					pthread_rwlock_init
#define oss_rwlock_destroy				pthread_rwlock_destroy
#define oss_rwlock_rdlock				pthread_rwlock_rdlock
#define oss_rwlock_rdunlock				pthread_rwlock_unlock
#define oss_rwlock_wrlock				pthread_rwlock_wrlock
#define oss_rwlock_wrunlock				pthread_rwlock_unlock
#define oss_rwlock_rdtrylock(__lock)	(pthread_rwlock_tryrdlock( (__lock) ) == 0 )
#define oss_rwlock_wrtrylock(__lock)	(pthread_rwlock_trywrlock ( ( __lock) ) == 0 )

#endif

enum OSS_LATCH_MODE
{
   SHARED ,
   EXCLUSIVE
} ;

class ossXLatch
{
private :
   oss_mutex_t _lock ;
public :
   ossXLatch ()
   {
      oss_mutex_init ( &_lock, 0 ) ;
   }
   ~ossXLatch ()
   {
		oss_mutex_destroy(&_lock);
   }
   void get ()
   {
		oss_mutex_lock(&_lock);
   }
   void release ()
   {
		oss_mutex_unlock(&_lock);
   }
   bool try_get ()
   {
		return oss_mutex_trylock(&_lock);
   }
} ;

class ossSLatch
{
private :
   oss_rwlock_t _lock ;
public :
   ossSLatch ()
   {
      oss_rwlock_init ( &_lock, 0 ) ;
   }

   ~ossSLatch ()
   {
      oss_rwlock_destroy ( &_lock ) ;
   }

   void get ()
   {
      oss_rwlock_wrlock ( &_lock ) ;
   }

   void release ()
   {
      oss_rwlock_wrunlock ( &_lock ) ;
   }

   bool try_get ()
   {
      return ( oss_rwlock_wrtrylock ( &_lock ) ) ;
   }

   void get_shared ()
   {
      oss_rwlock_rdlock ( &_lock ) ;
   }

   void release_shared ()
   {
      oss_rwlock_rdunlock ( &_lock ) ;
   }

   bool try_get_shared ()
   {
      return ( oss_rwlock_rdtrylock ( &_lock ) ) ;
   }
} ;

#endif

