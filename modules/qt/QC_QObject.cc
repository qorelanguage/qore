/*
 QC_QObject.cc
 
 Qore Programming Language
 
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

#include "QC_QObject.h"
#include "QC_QFont.h"

int CID_QOBJECT;

static void QO_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQObject *qo;
   int np = num_params(params);
   if (!np)
      qo = new QoreQObject();
   else 
   {
      QoreNode *p = test_param(params, NT_OBJECT, 0);
      QoreAbstractQObject *parent = p ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateDataFromMetaClass(CID_QOBJECT, xsink) : 0;
      if (!parent)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QOBJECT-CONSTRUCTOR-ERROR", "expecting an object derived from QObject as parameter to QObject::constructor() in first argument if passed, argument is either not an argument or not derived from QObject (type passed: %s)", p ? p->type->getName() : "NOTHING");
	 return;
      }
      ReferenceHolder<QoreAbstractQObject> holder(parent, xsink);
      qo = new QoreQObject(parent->getQObject());
   }

   self->setPrivate(CID_QOBJECT, qo);
}

static void QO_destructor(class Object *self, class QoreQObject *qo, ExceptionSink *xsink)
{
   qo->destructor(xsink);
   qo->deref(xsink);
}

static void QO_copy(class Object *self, class Object *old, class QoreQObject *qo, ExceptionSink *xsink)
{
   xsink->raiseException("QOBJECT-COPY-ERROR", "objects of this class cannot be copied");
}

typedef QoreNode *(*qo_func_t)(Object *, QoreQObject *, QoreNode *, ExceptionSink *);

class QoreClass *initQObjectClass()
{
   tracein("initQObjectClass()");
   
   class QoreClass *QC_QObject = new QoreClass("QObject", QDOM_GUI);
   CID_QOBJECT = QC_QObject->getID();
   QC_QObject->setConstructor(QO_constructor);
   QC_QObject->setDestructor((q_destructor_t)QO_destructor);
   QC_QObject->setCopy((q_copy_t)QO_copy);
   
   QC_QObject->addMethod("inherits",    (q_method_t)(qo_func_t)QO_inherits<QoreQObject>);

   traceout("initQObjectClass()");
   return QC_QObject;
}
