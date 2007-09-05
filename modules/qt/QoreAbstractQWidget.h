/*
 QoreAbstractQWidget.h
 
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

#ifndef _QORE_QOREABSTRACTQWIDGET_H

#define _QORE_QOREABSTRACTQWIDGET_H

#include "QoreAbstractQObject.h"
#include "QoreAbstractQPaintDevice.h"

class QoreQWidgetExtension {
   protected:
      // event methods
      Method *e_changeEvent, *e_enterEvent, *e_event, *e_leaveEvent,
         *e_paintEvent, 
         *e_mouseMoveEvent, *e_mousePressEvent, 
         *e_mouseReleaseEvent, *e_mouseDoubleClickEvent,
         *e_keyPressEvent, *e_keyReleaseEvent,
         *e_resizeEvent,
         *e_moveEvent;

   public:
      DLLLOCAL QoreQWidgetExtension(QoreClass *qc)
      {
         e_paintEvent             = qc->findMethod("paintEvent");
         e_mouseMoveEvent         = qc->findMethod("mouseMoveEvent");
         e_mousePressEvent        = qc->findMethod("mousePressEvent");
         e_mouseReleaseEvent      = qc->findMethod("mouseReleaseEvent");
         e_mouseDoubleClickEvent  = qc->findMethod("mouseDoubleClickEvent");
         e_keyPressEvent          = qc->findMethod("keyPressEvent");
         e_keyReleaseEvent        = qc->findMethod("keyReleaseEvent");
         e_changeEvent            = qc->findMethod("changeEvent");
         e_enterEvent             = qc->findMethod("enterEvent");
         e_event                  = qc->findMethod("event");
         e_leaveEvent             = qc->findMethod("leaveEvent");
         e_resizeEvent            = qc->findMethod("resizeEvent");
         e_moveEvent              = qc->findMethod("moveEvent");
      }
};

class QoreAbstractQWidget : public QoreAbstractQObject, public QoreAbstractQPaintDevice
{
   public:
      DLLLOCAL virtual QWidget *getQWidget() const = 0;
};

#endif
