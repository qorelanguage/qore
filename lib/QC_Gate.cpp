/*
  QC_Gate.cpp

  Qore Programming Language
  
  Copyright 2003 - 2010 David Nichols
  
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
#include <qore/intern/QC_Gate.h>

// rmutex class is depcreated and will be removed in the next major release
qore_classid_t CID_GATE, CID_RMUTEX;

static void GATE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   self->setPrivate(CID_GATE, new QoreGate);
}

static void GATE_destructor(QoreObject *self, QoreGate *g, ExceptionSink *xsink) {
   g->destructor(xsink);
   g->deref(xsink);
}

static void GATE_copy(QoreObject *self, QoreObject *old, QoreGate *g, ExceptionSink *xsink) {
   self->setPrivate(CID_GATE, new QoreGate());
}

// Gate::enter(timeout $timeout) returns int
static AbstractQoreNode *GATE_enter_to(QoreObject *self, QoreGate *g, const QoreListNode *params, ExceptionSink *xsink) {
   int rc = g->grab(xsink, (int)HARD_QORE_INT(params, 0));
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *GATE_enter(QoreObject *self, QoreGate *g, const QoreListNode *params, ExceptionSink *xsink) {
   g->grab(xsink);
   return 0;
}

static AbstractQoreNode *GATE_exit(QoreObject *self, QoreGate *g, const QoreListNode *params, ExceptionSink *xsink) {
   int rc = g->release(xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *GATE_tryEnter(QoreObject *self, QoreGate *g, const QoreListNode *params, ExceptionSink *xsink) {
   int rc = g->tryGrab();
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *GATE_numInside(QoreObject *self, QoreGate *g, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(g->get_count());
}

static AbstractQoreNode *GATE_numWaiting(QoreObject *self, QoreGate *g, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(g->get_waiting());
}

QoreClass *initGateClass() {
   QORE_TRACE("initGateClass()");

   QoreClass *QC_GATE = new QoreClass("Gate", QDOM_THREAD_CLASS);
   CID_GATE = QC_GATE->getID();

   QC_GATE->setConstructorExtended(GATE_constructor);
   QC_GATE->setDestructor((q_destructor_t)GATE_destructor);
   QC_GATE->setCopy((q_copy_t)GATE_copy);

   QC_GATE->addMethodExtended("enter",         (q_method_t)GATE_enter, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // Gate::enter(timeout $timeout) returns int
   QC_GATE->addMethodExtended("enter",         (q_method_t)GATE_enter_to, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, QORE_PARAM_NO_ARG);

   QC_GATE->addMethodExtended("exit",          (q_method_t)GATE_exit, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);
   QC_GATE->addMethodExtended("tryEnter",      (q_method_t)GATE_tryEnter, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);
   QC_GATE->addMethodExtended("numInside",     (q_method_t)GATE_numInside, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);
   QC_GATE->addMethodExtended("numWaiting",    (q_method_t)GATE_numWaiting, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   return QC_GATE;
}

QoreClass *initRMutexClass() {
   QoreClass *QC_RMUTEX = new QoreClass("RMutex", QDOM_THREAD_CLASS); 
   CID_RMUTEX = QC_RMUTEX->getID();
   QC_RMUTEX->setConstructorExtended(GATE_constructor, false, QC_DEPRECATED);
   QC_RMUTEX->setDestructor((q_destructor_t)GATE_destructor);
   QC_RMUTEX->setCopy((q_copy_t)GATE_copy);

   QC_RMUTEX->addMethodExtended("enter",         (q_method_t)GATE_enter, false, QC_DEPRECATED, QDOM_DEFAULT, nothingTypeInfo);

   // Gate::enter(timeout $timeout) returns int
   QC_RMUTEX->addMethodExtended("enter",         (q_method_t)GATE_enter_to, false, QC_DEPRECATED, QDOM_DEFAULT, bigIntTypeInfo, 1, timeoutTypeInfo, QORE_PARAM_NO_ARG);

   QC_RMUTEX->addMethodExtended("exit",          (q_method_t)GATE_exit, false, QC_DEPRECATED, QDOM_DEFAULT, bigIntTypeInfo);
   QC_RMUTEX->addMethodExtended("tryEnter",      (q_method_t)GATE_tryEnter, false, QC_DEPRECATED, QDOM_DEFAULT, bigIntTypeInfo);
   QC_RMUTEX->addMethodExtended("numInside",     (q_method_t)GATE_numInside, false, QC_RET_VALUE_ONLY | QC_DEPRECATED, QDOM_DEFAULT, bigIntTypeInfo);
   QC_RMUTEX->addMethodExtended("numWaiting",    (q_method_t)GATE_numWaiting, false, QC_RET_VALUE_ONLY | QC_DEPRECATED, QDOM_DEFAULT, bigIntTypeInfo);

   return QC_RMUTEX;
}
