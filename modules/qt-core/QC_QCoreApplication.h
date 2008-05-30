/*
 QC_QCoreApplication.h
 
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

#ifndef _QORE_QT_QC_QCOREAPPLICATION_H

#define _QORE_QT_QC_QCOREAPPLICATION_H

#include <QCoreApplication>
#include "QoreAbstractQCoreApplication.h"

DLLEXPORT extern qore_classid_t CID_QCOREAPPLICATION;
DLLEXPORT extern QoreClass *QC_QCoreApplication;

DLLEXPORT QoreClass *initQCoreApplicationClass(QoreClass *);
DLLEXPORT void initQCoreApplicationStaticFunctions();

class myQCoreApplication : public QCoreApplication, public QoreQObjectExtension
{
#define QOREQTYPE QCoreApplication
#define MYQOREQTYPE myQCoreApplication
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQCoreApplication(QoreObject *obj, int& argc, char ** argv) : QCoreApplication(argc, argv), QoreQObjectExtension(obj, this)
      {
         
      }
};

typedef QoreQCoreApplicationBase<myQCoreApplication, QoreAbstractQCoreApplication> QoreQCoreApplicationImpl;

class QoreQCoreApplication : public QoreQCoreApplicationImpl
{
   public:
      DLLLOCAL QoreQCoreApplication(QoreObject *obj) : QoreQCoreApplicationImpl(new myQCoreApplication(obj, static_argc, static_argv))
      {
      }
      DLLLOCAL QoreQCoreApplication(QoreObject *obj, int& argc, char ** argv) : QoreQCoreApplicationImpl(new myQCoreApplication(obj, argc, argv))
      {
      }
};

typedef QoreQtQCoreApplicationBase<QCoreApplication, QoreAbstractQCoreApplication> QoreQtQCoreApplicationImpl;

class QoreQtQCoreApplication : public QoreQtQCoreApplicationImpl
{
  public:
   DLLLOCAL QoreQtQCoreApplication(QoreObject *obj, QCoreApplication *qcoreapplication) : QoreQtQCoreApplicationImpl(obj, qcoreapplication)
   {
   }
};

#endif // _QORE_QT_QC_QCOREAPPLICATION_H
