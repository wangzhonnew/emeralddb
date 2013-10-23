/**
 *      Copyright (C) 2012 SequoiaDB Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
package com.emeralddb.base;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import com.emeralddb.exception.BaseException;
import com.emeralddb.net.ConfigOptions;
import com.emeralddb.net.ConnectionTCPImpl;
import com.emeralddb.net.IConnection;
import com.emeralddb.net.ServerAddress;
import com.emeralddb.util.HashAlgorithm;
import com.emeralddb.util.KetamaNodeLocator;

public class EmeralddbCommon {
   public  KetamaNodeLocator _locator = null;
   private List<ServerAddress> _serverAddressList = null;
   private Map<ServerAddress, IConnection> _connectionMap = null;

   private final int VIRTUAL_NODE_COUNT = 160;
   private String configFile = "";
   public KetamaNodeLocator getLocator() {
      return _locator;
   }

   public Map<ServerAddress, IConnection> getConnectionMap() {
      return _connectionMap;
   }

   public EmeralddbCommon() {
   }

   public int init(String configFile) {
      this.configFile = configFile;
      File file = new File(configFile);
      if(!file.exists()) {
         System.out.println("config file is not exist.");
         return -1;
      }
      try {
         setServerAddressVec(file);
      } catch (UnknownHostException e) {
         // TODO Auto-generated catch block
         e.printStackTrace();
         return -1;
      }
      _connectionMap = new HashMap<ServerAddress, IConnection>();
      initConnection();
      _locator = new KetamaNodeLocator(_serverAddressList, HashAlgorithm.KETAMA_HASH, VIRTUAL_NODE_COUNT);
      return 0;
   }

   /***
    *
    * @throws BaseException
    */
   private void initConnection() {
      ConfigOptions options = new ConfigOptions();
      int size = _serverAddressList.size();
      Vector<ServerAddress> tmpVec = new Vector<ServerAddress>();
      for(int i=0; i<size; i++) {
         ServerAddress sa = _serverAddressList.get(i);
         try {
            IConnection connection= new ConnectionTCPImpl(sa, options);
            connection.initialize();
            _connectionMap.put(sa, connection);
         } catch(BaseException e) {
            System.out.println(String.format("can't not connect server %s:%d", sa.getHost(), sa.getPort()));
            tmpVec.add(sa);
            continue;
         }
      }
      for(int i=0; i<tmpVec.size(); i++) {
         _serverAddressList.remove(tmpVec.get(i));
      }
   }

   private void setServerAddressVec(File file) throws UnknownHostException{
      _serverAddressList = new LinkedList<ServerAddress>();
      ServerAddress sa = null;
      try {
         BufferedReader reader = new BufferedReader(new FileReader(file));
         String tmpString = null;
         while((tmpString=reader.readLine())!=null) {
             String[] arr = tmpString.split(":");
             sa = new ServerAddress(arr[0], Integer.parseInt(arr[1]));
             _serverAddressList.add(sa);
          }
      }catch(FileNotFoundException e) {
         e.printStackTrace();
      } catch(IOException e) {
         e.printStackTrace();
      }
   }
}
