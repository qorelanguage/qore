/*
  hash_map.h

  header file for including hash_map functionality

  Qore Programming Language

  Copyright 2003 - 2008 David Nichols

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

#include <map>
typedef std::map<const char*, class Var *, class ltstr> map_var_t;

#ifdef HAVE_QORE_HASH_MAP

#include <qore/hash_map_include.h>

#include <strings.h>
#include <string.h>

#include <string>

class eqstr
{
   public:
      bool operator()(const char* s1, const char* s2) const
      {
	 //fprintf(stderr, "eqstr operator() this=%08p s1=%08p '%s' s2=%08p '%s' returning %s\n", this, s1, s1, s2, s2, !strcmp(s1, s2) ? "true" : "false");
         return !strcmp(s1, s2);
      }
};

#ifdef HAVE_UNORDERED_MAP
typedef unordered_map<const char*, class AbstractQoreNode *, hash<const char *>, class eqstr> hm_qn_t;
typedef unordered_map<const char*, class HashMember *,       hash<const char *>, class eqstr> hm_hm_t;
typedef unordered_map<const char*, class QoreMethod *,       hash<const char *>, class eqstr> hm_method_t;
typedef unordered_map<const char*, class BuiltinFunction *,  hash<const char *>, class eqstr> hm_bf_t;
typedef unordered_map<const char*, class QoreClass *,        hash<const char *>, class eqstr> hm_qc_t;
typedef unordered_map<const char*, class UserFunction *,     hash<const char *>, class eqstr> hm_uf_t;
#else
typedef hash_map<const char*, class AbstractQoreNode *, hash<const char *>, class eqstr> hm_qn_t;
typedef hash_map<const char*, class HashMember *,       hash<const char *>, class eqstr> hm_hm_t;
typedef hash_map<const char*, class QoreMethod *,       hash<const char *>, class eqstr> hm_method_t;
typedef hash_map<const char*, class BuiltinFunction *,  hash<const char *>, class eqstr> hm_bf_t;
typedef hash_map<const char*, class QoreClass *,        hash<const char *>, class eqstr> hm_qc_t;
typedef hash_map<const char*, class UserFunction *,     hash<const char *>, class eqstr> hm_uf_t;
#endif

#else // HAVE_QORE_HASH_MAP

typedef std::map<const char*, class AbstractQoreNode *, class ltstr> hm_qn_t;
typedef std::map<const char*, class HashMember *, class ltstr> hm_hm_t;
typedef std::map<const char*, const class QoreMethod *, class ltstr> hm_method_t;
typedef std::map<const char*, class BuiltinFunction *, class ltstr> hm_bf_t;
typedef std::map<const char*, class QoreClass *, class ltstr> hm_qc_t;
typedef std::map<const char*, class UserFunction *, class ltstr> hm_uf_t;

#endif // HAVE_QORE_HASH_MAP

#endif // _QORE_HASH_MAP_H
