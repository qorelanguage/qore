/*
 QC_AutoGate.cc
 
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

#include <qore/Qore.h>
#include <qore/intern/QC_AutoGate.h>

qore_classid_t CID_AUTOGATE;

static void AG_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreGate *g = p ? (QoreGate *)p->getReferencedPrivateData(CID_GATE, xsink) : 0;
   if (*xsink)
      return;

   if (!g)
   {
      xsink->raiseException("AUTOGATE-CONSTRUCTOR-ERROR", "expecting Gate type as argument to constructor");
      return;
   }

   QoreAutoGate *ag = new QoreAutoGate(g, xsink);
   if (*xsink)
      ag->deref(xsink);
   else
      self->setPrivate(CID_AUTOGATE, ag);
}

static void AG_destructor(QoreObject *self, class QoreAutoGate *ag, ExceptionSink *xsink)
{
   ag->destructor(xsink);
   ag->deref(xsink);
}

static void AG_copy(QoreObject *self, QoreObject *old, class QoreAutoGate *m, ExceptionSink *xsink)
{
   xsink->raiseException("AUTOGATE-COPY-ERROR", "objects of this class cannot be copied");
}

class QoreClass *initAutoGateClass()
{
   QORE_TRACE("initAutoGateClass()");
   
   class QoreClass *QC_AutoGate = new QoreClass("AutoGate", QDOM_THREAD_CLASS);
   CID_AUTOGATE = QC_AutoGate->getID();
   QC_AutoGate->setConstructor(AG_constructor);
   QC_AutoGate->setDestructor((q_destructor_t)AG_destructor);
   QC_AutoGate->setCopy((q_copy_t)AG_copy);
   

   return QC_AutoGate;
}
