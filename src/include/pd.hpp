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

#ifndef PD_HPP__
#define PD_HPP__

#include "core.hpp"

#define PD_LOG_STRINGMAX 4096

#define PD_LOG(level,fmt,...)                                         \
   do {                                                               \
      if ( _curPDLevel >= level )                                     \
      {                                                               \
         pdLog ( level, __func__, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
      }                                                               \
   } while ( 0 )                                                      \

#define PD_CHECK(cond,retCode,gotoLabel,level,fmt,...)                \
   do {                                                               \
      if ( !(cond) )                                                  \
      {                                                               \
         rc = (retCode) ;                                             \
         PD_LOG  ( (level), fmt, ##__VA_ARGS__) ;                     \
         goto gotoLabel ;                                             \
      }                                                               \
   } while ( 0 )                                                      \

#define PD_RC_CHECK(rc,level,fmt,...)                                 \
   do {                                                               \
      PD_CHECK ( (EDB_OK==(rc)), (rc), error, (level),                \
                 fmt, ##__VA_ARGS__) ;                                \
   } while ( 0 )                                                      \

#define EDB_VALIDATE_GOTOERROR(cond,ret,str)                          \
   {if(!(cond)) { pdLog(PDERROR, __func__, __FILE__, __LINE__, str) ; \
    rc = ret; goto error ; }}

#define EDB_ASSERT(cond,str)  {if(cond){}}
#define EDB_CHECK(cond,str)   {if(cond){}}

enum PDLEVEL
{
   PDSEVERE = 0,
   PDERROR,
   PDEVENT,
   PDWARNING,
   PDINFO,
   PDDEBUG
} ;

extern PDLEVEL _curPDLevel ;
const char *getPDLevelDesp ( PDLEVEL level ) ;

#define PD_DFT_DIAGLEVEL PDWARNING
void pdLog ( PDLEVEL level, const char *func, const char *file,
             unsigned int line, const char *format, ... ) ;
void pdLog ( PDLEVEL level, const char *func, const char *file,
             unsigned int line, std::string message ) ;
#endif
