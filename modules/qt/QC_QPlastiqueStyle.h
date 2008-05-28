/*
 QC_QPlastiqueStyle.h
 
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

#ifndef _QORE_QT_QC_QPLASTIQUESTYLE_H

#define _QORE_QT_QC_QPLASTIQUESTYLE_H

#include <QPlastiqueStyle>
#include "QoreAbstractQWindowsStyle.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QPLASTIQUESTYLE;
DLLEXPORT extern class QoreClass *QC_QPlastiqueStyle;

DLLEXPORT class QoreClass *initQPlastiqueStyleClass(QoreClass *);

class myQPlastiqueStyle : public QPlastiqueStyle, public QoreQStyleExtension
{
      friend class QoreQPlastiqueStyle;

#define QOREQTYPE QPlastiqueStyle
#define MYQOREQTYPE myQPlastiqueStyle
#include "qore-qt-qstyle-methods.h"
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQPlastiqueStyle(QoreObject *obj) : QPlastiqueStyle(), QoreQStyleExtension(obj, this)
      {
         
      }
};

typedef QoreQWindowsStyleBase<myQPlastiqueStyle, QoreAbstractQWindowsStyle> QoreQPlastiqueStyleImpl;

class QoreQPlastiqueStyle : public QoreQPlastiqueStyleImpl
{
   public:
      DLLLOCAL QoreQPlastiqueStyle(QoreObject *obj) : QoreQPlastiqueStyleImpl(new myQPlastiqueStyle(obj))
      {
      }
};

typedef QoreQtQWindowsStyleBase<QPlastiqueStyle, QoreAbstractQWindowsStyle> QoreQtQPlastiqueStyleImpl;

class QoreQtQPlastiqueStyle : public QoreQtQPlastiqueStyleImpl
{
   public:
      DLLLOCAL QoreQtQPlastiqueStyle(QoreObject *obj, QPlastiqueStyle *qps) : QoreQtQPlastiqueStyleImpl(obj, qps)
      {
      }
};

#endif // _QORE_QT_QC_QPLASTIQUESTYLE_H
