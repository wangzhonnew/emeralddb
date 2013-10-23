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
package com.emeralddb.util;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.lang.reflect.Type;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.List;

import org.bson.BSONCallback;
import org.bson.BSONDecoder;
import org.bson.BSONEncoder;
import org.bson.BSONObject;
import org.bson.BasicBSONCallback;
import org.bson.BasicBSONDecoder;
import org.bson.BasicBSONEncoder;
import org.bson.BasicBSONObject;
import org.bson.io.BasicOutputBuffer;
import org.bson.io.OutputBuffer;

import com.emeralddb.base.EDBMessage;
import com.emeralddb.base.EmeralddbConstants;
import com.emeralddb.base.EmeralddbConstants.Operation;
import com.emeralddb.exception.BaseException;
import com.google.gson.Gson;
import com.google.gson.internal.Primitives;

/**
 * @author zhengle zhou
 * 
 */
public class EDBMessageHelper {
   private final static int MESSAGE_HEADER_LENGTH = 8;

   // msg.hpp - struct _MsgOpInsert
   private final static int MESSAGE_OPINSERT_LENGTH = 12;

   // msg.hpp - struct _MsgOpDelete
   private final static int MESSAGE_OPDELETE_LENGTH = 8;

   // msg.hpp - struct _MsgOpDelete
   private final static int MESSAGE_OPQUERY_LENGTH = 8;

   @SuppressWarnings("unused")
   public static byte[] buildQueryRequest(EDBMessage edbMessage)
         throws BaseException {
      Operation opCode = edbMessage.getOperationCode();
      int messageLen = 0;
      byte[] bsonData = bsonObjectToByteArray(edbMessage.getQuery());
      messageLen = Helper.roundToMultipleXLength(MESSAGE_OPQUERY_LENGTH, 4)
            + Helper.roundToMultipleXLength(bsonData.length, 4);

      // header
      List<byte[]> fieldList = new ArrayList<byte[]>();
      fieldList.add(assembleHeader(messageLen,opCode.OP_QUERY.getCode()));
      // data
      fieldList.add(Helper.roundToMultipleX(bsonData, 4));
      // Concatenate everything
      byte[] msgInByteArray = Helper.concatByteArray(fieldList);

      return msgInByteArray;
   }

   @SuppressWarnings("unused")
   public static byte[] buildInsertRequest(EDBMessage edbMessage)
         throws BaseException {
      Operation opCode = edbMessage.getOperationCode();

      int messageLen = 0;

      byte[] bsonData = bsonObjectToByteArray(edbMessage.getInsertor());
      byte[] bsonRecord = bsonObjectToByteArray(edbMessage.getInsertor());      
      messageLen = Helper.roundToMultipleXLength(MESSAGE_OPINSERT_LENGTH, 4)
            + Helper.roundToMultipleXLength(bsonData.length, 4);

      // header
      List<byte[]> fieldList = new ArrayList<byte[]>();

      fieldList.add(assembleHeader(messageLen,opCode.OP_INSERT.getCode()));
      // data
      int numRecord = 1;
      ByteBuffer buf = ByteBuffer.allocate(4);
      if (EmeralddbConstants.SYSTEM_ENDIAN == EmeralddbConstants.LITTLE_ENDIAN) {
         buf.order(ByteOrder.LITTLE_ENDIAN);
      } else {
         buf.order(ByteOrder.BIG_ENDIAN);
      }
      //record number
      buf.putInt(numRecord);
      fieldList.add(Helper.roundToMultipleX(buf.array(), 4));
      fieldList.add(Helper.roundToMultipleX(bsonRecord, 4));
      // Concatenate everything
      byte[] msgInByteArray = Helper.concatByteArray(fieldList);

      return msgInByteArray;
   }

   @SuppressWarnings("unused")
   public static byte[] buildDeleteRequest(EDBMessage edbMessage)
         throws BaseException {
      Operation opCode = edbMessage.getOperationCode();
      int messageLen = 0;

      byte[] bsonData = bsonObjectToByteArray(edbMessage.getDelete());
      messageLen = Helper.roundToMultipleXLength(MESSAGE_OPDELETE_LENGTH, 4)
            + Helper.roundToMultipleXLength(bsonData.length, 4);

      // header
      List<byte[]> fieldList = new ArrayList<byte[]>();
      fieldList.add(assembleHeader(messageLen,opCode.OP_DELETE.getCode()));

      // data
      fieldList.add(Helper.roundToMultipleX(bsonData, 4));

      // Concatenate everything
      byte[] msgInByteArray = Helper.concatByteArray(fieldList);

      return msgInByteArray;
   }

