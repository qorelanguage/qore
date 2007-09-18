/*
 QC_QTableWidgetItem.h
 
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

#ifndef _QORE_QT_QC_QTABLEWIDGETITEM_H

#define _QORE_QT_QC_QTABLEWIDGETITEM_H

#include <QTableWidgetItem>

DLLLOCAL extern int CID_QTABLEWIDGETITEM;
DLLLOCAL extern class QoreClass *QC_QTableWidgetItem;

DLLLOCAL class QoreClass *initQTableWidgetItemClass();

class QoreQTableWidgetItem : public AbstractPrivateData, public QTableWidgetItem
{
   public:
      DLLLOCAL QoreQTableWidgetItem(int type = QTableWidgetItem::Type) : QTableWidgetItem(type)
      {
      }
      DLLLOCAL QoreQTableWidgetItem(const QString& text, int type = QTableWidgetItem::Type) : QTableWidgetItem(text, type)
      {
      }
      DLLLOCAL QoreQTableWidgetItem(const QIcon& icon, const QString& text, int type = QTableWidgetItem::Type) : QTableWidgetItem(icon, text, type)
      {
      }
      DLLLOCAL QoreQTableWidgetItem(const QTableWidgetItem& other) : QTableWidgetItem(other)
      {
      }
};

#endif // _QORE_QT_QC_QTABLEWIDGETITEM_H
