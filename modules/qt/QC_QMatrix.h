/*
 QC_QMatrix.h
 
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

#ifndef _QORE_QT_QC_QMATRIX_H

#define _QORE_QT_QC_QMATRIX_H

#include <QMatrix>

DLLLOCAL extern qore_classid_t CID_QMATRIX;
DLLLOCAL extern QoreClass *QC_QMatrix;
DLLLOCAL QoreClass *initQMatrixClass();

class QoreQMatrix : public AbstractPrivateData, public QMatrix
{
   public:
      DLLLOCAL QoreQMatrix() : QMatrix()
      {
      }
      DLLLOCAL QoreQMatrix(qreal m11, qreal m12, qreal m21, qreal m22, qreal dx, qreal dy) : QMatrix(m11, m12, m21, m22, dx, dy)
      {
      }
      DLLLOCAL QoreQMatrix(const QMatrix& matrix) : QMatrix(matrix)
      {
      }
};

#endif // _QORE_QT_QC_QMATRIX_H
