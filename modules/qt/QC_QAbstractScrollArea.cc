/*
 QC_QAbstractScrollArea.cc
 
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

#include "QC_QAbstractScrollArea.h"
#include "QC_QWidget.h"
#include "QC_QScrollBar.h"

#include "qore-qt.h"

qore_classid_t CID_QABSTRACTSCROLLAREA;
class QoreClass *QC_QAbstractScrollArea = 0;

//QAbstractScrollArea ( QWidget * parent = 0 )
static void QABSTRACTSCROLLAREA_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQWidget *parent = p ? (QoreQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);
   self->setPrivate(CID_QABSTRACTSCROLLAREA, new QoreQAbstractScrollArea(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QABSTRACTSCROLLAREA_copy(class QoreObject *self, class QoreObject *old, class QoreQAbstractScrollArea *qasa, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTSCROLLAREA-COPY-ERROR", "objects of this class cannot be copied");
}

//void addScrollBarWidget ( QWidget * widget, Qt::Alignment alignment )
static AbstractQoreNode *QABSTRACTSCROLLAREA_addScrollBarWidget(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o = test_object_param(params, 0);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTSCROLLAREA-ADDSCROLLBARWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QAbstractScrollArea::addScrollBarWidget()");
      return 0;
   }
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   qasa->getQAbstractScrollArea()->addScrollBarWidget(static_cast<QWidget *>(widget->getQWidget()), alignment);
   return 0;
}

//QWidget * cornerWidget () const
static AbstractQoreNode *QABSTRACTSCROLLAREA_cornerWidget(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = qasa->getQAbstractScrollArea()->cornerWidget();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QWidget, getProgram());
      rv_obj->setPrivate(CID_QWIDGET, new QoreQtQWidget(rv_obj, qt_qobj));
   }
   return rv_obj;
}

//QScrollBar * horizontalScrollBar () const
static AbstractQoreNode *QABSTRACTSCROLLAREA_horizontalScrollBar(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   QScrollBar *qt_qobj = qasa->getQAbstractScrollArea()->horizontalScrollBar();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QScrollBar, getProgram());
      rv_obj->setPrivate(CID_QSCROLLBAR, new QoreQtQScrollBar(rv_obj, qt_qobj));
   }
   return rv_obj;
}

//Qt::ScrollBarPolicy horizontalScrollBarPolicy () const
static AbstractQoreNode *QABSTRACTSCROLLAREA_horizontalScrollBarPolicy(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qasa->getQAbstractScrollArea()->horizontalScrollBarPolicy());
}

//QSize maximumViewportSize () const
static AbstractQoreNode *QABSTRACTSCROLLAREA_maximumViewportSize(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qasa->getQAbstractScrollArea()->maximumViewportSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

////QWidgetQoreListNode scrollBarWidgets ( Qt::Alignment alignment )
//static AbstractQoreNode *QABSTRACTSCROLLAREA_scrollBarWidgets(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
//   ??? return new QoreBigIntNode(qasa->getQAbstractScrollArea()->scrollBarWidgets(alignment));
//}

//void setCornerWidget ( QWidget * widget )
static AbstractQoreNode *QABSTRACTSCROLLAREA_setCornerWidget(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQWidget *widget = p ? (QoreQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTSCROLLAREA-SETCORNERWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QAbstractScrollArea::setCornerWidget()");
      return 0;
   }
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   qasa->getQAbstractScrollArea()->setCornerWidget(static_cast<QWidget *>(widget->getQWidget()));
   return 0;
}

//void setHorizontalScrollBar ( QScrollBar * scrollBar )
static AbstractQoreNode *QABSTRACTSCROLLAREA_setHorizontalScrollBar(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQScrollBar *scrollBar = p ? (QoreQScrollBar *)p->getReferencedPrivateData(CID_QSCROLLBAR, xsink) : 0;
   if (!scrollBar) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTSCROLLAREA-SETHORIZONTALSCROLLBAR-PARAM-ERROR", "expecting a QScrollBar object as first argument to QAbstractScrollArea::setHorizontalScrollBar()");
      return 0;
   }
   ReferenceHolder<QoreQScrollBar> scrollBarHolder(scrollBar, xsink);
   qasa->getQAbstractScrollArea()->setHorizontalScrollBar(static_cast<QScrollBar *>(scrollBar->qobj));
   return 0;
}

//void setHorizontalScrollBarPolicy ( Qt::ScrollBarPolicy )
static AbstractQoreNode *QABSTRACTSCROLLAREA_setHorizontalScrollBarPolicy(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::ScrollBarPolicy scrollbarpolicy = (Qt::ScrollBarPolicy)(p ? p->getAsInt() : 0);
   qasa->getQAbstractScrollArea()->setHorizontalScrollBarPolicy(scrollbarpolicy);
   return 0;
}

//void setVerticalScrollBar ( QScrollBar * scrollBar )
static AbstractQoreNode *QABSTRACTSCROLLAREA_setVerticalScrollBar(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQScrollBar *scrollBar = p ? (QoreQScrollBar *)p->getReferencedPrivateData(CID_QSCROLLBAR, xsink) : 0;
   if (!scrollBar) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTSCROLLAREA-SETVERTICALSCROLLBAR-PARAM-ERROR", "expecting a QScrollBar object as first argument to QAbstractScrollArea::setVerticalScrollBar()");
      return 0;
   }
   ReferenceHolder<QoreQScrollBar> scrollBarHolder(scrollBar, xsink);
   qasa->getQAbstractScrollArea()->setVerticalScrollBar(static_cast<QScrollBar *>(scrollBar->qobj));
   return 0;
}

//void setVerticalScrollBarPolicy ( Qt::ScrollBarPolicy )
static AbstractQoreNode *QABSTRACTSCROLLAREA_setVerticalScrollBarPolicy(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::ScrollBarPolicy scrollbarpolicy = (Qt::ScrollBarPolicy)(p ? p->getAsInt() : 0);
   qasa->getQAbstractScrollArea()->setVerticalScrollBarPolicy(scrollbarpolicy);
   return 0;
}

//void setViewport ( QWidget * widget )
static AbstractQoreNode *QABSTRACTSCROLLAREA_setViewport(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQWidget *widget = p ? (QoreQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTSCROLLAREA-SETVIEWPORT-PARAM-ERROR", "expecting a QWidget object as first argument to QAbstractScrollArea::setViewport()");
      return 0;
   }
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   qasa->getQAbstractScrollArea()->setViewport(static_cast<QWidget *>(widget->getQWidget()));
   return 0;
}

//QScrollBar * verticalScrollBar () const
static AbstractQoreNode *QABSTRACTSCROLLAREA_verticalScrollBar(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   QScrollBar *qt_qobj = qasa->getQAbstractScrollArea()->verticalScrollBar();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QScrollBar, getProgram());
      rv_obj->setPrivate(CID_QSCROLLBAR, new QoreQtQScrollBar(rv_obj, qt_qobj));
   }
   return rv_obj;
}

//Qt::ScrollBarPolicy verticalScrollBarPolicy () const
static AbstractQoreNode *QABSTRACTSCROLLAREA_verticalScrollBarPolicy(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qasa->getQAbstractScrollArea()->verticalScrollBarPolicy());
}

//QWidget * viewport () const
static AbstractQoreNode *QABSTRACTSCROLLAREA_viewport(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = qasa->getQAbstractScrollArea()->viewport();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QWidget, getProgram());
      rv_obj->setPrivate(CID_QWIDGET, new QoreQtQWidget(rv_obj, qt_qobj));
   }
   return rv_obj;
}

//void setupViewport ( QWidget * viewport )
static AbstractQoreNode *QABSTRACTSCROLLAREA_setupViewport(QoreObject *self, QoreAbstractQAbstractScrollArea *qasa, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQWidget *viewport = p ? (QoreQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!viewport) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTSCROLLAREA-SETUPVIEWPORT-PARAM-ERROR", "expecting a QWidget object as first argument to QAbstractScrollArea::setupViewport()");
      return 0;
   }
   ReferenceHolder<QoreQWidget> viewportHolder(viewport, xsink);
   qasa->setupViewport(static_cast<QWidget *>(viewport->getQWidget()));
   return 0;
}

QoreClass *initQAbstractScrollAreaClass(QoreClass *qframe)
{
   QC_QAbstractScrollArea = new QoreClass("QAbstractScrollArea", QDOM_GUI);
   CID_QABSTRACTSCROLLAREA = QC_QAbstractScrollArea->getID();

   QC_QAbstractScrollArea->addBuiltinVirtualBaseClass(qframe);

   QC_QAbstractScrollArea->setConstructor(QABSTRACTSCROLLAREA_constructor);
   QC_QAbstractScrollArea->setCopy((q_copy_t)QABSTRACTSCROLLAREA_copy);

   QC_QAbstractScrollArea->addMethod("addScrollBarWidget",          (q_method_t)QABSTRACTSCROLLAREA_addScrollBarWidget);
   QC_QAbstractScrollArea->addMethod("cornerWidget",                (q_method_t)QABSTRACTSCROLLAREA_cornerWidget);
   QC_QAbstractScrollArea->addMethod("horizontalScrollBar",         (q_method_t)QABSTRACTSCROLLAREA_horizontalScrollBar);
   QC_QAbstractScrollArea->addMethod("horizontalScrollBarPolicy",   (q_method_t)QABSTRACTSCROLLAREA_horizontalScrollBarPolicy);
   QC_QAbstractScrollArea->addMethod("maximumViewportSize",         (q_method_t)QABSTRACTSCROLLAREA_maximumViewportSize);
   //QC_QAbstractScrollArea->addMethod("scrollBarWidgets",            (q_method_t)QABSTRACTSCROLLAREA_scrollBarWidgets);
   QC_QAbstractScrollArea->addMethod("setCornerWidget",             (q_method_t)QABSTRACTSCROLLAREA_setCornerWidget);
   QC_QAbstractScrollArea->addMethod("setHorizontalScrollBar",      (q_method_t)QABSTRACTSCROLLAREA_setHorizontalScrollBar);
   QC_QAbstractScrollArea->addMethod("setHorizontalScrollBarPolicy", (q_method_t)QABSTRACTSCROLLAREA_setHorizontalScrollBarPolicy);
   QC_QAbstractScrollArea->addMethod("setVerticalScrollBar",        (q_method_t)QABSTRACTSCROLLAREA_setVerticalScrollBar);
   QC_QAbstractScrollArea->addMethod("setVerticalScrollBarPolicy",  (q_method_t)QABSTRACTSCROLLAREA_setVerticalScrollBarPolicy);
   QC_QAbstractScrollArea->addMethod("setViewport",                 (q_method_t)QABSTRACTSCROLLAREA_setViewport);
   QC_QAbstractScrollArea->addMethod("verticalScrollBar",           (q_method_t)QABSTRACTSCROLLAREA_verticalScrollBar);
   QC_QAbstractScrollArea->addMethod("verticalScrollBarPolicy",     (q_method_t)QABSTRACTSCROLLAREA_verticalScrollBarPolicy);
   QC_QAbstractScrollArea->addMethod("viewport",                    (q_method_t)QABSTRACTSCROLLAREA_viewport);

   // private members
   QC_QAbstractScrollArea->addMethod("setupViewport",               (q_method_t)QABSTRACTSCROLLAREA_setupViewport);

   return QC_QAbstractScrollArea;
}
