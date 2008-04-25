/*
 QC_QLayoutItem.h
 
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

#ifndef _QORE_QT_QC_QLAYOUTITEM_H

#define _QORE_QT_QC_QLAYOUTITEM_H

#include <QLayoutItem>
#include "QoreAbstractQLayoutItem.h"
#include "QC_QLayout.h"

DLLLOCAL extern qore_classid_t CID_QLAYOUTITEM;
DLLLOCAL extern QoreClass *QC_QLayoutItem;
DLLLOCAL QoreClass *initQLayoutItemClass();

class QoreQLayoutItem : public QoreAbstractQLayoutItemData, public QLayoutItem, public QoreQLayoutItemExtension
{
#define QORE_IS_QLAYOUTITEM
#define QOREQTYPE QLayoutItem
#include "qore-qt-qlayoutitem-methods.h"
#undef QOREQTYPE
#undef QORE_IS_QLAYOUTITEM

   public:
      DLLLOCAL QoreQLayoutItem(QoreObject *obj, Qt::Alignment alignment = 0) : QLayoutItem(alignment), QoreQLayoutItemExtension(obj)
      {
      }

      DLLLOCAL virtual QLayoutItem *getQLayoutItem() const
      {
         return const_cast<QLayoutItem *>(static_cast<const QLayoutItem *>(this));
      }
};

typedef QoreQtQLayoutItemImplBase<QLayoutItem, QoreAbstractQLayoutItemData> QoreQtQLayoutItemImpl;

class QoreQtQLayoutItem : public QoreQtQLayoutItemImpl
{
   public:
      DLLLOCAL QoreQtQLayoutItem(QLayoutItem *qo, bool n_managed = true) : QoreQtQLayoutItemImpl(qo, n_managed)
      {
      }

      // these functions are all pure virtual in the QLayoutItem
      DLLLOCAL virtual Qt::Orientations parent_expandingDirections () const
      {
	 return 0;
      }

      DLLLOCAL virtual QRect parent_geometry () const
      {
	 return QRect();
      }

      DLLLOCAL virtual bool parent_isEmpty () const
      {
	 return false;
      }

      DLLLOCAL virtual QSize parent_maximumSize () const
      {
	 return QSize();
      }

      DLLLOCAL virtual QSize parent_minimumSize () const
      {
	 return QSize();
      }

      DLLLOCAL virtual void parent_setGeometry ( const QRect & r )
      {
      }

      DLLLOCAL virtual QSize parent_sizeHint () const
      {
	 return QSize();
      }
};


#endif // _QORE_QT_QC_QLAYOUTITEM_H
