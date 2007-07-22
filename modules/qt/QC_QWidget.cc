/*
 QC_QWidget.cc
 
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

#include "QC_QWidget.h"
#include "QC_QFont.h"

int CID_QWIDGET;

static void QW_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQWidget *qw;
   int np = num_params(params);
   if (!np)
      qw = new QoreQWidget();
   else 
   {
      QoreNode *p = test_param(params, NT_OBJECT, 0);
      QoreQWidget *parent = p ? (QoreQWidget *)p->val.object->getReferencedPrivateDataFromMetaClass(CID_QWIDGET, xsink) : 0;
      if (!parent)
      {
	 if (!xsink->isException())
	    xsink->raiseException("QWIDGET-CONSTRUCTOR-ERROR", "expecting an object derived from QWidget as parameter to QWidget::constructor() in first argument if passed, however the argument is either NOTHING or not derived from QWidget (type passed: %s)", p ? p->type->getName() : "NOTHING");
	 return;
      }
      ReferenceHolder<QoreQWidget> holder(parent, xsink);
      p = get_param(params, 1);
      int window_flags = p ? p->getAsInt() : 0;
      qw = new QoreQWidget(parent->getQWidget(), (Qt::WindowFlags)window_flags);
   }

   self->setPrivate(CID_QWIDGET, CID_QOBJECT, qw);
}

static void QW_destructor(class Object *self, class QoreQWidget *qw, ExceptionSink *xsink)
{
   qw->destructor(xsink);
   qw->deref(xsink);
}

static void QW_copy(class Object *self, class Object *old, class QoreQWidget *qw, ExceptionSink *xsink)
{
   xsink->raiseException("QWIDGET-COPY-ERROR", "objects of this class cannot be copied");
}

typedef QoreNode *(*qw_func_t)(Object *, QoreQWidget *, QoreNode *, ExceptionSink *);

class QoreClass *initQWidgetClass()
{
   tracein("initQWidgetClass()");
   
   class QoreClass *QC_QWidget = new QoreClass("QWidget", QDOM_GUI);
   CID_QWIDGET = QC_QWidget->getID();
   QC_QWidget->setConstructor(QW_constructor);
   QC_QWidget->setDestructor((q_destructor_t)QW_destructor);
   QC_QWidget->setCopy((q_copy_t)QW_copy);

   // inherited functions from templates
   QC_QWidget->addMethod("inherits",     (q_method_t)(qw_func_t)QO_inherits<QoreQWidget>);
   QC_QWidget->addMethod("resize",       (q_method_t)(qw_func_t)QW_resize<QoreQWidget>);
   QC_QWidget->addMethod("setGeometry",  (q_method_t)(qw_func_t)QW_setGeometry<QoreQWidget>);
   QC_QWidget->addMethod("show",         (q_method_t)(qw_func_t)QW_show<QoreQWidget>);
   QC_QWidget->addMethod("setFont",      (q_method_t)(qw_func_t)QW_setFont<QoreQWidget>);
   
   traceout("initQWidgetClass()");
   return QC_QWidget;
}
