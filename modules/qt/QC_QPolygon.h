/*
 QC_QPolygon.h
 
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

#ifndef _QORE_QT_QC_QPOLYGON_H

#define _QORE_QT_QC_QPOLYGON_H

#include <QPolygon>

DLLLOCAL extern qore_classid_t CID_QPOLYGON;
DLLLOCAL extern class QoreClass *QC_QPolygon;

DLLLOCAL class QoreClass *initQPolygonClass();

class QoreQPolygon : public AbstractPrivateData, public QPolygon
{
   public:
      DLLLOCAL QoreQPolygon() : QPolygon()
      {
      }
      DLLLOCAL QoreQPolygon(int size) : QPolygon(size)
      {
      }
      DLLLOCAL QoreQPolygon(const QPolygon& polygon) : QPolygon(polygon)
      {
      }
      DLLLOCAL QoreQPolygon(const QVector<QPoint>& points) : QPolygon(points)
      {
      }
      DLLLOCAL QoreQPolygon(const QRect& rectangle, bool closed = false) : QPolygon(rectangle, closed)
      {
      }
};

#endif // _QORE_QT_QC_QPOLYGON_H
