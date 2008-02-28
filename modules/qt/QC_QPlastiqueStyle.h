/*
 QC_QPlastiqueStyle.h
 
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

#ifndef _QORE_QT_QC_QPLASTIQUESTYLE_H

#define _QORE_QT_QC_QPLASTIQUESTYLE_H

#include <QPlastiqueStyle>
#include "QoreAbstractQWindowsStyle.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QPLASTIQUESTYLE;
DLLLOCAL extern class QoreClass *QC_QPlastiqueStyle;

DLLLOCAL class QoreClass *initQPlastiqueStyleClass(QoreClass *);

class myQPlastiqueStyle : public QPlastiqueStyle, public QoreQStyleExtension
{
      friend class QoreQPlastiqueStyle;

#define QOREQTYPE QPlastiqueStyle
#include "qore-qt-qstyle-methods.h"
#include "qore-qt-metacode.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQPlastiqueStyle(QoreObject *obj) : QPlastiqueStyle(), QoreQStyleExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQPlastiqueStyle : public QoreAbstractQWindowsStyle
{
   public:
      QPointer<myQPlastiqueStyle> qobj;

      DLLLOCAL QoreQPlastiqueStyle(QoreObject *obj) : qobj(new myQPlastiqueStyle(obj))
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

class QoreQtQPlastiqueStyle : public QoreAbstractQWindowsStyle
{
   public:
      QoreObject *qore_obj;
      QPointer<QPlastiqueStyle> qobj;

      DLLLOCAL QoreQtQPlastiqueStyle(QoreObject *obj, QPlastiqueStyle *qps) : qore_obj(obj), qobj(qps)
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

#endif // _QORE_QT_QC_QPLASTIQUESTYLE_H
