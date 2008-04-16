/*
 QC_QTransform.h
 
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

#ifndef _QORE_QT_QC_QTRANSFORM_H

#define _QORE_QT_QC_QTRANSFORM_H

#include <QTransform>

DLLLOCAL extern qore_classid_t CID_QTRANSFORM;
DLLLOCAL extern QoreClass *QC_QTransform;
DLLLOCAL QoreNamespace *initQTransformNS();

class QoreQTransform : public AbstractPrivateData, public QTransform
{
   public:
      DLLLOCAL QoreQTransform() : QTransform()
      {
      }
      DLLLOCAL QoreQTransform(qreal h11, qreal h12, qreal h13, qreal h21, qreal h22, qreal h23, qreal h31, qreal h32, qreal h33 = 1.0) : QTransform(h11, h12, h13, h21, h22, h23, h31, h32, h33)
      {
      }
      DLLLOCAL QoreQTransform(qreal h11, qreal h12, qreal h21, qreal h22, qreal dx, qreal dy) : QTransform(h11, h12, h21, h22, dx, dy)
      {
      }
      DLLLOCAL QoreQTransform(const QMatrix& matrix) : QTransform(matrix)
      {
      }
      DLLLOCAL QoreQTransform(const QTransform &t) : QTransform(t)
      {
      }
};

#endif // _QORE_QT_QC_QTRANSFORM_H
