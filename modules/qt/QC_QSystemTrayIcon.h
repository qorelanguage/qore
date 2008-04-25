/*
 QC_QSystemTrayIcon.h
 
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

#ifndef _QORE_QT_QC_QSYSTEMTRAYICON_H

#define _QORE_QT_QC_QSYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include "QoreAbstractQObject.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QSYSTEMTRAYICON;
DLLLOCAL extern class QoreClass *QC_QSystemTrayIcon;

DLLLOCAL QoreNamespace *initQSystemTrayIconNS(QoreClass *);
DLLLOCAL void initQSystemTrayIconStaticFunctions();

class myQSystemTrayIcon : public QSystemTrayIcon, public QoreQObjectExtension
{
#define QOREQTYPE QSystemTrayIcon
#define MYQOREQTYPE myQSystemTrayIcon
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQSystemTrayIcon(QoreObject *obj, QObject* parent = 0) : QSystemTrayIcon(parent), QoreQObjectExtension(obj, this)
      {
      }
      DLLLOCAL myQSystemTrayIcon(QoreObject *obj, const QIcon& icon, QObject* parent = 0) : QSystemTrayIcon(icon, parent), QoreQObjectExtension(obj, this)
      {
      }
};

typedef QoreQObjectBase<myQSystemTrayIcon, QoreAbstractQObject> QoreQSystemTrayIconImpl; 

class QoreQSystemTrayIcon : public QoreQSystemTrayIconImpl
{
   public:
      DLLLOCAL QoreQSystemTrayIcon(QoreObject *obj, QObject* parent = 0) : QoreQSystemTrayIconImpl(new myQSystemTrayIcon(obj, parent))
      {
      }
      DLLLOCAL QoreQSystemTrayIcon(QoreObject *obj, const QIcon& icon, QObject* parent = 0) : QoreQSystemTrayIconImpl(new myQSystemTrayIcon(obj, icon, parent))
      {
      }
};

#endif // _QORE_QT_QC_QSYSTEMTRAYICON_H
