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
#ifndef DMSRECORD_HPP__
#define DMSRECORD_HPP__

typedef unsigned int PAGEID ;
typedef unsigned int SLOTID ;
// each record is represented by RID, which can be broken into page id and slot id
struct dmsRecordID
{
   PAGEID _pageID ;
   SLOTID _slotID ;
} ;

#endif
