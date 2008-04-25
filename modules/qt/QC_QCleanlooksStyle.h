/*
 QC_QCleanlooksStyle.h
 
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

#ifndef _QORE_QT_QC_QCLEANLOOKSSTYLE_H

#define _QORE_QT_QC_QCLEANLOOKSSTYLE_H

#include <QCleanlooksStyle>
#include "QoreAbstractQCleanlooksStyle.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QCLEANLOOKSSTYLE;
DLLLOCAL extern QoreClass *QC_QCleanlooksStyle;

DLLLOCAL class QoreClass *initQCleanlooksStyleClass(QoreClass *);

class myQCleanlooksStyle : public QCleanlooksStyle, public QoreQStyleExtension
{
      friend class QoreQCleanlooksStyle;

#define QOREQTYPE QCleanlooksStyle
#define MYQOREQTYPE myQCleanlooksStyle
#include "qore-qt-qstyle-methods.h"
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQCleanlooksStyle(QoreObject *obj) : QCleanlooksStyle(), QoreQStyleExtension(obj, this)
      {
         
      }
};

typedef QoreQCleanlooksStyleBase<myQCleanlooksStyle, QoreAbstractQCleanlooksStyle> QoreQCleanlooksStyleImpl;

class QoreQCleanlooksStyle : public QoreQCleanlooksStyleImpl
{
   public:
      DLLLOCAL QoreQCleanlooksStyle(QoreObject *obj) : QoreQCleanlooksStyleImpl(new myQCleanlooksStyle(obj))
      {
      }
};

typedef QoreQtQCleanlooksStyleBase<QCleanlooksStyle, QoreAbstractQCleanlooksStyle> QoreQtQCleanlooksStyleImpl;

class QoreQtQCleanlooksStyle : public QoreQtQCleanlooksStyleImpl
{
   public:
      DLLLOCAL QoreQtQCleanlooksStyle(QoreObject *obj, QCleanlooksStyle *qcleanlooksstyle) : QoreQtQCleanlooksStyleImpl(obj, qcleanlooksstyle)
      {
      }
};

#endif // _QORE_QT_QC_QCLEANLOOKSSTYLE_H
