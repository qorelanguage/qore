/*
 QC_QLayout.cc
 
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

#include "QC_QLayout.h"
#include "QC_QRect.h"

qore_classid_t CID_QLAYOUT;
QoreClass *QC_QLayout = 0;

//QLayout ( QWidget * parent )
//QLayout ()
static void QLAYOUT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QLAYOUT, new QoreQLayout(self));
      return;
   }
   QoreQWidget *parent = (p && p->getType() == NT_OBJECT) ? (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QLAYOUT-CONSTRUCTOR-PARAM-ERROR", "this version of QLayout::constructor() expects an object derived from QWidget as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
      return;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QLAYOUT, new QoreQLayout(self, parent->getQWidget()));
   return;
}

static void QLAYOUT_copy(QoreObject *self, QoreObject *old, QoreQLayout *ql, ExceptionSink *xsink)
{
   xsink->raiseException("QLAYOUT-COPY-ERROR", "objects of this class cannot be copied");
}

//bool activate ()
static AbstractQoreNode *QLAYOUT_activate(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(ql->getQLayout()->activate());
}

//virtual void addItem ( QLayoutItem * item ) = 0
static AbstractQoreNode *QLAYOUT_addItem(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreAbstractQLayoutItemData *item = (p && p->getType() == NT_OBJECT) ? (QoreQLayoutItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QLAYOUTITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QLAYOUT-ADDITEM-PARAM-ERROR", "expecting a QLayoutItem object as first argument to QLayout::addItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   ql->addItem(item->getQLayoutItem());
   item->setItemExternallyOwned();
   return 0;
}

//void addWidget ( QWidget * w )
static AbstractQoreNode *QLAYOUT_addWidget(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreAbstractQWidget *w = p ? (QoreAbstractQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!w) {
      if (!xsink->isException())
         xsink->raiseException("QLAYOUT-ADDWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QLayout::addWidget()");
      return 0;
   }

   ReferenceHolder<QoreAbstractQWidget> wHolder(w, xsink);
   ql->getQLayout()->addWidget(w->getQWidget());
   // the widget pointer is now owned by the layout
   w->setExternallyOwned();
   return 0;
}

//QRect contentsRect () const
static AbstractQoreNode *QLAYOUT_contentsRect(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(ql->getQLayout()->contentsRect());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//virtual int count () const = 0
static AbstractQoreNode *QLAYOUT_count(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->count());
}

//void getContentsMargins ( int * left, int * top, int * right, int * bottom ) const
static AbstractQoreNode *QLAYOUT_getContentsMargins(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   int left, top, right, bottom;
   ql->getQLayout()->getContentsMargins(&left, &top, &right, &bottom);
   
   QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("left", new QoreBigIntNode(left), xsink);
   h->setKeyValue("top", new QoreBigIntNode(top), xsink);
   h->setKeyValue("right", new QoreBigIntNode(right), xsink);
   h->setKeyValue("bottom", new QoreBigIntNode(bottom), xsink);

   return h;
}

//virtual int indexOf ( QWidget * widget ) const
static AbstractQoreNode *QLAYOUT_indexOf(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreAbstractQWidget *widget = p ? (QoreAbstractQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QLAYOUT-INDEXOF-PARAM-ERROR", "expecting a QWidget object as first argument to QLayout::indexOf()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> widgetHolder(widget, xsink);
   return new QoreBigIntNode(ql->indexOf(widget->getQWidget()));
}

//bool isEnabled () const
static AbstractQoreNode *QLAYOUT_isEnabled(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(ql->getQLayout()->isEnabled());
}

//virtual QLayoutItem * itemAt ( int index ) const = 0
static AbstractQoreNode *QLAYOUT_itemAt(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;

   QLayoutItem *qloi = ql->itemAt(index);
   if (!qloi)
      return 0;

   return return_object(QC_QLayoutItem, new QoreQtQLayoutItem(qloi, false));
}

//QWidget * menuBar () const
static AbstractQoreNode *QLAYOUT_menuBar(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_qwidget(ql->getQLayout()->menuBar());
}

//QWidget * parentWidget () const
static AbstractQoreNode *QLAYOUT_parentWidget(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_qwidget(ql->getQLayout()->parentWidget());
}

//void removeItem ( QLayoutItem * item )
static AbstractQoreNode *QLAYOUT_removeItem(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQLayoutItem *item = (p && p->getType() == NT_OBJECT) ? (QoreQLayoutItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QLAYOUTITEM, xsink) : 0;
   if (!item) {
      if (!xsink->isException())
         xsink->raiseException("QLAYOUT-REMOVEITEM-PARAM-ERROR", "expecting a QLayoutItem object as first argument to QLayout::removeItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> itemHolder(static_cast<AbstractPrivateData *>(item), xsink);
   ql->getQLayout()->removeItem(static_cast<QLayoutItem *>(item));
   return 0;
}

//void removeWidget ( QWidget * widget )
static AbstractQoreNode *QLAYOUT_removeWidget(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreAbstractQWidget *widget = p ? (QoreAbstractQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QLAYOUT-REMOVEWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QLayout::removeWidget()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> widgetHolder(widget, xsink);
   ql->getQLayout()->removeWidget(widget->getQWidget());
   return 0;
}

//bool setAlignment ( QWidget * w, Qt::Alignment alignment )
//bool setAlignment ( QLayout * l, Qt::Alignment alignment )
static AbstractQoreNode *QLAYOUT_setAlignment(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreAbstractQLayout *l = o ? (QoreAbstractQLayout *)o->getReferencedPrivateData(CID_QLAYOUT, xsink) : 0;
   if (!l) {
      QoreAbstractQWidget *w = o ? (QoreAbstractQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
      if (!w) {
	 if (!xsink->isException())
	    xsink->raiseException("QLAYOUT-SETALIGNMENT-PARAM-ERROR", "QLayout::setAlignment() expecting a QWidget or QLayout as first argument");
	 return 0;
      }
      ReferenceHolder<QoreAbstractQWidget> wHolder(w, xsink);
      const AbstractQoreNode *p = get_param(params, 1);
      Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
      return get_bool_node(ql->getQLayout()->setAlignment(w->getQWidget(), alignment));
   }
   ReferenceHolder<QoreAbstractQLayout> lHolder(l, xsink);
   const AbstractQoreNode *p = get_param(params, 1);
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   return get_bool_node(ql->getQLayout()->setAlignment(l->getQLayout(), alignment));
}

//void setContentsMargins ( int left, int top, int right, int bottom )
static AbstractQoreNode *QLAYOUT_setContentsMargins(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int left = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int top = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int right = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int bottom = p ? p->getAsInt() : 0;
   ql->getQLayout()->setContentsMargins(left, top, right, bottom);
   return 0;
}

//void setEnabled ( bool enable )
static AbstractQoreNode *QLAYOUT_setEnabled(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   ql->getQLayout()->setEnabled(enable);
   return 0;
}

//void setMenuBar ( QWidget * widget )
static AbstractQoreNode *QLAYOUT_setMenuBar(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreAbstractQWidget *widget = p ? (QoreAbstractQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QLAYOUT-SETMENUBAR-PARAM-ERROR", "expecting a QWidget object as first argument to QLayout::setMenuBar()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> widgetHolder(widget, xsink);
   ql->getQLayout()->setMenuBar(widget->getQWidget());
   return 0;
}

//void setSizeConstraint ( SizeConstraint )
static AbstractQoreNode *QLAYOUT_setSizeConstraint(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QLayout::SizeConstraint sizeconstraint = (QLayout::SizeConstraint)(p ? p->getAsInt() : 0);
   ql->getQLayout()->setSizeConstraint(sizeconstraint);
   return 0;
}

//void setSpacing ( int )
static AbstractQoreNode *QLAYOUT_setSpacing(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   ql->getQLayout()->setSpacing(x);
   return 0;
}

//SizeConstraint sizeConstraint () const
static AbstractQoreNode *QLAYOUT_sizeConstraint(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->getQLayout()->sizeConstraint());
}

//int spacing () const
static AbstractQoreNode *QLAYOUT_spacing(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(ql->getQLayout()->spacing());
}

//virtual QLayoutItem * takeAt ( int index ) = 0
static AbstractQoreNode *QLAYOUT_takeAt(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;

   QLayoutItem *qloi = ql->takeAt(index);
   if (!qloi)
      return 0;

   return return_object(QC_QLayoutItem, new QoreQtQLayoutItem(qloi));
}

//void update ()
static AbstractQoreNode *QLAYOUT_update(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   ql->getQLayout()->update();
   return 0;
}

//void setMargin (int);
static AbstractQoreNode *QLAYOUT_setMargin(QoreObject *self, QoreAbstractQLayout *ql, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int margin = p ? p->getAsInt() : 0;
   ql->getQLayout()->setMargin(margin);

   return 0;
}

QoreClass *initQLayoutClass(QoreClass *qobject, QoreClass *qlayoutitem)
{
   tracein("initQLayoutClass()");
   
   QC_QLayout = new QoreClass("QLayout", QDOM_GUI);
   CID_QLAYOUT = QC_QLayout->getID();

   QC_QLayout->addBuiltinVirtualBaseClass(qobject);
   QC_QLayout->addBuiltinVirtualBaseClass(qlayoutitem);

   QC_QLayout->setConstructor(QLAYOUT_constructor);
   QC_QLayout->setCopy((q_copy_t)QLAYOUT_copy);

   QC_QLayout->addMethod("activate",                    (q_method_t)QLAYOUT_activate);
   QC_QLayout->addMethod("addItem",                     (q_method_t)QLAYOUT_addItem);
   QC_QLayout->addMethod("addWidget",                   (q_method_t)QLAYOUT_addWidget);
   QC_QLayout->addMethod("contentsRect",                (q_method_t)QLAYOUT_contentsRect);
   QC_QLayout->addMethod("count",                       (q_method_t)QLAYOUT_count);
   QC_QLayout->addMethod("getContentsMargins",          (q_method_t)QLAYOUT_getContentsMargins);
   QC_QLayout->addMethod("indexOf",                     (q_method_t)QLAYOUT_indexOf);
   QC_QLayout->addMethod("isEnabled",                   (q_method_t)QLAYOUT_isEnabled);
   QC_QLayout->addMethod("itemAt",                      (q_method_t)QLAYOUT_itemAt);
   QC_QLayout->addMethod("menuBar",                     (q_method_t)QLAYOUT_menuBar);
   QC_QLayout->addMethod("parentWidget",                (q_method_t)QLAYOUT_parentWidget);
   QC_QLayout->addMethod("removeItem",                  (q_method_t)QLAYOUT_removeItem);
   QC_QLayout->addMethod("removeWidget",                (q_method_t)QLAYOUT_removeWidget);
   QC_QLayout->addMethod("setAlignment",                (q_method_t)QLAYOUT_setAlignment);
   QC_QLayout->addMethod("setContentsMargins",          (q_method_t)QLAYOUT_setContentsMargins);
   QC_QLayout->addMethod("setEnabled",                  (q_method_t)QLAYOUT_setEnabled);
   QC_QLayout->addMethod("setMargin",                   (q_method_t)QLAYOUT_setMargin);
   QC_QLayout->addMethod("setMenuBar",                  (q_method_t)QLAYOUT_setMenuBar);
   QC_QLayout->addMethod("setSizeConstraint",           (q_method_t)QLAYOUT_setSizeConstraint);
   QC_QLayout->addMethod("setSpacing",                  (q_method_t)QLAYOUT_setSpacing);
   QC_QLayout->addMethod("sizeConstraint",              (q_method_t)QLAYOUT_sizeConstraint);
   QC_QLayout->addMethod("spacing",                     (q_method_t)QLAYOUT_spacing);
   QC_QLayout->addMethod("takeAt",                      (q_method_t)QLAYOUT_takeAt);
   QC_QLayout->addMethod("update",                      (q_method_t)QLAYOUT_update);


   traceout("initQLayoutClass()");
   return QC_QLayout;
}
