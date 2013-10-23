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

import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
/***
 * 
 * @author zhouzhengle
 *
 */
public enum HashAlgorithm {
	/***
	 * MD5-based hash algorithm used by ketama.
	 */
	KETAMA_HASH;
	
	public long hash( byte[] digest, int nTime ) {
		long rv = ( (long) (digest[3 + nTime*4] & 0xFF) << 24 )
				| ( (long) (digest[2 + nTime*4] & 0xFF ) << 16 )
				| ( (long) (digest[1 + nTime*4] & 0xFF) << 8 )
				| (digest[0 + nTime*4] & 0xFF);
		return rv & 0xFFFFFFFFL;	
	}
	
	/***
	 * Get the md5 of the given key.
	 */
	public byte[] md5(String k) {
		MessageDigest md5;
		try {
			md5 = MessageDigest.getInstance("MD5");
		} catch( NoSuchAlgorithmException e ) {
			throw new RuntimeException("MD5 not supported", e);
		}
		
		md5.reset();
		byte[] keyBytes = null;
		try {
			keyBytes = k.getBytes("UTF-8");
		} catch( UnsupportedEncodingException e ) {
			throw new RuntimeException("Unknown string :" + k, e);
		}
		
		md5.update(keyBytes);
		return md5.digest();
	}
	
}
