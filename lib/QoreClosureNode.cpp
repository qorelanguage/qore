/*
  QoreClosureNode.cpp
   
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

#include <qore/Qore.h>

ThreadSafeLocalVarRuntimeEnvironment::ThreadSafeLocalVarRuntimeEnvironment(const lvar_set_t* vlist) {
   for (lvar_set_t::const_iterator i = vlist->begin(), e = vlist->end(); i != e; ++i) {
      ClosureVarValue* cvar = thread_find_closure_var((*i)->getName());
      //printd(5, "ThreadSafeLocalVarRuntimeEnvironment::ThreadSafeLocalVarRuntimeEnvironment() this: %p '%s' i: %p %p\n", this, (*i)->getName(), *i, cvar);
      cmap[*i] = cvar;
      cvar->ref();
   }
}

ThreadSafeLocalVarRuntimeEnvironment::~ThreadSafeLocalVarRuntimeEnvironment() {
   //printd(5, "ThreadSafeLocalVarRuntimeEnvironment::~ThreadSafeLocalVarRuntimeEnvironment() this: %p\n", this);
   assert(cmap.empty());
}

ClosureVarValue* ThreadSafeLocalVarRuntimeEnvironment::find(const LocalVar* id) {
   //printd(5, "ThreadSafeLocalVarRuntimeEnvironment::find(%p '%s') this: %p\n", id, id->getName(), this);
   cvar_map_t::iterator i = cmap.find(id);
   assert(i != cmap.end());
   return i->second;
}

void ThreadSafeLocalVarRuntimeEnvironment::del(ExceptionSink* xsink) {
   for (cvar_map_t::iterator i = cmap.begin(), e = cmap.end(); i != e; ++i)
      i->second->deref(xsink);

#ifdef DEBUG
   cmap.clear();
#endif
}

bool QoreClosureNode::derefImpl(ExceptionSink* xsink) {
   closure_env.del(xsink);
   if (pgm_ref)
      pgm->depDeref(xsink);
   return true;
}

AbstractQoreNode* QoreClosureNode::exec(const QoreListNode* args, ExceptionSink* xsink) const {
   return closure->exec(closure_env, args, 0, xsink);
}

bool QoreClosureNode::getAsBoolImpl() const {
   // check if we should do perl-style boolean evaluation
   QoreProgram* pgm = getProgram();
   if (pgm && runtime_check_parse_option(PO_STRICT_BOOLEAN_EVAL))
      return false;
   return true;
}

QoreObjectClosureNode::QoreObjectClosureNode(QoreObject* n_obj, const QoreClosureParseNode* n_closure) : QoreClosureBase(n_closure), closure_env(n_closure->getVList()), obj(n_obj) {
   obj->tRef();
}

QoreObjectClosureNode::~QoreObjectClosureNode() {
}

bool QoreObjectClosureNode::derefImpl(ExceptionSink* xsink) {
   closure_env.del(xsink);
   obj->tDeref();
#ifdef DEBUG
   obj = 0;
#endif
   return true;
}

AbstractQoreNode* QoreObjectClosureNode::exec(const QoreListNode* args, ExceptionSink* xsink) const {
   return closure->exec(closure_env, args, obj, xsink);
}
