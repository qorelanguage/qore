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

DLLLOCAL extern int CID_QWINDOWSSTYLE;
DLLLOCAL extern class QoreClass *QC_QWindowsStyle;

DLLLOCAL class QoreClass *initQWindowsStyleClass(QoreClass *);

class myQWindowsStyle : public QWindowsStyle
{
   friend class QoreQWindowsStyle;

#define QOREQTYPE QWindowsStyle
#include "qore-qt-metacode.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQWindowsStyle(Object *obj) : QWindowsStyle()
      {
         init(obj);
      }
};

class QoreQWindowsStyle : public QoreAbstractQWindowsStyle
{
   public:
      QPointer<myQWindowsStyle> qobj;

      DLLLOCAL QoreQWindowsStyle(Object *obj) : qobj(new myQWindowsStyle(obj))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QWindowsStyle *getQWindowsStyle() const
      {
         return static_cast<QWindowsStyle *>(&(*qobj));
      }
      QORE_VIRTUAL_QSTYLE_METHODS
};

#endif // _QORE_QT_QC_QWINDOWSSTYLE_H
