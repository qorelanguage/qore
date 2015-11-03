/*
   QoreClosureNode.h

   Qore Programming Language

   Copyright 2003 - 2009 David Nichols

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

#ifndef _QORE_QORECLOSUREPARSENODE_H

#define _QORE_QORECLOSUREPARSENODE_H 

#include <qore/intern/ParseNode.h>

#include <vector>

class LocalVar;

class ClosureParseEnvironment {
   private:
      lvar_set_t *vlist;
      VNode *high_water_mark;
      ClosureParseEnvironment *prev;

   public:
      DLLLOCAL ClosureParseEnvironment(lvar_set_t *n_vlist) : vlist(n_vlist), high_water_mark(getVStack()) {
	 prev = thread_get_closure_parse_env();
	 thread_set_closure_parse_env(this);
      }

      DLLLOCAL ~ClosureParseEnvironment() {
	 thread_set_closure_parse_env(prev);
      }

      DLLLOCAL VNode *getHighWaterMark() {
	 return high_water_mark;
      }

      DLLLOCAL void add(LocalVar *var) {
	 // insert var into the set
	 vlist->insert(var);
      }
};

class QoreClosureNode;
class QoreObjectClosureNode;

class QoreClosureParseNode : public ParseNode {
   private:
      UserFunction *uf;
      lvar_set_t vlist;       // closure local variable environment
      bool lambda, in_method;

      DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

      DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
      DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

      DLLLOCAL QoreClosureNode *evalClosure() const;
      DLLLOCAL QoreObjectClosureNode *evalObjectClosure() const;

   public:
      DLLLOCAL QoreClosureParseNode(UserFunction *n_uf, bool n_lambda = false);

      DLLLOCAL ~QoreClosureParseNode() {
	 uf->deref();
      }

      DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;
      DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;
      DLLLOCAL virtual const char *getTypeName() const;
      DLLLOCAL static const char *getStaticTypeName() {
         return "function closure";
      }

      DLLLOCAL bool isLambda() const { return lambda; }

      DLLLOCAL AbstractQoreNode *exec(const QoreListNode *args, QoreObject *self, ExceptionSink *xsink) const;

      DLLLOCAL const lvar_set_t *getVList() const {
	 return &vlist;
      }

      DLLLOCAL AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids);
};

#endif
