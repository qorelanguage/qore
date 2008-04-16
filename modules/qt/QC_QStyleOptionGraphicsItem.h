/*
 QC_QStyleOptionGraphicsItem.h
 
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

#ifndef _QORE_QT_QC_QSTYLEOPTIONGRAPHICSITEM_H

#define _QORE_QT_QC_QSTYLEOPTIONGRAPHICSITEM_H

#include <QStyleOptionGraphicsItem>

DLLLOCAL extern qore_classid_t CID_QSTYLEOPTIONGRAPHICSITEM;
DLLLOCAL extern QoreClass *QC_QStyleOptionGraphicsItem;
DLLLOCAL QoreNamespace *initQStyleOptionGraphicsItemNS(QoreClass *);

class QoreQStyleOptionGraphicsItem : public AbstractPrivateData, public QStyleOptionGraphicsItem
{
   public:
      DLLLOCAL QoreQStyleOptionGraphicsItem() : QStyleOptionGraphicsItem()
      {
      }
      DLLLOCAL QoreQStyleOptionGraphicsItem(const QStyleOptionGraphicsItem& other) : QStyleOptionGraphicsItem(other)
      {
      }
};

#endif // _QORE_QT_QC_QSTYLEOPTIONGRAPHICSITEM_H
