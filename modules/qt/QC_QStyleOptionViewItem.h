/*
 QC_QStyleOptionViewItem.h
 
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

#ifndef _QORE_QT_QC_QSTYLEOPTIONVIEWITEM_H

#define _QORE_QT_QC_QSTYLEOPTIONVIEWITEM_H

#include <QStyleOptionViewItem>

DLLEXPORT extern qore_classid_t CID_QSTYLEOPTIONVIEWITEM;
DLLEXPORT extern class QoreClass *QC_QStyleOptionViewItem;

DLLEXPORT class QoreClass *initQStyleOptionViewItemClass(QoreClass *);

class QoreQStyleOptionViewItem : public AbstractPrivateData, public QStyleOptionViewItem
{
   public:
      DLLLOCAL QoreQStyleOptionViewItem() : QStyleOptionViewItem()
      {
      }
      DLLLOCAL QoreQStyleOptionViewItem(const QStyleOptionViewItem &sovi) : QStyleOptionViewItem(sovi)
      {
      }
};

#endif // _QORE_QT_QC_QSTYLEOPTIONVIEWITEM_H
