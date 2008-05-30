/*
 QC_QMimeData.h
 
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

#ifndef _QORE_QT_QC_QMIMEDATA_H

#define _QORE_QT_QC_QMIMEDATA_H

#include <QMimeData>
#include "QoreAbstractQObject.h"

DLLEXPORT extern qore_classid_t CID_QMIMEDATA;
DLLEXPORT extern QoreClass *QC_QMimeData;

DLLEXPORT QoreClass *initQMimeDataClass(QoreClass *);

class myQMimeData : public QMimeData, public QoreQObjectExtension
{
#define QOREQTYPE QMimeData
#define MYQOREQTYPE myQMimeData
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQMimeData(QoreObject *obj) : QMimeData(), QoreQObjectExtension(obj, this)
      {
         
      }
};

typedef QoreQObjectBase<myQMimeData, QoreAbstractQObject> QoreQMimeDataImpl;

class QoreQMimeData : public QoreQMimeDataImpl
{
   public:
      DLLLOCAL QoreQMimeData(QoreObject *obj) : QoreQMimeDataImpl(new myQMimeData(obj))
      {
      }
};

typedef QoreQtQObjectBase<QMimeData, QoreAbstractQObject> QoreQtQMimeDataImpl;

class QoreQtQMimeData : public QoreQtQMimeDataImpl
{
   public:
      DLLLOCAL QoreQtQMimeData(QoreObject *o, QMimeData *qm, bool managed = true) : QoreQtQMimeDataImpl(o, qm, managed)
      {
      }
};

#endif // _QORE_QT_QC_QMIMEDATA_H
