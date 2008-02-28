/*
 QC_QLineF.h
 
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

#ifndef _QORE_QT_QC_QLINEF_H

#define _QORE_QT_QC_QLINEF_H

#include <QLineF>

DLLLOCAL extern qore_classid_t CID_QLINEF;
DLLLOCAL extern class QoreClass *QC_QLineF;

DLLLOCAL class QoreClass *initQLineFClass();

class QoreQLineF : public AbstractPrivateData, public QLineF
{
   public:
      DLLLOCAL QoreQLineF() : QLineF()
      {
      }
      DLLLOCAL QoreQLineF(QPointF& p1, QPointF& p2) : QLineF(p1, p2)
      {
      }
      DLLLOCAL QoreQLineF(qreal x1, qreal y1, qreal x2, qreal y2) : QLineF(x1, y1, x2, y2)
      {
      }
      DLLLOCAL QoreQLineF(const QLine& line) : QLineF(line)
      {
      }
      DLLLOCAL QoreQLineF(const QLineF& line) : QLineF(line)
      {
      }
};

#endif // _QORE_QT_QC_QLINEF_H
