/*
  hash_map.h

  header file for including hash_map functionality

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#ifdef HAVE_QORE_HASH_MAP

#include <qore/hash_map_include.h>

#include <strings.h>

#include <string.h>

class eqstr
{
   public:
      bool operator()(char* s1, char* s2) const
      {
         return !strcmp(s1, s2);
      }
};

typedef hash_map<char*, class QoreNode *, hash<char *>, class eqstr> hm_qn_t;
typedef hash_map<char*, class HashMember *, hash<char *>, class eqstr> hm_hm_t;
typedef hash_map<char*, class Method *, hash<char *>, class eqstr> hm_method_t;
typedef hash_map<char*, class BuiltinFunction *, hash<char *>, class eqstr> hm_bf_t;
typedef hash_map<char*, class QoreClass *, hash<char *>, class eqstr> hm_qc_t;
typedef hash_map<char*, class UserFunction *, hash<char *>, class eqstr> hm_uf_t;
typedef hash_map<char*, class Var *, hash<char *>, class eqstr> hm_var_t;

#else // HAVE_QORE_HASH_MAP
#include <map>

typedef std::map<char*, class QoreNode *, class ltstr> hm_qn_t;
typedef std::map<char*, class HashMember *, class ltstr> hm_hm_t;
typedef std::map<char*, class Method *, class ltstr> hm_method_t;
typedef std::map<char*, class BuiltinFunction *, class ltstr> hm_bf_t;
typedef std::map<char*, class QoreClass *, class ltstr> hm_qc_t;
typedef std::map<char*, class UserFunction *, class ltstr> hm_uf_t;
typedef std::map<char*, class Var *, class ltstr> hm_var_t;

#endif // HAVE_QORE_HASH_MAP

#endif // _QORE_HASH_MAP_H
