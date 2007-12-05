/*
 QC_QAbstractItemModel.h
 
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

#ifndef _QORE_QT_QC_QABSTRACTITEMMODEL_H

#define _QORE_QT_QC_QABSTRACTITEMMODEL_H

#include <QAbstractItemModel>
#include "QoreAbstractQAbstractItemModel.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QABSTRACTITEMMODEL;
DLLLOCAL extern class QoreClass *QC_QAbstractItemModel;

DLLLOCAL class QoreClass *initQAbstractItemModelClass(QoreClass *);

class QoreQtQAbstractItemModel : public QoreAbstractQAbstractItemModel
{
   public:
      QoreObject *qore_obj;
      QPointer<QAbstractItemModel> qobj;

      DLLLOCAL QoreQtQAbstractItemModel(QoreObject *obj, QAbstractItemModel *aim) : qore_obj(obj), qobj(aim)
      {
      }
      DLLLOCAL QObject *getQObject() const
      {
	 return qobj;
      }
      DLLLOCAL QAbstractItemModel *getQAbstractItemModel() const
      {
	 return qobj;
      }
#include "qore-qt-static-qabstractitemmodel-methods.h"
};

#endif // _QORE_QT_QC_QABSTRACTITEMMODEL_H
