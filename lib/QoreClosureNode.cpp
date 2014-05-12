/*
   QoreClosureNode.cpp

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

#include <qore/Qore.h>

ThreadSafeLocalVarRuntimeEnvironment::ThreadSafeLocalVarRuntimeEnvironment(const lvar_set_t* vlist) {
   for (lvar_set_t::const_iterator i = vlist->begin(), e = vlist->end(); i != e; ++i) {
      ClosureVarValue* cvar = thread_find_closure_var((*i)->getName());
      //printd(5, "ThreadSafeLocalVarRuntimeEnvironment::ThreadSafeLocalVarRuntimeEnvironment() this: %p '%s' %p\n", this, (*i)->getName(), cvar);
      cmap[*i] = cvar;
      cvar->ref();
   }
}

ThreadSafeLocalVarRuntimeEnvironment::~ThreadSafeLocalVarRuntimeEnvironment() {
   //printd(5, "ThreadSafeLocalVarRuntimeEnvironment::~ThreadSafeLocalVarRuntimeEnvironment() this: %p\n", this);
   assert(cmap.empty());
}

ClosureVarValue *ThreadSafeLocalVarRuntimeEnvironment::find(const LocalVar *id) {
   cvar_map_t::iterator i = cmap.find(id);
   assert(i != cmap.end());
   return i->second;
}

void ThreadSafeLocalVarRuntimeEnvironment::del(ExceptionSink *xsink) {
   for (cvar_map_t::iterator i = cmap.begin(), e = cmap.end(); i != e; ++i)
      i->second->deref(xsink);

#ifdef DEBUG
   cmap.clear();
#endif
}

bool QoreClosureNode::derefImpl(ExceptionSink *xsink) {
   closure_env.del(xsink);
   if (pgm_ref)
      pgm->depDeref(xsink);
   return true;
}

AbstractQoreNode *QoreClosureNode::exec(const QoreListNode *args, ExceptionSink *xsink) const {
   ThreadSafeLocalVarRuntimeEnvironmentHelper ch(&closure_env);
   return closure->exec(args, 0, xsink);
}

bool QoreClosureNode::getAsBoolImpl() const {
   // check if we should do perl-style boolean evaluation
   QoreProgram* pgm = getProgram();
   if (pgm && runtime_check_parse_option(PO_STRICT_BOOLEAN_EVAL))
      return false;
   return true;
}

QoreObjectClosureNode::QoreObjectClosureNode(QoreObject *n_obj, const QoreClosureParseNode *n_closure) : QoreClosureBase(n_closure), closure_env(n_closure->getVList()), obj(n_obj) {
   obj->tRef();
}

QoreObjectClosureNode::~QoreObjectClosureNode() {
}

bool QoreObjectClosureNode::derefImpl(ExceptionSink *xsink) {
   closure_env.del(xsink);
   obj->tDeref();
   return true;
}

AbstractQoreNode *QoreObjectClosureNode::exec(const QoreListNode *args, ExceptionSink *xsink) const {
   ThreadSafeLocalVarRuntimeEnvironmentHelper ch(&closure_env);
   return closure->exec(args, obj, xsink);
}
