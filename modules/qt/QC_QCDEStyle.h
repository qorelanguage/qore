/*
 QC_QCDEStyle.h
 
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

#ifndef _QORE_QT_QC_QCDESTYLE_H

#define _QORE_QT_QC_QCDESTYLE_H

#include <QCDEStyle>
#include "QoreAbstractQMotifStyle.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QCDESTYLE;
DLLLOCAL extern class QoreClass *QC_QCDEStyle;

DLLLOCAL class QoreClass *initQCDEStyleClass(QoreClass *);

class myQCDEStyle : public QCDEStyle, public QoreQStyleExtension
{
      friend class QoreQCDEStyle;

#define QOREQTYPE QCDEStyle
#define MYQOREQTYPE myQCDEStyle
#include "qore-qt-qstyle-methods.h"
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQCDEStyle(QoreObject *obj, bool useHighlightCols = false) : QCDEStyle(useHighlightCols), QoreQStyleExtension(obj, this)
      {
         
      }
};

typedef QoreQMotifStyleBase<myQCDEStyle, QoreAbstractQMotifStyle> QoreQCDEStyleImpl;

class QoreQCDEStyle : public QoreQCDEStyleImpl
{
   public:
      DLLLOCAL QoreQCDEStyle(QoreObject *obj, bool useHighlightCols = false) : QoreQCDEStyleImpl(new myQCDEStyle(obj, useHighlightCols))
      {
      }
};

typedef QoreQtQMotifStyleBase<QCDEStyle, QoreAbstractQMotifStyle> QoreQtQCDEStyleImpl;

class QoreQtQCDEStyle : public QoreQtQCDEStyleImpl
{
   public:
      DLLLOCAL QoreQtQCDEStyle(QoreObject *obj, QCDEStyle *qcdestyle) : QoreQtQCDEStyleImpl(obj, qcdestyle)
      {
      }
};

#endif // _QORE_QT_QC_QCDESTYLE_H
