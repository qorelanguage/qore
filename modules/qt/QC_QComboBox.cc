/*
 QC_QComboBox.cc
 
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

#include "QC_QComboBox.h"

int CID_QCOMBOBOX;
class QoreClass *QC_QComboBox = 0;

//QComboBox ( QWidget * parent = 0 )
static void QCOMBOBOX_constructor(QoreObject *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);
   self->setPrivate(CID_QCOMBOBOX, new QoreQComboBox(self, parent ? parent->getQWidget() : 0));
   return;
}

static void QCOMBOBOX_copy(class QoreObject *self, class QoreObject *old, class QoreQComboBox *qcb, ExceptionSink *xsink)
{
   xsink->raiseException("QCOMBOBOX-COPY-ERROR", "objects of this class cannot be copied");
}

//void addItem ( const QString & text, const QVariant & userData = QVariant() )
//void addItem ( const QIcon & icon, const QString & text, const QVariant & userData = QVariant() )
static QoreNode *QCOMBOBOX_addItem(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQIcon *icon = (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink);
      if (!icon) {
         if (!xsink->isException())
            xsink->raiseException("QCOMBOBOX-ADDITEM-PARAM-ERROR", "QComboBox::addItem() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<QoreQIcon> iconHolder(icon, xsink);
      p = get_param(params, 1);
      QString text;
      if (get_qstring(p, text, xsink))
	 return 0;
      QVariant userData;
      if (get_qvariant(p, userData, xsink, true))
	 qcb->getQComboBox()->addItem(*(static_cast<QIcon *>(icon)), text);
      else
	 qcb->getQComboBox()->addItem(*(static_cast<QIcon *>(icon)), text, userData);
      return 0;
   }
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   p = get_param(params, 1);
   QVariant userData;
   if (get_qvariant(p, userData, xsink, true))
      qcb->getQComboBox()->addItem(text);
   else {
      qcb->getQComboBox()->addItem(text, userData);
      //printd(0, "QComboBox::addItem(%s, %d)\n", text, userData.toInt());
   }
   return 0;
}

//void addItems ( const QStringList & texts )
static QoreNode *QCOMBOBOX_addItems(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_LIST) {
      xsink->raiseException("QCOMBOBOX-ADDITEMS-PARAM-ERROR", "expecting a list as first argument to QComboBox::addItems()");
      return 0;
   }
   QStringList texts;
   ListIterator li_texts(p->val.list);
   while (li_texts.next())
   {
      QoreNodeTypeHelper str(li_texts.getValue(), NT_STRING, xsink);
      if (*xsink)
         return 0;
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
         return 0;
      texts.push_back(tmp);
   }
   qcb->qobj->addItems(texts);
   return 0;
}

////QCompleter * completer () const
//static QoreNode *QCOMBOBOX_completer(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qcb->getQComboBox()->completer());
//}

//int count () const
static QoreNode *QCOMBOBOX_count(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcb->getQComboBox()->count());
}

//int currentIndex () const
static QoreNode *QCOMBOBOX_currentIndex(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcb->getQComboBox()->currentIndex());
}

//QString currentText () const
static QoreNode *QCOMBOBOX_currentText(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qcb->getQComboBox()->currentText().toUtf8().data(), QCS_UTF8));
}

//bool duplicatesEnabled () const
static QoreNode *QCOMBOBOX_duplicatesEnabled(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qcb->getQComboBox()->duplicatesEnabled());
}

//int findData ( const QVariant & data, int role = Qt::UserRole, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive ) const
static QoreNode *QCOMBOBOX_findData(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QVariant data;
   if (get_qvariant(p, data, xsink))
      return 0;
   p = get_param(params, 1);
   int role = !is_nothing(p) ? p->getAsInt() : Qt::UserRole;
   p = get_param(params, 2);
   Qt::MatchFlags flags = (Qt::MatchFlags)(p ? p->getAsInt() : 0);
   return new QoreNode((int64)qcb->qobj->findData(data, role, flags));
}

//int findText ( const QString & text, Qt::MatchFlags flags = Qt::MatchExactly | Qt::MatchCaseSensitive ) const
static QoreNode *QCOMBOBOX_findText(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;

   p = get_param(params, 1);
   Qt::MatchFlags flags = (Qt::MatchFlags)(p ? p->getAsInt() : 0);
   return new QoreNode((int64)qcb->getQComboBox()->findText(text, flags));
}

//bool hasFrame () const
static QoreNode *QCOMBOBOX_hasFrame(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qcb->getQComboBox()->hasFrame());
}

//virtual void hidePopup ()
static QoreNode *QCOMBOBOX_hidePopup(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   qcb->getQComboBox()->hidePopup();
   return 0;
}

//QSize iconSize () const
static QoreNode *QCOMBOBOX_iconSize(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qcb->getQComboBox()->iconSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//void insertItem ( int index, const QString & text, const QVariant & userData = QVariant() )
//void insertItem ( int index, const QIcon & icon, const QString & text, const QVariant & userData = QVariant() )
static QoreNode *QCOMBOBOX_insertItem(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (*xsink)
      return 0;
   int offset = 1;
   if (icon)
      p = get_param(params, ++offset);
   ReferenceHolder<QoreQIcon> iconHolder(icon, xsink);

   QString text;
   if (get_qstring(p, text, xsink))
      return 0;

   p = get_param(params, ++offset);

   QVariant userData;
   if (get_qvariant(p, userData, xsink, true))
      userData = QVariant();

   if (icon)
      qcb->qobj->insertItem(index, *(static_cast<QIcon *>(icon)), text, userData);
   else
      qcb->qobj->insertItem(index, text, userData);
   return 0;
}

//void insertItems ( int index, const QStringList & list )
static QoreNode *QCOMBOBOX_insertItems(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   if (!p || p->type != NT_LIST) {
      xsink->raiseException("QCOMBOBOX-INSERTITEMS-PARAM-ERROR", "expecting a list as second argument to QComboBox::insertItems()");
      return 0;
   }
   QStringList list;
   ListIterator li_list(p->val.list);
   while (li_list.next())
   {
      QoreNodeTypeHelper str(li_list.getValue(), NT_STRING, xsink);
      if (*xsink)
         return 0;
      QString tmp;
      if (get_qstring(*str, tmp, xsink))
	 return 0;
      list.push_back(tmp);
   }
   qcb->qobj->insertItems(index, list);
   return 0;
}

//InsertPolicy insertPolicy () const
static QoreNode *QCOMBOBOX_insertPolicy(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcb->getQComboBox()->insertPolicy());
}

//bool isEditable () const
static QoreNode *QCOMBOBOX_isEditable(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qcb->getQComboBox()->isEditable());
}

//QVariant itemData ( int index, int role = Qt::UserRole ) const
static QoreNode *QCOMBOBOX_itemData(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int role = !is_nothing(p) ? p->getAsInt() : Qt::UserRole;
   return return_qvariant(qcb->getQComboBox()->itemData(index, role));
}

//QAbstractItemDelegate * itemDelegate () const
static QoreNode *QCOMBOBOX_itemDelegate(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QAbstractItemDelegate *qt_qobj = qcb->getQComboBox()->itemDelegate();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//QIcon itemIcon ( int index ) const
static QoreNode *QCOMBOBOX_itemIcon(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qcb->getQComboBox()->itemIcon(index));
   o_qi->setPrivate(CID_QICON, q_qi);
   return new QoreNode(o_qi);
}

//QString itemText ( int index ) const
static QoreNode *QCOMBOBOX_itemText(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return new QoreNode(new QoreString(qcb->getQComboBox()->itemText(index).toUtf8().data(), QCS_UTF8));
}

//QLineEdit * lineEdit () const
static QoreNode *QCOMBOBOX_lineEdit(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QLineEdit *qt_qobj = qcb->getQComboBox()->lineEdit();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//int maxCount () const
static QoreNode *QCOMBOBOX_maxCount(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcb->getQComboBox()->maxCount());
}

//int maxVisibleItems () const
static QoreNode *QCOMBOBOX_maxVisibleItems(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcb->getQComboBox()->maxVisibleItems());
}

//int minimumContentsLength () const
static QoreNode *QCOMBOBOX_minimumContentsLength(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcb->getQComboBox()->minimumContentsLength());
}

////QAbstractItemModel * model () const
//static QoreNode *QCOMBOBOX_model(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qcb->getQComboBox()->model());
//}

//int modelColumn () const
static QoreNode *QCOMBOBOX_modelColumn(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcb->getQComboBox()->modelColumn());
}

//void removeItem ( int index )
static QoreNode *QCOMBOBOX_removeItem(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qcb->getQComboBox()->removeItem(index);
   return 0;
}

//QModelIndex rootModelIndex () const
static QoreNode *QCOMBOBOX_rootModelIndex(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qmi = new QoreObject(QC_QModelIndex, getProgram());
   QoreQModelIndex *q_qmi = new QoreQModelIndex(qcb->getQComboBox()->rootModelIndex());
   o_qmi->setPrivate(CID_QMODELINDEX, q_qmi);
   return new QoreNode(o_qmi);
}

////void setCompleter ( QCompleter * completer )
//static QoreNode *QCOMBOBOX_setCompleter(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QCompleter* completer = p;
//   qcb->getQComboBox()->setCompleter(completer);
//   return 0;
//}

//void setDuplicatesEnabled ( bool enable )
static QoreNode *QCOMBOBOX_setDuplicatesEnabled(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qcb->getQComboBox()->setDuplicatesEnabled(enable);
   return 0;
}

//void setEditable ( bool editable )
static QoreNode *QCOMBOBOX_setEditable(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool editable = p ? p->getAsBool() : false;
   qcb->getQComboBox()->setEditable(editable);
   return 0;
}

//void setFrame ( bool )
static QoreNode *QCOMBOBOX_setFrame(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qcb->getQComboBox()->setFrame(b);
   return 0;
}

//void setIconSize ( const QSize & size )
static QoreNode *QCOMBOBOX_setIconSize(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQSize *size = (p && p->type == NT_OBJECT) ? (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!size) {
      if (!xsink->isException())
         xsink->raiseException("QCOMBOBOX-SETICONSIZE-PARAM-ERROR", "expecting a QSize object as first argument to QComboBox::setIconSize()");
      return 0;
   }
   ReferenceHolder<QoreQSize> sizeHolder(size, xsink);
   qcb->getQComboBox()->setIconSize(*(static_cast<QSize *>(size)));
   return 0;
}

//void setInsertPolicy ( InsertPolicy policy )
static QoreNode *QCOMBOBOX_setInsertPolicy(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QComboBox::InsertPolicy policy = (QComboBox::InsertPolicy)(p ? p->getAsInt() : 0);
   qcb->getQComboBox()->setInsertPolicy(policy);
   return 0;
}

//void setItemData ( int index, const QVariant & value, int role = Qt::UserRole )
static QoreNode *QCOMBOBOX_setItemData(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QVariant value;
   if (get_qvariant(p, value, xsink))
      return 0;
   p = get_param(params, 2);
   int role = !is_nothing(p) ? p->getAsInt() : Qt::UserRole;
   qcb->qobj->setItemData(index, value, role);
   return 0;
}

//void setItemDelegate ( QAbstractItemDelegate * delegate )
static QoreNode *QCOMBOBOX_setItemDelegate(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQAbstractItemDelegate *delegate = (p && p->type == NT_OBJECT) ? (QoreAbstractQAbstractItemDelegate *)p->val.object->getReferencedPrivateData(CID_QABSTRACTITEMDELEGATE, xsink) : 0;
   if (!delegate) {
      if (!xsink->isException())
         xsink->raiseException("QCOMBOBOX-SETITEMDELEGATE-PARAM-ERROR", "expecting a QAbstractItemDelegate object as first argument to QComboBox::setItemDelegate()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQAbstractItemDelegate> delegateHolder(delegate, xsink);
   qcb->getQComboBox()->setItemDelegate(delegate->getQAbstractItemDelegate());
   return 0;
}

//void setItemIcon ( int index, const QIcon & icon )
static QoreNode *QCOMBOBOX_setItemIcon(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QCOMBOBOX-SETITEMICON-PARAM-ERROR", "expecting a QIcon object as second argument to QComboBox::setItemIcon()");
      return 0;
   }
   ReferenceHolder<QoreQIcon> iconHolder(icon, xsink);
   qcb->getQComboBox()->setItemIcon(index, *(static_cast<QIcon *>(icon)));
   return 0;
}

//void setItemText ( int index, const QString & text )
static QoreNode *QCOMBOBOX_setItemText(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   
   qcb->getQComboBox()->setItemText(index, text);
   return 0;
}

//void setLineEdit ( QLineEdit * edit )
static QoreNode *QCOMBOBOX_setLineEdit(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQLineEdit *edit = (p && p->type == NT_OBJECT) ? (QoreQLineEdit *)p->val.object->getReferencedPrivateData(CID_QLINEEDIT, xsink) : 0;
   if (!edit) {
      if (!xsink->isException())
         xsink->raiseException("QCOMBOBOX-SETLINEEDIT-PARAM-ERROR", "expecting a QLineEdit object as first argument to QComboBox::setLineEdit()");
      return 0;
   }
   ReferenceHolder<QoreQLineEdit> editHolder(edit, xsink);
   qcb->getQComboBox()->setLineEdit(static_cast<QLineEdit *>(edit->qobj));
   return 0;
}

//void setMaxCount ( int max )
static QoreNode *QCOMBOBOX_setMaxCount(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int max = p ? p->getAsInt() : 0;
   qcb->getQComboBox()->setMaxCount(max);
   return 0;
}

//void setMaxVisibleItems ( int maxItems )
static QoreNode *QCOMBOBOX_setMaxVisibleItems(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int maxItems = p ? p->getAsInt() : 0;
   qcb->getQComboBox()->setMaxVisibleItems(maxItems);
   return 0;
}

//void setMinimumContentsLength ( int characters )
static QoreNode *QCOMBOBOX_setMinimumContentsLength(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int characters = p ? p->getAsInt() : 0;
   qcb->getQComboBox()->setMinimumContentsLength(characters);
   return 0;
}

////void setModel ( QAbstractItemModel * model )
//static QoreNode *QCOMBOBOX_setModel(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QAbstractItemModel* model = p;
//   qcb->getQComboBox()->setModel(model);
//   return 0;
//}

//void setModelColumn ( int visibleColumn )
static QoreNode *QCOMBOBOX_setModelColumn(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int visibleColumn = p ? p->getAsInt() : 0;
   qcb->getQComboBox()->setModelColumn(visibleColumn);
   return 0;
}

//void setRootModelIndex ( const QModelIndex & index )
static QoreNode *QCOMBOBOX_setRootModelIndex(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQModelIndex *index = (p && p->type == NT_OBJECT) ? (QoreQModelIndex *)p->val.object->getReferencedPrivateData(CID_QMODELINDEX, xsink) : 0;
   if (!index) {
      if (!xsink->isException())
         xsink->raiseException("QCOMBOBOX-SETROOTMODELINDEX-PARAM-ERROR", "expecting a QModelIndex object as first argument to QComboBox::setRootModelIndex()");
      return 0;
   }
   ReferenceHolder<QoreQModelIndex> indexHolder(index, xsink);
   qcb->getQComboBox()->setRootModelIndex(*(static_cast<QModelIndex *>(index)));
   return 0;
}

//void setSizeAdjustPolicy ( SizeAdjustPolicy policy )
static QoreNode *QCOMBOBOX_setSizeAdjustPolicy(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QComboBox::SizeAdjustPolicy policy = (QComboBox::SizeAdjustPolicy)(p ? p->getAsInt() : 0);
   qcb->getQComboBox()->setSizeAdjustPolicy(policy);
   return 0;
}

////void setValidator ( const QValidator * validator )
//static QoreNode *QCOMBOBOX_setValidator(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QValidator* validator = p;
//   qcb->getQComboBox()->setValidator(validator);
//   return 0;
//}

////void setView ( QAbstractItemView * itemView )
//static QoreNode *QCOMBOBOX_setView(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QAbstractItemView* itemView = p;
//   qcb->getQComboBox()->setView(itemView);
//   return 0;
//}

//virtual void showPopup ()
static QoreNode *QCOMBOBOX_showPopup(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   qcb->getQComboBox()->showPopup();
   return 0;
}

//SizeAdjustPolicy sizeAdjustPolicy () const
static QoreNode *QCOMBOBOX_sizeAdjustPolicy(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcb->getQComboBox()->sizeAdjustPolicy());
}

////const QValidator * validator () const
//static QoreNode *QCOMBOBOX_validator(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qcb->getQComboBox()->validator());
//}

////QAbstractItemView * view () const
//static QoreNode *QCOMBOBOX_view(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qcb->getQComboBox()->view());
//}

//void clear ()
static QoreNode *QCOMBOBOX_clear(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   qcb->getQComboBox()->clear();
   return 0;
}

//void clearEditText ()
static QoreNode *QCOMBOBOX_clearEditText(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   qcb->getQComboBox()->clearEditText();
   return 0;
}

//void setCurrentIndex ( int index )
static QoreNode *QCOMBOBOX_setCurrentIndex(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qcb->getQComboBox()->setCurrentIndex(index);
   return 0;
}

//void setEditText ( const QString & text )
static QoreNode *QCOMBOBOX_setEditText(QoreObject *self, QoreQComboBox *qcb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QCOMBOBOX-SETEDITTEXT-PARAM-ERROR", "expecting a string as first argument to QComboBox::setEditText()");
      return 0;
   }
   const char *text = p->val.String->getBuffer();
   qcb->getQComboBox()->setEditText(text);
   return 0;
}

QoreClass *initQComboBoxClass(QoreClass *qwidget)
{
   QC_QComboBox = new QoreClass("QComboBox", QDOM_GUI);
   CID_QCOMBOBOX = QC_QComboBox->getID();

   QC_QComboBox->addBuiltinVirtualBaseClass(qwidget);

   QC_QComboBox->setConstructor(QCOMBOBOX_constructor);
   QC_QComboBox->setCopy((q_copy_t)QCOMBOBOX_copy);

   QC_QComboBox->addMethod("addItem",                     (q_method_t)QCOMBOBOX_addItem);
   QC_QComboBox->addMethod("addItems",                    (q_method_t)QCOMBOBOX_addItems);
   //QC_QComboBox->addMethod("completer",                   (q_method_t)QCOMBOBOX_completer);
   QC_QComboBox->addMethod("count",                       (q_method_t)QCOMBOBOX_count);
   QC_QComboBox->addMethod("currentIndex",                (q_method_t)QCOMBOBOX_currentIndex);
   QC_QComboBox->addMethod("currentText",                 (q_method_t)QCOMBOBOX_currentText);
   QC_QComboBox->addMethod("duplicatesEnabled",           (q_method_t)QCOMBOBOX_duplicatesEnabled);
   QC_QComboBox->addMethod("findData",                    (q_method_t)QCOMBOBOX_findData);
   QC_QComboBox->addMethod("findText",                    (q_method_t)QCOMBOBOX_findText);
   QC_QComboBox->addMethod("hasFrame",                    (q_method_t)QCOMBOBOX_hasFrame);
   QC_QComboBox->addMethod("hidePopup",                   (q_method_t)QCOMBOBOX_hidePopup);
   QC_QComboBox->addMethod("iconSize",                    (q_method_t)QCOMBOBOX_iconSize);
   QC_QComboBox->addMethod("insertItem",                  (q_method_t)QCOMBOBOX_insertItem);
   QC_QComboBox->addMethod("insertItems",                 (q_method_t)QCOMBOBOX_insertItems);
   QC_QComboBox->addMethod("insertPolicy",                (q_method_t)QCOMBOBOX_insertPolicy);
   QC_QComboBox->addMethod("isEditable",                  (q_method_t)QCOMBOBOX_isEditable);
   QC_QComboBox->addMethod("itemData",                    (q_method_t)QCOMBOBOX_itemData);
   QC_QComboBox->addMethod("itemDelegate",                (q_method_t)QCOMBOBOX_itemDelegate);
   QC_QComboBox->addMethod("itemIcon",                    (q_method_t)QCOMBOBOX_itemIcon);
   QC_QComboBox->addMethod("itemText",                    (q_method_t)QCOMBOBOX_itemText);
   QC_QComboBox->addMethod("lineEdit",                    (q_method_t)QCOMBOBOX_lineEdit);
   QC_QComboBox->addMethod("maxCount",                    (q_method_t)QCOMBOBOX_maxCount);
   QC_QComboBox->addMethod("maxVisibleItems",             (q_method_t)QCOMBOBOX_maxVisibleItems);
   QC_QComboBox->addMethod("minimumContentsLength",       (q_method_t)QCOMBOBOX_minimumContentsLength);
   //QC_QComboBox->addMethod("model",                       (q_method_t)QCOMBOBOX_model);
   QC_QComboBox->addMethod("modelColumn",                 (q_method_t)QCOMBOBOX_modelColumn);
   QC_QComboBox->addMethod("removeItem",                  (q_method_t)QCOMBOBOX_removeItem);
   QC_QComboBox->addMethod("rootModelIndex",              (q_method_t)QCOMBOBOX_rootModelIndex);
   //QC_QComboBox->addMethod("setCompleter",                (q_method_t)QCOMBOBOX_setCompleter);
   QC_QComboBox->addMethod("setDuplicatesEnabled",        (q_method_t)QCOMBOBOX_setDuplicatesEnabled);
   QC_QComboBox->addMethod("setEditable",                 (q_method_t)QCOMBOBOX_setEditable);
   QC_QComboBox->addMethod("setFrame",                    (q_method_t)QCOMBOBOX_setFrame);
   QC_QComboBox->addMethod("setIconSize",                 (q_method_t)QCOMBOBOX_setIconSize);
   QC_QComboBox->addMethod("setInsertPolicy",             (q_method_t)QCOMBOBOX_setInsertPolicy);
   QC_QComboBox->addMethod("setItemData",                 (q_method_t)QCOMBOBOX_setItemData);
   QC_QComboBox->addMethod("setItemDelegate",             (q_method_t)QCOMBOBOX_setItemDelegate);
   QC_QComboBox->addMethod("setItemIcon",                 (q_method_t)QCOMBOBOX_setItemIcon);
   QC_QComboBox->addMethod("setItemText",                 (q_method_t)QCOMBOBOX_setItemText);
   QC_QComboBox->addMethod("setLineEdit",                 (q_method_t)QCOMBOBOX_setLineEdit);
   QC_QComboBox->addMethod("setMaxCount",                 (q_method_t)QCOMBOBOX_setMaxCount);
   QC_QComboBox->addMethod("setMaxVisibleItems",          (q_method_t)QCOMBOBOX_setMaxVisibleItems);
   QC_QComboBox->addMethod("setMinimumContentsLength",    (q_method_t)QCOMBOBOX_setMinimumContentsLength);
   //QC_QComboBox->addMethod("setModel",                    (q_method_t)QCOMBOBOX_setModel);
   QC_QComboBox->addMethod("setModelColumn",              (q_method_t)QCOMBOBOX_setModelColumn);
   QC_QComboBox->addMethod("setRootModelIndex",           (q_method_t)QCOMBOBOX_setRootModelIndex);
   QC_QComboBox->addMethod("setSizeAdjustPolicy",         (q_method_t)QCOMBOBOX_setSizeAdjustPolicy);
   //QC_QComboBox->addMethod("setValidator",                (q_method_t)QCOMBOBOX_setValidator);
   //QC_QComboBox->addMethod("setView",                     (q_method_t)QCOMBOBOX_setView);
   QC_QComboBox->addMethod("showPopup",                   (q_method_t)QCOMBOBOX_showPopup);
   QC_QComboBox->addMethod("sizeAdjustPolicy",            (q_method_t)QCOMBOBOX_sizeAdjustPolicy);
   //QC_QComboBox->addMethod("validator",                   (q_method_t)QCOMBOBOX_validator);
   //QC_QComboBox->addMethod("view",                        (q_method_t)QCOMBOBOX_view);
   QC_QComboBox->addMethod("clear",                       (q_method_t)QCOMBOBOX_clear);
   QC_QComboBox->addMethod("clearEditText",               (q_method_t)QCOMBOBOX_clearEditText);
   QC_QComboBox->addMethod("setCurrentIndex",             (q_method_t)QCOMBOBOX_setCurrentIndex);
   QC_QComboBox->addMethod("setEditText",                 (q_method_t)QCOMBOBOX_setEditText);

   return QC_QComboBox;
}
