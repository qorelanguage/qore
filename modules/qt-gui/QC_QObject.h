/*
 QC_QObject.h
 
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

#ifndef _QORE_QC_QOBJECT_H

#define _QORE_QC_QOBJECT_H

#include "QoreAbstractQObject.h"

#include <QObject>

DLLEXPORT extern qore_classid_t CID_QOBJECT;
DLLEXPORT extern QoreClass *QC_QObject;

DLLEXPORT QoreClass *initQObjectClass();

class myQObject : public QObject, public QoreQObjectExtension 
{
#define QOREQTYPE QObject
#define MYQOREQTYPE myQObject
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE
   public:
      DLLLOCAL myQObject(QoreObject *obj, QObject *parent = 0) : QObject(parent), QoreQObjectExtension(obj, this)
      {
	 
      }
};

typedef QoreQObjectBase<myQObject, QoreAbstractQObject> QoreQObjectImpl;

class QoreQObject : public QoreQObjectImpl 
{
   public:
      DLLLOCAL QoreQObject(QoreObject *obj, QObject *parent = 0) : QoreQObjectImpl(new myQObject(obj, parent))
      {
      }
};

typedef QoreQtQObjectBase<QObject, QoreAbstractQObject> QoreQtQObjectImpl;

class QoreQtQObject : public QoreQtQObjectImpl
{
   public:
      DLLLOCAL QoreQtQObject(QoreObject *obj, QObject *qo) : QoreQtQObjectImpl(obj, qo)
      {
      }
};

#endif
