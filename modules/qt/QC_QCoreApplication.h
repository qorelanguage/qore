/*
 QC_QCoreApplication.h
 
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

#ifndef _QORE_QT_QC_QCOREAPPLICATION_H

#define _QORE_QT_QC_QCOREAPPLICATION_H

#include <QCoreApplication>
#include "QoreAbstractQCoreApplication.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QCOREAPPLICATION;
DLLLOCAL extern QoreClass *QC_QCoreApplication;

DLLLOCAL QoreClass *initQCoreApplicationClass(QoreClass *);
DLLLOCAL void initQCoreApplicationStaticFunctions();

class myQCoreApplication : public QCoreApplication, public QoreQObjectExtension
{
#define QOREQTYPE QCoreApplication
#include "qore-qt-metacode.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQCoreApplication(Object *obj, int& argc, char ** argv) : QCoreApplication(argc, argv), QoreQObjectExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQCoreApplication : public QoreAbstractQCoreApplication
{
   public:
      QPointer<myQCoreApplication> qobj;

      DLLLOCAL QoreQCoreApplication(Object *obj) : qobj(new myQCoreApplication(obj, static_argc, static_argv))
      {
      }
      DLLLOCAL QoreQCoreApplication(Object *obj, int& argc, char ** argv) : qobj(new myQCoreApplication(obj, argc, argv))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QCoreApplication *getQCoreApplication() const
      {
         return static_cast<QCoreApplication *>(&(*qobj));
      }
      QORE_VIRTUAL_QOBJECT_METHODS
};

class QoreQtQCoreApplication : public QoreAbstractQCoreApplication
{
  public:
   Object *qore_obj;
   QPointer<QCoreApplication> qobj;

   DLLLOCAL QoreQtQCoreApplication(Object *obj, QCoreApplication *qcoreapplication) : qore_obj(obj), qobj(qcoreapplication)
   {
   }
   DLLLOCAL virtual class QObject *getQObject() const
   {
      return static_cast<QObject *>(&(*qobj));
   }
   DLLLOCAL virtual class QCoreApplication *getQCoreApplication() const
   {
      return static_cast<QCoreApplication *>(&(*qobj));
   }
#include "qore-qt-static-qobject-methods.h"
};

#endif // _QORE_QT_QC_QCOREAPPLICATION_H
