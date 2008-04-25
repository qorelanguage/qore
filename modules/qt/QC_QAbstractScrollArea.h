/*
 QC_QAbstractScrollArea.h
 
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

#ifndef _QORE_QT_QC_QABSTRACTSCROLLAREA_H

#define _QORE_QT_QC_QABSTRACTSCROLLAREA_H

#include <QAbstractScrollArea>
#include "QoreAbstractQAbstractScrollArea.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QABSTRACTSCROLLAREA;
DLLLOCAL extern class QoreClass *QC_QAbstractScrollArea;

DLLLOCAL class QoreClass *initQAbstractScrollAreaClass(QoreClass *);

class myQAbstractScrollArea : public QAbstractScrollArea, public QoreQWidgetExtension
{
#define QOREQTYPE QAbstractScrollArea
#define MYQOREQTYPE myQAbstractScrollArea
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQAbstractScrollArea(QoreObject *obj, QWidget* parent = 0) : QAbstractScrollArea(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL void pub_setupViewport(QWidget *w)
      {
	 setupViewport(w);
      }
};

typedef QoreQAbstractScrollAreaBase<myQAbstractScrollArea, QoreAbstractQAbstractScrollArea> QoreQAbstractScrollAreaImpl;

class QoreQAbstractScrollArea : public QoreQAbstractScrollAreaImpl
{
   public:
      DLLLOCAL QoreQAbstractScrollArea(QoreObject *obj, QWidget* parent = 0) : QoreQAbstractScrollAreaImpl(new myQAbstractScrollArea(obj, parent))
      {
      }
};

#endif // _QORE_QT_QC_QABSTRACTSCROLLAREA_H
