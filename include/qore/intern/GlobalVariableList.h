/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreProgram.h
 
  Program QoreObject Definition
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2015 David Nichols
 
  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_GLOBALVARIABLELIST_H

#define _QORE_GLOBALVARIABLELIST_H

#include <qore/intern/Variable.h>

#include <map>

class Var;

#ifdef HAVE_QORE_HASH_MAP
//#warning compiling with hash_map
#include <qore/hash_map_include.h>
#include <qore/intern/xxhash.h>

typedef HASH_MAP<const char*, Var*, qore_hash_str, eqstr> map_var_t;
#else
typedef std::map<const char*, Var*, ltstr> map_var_t;
#endif

// this is a "grow-only" container
// all reading and writing is done within the parse lock on the containing program object
class GlobalVariableList {
protected:
   // xxx DLLLOCAL Var* parseCreatePendingVar(const char* name, QoreParseTypeInfo* typeInfo);

public:
   map_var_t vmap, pending_vmap;

   DLLLOCAL GlobalVariableList() {
   }

   DLLLOCAL GlobalVariableList(const GlobalVariableList& old, int64 po);

   DLLLOCAL void mergePublic(const GlobalVariableList& old);

   DLLLOCAL ~GlobalVariableList() {
      assert(vmap.empty());
   }

   DLLLOCAL void clearAll(ExceptionSink *xsink);
   DLLLOCAL void deleteAll(ExceptionSink* xsink);

   // called at runtime
   // returns a non-0 Var* if a new variable was created, 0 if not (because it already existed - exception raised)
   DLLLOCAL Var* import(Var* var, ExceptionSink* xsink, bool readonly = false);

   DLLLOCAL Var* runtimeCreateVar(const char* name, const QoreTypeInfo* typeInfo);

   DLLLOCAL Var* parseFindVar(const char* name);
   DLLLOCAL Var* parseCreatePendingVar(const char* name, const QoreTypeInfo* typeInfo);
   DLLLOCAL const Var* parseFindVar(const char* name) const;

   DLLLOCAL void parseAdd(Var* v);

   // xxx DLLLOCAL Var* parseFindCreateVar(const char* name, QoreParseTypeInfo* typeInfo, bool& new_var);
   // xxx DLLLOCAL Var* parseFindCreateVar(const char* name, const QoreTypeInfo* typeInfo, bool& new_var);

   DLLLOCAL Var* runtimeFindVar(const char* name) {
      map_var_t::iterator i = vmap.find(name);
      return i != vmap.end() ? i->second : 0;
   }

   DLLLOCAL QoreListNode* getVarList() const;

   DLLLOCAL void parseInit();
   DLLLOCAL void parseCommit();
   DLLLOCAL void parseRollback();
};

#endif
