/*
 QC_QPaintEvent.h
 
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

#ifndef _QORE_QC_QPAINTEVENT_H

#define _QORE_QC_QPAINTEVENT_H

#include <QPaintEvent>

DLLLOCAL extern qore_classid_t CID_QPAINTEVENT;
DLLLOCAL extern class QoreClass *QC_QPaintEvent;

DLLLOCAL class QoreClass *initQPaintEventClass(class QoreClass *parent);

class QoreQPaintEvent : public AbstractPrivateData, public QPaintEvent
{
   public:
      DLLLOCAL QoreQPaintEvent(const QPaintEvent &qr) : QPaintEvent(qr)
      {
      }
      DLLLOCAL QoreQPaintEvent(const QRect &qr) : QPaintEvent(qr)
      {
      }
      DLLLOCAL QoreQPaintEvent(const QRegion &qr) : QPaintEvent(qr)
      {
      }
};


#endif
