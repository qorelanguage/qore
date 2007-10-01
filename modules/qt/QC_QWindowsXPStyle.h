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

DLLLOCAL extern int CID_QWINDOWSXPSTYLE;
DLLLOCAL extern class QoreClass *QC_QWindowsXPStyle;

DLLLOCAL class QoreClass *initQWindowsXPStyleClass(QoreClass *);

class myQWindowsXPStyle : public QWindowsXPStyle, public QoreQStyleExtension
{
      friend class QoreQWindowsXPStyle;

#define QOREQTYPE QWindowsXPStyle
#include "qore-qt-qstyle-methods.h"
#include "qore-qt-metacode.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQWindowsXPStyle(Object *obj) : QWindowsXPStyle(), QoreQStyleExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQWindowsXPStyle : public QoreAbstractQWindowsStyle
{
   public:
      QPointer<myQWindowsXPStyle> qobj;

      DLLLOCAL QoreQWindowsXPStyle(Object *obj) : qobj(new myQWindowsXPStyle(obj))
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

class QoreQtQWindowsXPStyle : public QoreAbstractQWindowsStyle
{
   public:
      Object *qore_obj;
      QPointer<QWindowsXPStyle> qobj;

      DLLLOCAL QoreQWindowsXPStyle(Object *obj, QWindowsXPStyle *qxps) : qore_obj(obj), qobj(qxps)
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

#include "qore-qt-static-qstyle-methods.h"
};

#endif // _QORE_QT_QC_QWINDOWSXPSTYLE_H