   public static byte[] buildInsertSnapshotRequest(EDBMessage edbMessage) throws BaseException {
      Operation opCode = edbMessage.getOperationCode();
      int messageLen = 0;

      messageLen = Helper.roundToMultipleXLength(MESSAGE_HEADER_LENGTH, 4);

      // header
      List<byte[]> fieldList = new ArrayList<byte[]>();
      fieldList.add(assembleHeader(messageLen,opCode.OP_INSERTSNAPSHOT.getCode()));

      // Concatenate everything
      byte[] msgInByteArray = Helper.concatByteArray(fieldList);

      return msgInByteArray;
   }

   public static byte[] buildDisconnectRequest(EDBMessage edbMessage) throws BaseException {
      Operation opCode = edbMessage.getOperationCode();
      int messageLen = 0;

      messageLen = Helper.roundToMultipleXLength(MESSAGE_HEADER_LENGTH, 4);

      // header
      List<byte[]> fieldList = new ArrayList<byte[]>();
      fieldList.add(assembleHeader(messageLen,opCode.OP_DISCONNECT.getCode()));

      // Concatenate everything
      byte[] msgInByteArray = Helper.concatByteArray(fieldList);

      return msgInByteArray;
   }


   @SuppressWarnings("unused")
   private static byte[] assembleHeader(int messageLength, int operationCode) {
      ByteBuffer buf = ByteBuffer.allocate(MESSAGE_HEADER_LENGTH);
      if (EmeralddbConstants.SYSTEM_ENDIAN == EmeralddbConstants.LITTLE_ENDIAN) {
         buf.order(ByteOrder.LITTLE_ENDIAN);
      } else {
         buf.order(ByteOrder.BIG_ENDIAN);
      }

      buf.putInt(messageLength);
      buf.putInt(operationCode);
      return buf.array();
   }

   public static EDBMessage msgExtractReply(byte[] inBytes)
         throws BaseException {
      List<byte[]> tmp = Helper.splitByteArray(inBytes, MESSAGE_HEADER_LENGTH);
      byte[] header = tmp.get(0);
      byte[] remaining = tmp.get(1);

      if (header.length != MESSAGE_HEADER_LENGTH || remaining == null) {
         //throw new BaseException("EDB_INVALIDSIZE");
         return null;
      }

      EDBMessage edbMessage = new EDBMessage();
      // extract and set header
      extractHeader(edbMessage, header);

      // set return code
      tmp = Helper.splitByteArray(remaining, 4);
      byte[] returnCode = tmp.get(0);
      remaining = tmp.get(1);
      edbMessage.setRc(Helper.byteToInt(returnCode));

      // set numReturn
      tmp = Helper.splitByteArray(remaining, 4);
      byte[] numReturn = tmp.get(0);
      remaining = tmp.get(1);
      edbMessage.setNumReturn(Helper.byteToInt(numReturn));


      // query
      if( Operation.OP_QUERY == edbMessage.getOperationCode()) {
         List<BSONObject> list = extractBSONObject(remaining);
         int nr = edbMessage.getNumReturn();
         if( nr == list.size() ) {
            for(int i=0; i< nr; i++) {
               BSONObject bsonObj = list.get(i);
               System.out.println( "query result " + i + " is " + bsonObj.toString() );
            }
         } else {
            System.out.println( "query result's size is not valid" );
         }
      } else if(Operation.OP_DELETE == edbMessage.getOperationCode()) {
         if(0 != edbMessage.getRc() ) {
            System.out.println( "Delete error,  code=" + edbMessage.getRc() );
         }
      } else if(Operation.OP_INSERT == edbMessage.getOperationCode() ) {
         if( 0 != edbMessage.getRc() ) {
            System.out.println( "Insert error, code=" + edbMessage.getRc() );
         }
      } else if(Operation.OP_INSERTSNAPSHOT ==  edbMessage.getOperationCode()) {
         List<BSONObject> list = extractBSONObject(remaining);
         for(int i=0; i<list.size(); i++) {
            BSONObject bo = list.get(i);
            int insertTimes = Integer.parseInt(bo.get("insertTimes").toString());
            edbMessage.setInsertTimes(insertTimes);
            break;
         }

      }
      return edbMessage;
   }

   private static List<BSONObject> extractBSONObject(byte[] inBytes)
         throws BaseException {
      int objLen;
      int objAllotLen;
      byte[] remaining = inBytes;
      List<BSONObject> objList = new ArrayList<BSONObject>();
      while (remaining != null) {
         objLen = getBSONObjectLength(remaining);
         if (objLen <= 0) {
            throw new BaseException("SDB_CLI_BSON_INV_LEN");
         }
         objAllotLen = Helper.roundToMultipleXLength(objLen, 4);

         List<byte[]> tmp = Helper.splitByteArray(remaining, objAllotLen);
         byte[] obj = tmp.get(0);
         remaining = tmp.get(1);

         byte[] bsonObj = Arrays.copyOfRange(obj, 0, objLen);
         objList.add(byteArrayToBSONObject(bsonObj));
      }

      return objList;
   }

