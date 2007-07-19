/*
 QC_QPushButton.cc
 
 Qore Programming Langupbe
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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

#include "QC_QPushButton.h"

int CID_QPUSHBUTTON;

static void QPB_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_STRING, 0);

   QoreQPushButton *pb = p ? new QoreQPushButton(p->val.String->getBuffer()) : new QoreQPushButton();
   self->setPrivate(CID_QPUSHBUTTON, pb);
}

static void QPB_destructor(class Object *self, class QoreQPushButton *pb, ExceptionSink *xsink)
{
   pb->destructor(xsink);
   pb->deref(xsink);
}

static void QPB_copy(class Object *self, class Object *old, class QoreQPushButton *pb, ExceptionSink *xsink)
{
   xsink->raiseException("QPUSHBUTTON-COPY-ERROR", "objects of this class cannot be copied");
}

static class QoreNode *QPB_resize(class Object *self, class QoreQPushButton *pb, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      xsink->raiseException("QPUSHBUTTON-RESIZE-ERROR", "missing first argument: x size");
      return 0;
   }
   int x = p->getAsInt();
   
   p = get_param(params, 1);
   if (is_nothing(p)) {
      xsink->raiseException("QPUSHBUTTON-RESIZE-ERROR", "missing second argument: y size");
      return 0;
   }
   int y = p->getAsInt();

   pb->resize(x, y);
   return 0;
}

static class QoreNode *QPB_show(class Object *self, class QoreQPushButton *pb, class QoreNode *params, ExceptionSink *xsink)
{
   pb->show();
   return 0;
}

class QoreClass *initQPushButtonClass()
{
   tracein("initQPushButtonClass()");
   
   class QoreClass *QC_QPushButton = new QoreClass("QPushButton", QDOM_GUI);
   CID_QPUSHBUTTON = QC_QPushButton->getID();
   QC_QPushButton->setConstructor(QPB_constructor);
   QC_QPushButton->setDestructor((q_destructor_t)QPB_destructor);
   QC_QPushButton->setCopy((q_copy_t)QPB_copy);
   QC_QPushButton->addMethod("resize",  (q_method_t)QPB_resize);
   QC_QPushButton->addMethod("show",    (q_method_t)QPB_show);
   
   traceout("initQPushButtonClass()");
   return QC_QPushButton;
}
