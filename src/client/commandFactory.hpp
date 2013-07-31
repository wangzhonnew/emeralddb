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
#ifndef _COMMAND_FACTORY_HPP_
#define _COMMAND_FACTORY_HPP_

#include "command.hpp"
#define COMMAND_BEGIN void CommandFactory::addCommand() {
#define COMMAND_END }
#define COMMAND_ADD(cmdName,cmdClass) {                         \
   ICommand* pObj = new cmdClass();                             \
   std::string str = cmdName;                                   \
   _cmdMap.insert(COMMAND_MAP::value_type(str,pObj));           \
   }                                                            \

class CommandFactory {
   typedef std::map<std::string, ICommand*> COMMAND_MAP;
   public:
      CommandFactory();
      ~CommandFactory(){}
      void addCommand();
      ICommand * getCommandProcesser(const char * pcmd);
   private:
      COMMAND_MAP _cmdMap;
};

#endif
