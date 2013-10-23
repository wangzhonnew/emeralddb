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
package com.emeralddb.exception;

import java.io.InputStream;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Properties;

import com.emeralddb.base.EmeralddbConstants;

/**
 * @author Jacky Zhang
 * 
 */
public class EDBErrorLookup {
	private static HashMap<String, EDBError> mapByType;
	private static HashMap<Integer, EDBError> mapByCode;

	/**
	 * @throws Exception
	 */
	public EDBErrorLookup() throws Exception {
		loadErrorMap();
	}

	/**
	 * @throws Exception
	 */
	@SuppressWarnings("rawtypes")
	private static void loadErrorMap() throws Exception {
		mapByType = new HashMap<String, EDBError>();
		mapByCode = new HashMap<Integer, EDBError>();

		// Read properties file.
		InputStream in = EDBErrorLookup.class.getClassLoader()
				.getResourceAsStream("errors.properties");
		Properties properties = new Properties();
		properties.load(in);

		Enumeration em = properties.keys();
		while (em.hasMoreElements()) {
			String errorType = (String) em.nextElement();
			String proValue = (String) properties.get(errorType);
			String[] tmp = proValue.split(":");
			String errorCodeStr = tmp[0].trim();
			String errorDesc = tmp[1].trim();

			int errorCode = Integer.parseInt(errorCodeStr);

			EDBError err = new EDBError();
			err.setErrorCode(errorCode);
			err.setErrorDescription(errorDesc);
			err.setErrorType(errorType);

			mapByCode.put(errorCode, err);
			mapByType.put(errorType, err);
		}
	}

	/**
	 * @param errorType
	 * @return
	 * @throws Exception
	 */
	public static String getErrorDescriptionByType(String errorType)
			throws Exception {
		if (mapByType == null)
			loadErrorMap();

		EDBError errObj = mapByType.get(errorType);

		if (errObj != null)
			return errObj.getErrorDescription();

		return EmeralddbConstants.UNKNOWN_DESC;
	}

	/**
	 * @param errorCode
	 * @return
	 * @throws Exception
	 */
	public static String getErrorDescriptionByCode(int errorCode)
			throws Exception {
		if (mapByCode == null)
			loadErrorMap();

		EDBError errObj = mapByCode.get(errorCode);

		if (errObj != null)
			return errObj.getErrorDescription();

		return EmeralddbConstants.UNKNOWN_DESC;
	}

	/**
	 * @param errorType
	 * @return
	 * @throws Exception
	 */
	public static int getErrorCodeByType(String errorType) throws Exception {
		if (mapByType == null)
			loadErrorMap();

		EDBError errObj = mapByType.get(errorType);

		if (errObj != null)
			return errObj.getErrorCode();

		return EmeralddbConstants.UNKNOWN_CODE;
	}

	/**
	 * @param errorCode
	 * @return
	 * @throws Exception
	 */
	public static String getErrorTypeByCode(int errorCode) throws Exception {
		if (mapByCode == null)
			loadErrorMap();

		EDBError errObj = mapByCode.get(errorCode);

		if (errObj != null)
			return errObj.getErrorType();

		return EmeralddbConstants.UNKNOWN_TYPE;
	}
}
