/*
 QC_QMouseEvent.h
 
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

#ifndef _QORE_QC_QMOUSEEVENT_H

#define _QORE_QC_QMOUSEEVENT_H

#include <QMouseEvent>

DLLLOCAL extern qore_classid_t CID_QMOUSEEVENT;
DLLLOCAL extern class QoreClass *QC_QMouseEvent;

DLLLOCAL class QoreClass *initQMouseEventClass(class QoreClass *parent);

class QoreQMouseEvent : public AbstractPrivateData, public QMouseEvent
{
   public:
      DLLLOCAL QoreQMouseEvent(const QMouseEvent &qr) : QMouseEvent(qr)
      {
      }
      DLLLOCAL QoreQMouseEvent(Type type, const QPoint & position, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers) : QMouseEvent(type, position, button, buttons, modifiers)
      {
      }
      DLLLOCAL QoreQMouseEvent(Type type, const QPoint & pos, const QPoint & globalPos, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers) : QMouseEvent(type, pos, globalPos, button, buttons, modifiers)
      {
      }
};


#endif
