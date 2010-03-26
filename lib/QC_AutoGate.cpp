/*
 QC_AutoGate.cpp
 
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
#include <qore/intern/QC_AutoGate.h>

qore_classid_t CID_AUTOGATE;

static void AG_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(g, QoreGate, params, 0, CID_GATE, "Gate", "AutoGate::constructor", xsink);
   if (*xsink)
      return;

   assert(g);

   QoreAutoGate *ag = new QoreAutoGate(g, xsink);
   if (*xsink)
      ag->deref(xsink);
   else
      self->setPrivate(CID_AUTOGATE, ag);
}

static void AG_destructor(QoreObject *self, QoreAutoGate *ag, ExceptionSink *xsink) {
   ag->destructor(xsink);
   ag->deref(xsink);
}

static void AG_copy(QoreObject *self, QoreObject *old, QoreAutoGate *m, ExceptionSink *xsink) {
   xsink->raiseException("AUTOGATE-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initAutoGateClass(QoreClass *Gate) {
   QORE_TRACE("initAutoGateClass()");
   
   QoreClass *QC_AutoGate = new QoreClass("AutoGate", QDOM_THREAD_CLASS);
   CID_AUTOGATE = QC_AutoGate->getID();
   QC_AutoGate->setConstructorExtended(AG_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, Gate->getTypeInfo(), QORE_PARAM_NO_ARG);
   QC_AutoGate->setDestructor((q_destructor_t)AG_destructor);
   QC_AutoGate->setCopy((q_copy_t)AG_copy);
   
   return QC_AutoGate;
}
