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
#ifndef RTN_HPP__
#define RTN_HPP__

#include "bson.h"
#include "dms.hpp"
// define the storage file name
#define RTN_FILE_NAME "data.1"

class rtn
{
private :
   dmsFile           *_dmsFile ;
public :
   rtn () ;
   ~rtn() ;
   int rtnInitialize () ;
   int rtnInsert ( bson::BSONObj &record ) ;
   int rtnFind ( bson::BSONObj &inRecord, bson::BSONObj &outRecord ) ;
   int rtnRemove ( bson::BSONObj &record ) ;
} ;

#endif
