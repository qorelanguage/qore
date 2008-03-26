/*
 QC_QWindowsXPStyle.h
 
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

#ifndef _QORE_QT_QC_QWINDOWSXPSTYLE_H

#define _QORE_QT_QC_QWINDOWSXPSTYLE_H

#include <QWindowsXPStyle>
#include "QoreAbstractQWindowsStyle.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QWINDOWSXPSTYLE;
DLLLOCAL extern class QoreClass *QC_QWindowsXPStyle;

DLLLOCAL class QoreClass *initQWindowsXPStyleClass(QoreClass *);

class myQWindowsXPStyle : public QWindowsXPStyle, public QoreQStyleExtension
{
      friend class QoreQWindowsXPStyle;

#define QOREQTYPE QWindowsXPStyle
#define MYQOREQTYPE myQWindowsXPStyle
#include "qore-qt-qstyle-methods.h"
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQWindowsXPStyle(QoreObject *obj) : QWindowsXPStyle(), QoreQStyleExtension(obj, this)
      {
         
      }
};

typedef QoreQWindowsStyleBase<myQWindowsXPStyle, QoreAbstractQWindowsStyle> QoreQWindowsXPStyleImpl;

class QoreQWindowsXPStyle : public QoreQWindowsXPStyleImpl
{
   public:
      DLLLOCAL QoreQWindowsXPStyle(QoreObject *obj) : QoreQWindowsXPStyleImpl(new myQWindowsXPStyle(obj))
      {
      }
};

typedef QoreAtQWindowsStyleBase<QWindowsXPStyle, QoreAbstractQWindowsStyle> QoreQtQWindowsXPStyleImpl;

class QoreQtQWindowsXPStyle : public QoreQtQWindowsXPStyleImpl
{
   public:
      DLLLOCAL QoreQWindowsXPStyle(QoreObject *obj, QWindowsXPStyle *qxps) : QoreQtQWindowsXPStyleImpl(obj, qxps)
      {
      }
};

#endif // _QORE_QT_QC_QWINDOWSXPSTYLE_H
