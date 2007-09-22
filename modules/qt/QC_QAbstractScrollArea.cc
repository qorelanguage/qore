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

int CID_QABSTRACTSCROLLAREA;
class QoreClass *QC_QAbstractScrollArea = 0;

//QAbstractScrollArea ( QWidget * parent = 0 )
static void QABSTRACTSCROLLAREA_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);
   self->setPrivate(CID_QABSTRACTSCROLLAREA, new QoreQAbstractScrollArea(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QABSTRACTSCROLLAREA_copy(class Object *self, class Object *old, class QoreQAbstractScrollArea *qasa, ExceptionSink *xsink)
{
   xsink->raiseException("QABSTRACTSCROLLAREA-COPY-ERROR", "objects of this class cannot be copied");
}

//void addScrollBarWidget ( QWidget * widget, Qt::Alignment alignment )
static QoreNode *QABSTRACTSCROLLAREA_addScrollBarWidget(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QABSTRACTSCROLLAREA-ADDSCROLLBARWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QAbstractScrollArea::addScrollBarWidget()");
      return 0;
   }
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   p = get_param(params, 1);
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   qasa->getQAbstractScrollArea()->addScrollBarWidget(static_cast<QWidget *>(widget->getQWidget()), alignment);
   return 0;
}

//QWidget * cornerWidget () const
static QoreNode *QABSTRACTSCROLLAREA_cornerWidget(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = qasa->getQAbstractScrollArea()->cornerWidget();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new Object(QC_QWidget, getProgram());
      rv_obj->setPrivate(CID_QWIDGET, new QoreQtQWidget(rv_obj, qt_qobj));
   }
   return new QoreNode(rv_obj);
}

//QScrollBar * horizontalScrollBar () const
static QoreNode *QABSTRACTSCROLLAREA_horizontalScrollBar(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   QScrollBar *qt_qobj = qasa->getQAbstractScrollArea()->horizontalScrollBar();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new Object(QC_QScrollBar, getProgram());
      rv_obj->setPrivate(CID_QSCROLLBAR, new QoreQtQScrollBar(rv_obj, qt_qobj));
   }
   return new QoreNode(rv_obj);
}

//Qt::ScrollBarPolicy horizontalScrollBarPolicy () const
static QoreNode *QABSTRACTSCROLLAREA_horizontalScrollBarPolicy(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qasa->getQAbstractScrollArea()->horizontalScrollBarPolicy());
}

//QSize maximumViewportSize () const
static QoreNode *QABSTRACTSCROLLAREA_maximumViewportSize(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qasa->getQAbstractScrollArea()->maximumViewportSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

////QWidgetList scrollBarWidgets ( Qt::Alignment alignment )
//static QoreNode *QABSTRACTSCROLLAREA_scrollBarWidgets(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
//   ??? return new QoreNode((int64)qasa->getQAbstractScrollArea()->scrollBarWidgets(alignment));
//}

//void setCornerWidget ( QWidget * widget )
static QoreNode *QABSTRACTSCROLLAREA_setCornerWidget(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
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
static QoreNode *QABSTRACTSCROLLAREA_setHorizontalScrollBar(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQScrollBar *scrollBar = (p && p->type == NT_OBJECT) ? (QoreQScrollBar *)p->val.object->getReferencedPrivateData(CID_QSCROLLBAR, xsink) : 0;
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
static QoreNode *QABSTRACTSCROLLAREA_setHorizontalScrollBarPolicy(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ScrollBarPolicy scrollbarpolicy = (Qt::ScrollBarPolicy)(p ? p->getAsInt() : 0);
   qasa->getQAbstractScrollArea()->setHorizontalScrollBarPolicy(scrollbarpolicy);
   return 0;
}

//void setVerticalScrollBar ( QScrollBar * scrollBar )
static QoreNode *QABSTRACTSCROLLAREA_setVerticalScrollBar(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQScrollBar *scrollBar = (p && p->type == NT_OBJECT) ? (QoreQScrollBar *)p->val.object->getReferencedPrivateData(CID_QSCROLLBAR, xsink) : 0;
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
static QoreNode *QABSTRACTSCROLLAREA_setVerticalScrollBarPolicy(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ScrollBarPolicy scrollbarpolicy = (Qt::ScrollBarPolicy)(p ? p->getAsInt() : 0);
   qasa->getQAbstractScrollArea()->setVerticalScrollBarPolicy(scrollbarpolicy);
   return 0;
}

//void setViewport ( QWidget * widget )
static QoreNode *QABSTRACTSCROLLAREA_setViewport(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
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
static QoreNode *QABSTRACTSCROLLAREA_verticalScrollBar(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   QScrollBar *qt_qobj = qasa->getQAbstractScrollArea()->verticalScrollBar();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new Object(QC_QScrollBar, getProgram());
      rv_obj->setPrivate(CID_QSCROLLBAR, new QoreQtQScrollBar(rv_obj, qt_qobj));
   }
   return new QoreNode(rv_obj);
}

//Qt::ScrollBarPolicy verticalScrollBarPolicy () const
static QoreNode *QABSTRACTSCROLLAREA_verticalScrollBarPolicy(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qasa->getQAbstractScrollArea()->verticalScrollBarPolicy());
}

//QWidget * viewport () const
static QoreNode *QABSTRACTSCROLLAREA_viewport(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = qasa->getQAbstractScrollArea()->viewport();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   Object *rv_obj = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new Object(QC_QWidget, getProgram());
      rv_obj->setPrivate(CID_QWIDGET, new QoreQtQWidget(rv_obj, qt_qobj));
   }
   return new QoreNode(rv_obj);
}

//void setupViewport ( QWidget * viewport )
static QoreNode *QABSTRACTSCROLLAREA_setupViewport(Object *self, QoreAbstractQAbstractScrollArea *qasa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *viewport = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
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
