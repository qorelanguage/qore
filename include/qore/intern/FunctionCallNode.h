/*
  FunctionCallNode.h

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

#ifndef _QORE_FUNCTIONCALLNODE_H

#define _QORE_FUNCTIONCALLNODE_H

#include <qore/Qore.h>

class ImportedFunctionCall {
   public:
      QoreProgram *pgm;
      const UserFunction *func;

      DLLLOCAL ImportedFunctionCall(QoreProgram *p, const UserFunction *f) { pgm = p; func = f; }
      DLLLOCAL class AbstractQoreNode *eval(const QoreListNode *args, ExceptionSink *xsink) const;
};

class SelfFunctionCall {
  public:
      char *name;
      class NamedScope *ns;
      const class QoreMethod *func;

      DLLLOCAL SelfFunctionCall(char *n);
      DLLLOCAL SelfFunctionCall(NamedScope *n);
      DLLLOCAL SelfFunctionCall(const QoreMethod *f);
      DLLLOCAL ~SelfFunctionCall();
      DLLLOCAL class AbstractQoreNode *eval(const QoreListNode *args, ExceptionSink *xsink) const;
      DLLLOCAL void resolve();
      DLLLOCAL char *takeName();
      DLLLOCAL class NamedScope *takeNScope();
};

// FIXME: split this into different function call subclasses
class FunctionCallNode : public ParseNode
{
   protected:
      // eval(): return value requires a deref(xsink)
      DLLLOCAL virtual class AbstractQoreNode *evalImpl(ExceptionSink *) const;

      //! optionally evaluates the argument
      /** return value requires a deref(xsink) if needs_deref is true
	  @see AbstractQoreNode::eval()
      */
      DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

      DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

   public:
      union uFCall {
	    const UserFunction *ufunc;
	    const BuiltinFunction *bfunc;
	    class SelfFunctionCall *sfunc;
	    class ImportedFunctionCall *ifunc;
	    char *c_str;
      } f;
      QoreListNode *args;
      int ftype;

      DLLLOCAL FunctionCallNode(const UserFunction *u, QoreListNode *a);
      DLLLOCAL FunctionCallNode(const BuiltinFunction *b, QoreListNode *a);

      // "self" in-object function call constructors
      DLLLOCAL FunctionCallNode(QoreListNode *a, char *name);
      DLLLOCAL FunctionCallNode(QoreListNode *a, class NamedScope *n);
      DLLLOCAL FunctionCallNode(const QoreMethod *func, QoreListNode *a);

      // normal function call constructor
      DLLLOCAL FunctionCallNode(char *name, QoreListNode *a);
      // method call constructor
      DLLLOCAL FunctionCallNode(char *n_c_str);
      
      DLLLOCAL FunctionCallNode(class QoreProgram *p, const UserFunction *u, QoreListNode *a);

      DLLLOCAL virtual ~FunctionCallNode();

      // get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
      // the ExceptionSink is only needed for QoreObject where a method may be executed
      // use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using these functions directly
      // returns -1 for exception raised, 0 = OK
      DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;

      // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;

      // returns the type name as a c string
      DLLLOCAL virtual const char *getTypeName() const;

      // to transform an "unresolved" function to a "method" type
      DLLLOCAL void parseMakeMethod();
      DLLLOCAL class AbstractQoreNode *parseMakeNewObject();
      DLLLOCAL int existsUserParam(int i) const;
      DLLLOCAL int getFunctionType() const;
      DLLLOCAL const char *getName() const;
      DLLLOCAL char *takeName();
};

#endif
