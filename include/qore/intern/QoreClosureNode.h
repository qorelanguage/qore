/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreClosureNode.h

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

#ifndef _QORE_QORECLOSURENODE_H

#define _QORE_QORECLOSURENODE_H 

#include <qore/intern/QoreObjectIntern.h>

#include <map>

class QoreClosureBase : public ResolvedCallReferenceNode {
protected:
   const QoreClosureParseNode* closure;
   mutable ThreadSafeLocalVarRuntimeEnvironment closure_env;
   bool pgm_ref;

   DLLLOCAL void del(ExceptionSink* xsink) {
      closure_env.del(xsink);
   }

public:
   //! constructor is not exported outside the library
   DLLLOCAL QoreClosureBase(const QoreClosureParseNode* n_closure) : ResolvedCallReferenceNode(false, NT_RUNTIME_CLOSURE), closure(n_closure), closure_env(n_closure->getVList()), pgm_ref(true) {
      //printd(5, "QoreClosureBase::QoreClosureBase() this: %p closure: %p\n", this, closure);
      closure->ref();
   }

   DLLLOCAL ~QoreClosureBase() {
      //printd(5, "QoreClosureBase::~QoreClosureBase() this: %p closure: %p\n", this, closure);
      const_cast<QoreClosureParseNode*>(closure)->deref();
   }

   DLLLOCAL ClosureVarValue* find(const LocalVar* id) const {
      return closure_env.find(id);
   }

   DLLLOCAL bool hasVar(ClosureVarValue* cvv) const {
      return closure_env.hasVar(cvv);
   }

   DLLLOCAL static const char* getStaticTypeName() {
      return "closure";
   }      

   DLLLOCAL virtual QoreFunction* getFunction() {
      return closure->getFunction();
   }

   DLLLOCAL virtual void derefProgramCycle(QoreProgram* cpgm) = 0;
};

class QoreClosureNode : public QoreClosureBase {
private:
   QoreProgram* pgm;

   DLLLOCAL QoreClosureNode(const QoreClosureNode&); // not implemented
   DLLLOCAL QoreClosureNode& operator=(const QoreClosureNode&); // not implemented

protected:
   DLLLOCAL virtual bool derefImpl(ExceptionSink* xsink);
      
public:
   DLLLOCAL QoreClosureNode(const QoreClosureParseNode* n_closure) : QoreClosureBase(n_closure), pgm(::getProgram()) {
      //pgm->depRef();
      pgm->ref();
   }

   DLLLOCAL virtual ~QoreClosureNode() {
   }

   DLLLOCAL virtual AbstractQoreNode* exec(const QoreListNode* args, ExceptionSink* xsink) const;

   DLLLOCAL virtual QoreProgram* getProgram() const {
      return pgm;
   }

   //! returns false unless perl-boolean-evaluation is enabled, in which case it returns true
   /** @return false unless perl-boolean-evaluation is enabled, in which case it returns true
   */
   DLLEXPORT virtual bool getAsBoolImpl() const;

   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
      str.sprintf("function closure (%slambda, %p)", closure->isLambda() ? "" : "non-", this);
      return 0;
   }

   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const {
      del = true;
      QoreString* rv = new QoreString;
      getAsString(*rv, foff, xsink);
      return rv;
   }

   DLLLOCAL virtual const char* getTypeName() const {
      return getStaticTypeName();
   }

   DLLLOCAL bool isLambda() const { return closure->isLambda(); }

   DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode* v, ExceptionSink* xsink) const {
      return QoreClosureNode::is_equal_hard(v, xsink);
   }

   DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
      return v == this;
   }

   DLLLOCAL virtual void derefProgramCycle(QoreProgram* cpgm) {
      //printd(5, "QoreClosureNode::derefProgramCycle(cpgm: %p) this: %p pgm_ref: %d pgm: %p\n", cpgm, this, pgm_ref, pgm);
      if (pgm_ref) {
         assert(cpgm == pgm);
         pgm->deref(0);
         pgm_ref = false;
      }
   }
};

class QoreObjectClosureNode : public QoreClosureBase {
private:
   QoreObject* obj;

   DLLLOCAL QoreObjectClosureNode(const QoreObjectClosureNode&); // not implemented
   DLLLOCAL QoreObjectClosureNode& operator=(const QoreObjectClosureNode&); // not implemented

protected:
   DLLLOCAL virtual bool derefImpl(ExceptionSink* xsink);

public:
   DLLLOCAL QoreObjectClosureNode(QoreObject* n_obj, const QoreClosureParseNode* n_closure);
   DLLLOCAL ~QoreObjectClosureNode();
   DLLLOCAL virtual AbstractQoreNode* exec(const QoreListNode* args, ExceptionSink* xsink) const;

   DLLLOCAL virtual QoreProgram* getProgram() const {
      return obj->getProgram();
   }

   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
      str.sprintf("function closure (%slambda, in object of class '%s', %p)", closure->isLambda() ? "" : "non-", obj->getClassName(), this);
      return 0;
   }

   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const {
      del = true;
      QoreString* rv = new QoreString();
      getAsString(*rv, foff, xsink);
      return rv;
   }

   DLLLOCAL virtual const char* getTypeName() const {
      return getStaticTypeName();
   }

   DLLLOCAL bool isLambda() const { return closure->isLambda(); }

   DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode* v, ExceptionSink* xsink) const {
      return QoreObjectClosureNode::is_equal_hard(v, xsink);
   }

   DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
      return v == this;
   }

   DLLLOCAL virtual void derefProgramCycle(QoreProgram* cpgm) {
      //printd(5, "QoreObjectClosureNode::derefProgramCycle(cpgm: %p) this: %p pgm_ref: %d obj: %p\n", cpgm, this, pgm_ref, obj);
      if (pgm_ref) {
         qore_object_private::derefProgramCycle(obj, cpgm);
         pgm_ref = false;
      }
   }
};

#endif
