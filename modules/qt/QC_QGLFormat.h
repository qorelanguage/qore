/*
 QC_QGLFormat.h
 
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

#ifndef _QORE_QT_QC_QGLFORMAT_H

#define _QORE_QT_QC_QGLFORMAT_H

#include <QGLFormat>

DLLLOCAL extern int CID_QGLFORMAT;
DLLLOCAL extern QoreClass *QC_QGLFormat;
DLLLOCAL QoreNamespace *initQGLFormatNS();
DLLLOCAL QoreNamespace *initQGLNS();

DLLLOCAL void  initQGLFormatStaticFunctions();

class QoreQGLFormat : public AbstractPrivateData, public QGLFormat
{
   public:
      DLLLOCAL QoreQGLFormat() : QGLFormat()
      {
      }
      DLLLOCAL QoreQGLFormat(QGL::FormatOptions options, int plane = 0) : QGLFormat(options, plane)
      {
      }
      DLLLOCAL QoreQGLFormat(const QGLFormat& other) : QGLFormat(other)
      {
      }
};

#endif // _QORE_QT_QC_QGLFORMAT_H
