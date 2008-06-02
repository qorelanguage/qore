/*
 QC_QAbstractItemModel.h
 
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

#ifndef _QORE_QT_QC_QABSTRACTITEMMODEL_H

#define _QORE_QT_QC_QABSTRACTITEMMODEL_H

#include <QAbstractItemModel>
#include "QoreAbstractQAbstractItemModel.h"

DLLEXPORT extern qore_classid_t CID_QABSTRACTITEMMODEL;
DLLEXPORT extern QoreClass *QC_QAbstractItemModel;

DLLEXPORT QoreClass *initQAbstractItemModelClass(QoreClass *);

typedef QoreQtQAbstractItemModelBase<QAbstractItemModel, QoreAbstractQAbstractItemModel> QoreQtQAbstractItemModelImpl;

class QoreQtQAbstractItemModel : public QoreQtQAbstractItemModelImpl
{
   public:
      DLLLOCAL QoreQtQAbstractItemModel(QoreObject *obj, QAbstractItemModel *aim) : QoreQtQAbstractItemModelImpl(obj, aim)
      {
      }
};

#endif // _QORE_QT_QC_QABSTRACTITEMMODEL_H
