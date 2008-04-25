/*
 QC_QClipboard.h
 
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

#ifndef _QORE_QT_QC_QCLIPBOARD_H

#define _QORE_QT_QC_QCLIPBOARD_H

#include <QClipboard>
#include "QoreAbstractQObject.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QCLIPBOARD;
DLLLOCAL extern QoreClass *QC_QClipboard;

DLLLOCAL QoreClass *initQClipboardClass(QoreClass *);

typedef QoreQtQObjectPrivateBase<QClipboard, QoreAbstractQObject> QoreQClipboardImpl;

class QoreQClipboard : public QoreQClipboardImpl
{
   protected:
      DLLLOCAL ~QoreQClipboard()
      {
      }

   public:
      DLLLOCAL QoreQClipboard(QoreObject *obj, QClipboard *cb) : QoreQClipboardImpl(obj, cb)
      {
	 // set pointer to object owner as a property
         qobj->setProperty("qobject", reinterpret_cast<qulonglong>(obj));
      }
};

#endif // _QORE_QT_QC_QCLIPBOARD_H
