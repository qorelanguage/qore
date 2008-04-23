/*
 QC_QSplitter.cc
 
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

#include "QC_QSplitter.h"

qore_classid_t CID_QSPLITTER;
QoreClass *QC_QSplitter = 0;

//QSplitter ( QWidget * parent = 0 )
//QSplitter ( Qt::Orientation orientation, QWidget * parent = 0 )
static void QSPLITTER_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QSPLITTER, new QoreQSplitter(self));
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQWidget *parent = (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QSPLITTER, new QoreQSplitter(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
      return;
   }
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQWidget *parent = (p && p->getType() == NT_OBJECT) ? (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QSPLITTER, new QoreQSplitter(self, orientation, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QSPLITTER_copy(QoreObject *self, QoreObject *old, QoreQSplitter *qs, ExceptionSink *xsink)
{
   xsink->raiseException("QSPLITTER-COPY-ERROR", "objects of this class cannot be copied");
}

//void addWidget ( QWidget * widget )
static AbstractQoreNode *QSPLITTER_addWidget(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->getType() == NT_OBJECT) ? (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSPLITTER-ADDWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QSplitter::addWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qs->getQSplitter()->addWidget(static_cast<QWidget *>(widget->getQWidget()));
   return 0;
}

//bool childrenCollapsible () const
static AbstractQoreNode *QSPLITTER_childrenCollapsible(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qs->getQSplitter()->childrenCollapsible());
}

//int count () const
static AbstractQoreNode *QSPLITTER_count(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qs->getQSplitter()->count());
}

//void getRange ( int index, int * min, int * max ) const
static AbstractQoreNode *QSPLITTER_getRange(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;

   const ReferenceNode *rmin = test_reference_param(params, 1);
   if (!rmin) {
      xsink->raiseException("QSPLITTER-GETRANGE-ERROR", "expecting an lvalue reference as the second argument to QSplitter::getRange()");
      return 0;
   }

   AutoVLock vl(xsink);
   ReferenceHelper rhmin(rmin, vl, xsink);
   if (!rhmin)
      return 0;

   const ReferenceNode *rmax = test_reference_param(params, 2);
   if (!rmin) {
      xsink->raiseException("QSPLITTER-GETRANGE-ERROR", "expecting an lvalue reference as the second argument to QSplitter::getRange()");
      return 0;
   }

   ReferenceHelper rhmax(rmax, vl, xsink);
   if (!rhmax)
      return 0;

   int min, max;
   
   qs->getQSplitter()->getRange(index, &min, &max);

   if (rhmin.assign(new QoreBigIntNode(min), xsink))
      return 0;

   rhmax.assign(new QoreBigIntNode(max), xsink);
   return 0;
}

//QSplitterHandle * handle ( int index ) const
static AbstractQoreNode *QSPLITTER_handle(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   QSplitterHandle *qt_qobj = qs->getQSplitter()->handle(index);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      return rv_obj->refSelf();
   rv_obj = new QoreObject(QC_QSplitterHandle, getProgram());
   QoreQtQSplitterHandle *t_qobj = new QoreQtQSplitterHandle(rv_obj, qt_qobj);
   rv_obj->setPrivate(CID_QSPLITTERHANDLE, t_qobj);
   return rv_obj;
}

//int handleWidth () const
static AbstractQoreNode *QSPLITTER_handleWidth(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qs->getQSplitter()->handleWidth());
}

//int indexOf ( QWidget * widget ) const
static AbstractQoreNode *QSPLITTER_indexOf(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->getType() == NT_OBJECT) ? (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSPLITTER-INDEXOF-PARAM-ERROR", "expecting a QWidget object as first argument to QSplitter::indexOf()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreBigIntNode(qs->getQSplitter()->indexOf(static_cast<QWidget *>(widget->getQWidget())));
}

//void insertWidget ( int index, QWidget * widget )
static AbstractQoreNode *QSPLITTER_insertWidget(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQWidget *widget = (p && p->getType() == NT_OBJECT) ? (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSPLITTER-INSERTWIDGET-PARAM-ERROR", "expecting a QWidget object as second argument to QSplitter::insertWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qs->getQSplitter()->insertWidget(index, static_cast<QWidget *>(widget->getQWidget()));
   return 0;
}

//bool isCollapsible ( int index ) const
static AbstractQoreNode *QSPLITTER_isCollapsible(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return get_bool_node(qs->getQSplitter()->isCollapsible(index));
}

//bool opaqueResize () const
static AbstractQoreNode *QSPLITTER_opaqueResize(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qs->getQSplitter()->opaqueResize());
}

//Qt::Orientation orientation () const
static AbstractQoreNode *QSPLITTER_orientation(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qs->getQSplitter()->orientation());
}

//void refresh ()
static AbstractQoreNode *QSPLITTER_refresh(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   qs->getQSplitter()->refresh();
   return 0;
}

//bool restoreState ( const QByteArray & state )
static AbstractQoreNode *QSPLITTER_restoreState(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QByteArray state;
   if (get_qbytearray(p, state, xsink))
      return 0;
   return get_bool_node(qs->getQSplitter()->restoreState(state));
}

//QByteArray saveState () const
static AbstractQoreNode *QSPLITTER_saveState(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QByteArray, new QoreQByteArray(qs->getQSplitter()->saveState()));
}

//void setChildrenCollapsible ( bool )
static AbstractQoreNode *QSPLITTER_setChildrenCollapsible(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qs->getQSplitter()->setChildrenCollapsible(b);
   return 0;
}

//void setCollapsible ( int index, bool collapse )
static AbstractQoreNode *QSPLITTER_setCollapsible(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   bool collapse = p ? p->getAsBool() : false;
   qs->getQSplitter()->setCollapsible(index, collapse);
   return 0;
}

//void setHandleWidth ( int )
static AbstractQoreNode *QSPLITTER_setHandleWidth(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qs->getQSplitter()->setHandleWidth(x);
   return 0;
}

//void setOpaqueResize ( bool opaque = true )
static AbstractQoreNode *QSPLITTER_setOpaqueResize(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool opaque = !is_nothing(p) ? p->getAsBool() : true;
   qs->getQSplitter()->setOpaqueResize(opaque);
   return 0;
}

//void setOrientation ( Qt::Orientation )
static AbstractQoreNode *QSPLITTER_setOrientation(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   qs->getQSplitter()->setOrientation(orientation);
   return 0;
}

//void setSizes ( const QList<int> & list )
static AbstractQoreNode *QSPLITTER_setSizes(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_LIST) {
      xsink->raiseException("QSPLITTER-SETSIZES-PARAM-ERROR", "expecting a list as first argument to QSplitter::setSizes()");
      return 0;
   }
   QList<int> list;
   ConstListIterator li_list(reinterpret_cast<const QoreListNode *>(p));
   while (li_list.next()) {
      const AbstractQoreNode *n = li_list.getValue();
      list.push_back(n ? n->getAsInt() : 0);
   }
   qs->getQSplitter()->setSizes(list);
   return 0;
}

//void setStretchFactor ( int index, int stretch )
static AbstractQoreNode *QSPLITTER_setStretchFactor(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int stretch = p ? p->getAsInt() : 0;
   qs->getQSplitter()->setStretchFactor(index, stretch);
   return 0;
}

//QList<int> sizes () const
static AbstractQoreNode *QSPLITTER_sizes(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   QList<int> ilist_rv = qs->getQSplitter()->sizes();
   QoreListNode *l = new QoreListNode();
   for (QList<int>::iterator i = ilist_rv.begin(), e = ilist_rv.end(); i != e; ++i)
      l->push(new QoreBigIntNode((*i)));
   return l;
}

//QWidget * widget ( int index ) const
static AbstractQoreNode *QSPLITTER_widget(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return return_qwidget(qs->getQSplitter()->widget(index));
}

//int closestLegalPosition ( int pos, int index )
static AbstractQoreNode *QSPLITTER_closestLegalPosition(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int pos = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int index = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qs->closestLegalPosition(pos, index));
}

//virtual QSplitterHandle * createHandle ()
static AbstractQoreNode *QSPLITTER_createHandle(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   QSplitterHandle *qt_qobj = qs->createHandle();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      return rv_obj->refSelf();
   rv_obj = new QoreObject(QC_QSplitterHandle, getProgram());
   QoreQtQSplitterHandle *t_qobj = new QoreQtQSplitterHandle(rv_obj, qt_qobj);
   rv_obj->setPrivate(CID_QSPLITTERHANDLE, t_qobj);
   return rv_obj;
}

//void moveSplitter ( int pos, int index )
static AbstractQoreNode *QSPLITTER_moveSplitter(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int pos = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int index = p ? p->getAsInt() : 0;
   qs->moveSplitter(pos, index);
   return 0;
}

//void setRubberBand ( int pos )
static AbstractQoreNode *QSPLITTER_setRubberBand(QoreObject *self, QoreAbstractQSplitter *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int pos = p ? p->getAsInt() : 0;
   qs->setRubberBand(pos);
   return 0;
}

QoreClass *initQSplitterClass(QoreClass *qwidget)
{
   QC_QSplitter = new QoreClass("QSplitter", QDOM_GUI);
   CID_QSPLITTER = QC_QSplitter->getID();

   QC_QSplitter->addBuiltinVirtualBaseClass(qwidget);

   QC_QSplitter->setConstructor(QSPLITTER_constructor);
   QC_QSplitter->setCopy((q_copy_t)QSPLITTER_copy);

   QC_QSplitter->addMethod("addWidget",                   (q_method_t)QSPLITTER_addWidget);
   QC_QSplitter->addMethod("childrenCollapsible",         (q_method_t)QSPLITTER_childrenCollapsible);
   QC_QSplitter->addMethod("count",                       (q_method_t)QSPLITTER_count);
   QC_QSplitter->addMethod("getRange",                    (q_method_t)QSPLITTER_getRange);
   QC_QSplitter->addMethod("handle",                      (q_method_t)QSPLITTER_handle);
   QC_QSplitter->addMethod("handleWidth",                 (q_method_t)QSPLITTER_handleWidth);
   QC_QSplitter->addMethod("indexOf",                     (q_method_t)QSPLITTER_indexOf);
   QC_QSplitter->addMethod("insertWidget",                (q_method_t)QSPLITTER_insertWidget);
   QC_QSplitter->addMethod("isCollapsible",               (q_method_t)QSPLITTER_isCollapsible);
   QC_QSplitter->addMethod("opaqueResize",                (q_method_t)QSPLITTER_opaqueResize);
   QC_QSplitter->addMethod("orientation",                 (q_method_t)QSPLITTER_orientation);
   QC_QSplitter->addMethod("refresh",                     (q_method_t)QSPLITTER_refresh);
   QC_QSplitter->addMethod("restoreState",                (q_method_t)QSPLITTER_restoreState);
   QC_QSplitter->addMethod("saveState",                   (q_method_t)QSPLITTER_saveState);
   QC_QSplitter->addMethod("setChildrenCollapsible",      (q_method_t)QSPLITTER_setChildrenCollapsible);
   QC_QSplitter->addMethod("setCollapsible",              (q_method_t)QSPLITTER_setCollapsible);
   QC_QSplitter->addMethod("setHandleWidth",              (q_method_t)QSPLITTER_setHandleWidth);
   QC_QSplitter->addMethod("setOpaqueResize",             (q_method_t)QSPLITTER_setOpaqueResize);
   QC_QSplitter->addMethod("setOrientation",              (q_method_t)QSPLITTER_setOrientation);
   QC_QSplitter->addMethod("setSizes",                    (q_method_t)QSPLITTER_setSizes);
   QC_QSplitter->addMethod("setStretchFactor",            (q_method_t)QSPLITTER_setStretchFactor);
   QC_QSplitter->addMethod("sizes",                       (q_method_t)QSPLITTER_sizes);
   QC_QSplitter->addMethod("widget",                      (q_method_t)QSPLITTER_widget);
   QC_QSplitter->addMethod("closestLegalPosition",        (q_method_t)QSPLITTER_closestLegalPosition);
   QC_QSplitter->addMethod("createHandle",                (q_method_t)QSPLITTER_createHandle);
   QC_QSplitter->addMethod("moveSplitter",                (q_method_t)QSPLITTER_moveSplitter);
   QC_QSplitter->addMethod("setRubberBand",               (q_method_t)QSPLITTER_setRubberBand);

   return QC_QSplitter;
}
