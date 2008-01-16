/*
 QC_QListWidgetItem.cc
 
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

#include "QC_QListWidgetItem.h"
#include "QC_QIcon.h"
#include "QC_QBrush.h"
#include "QC_QListWidget.h"
#include "QC_QFont.h"
#include "QC_QSize.h"

#include "qore-qt.h"

int CID_QLISTWIDGETITEM;
class QoreClass *QC_QListWidgetItem = 0;

//QListWidgetItem ( QListWidget * parent = 0, int type = Type )
//QListWidgetItem ( const QString & text, QListWidget * parent = 0, int type = Type )
//QListWidgetItem ( const QIcon & icon, const QString & text, QListWidget * parent = 0, int type = Type )
//QListWidgetItem ( const QListWidgetItem & other )
static void QLISTWIDGETITEM_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QLISTWIDGETITEM, new QoreQListWidgetItem());
      return;
   }
   
   QoreQIcon *icon = p->type == NT_OBJECT ? (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (*xsink)
      return;

   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);

   bool got_icon = false;
   int offset = 0;
   if (icon) {
      p = get_param(params, ++offset);
      got_icon = true;
   }

   QString text;
   bool got_text = false;
   if (!get_qstring(p, text, xsink, true))
   {
      p = get_param(params, ++offset);
      got_text = true;
   }
   else {
      if (*xsink)
	 return;
      if (offset) { // we got icon but not text
	 xsink->raiseException("QLISTWIDGETITEM-CONSTRUTOR-ERROR", "expecting a string as second argument to QListWidgetItem::constructor() when the first argument is a QIcon, got type '%s' instead", p ? p->getTypeName() : "NOTHING");
	 return;
      }
   }

   QoreQListWidget *parent = p && p->type == NT_OBJECT ? (QoreQListWidget *)p->val.object->getReferencedPrivateData(CID_QLISTWIDGET, xsink) : 0;
   if (*xsink)
      return;

   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, ++offset);
   int type = !is_nothing(p) ? p->getAsInt() : QListWidgetItem::Type;

   //printd(5, "QListWidgetItem::constructor() offset=%d, parent=%08p\n", offset, parent);
   if (offset == 1)
      self->setPrivate(CID_QLISTWIDGETITEM, new QoreQListWidgetItem(parent ? static_cast<QListWidget *>(parent->qobj) : 0, type));
   else if (offset == 2)
      self->setPrivate(CID_QLISTWIDGETITEM, new QoreQListWidgetItem(text, parent ? static_cast<QListWidget *>(parent->qobj) : 0, type));
   else
      self->setPrivate(CID_QLISTWIDGETITEM, new QoreQListWidgetItem(*(static_cast<QIcon *>(icon)), text, parent ? static_cast<QListWidget *>(parent->qobj) : 0, type));
}

static void QLISTWIDGETITEM_copy(class QoreObject *self, class QoreObject *old, class QoreQListWidgetItem *qlwi, ExceptionSink *xsink)
{
   self->setPrivate(CID_QLISTWIDGETITEM, new QoreQListWidgetItem(*qlwi->getQListWidgetItem()));
}

//QBrush background () const
static QoreNode *QLISTWIDGETITEM_background(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qlwi->getQListWidgetItem()->background());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//Qt::CheckState checkState () const
static QoreNode *QLISTWIDGETITEM_checkState(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlwi->getQListWidgetItem()->checkState());
}

//virtual QListWidgetItem * clone () const
static QoreNode *QLISTWIDGETITEM_clone(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qlwi = new QoreObject(self->getClass(CID_QLISTWIDGETITEM), getProgram());
   QoreQListWidgetItem *q_qlwi = new QoreQListWidgetItem(*qlwi);
   o_qlwi->setPrivate(CID_QLISTWIDGETITEM, q_qlwi);
   return new QoreNode(o_qlwi);
}

//virtual QVariant data ( int role ) const
static QoreNode *QLISTWIDGETITEM_data(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int role = p ? p->getAsInt() : 0;
   return return_qvariant(qlwi->getQListWidgetItem()->data(role));
}

//Qt::ItemFlags flags () const
static QoreNode *QLISTWIDGETITEM_flags(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlwi->getQListWidgetItem()->flags());
}

//QFont font () const
static QoreNode *QLISTWIDGETITEM_font(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qf = new QoreObject(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(qlwi->getQListWidgetItem()->font());
   o_qf->setPrivate(CID_QFONT, q_qf);
   return new QoreNode(o_qf);
}

//QBrush foreground () const
static QoreNode *QLISTWIDGETITEM_foreground(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qlwi->getQListWidgetItem()->foreground());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return new QoreNode(o_qb);
}

//QIcon icon () const
static QoreNode *QLISTWIDGETITEM_icon(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qlwi->getQListWidgetItem()->icon());
   o_qi->setPrivate(CID_QICON, q_qi);
   return new QoreNode(o_qi);
}

//bool isHidden () const
static QoreNode *QLISTWIDGETITEM_isHidden(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qlwi->getQListWidgetItem()->isHidden());
}

//bool isSelected () const
static QoreNode *QLISTWIDGETITEM_isSelected(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qlwi->getQListWidgetItem()->isSelected());
}

//QListWidget * listWidget () const
static QoreNode *QLISTWIDGETITEM_listWidget(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QListWidget *qt_qobj = qlwi->getQListWidgetItem()->listWidget();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QListWidget, getProgram());
      QoreQtQListWidget *t_qobj = new QoreQtQListWidget(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QLISTWIDGET, t_qobj);
   }
   return new QoreNode(rv_obj);
}

/*
//virtual void read ( QDataStream & in )
static QoreNode *QLISTWIDGETITEM_read(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   ??? QDataStream in = p;
   qlwi->getQListWidgetItem()->read(in);
   return 0;
}
*/

