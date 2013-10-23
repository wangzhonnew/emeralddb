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
package com.emeralddb.net;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

import com.emeralddb.exception.BaseException;
import com.emeralddb.util.Helper;
/**
 * @author zhengle zhou
 * 
 */
public class ConnectionTCPImpl implements IConnection {

   private Socket clientSocket;
   private InputStream input;
   private OutputStream output;
   private ConfigOptions options;
   private ServerAddress hostAddress;

   public ConnectionTCPImpl(ServerAddress addr, ConfigOptions options) {
      this.hostAddress = addr;
      this.options = options;
   }

   private void connect() throws BaseException {
      if (clientSocket != null) {
         return;
      }

      long sleepTime = 100;
      long maxAutoConnectRetryTime = options.getMaxAutoConnectRetryTime();

      long start = System.currentTimeMillis();
      while (true) {
         BaseException lastError = null;
         InetSocketAddress addr = hostAddress.getHostAddress();
         try {
            clientSocket = new Socket();
            clientSocket.connect(addr, options.getConnectTimeout());

            clientSocket.setTcpNoDelay(false);
            clientSocket.setKeepAlive(options.getSocketKeepAlive());
            clientSocket.setSoTimeout(options.getSocketTimeout());
            input = new BufferedInputStream(clientSocket.getInputStream());
            output = clientSocket.getOutputStream();
            return;
         } catch (IOException ioe) {
            ioe.printStackTrace();
            lastError = new BaseException("EDB_NETWORK");
            close();
         }

         long executedTime = System.currentTimeMillis() - start;

         if (executedTime >= maxAutoConnectRetryTime)
            throw lastError;

         if (sleepTime + executedTime > maxAutoConnectRetryTime)
            sleepTime = maxAutoConnectRetryTime - executedTime;

         try {
            Thread.sleep(sleepTime);
         } catch (InterruptedException e) {
         }
         sleepTime *= 2;
      }
   }

   public void close() {
      if (clientSocket != null) {
         try {
            clientSocket.close();
         } catch (Exception e) {
         } finally {
            input = null;
            output = null;
            clientSocket = null;
         }
      }
   }

   /*
    * (non-Javadoc)
    * 
    * @see com.sequoiadb.net.IConnection#changeConfigOptions(com.sequoiadb.net.
    * ConfigOptions)
    */
   @Override
   public void changeConfigOptions(ConfigOptions opts) throws BaseException {
      this.options = opts;
      close();
      connect();
   }

   /*
    * (non-Javadoc)
    * 
    * @see com.sequoiadb.net.IConnection#sendMessage(byte[], int)
    */
   @Override
   public long sendMessage(byte[] msg) throws BaseException {
      long start = System.nanoTime();
      try
      {
         output.write(msg);
      }
      catch ( IOException e )
      {
         throw new BaseException ( "EDB_NETWORK" ) ;
      }
      long end = System.nanoTime();
      return (end-start);
   }

   /*
    * (non-Javadoc)
    * 
    * @see com.sequoiadb.net.IConnection#receiveMessage()
    */
   @Override
   public byte[] receiveMessage() throws BaseException {
      byte[] buf = new byte[4];

      input.mark(4);

      try
      {
         int rtn = input.read(buf);

         if (rtn != 4) {
            close();
            throw new BaseException("EDB_NETWORK");
         }
         int msgSize = Helper.byteToInt(buf);

         input.reset();

         buf = new byte[msgSize];
         rtn = 0;
         int retSize = 0;
         while (rtn < msgSize) {
            retSize = input.read(buf, rtn, msgSize - rtn);
            if (-1 == retSize) {
               close();
               throw new BaseException("EDB_NETWORK");
            }
            rtn += retSize;
         }
         if (rtn != msgSize) {
            StringBuffer bbf = new StringBuffer();
            for (byte by : buf) {
               bbf.append(String.format("%02x", by));
            }
            close();
            throw new BaseException("EDB_INVALIDARG");
         }
      }
      catch ( IOException e )
      {
         throw new BaseException ( "EDB_NETWORK" ) ;
      }

      return buf;
   }

   /*
    * (non-Javadoc)
    *
    * @see com.sequoiadb.net.IConnection#initialize()
    */
   @Override
   public void initialize() throws BaseException {
      connect();
   }

}
