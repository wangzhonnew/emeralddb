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

import java.util.List;

import org.bson.BSONObject;

import com.emeralddb.base.EmeralddbConstants.Operation;

/**
 * @author zhengle zhou
 * 
 */
public class EDBMessage {
	private int 		requestLength;
	private long 		requestID;
	private int 		responseTo;
	private Operation 	operationCode;
	private Operation 	returnCode;
	private int			numReturn;
	private int			insertTimes;        /*inert times*/
	private String 		messageText;
	private int 		rc;
	private BSONObject insertor;
	private BSONObject query;
	private BSONObject delete;
	
	private short w;
	private short padding;
	private int flags;
	private BSONObject matcher;
	private BSONObject selector;
	private BSONObject orderBy;
	private BSONObject hint;
	private int version;
	private int returnRowsCount2;
	private long returnRowsCount;
	private List<Long> contextIDList;

	private byte[] nodeID = new byte[12];
	public EDBMessage() {
	}
	
	public int getRc() {
		return rc;
	}

	public void setRc(int rc) {
		this.rc = rc;
	}

	public int getRequestLength() {
		return requestLength;
	}

	public void setRequestLength(int requestLength) {
		this.requestLength = requestLength;
	}

	public int getResponseTo() {
		return responseTo;
	}

	public void setResponseTo(int responseTo) {
		this.responseTo = responseTo;
	}

	public Operation getOperationCode() {
		return operationCode;
	}

	public void setOperationCode(Operation operationCode) {
		this.operationCode = operationCode;
	}

	public String getMessageText() {
		return messageText;
	}

	public void setMessageText(String messageText) {
		this.messageText = messageText;
	}

	public void setReturnCode(Operation returnCode) {
		this.returnCode = returnCode;
	}

	public Operation getReturnCode() {
		return returnCode;
	}

	public void setNumReturn(int numReturn) {
		this.numReturn = numReturn;
	}

	public int getNumReturn() {
		return numReturn;
	}

	public void setInsertor(BSONObject insertor) {
		this.insertor = insertor;
	}

	public BSONObject getInsertor() {
		return insertor;
	}

	public void setQuery(BSONObject query) {
		this.query = query;
	}

	public BSONObject getQuery() {
		return query;
	}

	public void setDelete(BSONObject delete) {
		this.delete = delete;
	}

	public BSONObject getDelete() {
		return delete;
	}

	public void setW(short w) {
		this.w = w;
	}

	public short getW() {
		return w;
	}

	public void setPadding(short padding) {
		this.padding = padding;
	}

	public short getPadding() {
		return padding;
	}

	public void setFlags(int flags) {
		this.flags = flags;
	}

	public int getFlags() {
		return flags;
	}

	public void setMatcher(BSONObject matcher) {
		this.matcher = matcher;
	}

	public BSONObject getMatcher() {
		return matcher;
	}

	public void setSelector(BSONObject selector) {
		this.selector = selector;
	}

	public BSONObject getSelector() {
		return selector;
	}

	public void setOrderBy(BSONObject orderBy) {
		this.orderBy = orderBy;
	}

	public BSONObject getOrderBy() {
		return orderBy;
	}

	public void setHint(BSONObject hint) {
		this.hint = hint;
	}

	public BSONObject getHint() {
		return hint;
	}

	public void setVersion(int version) {
		this.version = version;
	}

	public int getVersion() {
		return version;
	}

	public void setNodeID(byte[] nodeID) {
		this.nodeID = nodeID;
	}

	public byte[] getNodeID() {
		return nodeID;
	}

	public void setReturnRowsCount(long returnRowsCount) {
		this.returnRowsCount = returnRowsCount;
	}

	public long getReturnRowsCount() {
		return returnRowsCount;
	}

	public void setRequestID(long requestID) {
		this.requestID = requestID;
	}

	public long getRequestID() {
		return requestID;
	}

	public void setReturnRowsCount2(int returnRowsCount2) {
		this.returnRowsCount2 = returnRowsCount2;
	}

	public int getReturnRowsCount2() {
		return returnRowsCount2;
	}

	public void setContextIDList(List<Long> contextIDList) {
		this.contextIDList = contextIDList;
	}

	public List<Long> getContextIDList() {
		return contextIDList;
	}

	public void setInsertTimes(int insertTimes) {
		this.insertTimes = insertTimes;
	}

	public int getInsertTimes() {
		return insertTimes;
	}

}
