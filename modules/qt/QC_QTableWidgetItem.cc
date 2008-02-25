/*
 QC_QTableWidgetItem.cc
 
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

#include "QC_QTableWidgetItem.h"
#include "QC_QWidget.h"
#include "QC_QIcon.h"
#include "QC_QBrush.h"
#include "QC_QFont.h"
#include "QC_QSize.h"

#include "qore-qt.h"

int CID_QTABLEWIDGETITEM;
class QoreClass *QC_QTableWidgetItem = 0;

//QTableWidgetItem ( int type = Type )
//QTableWidgetItem ( const QString & text, int type = Type )
//QTableWidgetItem ( const QIcon & icon, const QString & text, int type = Type )
//QTableWidgetItem ( const QTableWidgetItem & other )
static void QTABLEWIDGETITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   //printd(0, "QTableWidgetItem() self=%08p, p=%s\n", self, p ? p->getTypeName() : "(null)");
   if (p && p->getType() == NT_OBJECT) {
      QoreQIcon *icon = (QoreQIcon *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QICON, xsink);
      if (!icon) {
         if (!xsink->isException())
            xsink->raiseException("QTABLEWIDGETITEM-CONSTRUCTOR-PARAM-ERROR", "QTableWidgetItem::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return;
      }
      ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
      p = get_param(params, 1);
      QString text;
      if (get_qstring(p, text, xsink))
         return;
      p = get_param(params, 2);
      int type = !is_nothing(p) ? p->getAsInt() : QTableWidgetItem::Type;
      self->setPrivate(CID_QTABLEWIDGETITEM, new QoreQTableWidgetItem(*(static_cast<QIcon *>(icon)), text, type));
      return;
   }
   if (p && p->getType() == NT_STRING) {
      QString text;
      if (get_qstring(p, text, xsink))
         return;
      p = get_param(params, 1);
      int type = !is_nothing(p) ? p->getAsInt() : QTableWidgetItem::Type;
      self->setPrivate(CID_QTABLEWIDGETITEM, new QoreQTableWidgetItem(text, type));
      return;
   }
   int type = !is_nothing(p) ? p->getAsInt() : QTableWidgetItem::Type;
   self->setPrivate(CID_QTABLEWIDGETITEM, new QoreQTableWidgetItem(type));
   return;
}

static void QTABLEWIDGETITEM_copy(class QoreObject *self, class QoreObject *old, class QoreQTableWidgetItem *qtwi, ExceptionSink *xsink)
{
   xsink->raiseException("QTABLEWIDGETITEM-COPY-ERROR", "objects of this class cannot be copied");
}

//QBrush background () const
static AbstractQoreNode *QTABLEWIDGETITEM_background(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qtwi->qore_obj->background());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//Qt::CheckState checkState () const
static AbstractQoreNode *QTABLEWIDGETITEM_checkState(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtwi->qore_obj->checkState());
}

////virtual QTableWidgetItem * clone () const
//static AbstractQoreNode *QTABLEWIDGETITEM_clone(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qtwi->qore_obj->clone());
//}

//int column () const
static AbstractQoreNode *QTABLEWIDGETITEM_column(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtwi->qore_obj->column());
}

//virtual QVariant data ( int role ) const
static AbstractQoreNode *QTABLEWIDGETITEM_data(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int role = p ? p->getAsInt() : 0;
   return return_qvariant(qtwi->qore_obj->data(role));
}

//Qt::ItemFlags flags () const
static AbstractQoreNode *QTABLEWIDGETITEM_flags(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtwi->qore_obj->flags());
}

//QFont font () const
static AbstractQoreNode *QTABLEWIDGETITEM_font(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qf = new QoreObject(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(qtwi->qore_obj->font());
   o_qf->setPrivate(CID_QFONT, q_qf);
   return o_qf;
}

//QBrush foreground () const
static AbstractQoreNode *QTABLEWIDGETITEM_foreground(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qtwi->qore_obj->foreground());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//QIcon icon () const
static AbstractQoreNode *QTABLEWIDGETITEM_icon(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qtwi->qore_obj->icon());
   o_qi->setPrivate(CID_QICON, q_qi);
   return o_qi;
}

//bool isSelected () const
static AbstractQoreNode *QTABLEWIDGETITEM_isSelected(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qtwi->qore_obj->isSelected());
}

////virtual void read ( QDataStream & in )
//static AbstractQoreNode *QTABLEWIDGETITEM_read(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? QDataStream in = p;
//   qtwi->qore_obj->read(in);
//   return 0;
//}

//int row () const
static AbstractQoreNode *QTABLEWIDGETITEM_row(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtwi->qore_obj->row());
}

//void setBackground ( const QBrush & brush )
static AbstractQoreNode *QTABLEWIDGETITEM_setBackground(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QBrush brush;
   if (get_qbrush(p, brush, xsink))
      return 0;
   qtwi->qore_obj->setBackground(brush);
   return 0;
}

//void setCheckState ( Qt::CheckState state )
static AbstractQoreNode *QTABLEWIDGETITEM_setCheckState(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::CheckState state = (Qt::CheckState)(p ? p->getAsInt() : 0);
   qtwi->qore_obj->setCheckState(state);
   return 0;
}

//virtual void setData ( int role, const QVariant & value )
static AbstractQoreNode *QTABLEWIDGETITEM_setData(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int role = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QVariant value;
   if (get_qvariant(p, value, xsink))
      return 0;
   qtwi->qore_obj->setData(role, value);
   return 0;
}

//void setFlags ( Qt::ItemFlags flags )
static AbstractQoreNode *QTABLEWIDGETITEM_setFlags(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::ItemFlags flags = (Qt::ItemFlags)(p ? p->getAsInt() : 0);
   qtwi->qore_obj->setFlags(flags);
   return 0;
}

//void setFont ( const QFont & font )
static AbstractQoreNode *QTABLEWIDGETITEM_setFont(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQFont *font = p ? (QoreQFont *)p->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGETITEM-SETFONT-PARAM-ERROR", "expecting a QFont object as first argument to QTableWidgetItem::setFont()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> fontHolder(static_cast<AbstractPrivateData *>(font), xsink);
   qtwi->qore_obj->setFont(*(static_cast<QFont *>(font)));
   return 0;
}

//void setForeground ( const QBrush & brush )
static AbstractQoreNode *QTABLEWIDGETITEM_setForeground(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QBrush brush;
   if (get_qbrush(p, brush, xsink))
      return 0;
   qtwi->qore_obj->setForeground(brush);
   return 0;
}

//void setIcon ( const QIcon & icon )
static AbstractQoreNode *QTABLEWIDGETITEM_setIcon(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQIcon *icon = p ? (QoreQIcon *)p->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGETITEM-SETICON-PARAM-ERROR", "expecting a QIcon object as first argument to QTableWidgetItem::setIcon()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
   qtwi->qore_obj->setIcon(*(static_cast<QIcon *>(icon)));
   return 0;
}

//void setSelected ( bool select )
static AbstractQoreNode *QTABLEWIDGETITEM_setSelected(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool select = p ? p->getAsBool() : false;
   qtwi->qore_obj->setSelected(select);
   return 0;
}

//void setSizeHint ( const QSize & size )
static AbstractQoreNode *QTABLEWIDGETITEM_setSizeHint(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQSize *size = p ? (QoreQSize *)p->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!size) {
      if (!xsink->isException())
         xsink->raiseException("QTABLEWIDGETITEM-SETSIZEHINT-PARAM-ERROR", "expecting a QSize object as first argument to QTableWidgetItem::setSizeHint()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
   qtwi->qore_obj->setSizeHint(*(static_cast<QSize *>(size)));
   return 0;
}

//void setStatusTip ( const QString & statusTip )
static AbstractQoreNode *QTABLEWIDGETITEM_setStatusTip(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString statusTip;
   if (get_qstring(p, statusTip, xsink))
      return 0;
   qtwi->qore_obj->setStatusTip(statusTip);
   return 0;
}

//void setText ( const QString & text )
static AbstractQoreNode *QTABLEWIDGETITEM_setText(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qtwi->qore_obj->setText(text);
   return 0;
}

//void setTextAlignment ( int alignment )
static AbstractQoreNode *QTABLEWIDGETITEM_setTextAlignment(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int alignment = p ? p->getAsInt() : 0;
   qtwi->qore_obj->setTextAlignment(alignment);
   return 0;
}

//void setToolTip ( const QString & toolTip )
static AbstractQoreNode *QTABLEWIDGETITEM_setToolTip(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString toolTip;
   if (get_qstring(p, toolTip, xsink))
      return 0;
   qtwi->qore_obj->setToolTip(toolTip);
   return 0;
}

//void setWhatsThis ( const QString & whatsThis )
static AbstractQoreNode *QTABLEWIDGETITEM_setWhatsThis(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString whatsThis;
   if (get_qstring(p, whatsThis, xsink))
      return 0;
   qtwi->qore_obj->setWhatsThis(whatsThis);
   return 0;
}

//QSize sizeHint () const
static AbstractQoreNode *QTABLEWIDGETITEM_sizeHint(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qtwi->qore_obj->sizeHint());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//QString statusTip () const
static AbstractQoreNode *QTABLEWIDGETITEM_statusTip(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qtwi->qore_obj->statusTip().toUtf8().data(), QCS_UTF8);
}

////QTableWidget * tableWidget () const
//static AbstractQoreNode *QTABLEWIDGETITEM_tableWidget(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qtwi->qore_obj->tableWidget());
//}

//QString text () const
static AbstractQoreNode *QTABLEWIDGETITEM_text(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qtwi->qore_obj->text().toUtf8().data(), QCS_UTF8);
}

//int textAlignment () const
static AbstractQoreNode *QTABLEWIDGETITEM_textAlignment(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtwi->qore_obj->textAlignment());
}

//QString toolTip () const
static AbstractQoreNode *QTABLEWIDGETITEM_toolTip(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qtwi->qore_obj->toolTip().toUtf8().data(), QCS_UTF8);
}

//int type () const
static AbstractQoreNode *QTABLEWIDGETITEM_type(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtwi->qore_obj->type());
}

//QString whatsThis () const
static AbstractQoreNode *QTABLEWIDGETITEM_whatsThis(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qtwi->qore_obj->whatsThis().toUtf8().data(), QCS_UTF8);
}

////virtual void write ( QDataStream & out ) const
//static AbstractQoreNode *QTABLEWIDGETITEM_write(QoreObject *self, QoreQTableWidgetItem *qtwi, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? QDataStream out = p;
//   qtwi->qore_obj->write(out);
//   return 0;
//}

QoreClass *initQTableWidgetItemClass()
{
   QC_QTableWidgetItem = new QoreClass("QTableWidgetItem", QDOM_GUI);
   CID_QTABLEWIDGETITEM = QC_QTableWidgetItem->getID();

   QC_QTableWidgetItem->setConstructor(QTABLEWIDGETITEM_constructor);
   QC_QTableWidgetItem->setCopy((q_copy_t)QTABLEWIDGETITEM_copy);

   QC_QTableWidgetItem->addMethod("background",                  (q_method_t)QTABLEWIDGETITEM_background);
   QC_QTableWidgetItem->addMethod("checkState",                  (q_method_t)QTABLEWIDGETITEM_checkState);
   //QC_QTableWidgetItem->addMethod("clone",                       (q_method_t)QTABLEWIDGETITEM_clone);
   QC_QTableWidgetItem->addMethod("column",                      (q_method_t)QTABLEWIDGETITEM_column);
   QC_QTableWidgetItem->addMethod("data",                        (q_method_t)QTABLEWIDGETITEM_data);
   QC_QTableWidgetItem->addMethod("flags",                       (q_method_t)QTABLEWIDGETITEM_flags);
   QC_QTableWidgetItem->addMethod("font",                        (q_method_t)QTABLEWIDGETITEM_font);
   QC_QTableWidgetItem->addMethod("foreground",                  (q_method_t)QTABLEWIDGETITEM_foreground);
   QC_QTableWidgetItem->addMethod("icon",                        (q_method_t)QTABLEWIDGETITEM_icon);
   QC_QTableWidgetItem->addMethod("isSelected",                  (q_method_t)QTABLEWIDGETITEM_isSelected);
   //QC_QTableWidgetItem->addMethod("read",                        (q_method_t)QTABLEWIDGETITEM_read);
   QC_QTableWidgetItem->addMethod("row",                         (q_method_t)QTABLEWIDGETITEM_row);
   QC_QTableWidgetItem->addMethod("setBackground",               (q_method_t)QTABLEWIDGETITEM_setBackground);
   QC_QTableWidgetItem->addMethod("setCheckState",               (q_method_t)QTABLEWIDGETITEM_setCheckState);
   QC_QTableWidgetItem->addMethod("setData",                     (q_method_t)QTABLEWIDGETITEM_setData);
   QC_QTableWidgetItem->addMethod("setFlags",                    (q_method_t)QTABLEWIDGETITEM_setFlags);
   QC_QTableWidgetItem->addMethod("setFont",                     (q_method_t)QTABLEWIDGETITEM_setFont);
   QC_QTableWidgetItem->addMethod("setForeground",               (q_method_t)QTABLEWIDGETITEM_setForeground);
   QC_QTableWidgetItem->addMethod("setIcon",                     (q_method_t)QTABLEWIDGETITEM_setIcon);
   QC_QTableWidgetItem->addMethod("setSelected",                 (q_method_t)QTABLEWIDGETITEM_setSelected);
   QC_QTableWidgetItem->addMethod("setSizeHint",                 (q_method_t)QTABLEWIDGETITEM_setSizeHint);
   QC_QTableWidgetItem->addMethod("setStatusTip",                (q_method_t)QTABLEWIDGETITEM_setStatusTip);
   QC_QTableWidgetItem->addMethod("setText",                     (q_method_t)QTABLEWIDGETITEM_setText);
   QC_QTableWidgetItem->addMethod("setTextAlignment",            (q_method_t)QTABLEWIDGETITEM_setTextAlignment);
   QC_QTableWidgetItem->addMethod("setToolTip",                  (q_method_t)QTABLEWIDGETITEM_setToolTip);
   QC_QTableWidgetItem->addMethod("setWhatsThis",                (q_method_t)QTABLEWIDGETITEM_setWhatsThis);
   QC_QTableWidgetItem->addMethod("sizeHint",                    (q_method_t)QTABLEWIDGETITEM_sizeHint);
   QC_QTableWidgetItem->addMethod("statusTip",                   (q_method_t)QTABLEWIDGETITEM_statusTip);
   //QC_QTableWidgetItem->addMethod("tableWidget",                 (q_method_t)QTABLEWIDGETITEM_tableWidget);
   QC_QTableWidgetItem->addMethod("text",                        (q_method_t)QTABLEWIDGETITEM_text);
   QC_QTableWidgetItem->addMethod("textAlignment",               (q_method_t)QTABLEWIDGETITEM_textAlignment);
   QC_QTableWidgetItem->addMethod("toolTip",                     (q_method_t)QTABLEWIDGETITEM_toolTip);
   QC_QTableWidgetItem->addMethod("type",                        (q_method_t)QTABLEWIDGETITEM_type);
   QC_QTableWidgetItem->addMethod("whatsThis",                   (q_method_t)QTABLEWIDGETITEM_whatsThis);
   //QC_QTableWidgetItem->addMethod("write",                       (q_method_t)QTABLEWIDGETITEM_write);

   return QC_QTableWidgetItem;
}
