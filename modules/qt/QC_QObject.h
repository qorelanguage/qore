/*
 QC_QObject.h
 
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

#ifndef _QORE_QC_QOBJECT_H

#define _QORE_QC_QOBJECT_H

#include "QoreAbstractQObject.h"

#include <QObject>

DLLLOCAL extern qore_classid_t CID_QOBJECT;
DLLLOCAL extern QoreClass *QC_QObject;

DLLLOCAL class QoreClass *initQObjectClass();

class myQObject : public QObject, public QoreQObjectExtension 
{
#define QOREQTYPE QObject
#define MYQOREQTYPE myQObject
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE
   public:
      DLLLOCAL myQObject(QoreObject *obj, QObject *parent = 0) : QObject(parent), QoreQObjectExtension(obj->getClass())
      {
	 init(obj);
      }
};

class QoreQObject : public QoreAbstractQObject
{
   private:
   public:
      QPointer<myQObject> qobj;

      DLLLOCAL QoreQObject(QoreObject *obj, QObject *parent = 0) : qobj(new myQObject(obj, parent))
      {
      }

      DLLLOCAL virtual class QObject *getQObject() const
      {
	 return &*qobj;
      }

      QORE_VIRTUAL_QOBJECT_METHODS
};

class QoreQtQObject : public QoreAbstractQObject
{
   public:
      QoreObject *qore_obj;
      QPointer<QObject> qobj;

      DLLLOCAL QoreQtQObject(QoreObject *obj, QObject *qo) : qore_obj(obj), qobj(qo)
      {
      }

      DLLLOCAL virtual class QObject *getQObject() const
      {
	 return &*qobj;
      }

#include "qore-qt-static-qobject-methods.h"
};

#endif
