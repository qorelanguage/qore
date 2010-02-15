/* -*- mode: c++; indent-tabs-mode nil -*- */
/*
 ImportedFunctionList.h
 
 Qore Programming Language
 
 Copyright 2003 - 2010 David Nichols
 
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

#ifndef _QORE_IMPORTEDFUNCTIONLIST_H

#define _QORE_IMPORTEDFUNCTIONLIST_H

#include <qore/common.h>

#include <map>

// imported functions are handled with FunctionCallNode; the program is set and the function is executed
class ImportedFunctionEntry {
protected:
   QoreProgram *pgm;
   UserFunction *func;

public:
   DLLLOCAL ImportedFunctionEntry(QoreProgram *p, UserFunction *u) : pgm(p), func(u) {
   }
   DLLLOCAL ImportedFunctionEntry(const ImportedFunctionEntry &ife) : pgm(ife.pgm), func(ife.func) {
   }
   DLLLOCAL QoreProgram *getProgram() {
      return pgm;
   }
   DLLLOCAL UserFunction *getFunction() {
      return func;
   }
/*
   DLLLOCAL virtual const char *getName() const {
      return func->getName();
   }
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const {
      // we do not have to call UserFunction::parseGetReturnTypeInfo()
      // because ImportedFunctionEntry objects are only created at
      // run time
      return func->getUniqueReturnTypeInfo();
   }
   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const {
      return func->getUniqueReturnTypeInfo();
   }
   DLLLOCAL virtual void ref() {}
   DLLLOCAL virtual void deref() {}
   DLLLOCAL virtual void parseInit() {
   }
   DLLLOCAL virtual void parseCommit() {
   }
   DLLLOCAL virtual void parseRollback() {
   }
*/
};

typedef std::map<const char *, ImportedFunctionEntry *, class ltstr> ifn_map_t;

class ImportedFunctionList : public ifn_map_t {
public:
   DLLLOCAL ImportedFunctionList();
   DLLLOCAL ~ImportedFunctionList();
   DLLLOCAL void add(QoreProgram *pgm, UserFunction *func);
   DLLLOCAL UserFunction *find(const char *name, QoreProgram *&pgm) const;
   DLLLOCAL ImportedFunctionEntry *findNode(const char *name) const;
};

#endif
