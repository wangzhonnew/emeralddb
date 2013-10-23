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
/**
 * @package com.sequoiadb.base;
 * @brief Emeralddb Driver for Java
 * @author zhengle zhou
 */
package com.emeralddb.base;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;

import org.bson.BSONObject;
import org.bson.util.JSON;

import com.emeralddb.exception.BaseException;
import com.emeralddb.net.IConnection;
import com.emeralddb.net.ServerAddress;
import com.emeralddb.util.EDBMessageHelper;
import com.emeralddb.base.EmeralddbConstants.Operation;
import com.emeralddb.base.EDBMessage;

/**
 * @class Emeralddb
 * @brief Database operation interfaces of user
 */
public class Emeralddb {

   private EmeralddbCommon _edbCommon;
   private Map<ServerAddress, Integer> _nodeRecordMap = null;
   private long  insertTotalTime = 0;
   /**
    * @fn Emeralddb(String addr, int port)
    * @brief Constructor
    * @param addr
    *            IP address
    * @param port
    *            Port
    * @exception com.emeralddb.exception.BaseException
    */
   public Emeralddb() throws BaseException {
      _edbCommon = new EmeralddbCommon();
   }
   public void endStat(int total) {
      for(Map.Entry<ServerAddress, Integer> entry : _nodeRecordMap.entrySet()) {
         System.out.println("Node name:" + entry.getKey().getPort()
               + "-Times:" + entry.getValue()
               + " - Percent: " + (float)entry.getValue()/total*100 + "%");
      }
/*
      long totalTime = 1000*1000*1000;
      System.out.println(" sendtime is "
            + (double)insertTotalTime/totalTime );
      _nodeRecordMap.clear();
*/
   }

   public void startStat() {
      insertTotalTime = 0;
      _nodeRecordMap = new HashMap<ServerAddress,Integer>();
   }

   public int init(String configFilePath) {
      return _edbCommon.init( configFilePath);
   }

   /**
    * @fn void disconnect()
    * @brief Disconnect the remote server
    * @exception com.emeralddb.exception.BaseException
    * @author zhouzhengle
    */
   public void disconnect() throws BaseException {
      EDBMessage edbMessage = new EDBMessage();
      byte[] request = EDBMessageHelper.buildDisconnectRequest(edbMessage);
      Map<ServerAddress, IConnection> connectionMap = _edbCommon.getConnectionMap();
      Iterator<Entry<ServerAddress, IConnection>> it = connectionMap.entrySet().iterator();
      while(it.hasNext()) {
         Entry<ServerAddress, IConnection> entry = it.next();
         IConnection connection = entry.getValue();
         ServerAddress servAddress = entry.getKey();
         connection.sendMessage(request);
         connection.close();
      }
   }

   /**
    * @funtion query data
    * @param key
    * @return
    * @throws BaseException
    */
   public EDBMessage query(String key)
      throws BaseException {
      IConnection connection = getConnection(key);

      if( null == connection ) {
         System.out.println("connection is failed");
         return null;
      }

      EDBMessage edbMessage = new EDBMessage();
      edbMessage.setOperationCode(Operation.OP_QUERY);

      BSONObject bson = (BSONObject)JSON.parse(key);
      edbMessage.setQuery(bson);
      byte[] request = EDBMessageHelper.buildQueryRequest(edbMessage);
      // send message
      connection.sendMessage(request);

      byte[] recvBuffer = connection.receiveMessage();
      EDBMessage rtnSDBMessage = EDBMessageHelper.msgExtractReply(recvBuffer);


      return rtnSDBMessage;
   }

   /***
    *
    * @param record
    * @return
    * @throws BaseException
    * @throws
    * @author zhouzhengle
    */
   public EDBMessage insert(String key,String record)
         throws BaseException {

      IConnection connection = getConnection(key);
      if( null == connection ) {
         System.out.println("connection is failed");
         return null;
      }
      //long start = System.currentTimeMillis();

      EDBMessage edbMessage = new EDBMessage();
      edbMessage.setOperationCode(Operation.OP_INSERT);
      edbMessage.setMessageText(record);

      BSONObject bson = (BSONObject)JSON.parse(record);
      edbMessage.setInsertor(bson);

      byte[] request = EDBMessageHelper.buildInsertRequest(edbMessage);
      connection.sendMessage(request);
      //insertTotalTime += connection.sendMessage(request);

      byte[] recvBuffer = connection.receiveMessage();

      EDBMessage rtnSDBMessage = EDBMessageHelper.msgExtractReply(recvBuffer);

      return rtnSDBMessage;
   }
   /**
    *
    * @param key
    * @return
    * @throws BaseException
    */
   public EDBMessage delete(String key)
         throws BaseException {
      IConnection connection = getConnection(key);

      if( null == connection ) {
         System.out.println("connection is failed");
         return null;
      }

      EDBMessage edbMessage = new EDBMessage();
      edbMessage.setOperationCode(Operation.OP_DELETE);

      BSONObject bson = (BSONObject)JSON.parse(key);
      edbMessage.setDelete(bson);
      byte[] request = EDBMessageHelper.buildDeleteRequest(edbMessage);

      // send message
      connection.sendMessage(request);

      byte[] recvBuffer = connection.receiveMessage();
      EDBMessage rtnSDBMessage = EDBMessageHelper.msgExtractReply(recvBuffer);
      return rtnSDBMessage;
   }

   private IConnection getConnection(String key) {
      ServerAddress sa = _edbCommon.getLocator().getPrimary(key);
      if(sa != null) {
         // for testing
         Integer times = _nodeRecordMap.get(sa);
         if(times == null) {
            _nodeRecordMap.put(sa, 1);
         } else {
            _nodeRecordMap.put(sa, times+1);
         }
      }

      return _edbCommon.getConnectionMap().get(sa);
   }


}
