/*
 QC_QPen.h
 
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

#ifndef _QORE_QT_QC_QPEN_H

#define _QORE_QT_QC_QPEN_H

#include <QPen>

DLLLOCAL extern qore_classid_t CID_QPEN;
DLLLOCAL extern class QoreClass *QC_QPen;

DLLLOCAL class QoreClass *initQPenClass();

class QoreQPen : public AbstractPrivateData, public QPen
{
   public:
      DLLLOCAL QoreQPen() : QPen()
      {
      }
      DLLLOCAL QoreQPen(const QPen &pen) : QPen(pen)
      {
      }
      DLLLOCAL QoreQPen(Qt::PenStyle style) : QPen(style)
      {
      }
      DLLLOCAL QoreQPen(QColor& color) : QPen(color)
      {
      }
      DLLLOCAL QoreQPen(QBrush& brush, qreal width, Qt::PenStyle style = Qt::SolidLine, Qt::PenCapStyle cap = Qt::SquareCap, Qt::PenJoinStyle join = Qt::BevelJoin) : QPen(brush, width, style, cap, join)
      {
      }
};

#endif // _QORE_QT_QC_QPEN_H
