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

#ifndef OSSQUEUE_HPP__
#define OSSQUEUE_HPP__

#include <queue>
#include <boost/thread.hpp>
#include <boost/thread/thread_time.hpp>

#include "core.hpp"

template<typename Data>
class ossQueue
{
private :
   std::queue<Data> _queue ;
   boost::mutex _mutex ;
   boost::condition_variable _cond ;
public :
   unsigned int size ()
   {
      boost::mutex::scoped_lock lock ( _mutex ) ;
      return (unsigned int)_queue.size () ;
   }

   void push ( Data const &data )
   {
      boost::mutex::scoped_lock lock ( _mutex ) ;
      _queue.push ( data ) ;
      lock.unlock () ;
      _cond.notify_one () ;
   }

   bool empty () const
   {
      boost::mutex::scoped_lock lock ( _mutex ) ;
      return _queue.empty () ;
   }

   bool try_pop ( Data &value )
   {
      boost::mutex::scoped_lock lock ( _mutex ) ;
      if ( _queue.empty () )
         return false ;
      value = _queue.front () ;
      _queue.pop () ;
      return true ;
   }

   void wait_and_pop ( Data &value )
   {
      boost::mutex::scoped_lock lock ( _mutex ) ;
      while ( _queue.empty () )
      {
         _cond.wait ( lock ) ;
      }
      value = _queue.front () ;
      _queue.pop () ;
   }

   bool timed_wait_and_pop ( Data &value, long long millsec )
   {
      boost::system_time const timeout = boost::get_system_time () +
            boost::posix_time::milliseconds(millsec) ;
      boost::mutex::scoped_lock lock ( _mutex ) ;
      // if timed_wait return false, that means we failed by timeout
      while ( _queue.empty () )
      {
         if ( !_cond.timed_wait ( lock, timeout ) )
         {
            return false ;
         }
      }
      value = _queue.front () ;
      _queue.pop () ;
      return true ;
   }
} ;
#endif
