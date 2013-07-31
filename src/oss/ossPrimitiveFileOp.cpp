/*******************************************************************************
   Copyright (C) 2013 SequoiaDB Software Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License, version 3,
   as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU Affero General Public License for more details.

   You shoudl have received a copy of the GNU Affero General Public License
   along with this program. If not, see <http://www.gnu.org/license/>.
*******************************************************************************/
#include "core.hpp"
#include "ossPrimitiveFileOp.hpp"

ossPrimitiveFileOp::ossPrimitiveFileOp()
{
   _fileHandle = OSS_INVALID_HANDLE_FD_VALUE ;
   _bIsStdout  = false ;
}

bool ossPrimitiveFileOp::isValid ()
{
   return ( OSS_INVALID_HANDLE_FD_VALUE != _fileHandle ) ;
}

void ossPrimitiveFileOp::Close()
{
   if ( isValid() && ( ! _bIsStdout ) )
   {
      oss_close( _fileHandle ) ;
      _fileHandle = OSS_INVALID_HANDLE_FD_VALUE ;
   }
}

int ossPrimitiveFileOp::Open( const char * pFilePath, unsigned int options )
{
   int rc = 0 ;
   int mode = O_RDWR ;

   if ( options & OSS_PRIMITIVE_FILE_OP_READ_ONLY )
   {
      mode = O_RDONLY ;
   }
   else if ( options & OSS_PRIMITIVE_FILE_OP_WRITE_ONLY )
   {
      mode = O_WRONLY ;
   }

   if ( options & OSS_PRIMITIVE_FILE_OP_OPEN_EXISTING )
   {
   }
   else if ( options & OSS_PRIMITIVE_FILE_OP_OPEN_ALWAYS )
   {
      mode |= O_CREAT ;
   }
   if ( options & OSS_PRIMITIVE_FILE_OP_OPEN_TRUNC )
   {
      mode |= O_TRUNC ;
   }

   do
   {
      _fileHandle = oss_open( pFilePath, mode, 0644 ) ;
   } while (( -1 == _fileHandle ) && ( EINTR == errno )) ;

   if ( _fileHandle <= OSS_INVALID_HANDLE_FD_VALUE )
   {
      rc = errno ;
      goto exit ;
   }

exit :
   return rc ;
}

void ossPrimitiveFileOp::openStdout()
{
   setFileHandle(STDOUT_FILENO) ;
   _bIsStdout = true ;
}

offsetType ossPrimitiveFileOp::getCurrentOffset () const
{
  return oss_lseek( _fileHandle, 0, SEEK_CUR ) ;
}

void ossPrimitiveFileOp::seekToEnd( void )
{
   oss_lseek( _fileHandle, 0, SEEK_END ) ;
}

void ossPrimitiveFileOp::seekToOffset( offsetType offset )
{
   if ( ( oss_off_t )-1 != offset )
   {
      oss_lseek( _fileHandle, offset, SEEK_SET ) ;
   }
}

int ossPrimitiveFileOp::Read ( const size_t size,
                               void * const pBuffer,
                               int * const  pBytesRead )
{
   int     retval    = 0 ;
   ssize_t bytesRead = 0 ;
   if ( isValid() )
   {
      do
      {
         bytesRead = oss_read( _fileHandle, pBuffer, size ) ;
      } while (( -1 == bytesRead ) && ( EINTR == errno )) ;
      if ( -1 == bytesRead )
      {
         goto err_read ;
      }
   }
   else
   {
      goto err_read ;
   }

   if ( pBytesRead )
   {
      *pBytesRead = bytesRead ;
   }
exit :
   return retval ;

err_read :
   *pBytesRead = 0 ;
   retval      = errno ;
   goto exit ;
}

int ossPrimitiveFileOp::Write( const void * pBuffer, size_t size )
{
   int rc = 0 ;
   size_t currentSize = 0 ;
   if ( 0 == size )
   {
      size = strlen( ( char * )pBuffer ) ;
   }

   if ( isValid() )
   {
      do
      {
         rc = oss_write( _fileHandle, &((char*)pBuffer)[currentSize],
                         size-currentSize ) ;
         if ( rc >= 0 )
            currentSize += rc ;
      } while ((( -1 == rc ) && ( EINTR == errno )) ||
               (( -1 != rc ) && ( currentSize != size ))) ;
      if ( -1 == rc )
      {
         rc = errno ;
         goto exit ;
      }
      rc = 0 ;
   }
exit :
   return rc ;
}

int ossPrimitiveFileOp::fWrite( const char * format, ... )
{
   int rc = 0 ;
   va_list ap ;
   char buf[OSS_PRIMITIVE_FILE_OP_FWRITE_BUF_SIZE] = { 0 } ;

   va_start( ap, format ) ;
   vsnprintf( buf, sizeof( buf ), format, ap ) ;
   va_end( ap ) ;

   rc = Write( buf ) ;

   return rc ;
}

void ossPrimitiveFileOp::setFileHandle( handleType handle )
{
   _fileHandle = handle ;
}

int ossPrimitiveFileOp::getSize( offsetType * const pFileSize )
{
   int             rc        = 0 ;
   oss_struct_stat buf       = { 0 } ;

   if ( -1 == oss_fstat( _fileHandle, &buf ) )
   {
      rc = errno ;
      goto err_exit ;
   }

   *pFileSize = buf.st_size ;

exit :
   return rc ;

err_exit :
   *pFileSize = 0 ;
   goto exit ;
}

