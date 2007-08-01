/*
 QC_QTimer.h
 
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

#ifndef _QORE_QC_QTIMER_H

#define _QORE_QC_QTIMER_H

#include "QoreAbstractQLayout.h"

#include <QTimer>

DLLEXPORT extern int CID_QTIMER;

DLLLOCAL class QoreClass *initQTimerClass(class QoreClass *qobject);

class myQTimer : public QTimer
{
#define QOREQTYPE QTimer
#include "qore-qt-metacode.h"
#undef QOREQTYPE

      DLLLOCAL myQTimer(Object *obj, QObject *parent) : QTimer(parent)
      {
	 init(obj);
      }      
};

class QoreQTimer : public QoreAbstractQObject
{
   public:
      QPointer<myQTimer> qobj;

      DLLLOCAL QoreQTimer(Object *obj, QObject *parent = 0) : qobj(new myQTimer(obj, parent))
      {
      }

      DLLLOCAL virtual QObject *getQObject() const
      {
	 return static_cast<QObject *>(&(*qobj));
      }

      QORE_VIRTUAL_QOBJECT_METHODS
};

#endif
