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
#ifndef DMS_HPP__
#define DMS_HPP__

#include "ossLatch.hpp"
#include "ossMmapFile.hpp"
#include "bson.h"
#include "dmsRecord.hpp"
#include <vector>

#define DMS_EXTEND_SIZE 65536
// 4MB for page size
#define DMS_PAGESIZE   4194304
#define DMS_MAX_RECORD (DMS_PAGESIZE-sizeof(dmsHeader)-sizeof(dmsRecord)-sizeof(SLOTOFF))
#define DMS_MAX_PAGES  262144
typedef unsigned int SLOTOFF ;
#define DMS_INVALID_SLOTID       0xFFFFFFFF
#define DMS_INVALID_PAGEID       0xFFFFFFFF

#define DMS_KEY_FIELDNAME        "_id"
extern const char *gKeyFieldName ;

// each record has the following header, include 4 bytes size and 4 bytes flag
#define DMS_RECORD_FLAG_NORMAL 0
#define DMS_RECORD_FLAG_DROPPED 1
struct dmsRecord
{
   unsigned int _size ;
   unsigned int _floag ;
   char         _data[0] ;
} ;

// dms header
#define DMS_HEADER_EYECATCHER "DMSH"
#define DMS_HEADER_EYECATCHER_LEN 4
#define DMS_HEADER_FLAG_NORMAL 0
#define DMS_HEADER_FLAG_DROPPED 1

#define DMS_HEADER_VERSION_0   0
#define DMS_HEADER_VERSION_CURRENT DMS_HEADER_VERSION_0

struct dmsHeader
{
   char         _eyeCatcher[DMS_HEADER_EYECATCHER_LEN] ;
   unsigned int _size ;
   unsigned int _flag ;
   unsigned int _version ;
} ;

// page structure
/*********************************************************
PAGE STRUCTURE
-------------------------
| PAGE HEADER           |
-------------------------
| Slot List             |
-------------------------
| Free Space            |
-------------------------
| Data                  |
-------------------------
**********************************************************/
#define DMS_PAGE_EYECATCHER "PAGH"
#define DMS_PAGE_EYECATCHER_LEN 4
#define DMS_PAGE_FLAG_NORMAL    0
#define DMS_PAGE_FLAG_UNALLOC   1
#define DMS_SLOT_EMPTY 0xFFFFFFFF

struct dmsPageHeader
{
   char             _eyeCatcher[DMS_PAGE_EYECATCHER_LEN] ;
   unsigned int     _size ;
   unsigned int     _flag ;
   unsigned int     _numSlots ;
   unsigned int     _slotOffset ;
   unsigned int     _freeSpace ;
   unsigned int     _freeOffset ;
   char             _data[0] ;
} ;

#define DMS_FILE_SEGMENT_SIZE 134217728
#define DMS_FILE_HEADER_SIZE  65536
#define DMS_PAGES_PER_SEGMENT (DMS_FILE_SEGMENT_SIZE/DMS_PAGESIZE)
#define DMS_MAX_SEGMENTS      (DMS_MAX_PAGES/DMS_PAGES_PER_SEGMENT)
class dmsFile : public ossMmapFile
{
private :
   // points to memory where header is located
   dmsHeader            *_header ;
   std::vector<char *>   _body ;
   // free space to page id map
   std::multimap<unsigned int, PAGEID> _freeSpaceMap ;
   ossSLatch             _mutex ;
   ossXLatch             _extendMutex ;
   char                 *_pFileName ;
public :
   dmsFile () ;
   ~dmsFile () ;
   // initialize the dms file
   int initialize ( const char *pFileName ) ;
   // insert into file
   int insert ( bson::BSONObj &record, bson::BSONObj &outRecord, dmsRecordID &rid ) ;
   int remove ( dmsRecordID &rid ) ;
   int find ( dmsRecord &rid, bson::BSONObj &result ) ;
private :
   // create a new segment for the current file
   int _extendSegment () ;
   // init from empty file, creating header only
   int _initNew () ;
   // extend the file for given bytes
   int _extendFile ( int size ) ;
   // load data from beginning
   int _loadData () ;
   // search slot
   int _searchSlot ( char *page,
                     dmsRecordID &recordID,
                     SLOTOFF &slot ) ;
   // reorg
   void _recoverSpace ( char *page ) ;
   // update free space
   void _updateFreeSpace ( dmsPageHeader *header, int changeSize,
                           PAGEID pageID ) ;
   // find a page id to insert, return invalid_pageid if there's no page canb e found for required size bytes
   PAGEID _findPage ( size_t requiredSize ) ;
public :
   inline unsigned int getNumSegments ()
   {
      return _body.size() ;
   }
   inline unsigned int getNumPages ()
   {
      return getNumSegments() * DMS_PAGES_PER_SEGMENT ;
   }
   inline char *pageToOffset ( PAGEID pageID )
   {
      if ( pageID >= getNumPages () )
      {
         return NULL ;
      }
      return _body [ pageID / DMS_PAGES_PER_SEGMENT ] + DMS_PAGESIZE * ( pageID % DMS_PAGES_PER_SEGMENT ) ;
   }

   inline bool validSize ( size_t size )
   {
      if ( size < DMS_FILE_HEADER_SIZE )
      {
         return false ;
      }
      size = size - DMS_FILE_HEADER_SIZE ;
      if ( size % DMS_FILE_SEGMENT_SIZE != 0 )
      {
         return false ;
      }
      return true ;
   }
} ;

#endif
