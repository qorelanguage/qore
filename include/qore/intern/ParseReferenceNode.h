/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ParseReferenceNode.h

  Qore Programming Language

  Copyright 2003 - 2013 David Nichols

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

#ifndef _QORE_INTERN_PARSEREFERENCENODE_H
#define _QORE_INTERN_PARSEREFERENCENODE_H

#include <qore/intern/ParseNode.h>

class IntermediateParseReferenceNode;

class ParseReferenceNode : public ParseNode {
protected:
   //! lvalue expression for reference
   AbstractQoreNode* lvexp;

   //! frees all memory and destroys the object
   DLLLOCAL ~ParseReferenceNode() {
      if (lvexp)
         lvexp->deref(0);
   }

   // returns a runtime reference (ReferenceNode)
   DLLLOCAL virtual AbstractQoreNode* evalImpl(ExceptionSink* xsink) const {
      return evalToRef(xsink);
   }

   DLLLOCAL virtual AbstractQoreNode* evalImpl(bool& needs_deref, ExceptionSink* xsink) const {
      needs_deref = true;
      return evalToRef(xsink);
   }

   //! should never be called
   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink* xsink) const {
      assert(false);
      return 0;
   }

   //! should never be called
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink* xsink) const {
      assert(false);
      return 0;
   }

   //! should never be called
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink* xsink) const {
      assert(false);
      return false;
   }

   //! should never be called
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink* xsink) const {
      assert(false);
      return 0.0;
   }

   DLLLOCAL AbstractQoreNode* doPartialEval(AbstractQoreNode* n, QoreObject*& self, const void*& lvalue_id, ExceptionSink* xsink) const;

   //! initializes during parsing
   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

public:
   //! creates the ReferenceNode object with the given lvalue expression
   /** @param exp must be a parse expression for an lvalue
    */
   DLLLOCAL ParseReferenceNode(AbstractQoreNode* exp) : ParseNode(NT_PARSEREFERENCE, true, false), lvexp(exp) {
   }

   //! concatenate the verbose string representation of the value to an existing QoreString
   /** used for %n and %N printf formatting
       @param str the string representation of the type will be concatenated to this QoreString reference
       @param foff for multi-line formatting offset, -1 = no line breaks (ignored in this version of the function)
       @param xsink ignored in this version of the function
       @return this implementation of the function always returns 0 for no error raised
   */
   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
      str.sprintf("parse reference expression (%p)", this);
      return 0;
   }

   //! returns a QoreString giving the verbose string representation of the value
   /** Used for %n and %N printf formatting.  Do not call this function directly; use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead
       @param del if this is true when the function returns, then the returned QoreString pointer should be deleted, if false, then it must not be
       @param foff for multi-line formatting offset, -1 = no line breaks (ignored in this version of the function)
       @param xsink ignored in this version of the function
       @see QoreNodeAsStringHelper
   */
   DLLLOCAL virtual QoreString* getAsString(bool &del, int foff, class ExceptionSink* xsink) const {
      del = true;
      QoreString* rv = new QoreString();
      getAsString(*rv, foff, xsink);
      return rv;
   }

   //! returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const {
      return "reference to lvalue";
   }

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return referenceTypeInfo;
   }

   // returns an intermediate reference for use with the backgroun operator
   DLLLOCAL IntermediateParseReferenceNode* evalToIntermediate(ExceptionSink* xsink) const;

   // returns a runtime reference
   DLLLOCAL virtual ReferenceNode* evalToRef(ExceptionSink* xsink) const;
};

class IntermediateParseReferenceNode : public ParseReferenceNode {
protected:
   QoreObject* self;
   const void* lvalue_id;

   DLLLOCAL virtual bool derefImpl(ExceptionSink* xsink) {
      if (lvexp)
         lvexp->deref(xsink);
      lvexp = 0;
      return true;
   }

public:
   DLLLOCAL IntermediateParseReferenceNode(AbstractQoreNode* exp, QoreObject* o, const void* lvid) : ParseReferenceNode(exp), self(o), lvalue_id(lvid) {
   }

   // returns a runtime reference
   DLLLOCAL virtual ReferenceNode* evalToRef(ExceptionSink* xsink) const {
      return new ReferenceNode(lvexp->refSelf(), self, lvalue_id);
   }
};

#endif // _QORE_INTERN_PARSEREFERENCENODE_H
