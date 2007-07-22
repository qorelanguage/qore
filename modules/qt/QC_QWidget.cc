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
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateDataFromMetaClass(CID_QWIDGET, xsink) : 0;

   if (!parent)
      qw = new QoreQWidget();
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
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
   QC_QWidget->addMethod("inherits",          (q_method_t)(qw_func_t)QO_inherits<QoreQWidget>);
   QC_QWidget->addMethod("resize",            (q_method_t)(qw_func_t)QW_resize<QoreQWidget>);
   QC_QWidget->addMethod("setGeometry",       (q_method_t)(qw_func_t)QW_setGeometry<QoreQWidget>);
   QC_QWidget->addMethod("show",              (q_method_t)(qw_func_t)QW_show<QoreQWidget>);
   QC_QWidget->addMethod("setFont",           (q_method_t)(qw_func_t)QW_setFont<QoreQWidget>);
   QC_QWidget->addMethod("setFixedHeight",    (q_method_t)(qw_func_t)QW_setFixedHeight<QoreQWidget>);
   QC_QWidget->addMethod("setFixedWidth",     (q_method_t)(qw_func_t)QW_setFixedWidth<QoreQWidget>);
   QC_QWidget->addMethod("setFixedSize",      (q_method_t)(qw_func_t)QW_setFixedSize<QoreQWidget>);
   QC_QWidget->addMethod("setMinimumHeight",  (q_method_t)(qw_func_t)QW_setMinimumHeight<QoreQWidget>);
   QC_QWidget->addMethod("setMinimumWidth",   (q_method_t)(qw_func_t)QW_setMinimumWidth<QoreQWidget>);
   QC_QWidget->addMethod("setMinimumSize",    (q_method_t)(qw_func_t)QW_setMinimumSize<QoreQWidget>);
   QC_QWidget->addMethod("setMaximumHeight",  (q_method_t)(qw_func_t)QW_setMaximumHeight<QoreQWidget>);
   QC_QWidget->addMethod("setMaximumWidth",   (q_method_t)(qw_func_t)QW_setMaximumWidth<QoreQWidget>);
   QC_QWidget->addMethod("setMaximumSize",    (q_method_t)(qw_func_t)QW_setMaximumSize<QoreQWidget>);
   QC_QWidget->addMethod("setLayout",         (q_method_t)(qw_func_t)QW_setLayout<QoreQWidget>);
   
   traceout("initQWidgetClass()");
   return QC_QWidget;
}
