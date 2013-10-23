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

import com.emeralddb.exception.BaseException;
/**
 * @author Jacky Zhang
 * 
 */
public interface IConnection {
	public void initialize() throws BaseException;
	public void close();
	public void changeConfigOptions(ConfigOptions opts) throws BaseException;

	public long sendMessage(byte[] msg) throws BaseException;

	public byte[] receiveMessage() throws BaseException;

}
