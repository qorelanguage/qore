/*
 QC_QApplication.cc
 
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
#include "QC_QApplication.h"

DLLLOCAL int CID_QAPPLICATION;

DLLLOCAL int static_argc    = 0;
DLLLOCAL char **static_argv = 0;

static LockedObject qapp_lock;
static int qapp_count = 0;

void qapp_dec()
{
   AutoLocker al(&qapp_lock);
   --qapp_count;
}

static void QA_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   AutoLocker al(&qapp_lock);
   if (qapp_count) {
      xsink->raiseException("QAPPLICATION-ERROR", "only one QApplication can exist at one time");
      return;
   }
   ++qapp_count;

   QoreQApplication *qa = new QoreQApplication();
   self->setPrivate(CID_QAPPLICATION, qa);
}

static void QA_destructor(class Object *self, class QoreQApplication *qa, ExceptionSink *xsink)
{
   qa->destructor(xsink);
   qa->deref(xsink);
}

static void QA_copy(class Object *self, class Object *old, class QoreQApplication *qa, ExceptionSink *xsink)
{
   xsink->raiseException("QAPPLICATION-COPY-ERROR", "objects of this class cannot be copied");
}

static class QoreNode *QA_exec(class Object *self, class QoreQApplication *qa, class QoreNode *params, ExceptionSink *xsink)
{
   qa->exec();
   return 0;
}

class QoreClass *initQApplicationClass()
{
   tracein("initQApplicationClass()");
   
   class QoreClass *QC_QApplication = new QoreClass("QApplication", QDOM_GUI);
   CID_QAPPLICATION = QC_QApplication->getID();
   QC_QApplication->setConstructor(QA_constructor);
   QC_QApplication->setDestructor((q_destructor_t)QA_destructor);
   QC_QApplication->setCopy((q_copy_t)QA_copy);

   QC_QApplication->addMethod("exec",    (q_method_t)QA_exec);

   traceout("initQApplicationClass()");
   return QC_QApplication;
}