//void setBackground ( const QBrush & brush )
static QoreNode *QLISTWIDGETITEM_setBackground(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QBrush brush;
   if (get_qbrush(p, brush, xsink))
      return 0;
   qlwi->getQListWidgetItem()->setBackground(brush);
   return 0;
}

//void setCheckState ( Qt::CheckState state )
static QoreNode *QLISTWIDGETITEM_setCheckState(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::CheckState state = (Qt::CheckState)(p ? p->getAsInt() : 0);
   qlwi->getQListWidgetItem()->setCheckState(state);
   return 0;
}

//virtual void setData ( int role, const QVariant & value )
static QoreNode *QLISTWIDGETITEM_setData(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int role = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QVariant value;
   if (get_qvariant(p, value, xsink))
      return 0;
   qlwi->getQListWidgetItem()->setData(role, value);
   return 0;
}

//void setFlags ( Qt::ItemFlags flags )
static QoreNode *QLISTWIDGETITEM_setFlags(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ItemFlags flags = (Qt::ItemFlags)(p ? p->getAsInt() : 0);
   qlwi->getQListWidgetItem()->setFlags(flags);
   return 0;
}

//void setFont ( const QFont & font )
static QoreNode *QLISTWIDGETITEM_setFont(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQFont *font = (p && p->type == NT_OBJECT) ? (QoreQFont *)p->val.object->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGETITEM-SETFONT-PARAM-ERROR", "expecting a QFont object as first argument to QListWidgetItem::setFont()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> fontHolder(static_cast<AbstractPrivateData *>(font), xsink);
   qlwi->getQListWidgetItem()->setFont(*(static_cast<QFont *>(font)));
   return 0;
}

//void setForeground ( const QBrush & brush )
static QoreNode *QLISTWIDGETITEM_setForeground(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QBrush brush;
   if (get_qbrush(p, brush, xsink))
      return 0;
   qlwi->getQListWidgetItem()->setForeground(brush);
   return 0;
}

//void setHidden ( bool hide )
static QoreNode *QLISTWIDGETITEM_setHidden(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool hide = p ? p->getAsBool() : false;
   qlwi->getQListWidgetItem()->setHidden(hide);
   return 0;
}

//void setIcon ( const QIcon & icon )
static QoreNode *QLISTWIDGETITEM_setIcon(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGETITEM-SETICON-PARAM-ERROR", "expecting a QIcon object as first argument to QListWidgetItem::setIcon()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
   qlwi->getQListWidgetItem()->setIcon(*(static_cast<QIcon *>(icon)));
   return 0;
}

//void setSelected ( bool select )
static QoreNode *QLISTWIDGETITEM_setSelected(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool select = p ? p->getAsBool() : false;
   qlwi->getQListWidgetItem()->setSelected(select);
   return 0;
}

