/*
 QC_QApplication.h
 
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

#ifndef _QORE_QC_QAPPLICATION_H

#define _QORE_QC_QAPPLICATION_H

#include "QoreAbstractQCoreApplication.h"

#include <QApplication>

extern qore_classid_t CID_QAPPLICATION;

DLLLOCAL class QoreClass *initQApplicationClass(class QoreClass *parent);
DLLLOCAL void initQApplicationStaticFunctions();

DLLLOCAL extern void qapp_dec();
DLLLOCAL extern AbstractQoreNode *get_qore_qapp();

class myQApplication : public QApplication, public QoreQObjectExtension
{
#define QOREQTYPE QApplication
#define MYQOREQTYPE myQApplication
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

      DLLLOCAL myQApplication(QoreObject *obj, int &argc, char **argv) : QApplication(argc, argv), QoreQObjectExtension(obj, this)
      {
	 
      }
};

typedef QoreQCoreApplicationBase<myQApplication, QoreAbstractQCoreApplication> QoreQApplicationImpl;

class QoreQApplication : public QoreQApplicationImpl
{
   public:
      DLLLOCAL QoreQApplication(QoreObject *obj) : QoreQApplicationImpl(new myQApplication(obj, static_argc, static_argv))
      {
      }

      DLLLOCAL ~QoreQApplication()
      {
	 qapp_dec();
      }
};


#endif
