/*
 QC_QApplication.h
 
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

#ifndef _QORE_QC_QAPPLICATION_H

#define _QORE_QC_QAPPLICATION_H

#include "QoreAbstractQObject.h"

#include <QApplication>

extern int CID_QAPPLICATION;

DLLLOCAL class QoreClass *initQApplicationClass(class QoreClass *parent);
DLLLOCAL void initQApplicationStaticFunctions();

extern int static_argc;
extern char **static_argv;

DLLLOCAL extern void qapp_dec();
DLLLOCAL extern QoreNode *get_qore_qapp();

class myQApplication : public QApplication, public QoreQObjectExtension
{
#define QOREQTYPE QApplication
#include "qore-qt-metacode.h"
#undef QOREQTYPE

      DLLLOCAL myQApplication(Object *obj, int &argc, char **argv) : QApplication(argc, argv), QoreQObjectExtension(obj->getClass())
      {
	 init(obj);
      }
};

class QoreQApplication : public QoreAbstractQObject
{
   public:
      myQApplication *qobj;

      DLLLOCAL QoreQApplication(Object *obj) : qobj(new myQApplication(obj, static_argc, static_argv))
      {
      }

      DLLLOCAL ~QoreQApplication()
      {
	 qapp_dec();
      }

      DLLLOCAL virtual class QObject *getQObject() const
      {
	 return static_cast<QObject *>(qobj);
      }
      QORE_VIRTUAL_QOBJECT_METHODS
};


#endif
