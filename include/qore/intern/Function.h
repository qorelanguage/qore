/*
  Function.h

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

#ifndef _QORE_FUNCTION_H

#define _QORE_FUNCTION_H

#include <qore/QoreReferenceCounter.h>
#include <qore/Restrictions.h>
#include <qore/common.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constructors and destructors can never be explicitly called so we don't need FunctionCall constants for them
#define FC_UNRESOLVED     1
#define FC_USER           2
#define FC_BUILTIN        3
#define FC_SELF           4
#define FC_IMPORTED       5
#define FC_METHOD         6
#define FC_STATICUSERREF  7 // only used by CallReference
#define FC_STATIC_METHOD2 8

// these data structures are all private to the library
// FIXME: messy implementation - clean up!

AbstractQoreNode *doPartialEval(class AbstractQoreNode *n, bool *is_self_ref, ExceptionSink *xsink);

// object definitions and interfaces

class LocalVar;
class VarRefNode;

class ParamList {
protected:
   unsigned num_params;

public:
   DLLLOCAL ParamList(unsigned n_num_params) : num_params(n_num_params) {
   }
   DLLLOCAL virtual ~ParamList() {
   }
   DLLLOCAL unsigned numParams() const {
      return num_params;
   }
   DLLLOCAL const QoreTypeInfo *getParamTypeInfo(unsigned num) const {
      return num >= num_params ? 0 : getParamTypeInfoImpl(num);
   }
   DLLLOCAL virtual void resolve() = 0;
   DLLLOCAL virtual const QoreTypeInfo *getParamTypeInfoImpl(unsigned num) const = 0;
};

class UserParamList : public ParamList {
protected:
   DLLLOCAL void assignParam(int i, VarRefNode *v);

   DLLLOCAL void setSingleParamIntern(VarRefNode *v) {
      num_params = 1;
      names = new char *[1];
      typeList = new QoreParseTypeInfo *[1];
      assignParam(0, v);
   }

   DLLLOCAL static void param_error() {
      parse_error("parameter list contains non-variable reference expressions");
   }

public:
   char **names;
   QoreParseTypeInfo **typeList;
   LocalVar **lv;
   LocalVar *argvid;
   LocalVar *selfid;
   bool resolved;

   DLLLOCAL UserParamList(AbstractQoreNode *params) : ParamList(0), resolved(false) {
      ReferenceHolder<AbstractQoreNode> param_holder(params, 0);
      
      lv = 0;
      if (!params) {
	 num_params = 0;
	 names = 0;
	 typeList = 0;
	 return;
      }
      
      if (params->getType() == NT_VARREF) {
	 setSingleParamIntern(reinterpret_cast<VarRefNode *>(params));
	 return;
      }

      if (params->getType() != NT_LIST) {
	 param_error();
	 num_params = 0;
	 names = 0;
	 typeList = 0;
	 return;
      }
      
      QoreListNode *l = reinterpret_cast<QoreListNode *>(params);
      
      num_params = l->size();
      names = new char *[num_params];
      typeList = new QoreParseTypeInfo *[num_params];
      for (unsigned i = 0; i < num_params; i++) {
	 AbstractQoreNode *n = l->retrieve_entry(i);
	 qore_type_t t = n ? n->getType() : 0;
	 if (t != NT_VARREF) {
	    if (n)
	       param_error();
	    num_params = 0;
	    delete [] names;
	    names = 0;
	    delete [] typeList;
	    typeList = 0;
	    break;
	 }
	 
	 assignParam(i, reinterpret_cast<VarRefNode *>(n));
	 //printd(5, "UserParamList::UserParamList() this=%p i=%d %s typelist[%d]=%p has_type=%d type=%d class=%s\n", this, i, names[i], i, typeList[i], typeList[i] ? typeList[i]->has_type : 0, typeList[i] ? typeList[i]->qt : 0, typeList[i] && typeList[i]->qc ? typeList[i]->qc->getName() : "n/a");
      }
   }

   DLLLOCAL virtual ~UserParamList() {
      for (unsigned i = 0; i < num_params; i++)
	 free(names[i]);
      if (names)
	 delete [] names;
      if (typeList) {
	 for (unsigned i = 0; i < num_params; i++)
	    delete typeList[i];
	 delete [] typeList;
      }
      if (lv)
	 delete [] lv;
   }

   DLLLOCAL virtual const QoreTypeInfo *getParamTypeInfoImpl(unsigned num) const {
      return typeList[num];
   }
      
   DLLLOCAL virtual void resolve() {
      if (resolved)
	 return;
      
      resolved = true;
      if (!num_params)
	 return;

      for (unsigned i = 0; i < num_params; ++i)
	 if (typeList[i])
	    typeList[i]->resolve();			 
   }

   DLLLOCAL void parseInitPushLocalVars(const QoreTypeInfo *classTypeInfo);

   DLLLOCAL void parseInitPopLocalVars();
};

class UserParamListLocalVarHelper {
protected:
   UserParamList *l;

public:
   DLLLOCAL UserParamListLocalVarHelper(UserParamList *n_l, const QoreTypeInfo *classTypeInfo = 0) : l(n_l) {
      l->parseInitPushLocalVars(classTypeInfo);
   }
   DLLLOCAL ~UserParamListLocalVarHelper() {
      l->parseInitPopLocalVars();
   }
};

class VRMutex;

class UserFunction : public QoreReferenceCounter {
private:
   bool synchronized;
   // for "synchronized" functions
   VRMutex *gate;
   char *name;
   QoreParseTypeInfo *returnTypeInfo;

protected:
   DLLLOCAL ~UserFunction();

public:
   UserParamList *params;
   StatementBlock *statements;

   // the object owns the memory for "n_name", 0 for anonymous closures, takes ownership of parms, b, rv
   DLLLOCAL UserFunction(char *n_name, UserParamList *parms, StatementBlock *b, QoreParseTypeInfo *rv, bool synced = false);
   DLLLOCAL int setupCall(const QoreListNode *args, ReferenceHolder<QoreListNode> &argv, ExceptionSink *xsink) const;
   DLLLOCAL AbstractQoreNode *eval(const QoreListNode *args, QoreObject *self, ExceptionSink *xsink, const char *class_name = 0) const;
   DLLLOCAL AbstractQoreNode *evalConstructor(const QoreListNode *args, QoreObject *self, class BCList *bcl, class BCEAList *scbceal, const char *class_name, ExceptionSink *xsink) const;
   DLLLOCAL void evalCopy(QoreObject *old, QoreObject *self, const char *class_name, ExceptionSink *xsink) const;
   DLLLOCAL bool isSynchronized() const { 
      return synchronized; 
   }
   DLLLOCAL void deref();
   DLLLOCAL const char *getName() const {
      return name ? name : "<anonymous closure>";
   }
   DLLLOCAL void parseInit();
   DLLLOCAL void parseInitMethod(const QoreClass &parent_class, bool static_flag);
   DLLLOCAL void parseInitConstructor(const QoreClass &parent_class, BCList *bcl);
   DLLLOCAL void parseInitDestructor(const QoreClass &parent_class);
   DLLLOCAL void parseInitCopy(const QoreClass &parent_class);

   DLLLOCAL const QoreTypeInfo *getReturnTypeInfo() const {
      return static_cast<QoreTypeInfo *>(returnTypeInfo);
   }
};

#endif // _QORE_FUNCTION_H
