/*
 QC_QLinearGradient.h
 
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

#ifndef _QORE_QT_QC_QLINEARGRADIENT_H

#define _QORE_QT_QC_QLINEARGRADIENT_H

#include "QC_QGradient.h"

#include <QLinearGradient>

DLLEXPORT extern int CID_QLINEARGRADIENT;
DLLEXPORT extern QoreClass *QC_QLinearGradient;
DLLEXPORT QoreClass *initQLinearGradientClass(QoreClass *);

class QoreQLinearGradient : public QoreAbstractQGradient, public QLinearGradient
{
   public:
      DLLLOCAL QoreQLinearGradient() : QLinearGradient()
      {
      }
      DLLLOCAL QoreQLinearGradient(const QPointF& start, const QPointF& finalStop) : QLinearGradient(start, finalStop)
      {
      }
      DLLLOCAL QoreQLinearGradient(qreal x1, qreal y1, qreal x2, qreal y2) : QLinearGradient(x1, y1, x2, y2)
      {
      }

      DLLLOCAL virtual QGradient *getQGradient()
      {
	 return this;
      }

};

#endif // _QORE_QT_QC_QLINEARGRADIENT_H
