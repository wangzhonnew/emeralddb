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
 * @package com.sequoiadb.net;
 * @brief SequoiaDB Driver for Java
 * @author Jacky Zhang
 */
package com.emeralddb.net;

/**
 * @class ConfigOptions
 * @brief Database Connection Configuration Option
 */
public class ConfigOptions {
	private long maxAutoConnectRetryTime = 15000;
	private int connectTimeout = 10000;
	private int socketTimeout = 0;
	private boolean socketKeepAlive = false;
	private boolean useNagle = false;

	/**
	 * @fn int getSocketTimeout()
	 * @brief Get the socket timeout(milliseconds)
	 * @return the socket timeout(milliseconds)(int)
	 */
	public int getSocketTimeout() {
		return socketTimeout;
	}

	/**
	 * @fn void setSocketTimeout(int socketTimeout)
	 * @brief Set the socket timeout(milliseconds)
	 * @param socketTimeout(int)
	 */
	public void setSocketTimeout(int socketTimeout) {
		this.socketTimeout = socketTimeout;
	}

	/**
	 * @fn boolean getSocketKeepAlive()
	 * @brief Get whether the socket keeps alive or not
	 * @return the status(boolean)
	 */
	public boolean getSocketKeepAlive() {
		return socketKeepAlive;
	}

	/**
	 * @fn void setSocketKeepAlive(boolean socketKeepAlive)
	 * @brief Set the status of socket
	 * @param socketKeepAlive the the status of socket(boolean)
	 */
	public void setSocketKeepAlive(boolean socketKeepAlive) {
		this.socketKeepAlive = socketKeepAlive;
	}

	/**
	 * @fn boolean getUseNagle()
	 * @brief Get whether use the Nagle Algorithm or not
	 * @return boolean
	 */
	public boolean getUseNagle() {
		return useNagle;
	}

	/**
	 * @fn void setUseNagle(boolean useNagle)
	 * @brief Set whether use the Nagle Algorithm or not
	 * @param useNagle(boolean)
	 */
	public void setUseNagle(boolean useNagle) {
		this.useNagle = useNagle;
	}

	/**
	 * @fn int getConnectTimeout()
	 * @brief Get the connect timeout(milliseconds)
	 * @return the connect timeout(int)
	 */
	public int getConnectTimeout() {
		return connectTimeout;
	}

	/**
	 * @fn void setConnectTimeout(int connectTimeout)
	 * @brief Set the connect timeout(milliseconds)
	 * @param connectTimeout(int)
	 */
	public void setConnectTimeout(int connectTimeout) {
		this.connectTimeout = connectTimeout;
	}

	/**
	 * @fn long getMaxAutoConnectRetryTime()
	 * @brief Get the max auto connect retry time(milliseconds)
	 * @return the max auto connect retry time(long)
	 */
	public long getMaxAutoConnectRetryTime() {
		return maxAutoConnectRetryTime;
	}

	/**
	 * @fn void setMaxAutoConnectRetryTime(long maxAutoConnectRetryTime)
	 * @brief Set the max auto connect retry time(milliseconds)
	 * @param maxAutoConnectRetryTime(long)
	 */
	public void setMaxAutoConnectRetryTime(long maxAutoConnectRetryTime) {
		this.maxAutoConnectRetryTime = maxAutoConnectRetryTime;
	}
}
