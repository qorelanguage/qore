/*
 QC_QCDEStyle.h
 
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

#ifndef _QORE_QT_QC_QCDESTYLE_H

#define _QORE_QT_QC_QCDESTYLE_H

#include <QCDEStyle>
#include "QoreAbstractQMotifStyle.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QCDESTYLE;
DLLLOCAL extern class QoreClass *QC_QCDEStyle;

DLLLOCAL class QoreClass *initQCDEStyleClass(QoreClass *);

class myQCDEStyle : public QCDEStyle, public QoreQStyleExtension
{
      friend class QoreQCDEStyle;

#define QOREQTYPE QCDEStyle
#include "qore-qt-qstyle-methods.h"
#include "qore-qt-metacode.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQCDEStyle(Object *obj, bool useHighlightCols = false) : QCDEStyle(useHighlightCols), QoreQStyleExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQCDEStyle : public QoreAbstractQMotifStyle
{
   public:
      QPointer<myQCDEStyle> qobj;

      DLLLOCAL QoreQCDEStyle(Object *obj, bool useHighlightCols = false) : qobj(new myQCDEStyle(obj, useHighlightCols))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QMotifStyle *getQMotifStyle() const
      {
         return static_cast<QMotifStyle *>(&(*qobj));
      }
      DLLLOCAL virtual class QCDEStyle *getQCDEStyle() const
      {
         return static_cast<QCDEStyle *>(&(*qobj));
      }
      QORE_VIRTUAL_QSTYLE_METHODS
};

class QoreQtQCDEStyle : public QoreAbstractQMotifStyle
{
   public:
      Object *qore_obj;
      QPointer<QCDEStyle> qobj;

      DLLLOCAL QoreQtQCDEStyle(Object *obj, QCDEStyle *qcdestyle) : qore_obj(obj), qobj(qcdestyle)
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QMotifStyle *getQMotifStyle() const
      {
         return static_cast<QMotifStyle *>(&(*qobj));
      }
      DLLLOCAL virtual class QCDEStyle *getQCDEStyle() const
      {
         return static_cast<QCDEStyle *>(&(*qobj));
      }
      DLLLOCAL virtual class QStyle *getQStyle() const
      {
         return static_cast<QStyle *>(&(*qobj));
      }
#include "qore-qt-static-qstyle-methods.h"
};

#endif // _QORE_QT_QC_QCDESTYLE_H
