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
#ifndef OSSMMAPFILE_HPP_
#define OSSMMAPFILE_HPP_

#include "core.hpp"
#include "ossLatch.hpp"
#include "ossPrimitiveFileOp.hpp"

class _ossMmapFile
{
protected :
   class _ossMmapSegment
   {
   public :
      void *_ptr ;
      unsigned int       _length ;
      unsigned long long _offset ;
      _ossMmapSegment ( void *ptr,
                        unsigned int length,
                        unsigned long long offset )
      {
         _ptr = ptr ;
         _length = length ;
         _offset = offset ;
      }
   } ;
   typedef _ossMmapSegment ossMmapSegment ;

   ossPrimitiveFileOp _fileOp ;
   ossXLatch _mutex ;
   bool _opened ;
   std::vector<ossMmapSegment> _segments ;
   char _fileName [ OSS_MAX_PATHSIZE ] ;
public :
   typedef std::vector<ossMmapSegment>::const_iterator CONST_ITR ;

   inline CONST_ITR begin ()
   {
      return _segments.begin () ;
   }

   inline CONST_ITR end ()
   {
      return _segments.end() ;
   }

   inline unsigned int segmentSize ()
   {
      return _segments.size() ;
   }
public :
   _ossMmapFile ()
   {
      _opened = false ;
      memset ( _fileName, 0, sizeof(_fileName) ) ;
   }
   ~_ossMmapFile ()
   {
      close () ;
   }

   int open ( const char *pFilename, unsigned int options ) ;
   void close () ;
   int map ( unsigned long long offset, unsigned int length, void **pAddress ) ;
} ;
typedef class _ossMmapFile ossMmapFile ;

#endif
