/*
 QC_QMacStyle.h
 
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

#ifndef _QORE_QT_QC_QMACSTYLE_H

#define _QORE_QT_QC_QMACSTYLE_H

#include <QMacStyle>
#include "QoreAbstractQWindowsStyle.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QMACSTYLE;
DLLLOCAL extern class QoreClass *QC_QMacStyle;

DLLLOCAL class QoreClass *initQMacStyleClass(QoreClass *);

class myQMacStyle : public QMacStyle, public QoreQStyleExtension
{
      friend class QoreQMacStyle;

#define QOREQTYPE QMacStyle
#define MYQOREQTYPE myQMacStyle
#include "qore-qt-qstyle-methods.h"
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQMacStyle(QoreObject *obj) : QMacStyle(), QoreQStyleExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQMacStyle : public QoreAbstractQWindowsStyle
{
   public:
      QPointer<myQMacStyle> qobj;

      DLLLOCAL QoreQMacStyle(QoreObject *obj) : qobj(new myQMacStyle(obj))
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

class QoreQtQMacStyle : public QoreAbstractQWindowsStyle
{
   public:
      QoreObject *qore_obj;
      QPointer<QMacStyle> qobj;

      DLLLOCAL QoreQtQMacStyle(QoreObject *obj, QMacStyle *qms) : qore_obj(obj), qobj(qms)
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
      DLLLOCAL virtual class QStyle *getQStyle() const
      {
         return static_cast<QStyle *>(&(*qobj));
      }

#include "qore-qt-static-qstyle-methods.h"
};

#endif // _QORE_QT_QC_QMACSTYLE_H
