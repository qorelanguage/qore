/*
 QC_QStackedWidget.cc
 
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

#include "QC_QStackedWidget.h"
#include "QC_QWidget.h"

#include "qore-qt.h"

int CID_QSTACKEDWIDGET;
class QoreClass *QC_QStackedWidget = 0;

//QStackedWidget ( QWidget * parent = 0 )
static void QSTACKEDWIDGET_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QSTACKEDWIDGET, new QoreQStackedWidget(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QSTACKEDWIDGET_copy(class QoreObject *self, class QoreObject *old, class QoreQStackedWidget *qsw, ExceptionSink *xsink)
{
   xsink->raiseException("QSTACKEDWIDGET-COPY-ERROR", "objects of this class cannot be copied");
}

//int addWidget ( QWidget * widget )
static QoreNode *QSTACKEDWIDGET_addWidget(QoreObject *self, QoreQStackedWidget *qsw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSTACKEDWIDGET-ADDWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QStackedWidget::addWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreBigIntNode(qsw->qobj->addWidget(static_cast<QWidget *>(widget->getQWidget())));
}

//int count () const
static QoreNode *QSTACKEDWIDGET_count(QoreObject *self, QoreQStackedWidget *qsw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qsw->qobj->count());
}

//int currentIndex () const
static QoreNode *QSTACKEDWIDGET_currentIndex(QoreObject *self, QoreQStackedWidget *qsw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qsw->qobj->currentIndex());
}

//QWidget * currentWidget () const
static QoreNode *QSTACKEDWIDGET_currentWidget(QoreObject *self, QoreQStackedWidget *qsw, const QoreListNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = qsw->qobj->currentWidget();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QWidget, getProgram());
      QoreQtQWidget *t_qobj = new QoreQtQWidget(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QWIDGET, t_qobj);
   }
   return rv_obj;
}

//int indexOf ( QWidget * widget ) const
static QoreNode *QSTACKEDWIDGET_indexOf(QoreObject *self, QoreQStackedWidget *qsw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSTACKEDWIDGET-INDEXOF-PARAM-ERROR", "expecting a QWidget object as first argument to QStackedWidget::indexOf()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreBigIntNode(qsw->qobj->indexOf(static_cast<QWidget *>(widget->getQWidget())));
}

//int insertWidget ( int index, QWidget * widget )
static QoreNode *QSTACKEDWIDGET_insertWidget(QoreObject *self, QoreQStackedWidget *qsw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSTACKEDWIDGET-INSERTWIDGET-PARAM-ERROR", "expecting a QWidget object as second argument to QStackedWidget::insertWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreBigIntNode(qsw->qobj->insertWidget(index, static_cast<QWidget *>(widget->getQWidget())));
}

//void removeWidget ( QWidget * widget )
static QoreNode *QSTACKEDWIDGET_removeWidget(QoreObject *self, QoreQStackedWidget *qsw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSTACKEDWIDGET-REMOVEWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QStackedWidget::removeWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qsw->qobj->removeWidget(static_cast<QWidget *>(widget->getQWidget()));
   return 0;
}

//QWidget * widget ( int index ) const
static QoreNode *QSTACKEDWIDGET_widget(QoreObject *self, QoreQStackedWidget *qsw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   QWidget *qt_qobj = qsw->qobj->widget(index);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QWidget, getProgram());
      QoreQtQWidget *t_qobj = new QoreQtQWidget(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QWIDGET, t_qobj);
   }
   return rv_obj;
}

//void setCurrentIndex ( int index )
static QoreNode *QSTACKEDWIDGET_setCurrentIndex(QoreObject *self, QoreQStackedWidget *qsw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qsw->qobj->setCurrentIndex(index);
   return 0;
}

//void setCurrentWidget ( QWidget * widget )
static QoreNode *QSTACKEDWIDGET_setCurrentWidget(QoreObject *self, QoreQStackedWidget *qsw, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSTACKEDWIDGET-SETCURRENTWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QStackedWidget::setCurrentWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qsw->qobj->setCurrentWidget(static_cast<QWidget *>(widget->getQWidget()));
   return 0;
}

QoreClass *initQStackedWidgetClass(QoreClass *qwidget)
{
   QC_QStackedWidget = new QoreClass("QStackedWidget", QDOM_GUI);
   CID_QSTACKEDWIDGET = QC_QStackedWidget->getID();

   QC_QStackedWidget->addBuiltinVirtualBaseClass(qwidget);

   QC_QStackedWidget->setConstructor(QSTACKEDWIDGET_constructor);
   QC_QStackedWidget->setCopy((q_copy_t)QSTACKEDWIDGET_copy);

   QC_QStackedWidget->addMethod("addWidget",                   (q_method_t)QSTACKEDWIDGET_addWidget);
   QC_QStackedWidget->addMethod("count",                       (q_method_t)QSTACKEDWIDGET_count);
   QC_QStackedWidget->addMethod("currentIndex",                (q_method_t)QSTACKEDWIDGET_currentIndex);
   QC_QStackedWidget->addMethod("currentWidget",               (q_method_t)QSTACKEDWIDGET_currentWidget);
   QC_QStackedWidget->addMethod("indexOf",                     (q_method_t)QSTACKEDWIDGET_indexOf);
   QC_QStackedWidget->addMethod("insertWidget",                (q_method_t)QSTACKEDWIDGET_insertWidget);
   QC_QStackedWidget->addMethod("removeWidget",                (q_method_t)QSTACKEDWIDGET_removeWidget);
   QC_QStackedWidget->addMethod("widget",                      (q_method_t)QSTACKEDWIDGET_widget);
   QC_QStackedWidget->addMethod("setCurrentIndex",             (q_method_t)QSTACKEDWIDGET_setCurrentIndex);
   QC_QStackedWidget->addMethod("setCurrentWidget",            (q_method_t)QSTACKEDWIDGET_setCurrentWidget);

   return QC_QStackedWidget;
}
