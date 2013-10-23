/**
 *      Copyright (C) 2012 SequoiaDB Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE= -2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
package com.emeralddb.base;

import com.emeralddb.exception.BaseException;

/**
 * @author zhengle zhou
 * 
 */
public class EmeralddbConstants {
	public final static String 	OP_ERRNOFIELD = "errno";
	public final static int 	OP_MAXNAMELENGTH = 255;

	public final static int 	COLLECTION_SPACE_MAX_SZ = 127;
	public final static int 	COLLECTION_MAX_SZ = 127;
	public final static int 	MAX_CS_SIZE = 127;

	public static final String 	UNKNOWN_TYPE = "UNKNOWN";
	public static final String 	UNKNOWN_DESC = "Unknown Error";
	public static final int 	UNKNOWN_CODE = 1;

	public static final String 	ADMIN_PROMPT = "$";
	public static final String 	LIST_CMD = "list";
	public static final String 	CREATE_CMD = "create";
	public static final String 	DROP_CMD = "drop";
	public static final String 	SNAP_CMD = "snapshot ";
	public static final String 	TEST_CMD = "test";
	public static final String 	ACTIVE_CMD = "active";
	public static final String 	SHUTDOWN_CMD = "shutdown";
	public final static String 	COLSPACES = "collectionspaces";
	public final static String 	COLLECTIONS = "collections";
	public final static String 	COLSPACE = "collectionspace";
	public final static String 	NODE = "node";
	public final static String 	NODE_NAME_SEP = ":";
	public final static String 	CONTEXTS = "contexts";
	public final static String 	CONTEXTS_CUR = "contexts current";
	public final static String 	SESSIONS = "sessions";
	public final static String 	SESSIONS_CUR = "sessions current";
	public final static String 	STOREUNITS = "storageunits";
	public final static String 	COLLECTION = "collection";
	public final static String 	CREATE_INX = "create index";
	public final static String 	DROP_INX = "drop index";
	public final static String 	GET_INXES = "get indexes";
	public final static String 	GET_COUNT = "get count";
	public final static String 	DATABASE = "database";
	public final static String 	SYSTEM = "system";
	public final static String 	RESET = "reset";
	public final static String 	RENAME_COLLECTION = "rename collection";
	public final static String 	GROUP = "group";
	public final static String 	GROUPS = "groups";

	public final static String 	CMD_NAME_LIST_CONTEXTS = "list contexts";
	public final static String 	CMD_NAME_LIST_CONTEXTS_CURRENT = "list contexts current";
	public final static String 	CMD_NAME_LIST_SESSIONS = "list sessions";
	public final static String 	CMD_NAME_LIST_SESSIONS_CURRENT = "list sessions current";
	public final static String 	CMD_NAME_LIST_COLLECTIONS = "list collections";
	public final static String 	CMD_NAME_LIST_COLLECTIONSPACES = "list collectionspaces";
	public final static String 	CMD_NAME_LIST_STORAGEUNITS = "list storageunits";
	public final static String 	CMD_NAME_LIST_GROUPS = "list groups";
	public final static String 	CMD_NAME_CREATE_GROUP = "create group";
	public final static String 	CMD_NAME_ACTIVE_GROUP = "active group";
	public final static String 	CMD_NAME_STARTUP_NODE = "startup node";
	public final static String 	CMD_NAME_SHUTDOWN_NODE = "shutdown node";
	public final static String 	CMD_NAME_SPLIT = "split";
	public final static String 	CMD_NAME_CREATE_CATA_GROUP = "create catalog group";

	public final static String 	FIELD_NAME_NAME = "Name";
	public final static String 	FIELD_NAME_OLDNAME = "OldName";
	public final static String 	FIELD_NAME_NEWNAME = "NewName";
	public final static String 	FIELD_NAME_PAGE_SIZE = "PageSize";
	public final static String 	FIELD_NAME_HOST = "HostName";
	public final static String 	FIELD_NAME_COLLECTIONSPACE = "CollectionSpace";
	public final static String 	FIELD_NAME_GROUPNAME = "GroupName";
	public final static String 	FIELD_NAME_GROUPSERVICE = "Service";
	public final static String 	FIELD_NAME_GROUP = "Group";
	public final static String 	FIELD_NAME_NODEID = "NodeID";
	public final static String 	FIELD_NAME_GROUPID = "GroupID";
	public final static String 	FIELD_NAME_PRIMARY = "PrimaryNode";
	public final static String 	FIELD_NAME_SERVICENAME = "Name";
	public final static String 	FIELD_NAME_SERVICETYPE = "Type";
	public final static String 	FIELD_NAME_SOURCE = "Source";
	public final static String 	FIELD_NAME_TARGET = "Target";
	public final static String 	FIELD_NAME_SPLITQUERY = "SplitQuery";
	public final static String 	FIELD_NAME_CATALOGSHARDINGKEY = "ShardingKey";
	public final static String 	FIELD_COLLECTION = "Collection";
	public final static String 	FIELD_TOTAL = "Total";
	public final static String 	FIELD_INDEX = "Index";
	
	public final static String 	IXM_NAME = "name";
	public final static String 	IXM_KEY = "key";
	public final static String 	IXM_UNIQUE = "unique";
	public final static String 	IXM_ENFORCED = "enforced";
	public final static String 	IXM_INDEXDEF = "IndexDef";
	
	public final static String 	PMD_OPTION_SVCNAME = "svcname";
	public final static String 	PMD_OPTION_DBPATH = "dbpath";

	public final static String 	OID = "_id";

	public final static int 	FLG_UPDATE_UPSERT = 0x00000001;
	public final static int 	FLG_REPLY_CONTEXTSORNOTFOUND = 0x00000001;
	public final static int 	FLG_REPLY_SHARDCONFSTALE = 0x00000004;

	public final static int 	EDB_DMS_EOC = new BaseException("EDB_DMS_EOC").getErrorCode();

	public final static String 	LITTLE_ENDIAN = "LITTLE_ENDIAN";
	public final static String 	BIG_ENDIAN = "BIG_ENDIAN";
	public final static String 	SYSTEM_ENDIAN = LITTLE_ENDIAN;

	public final static byte[] 	ZERO_NODEID = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	public enum Operation {
		OP_REPLY(1), 
		OP_INSERT(2), 
		OP_DELETE(3), 
		OP_QUERY(4), 
		OP_COMMAND(5),
		OP_DISCONNECT(6),
		OP_RETURN_OK(7), 
		OP_INSERTSNAPSHOT(8), 
		RESERVED(9);
		
		private int code;

		private Operation(int code) {
			this.code = code;
		}

		public int getCode() {
			return code;
		}

		public static Operation getByValue(int inVal) {
			Operation rtnOper = Operation.RESERVED;
			for (Operation oper : Operation.values()) {
				if (oper.getCode() == inVal) {
					rtnOper = oper;
					break;
				}
			}
			return rtnOper;
		}
	} /*Operation*/
}
