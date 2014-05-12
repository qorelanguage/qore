/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
   QoreClosureNode.h

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

#ifndef _QORE_QORECLOSURENODE_H

#define _QORE_QORECLOSURENODE_H 

#include <qore/intern/QoreObjectIntern.h>

#include <map>

class QoreClosureBase : public ResolvedCallReferenceNode {
protected:
   const QoreClosureParseNode* closure;
   bool pgm_ref;

public:
   //! constructor is not exported outside the library
   DLLLOCAL QoreClosureBase(const QoreClosureParseNode *n_closure) : ResolvedCallReferenceNode(false, NT_RUNTIME_CLOSURE), closure(n_closure), pgm_ref(true) {
      closure->ref();
   }

   DLLLOCAL ~QoreClosureBase() {
      const_cast<QoreClosureParseNode*>(closure)->deref();
   }
      
   DLLLOCAL static const char *getStaticTypeName() {
      return "closure";
   }      

   DLLLOCAL virtual QoreFunction *getFunction() {
      return closure->getFunction();
   }

   DLLLOCAL virtual void derefProgramCycle(QoreProgram *cpgm) = 0;
};

class QoreClosureNode : public QoreClosureBase {
private:
   mutable ThreadSafeLocalVarRuntimeEnvironment closure_env;
   QoreProgram *pgm;

   DLLLOCAL QoreClosureNode(const QoreClosureNode&); // not implemented
   DLLLOCAL QoreClosureNode& operator=(const QoreClosureNode&); // not implemented

protected:
   DLLLOCAL virtual bool derefImpl(ExceptionSink* xsink);
      
public:
   DLLLOCAL QoreClosureNode(const QoreClosureParseNode *n_closure) : QoreClosureBase(n_closure), closure_env(n_closure->getVList()), pgm(::getProgram()) {
      pgm->depRef();
   }

   DLLLOCAL virtual ~QoreClosureNode() {
   }

   DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink* xsink) const;

   DLLLOCAL virtual QoreProgram *getProgram() const {
      return pgm;
   }

   //! returns false unless perl-boolean-evaluation is enabled, in which case it returns true
   /** @return false unless perl-boolean-evaluation is enabled, in which case it returns true
    */
   DLLEXPORT virtual bool getAsBoolImpl() const;

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink* xsink) const {
      str.sprintf("function closure (%slambda, 0x%08p)", closure->isLambda() ? "" : "non-", this);
      return 0;
   }

   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink* xsink) const {
      del = true;
      QoreString *rv = new QoreString();
      getAsString(*rv, foff, xsink);
      return rv;
   }

   DLLLOCAL virtual const char *getTypeName() const {
      return getStaticTypeName();
   }

   DLLLOCAL bool isLambda() const { return closure->isLambda(); }

   DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink* xsink) const {
      return QoreClosureNode::is_equal_hard(v, xsink);
   }

   DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink* xsink) const {
      return v == this;
   }

   DLLLOCAL virtual void derefProgramCycle(QoreProgram *cpgm) {
      if (pgm_ref) {
         assert(cpgm == pgm);
         pgm->depDeref(0);
         pgm_ref = false;
      }
   }
};

class QoreObjectClosureNode : public QoreClosureBase {
private:
   mutable ThreadSafeLocalVarRuntimeEnvironment closure_env;
   QoreObject *obj;

   DLLLOCAL QoreObjectClosureNode(const QoreObjectClosureNode&); // not implemented
   DLLLOCAL QoreObjectClosureNode& operator=(const QoreObjectClosureNode&); // not implemented

protected:
   DLLLOCAL virtual bool derefImpl(ExceptionSink* xsink);

public:
   DLLLOCAL QoreObjectClosureNode(QoreObject *n_obj, const QoreClosureParseNode *n_closure);
   DLLLOCAL ~QoreObjectClosureNode();
   DLLLOCAL virtual AbstractQoreNode *exec(const QoreListNode *args, ExceptionSink* xsink) const;

   DLLLOCAL virtual QoreProgram *getProgram() const {
      return obj->getProgram();
   }

   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink* xsink) const {
      str.sprintf("function closure (%slambda, in object of class '%s', 0x%08p)", closure->isLambda() ? "" : "non-", obj->getClassName(), this);
      return 0;
   }

   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink* xsink) const {
      del = true;
      QoreString *rv = new QoreString();
      getAsString(*rv, foff, xsink);
      return rv;
   }

   DLLLOCAL virtual const char *getTypeName() const {
      return getStaticTypeName();
   }

   DLLLOCAL bool isLambda() const { return closure->isLambda(); }

   DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink* xsink) const {
      return QoreObjectClosureNode::is_equal_hard(v, xsink);
   }

   DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink* xsink) const {
      return v == this;
   }

   DLLLOCAL virtual void derefProgramCycle(QoreProgram *cpgm) {
      if (pgm_ref) {
         qore_object_private::derefProgramCycle(obj, cpgm);
         pgm_ref = false;
      }
   }
};

#endif
