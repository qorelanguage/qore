/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 ObjectMethodReference.h
 
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

#ifndef _QORE_OBJECTMETHODREFERENCE_H

#define _QORE_OBJECTMETHODREFERENCE_H

#include <string>

class AbstractParseObjectMethodReferenceNode : public ParseNode {
protected:
   // returns a RunTimeObjectMethodReference or NULL if there's an exception
   DLLLOCAL virtual AbstractQoreNode* evalImpl(ExceptionSink* xsink) const = 0;
   
   //! optionally evaluates the argument
   /** return value requires a deref(xsink) if needs_deref is true
       @see AbstractQoreNode::eval()
   */
   DLLLOCAL virtual AbstractQoreNode* evalImpl(bool& needs_deref, ExceptionSink* xsink) const = 0;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink* xsink) const = 0;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink* xsink) const = 0;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink* xsink) const = 0;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink* xsink) const = 0;

public:
   DLLLOCAL AbstractParseObjectMethodReferenceNode() : ParseNode(NT_OBJMETHREF) {
   }

   DLLLOCAL virtual ~AbstractParseObjectMethodReferenceNode() {
   }

   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
      str.sprintf("object method reference (0x%08p)", this);
      return 0;
   }

   // if del is true, then the returned QoreString*  should be deleted, if false, then it must not be
   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const {
      del = true;
      QoreString* rv = new QoreString;
      getAsString(*rv, foff, xsink);
      return rv;
   }
   
   // returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const {
      return "object method reference";
   }
};

class ParseObjectMethodReferenceNode : public AbstractParseObjectMethodReferenceNode {
private:
   AbstractQoreNode* exp;
   std::string method;
   const QoreClass *qc;
   mutable const QoreMethod* m;
   mutable QoreThreadLock lck;
   
   DLLLOCAL virtual ~ParseObjectMethodReferenceNode();

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return callReferenceTypeInfo;
   }

protected:
   // returns a RunTimeObjectMethodReference or NULL if there's an exception
   DLLLOCAL virtual AbstractQoreNode* evalImpl(ExceptionSink* xsink) const;

   //! optionally evaluates the argument
   /** return value requires a deref(xsink) if needs_deref is true
       @see AbstractQoreNode::eval()
   */
   DLLLOCAL virtual AbstractQoreNode* evalImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink* xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink* xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink* xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink* xsink) const;
   
public:
   DLLLOCAL ParseObjectMethodReferenceNode(AbstractQoreNode* n_exp, char* n_method);
   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);
};

class ParseSelfMethodReferenceNode : public AbstractParseObjectMethodReferenceNode {
private:
   std::string method;
   const QoreMethod* meth;

   DLLLOCAL ~ParseSelfMethodReferenceNode() {
   }

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return callReferenceTypeInfo;
   }

protected:
   // returns a RunTimeObjectMethodReference or NULL if there's an exception
   DLLLOCAL virtual AbstractQoreNode* evalImpl(ExceptionSink* xsink) const;

   //! optionally evaluates the argument
   /** return value requires a deref(xsink) if needs_deref is true
       @see AbstractQoreNode::eval()
   */
   DLLLOCAL virtual AbstractQoreNode* evalImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink* xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink* xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink* xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink* xsink) const;

public:
   DLLLOCAL ParseSelfMethodReferenceNode(char* n_method) : method(n_method), meth(0) {
   }

   DLLLOCAL ParseSelfMethodReferenceNode(const QoreMethod* m) : meth(m) {
   }
};

class ParseScopedSelfMethodReferenceNode : public AbstractParseObjectMethodReferenceNode {
private:
   NamedScope *nscope;
   const QoreMethod* method;

   DLLLOCAL virtual ~ParseScopedSelfMethodReferenceNode();

protected:
   // returns a RunTimeObjectMethodReference or NULL if there's an exception
   DLLLOCAL virtual AbstractQoreNode* evalImpl(ExceptionSink* xsink) const;

   //! optionally evaluates the argument
   /** return value requires a deref(xsink) if needs_deref is true
       @see AbstractQoreNode::eval()
   */
   DLLLOCAL virtual AbstractQoreNode* evalImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink* xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink* xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink* xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink* xsink) const;

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);
   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return callReferenceTypeInfo;
   }

public:
   DLLLOCAL ParseScopedSelfMethodReferenceNode(NamedScope *n_nscope);
};

#endif
