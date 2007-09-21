/*
 QC_QClipboard.h
 
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

#ifndef _QORE_QT_QC_QCLIPBOARD_H

#define _QORE_QT_QC_QCLIPBOARD_H

#include <QClipboard>
#include "QoreAbstractQObject.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QCLIPBOARD;
DLLLOCAL extern class QoreClass *QC_QClipboard;

DLLLOCAL class QoreClass *initQClipboardClass(QoreClass *);

class QoreQClipboard : public QoreAbstractQObject
{
   private:
      Object *qore_obj;     // containing qore object

   protected:
      DLLLOCAL ~QoreQClipboard()
      {
	 
      }

   public:
      QPointer<QClipboard> qobj;

      DLLLOCAL QoreQClipboard(Object *obj, QClipboard *cb) : qore_obj(obj), qobj(cb)
      {
	 // set pointer to object owner as a property
         qobj->setProperty("qobject", reinterpret_cast<qulonglong>(obj));
      }

      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }

#include "qore-qt-static-qobject-methods.h"

};

#endif // _QORE_QT_QC_QCLIPBOARD_H