//void setSizeHint ( const QSize & size )
static QoreNode *QLISTWIDGETITEM_setSizeHint(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQSize *size = (p && p->type == NT_OBJECT) ? (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!size) {
      if (!xsink->isException())
         xsink->raiseException("QLISTWIDGETITEM-SETSIZEHINT-PARAM-ERROR", "expecting a QSize object as first argument to QListWidgetItem::setSizeHint()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
   qlwi->getQListWidgetItem()->setSizeHint(*(static_cast<QSize *>(size)));
   return 0;
}

//void setStatusTip ( const QString & statusTip )
static QoreNode *QLISTWIDGETITEM_setStatusTip(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString statusTip;
   if (get_qstring(p, statusTip, xsink))
      return 0;
   qlwi->getQListWidgetItem()->setStatusTip(statusTip);
   return 0;
}

//void setText ( const QString & text )
static QoreNode *QLISTWIDGETITEM_setText(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qlwi->getQListWidgetItem()->setText(text);
   return 0;
}

//void setTextAlignment ( int alignment )
static QoreNode *QLISTWIDGETITEM_setTextAlignment(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int alignment = p ? p->getAsInt() : 0;
   qlwi->getQListWidgetItem()->setTextAlignment(alignment);
   return 0;
}

//void setToolTip ( const QString & toolTip )
static QoreNode *QLISTWIDGETITEM_setToolTip(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString toolTip;
   if (get_qstring(p, toolTip, xsink))
      return 0;
   qlwi->getQListWidgetItem()->setToolTip(toolTip);
   return 0;
}

//void setWhatsThis ( const QString & whatsThis )
static QoreNode *QLISTWIDGETITEM_setWhatsThis(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString whatsThis;
   if (get_qstring(p, whatsThis, xsink))
      return 0;
   qlwi->getQListWidgetItem()->setWhatsThis(whatsThis);
   return 0;
}

//QSize sizeHint () const
static QoreNode *QLISTWIDGETITEM_sizeHint(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qlwi->getQListWidgetItem()->sizeHint());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//QString statusTip () const
static QoreNode *QLISTWIDGETITEM_statusTip(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qlwi->getQListWidgetItem()->statusTip().toUtf8().data(), QCS_UTF8);
}

//QString text () const
static QoreNode *QLISTWIDGETITEM_text(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qlwi->getQListWidgetItem()->text().toUtf8().data(), QCS_UTF8);
}

//int textAlignment () const
static QoreNode *QLISTWIDGETITEM_textAlignment(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlwi->getQListWidgetItem()->textAlignment());
}

//QString toolTip () const
static QoreNode *QLISTWIDGETITEM_toolTip(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qlwi->getQListWidgetItem()->toolTip().toUtf8().data(), QCS_UTF8);
}

//int type () const
static QoreNode *QLISTWIDGETITEM_type(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qlwi->getQListWidgetItem()->type());
}

//QString whatsThis () const
static QoreNode *QLISTWIDGETITEM_whatsThis(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qlwi->getQListWidgetItem()->whatsThis().toUtf8().data(), QCS_UTF8);
}

/*
//virtual void write ( QDataStream & out ) const
static QoreNode *QLISTWIDGETITEM_write(QoreObject *self, QoreQListWidgetItem *qlwi, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   ??? QDataStream out = p;
   qlwi->getQListWidgetItem()->write(out);
   return 0;
}
*/

