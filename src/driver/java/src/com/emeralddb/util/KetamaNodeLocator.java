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

import java.util.List;
import java.util.SortedMap;
import java.util.TreeMap;
import com.emeralddb.net.ServerAddress;
public final class KetamaNodeLocator {
   private TreeMap<Long, ServerAddress> _ketamaNodes;
   private HashAlgorithm       _hashAlg;
   private int               _numReps = 160;

   public KetamaNodeLocator(List<ServerAddress> nodeList, HashAlgorithm alg, int nodeCopies) {
      _hashAlg = alg;
      _ketamaNodes = new TreeMap<Long,ServerAddress>();

      _numReps = nodeCopies;

      for(ServerAddress node : nodeList) {
         for(int i=0; i<_numReps/4; i++) {
            byte[] digest = _hashAlg.md5(node.getHost() + ":" + node.getPort() + i);
            for(int h=0; h<4; h++) {
               long m = _hashAlg.hash(digest,h);
               _ketamaNodes.put(m, node);
            }
         }
      }
   }

   public ServerAddress getPrimary(final String str) {
      byte[] digest = _hashAlg.md5(str);
      ServerAddress rv = getNodeForKey(_hashAlg.hash(digest, 0));
      return rv;
   }

   ServerAddress getNodeForKey(long hash) {
      final ServerAddress rv;
      Long key = hash;
      if(_ketamaNodes.isEmpty()) {
         return null;
      }
      if(!_ketamaNodes.containsKey(key)) {
         SortedMap<Long,ServerAddress> tailMap = _ketamaNodes.tailMap(key);
         if(tailMap.isEmpty()) {
            key = _ketamaNodes.firstKey();
         } else {
            key = tailMap.firstKey();
         }
      }
      rv = _ketamaNodes.get(key);
      return rv;
   }
}
