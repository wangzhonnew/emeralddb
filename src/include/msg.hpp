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

#ifndef MSG_HPP__
#define MSG_HPP__

#include "bson.h"

#define OP_REPLY                   1
#define OP_INSERT                  2
#define OP_DELETE                  3
#define OP_QUERY                   4 
#define OP_COMMAND                 5
#define OP_DISCONNECT              6
#define OP_CONNECT                 7
#define OP_SNAPSHOT                8

#define RETURN_CODE_STATE_OK       1

struct MsgHeader
{
   int messageLen ;
   int opCode ;
} ;

struct MsgReply
{
   MsgHeader header ;
   int       returnCode ;
   int       numReturn ;
   char      data[0] ;
} ;

struct MsgInsert
{
   MsgHeader header ;
   int       numInsert ;
   char      data[0] ;
} ;

struct MsgDelete
{
   MsgHeader header ;
   char      key[0] ;
} ;

struct MsgQuery
{
   MsgHeader header ;
   char      key[0] ;
} ;

struct MsgCommand
{
   MsgHeader header ;
   int       numArgs ;
   char      data[0] ;
} ;

int msgBuildReply ( char **ppBuffer, int *pBufferSize,
                    int returnCode, bson::BSONObj *objReturn ) ;

int msgExtractReply ( char *pBuffer, int &returnCode, int &numReturn,
                      const char **ppObjStart ) ;

int msgBuildInsert ( char **ppBuffer, int *pBufferSize, bson::BSONObj &obj ) ;

int msgBuildInsert ( char **ppBuffer, int *pBufferSize, vector<bson::BSONObj*> &obj ) ;

int msgExtractInsert ( char *pBuffer, int &numInsert, const char **ppObjStart ) ;

int msgBuildDelete ( char **ppBuffer, int *pBufferSize, bson::BSONObj &key ) ;

int msgExtractDelete  ( char *pBuffer, bson::BSONObj &key ) ;

int msgBuildQuery ( char **ppBuffer, int *pBufferSize, bson::BSONObj &key ) ;

int msgExtractQuery ( char *pBuffer, bson::BSONObj &key ) ;

int msgBuildCommand ( char **ppBuffer, int *pBufferSize, bson::BSONObj &obj ) ;

int msgBuildCommand ( char **ppBuffer, int *pBufferSize, vector<bson::BSONObj*>&obj ) ;

int msgExtractCommand ( char *pBuffer, int &numArgs, const char **ppObjStart ) ;

#endif
