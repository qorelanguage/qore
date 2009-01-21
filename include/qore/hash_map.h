/*
  hash_map.h

  header file for including hash_map functionality

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _QORE_HASH_MAP_H

#define _QORE_HASH_MAP_H

#include <qore/common.h>

// note: hash-map functionality has been removed for now pending redesign of the data structures
//       and implementation of an appropriate hashing algorithm

#include <map>

typedef std::map<const char*,  class Var *,               class ltstr> map_var_t;
typedef std::map<const char*,  class AbstractQoreNode *,  class ltstr> hm_qn_t;
typedef std::map<const char*,  class HashMember *,        class ltstr> hm_hm_t;
typedef std::map<const char*,  class QoreMethod *,        class ltstr> hm_method_t;
typedef std::map<const char*,  class BuiltinFunction *,   class ltstr> hm_bf_t;
typedef std::map<const char*,  class QoreClass *,         class ltstr> hm_qc_t;
typedef std::map<const char*,  class UserFunction *,      class ltstr> hm_uf_t;

#endif // _QORE_HASH_MAP_H
