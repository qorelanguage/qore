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

class ImportedFunctionEntry : public AbstractQoreFunction {
protected:
   QoreProgram *pgm;
   UserFunction *func;

public:
   DLLLOCAL ImportedFunctionEntry(QoreProgram *p, UserFunction *u) : pgm(p), func(u) {
   }
   DLLLOCAL ImportedFunctionEntry(const ImportedFunctionEntry &ife) : pgm(ife.pgm), func(ife.func) {
   }
   DLLLOCAL virtual AbstractQoreNode *evalFunction(const QoreListNode *args, ExceptionSink *xsink) const {
      // save current program location in case there's an exception
      const char *o_fn = get_pgm_file();
      int o_ln, o_eln;
      get_pgm_counter(o_ln, o_eln);
                                                                                                                                                                                   
      AbstractQoreNode *rv = pgm->callFunction(func, args, xsink);
      if (xsink->isException())
	 xsink->addStackInfo(CT_USER, 0, func->getName(), o_fn, o_ln, o_eln);

      return rv;                             
   }
   DLLLOCAL virtual const char *getName() const {
      return func->getName();
   }
   DLLLOCAL virtual const QoreTypeInfo *parseGetReturnTypeInfo() const {
      // we do not have to call UserFunction::parseGetReturnTypeInfo()
      // because ImportedFunctionEntry objects are only created at
      // run time
      return func->getReturnTypeInfo();
   }
   DLLLOCAL virtual const QoreTypeInfo *getReturnTypeInfo() const {
      return func->getReturnTypeInfo();
   }
   DLLLOCAL virtual ParamList *getParams() const {
      return func->getParams();
   }
   DLLLOCAL virtual bool isUserCode() const {
      return true;
   }
   DLLLOCAL virtual void ref() {}
   DLLLOCAL virtual void deref() {}
   DLLLOCAL QoreProgram *getProgram() {
      return pgm;
   }
   DLLLOCAL UserFunction *getFunction() {
      return func;
   }
};

typedef std::map<const char *, ImportedFunctionEntry *, class ltstr> ifn_map_t;

class ImportedFunctionList : public ifn_map_t {
public:
   DLLLOCAL ImportedFunctionList();
   DLLLOCAL ~ImportedFunctionList();
   DLLLOCAL void add(QoreProgram *pgm, UserFunction *func);
   DLLLOCAL UserFunction *find(const char *name) const;
   DLLLOCAL ImportedFunctionEntry *findNode(const char *name) const;
};

#endif
