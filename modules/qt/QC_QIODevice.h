/*
 QC_QIODevice.h
 
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

#ifndef _QORE_QT_QC_QIODEVICE_H

#define _QORE_QT_QC_QIODEVICE_H

#include <QIODevice>
#include "QoreAbstractQIODevice.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QIODEVICE;
DLLLOCAL extern class QoreClass *QC_QIODevice;

DLLLOCAL class QoreClass *initQIODeviceClass(QoreClass *);

class QoreQtQIODevice : public QoreAbstractQIODevice
{
   public:
      QoreObject *qore_obj;
      QPointer<QIODevice> qobj;

      DLLLOCAL QoreQtQIODevice(QoreObject *obj, QIODevice* qiod) : qore_obj(obj), qobj(qiod)
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QIODevice *getQIODevice() const
      {
         return static_cast<QIODevice *>(&(*qobj));
      }
#include "qore-qt-static-qobject-methods.h"
};

#endif // _QORE_QT_QC_QIODEVICE_H
