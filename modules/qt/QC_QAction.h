/*
 QC_QAction.h
 
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

#ifndef _QORE_QT_QC_QACTION_H

#define _QORE_QT_QC_QACTION_H

#include <QAction>
#include "QoreAbstractQAction.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QACTION;
DLLLOCAL extern class QoreClass *QC_QAction;

DLLLOCAL class QoreClass *initQActionClass(QoreClass *);

class myQAction : public QAction, public QoreQObjectExtension
{
#define QOREQTYPE QAction
#define MYQOREQTYPE myQAction
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQAction(QoreObject *obj, QObject* parent) : QAction(parent), QoreQObjectExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQAction(QoreObject *obj, const QString& text, QObject* parent) : QAction(text, parent), QoreQObjectExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQAction(QoreObject *obj, const QIcon& icon, const QString& text, QObject* parent) : QAction(icon, text, parent), QoreQObjectExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQAction : public QoreAbstractQAction
{
   public:
      QPointer<myQAction> qobj;

      DLLLOCAL QoreQAction(QoreObject *obj, QObject* parent) : qobj(new myQAction(obj, parent))
      {
      }
      DLLLOCAL QoreQAction(QoreObject *obj, const QString& text, QObject* parent) : qobj(new myQAction(obj, text, parent))
      {
      }
      DLLLOCAL QoreQAction(QoreObject *obj, const QIcon& icon, const QString& text, QObject* parent) : qobj(new myQAction(obj, icon, text, parent))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QAction *getQAction() const
      {
         return static_cast<QAction *>(&(*qobj));
      }
      QORE_VIRTUAL_QOBJECT_METHODS
};

class QoreQtQAction : public QoreAbstractQAction
{
   public:
      QoreObject *qore_obj;
      QPointer<QAction> qobj;

      DLLLOCAL QoreQtQAction(QoreObject *obj, QAction *qaction) : qore_obj(obj), qobj(qaction)
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QAction *getQAction() const
      {
         return static_cast<QAction *>(&(*qobj));
      }
#include "qore-qt-static-qobject-methods.h"
};

#endif // _QORE_QT_QC_QACTION_H
