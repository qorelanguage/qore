/*
 QoreAbstractQLayoutItem.h
 
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

#ifndef _QORE_QT_QOREABSTRACTQLAYOUTITEM_H

#define _QORE_QT_QOREABSTRACTQLAYOUTITEM_H

#include "QLayout"

#include "QC_QRect.h"
#include "QC_QWidget.h"

DLLEXPORT extern qore_classid_t CID_QLAYOUTITEM;
DLLEXPORT extern QoreClass *QC_QLayoutItem;

class QoreAbstractQLayoutItem
{
   public:
      DLLLOCAL virtual ~QoreAbstractQLayoutItem()
      {
      }
      DLLLOCAL virtual class QLayoutItem *getQLayoutItem() const = 0;
      
      DLLLOCAL virtual Qt::Orientations parent_expandingDirections () const = 0;
      DLLLOCAL virtual QRect parent_geometry () const = 0;
      DLLLOCAL virtual bool parent_hasHeightForWidth () const = 0;
      DLLLOCAL virtual int parent_heightForWidth ( int w ) const = 0;
      DLLLOCAL virtual void parent_invalidate () = 0;
      DLLLOCAL virtual bool parent_isEmpty () const = 0;
      DLLLOCAL virtual QLayout *parent_layout () = 0;
      DLLLOCAL virtual QSize parent_maximumSize () const = 0;
      DLLLOCAL virtual int parent_minimumHeightForWidth ( int w ) const = 0;
      DLLLOCAL virtual QSize parent_minimumSize () const = 0;
      DLLLOCAL virtual void parent_setGeometry ( const QRect & r ) = 0;
      DLLLOCAL virtual QSize parent_sizeHint () const = 0;
      DLLLOCAL virtual QSpacerItem * parent_spacerItem () = 0;
      DLLLOCAL virtual QWidget * parent_widget () = 0;

      DLLLOCAL virtual bool layoutItemDeleteBlocker() = 0;
      DLLLOCAL virtual void setItemExternallyOwned() = 0;
};

class QoreQLayoutItemExtensionBase
{
   protected:
      const QoreMethod *m_expandingDirections, *m_geometry, *m_hasHeightForWidth,
	 *m_heightForWidth, *m_invalidate, *m_isEmpty, *m_layout, *m_maximumSize,
	 *m_minimumHeightForWidth, *m_minimumSize, *m_setGeometry, *m_sizeHint,
	 *m_spacerItem, *m_widget;

   public:
      DLLLOCAL QoreQLayoutItemExtensionBase(const QoreClass *oc)
      {
         m_expandingDirections    = oc->findMethod("expandingDirections");
         m_geometry               = oc->findMethod("geometry");
         m_hasHeightForWidth      = oc->findMethod("hasHeightForWidth");
	 m_heightForWidth         = oc->findMethod("heightForWidth");
         m_invalidate             = oc->findMethod("invalidate");
         m_isEmpty                = oc->findMethod("isEmpty");
         m_layout                 = oc->findMethod("layout");
         m_maximumSize            = oc->findMethod("maximumSize");
         m_minimumHeightForWidth  = oc->findMethod("minimumHeightForWidth");
         m_minimumSize            = oc->findMethod("minimumSize");
         m_setGeometry            = oc->findMethod("setGeometry");
         m_sizeHint               = oc->findMethod("sizeHint");
         m_spacerItem             = oc->findMethod("spacerItem");
         m_widget                 = oc->findMethod("widget");	 
      }
};

class QoreQLayoutItemExtension : public QoreQLayoutItemExtensionBase, public QoreQtEventDispatcher
{
   protected:
      QoreObject *qore_obj;
      bool item_externally_owned;
      bool manual_delete;

   public:
      DLLLOCAL QoreQLayoutItemExtension(QoreObject *obj) : QoreQLayoutItemExtensionBase(obj->getClass()), qore_obj(obj), item_externally_owned(false), manual_delete(false)
      {
	 qore_obj->tRef();
      }
      DLLLOCAL ~QoreQLayoutItemExtension()
      {
	 if (manual_delete && qore_obj->isValid()) {
	    ExceptionSink xsink;
	    qore_obj->doDelete(&xsink);
	 }

	 qore_obj->tDeref();
      }
};

class QoreAbstractQLayoutItemData : public AbstractPrivateData, public QoreAbstractQLayoutItem
{
};

template<typename T, typename V>
class QoreQtQLayoutItemImplBase : public V
{
   public:
      T *qobj;
      bool managed;

      DLLLOCAL QoreQtQLayoutItemImplBase(T *qo, bool n_managed = true) : qobj(qo), managed(n_managed)
      {
      }

      DLLLOCAL virtual ~QoreQtQLayoutItemImplBase()
      {
	 if (managed)
	    delete qobj;
      }

      DLLLOCAL virtual class QLayoutItem *getQLayoutItem() const
      {
         return qobj;
      }

      DLLLOCAL virtual bool layoutItemDeleteBlocker()
      {
	 return false;
      }

      DLLLOCAL virtual void setItemExternallyOwned()
      {
	 managed = false;
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
};

template<typename T, typename V>
class QoreQtQLayoutItemBase : QoreQtQLayoutItemImplBase<T, V>
{
   public:
      DLLLOCAL QoreQtQLayoutItemBase(T *qo, bool n_managed = true) : QoreQtQLayoutItemImplBase<T, V>(qo, n_managed)
      {
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
};


#endif  // _QORE_QT_QOREABSTRACTQLAYOUT_H
