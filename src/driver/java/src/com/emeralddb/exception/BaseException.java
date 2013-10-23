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

import com.emeralddb.base.EmeralddbConstants;

/**
 * @author Jacky Zhang
 * 
 */
public class BaseException extends RuntimeException {

	/**
	 * 
	 */
	private static final long serialVersionUID = -6115487863398926195L;

	private EDBError error;
	private String infos = "";

	/**
	 * @param errorType
	 * @throws Exception
	 */
	public BaseException(String errorType, Object... info) {
		error = new EDBError();
		error.setErrorType(errorType);

		if (info != null) {
			for (Object obj : info)
				infos += (obj + " ");
		} else {
			infos = "no more exception info";
		}
		try {
			error.setErrorCode(EDBErrorLookup.getErrorCodeByType(errorType));
			error.setErrorDescription(EDBErrorLookup
					.getErrorDescriptionByType(errorType) + "\n Exception Detail:" + infos);
		} catch (Exception e) {
			error.setErrorCode(0);
			error.setErrorDescription(EmeralddbConstants.UNKNOWN_DESC + "\n Exception Detail:"
					+ infos);
		}
	}

	/**
	 * 
	 * @param errorCode
	 * @throws Exception
	 */
	public BaseException(int errorCode, Object... info) {
		error = new EDBError();
		error.setErrorCode(errorCode);
		if (info != null) {
			for (Object obj : info)
				infos += (obj + " ");
		} else {
			infos = "no more exception info";
		}
		try {
			error.setErrorType(EDBErrorLookup.getErrorTypeByCode(errorCode));
			error.setErrorDescription(EDBErrorLookup
					.getErrorDescriptionByCode(errorCode) + "\n Exception Detail:" + infos);
		} catch (Exception e) {
			error.setErrorType(EmeralddbConstants.UNKNOWN_DESC);
			error.setErrorDescription(EmeralddbConstants.UNKNOWN_DESC + "\n Exception Detail:"
					+ infos);
		}
	}

	@Override
	public String getMessage() {
		return error.getErrorDescription();
	}

	public String getErrorType() {
		return error.getErrorType();
	}

	public int getErrorCode() {
		return error.getErrorCode();
	}
}
