/*
 QC_QWindowsStyle.h
 
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

#ifndef _QORE_QT_QC_QWINDOWSSTYLE_H

#define _QORE_QT_QC_QWINDOWSSTYLE_H

#include <QWindowsStyle>
#include "QoreAbstractQWindowsStyle.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QWINDOWSSTYLE;
DLLLOCAL extern class QoreClass *QC_QWindowsStyle;

DLLLOCAL class QoreClass *initQWindowsStyleClass(QoreClass *);

class myQWindowsStyle : public QWindowsStyle, public QoreQStyleExtension
{
   friend class QoreQWindowsStyle;

#define QOREQTYPE QWindowsStyle
#define MYQOREQTYPE myQWindowsStyle
#include "qore-qt-qstyle-methods.h"
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQWindowsStyle(QoreObject *obj) : QWindowsStyle(), QoreQStyleExtension(obj, this)
      {
         
      }
};

typedef QoreQWindowsStyleBase<myQWindowsStyle, QoreAbstractQWindowsStyle> QoreQWindowsStyleImpl;

class QoreQWindowsStyle : public QoreQWindowsStyleImpl
{
   public:
      DLLLOCAL QoreQWindowsStyle(QoreObject *obj) : QoreQWindowsStyleImpl(new myQWindowsStyle(obj))
      {
      }
};

typedef QoreQtQWindowsStyleBase<QWindowsStyle, QoreAbstractQWindowsStyle> QoreQtQWindowsStyleImpl;

class QoreQtQWindowsStyle : public QoreQtQWindowsStyleImpl
{
   public:
      DLLLOCAL QoreQtQWindowsStyle(QoreObject *obj, QWindowsStyle *qws) : QoreQtQWindowsStyleImpl(obj, qws)
      {
      }
};

#endif // _QORE_QT_QC_QWINDOWSSTYLE_H
