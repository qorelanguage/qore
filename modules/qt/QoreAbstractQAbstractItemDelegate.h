/*
 QoreAbstractQAbstractItemDelegate.h
 
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

#ifndef _QORE_QOREABSTRACTQABSTRACTITEMDELEGATE_H

#define _QORE_QOREABSTRACTQABSTRACTITEMDELEGATE_H

#include "QoreAbstractQObject.h"

#include "QoreQtEventDispatcher.h"

#include "QC_QPainter.h"
#include "QC_QStyleOptionViewItem.h"
#include "QC_QModelIndex.h"
#include "QC_QAbstractItemModel.h"
#include "QC_QWidget.h"
#include "QC_QSize.h"

extern qore_classid_t CID_QWIDGET;

class QoreAbstractQAbstractItemDelegate : public QoreAbstractQObject
{
   public:
      DLLLOCAL virtual QAbstractItemDelegate *getQAbstractItemDelegate() const = 0;

      // other virtual methods
      virtual QWidget * createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const = 0;
      virtual bool editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index ) = 0;
      virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const = 0;
      virtual void setEditorData ( QWidget * editor, const QModelIndex & index ) const = 0;
      virtual void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const = 0;
      virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const = 0;
      virtual void updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const = 0;
};

template<typename T, typename V>
class QoreQAbstractItemDelegateBase : public QoreQObjectBase<T, V>
{
   public:
      DLLLOCAL QoreQAbstractItemDelegateBase(T *qo) : QoreQObjectBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QAbstractItemDelegate *getQAbstractItemDelegate() const
      {
         return &(*this->qobj);
      }
      DLLLOCAL virtual QWidget * createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
      { 
	 return this->qobj->parent_createEditor(parent, option, index); 
      }
      DLLLOCAL virtual bool editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index ) 
      {
	 return this->qobj->parent_editorEvent(event, model, option, index); 
      }
      DLLLOCAL virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const 
      {
	 this->qobj->parent_paint(painter, option, index); 
      }
      DLLLOCAL virtual void setEditorData ( QWidget * editor, const QModelIndex & index ) const 
      {
	 this->qobj->parent_setEditorData(editor, index); 
      }
      DLLLOCAL virtual void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const 
      { 
	 this->qobj->parent_setModelData(editor, model, index); 
      }
      DLLLOCAL virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const 
      { 
	 return this->qobj->parent_sizeHint(option, index); 
      }
      DLLLOCAL virtual void updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const 
      {
	 this->qobj->parent_updateEditorGeometry(editor, option, index); 
      }
};

template<typename T, typename V>
class QoreQtQAbstractItemDelegateBase : public QoreQtQObjectBase<T, V>
{
   public:
      DLLLOCAL QoreQtQAbstractItemDelegateBase(QoreObject *obj, T *qo) : QoreQtQObjectBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QAbstractItemDelegate *getQAbstractItemDelegate() const
      {
         return this->qobj;
      }

      // these functions will never be called
      DLLLOCAL virtual QWidget * createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
      { 
	 return 0;
      }
      DLLLOCAL virtual bool editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index ) 
      {
	 return false;
      }
      DLLLOCAL virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
      {
      }
      DLLLOCAL virtual void setEditorData ( QWidget * editor, const QModelIndex & index ) const 
      {
      }
      DLLLOCAL virtual void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const 
      { 
      }
      DLLLOCAL virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const 
      { 
	 return QSize();
      }
      DLLLOCAL virtual void updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const 
      {
      }
};


class QoreQAbstractItemDelegateExtension : public QoreQObjectExtension
{
   protected:
      const QoreMethod *m_createEditor, *m_editorEvent, *m_paint, *m_setEditorData, 
	 *m_setModelData, *m_sizeHint, *m_updateEditorGeometry;

      DLLLOCAL QoreQAbstractItemDelegateExtension(QoreObject *obj, QObject *qo) : QoreQObjectExtension(obj, qo)
      {
	 const QoreClass *qc = obj->getClass();
         m_createEditor           = findMethod(qc, "createEditor");
         m_editorEvent            = findMethod(qc, "editorEvent");
         m_paint                  = findMethod(qc, "paint");
         m_setEditorData          = findMethod(qc, "setEditorData");
         m_setModelData           = findMethod(qc, "setModelData");
         m_sizeHint               = findMethod(qc, "sizeHint");
         m_updateEditorGeometry   = findMethod(qc, "updateEditorGeometry");
      }
};

#endif
