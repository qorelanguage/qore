/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreClosureNode.h

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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

#ifndef _QORE_QORECLOSUREPARSENODE_H

#define _QORE_QORECLOSUREPARSENODE_H

#include "qore/intern/ParseNode.h"

#include <vector>

class LocalVar;
class ThreadSafeLocalVarRuntimeEnvironment;

class ClosureParseEnvironment {
private:
   LVarSet* vlist;
   VNode* high_water_mark;
   ClosureParseEnvironment* prev;

public:
   DLLLOCAL ClosureParseEnvironment(LVarSet* n_vlist) : vlist(n_vlist), high_water_mark(getVStack()) {
      prev = thread_get_closure_parse_env();
      thread_set_closure_parse_env(this);
   }

   DLLLOCAL ~ClosureParseEnvironment() {
      thread_set_closure_parse_env(prev);
   }

   DLLLOCAL VNode* getHighWaterMark() {
      return high_water_mark;
   }

   DLLLOCAL void add(LocalVar* var) {
      // add var to the set
      vlist->add(var);
   }
};

class QoreClosureNode;
class QoreObjectClosureNode;

class QoreClosureParseNode : public ParseNode {
   friend class QoreClosureParseNodeBackground;
private:
   UserClosureFunction* uf;
   bool lambda, in_method;

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);
   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return runTimeClosureTypeInfo;
   }

   DLLLOCAL QoreClosureNode* evalClosure() const;
   DLLLOCAL QoreObjectClosureNode* evalObjectClosure() const;

public:
   DLLLOCAL QoreClosureParseNode(UserClosureFunction* n_uf, bool n_lambda = false);

   DLLLOCAL ~QoreClosureParseNode() {
      delete uf;
   }

   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const;
   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const;
   DLLLOCAL virtual const char* getTypeName() const;
   DLLLOCAL static const char* getStaticTypeName() {
      return "function closure";
   }

   DLLLOCAL bool isLambda() const { return lambda; }

   DLLLOCAL QoreValue exec(const QoreClosureBase& closure_base, QoreProgram* pgm, const QoreListNode* args, QoreObject* self, const qore_class_private* class_ctx, ExceptionSink* xsink) const;

   DLLLOCAL QoreClosureBase* evalBackground(ExceptionSink* xsink) const;

   DLLLOCAL const LVarSet* getVList() const {
      return uf->getVList();
   }

   // returns true if at least one variable in the set of closure-bound local variables could contain an object or a closure (also through a container)
   DLLLOCAL bool needsScan() const {
      return uf->needsScan();
   }

   DLLLOCAL UserClosureFunction* getFunction() const {
      return uf;
   }

   DLLLOCAL QoreClosureParseNode* refSelf() const {
      ref();
      return const_cast<QoreClosureParseNode*>(this);
   }
};

class QoreClosureParseNodeBackground : public ParseNode {
protected:
   QoreClosureParseNode* closure;
   mutable cvv_vec_t* cvec;

   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo) {
      assert(false);
      return this;
   }

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      return runTimeClosureTypeInfo;
   }

public:
   DLLLOCAL QoreClosureParseNodeBackground(QoreClosureParseNode* c) : ParseNode(NT_CLOSURE), closure(c), cvec(thread_get_all_closure_vars()) {
   }
   DLLLOCAL ~QoreClosureParseNodeBackground() {
      assert(!cvec);
      assert(!closure);
   }
   DLLLOCAL virtual void deref(ExceptionSink* xsink) {
      if (ROdereference()) {
         if (cvec) {
            for (cvv_vec_t::iterator i = cvec->begin(), e = cvec->end(); i != e; ++i)
               (*i)->deref(xsink);
            delete cvec;
         }
         closure->deref();
#ifdef DEBUG
         cvec = 0;
         closure = 0;
#endif
      }
   }
   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
      return closure->getAsString(str, foff, xsink);
   }
   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const {
      return closure->getAsString(del, foff, xsink);
   }
   DLLLOCAL virtual const char* getTypeName() const {
      return closure->getTypeName();
   }
};

#endif
