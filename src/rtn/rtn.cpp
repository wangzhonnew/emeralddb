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
#include "core.hpp"
#include "rtn.hpp"
#include "pd.hpp"
#include "pmd.hpp"

using namespace bson ;

rtn::rtn() :
_dmsFile(NULL)
{
}

rtn::~rtn()
{
   if ( _dmsFile )
   {
      delete _dmsFile ;
   }
}

int rtn::rtnInitialize ()
{
   int rc = EDB_OK ;
   _dmsFile = new(std::nothrow) dmsFile () ;
   if ( !_dmsFile )
   {
      rc = EDB_OOM ;
      PD_LOG ( PDERROR, "Failed to new dms file" ) ;
      goto error ;
   }
   // init dms
   rc = _dmsFile->initialize ( pmdGetKRCB()->getDataFilePath () ) ;
   if ( rc )
   {
      PD_LOG ( PDERROR, "Failed to call dms initialize, rc = %d", rc ) ;
      goto error ;
   }
done :
   return rc ;
error :
   goto done ;
}

int rtn::rtnInsert ( BSONObj &record )
{
   int rc = EDB_OK ;
   dmsRecordID recordID ;
   BSONObj outRecord ;
   // write data into file
   rc = _dmsFile->insert ( record, outRecord, recordID ) ;
   if ( rc )
   {
      PD_LOG ( PDERROR, "Failed to call dms insert, rc = %d", rc ) ;
      goto error ;
   }
done :
   return rc ;
error :
   goto done ;
}

int rtn::rtnFind ( BSONObj &inRecord, BSONObj &outRecord )
{
   return EDB_OK ;
}

int rtn::rtnRemove ( BSONObj &record )
{
   return EDB_OK ;
}
