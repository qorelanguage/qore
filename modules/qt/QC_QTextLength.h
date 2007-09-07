/*
 QC_QTextLength.h
 
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

#ifndef _QORE_QT_QC_QTEXTLENGTH_H

#define _QORE_QT_QC_QTEXTLENGTH_H

#include <QTextLength>

DLLLOCAL extern int CID_QTEXTLENGTH;
DLLLOCAL extern class QoreClass *QC_QTextLength;

DLLLOCAL class QoreClass *initQTextLengthClass();

class QoreQTextLength : public AbstractPrivateData, public QTextLength
{
   public:
      DLLLOCAL QoreQTextLength() : QTextLength()
      {
      }
      DLLLOCAL QoreQTextLength(const QTextLength &len) : QTextLength(len)
      {
      }
      DLLLOCAL QoreQTextLength(Type type, qreal value) : QTextLength(type, value)
      {
      }
};

#endif // _QORE_QT_QC_QTEXTLENGTH_H
