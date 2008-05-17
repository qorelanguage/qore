/*
 QC_QLayout.h
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols

 Abstract class for QT
 
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

#ifndef _QORE_QC_QLAYOUT_H

#define _QORE_QC_QLAYOUT_H

#include "QoreAbstractQLayout.h"
#include "QC_QLayoutItem.h"

#include <QLayout>

DLLLOCAL extern qore_classid_t CID_QLAYOUT;
DLLLOCAL extern QoreClass *QC_QLayout;

#include "QC_QLayoutItem.h"


DLLLOCAL QoreClass *initQLayoutClass(QoreClass *qobject, QoreClass *qlayoutitem);

class QoreQLayout;

class myQLayout : public QLayout, public QoreQLayoutExtension
{
#define QORE_IS_QLAYOUT
#define QOREQTYPE QLayout
#define MYQOREQTYPE myQLayout
#include "qore-qt-metacode.h"
#include "qore-qt-qlayout-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE
#undef QORE_IS_QLAYOUT

   public:
      DLLLOCAL myQLayout(QoreObject *obj, QWidget* parent) : QLayout(parent), QoreQLayoutExtension(obj, this)
      {
      }
      DLLLOCAL myQLayout(QoreObject *obj) : QLayout(), QoreQLayoutExtension(obj, this)
      {
      }
};

typedef QoreQLayoutBase<myQLayout, QoreAbstractQLayout> QoreQLayoutImpl;

class QoreQLayout : public QoreQLayoutImpl
{
   public:
      DLLLOCAL QoreQLayout(QoreObject *obj, QWidget* parent) : QoreQLayoutImpl(new myQLayout(obj, parent))
      {
      }
      DLLLOCAL QoreQLayout(QoreObject *obj) : QoreQLayoutImpl(new myQLayout(obj))
      {
      }
};

typedef QoreQtQLayoutBase<QLayout, QoreAbstractQLayout> QoreQtQLayoutImpl;

class QoreQtQLayout : public QoreQtQLayoutImpl
{
   public:
      DLLLOCAL QoreQtQLayout(QoreObject *obj, QLayout *ql) : QoreQtQLayoutImpl(obj, ql)
      {
      }
};

#endif
