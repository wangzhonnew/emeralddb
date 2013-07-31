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
#include "pmdOptions.hpp"
#include "pd.hpp"

pmdOptions::pmdOptions ()
{
   memset ( _dbPath, 0, sizeof(_dbPath) ) ;
   memset ( _logPath, 0, sizeof(_logPath) ) ;
   memset ( _confPath, 0, sizeof(_confPath) ) ;
   memset ( _svcName, 0, sizeof(_svcName) ) ;
   _maxPool = NUMPOOL ;
}

pmdOptions::~pmdOptions ()
{
}

int pmdOptions::readCmd ( int argc, char **argv,
                          po::options_description &desc,
                          po::variables_map &vm )
{
   int rc = EDB_OK ;
   try
   {
      po::store ( po::command_line_parser ( argc, argv ).options (
                  desc ).allow_unregistered().run(), vm ) ;
      po::notify ( vm ) ;
   }
   catch ( po::unknown_option &e )
   {
      std::cerr << "Unknown arguments: "
                << e.get_option_name() << std::endl ;
      rc = EDB_INVALIDARG ;
      goto error ;
   }
   catch ( po::invalid_option_value &e )
   {
      std::cerr << "Invalid arguments: "
                << e.get_option_name() << std::endl ;
      rc = EDB_INVALIDARG ;
      goto error ;
   }
   catch ( po::error &e )
   {
      std::cerr << "Error: " << e.what() << std::endl ;
      rc = EDB_INVALIDARG ;
      goto error ;
   }
done :
   return rc ;
error :
   goto done ;
}

int pmdOptions::importVM ( const po::variables_map &vm, bool isDefault )
{
   int rc = EDB_OK ;
   const char *p = NULL ;
   // conf path
   if ( vm.count ( PMD_OPTION_CONFPATH ) )
   {
      p = vm[PMD_OPTION_CONFPATH].as<string>().c_str() ;
      strncpy ( _confPath, p, OSS_MAX_PATHSIZE ) ;
   }
   else if ( isDefault )
   {
      strcpy ( _confPath, "./"CONFFILENAME ) ;
   }
   // log path
   if ( vm.count ( PMD_OPTION_LOGPATH ) )
   {
      p = vm[PMD_OPTION_LOGPATH].as<string>().c_str() ;
      strncpy ( _logPath, p, OSS_MAX_PATHSIZE ) ;
   }
   else if ( isDefault )
   {
      strcpy ( _logPath, "./"LOGFILENAME ) ;
   }
   // db file path
   if ( vm.count ( PMD_OPTION_DBPATH ) )
   {
      p = vm[PMD_OPTION_DBPATH].as<string>().c_str() ;
      strncpy ( _dbPath, p, OSS_MAX_PATHSIZE ) ;
   }
   else if ( isDefault )
   {
      strcpy ( _dbPath, "./"DBFILENAME ) ;
   }

   // svcname
   if ( vm.count ( PMD_OPTION_SVCNAME ) )
   {
      p = vm[PMD_OPTION_SVCNAME].as<string>().c_str() ;
      strncpy ( _svcName, p, NI_MAXSERV ) ;
   }
   else if ( isDefault )
   {
      strcpy ( _svcName, SVCNAME ) ;
   }

   // maxpool
   if ( vm.count ( PMD_OPTION_MAXPOOL ) )
   {
      _maxPool = vm [ PMD_OPTION_MAXPOOL ].as<unsigned int> () ;
   }
   else if ( isDefault )
   {
      _maxPool = NUMPOOL ;
   }
   return rc ;
}

int pmdOptions::readConfigureFile ( const char *path,
                                    po::options_description &desc,
                                    po::variables_map &vm )
{
   int rc                        = EDB_OK ;
   char conf[OSS_MAX_PATHSIZE+1] = {0} ;
   strncpy ( conf, path, OSS_MAX_PATHSIZE ) ;
   try
   {
      po::store ( po::parse_config_file<char> ( conf, desc, true ), vm ) ;
      po::notify ( vm ) ;
   }
   catch( po::reading_file )
   {
      std::cerr << "Failed to open config file: "
                <<( std::string ) conf << std::endl
                << "Using default settings" << std::endl ;
      rc = EDB_IO ;
      goto error ;
   }
   catch ( po::unknown_option &e )
   {
      std::cerr << "Unkown config element: "
                << e.get_option_name () << std::endl ;
      rc = EDB_INVALIDARG ;
      goto error ;
   }
   catch ( po::invalid_option_value &e )
   {
      std::cerr << ( std::string ) "Invalid config element: "
                << e.get_option_name () << std::endl ;
      rc = EDB_INVALIDARG ;
      goto error ;
   }
   catch( po::error &e )
   {
      std::cerr << e.what () << std::endl ;
      rc = EDB_INVALIDARG ;
      goto error ;
   }
done :
   return rc ;
error :
   goto done ;
}

int pmdOptions::init ( int argc, char **argv )
{
   int rc = EDB_OK ;
   po::options_description all ( "Command options" ) ;
   po::variables_map vm ;
   po::variables_map vm2 ;

   PMD_ADD_PARAM_OPTIONS_BEGIN( all )
      PMD_COMMANDS_OPTIONS
   PMD_ADD_PARAM_OPTIONS_END
   rc = readCmd ( argc, argv, all, vm ) ;
   if ( rc )
   {
      PD_LOG ( PDERROR, "Failed to read cmd, rc = %d", rc ) ;
      goto error ;
   }
   // check if we have help options
   if ( vm.count ( PMD_OPTION_HELP ) )
   {
      std::cout << all << std::endl ;
      rc = EDB_PMD_HELP_ONLY ;
      goto done ;
   }
   // check if there's conf path
   if ( vm.count ( PMD_OPTION_CONFPATH ) )
   {
      rc = readConfigureFile ( vm[PMD_OPTION_CONFPATH].as<string>().c_str(),
                               all, vm2 ) ;
   }
   if ( rc )
   {
      PD_LOG ( PDERROR, "Unexpected error when reading conf file, rc = %d",
               rc ) ;
      goto error ;
   }
   // load vm from file
   rc = importVM ( vm2 ) ;
   if ( rc )
   {
      PD_LOG ( PDERROR, "Failed to import from vm2, rc = %d", rc ) ;
      goto error ;
   }
   // load vm from command line
   rc = importVM ( vm ) ;
   if ( rc )
   {
      PD_LOG ( PDERROR, "Failed to import from vm, rc = %d", rc ) ;
      goto error ;
   }
done :
   return rc ;
error :
   goto done ;
}

