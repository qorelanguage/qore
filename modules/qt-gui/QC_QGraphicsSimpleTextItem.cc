/*
 QC_QGraphicsSimpleTextItem.cc
 
  Qore Programming Language

 Copyright 2003 - 2008 David Nichols

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

#include "qore-qt.h"

#include "QC_QGraphicsSimpleTextItem.h"
#include "QC_QGraphicsItem.h"
#include "QC_QFont.h"

qore_classid_t CID_QGRAPHICSSIMPLETEXTITEM;
QoreClass *QC_QGraphicsSimpleTextItem = 0;

//QGraphicsSimpleTextItem ( QGraphicsItem * parent = 0 )
//QGraphicsSimpleTextItem ( const QString & text, QGraphicsItem * parent = 0 )
static void QGRAPHICSSIMPLETEXTITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QGRAPHICSSIMPLETEXTITEM, new QoreQGraphicsSimpleTextItem(self));
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQGraphicsItem *parent = (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink);
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QGRAPHICSSIMPLETEXTITEM, new QoreQGraphicsSimpleTextItem(self, parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
      return;
   }
   QString text;
   if (get_qstring(p, text, xsink))
      return;
   p = get_param(params, 1);
   QoreQGraphicsItem *parent = (p && p->getType() == NT_OBJECT) ? (QoreQGraphicsItem *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGRAPHICSITEM, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QGRAPHICSSIMPLETEXTITEM, new QoreQGraphicsSimpleTextItem(self, text, parent ? static_cast<QGraphicsItem *>(parent->getQGraphicsItem()) : 0));
   return;
}

static void QGRAPHICSSIMPLETEXTITEM_copy(QoreObject *self, QoreObject *old, QoreQGraphicsSimpleTextItem *qgsti, ExceptionSink *xsink)
{
   xsink->raiseException("QGRAPHICSSIMPLETEXTITEM-COPY-ERROR", "objects of this class cannot be copied");
}

//QFont font () const
static AbstractQoreNode *QGRAPHICSSIMPLETEXTITEM_font(QoreObject *self, QoreAbstractQGraphicsSimpleTextItem *qgsti, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QFont, new QoreQFont(qgsti->getQGraphicsSimpleTextItem()->font()));
}

//void setFont ( const QFont & font )
static AbstractQoreNode *QGRAPHICSSIMPLETEXTITEM_setFont(QoreObject *self, QoreAbstractQGraphicsSimpleTextItem *qgsti, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQFont *font = (p && p->getType() == NT_OBJECT) ? (QoreQFont *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QGRAPHICSSIMPLETEXTITEM-SETFONT-PARAM-ERROR", "expecting a QFont object as first argument to QGraphicsSimpleTextItem::setFont()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> fontHolder(static_cast<AbstractPrivateData *>(font), xsink);
   qgsti->getQGraphicsSimpleTextItem()->setFont(*(static_cast<QFont *>(font)));
   return 0;
}

//void setText ( const QString & text )
static AbstractQoreNode *QGRAPHICSSIMPLETEXTITEM_setText(QoreObject *self, QoreAbstractQGraphicsSimpleTextItem *qgsti, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qgsti->getQGraphicsSimpleTextItem()->setText(text);
   return 0;
}

//QString text () const
static AbstractQoreNode *QGRAPHICSSIMPLETEXTITEM_text(QoreObject *self, QoreAbstractQGraphicsSimpleTextItem *qgsti, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qgsti->getQGraphicsSimpleTextItem()->text().toUtf8().data(), QCS_UTF8);
}

QoreClass *initQGraphicsSimpleTextItemClass(QoreClass *qabstractgraphicsshapeitem)
{
   QC_QGraphicsSimpleTextItem = new QoreClass("QGraphicsSimpleTextItem", QDOM_GUI);
   CID_QGRAPHICSSIMPLETEXTITEM = QC_QGraphicsSimpleTextItem->getID();

   QC_QGraphicsSimpleTextItem->addBuiltinVirtualBaseClass(qabstractgraphicsshapeitem);

   QC_QGraphicsSimpleTextItem->setConstructor(QGRAPHICSSIMPLETEXTITEM_constructor);
   QC_QGraphicsSimpleTextItem->setCopy((q_copy_t)QGRAPHICSSIMPLETEXTITEM_copy);

   QC_QGraphicsSimpleTextItem->addMethod("font",                        (q_method_t)QGRAPHICSSIMPLETEXTITEM_font);
   QC_QGraphicsSimpleTextItem->addMethod("setFont",                     (q_method_t)QGRAPHICSSIMPLETEXTITEM_setFont);
   QC_QGraphicsSimpleTextItem->addMethod("setText",                     (q_method_t)QGRAPHICSSIMPLETEXTITEM_setText);
   QC_QGraphicsSimpleTextItem->addMethod("text",                        (q_method_t)QGRAPHICSSIMPLETEXTITEM_text);

   return QC_QGraphicsSimpleTextItem;
}
