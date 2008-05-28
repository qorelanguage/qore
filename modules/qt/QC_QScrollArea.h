/*
 QC_QScrollArea.h
 
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

#ifndef _QORE_QT_QC_QSCROLLAREA_H

#define _QORE_QT_QC_QSCROLLAREA_H

#include <QScrollArea>
#include "QoreAbstractQAbstractScrollArea.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QSCROLLAREA;
DLLEXPORT extern class QoreClass *QC_QScrollArea;

DLLEXPORT class QoreClass *initQScrollAreaClass(QoreClass *);

class myQScrollArea : public QScrollArea, public QoreQWidgetExtension
{
#define QOREQTYPE QScrollArea
#define MYQOREQTYPE myQScrollArea
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQScrollArea(QoreObject *obj, QWidget* parent = 0) : QScrollArea(parent), QoreQWidgetExtension(obj, this)
      {
         
      }
      DLLLOCAL void pub_setupViewport(QWidget *w)
      {
	 setupViewport(w);
      }
};

typedef QoreQAbstractScrollAreaBase<myQScrollArea, QoreAbstractQAbstractScrollArea> QoreQScrollAreaImpl;

class QoreQScrollArea : public QoreQScrollAreaImpl
{
   public:
      DLLLOCAL QoreQScrollArea(QoreObject *obj, QWidget* parent = 0) : QoreQScrollAreaImpl(new myQScrollArea(obj, parent))
      {
      }
};

#endif // _QORE_QT_QC_QSCROLLAREA_H
