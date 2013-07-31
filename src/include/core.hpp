/******************************************************************************
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
#ifndef CORE_HPP__
#define CORE_HPP__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <syscall.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/mman.h>

#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>

#define OSS_MAX_PATHSIZE  PATH_MAX
#define OSS_FILE_SEP_STR  "/"
#define OSS_FILE_SEP_CHAR *((const char*)OSS_FILE_SEP_STR)[0]
#define OSS_NEWLINE       "\n"
// error code list
#define EDB_OK                                           0
#define EDB_IO                                          -1
#define EDB_INVALIDARG                                  -2
#define EDB_PERM                                        -3
#define EDB_OOM                                         -4
#define EDB_SYS                                         -5
#define EDB_PMD_HELP_ONLY                               -6
#define EDB_PMD_FORCE_SYSTEM_EDU                        -7
#define EDB_TIMEOUT                                     -8
#define EDB_QUIESCED                                    -9
#define EDB_EDU_INVAL_STATUS                            -10
#define EDB_NETWORK                                     -11
#define EDB_NETWORK_CLOSE                               -12
#define EDB_APP_FORCED                                  -13
#define EDB_IXM_ID_EXIST                                -14
#define EDB_HEADER_INVALID                              -15
#define EDB_IXM_ID_NOT_EXIST                            -16
#define EDB_NO_ID                                       -17

#endif