static QoreClass *initQListWidgetItemClass()
{
   QC_QListWidgetItem = new QoreClass("QListWidgetItem", QDOM_GUI);
   CID_QLISTWIDGETITEM = QC_QListWidgetItem->getID();

   QC_QListWidgetItem->setConstructor(QLISTWIDGETITEM_constructor);
   QC_QListWidgetItem->setCopy((q_copy_t)QLISTWIDGETITEM_copy);

   QC_QListWidgetItem->addMethod("background",                  (q_method_t)QLISTWIDGETITEM_background);
   QC_QListWidgetItem->addMethod("checkState",                  (q_method_t)QLISTWIDGETITEM_checkState);
   QC_QListWidgetItem->addMethod("clone",                       (q_method_t)QLISTWIDGETITEM_clone);
   QC_QListWidgetItem->addMethod("data",                        (q_method_t)QLISTWIDGETITEM_data);
   QC_QListWidgetItem->addMethod("flags",                       (q_method_t)QLISTWIDGETITEM_flags);
   QC_QListWidgetItem->addMethod("font",                        (q_method_t)QLISTWIDGETITEM_font);
   QC_QListWidgetItem->addMethod("foreground",                  (q_method_t)QLISTWIDGETITEM_foreground);
   QC_QListWidgetItem->addMethod("icon",                        (q_method_t)QLISTWIDGETITEM_icon);
   QC_QListWidgetItem->addMethod("isHidden",                    (q_method_t)QLISTWIDGETITEM_isHidden);
   QC_QListWidgetItem->addMethod("isSelected",                  (q_method_t)QLISTWIDGETITEM_isSelected);
   QC_QListWidgetItem->addMethod("listWidget",                  (q_method_t)QLISTWIDGETITEM_listWidget);
   //QC_QListWidgetItem->addMethod("read",                        (q_method_t)QLISTWIDGETITEM_read);
   QC_QListWidgetItem->addMethod("setBackground",               (q_method_t)QLISTWIDGETITEM_setBackground);
   QC_QListWidgetItem->addMethod("setCheckState",               (q_method_t)QLISTWIDGETITEM_setCheckState);
   QC_QListWidgetItem->addMethod("setData",                     (q_method_t)QLISTWIDGETITEM_setData);
   QC_QListWidgetItem->addMethod("setFlags",                    (q_method_t)QLISTWIDGETITEM_setFlags);
   QC_QListWidgetItem->addMethod("setFont",                     (q_method_t)QLISTWIDGETITEM_setFont);
   QC_QListWidgetItem->addMethod("setForeground",               (q_method_t)QLISTWIDGETITEM_setForeground);
   QC_QListWidgetItem->addMethod("setHidden",                   (q_method_t)QLISTWIDGETITEM_setHidden);
   QC_QListWidgetItem->addMethod("setIcon",                     (q_method_t)QLISTWIDGETITEM_setIcon);
   QC_QListWidgetItem->addMethod("setSelected",                 (q_method_t)QLISTWIDGETITEM_setSelected);
   QC_QListWidgetItem->addMethod("setSizeHint",                 (q_method_t)QLISTWIDGETITEM_setSizeHint);
   QC_QListWidgetItem->addMethod("setStatusTip",                (q_method_t)QLISTWIDGETITEM_setStatusTip);
   QC_QListWidgetItem->addMethod("setText",                     (q_method_t)QLISTWIDGETITEM_setText);
   QC_QListWidgetItem->addMethod("setTextAlignment",            (q_method_t)QLISTWIDGETITEM_setTextAlignment);
   QC_QListWidgetItem->addMethod("setToolTip",                  (q_method_t)QLISTWIDGETITEM_setToolTip);
   QC_QListWidgetItem->addMethod("setWhatsThis",                (q_method_t)QLISTWIDGETITEM_setWhatsThis);
   QC_QListWidgetItem->addMethod("sizeHint",                    (q_method_t)QLISTWIDGETITEM_sizeHint);
   QC_QListWidgetItem->addMethod("statusTip",                   (q_method_t)QLISTWIDGETITEM_statusTip);
   QC_QListWidgetItem->addMethod("text",                        (q_method_t)QLISTWIDGETITEM_text);
   QC_QListWidgetItem->addMethod("textAlignment",               (q_method_t)QLISTWIDGETITEM_textAlignment);
   QC_QListWidgetItem->addMethod("toolTip",                     (q_method_t)QLISTWIDGETITEM_toolTip);
   QC_QListWidgetItem->addMethod("type",                        (q_method_t)QLISTWIDGETITEM_type);
   QC_QListWidgetItem->addMethod("whatsThis",                   (q_method_t)QLISTWIDGETITEM_whatsThis);
   //QC_QListWidgetItem->addMethod("write",                       (q_method_t)QLISTWIDGETITEM_write);

   return QC_QListWidgetItem;
}

QoreNamespace *initQListWidgetItemNS()
{
   QoreNamespace *ns = new QoreNamespace("QListWidgetItem");
   ns->addSystemClass(initQListWidgetItemClass());

   // ItemType enum
   ns->addConstant("Type",                     new QoreNode((int64)QListWidgetItem::Type));
   ns->addConstant("UserType",                 new QoreNode((int64)QListWidgetItem::UserType));

   return ns;
}
