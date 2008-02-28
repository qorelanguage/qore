/*
 QC_QInputEvent.h
 
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

#ifndef _QORE_QC_QINPUTEVENT_H

#define _QORE_QC_QINPUTEVENT_H

#include <QInputEvent>

DLLLOCAL extern qore_classid_t CID_QINPUTEVENT;
DLLLOCAL extern class QoreClass *QC_QInputEvent;

DLLLOCAL class QoreClass *initQInputEventClass(class QoreClass *parent);

class QoreQInputEvent : public AbstractPrivateData, public QInputEvent
{
   public:
      DLLLOCAL QoreQInputEvent(const QInputEvent &qr) : QInputEvent(qr)
      {
      }

};

#endif
