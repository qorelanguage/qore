/*
 QC_QDragMoveEvent.h
 
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

#ifndef _QORE_QT_QC_QDRAGMOVEEVENT_H

#define _QORE_QT_QC_QDRAGMOVEEVENT_H

#include <QDragMoveEvent>

DLLLOCAL extern qore_classid_t CID_QDRAGMOVEEVENT;
DLLLOCAL extern class QoreClass *QC_QDragMoveEvent;

DLLLOCAL class QoreClass *initQDragMoveEventClass(QoreClass *);

class QoreQDragMoveEvent : public AbstractPrivateData, public QDragMoveEvent
{
   public:
      DLLLOCAL QoreQDragMoveEvent(const QPoint& pos, Qt::DropActions actions, const QMimeData* data, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, Type type = DragMove) : QDragMoveEvent(pos, actions, data, buttons, modifiers, type)
      {
      }
      DLLLOCAL QoreQDragMoveEvent(const QDragMoveEvent &event) : QDragMoveEvent(event)
      {
      }
};

#endif // _QORE_QT_QC_QDRAGMOVEEVENT_H
