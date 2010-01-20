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

class Paramlist {
   protected:
      DLLLOCAL void assignParam(int i, VarRefNode *v);

      DLLLOCAL void setSingleParamIntern(VarRefNode *v) {
	 num_params = 1;
	 names = new char *[1];
	 typeList = new QoreParseTypeInfo *[1];
	 assignParam(0, v);
      }

   public:
      unsigned num_params;
      char **names;
      QoreParseTypeInfo **typeList;
      LocalVar **lv;
      LocalVar *argvid;
      LocalVar *selfid;
      bool resolved;

      DLLLOCAL Paramlist(AbstractQoreNode *params);
      DLLLOCAL ~Paramlist();

/*
      // takes over QoreParseTypeInfo structure
      DLLLOCAL void setSingleParam(VarRefNode *v) {
	 assert(!num_params);
	 setSingleParamIntern(v);
      }
*/

      DLLLOCAL void resolve() {
	 if (resolved)
	    return;

	 resolved = true;
	 if (!num_params)
	    return;

	 for (unsigned i = 0; i < num_params; ++i)
	    if (typeList[i])
	       typeList[i]->resolve();			 
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
      Paramlist *params;
      StatementBlock *statements;

      // the object owns the memory for "n_name", 0 for anonymous closures, takes ownership of parms, b, rv
      DLLLOCAL UserFunction(char *n_name, Paramlist *parms, StatementBlock *b, QoreParseTypeInfo *rv, bool synced = false);
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
