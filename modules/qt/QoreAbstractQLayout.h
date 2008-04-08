/*
 QoreAbstractQLayout.h
 
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

#ifndef _QORE_QOREABSTRACTQLAYOUT_H

#define _QORE_QOREABSTRACTQLAYOUT_H

#include "QoreAbstractQObject.h"
#include "QoreAbstractQWidget.h"
#include "QoreAbstractQLayoutItem.h"

extern qore_classid_t CID_QWIDGET;

class QoreAbstractQLayout : public QoreAbstractQObject, public QoreAbstractQLayoutItem
{
   public:
      DLLLOCAL virtual QLayout *getQLayout() const = 0;

      DLLLOCAL virtual void addItem ( QLayoutItem * item ) = 0;
      DLLLOCAL virtual int count () const = 0;
      DLLLOCAL virtual int indexOf ( QWidget * widget ) const = 0;
      DLLLOCAL virtual QLayoutItem * itemAt ( int index ) const = 0;
      DLLLOCAL virtual QLayoutItem * takeAt ( int index ) = 0;
};

class QoreQLayoutExtension : public QoreQObjectExtension, public QoreQLayoutItemExtensionBase
{
   protected:
      const QoreMethod *m_addItem, *m_count, *m_indexOf, *m_itemAt, *m_takeAt;
      bool item_externally_owned;
      bool manual_delete;

   public:
      DLLLOCAL QoreQLayoutExtension(QoreObject *obj, QObject *qo) : QoreQObjectExtension(obj, qo), QoreQLayoutItemExtensionBase(obj->getClass())
      {
	 const QoreClass *oc = obj->getClass();
         m_addItem                = oc->findMethod("addItem");
         m_count                  = oc->findMethod("count");
         m_indexOf                = oc->findMethod("indexOf");
         m_itemAt                 = oc->findMethod("itemAt");
         m_takeAt                 = oc->findMethod("takeAt");
      }

      DLLLOCAL ~QoreQLayoutExtension()
      {
	 if (manual_delete && qore_obj->isValid()) {
	    ExceptionSink xsink;
	    qore_obj->doDelete(&xsink);
	 }
      }

};

template<typename T, typename V>
class QoreQLayoutBase : public QoreQObjectBase<T, V>
{
   public:
      DLLLOCAL QoreQLayoutBase(T *qo) : QoreQObjectBase<T, V>(qo)
      {
      }

      DLLLOCAL virtual QLayout *getQLayout() const
      {
         return &(*this->qobj);
      }

      DLLLOCAL virtual QLayoutItem *getQLayoutItem() const
      {
         return &(*this->qobj);
      }

      DLLLOCAL virtual bool layoutItemDeleteBlocker()
      {
	 return this->qobj ? this->qobj->layoutItemDeleteBlocker() : false;
      }

      DLLLOCAL virtual void setItemExternallyOwned()
      {
	 this->qobj->setItemExternallyOwned();
      }

      DLLLOCAL virtual bool parent_hasHeightForWidth () const
      {
	 return this->qobj->parent_hasHeightForWidth();
      }

      DLLLOCAL virtual int parent_heightForWidth ( int w ) const
      {
	 return this->qobj->parent_heightForWidth(w);
      }

      DLLLOCAL virtual void parent_invalidate ()
      {
	 return this->qobj->parent_invalidate();
      }

      DLLLOCAL virtual QLayout * parent_layout ()
      {
	 return this->qobj->parent_layout();
      }

      DLLLOCAL virtual int parent_minimumHeightForWidth ( int w ) const
      {
	 return this->qobj->parent_minimumHeightForWidth(w);
      }

      DLLLOCAL virtual QSpacerItem * parent_spacerItem ()
      {
	 return this->qobj->parent_spacerItem();
      }

      DLLLOCAL virtual QWidget * parent_widget ()
      {
	 return this->qobj->parent_widget();
      }

      DLLLOCAL virtual Qt::Orientations parent_expandingDirections () const
      {
	 return this->qobj->parent_expandingDirections();
      }

      DLLLOCAL virtual QRect parent_geometry () const
      {
	 return this->qobj->parent_geometry();
      }

      DLLLOCAL virtual bool parent_isEmpty () const
      {
	 return this->qobj->parent_isEmpty();
      }

      DLLLOCAL virtual QSize parent_maximumSize () const
      {
	 return this->qobj->parent_maximumSize();
      }

      DLLLOCAL virtual QSize parent_minimumSize () const
      {
	 return this->qobj->parent_minimumSize();
      }

      DLLLOCAL virtual void parent_setGeometry ( const QRect & r )
      {
	 return this->qobj->parent_setGeometry(r);
      }

      DLLLOCAL virtual QSize parent_sizeHint () const
      {
	 return this->qobj->parent_sizeHint();
      }

      DLLLOCAL virtual void addItem ( QLayoutItem * item )
      {
	 this->qobj->parent_addItem(item);
      }

      DLLLOCAL virtual int count () const
      {
	 return this->qobj->parent_count();
      }

      DLLLOCAL virtual int indexOf ( QWidget * widget ) const
      {
	 return this->qobj->parent_indexOf(widget);
      }

      DLLLOCAL virtual QLayoutItem * itemAt ( int index ) const
      {
	 return this->qobj->parent_itemAt(index);
      }

      DLLLOCAL virtual QLayoutItem * takeAt ( int index )
      {
	 return this->qobj->parent_takeAt(index);
      }
};

template<typename T, typename V>
class QoreQtQLayoutBase : public QoreQtQObjectBase<T, V>
{
   public:
      DLLLOCAL QoreQtQLayoutBase(QoreObject *obj, T *qo) : QoreQtQObjectBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QLayout *getQLayout() const
      {
         return this->qobj;
      }

      DLLLOCAL virtual QLayoutItem *getQLayoutItem() const
      {
         return this->qobj;
      }

      DLLLOCAL virtual bool layoutItemDeleteBlocker()
      {
	 return false;
      }

      DLLLOCAL virtual void setItemExternallyOwned()
      {
      }

      DLLLOCAL virtual bool parent_hasHeightForWidth () const
      {
	 return this->qobj->hasHeightForWidth();
      }

      DLLLOCAL virtual int parent_heightForWidth ( int w ) const
      {
	 return this->qobj->heightForWidth(w);
      }

      DLLLOCAL virtual void parent_invalidate ()
      {
	 return this->qobj->invalidate();
      }

      DLLLOCAL virtual QLayout * parent_layout ()
      {
	 return this->qobj->layout();
      }

      DLLLOCAL virtual int parent_minimumHeightForWidth ( int w ) const
      {
	 return this->qobj->minimumHeightForWidth(w);
      }

      DLLLOCAL virtual QSpacerItem * parent_spacerItem ()
      {
	 return this->qobj->spacerItem();
      }

      DLLLOCAL virtual QWidget * parent_widget ()
      {
	 return this->qobj->widget();
      }

      DLLLOCAL virtual Qt::Orientations parent_expandingDirections () const
      {
	 return this->qobj->expandingDirections();
      }

      DLLLOCAL virtual QRect parent_geometry () const
      {
	 return this->qobj->geometry();
      }

      DLLLOCAL virtual bool parent_isEmpty () const
      {
	 return this->qobj->isEmpty();
      }

      DLLLOCAL virtual QSize parent_maximumSize () const
      {
	 return this->qobj->maximumSize();
      }

      DLLLOCAL virtual QSize parent_minimumSize () const
      {
	 return this->qobj->minimumSize();
      }

      DLLLOCAL virtual void parent_setGeometry ( const QRect & r )
      {
	 return this->qobj->setGeometry(r);
      }

      DLLLOCAL virtual QSize parent_sizeHint () const
      {
	 return this->qobj->sizeHint();
      }

      DLLLOCAL virtual void addItem ( QLayoutItem * item )
      {
	 this->qobj->addItem(item);
      }

      DLLLOCAL virtual int count () const
      {
	 return this->qobj->count();
      }

      DLLLOCAL virtual int indexOf ( QWidget * widget ) const
      {
	 return this->qobj->indexOf(widget);
      }

      DLLLOCAL virtual QLayoutItem * itemAt ( int index ) const
      {
	 return this->qobj->itemAt(index);
      }

      DLLLOCAL virtual QLayoutItem * takeAt ( int index )
      {
	 return this->qobj->takeAt(index);
      }

};

#endif
