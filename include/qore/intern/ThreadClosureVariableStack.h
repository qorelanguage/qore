/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ThreadClosureVariableStack.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_INTERN_THREADCLOSUREVARIABLESTACK_H
#define _QORE_INTERN_THREADCLOSUREVARIABLESTACK_H

class ThreadClosureVariableStack : public ThreadLocalData<ClosureVarValue*> {
private:
   DLLLOCAL void instantiateIntern(ClosureVarValue* cvar) {
      //printd(5, "ThreadClosureVariableStack::instantiateIntern(%p = '%s') this: %p pgm: %p\n", cvar->id, cvar->id, this, getProgram());

      if (curr->pos == QORE_THREAD_STACK_BLOCK) {
         if (curr->next)
            curr = curr->next;
         else {
            curr->next = new Block(curr);
            //printf("this: %p: add curr: %p, curr->next: %p\n", this, curr, curr->next);
            curr = curr->next;
         }
      }
      curr->var[curr->pos++] = cvar;
   }

public:
   // marks all variables as finalized on the stack
   DLLLOCAL void finalize(arg_vec_t*& cl) {
      ThreadClosureVariableStack::iterator i(curr);
      while (i.next()) {
         AbstractQoreNode* n = i.get()->finalize();
         if (n && n->isReferenceCounted()) {
            if (!cl)
               cl = new arg_vec_t;
            cl->push_back(n);
         }
      }
   }

   // deletes everything on the stack
   DLLLOCAL void del(ExceptionSink* xsink) {
      while (curr->prev || curr->pos)
         uninstantiate(xsink);
   }

   DLLLOCAL ClosureVarValue* instantiate(const char* id, const QoreTypeInfo* typeInfo, QoreValue& nval) {
      ClosureVarValue* cvar = new ClosureVarValue(id, typeInfo, nval);
      instantiateIntern(cvar);
      return cvar;
   }

   DLLLOCAL void instantiate(ClosureVarValue* cvar) {
      instantiateIntern(cvar);
   }

   DLLLOCAL void uninstantiateIntern() {
#if 0
      if (!curr->pos)
         printd(5, "ThreadClosureVariableStack::uninstantiate() this: %p pos: %d %p %s\n", this, curr->prev->pos - 1, curr->prev->var[curr->prev->pos - 1]->id, curr->prev->var[curr->prev->pos - 1]->id);
      else
         printd(5, "ThreadClosureVariableStack::uninstantiate() this: %p pos: %d %p %s\n", this, curr->pos - 1, curr->var[curr->pos - 1]->id, curr->var[curr->pos - 1]->id);
#endif
      if (!curr->pos) {
         if (curr->next) {
            //printf("this %p: del curr: %p, curr->next: %p\n", this, curr, curr->next);
            delete curr->next;
            curr->next = 0;
         }
         curr = curr->prev;
         assert(curr);
      }
      --curr->pos;
   }

   DLLLOCAL void uninstantiate(ExceptionSink* xsink) {
      uninstantiateIntern();
      assert(curr->var[curr->pos]);
      curr->var[curr->pos]->deref(xsink);
   }

   DLLLOCAL ClosureVarValue* find(const char* id) {
      printd(5, "ThreadClosureVariableStack::find() this: %p id: %p\n", this, id);
      Block* w = curr;
      while (true) {
         int p = w->pos;
         while (p) {
            --p;
            ClosureVarValue* rv = w->var[p];
            printd(5, "ThreadClosureVariableStack::find(%p '%s') this: %p checking %p '%s' skip: %d\n", id, id, this, rv ? rv->id : nullptr, rv ? rv->id : "n/a", rv ? rv->skip : false);
            if (rv && rv->id == id && !rv->skip) {
               printd(5, "ThreadClosureVariableStack::find(%p '%s') this: %p returning: %p\n", id, id, this, rv);
               return rv;
            }
         }
         w = w->prev;
#ifdef DEBUG
         if (!w) {
            printd(0, "ThreadClosureVariableStack::find() this: %p no closure-bound local variable '%s' (%p) on stack (pgm: %p) p: %d curr->prev: %p\n", this, id, id, getProgram(), p, curr->prev);
            p = curr->pos - 1;
            while (p >= 0) {
               ClosureVarValue* cvv = w->var[p];
               printd(0, "var p: %d: %s (%p) (skip: %d)\n", p, cvv ? cvv->id : "frame boundary", cvv ? cvv->id : nullptr, cvv ? cvv->skip : false);
               --p;
            }
         }
#endif
         assert(w);
      }
      // to avoid a warning on most compilers - note that this generates a warning on aCC!
      return 0;
   }

   DLLLOCAL cvv_vec_t* getAll() const {
      cvv_vec_t* cv = 0;
      Block* w = curr;
      while (w) {
         int p = w->pos;
         while (p) {
            --p;
            ClosureVarValue* cvv = w->var[p];
            // skip frame boundaries
            if (!cvv)
               continue;
            if (!cv)
               cv = new cvv_vec_t;
            cv->push_back(cvv->refSelf());
         }
         w = w->prev;
      }
      //printd(5, "ThreadClosureVariableStack::getAll() this: %p cv: %p size: %d\n", this, cv, cv ? cv->size() : 0);
      return cv;
   }

   DLLLOCAL void pushFrameBoundary() {
      //printd(5, "ThreadClosureVariableStack::pushFrameBoundary()\n");
      instantiateIntern(nullptr);
   }

   DLLLOCAL void popFrameBoundary() {
      //printd(5, "ThreadClosureVariableStack::popFrameBoundary()\n");
      uninstantiateIntern();
      assert(!curr->var[curr->pos]);
   }

   DLLLOCAL void getLocalVars(QoreHashNode& h, int frame, ExceptionSink* xsink);

   // returns 0 = OK, 1 = no such variable, -1 exception setting variable
   DLLLOCAL int setVarValue(const char* name, const QoreValue& val, ExceptionSink* xsink);
};

#endif
