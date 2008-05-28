/*
 QC_QFrame.h
 
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

#ifndef _QORE_QC_QFRAME_H

#define _QORE_QC_QFRAME_H

#include "QoreAbstractQFrame.h"

#include <QFrame>

#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QFRAME;

DLLEXPORT class QoreClass *initQFrameClass(class QoreClass *parent);

class myQFrame : public QFrame, public QoreQWidgetExtension
{
#define QOREQTYPE QFrame
#define MYQOREQTYPE myQFrame
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   DLLLOCAL myQFrame(QoreObject *obj, QWidget *parent = 0, Qt::WindowFlags window_flags = 0) : QFrame(parent, window_flags), QoreQWidgetExtension(obj, this)
      {
	 
      }
};

typedef QoreQFrameBase<myQFrame, QoreAbstractQFrame> QoreQFrameImpl;

class QoreQFrame : public QoreQFrameImpl
{
   public:
      DLLLOCAL QoreQFrame(QoreObject *obj, QWidget *parent = 0, Qt::WindowFlags window_flags = 0) : QoreQFrameImpl(new myQFrame(obj, parent, window_flags))
      {
      }
};

#endif
