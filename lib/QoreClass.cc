/*
  QoreClass.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/QoreClass.h>
#include <qore/qore_thread.h>
#include <qore/params.h>
#include <qore/Namespace.h>
#include <qore/Sequence.h>
#include <qore/BuiltinFunctionList.h>

// global class ID sequence
class Sequence classIDSeq;

inline void BCSMList::addBaseClassesToSubclass(class QoreClass *thisclass, class QoreClass *sc)
{
   class_list_t::const_iterator i = begin();
   while (i != end())
   {
      sc->scl->sml.add(thisclass, *i);
      i++;
   }
}

inline void BCSMList::add(class QoreClass *thisclass, class QoreClass *qc)
{
   if (thisclass == qc)
   {
      parse_error("class '%s' cannot inherit itself", qc->getName());
      return;
   }
   // see if class already exists in list
   class_list_t::const_iterator i = begin();
   while (i != end())
   {
      if (*i == qc)
         return;
      if (*i == thisclass)
      {
      	 parse_error("circular reference in class hierarchy, '%s' is an ancestor of itself", thisclass->getName());
      	 return;
      }
      i++;
   }

   // append to the end of the doubly-linked list
   push_back(qc);
}

inline void BCSMList::execDestructors(class Object *o, class ExceptionSink *xsink)
{
   class_list_t::const_reverse_iterator i = rbegin();
   // cast below required by g++ 3.2 at least
   while (i != (class_list_t::const_reverse_iterator)rend())
   {
      printd(5, "BCSMList::execDestructors() %s::destructor() o=%08p (subclass %s)\n", (*i)->getName(), o, o->getClass()->getName());
      (*i)->execSubclassDestructor(o, xsink);
      i++;
   }
}

inline void BCSMList::execSystemDestructors(class Object *o, class ExceptionSink *xsink)
{
   class_list_t::const_reverse_iterator i = rbegin();
   while (i != (class_list_t::const_reverse_iterator)rend())
   {
      printd(5, "BCSMList::execSystemDestructors() %s::destructor() o=%08p (subclass %s)\n", (*i)->getName(), o, o->getClass()->getName());
      (*i)->execSubclassSystemDestructor(o, xsink);
      i++;
   }
}

inline void BCSMList::execCopyMethods(class Object *self, class Object *old, class ExceptionSink *xsink)
{
   class_list_t::const_iterator i = begin();
   while (i != end())
   {
      (*i)->execSubclassCopy(self, old, xsink);
      if (xsink->isEvent())
	 break;
      i++;
   }
}

inline class QoreClass *BCSMList::getClass(int cid) const
{
   class_list_t::const_iterator i = begin();
   while (i != end())
   {
      if ((*i)->getID() == cid)
	 return *i;
      i++;
   }
   return NULL;
}

inline class Method *BCList::resolveSelfMethod(char *name)
{
   class Method *m;
   class BCNode *w = head;
   while (w)
   {
      if (w->sclass)
      {
	 if (w->sclass->scl)
	    w->sclass->scl->parseInit(w->sclass, w->sclass->bcal);
	 if ((m = w->sclass->resolveSelfMethodIntern(name)))
	    return m;
      }
      w = w->next;
   }
   return NULL;
}

inline void BCList::execConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink)
{
   class BCNode *w = head;
   while (w)
   {
      printd(5, "BCList::execConstructors() %s::constructor() o=%08p (for subclass %s)\n", w->sclass->getName(), o, o->getClass()->getName()); 
      w->sclass->execSubclassConstructor(o, bceal, xsink);
      if (xsink->isEvent())
	 break;
      w = w->next;
   }
}

inline void BCList::execSystemConstructors(class Object *o, class BCEAList *bceal, class ExceptionSink *xsink)
{
   class BCNode *w = head;
   while (w)
   {
      printd(5, "BCList::execSystemConstructors() %s::constructor() o=%08p (for subclass %s)\n", w->sclass->getName(), o, o->getClass()->getName()); 
      w->sclass->execSubclassSystemConstructor(o, bceal, xsink);
      if (xsink->isEvent())
	 break;
      w = w->next;
   }
}

inline void QoreClass::init(char *nme, int dom)
{
   initialized = false;
   domain = dom;
   scl = NULL;
   name = nme;
   sys  = false;
   pending_head = NULL;
   bcal = NULL;

   // quick pointers
   constructor = NULL;
   destructor  = NULL;
   copyMethod  = NULL;
   methodGate  = NULL;
   memberGate  = NULL;

   system_constructor = NULL;
}

QoreClass::QoreClass(int dom, char *nme)
{
   init(nme, dom);

   classID = classIDSeq.next();
   printd(5, "QoreClass::QoreClass() creating '%s' ID:%d (this=%08p)\n", name, classID, this);
}

QoreClass::QoreClass(char *nme)
{
   init(nme);

   classID = classIDSeq.next();
   printd(5, "QoreClass::QoreClass() creating '%s' ID:%d (this=%08p)\n", name, classID, this);
}

QoreClass::QoreClass()
{
   init(NULL);

   classID = classIDSeq.next();
   printd(5, "QoreClass::QoreClass() creating unnamed class ID:%d (this=%08p)\n", classID, this);
}

QoreClass::QoreClass(char *nme, int id)
{
   init(strdup(nme));

   classID = id;
   printd(5, "QoreClass::QoreClass() creating copy of '%s' ID:%d (this=%08p)\n", name, classID, this);
}

QoreClass::~QoreClass()
{
   //printd(5, "QoreClass::~QoreClass() deleting %08p %s\n", this, name);
   hm_method_t::iterator i;
   while ((i = hm.begin()) != hm.end())
   {
      class Method *m = i->second;
      //printd(5, "QoreClass::~QoreClass() deleting method %08p %s::%s()\n", m, name, m->name);
      hm.erase(i);
      delete m;
   }   
   // delete private member list
   hm_qn_t::iterator j;
   while ((j = pmm.begin()) != pmm.end())
   {
      char *n = j->first;
      pmm.erase(j);
      free(n);
   }
   while ((j = pending_pmm.begin()) != pending_pmm.end())
   {
      char *n = j->first;
      pending_pmm.erase(j);
      free(n);
   }
   // delete any pending methods
   delete_pending_methods();
   free(name);
   if (scl)
      scl->deref();
   if (system_constructor)
      delete system_constructor;
}

class QoreClass *QoreClass::getClass(int cid) const
{
   if (cid == classID)
      return (QoreClass *)this;
   return scl ? scl->sml.getClass(cid) : NULL;
}

class QoreNode *Method::eval(Object *self, QoreNode *args, ExceptionSink *xsink)
{
   QoreNode *rv = NULL;

   tracein("Method::eval()");
#ifdef DEBUG
   char *oname = self->getClass()->getName();
   printd(5, "Method::eval() %s::%s() (object=%08p, pgm=%08p)\n", 
	  oname, name, self, self->getProgram());
#endif

   int need_deref = 0;

   // need to evaluate arguments before pushing object context
   QoreNode *new_args;
   if (args)
   {
      if (args->val.list->needsEval())
      {
	 printd(5, "Method::eval() about to evaluate args=%08p (%s)\n", args, args->type->name);
	 new_args = args->eval(xsink);
	 printd(5, "Method::eval() args=%08p (%s) new_args=%08p (%s)\n",
		args, args->type->name, new_args, new_args ? new_args->type->name : "NONE");
	 if (xsink->isEvent())
	 {
	    if (new_args)
	       new_args->deref(xsink);
	    traceout("Method::eval()");
	    return NULL;
	 }
	 need_deref = 1;
      }
      else
	 new_args = args;
   }
   else
      new_args = NULL;

   // switch to new program for imported objects
   QoreProgram *cpgm;
   QoreProgram *opgm = self->getProgram();
   if (opgm)
   {
      cpgm = getProgram();
      if (opgm && cpgm != opgm)
	 pushProgram(opgm);
   }
   else
      cpgm = NULL;

   if (type == OTF_USER)
      rv = func.userFunc->eval(new_args, self, xsink);
   else
      rv = self->evalBuiltinMethodWithPrivateData(func.builtin, new_args, xsink);

   // switch back to original program if necessary
   if (opgm && cpgm != opgm)
      popProgram();

   if (new_args && need_deref)
      new_args->deref(xsink);
#ifdef DEBUG
   printd(5, "Method::eval() %s::%s() returning %08p (type=%s, refs=%d)\n",
	  oname, name, rv, rv ? rv->type->name : "(null)", rv ? rv->reference_count() : 0);
#endif
   traceout("Method::eval()");
   return rv;
}

void Method::evalConstructor(Object *self, QoreNode *args, class BCList *bcl, class BCEAList *bceal, ExceptionSink *xsink)
{
   tracein("Method::evalConstructor()");
#ifdef DEBUG
   char *oname = self->getClass()->getName();
   printd(5, "Method::evalConstructor() %s::%s() (object=%08p, pgm=%08p)\n", 
	  oname, name, self, self->getProgram());
#endif

   int need_deref = 0;

   // need to evaluate arguments before pushing object context
   QoreNode *new_args;
   if (args)
   {
      if (args->val.list->needsEval())
      {
	 printd(5, "Method::evalConstructor() about to evaluate args=%08p (%s)\n", args, args->type->name);
	 new_args = args->eval(xsink);
	 printd(5, "Method::evalConstructor() args=%08p (%s) new_args=%08p (%s)\n",
		args, args->type->name, new_args, new_args ? new_args->type->name : "NONE");
	 if (xsink->isEvent())
	 {
	    if (new_args)
	       new_args->deref(xsink);
	    traceout("Method::evalConstructor()");
	    return;
	 }
	 need_deref = 1;
      }
      else
	 new_args = args;
   }
   else
      new_args = NULL;

   if (!xsink->isEvent())
   {
      if (type == OTF_USER)
      {
	 class QoreNode *rv = func.userFunc->evalConstructor(new_args, self, bcl, bceal, xsink);
	 if (rv)
	    rv->deref(xsink);
      }
      else
      {
	 // switch to new program for imported objects
	 QoreProgram *cpgm;
	 QoreProgram *opgm = self->getProgram();
	 if (opgm)
	 {
	    cpgm = getProgram();
	    if (opgm && cpgm != opgm)
	       pushProgram(opgm);
	 }
	 else
	    cpgm = NULL;

	 func.builtin->evalConstructor(self, new_args, xsink);

	 // switch back to original program if necessary
	 if (opgm && cpgm != opgm)
	    popProgram();
      }
   }

   if (new_args && need_deref)
      new_args->deref(xsink);
#ifdef DEBUG
   printd(5, "Method::evalConstructor() %s::%s() done\n", oname, name);
#endif
   traceout("Method::evalConstructor()");
}

void Method::evalCopy(Object *self, Object *old, ExceptionSink *xsink)
{
   // switch to new program for imported objects
   QoreProgram *cpgm;
   QoreProgram *opgm = self->getProgram();
   if (opgm)
   {
      cpgm = getProgram();
      if (opgm && cpgm != opgm)
	 pushProgram(opgm);
   }
   else
      cpgm = NULL;

   if (type == OTF_USER)
      func.userFunc->evalCopy(old, self, xsink);
   else // builtin function
      old->evalCopyMethodWithPrivateData(func.builtin, self, xsink);

   // switch back to original program if necessary
   if (opgm && cpgm != opgm)
      popProgram();
}

void Method::evalDestructor(Object *self, ExceptionSink *xsink)
{
   // switch to new program for imported objects
   QoreProgram *cpgm;
   QoreProgram *opgm = self->getProgram();
   if (opgm)
   {
      cpgm = getProgram();
      if (opgm && cpgm != opgm)
	 pushProgram(opgm);
   }
   else
      cpgm = NULL;

   if (type == OTF_USER)
      func.userFunc->eval(NULL, self, xsink);
   else // builtin function
   {
      void *ptr = self->getAndClearPrivateData(func.builtin->myclass->getID());
      if (ptr)
	 func.builtin->evalDestructor(self, ptr, xsink);
      else
      {
	 if (self->getClass() == func.builtin->myclass)
	    xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::destructor() cannot be executed because the object has already been deleted", self->getClass()->getName());
	 else
	    xsink->raiseException("OBJECT-ALREADY-DELETED", "the method %s::destructor() (base class of '%s') cannot be executed because the object has already been deleted", func.builtin->myclass->getName(), self->getClass()->getName());
      }
   }

   // switch back to original program if necessary
   if (opgm && cpgm != opgm)
      popProgram();
}

class QoreClass *QoreClass::copyAndDeref()
{
   tracein("QoreClass::copyAndDeref");
   class QoreClass *noc = new QoreClass(name, classID);

   printd(0, "QoreClass::copyAndDeref() name=%s (%08p) new name=%s (%08p)\n", name, name, noc->name, noc->name);

   // set up function list

   for (hm_method_t::iterator i = hm.begin(); i != hm.end(); i++)
   {
      class Method *nf = i->second->copy();

      noc->hm[nf->name] = nf;
      if (i->second == constructor)
	 noc->constructor  = nf;
      else if (i->second == destructor)
	 noc->destructor   = nf;
      else if (i->second == copyMethod)
	 noc->copyMethod   = nf;
      else if (i->second == methodGate)
	 noc->methodGate   = nf;
      else if (i->second == memberGate)
	 noc->memberGate   = nf;
   }
   // copy private member list
   for (hm_qn_t::iterator i = pmm.begin(); i != pmm.end(); i++)
      noc->pmm[strdup(i->first)] = NULL;

   // note that if there is a base class argument list, it
   // is referenced when the constructor is copied
   noc->bcal = bcal;
   if (scl)
   {
      scl->ref();
      noc->scl = scl;
   }

   nderef();
   traceout("QoreClass::copyAndDeref");
   return noc;
}

inline void QoreClass::insertMethod(Method *o)
{
   //printd(5, "QoreClass::insertMethod() %s::%s() size=%d\n", name, o->name, numMethods());

   hm[o->name] = o;
}      

class QoreNode *QoreClass::evalMethod(Object *self, char *nme, QoreNode *args, class ExceptionSink *xsink)
{
   tracein("QoreClass::evalMethod()");
   Method *w;
   int external = (this != getStackClass());
   printd(5, "QoreClass::evalMethod() %s::%s() %s call attempted\n", name, nme, external ? "external" : "internal" );

   if (!strcmp(nme, "copy"))
   {
      traceout("QoreClass::evalMethod()");
      return execCopy(self, xsink);
   }

   bool priv = false;
   if (!(w = findMethod(nme, &priv)))
   {
      if (methodGate && !methodGate->inMethod(self)) // call methodGate with unknown method name and arguments
	 return evalMethodGate(self, nme, args, xsink);
      // otherwise return an exception
      xsink->raiseException("METHOD-DOES-NOT-EXIST", "no method %s::%s() has been defined", name, nme);
      traceout("QoreClass::evalMethod()");
      return NULL;
   }
   // check for illegal explicit call
   if (//w == copyMethod || 
       w == constructor || w == destructor)
   {
      xsink->raiseException("ILLEGAL-EXPLICIT-METHOD-CALL", "explicit calls to ::%s() methods are not allowed", nme);
      traceout("QoreClass::evalMethod()");
      return NULL;      
   }

   if (external)
      if (w->isPrivate())
      {
	 xsink->raiseException("METHOD-IS-PRIVATE", "%s::%s() is private and cannot be accessed externally", name, nme);
	 traceout("QoreClass::evalMethod()");
	 return NULL;
      }
      else if (priv)
      {
	 xsink->raiseException("BASE-CLASS-IS-PRIVATE", "%s() is a method of a privately-inherited class of %s", nme, name);
	 traceout("QoreClass::evalMethod()");
	 return NULL;
      }
   traceout("QoreClass::evalMethod()");
   return w->eval(self, args, xsink);
}

class QoreNode *QoreClass::evalMethodGate(Object *self, char *nme, QoreNode *args, ExceptionSink *xsink)
{
   tracein("QoreClass::evalMethodGate()");
   printd(5, "QoreClass::evalMethodGate() method=%s args=%08p\n", nme, args);
   // build new argument list
   if (args)
   {
      args = args->eval(xsink);
      if (xsink->isEvent())
      {
	 args->deref(xsink);
	 traceout("QoreClass::evalMethodGate()");
	 return NULL;
      }
      args->val.list->insert(new QoreNode(nme));
   }
   else
   {
      args = new QoreNode(new List());
      args->val.list->push(new QoreNode(nme));
   }
   QoreNode *rv = methodGate->eval(self, args, xsink);
   args->deref(xsink);
   
   traceout("QoreClass::evalMethodGate()");
   return rv;
}

bool QoreClass::isPrivateMember(char *str) const
{
   hm_qn_t::const_iterator i = pmm.find(str);
   if (i != pmm.end())
      return true;

   if (scl)
      return scl->isPrivateMember(str);
   return false;
}

inline class QoreNode *QoreClass::evalMemberGate(class Object *self, class QoreNode *nme, class ExceptionSink *xsink)
{
   tracein("QoreClass::evalMembeGatre()");
   printd(5, "QoreClass::evalMemberGate() member=%s\n", nme->val.String->getBuffer());
   // do not run memberGate method if we are already in it...
   if (!memberGate || memberGate->inMethod(self))
   {
      traceout("QoreClass::evalMemberGate()");
      return NULL;
   }
   class QoreNode *args = new QoreNode(new List());
   args->val.list->push(nme->RefSelf());
   class QoreNode *rv = memberGate->eval(self, args, xsink);
   args->deref(xsink);
   traceout("QoreClass::evalMemberGate()");
   return rv;
}

inline class QoreNode *QoreClass::execConstructor(QoreNode *args, ExceptionSink *xsink)
{
   // create new object
   class Object *o = new Object(this, getProgram());
   class BCEAList *bceal;
   if (scl)
      bceal = new BCEAList();
   else
      bceal = NULL;

   printd(5, "QoreClass::execConstructor() %s::constructor() o=%08p\n", name, o);

   if (!constructor)
   {
      if (scl) // execute superclass constructors if any
	 scl->execConstructors(o, bceal, xsink);
   }
   else // no lock is sent with constructor, because no variable has been assigned yet
      constructor->evalConstructor(o, args, scl, bceal, xsink);

   if (bceal)
      bceal->deref(xsink);

   if (xsink->isEvent())
   {
      // instead of executing the destructors for the superclasses that were already executed we call Object::obliterate()
      // which will clear out all the private data by running their dereference methods which should generally be OK
      o->obliterate(xsink);
      printd(5, "QoreClass::execConstructor() %s::constructor() o=%08p, exception in constructor, dereferencing object and returning NULL\n", name, o);
      return NULL;
   }

   QoreNode *rv = new QoreNode(o);
   printd(5, "QoreClass::execConstructor() %s::constructor() o=%08p, returning %08p\n", name, o, rv);
   return rv;
}

class QoreNode *QoreClass::execSystemConstructor(QoreNode *args, class ExceptionSink *xsink)
{
   // create new object
   class Object *o = new Object(this, NULL);
   class BCEAList *bceal;
   if (scl)
      bceal = new BCEAList();
   else
      bceal = NULL;

   printd(5, "QoreClass::execSystemConstructor() %s::constructor() o=%08p\n", name, o);

   if (!constructor)
   {
      if (scl) // execute superclass constructors if any
	 scl->execSystemConstructors(o, bceal, xsink);
   }
   else // no lock is sent with constructor, because no variable has been assigned yet
      system_constructor->evalSystemConstructor(o, args, scl, bceal, xsink);

   if (bceal)
      bceal->deref(xsink);

   // should never happen!
#ifdef DEBUG
   if (xsink->isEvent())
   {
      o->dereference(xsink);
      run_time_error("QoreClass::execSystemConstructor() %s::constructor() o=%08p, exception in constructor, dereferencing object and returning NULL\n", name, o);
      return NULL;
   }
#endif

   QoreNode *rv = new QoreNode(o);
   printd(5, "QoreClass::execSystemConstructor() %s::constructor() o=%08p, returning %08p\n", name, o, rv);
   return rv;
}

inline void QoreClass::execSubclassConstructor(class Object *self, class BCEAList *bceal, class ExceptionSink *xsink)
{
   if (!constructor)
   {
      if (scl) // execute superclass constructors if any
	 scl->execConstructors(self, bceal, xsink);
   }
   else // no lock is sent with constructor, because no variable has been assigned yet
   {
      bool already_executed;
      class QoreNode *args = bceal->findArgs(this, &already_executed);
      if (!already_executed)
	 constructor->evalConstructor(self, args, scl, bceal, xsink);
   }
}

inline void QoreClass::execSubclassSystemConstructor(class Object *self, class BCEAList *bceal, class ExceptionSink *xsink)
{
   if (!constructor)
   {
      if (scl) // execute superclass constructors if any
	 scl->execSystemConstructors(self, bceal, xsink);
   }
   else // no lock is sent with constructor, because no variable has been assigned yet
   {
      bool already_executed;
      class QoreNode *args = bceal->findArgs(this, &already_executed);
      if (!already_executed)
	 system_constructor->evalSystemConstructor(self, args, scl, bceal, xsink);
   }
}

void QoreClass::execDestructor(Object *self, ExceptionSink *xsink)
{
   printd(5, "QoreClass::execDestructor() %s::destructor() o=%08p\n", name, self);

   class ExceptionSink de;

   if (self->isSystemObject())
   {
      if (destructor)
	 destructor->evalSystemDestructor(self, &de);

      // execute superclass destructors
      if (scl)
	 scl->sml.execSystemDestructors(self, &de);
   }
   else
   {
      if (destructor)
	 destructor->evalDestructor(self, &de);

      // execute superclass destructors
      if (scl)
	 scl->sml.execDestructors(self, &de);
   }

   xsink->assimilate(&de);
}

inline void QoreClass::execSubclassDestructor(Object *self, ExceptionSink *xsink)
{
   class ExceptionSink de;
   if (destructor)
      destructor->evalDestructor(self, &de);

   xsink->assimilate(&de);
}

inline void QoreClass::execSubclassSystemDestructor(Object *self, ExceptionSink *xsink)
{
   class ExceptionSink de;
   if (destructor)
      destructor->evalSystemDestructor(self, &de);

   xsink->assimilate(&de);
}

class QoreNode *QoreClass::execCopy(Object *old, ExceptionSink *xsink)
{
   class Hash *h = old->evalData(xsink);
   if (xsink->isEvent())
      return NULL;

   class Object *self = new Object(this, getProgram(), h);

   // execute superclass copy methods
   if (scl)
      scl->sml.execCopyMethods(self, old, xsink);

   if (copyMethod && !xsink->isEvent())
      copyMethod->evalCopy(self, old, xsink);

   if (!xsink->isEvent())
      return new QoreNode(self);

   return NULL;
}

inline void QoreClass::execSubclassCopy(Object *self, Object *old, ExceptionSink *xsink)
{
   if (copyMethod)
      copyMethod->evalCopy(self, old, xsink);
}

void QoreClass::addBaseClassesToSubclass(class QoreClass *sc)
{      
   if (scl)
   {
      // initialize list, just in case
      scl->parseInit(this, bcal);
      scl->sml.addBaseClassesToSubclass(this, sc);
   }
   sc->scl->sml.add(sc, this);
}

// private, called from subclasses only
inline Method *QoreClass::resolveSelfMethodIntern(char *nme)
{
   class Method *m = parseFindMethod(nme);

   // if still not found now look in superclass methods
   if (!m && scl)
      m = scl->resolveSelfMethod(nme);

   return m;
}

Method *QoreClass::resolveSelfMethod(char *nme)
{
   class Method *m = findLocalMethod(nme);
#ifdef DEBUG
   if (m)
      printd(5, "QoreClass::resolveSelfMethod(%s) resolved to %s::%s() %08p\n", nme, name, nme, m);
#endif
   bool err = false;
   if (m && (//m == copyMethod || 
	  m == constructor || m == destructor))
      err = true;

   // look in pending methods
   if (!m)
   {
      // pending methods are not set to the quick pointers, so we have to compare the strings...
      if (//!!strcmp(nme, "copy") || 
	 !strcmp(nme, "constructor") || !strcmp(nme, "destructor"))
	 err = true;
      else
      {
	 m = pending_head;

	 while (m)
	 {
	    if (!strcmp(m->name, nme))
	    {
	       printd(5, "QoreClass::resolveSelfMethod(%s) resolved to pending method %s::%s() %08p\n", nme, name, nme, m);
	       break;
	    }

	    m = m->next;
	 }
      }
   }

   // if still not found now look in superclass methods
   if (!err && !m && scl)
   {
      m = scl->resolveSelfMethod(nme);
#ifdef DEBUG
      if (m)
	 printd(5, "QoreClass::resolveSelfMethod(%s) resolved to <base class>::%s() %08p\n", nme, nme, m);
#endif
   }

   if (err)
   {
      parse_error("explicit calls to ::%s() methods are not allowed", nme);
      m = NULL;
   }
   else if (!m)
      parse_error("no method %s::%s() has been defined", name, nme);

   return m;
}

Method *QoreClass::resolveSelfMethod(class NamedScope *nme)
{
   // first find class
   class QoreClass *qc = getRootNS()->parseFindScopedClassWithMethod(nme);
   if (!qc)
      return NULL;

   // see if class is base class of this class
   if (qc != this && !scl->sml.isBaseClass(qc))
   {
      parse_error("'%s' is not a base class of '%s'", qc->getName(), name);
      return NULL;
   }

   char *nstr = nme->getIdentifier();
   class Method *m = qc->findMethod(nstr);
   bool err = false;
   if (m && (//m == copyMethod || 
	     m == constructor || m == destructor))
      err = true;

   // look in pending methods
   if (!m)
   {
      // pending methods are not set to the quick pointers, so we have to compare the strings...
      if (//!strcmp(nstr, "copy") || 
	  !strcmp(nstr, "constructor") || !strcmp(nstr, "destructor"))
	 err = true;
      else
      {
	 m = qc->pending_head;

	 while (m)
	 {
	    if (!strcmp(m->name, nstr))
	       break;
	    m = m->next;
	 }
      }
   }

   if (err)
   {
      parse_error("explicit calls to ::%s() methods are not allowed", nstr);
      m = NULL;
   }
   else if (!m)
      parse_error("no method %s::%s() has been defined", qc->getName(), nstr);

   return m;
}

void QoreClass::addMethod(Method *m)
{
   printd(5, "QoreClass::addMethod(%08p) %s.%s() (this=%08p)\n", m, name ? name : "<pending>", m->name, this);

   // check for illegal private constructor or destructor
   if (!strcmp(m->name, "constructor") || !strcmp(m->name, "destructor"))
   {
      if (m->isPrivate())
	 parseException("ILLEGAL-PRIVATE-METHOD", "%s methods cannot be private", m->name);
   }

   if (parseFindMethod(m->name))
   {
      parse_error("method '%s::%s()' has already been defined", name, m->name);
      delete m;
   }
   else
   {
      // insert in pending list for parse init
      m->next = pending_head;
      pending_head = m;

      // if there is a base class constructor argument list, then put it at the class level
      if (m->bcal)
      {
	 // if the constructor is being defined after the class has already been initialized, then throw a parse exception
	 if (numMethods())
	    parse_error("constructors making explicit calls to base classes must be defined when the class is defined");
	 else
	    bcal = m->bcal;
      }
   }
}

// adds a builtin method to the class - no duplicate checking is made
void QoreClass::addMethod(char *nme, q_method_t m)
{
#ifdef DEBUG
   if (!strcmp(nme, "constructor") || !strcmp(nme, "destructor") || !strcmp(nme, "copy"))
      run_time_error("cannot call QoreClass::addMethod('%s')  use setConstructor() setDestructor() setCopy() instead", nme);
#endif

   sys = true;
   BuiltinMethod *b = new BuiltinMethod(this, nme, m);
   Method *o = new Method(b);
   insertMethod(o);
   // check for special methods (except constructor and destructor)
   checkSpecialIntern(o);
}

// sets a builtin function as class constructor - no duplicate checking is made
void QoreClass::setConstructor(q_constructor_t m)
{
   sys = true;
   Method *o = new Method(new BuiltinMethod(this, m));
   insertMethod(o);
   constructor = o;
}

// sets a builtin function as class destructor - no duplicate checking is made
void QoreClass::setDestructor(q_destructor_t m)
{
   sys = true;
   Method *o = new Method(new BuiltinMethod(this, m));
   insertMethod(o);
   destructor = o;
}

// sets a builtin function as class copy constructor - no duplicate checking is made
void QoreClass::setCopy(q_copy_t m)
{
   sys = true;
   Method *o = new Method(new BuiltinMethod(this, m));
   insertMethod(o);
   copyMethod = o;
}

List *QoreClass::getMethodList() const
{
   List *l = new List();

   for (hm_method_t::const_iterator i = hm.begin(); i != hm.end(); i++)
      l->push(new QoreNode(i->first));
   return l;
}

// initializes all user methods
void QoreClass::parseInit()
{
   setParseClass(this);
   if (!initialized)
   {
      printd(5, "QoreClass::parseInit() %s this=%08p start=%08p\n", name, this, pending_head);
      if (scl)
	 scl->parseInit(this, bcal);

      if (!sys && domain & getProgram()->getParseOptions())
	 parseException("ILLEGAL-CLASS-DEFINITION", "class '%s' inherits functionality from base classes that is restricted by current parse options", name);
      initialized = true;
   }

   class Method *w = pending_head;
   while (w)
   {
      // initialize method
      if (w->bcal)
	 w->parseInitConstructor(scl);
      else
	 w->parseInit();
      w = w->next;
   }
}

// commits all pending user methods
void QoreClass::parseCommit()
{
   class Method *w = pending_head;

   printd(5, "QoreClass::parseCommit() %s this=%08p start=%08p\n", name, this, w);
   while (w)
   {
      insertMethod(w);
      checkSpecial(w);
      w = w->next;
   }

   // add all pending private members
   hm_qn_t::iterator i;
   while ((i = pending_pmm.begin()) != pending_pmm.end())
   { 
      //printd(5, "QoreClass::parseCommit() %s committing private member %08p %s\n", name, i->first, i->first);
      pmm[i->first] = NULL;
      pending_pmm.erase(i);
   }

   pending_head = NULL;
}

class QoreNode *internalObjectVarRef(QoreNode *n, ExceptionSink *xsink)
{
   //tracein("internalObjectVarRef()");
   //traceout("internalObjectVarRef()");
   return evalStackObjectValue(n->val.c_str, xsink);
}

static QoreNode *f_getMethodList(QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   if (!p0)
      return NULL;

   return new QoreNode(p0->val.object->getClass()->getMethodList());
}

static QoreNode *f_callObjectMethod(QoreNode *params, ExceptionSink *xsink)
{
   // get object
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   if (!p0)
      return NULL;

   // get method name
   QoreNode *p1 = test_param(params, NT_STRING, 1);
   if (!p1)
      return NULL;

   QoreNode *args;

   // if there are arguments to pass
   if (get_param(params, 2))
   {
      // create argument list by copying current list
      List *l = params->val.list->copyListFrom(2);
      if (xsink->isEvent())
      {
         if (l)
	    l->derefAndDelete(xsink);
         return NULL;
      }
      args = new QoreNode(l);
   }
   else
      args = NULL;

   // make sure method call is internal (allows access to private methods) if this function was called internally
   substituteObjectIfEqual(p0->val.object);
   QoreNode *rv = p0->val.object->evalMethod(p1->val.String, args, xsink);
   if (args)
      args->deref(xsink);
   return rv;
}

static QoreNode *f_callObjectMethodArgs(QoreNode *params, ExceptionSink *xsink)
{
   // get object
   QoreNode *p0 = test_param(params, NT_OBJECT, 0);
   if (!p0)
      return NULL;

   // get method name
   QoreNode *p1 = test_param(params, NT_STRING, 1);
   if (!p1)
      return NULL;

   QoreNode *args, *p2;

   // if there are arguments to pass
   if ((p2 = get_param(params, 2)))
   {
      if (p2->type == NT_LIST)
	 args = p2;
      else
      {
	 args = new QoreNode(new List());
	 args->val.list->push(p2);
      }
   }
   else
      args = NULL;

   // make sure method call is internal (allows access to private methods) if this function was called internally
   substituteObjectIfEqual(p0->val.object);
   QoreNode *rv = p0->val.object->evalMethod(p1->val.String, args, xsink);

   if (p2 != args)
   {
      args->val.list->shift();
      args->deref(xsink);
   }

   return rv;
}

// object subsystem is initialized here
void initObjects()
{
   tracein("initObjects()");

   builtinFunctions.add("getMethodList", f_getMethodList);
   builtinFunctions.add("callObjectMethod", f_callObjectMethod);
   builtinFunctions.add("callObjectMethodArgs", f_callObjectMethodArgs);
   traceout("initObjects()");
}

void deleteObjects()
{
}
