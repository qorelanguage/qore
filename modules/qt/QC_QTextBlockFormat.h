/*
 QC_QTextBlockFormat.h
 
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

#ifndef _QORE_QT_QC_QTEXTBLOCKFORMAT_H

#define _QORE_QT_QC_QTEXTBLOCKFORMAT_H

#include <QTextBlockFormat>

DLLLOCAL extern qore_classid_t CID_QTEXTBLOCKFORMAT;
DLLLOCAL extern class QoreClass *QC_QTextBlockFormat;

DLLLOCAL class QoreClass *initQTextBlockFormatClass(QoreClass *);

class QoreQTextBlockFormat : public AbstractPrivateData, public QTextBlockFormat
{
   public:
      DLLLOCAL QoreQTextBlockFormat() : QTextBlockFormat()
      {
      }
      DLLLOCAL QoreQTextBlockFormat(const QTextBlockFormat &tbf) : QTextBlockFormat(tbf)
      {
      }
};

#endif // _QORE_QT_QC_QTEXTBLOCKFORMAT_H
