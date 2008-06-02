/*
 QC_QTimeLine.h
 
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

#ifndef _QORE_QT_QC_QTIMELINE_H

#define _QORE_QT_QC_QTIMELINE_H

#include <QTimeLine>
#include "QoreAbstractQObject.h"

DLLEXPORT extern int CID_QTIMELINE;
DLLEXPORT extern QoreClass *QC_QTimeLine;
DLLEXPORT QoreNamespace *initQTimeLineNS(QoreClass *qobject);

class myQTimeLine : public QTimeLine, public QoreQObjectExtension
{
#define QOREQTYPE QTimeLine
#define MYQOREQTYPE myQTimeLine
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQTimeLine(QoreObject *obj, int duration = 1000, QObject* parent = 0) : QTimeLine(duration, parent), QoreQObjectExtension(obj, this)
      {
      }
};

typedef QoreQObjectBase<myQTimeLine, QoreAbstractQObject> QoreQTimeLineImpl;

class QoreQTimeLine : public QoreQTimeLineImpl
{
   public:
      DLLLOCAL QoreQTimeLine(QoreObject *obj, int duration = 1000, QObject* parent = 0) : QoreQTimeLineImpl(new myQTimeLine(obj, duration, parent))
      {
      }
};

typedef QoreQtQObjectBase<QTimeLine, QoreAbstractQObject> QoreQtQTimeLineImpl;

class QoreQtQTimeLine : public QoreQtQTimeLineImpl
{
   public:
      DLLLOCAL QoreQtQTimeLine(QoreObject *obj, QTimeLine *qtimeline) : QoreQtQTimeLineImpl(obj, qtimeline)
      {
      }
};

#endif // _QORE_QT_QC_QTIMELINE_H
