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
#include "ossMmapFile.hpp"
#include "pd.hpp"

using namespace std ;
int _ossMmapFile::open ( const char * pFilename,
                         unsigned int options )
{
   int rc = EDB_OK ;
   _mutex.get() ;
   rc = _fileOp.Open ( pFilename, options ) ;
   if ( EDB_OK == rc )
      _opened = true ;
   else
   {
      PD_LOG ( PDERROR, "Failed to open file, rc = %d",
               rc ) ;
      goto error ;
   }
   strncpy ( _fileName, pFilename, sizeof(_fileName) ) ;
done :
   _mutex.release () ;
   return rc ;
error :
   goto done ;
}

void _ossMmapFile::close ()
{
   _mutex.get() ;
   for ( vector<ossMmapSegment>::iterator i = _segments.begin() ;
         i != _segments.end(); i++ )
   {
      munmap ((void*)(*i)._ptr, (*i)._length ) ;
   }
   _segments.clear () ;
   if ( _opened )
   {
      _fileOp.Close () ;
      _opened = false ;
   }
   _mutex.release () ;
}

int _ossMmapFile::map ( unsigned long long offset,
                        unsigned int length,
                        void **pAddress )
{
   _mutex.get() ;
   int rc = EDB_OK ;
   ossMmapSegment seg ( 0, 0, 0 ) ;
   unsigned long long fileSize = 0 ;
   void *segment = NULL ;
   if ( 0 == length )
      goto done ;
   rc = _fileOp.getSize ((off_t*)&fileSize ) ;
   if ( rc )
   {
      PD_LOG ( PDERROR,
               "Failed to get file size, rc = %d", rc ) ;
      goto error ;
   }
   if ( offset + length > fileSize )
   {
      PD_LOG ( PDERROR,
               "Offset is greater than file size" ) ;
      rc = EDB_INVALIDARG ;
      goto error ;
   }
   // map region into memory
   segment = mmap ( NULL, length, PROT_READ | PROT_WRITE,
                    MAP_SHARED, _fileOp.getHandle () , offset ) ;
   if ( MAP_FAILED == segment )
   {
      PD_LOG ( PDERROR,
               "Failed to map offset %ld length %d, errno = %d",
               offset, length, errno ) ;
      if ( ENOMEM == errno )
         rc = EDB_OOM ;
      else if ( EACCES == errno )
         rc = EDB_PERM ;
      else
         rc = EDB_SYS ;
      goto error ;
   }
   seg._ptr = segment ;
   seg._length = length ;
   seg._offset = offset ;
   _segments.push_back ( seg ) ;
   if ( pAddress )
      *pAddress = segment ;
done :
   _mutex.release () ;
   return rc ;
error :
   goto done ;
}