   private static int getBSONObjectLength(byte[] inBytes) {
      byte[] tmp = new byte[4];

      tmp[0] = inBytes[0];
      tmp[1] = inBytes[1];
      tmp[2] = inBytes[2];
      tmp[3] = inBytes[3];

      return Helper.byteToInt(tmp);
   }

   private static void extractHeader(EDBMessage edbMessage, byte[] header) {
      List<byte[]> tmp = Helper.splitByteArray(header, 4);
      byte[] msgLength = tmp.get(0);
      byte[] remaining = tmp.get(1);

      // Request message length
      edbMessage.setRequestLength(Helper.byteToInt(msgLength));

      tmp = Helper.splitByteArray(remaining, 4);
      byte[] opCode = tmp.get(0);
      remaining = tmp.get(1);

      // Action code
      edbMessage.setOperationCode(Operation.getByValue(Helper.byteToInt(opCode)));
   }

   public static byte[] bsonObjectToByteArray(BSONObject obj) {
      BSONEncoder e = new BasicBSONEncoder();
      OutputBuffer buf = new BasicOutputBuffer();

      e.set(buf);
      e.putObject(obj);
      e.done();

      return buf.toByteArray();
   }

   @SuppressWarnings("unused")
   public static BSONObject byteArrayToBSONObject(byte[] inBytes)
         throws BaseException {
      if (inBytes == null || inBytes.length == 0)
         return null;

      BSONDecoder d = new BasicBSONDecoder();
      BSONCallback cb = new BasicBSONCallback();
      try {
         int s = d.decode(new ByteArrayInputStream(inBytes), cb);
         BSONObject o1 = (BSONObject) cb.get();
         return o1;
      } catch (IOException e) {
         throw new BaseException("SDB_INVALIDARG");
      }
   }

   public static BSONObject fromObject(Object object) throws BaseException {
      Gson gson = new Gson();
      String jString = gson.toJson(object);

      return fromJson(jString);
   }

   public static <T> T fromBson(BSONObject bObj, Class<T> classOfT) {
      bObj.removeField("_id");
      Gson gson = new Gson();
      Object object = gson.fromJson(bObj.toString(), (Type) classOfT);
      return Primitives.wrap(classOfT).cast(object);
   }

   public static BSONObject fromJson(String jsonString) throws BaseException {

      String fullString = "{\"bsonMap\":" + jsonString + "}";

      Gson gson = new Gson();
      ConvertHelpObject obj = gson.fromJson(fullString,
            ConvertHelpObject.class);

      LinkedHashMap<String, Object> bsonMap = obj.getBsonMap();

      BSONObject o1 = new BasicBSONObject();
      o1.putAll(bsonMap);

      return o1;
   }

   private class ConvertHelpObject {
      private LinkedHashMap<String, Object> bsonMap;

      public LinkedHashMap<String, Object> getBsonMap() {
         return bsonMap;
      }

      @SuppressWarnings("unused")
      public void setBsonMap(LinkedHashMap<String, Object> bsonMap) {
         this.bsonMap = bsonMap;
      }
   }

   @SuppressWarnings("unused")
   public static byte[] appendInsertMsg(byte[] msg, BSONObject append) {
      List<byte[]> tmp = Helper.splitByteArray(msg, 4);
      byte[] msgLength = tmp.get(0);
      byte[] remaining = tmp.get(1);
      byte[] insertor = bsonObjectToByteArray(append);
      int length = Helper.byteToInt(msgLength);
      int messageLength = length
            + Helper.roundToMultipleXLength(insertor.length, 4);

      ByteBuffer buf = ByteBuffer.allocate(messageLength);
      if (EmeralddbConstants.SYSTEM_ENDIAN == EmeralddbConstants.LITTLE_ENDIAN) {
         buf.order(ByteOrder.LITTLE_ENDIAN);
      } else {
         buf.order(ByteOrder.BIG_ENDIAN);
      }

      buf.putInt(messageLength);
      buf.put(remaining);
      buf.put(Helper.roundToMultipleX(insertor, 4));

      return buf.array();
   }

   private static String getMD5FromStr(String inStr) {
      MessageDigest md5 = null;
      try {
         md5 = MessageDigest.getInstance("MD5");
      } catch (Exception e) {
         e.printStackTrace();
         return "";
      }
      char[] charArray = inStr.toCharArray();
      byte[] byteArray = new byte[charArray.length];

      for (int i = 0; i < charArray.length; i++) {
         byteArray[i] = (byte) charArray[i];
      }

      byte[] md5Bytes = md5.digest(byteArray);

      StringBuffer hexValue = new StringBuffer();

      for (int i = 0; i < md5Bytes.length; i++) {
         int val = ((int) md5Bytes[i]) & 0xff;
         if (val < 16)
            hexValue.append("0");
         hexValue.append(Integer.toHexString(val));
      }

      return hexValue.toString();
   }
}
