/*
 QC_QListWidgetItem.h
 
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

#ifndef _QORE_QT_QC_QLISTWIDGETITEM_H

#define _QORE_QT_QC_QLISTWIDGETITEM_H

#include <QListWidgetItem>

DLLLOCAL extern int CID_QLISTWIDGETITEM;
DLLLOCAL extern QoreClass *QC_QListWidgetItem;
DLLLOCAL Namespace *initQListWidgetItemNS();

class QoreQListWidgetItem : public AbstractPrivateData
{
   private:
      QListWidgetItem *item;
      bool managed;

   public:
      DLLLOCAL QoreQListWidgetItem(QListWidget* parent = 0, int type = QListWidgetItem::Type) : item(new QListWidgetItem(parent, type)), managed(parent ? false : true)
      {
      }
      DLLLOCAL QoreQListWidgetItem(QListWidgetItem *i) : item(i), managed(false)
      {
      }
      DLLLOCAL ~QoreQListWidgetItem()
      {
	 if (managed)
	    delete item;
      }
      DLLLOCAL QoreQListWidgetItem(const QString& text, QListWidget* parent = 0, int type = QListWidgetItem::Type) : item(new QListWidgetItem(text, parent, type)), managed(parent ? false :true)
      {
      }
      DLLLOCAL QoreQListWidgetItem(const QIcon& icon, const QString& text, QListWidget* parent = 0, int type = QListWidgetItem::Type) : item(new QListWidgetItem(icon, text, parent, type)), managed(parent ? false :true)
      {
      }
      DLLLOCAL QoreQListWidgetItem(const QListWidgetItem& other) : item(new QListWidgetItem(other)), managed(true)
      {
      }
      DLLLOCAL QListWidgetItem *getQListWidgetItem() const
      {
	 return item;
      }
};

#endif // _QORE_QT_QC_QLISTWIDGETITEM_H
