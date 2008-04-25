/*
 QC_QMotifStyle.h
 
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

#ifndef _QORE_QT_QC_QMOTIFSTYLE_H

#define _QORE_QT_QC_QMOTIFSTYLE_H

#include <QMotifStyle>
#include "QoreAbstractQMotifStyle.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QMOTIFSTYLE;
DLLLOCAL extern class QoreClass *QC_QMotifStyle;

DLLLOCAL class QoreClass *initQMotifStyleClass(QoreClass *);

class myQMotifStyle : public QMotifStyle, public QoreQStyleExtension
{
   friend class QoreQMotifStyle;
#define QOREQTYPE QMotifStyle
#define MYQOREQTYPE myQMotifStyle
#include "qore-qt-qstyle-methods.h"
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQMotifStyle(QoreObject *obj, bool useHighlightCols = false) : QMotifStyle(useHighlightCols), QoreQStyleExtension(obj, this)
      {
         
      }
};

typedef QoreQMotifStyleBase<myQMotifStyle, QoreAbstractQMotifStyle> QoreQMotifStyleImpl;

class QoreQMotifStyle : public QoreQMotifStyleImpl
{
   public:
      DLLLOCAL QoreQMotifStyle(QoreObject *obj, bool useHighlightCols = false) : QoreQMotifStyleImpl(new myQMotifStyle(obj, useHighlightCols))
      {
      }
};

typedef QoreQtQMotifStyleBase<QMotifStyle, QoreAbstractQMotifStyle> QoreQtQMotifStyleImpl;

class QoreQtQMotifStyle : public QoreQtQMotifStyleImpl
{
   public:
      DLLLOCAL QoreQtQMotifStyle(QoreObject *obj, QMotifStyle *qms) : QoreQtQMotifStyleImpl(obj, qms)
      {
      }
};

#endif // _QORE_QT_QC_QMOTIFSTYLE_H
