/*
  FunctionCallNode.h

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

#ifndef _QORE_FUNCTIONCALLNODE_H

#define _QORE_FUNCTIONCALLNODE_H

#include <qore/Qore.h>

class ImportedFunctionCall {
   public:
      class QoreProgram *pgm;
      class UserFunction *func;

      DLLLOCAL ImportedFunctionCall(class QoreProgram *p, class UserFunction *f) { pgm = p; func = f; }
      DLLLOCAL class QoreNode *eval(const class QoreListNode *args, class ExceptionSink *xsink) const;
};

class SelfFunctionCall {
  public:
      char *name;
      class NamedScope *ns;
      const class QoreMethod *func;

      DLLLOCAL SelfFunctionCall(char *n);
      DLLLOCAL SelfFunctionCall(class NamedScope *n);
      DLLLOCAL SelfFunctionCall(const QoreMethod *f);
      DLLLOCAL ~SelfFunctionCall();
      DLLLOCAL class QoreNode *eval(const class QoreListNode *args, class ExceptionSink *xsink) const;
      DLLLOCAL void resolve();
      DLLLOCAL char *takeName();
      DLLLOCAL class NamedScope *takeNScope();
};

// FIXME: split this into different function call subclasses
class FunctionCallNode : public ParseNode
{
   protected:
      DLLLOCAL virtual ~FunctionCallNode();

   public:
      union uFCall {
	    class UserFunction *ufunc;
	    class BuiltinFunction *bfunc;
	    class SelfFunctionCall *sfunc;
	    class ImportedFunctionCall *ifunc;
	    char *c_str;
      } f;
      QoreListNode *args;
      int ftype;

      DLLLOCAL FunctionCallNode(class UserFunction *u, QoreListNode *a);
      DLLLOCAL FunctionCallNode(class BuiltinFunction *b, QoreListNode *a);

      // "self" in-object function call constructors
      DLLLOCAL FunctionCallNode(QoreListNode *a, char *name);
      DLLLOCAL FunctionCallNode(QoreListNode *a, class NamedScope *n);
      DLLLOCAL FunctionCallNode(const class QoreMethod *func, QoreListNode *a);

      // normal function call constructor
      DLLLOCAL FunctionCallNode(char *name, QoreListNode *a);
      // method call constructor
      DLLLOCAL FunctionCallNode(char *n_c_str);
      
      DLLLOCAL FunctionCallNode(class QoreProgram *p, class UserFunction *u, QoreListNode *a);

      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      // the ExceptionSink is only needed for QoreObject where a method may be executed
      // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
      // returns -1 for exception raised, 0 = OK
      DLLLOCAL virtual int getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const;
      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, class ExceptionSink *xsink) const;

      // returns the data type
      DLLLOCAL virtual const QoreType *getType() const;
      // returns the type name as a c string
      DLLLOCAL virtual const char *getTypeName() const;
      // eval(): return value requires a deref(xsink)
      DLLLOCAL virtual class QoreNode *eval(class ExceptionSink *) const;

      // to transform an "unresolved" function to a "method" type
      DLLLOCAL void parseMakeMethod();
      DLLLOCAL class QoreNode *parseMakeNewObject();
      DLLLOCAL int existsUserParam(int i) const;
      DLLLOCAL int getFunctionType() const;
      DLLLOCAL const char *getName() const;
      DLLLOCAL char *takeName();
};

#endif
