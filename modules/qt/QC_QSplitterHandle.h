/*
 QC_QSplitterHandle.h
 
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

#ifndef _QORE_QT_QC_QSPLITTERHANDLE_H

#define _QORE_QT_QC_QSPLITTERHANDLE_H

#include <QSplitterHandle>
#include "QoreAbstractQSplitterHandle.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QSPLITTERHANDLE;
DLLLOCAL extern QoreClass *QC_QSplitterHandle;
DLLLOCAL QoreClass *initQSplitterHandleClass(QoreClass *);

class myQSplitterHandle : public QSplitterHandle, public QoreQWidgetExtension
{
#define QOREQTYPE QSplitterHandle
#define MYQOREQTYPE myQSplitterHandle
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQSplitterHandle(QoreObject *obj, Qt::Orientation orientation, QSplitter* parent) : QSplitterHandle(orientation, parent), QoreQWidgetExtension(obj, this)
      {
      }

      int parent_closestLegalPosition ( int pos )
      {
	 return QSplitterHandle::closestLegalPosition(pos);
      }

      void parent_moveSplitter ( int pos )
      {
	 return QSplitterHandle::moveSplitter(pos);
      }
};

typedef QoreQSplitterHandleBase<myQSplitterHandle, QoreAbstractQSplitterHandle> QoreQSplitterHandleImpl;

class QoreQSplitterHandle : public QoreQSplitterHandleImpl
{
   public:
      DLLLOCAL QoreQSplitterHandle(QoreObject *obj, Qt::Orientation orientation, QSplitter* parent) : QoreQSplitterHandleImpl(new myQSplitterHandle(obj, orientation, parent))
      {
      }
};

typedef QoreQtQSplitterHandleBase<QSplitterHandle, QoreAbstractQSplitterHandle> QoreQtQSplitterHandleImpl;

class QoreQtQSplitterHandle : public QoreQtQSplitterHandleImpl
{
   public:
      DLLLOCAL QoreQtQSplitterHandle(QoreObject *obj, QSplitterHandle *qsplitterhandle) : QoreQtQSplitterHandleImpl(obj, qsplitterhandle)
      {
      }
};

#endif // _QORE_QT_QC_QSPLITTERHANDLE_H
