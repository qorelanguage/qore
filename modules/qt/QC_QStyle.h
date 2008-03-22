/*
 QC_QStyle.h
 
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

#ifndef _QORE_QT_QC_QSTYLE_H

#define _QORE_QT_QC_QSTYLE_H

#include <QStyle>
#include "QoreAbstractQStyle.h"
#include "qore-qt-events.h"

#include "QC_QMotifStyle.h"
#include "QC_QCDEStyle.h"
#include "QC_QWindowsStyle.h"
#include "QC_QCleanlooksStyle.h"
#include "QC_QPlastiqueStyle.h"

#ifdef DARWIN
#include "QC_QMacStyle.h"
#endif

#ifdef WINDOWS
#include "QC_QWindowsXPStyle.h"
#endif

DLLLOCAL extern qore_classid_t CID_QSTYLE;
DLLLOCAL extern class QoreClass *QC_QStyle;

DLLLOCAL class QoreNamespace *initQStyleNS(QoreClass *);

class myQStyle : public QStyle, public QoreQStyleExtension 
{
      friend class QoreQStyle;

#define QORE_IS_QSTYLE
#define QOREQTYPE QStyle
#define MYQOREQTYPE myQStyle
#include "qore-qt-qstyle-methods.h"
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE
#undef QORE_IS_QSTYLE

   public:
      DLLLOCAL myQStyle(QoreObject *obj) : QStyle(), QoreQStyleExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQStyle : public QoreAbstractQStyle
{
   public:
      QPointer<myQStyle> qobj;

      DLLLOCAL QoreQStyle(QoreObject *obj) : qobj(new myQStyle(obj))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      QORE_VIRTUAL_QSTYLE_METHODS
};

// for non-qore-generated QStyle object
class QoreQtQStyle : public QoreAbstractQStyle
{
   public:
      QoreObject *qore_obj;
      QPointer<QStyle> qobj;

      DLLLOCAL QoreQtQStyle(QoreObject *obj, QStyle *qs) : qore_obj(obj), qobj(qs)
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
	 return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QStyle *getQStyle() const
      {
	 return static_cast<QStyle *>(&(*qobj));
      }
#include "qore-qt-static-qstyle-methods.h"
};

#endif // _QORE_QT_QC_QSTYLE_H
