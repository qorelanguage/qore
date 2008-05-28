/*
 QC_QStyleOption.h
 
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

#ifndef _QORE_QT_QC_QSTYLEOPTION_H

#define _QORE_QT_QC_QSTYLEOPTION_H

#include <QStyleOption>

DLLEXPORT extern qore_classid_t CID_QSTYLEOPTION;
DLLEXPORT extern class QoreClass *QC_QStyleOption;

DLLEXPORT class QoreClass *initQStyleOptionClass();

class QoreQStyleOption : public AbstractPrivateData, public QStyleOption
{
   public:
      DLLLOCAL QoreQStyleOption(int version = QStyleOption::Version, int type = SO_Default) : QStyleOption(version, type)
      {
      }
      DLLLOCAL QoreQStyleOption(const QStyleOption &qso) : QStyleOption(qso)
      {
      }
};

int QStyleOption_Notification(QoreObject *obj, QStyleOption *qso, const char *mem, ExceptionSink *xsink);
AbstractQoreNode *QStyleOption_MemberGate(QStyleOption *qso, const char *mem);

#endif // _QORE_QT_QC_QSTYLEOPTION_H
