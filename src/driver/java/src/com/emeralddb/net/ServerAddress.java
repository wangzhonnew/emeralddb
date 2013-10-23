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

import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;

import com.emeralddb.exception.BaseException;

/**
 * @author Jacky Zhang
 * 
 */
public class ServerAddress {
	private final static String DEFAULT_HOST = "127.0.0.1";
	private final static int DEFAULT_PORT = 48123;
	private InetSocketAddress hostAddress;
	private String host;
	private int port;

	/**
	 * @return
	 */
	public InetSocketAddress getHostAddress() {
		return hostAddress;
	}

	/**
	 * @return
	 */
	public String getHost() {
		return host;
	}

	/**
	 * @return
	 */
	public int getPort() {
		return port;
	}

	/**
	 * 
	 */
	public ServerAddress() {
		this(new InetSocketAddress(DEFAULT_HOST, DEFAULT_PORT));
	}

	/**
	 * @param host
	 * @param port
	 * @throws UnknownHostException
	 */
	public ServerAddress(String host, int port) throws UnknownHostException {
		hostAddress = new InetSocketAddress(InetAddress.getByName(host).toString().split("/")[1], port);
		this.host = host;
		this.port = port;
	}

	/**
	 * @param host
	 * @throws UnknownHostException
	 */
	public ServerAddress(String host) throws UnknownHostException {
		if (host.indexOf(":") > 0) {
			String[] tmp = host.split(":");
			this.host = tmp[0].trim();
			try {
			this.host = InetAddress.getByName(this.host).toString().split("/")[1];
			} catch (Exception e) {
				throw new BaseException("SDB_INVALIDARG");
			}
			this.port = Integer.parseInt(tmp[1].trim());
		} else {
			this.host = host;
			this.port = DEFAULT_PORT;
		}
		hostAddress = new InetSocketAddress(this.host, this.port);
	}

	/**
	 * @param addr
	 */
	public ServerAddress(InetAddress addr) {
		this(new InetSocketAddress(addr, DEFAULT_PORT));
	}

	/**
	 * @param addr
	 * @param port
	 */
	public ServerAddress(InetAddress addr, int port) {
		this(new InetSocketAddress(addr, port));
	}

	/**
	 * @param addr
	 */
	public ServerAddress(InetSocketAddress addr) {
		hostAddress = addr;
		host = addr.getHostName();
		port = addr.getPort();
	}
}
