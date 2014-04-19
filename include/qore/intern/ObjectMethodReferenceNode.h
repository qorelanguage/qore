/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 ObjectMethodReference.h
 
 Qore Programming Language
 
 Copyright (C) 2003 - 2014 David Nichols
 
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

#ifndef _QORE_OBJECTMETHODREFERENCE_H

#define _QORE_OBJECTMETHODREFERENCE_H

class AbstractParseObjectMethodReferenceNode : public ParseNode {
protected:
   // returns a RunTimeObjectMethodReference or NULL if there's an exception
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const = 0;
   
   //! optionally evaluates the argument
   /** return value requires a deref(xsink) if needs_deref is true
       @see AbstractQoreNode::eval()
   */
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, class ExceptionSink *xsink) const = 0;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const = 0;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const = 0;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const = 0;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const = 0;

public:
   DLLLOCAL AbstractParseObjectMethodReferenceNode() : ParseNode(NT_OBJMETHREF) {
   }

   DLLLOCAL virtual ~AbstractParseObjectMethodReferenceNode() {
   }

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
      str.sprintf("object method reference (0x%08p)", this);
      return 0;
   }

   // if del is true, then the returned QoreString * should be deleted, if false, then it must not be
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const {
      del = true;
      QoreString *rv = new QoreString();
      getAsString(*rv, foff, xsink);
      return rv;
   }
   
   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const {
      return "object method reference";
   }
};

class ParseObjectMethodReferenceNode : public AbstractParseObjectMethodReferenceNode {
private:
   AbstractQoreNode *exp;
   char *method;
   const QoreClass *qc;
   const QoreMethod *m;

   DLLLOCAL virtual ~ParseObjectMethodReferenceNode();

   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return callReferenceTypeInfo;
   }

protected:
   // returns a RunTimeObjectMethodReference or NULL if there's an exception
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

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
   DLLLOCAL ParseObjectMethodReferenceNode(AbstractQoreNode *n_exp, char *n_method);
   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);
};

class ParseSelfMethodReferenceNode : public AbstractParseObjectMethodReferenceNode {
private:
   char *method;
   const QoreMethod *meth;

   DLLLOCAL ~ParseSelfMethodReferenceNode() {
      if (method)
         free(method);
   }

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return callReferenceTypeInfo;
   }

protected:
   // returns a RunTimeObjectMethodReference or NULL if there's an exception
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

   //! optionally evaluates the argument
   /** return value requires a deref(xsink) if needs_deref is true
       @see AbstractQoreNode::eval()
   */
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, class ExceptionSink *xsink) const;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

public:
   DLLLOCAL ParseSelfMethodReferenceNode(char *n_method) : method(n_method), meth(0) {
   }

   DLLLOCAL ParseSelfMethodReferenceNode(const QoreMethod *m) : method(0), meth(m) {
   }
};

class ParseScopedSelfMethodReferenceNode : public AbstractParseObjectMethodReferenceNode {
private:
   NamedScope *nscope;
   const QoreMethod *method;

   DLLLOCAL virtual ~ParseScopedSelfMethodReferenceNode();

protected:
   // returns a RunTimeObjectMethodReference or NULL if there's an exception
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

   //! optionally evaluates the argument
   /** return value requires a deref(xsink) if needs_deref is true
       @see AbstractQoreNode::eval()
   */
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, class ExceptionSink *xsink) const;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);
   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      return callReferenceTypeInfo;
   }

public:
   DLLLOCAL ParseScopedSelfMethodReferenceNode(NamedScope *n_nscope);
};

#endif
