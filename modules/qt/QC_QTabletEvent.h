/*
 QC_QTabletEvent.h
 
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

#ifndef _QORE_QT_QC_QTABLETEVENT_H

#define _QORE_QT_QC_QTABLETEVENT_H

#include <QTabletEvent>

DLLLOCAL extern int CID_QTABLETEVENT;
DLLLOCAL extern class QoreClass *QC_QTabletEvent;

DLLLOCAL class QoreClass *initQTabletEventClass(QoreClass *);

class QoreQTabletEvent : public AbstractPrivateData, public QTabletEvent
{
   public:
      DLLLOCAL QoreQTabletEvent(Type type, const QPoint& pos, const QPoint& globalPos, const QPointF& hiResGlobalPos, int device, int pointerType, qreal pressure, int xTilt, int yTilt, qreal tangentialPressure, qreal rotation, int z, Qt::KeyboardModifiers keyState, qint64 uniqueID) : QTabletEvent(type, pos, globalPos, hiResGlobalPos, device, pointerType, pressure, xTilt, yTilt, tangentialPressure, rotation, z, keyState, uniqueID)
      {
      }
      DLLLOCAL QoreQTabletEvent(const QTabletEvent &e) : QTabletEvent(e)
      {
      }
};

#endif // _QORE_QT_QC_QTABLETEVENT_H
