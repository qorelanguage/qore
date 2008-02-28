/*
 QC_QLayout.h
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

 Abstract class for QT
 
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

#ifndef _QORE_QC_QLAYOUT_H

#define _QORE_QC_QLAYOUT_H

#include "QoreAbstractQLayout.h"

#include <QLayout>

DLLLOCAL extern qore_classid_t CID_QLAYOUT;
DLLLOCAL extern QoreClass *QC_QLayout;

DLLLOCAL class QoreClass *initQLayoutClass(class QoreClass *qobject);

class QoreQtQLayout : public QoreAbstractQLayout
{
   public:
      QoreObject *qore_obj;
      QPointer<QLayout> qobj;

      DLLLOCAL QoreQtQLayout(QoreObject *obj, QLayout *ql) : qore_obj(obj), qobj(ql)
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QLayout *getQLayout() const
      {
         return static_cast<QLayout *>(&(*qobj));
      }
#include "qore-qt-static-qwidget-methods.h"
};

#endif
