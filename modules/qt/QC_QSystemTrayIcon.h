/*
 QC_QSystemTrayIcon.h
 
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

#ifndef _QORE_QT_QC_QSYSTEMTRAYICON_H

#define _QORE_QT_QC_QSYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include "QoreAbstractQObject.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QSYSTEMTRAYICON;
DLLLOCAL extern class QoreClass *QC_QSystemTrayIcon;

DLLLOCAL QoreNamespace *initQSystemTrayIconNS(QoreClass *);
DLLLOCAL void initQSystemTrayIconStaticFunctions();

class myQSystemTrayIcon : public QSystemTrayIcon, public QoreQObjectExtension
{
#define QOREQTYPE QSystemTrayIcon
#include "qore-qt-metacode.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQSystemTrayIcon(QoreObject *obj, QObject* parent = 0) : QSystemTrayIcon(parent), QoreQObjectExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQSystemTrayIcon(QoreObject *obj, const QIcon& icon, QObject* parent = 0) : QSystemTrayIcon(icon, parent), QoreQObjectExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQSystemTrayIcon : public QoreAbstractQObject
{
   public:
      QPointer<myQSystemTrayIcon> qobj;

      DLLLOCAL QoreQSystemTrayIcon(QoreObject *obj, QObject* parent = 0) : qobj(new myQSystemTrayIcon(obj, parent))
      {
      }
      DLLLOCAL QoreQSystemTrayIcon(QoreObject *obj, const QIcon& icon, QObject* parent = 0) : qobj(new myQSystemTrayIcon(obj, icon, parent))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      QORE_VIRTUAL_QOBJECT_METHODS
};

#endif // _QORE_QT_QC_QSYSTEMTRAYICON_H
