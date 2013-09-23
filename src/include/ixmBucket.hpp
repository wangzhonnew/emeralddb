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
#ifndef IXM_HPP__
#define IXM_HPP__
#include "ossLatch.hpp"
#include "bson.h"
#include <map>
#include "dmsRecord.hpp"
using namespace bson ;

#define IXM_KEY_FIELDNAME "_id"
#define IXM_HASH_MAP_SIZE 1000

struct ixmEleHash
{
   const char  *data ;
   dmsRecordID recordID ;
} ;


class ixmBucketManager
{
private:
   class ixmBucket
   {
   private:
      // the map is hashNum and eleHash
      std::multimap<unsigned int, ixmEleHash> _bucketMap;
      ossSLatch _mutex ;
   public:
      // get the record whether exist
      int isIDExist ( unsigned int hashNum, ixmEleHash &eleHash ) ;
      int createIndex ( unsigned int hashNum, ixmEleHash &eleHash ) ;
      int findIndex ( unsigned int hashNum, ixmEleHash &eleHash ) ;
      int removeIndex ( unsigned int hashNum, ixmEleHash &eleHash ) ;
   } ;
   // process data
   int _processData ( BSONObj &record, dmsRecordID &recordID, // in
                      unsigned int &hashNum,                  // out
                      ixmEleHash &eleHash,                    // out
                      unsigned int &random ) ;
private :
   std::vector<ixmBucket *>_bucket ;
public :
   ixmBucketManager ()
   {
   }
   ~ixmBucketManager ()
   {
      ixmBucket *pIxmBucket = NULL ;
      for ( int i = 0; i < IXM_HASH_MAP_SIZE; ++i )
      {
         pIxmBucket = _bucket[i] ;
         if ( pIxmBucket )
            delete pIxmBucket ;
      }
   }
   int initialize () ;
   int isIDExist ( BSONObj &record ) ;
   int createIndex ( BSONObj &record, dmsRecordID &recordID ) ;
   int findIndex ( BSONObj &record, dmsRecordID &recordID ) ;
   int removeIndex ( BSONObj &record, dmsRecordID &recordID ) ;
} ;
#endif
