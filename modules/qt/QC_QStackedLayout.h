/*
 QC_QStackedLayout.h
 
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

#ifndef _QORE_QT_QC_QSTACKEDLAYOUT_H

#define _QORE_QT_QC_QSTACKEDLAYOUT_H

#include <QStackedLayout>
#include "QoreAbstractQLayout.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QSTACKEDLAYOUT;
DLLLOCAL extern QoreClass *QC_QStackedLayout;
DLLLOCAL QoreClass *initQStackedLayoutClass(QoreClass *);

class myQStackedLayout : public QStackedLayout, public QoreQObjectExtension
{
#define QOREQTYPE QStackedLayout
#define MYQOREQTYPE myQStackedLayout
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQStackedLayout(QoreObject *obj) : QStackedLayout(), QoreQObjectExtension(obj, this)
      {
      }
      DLLLOCAL myQStackedLayout(QoreObject *obj, QWidget* parent) : QStackedLayout(parent), QoreQObjectExtension(obj, this)
      {
      }
      DLLLOCAL myQStackedLayout(QoreObject *obj, QLayout* parentLayout) : QStackedLayout(parentLayout), QoreQObjectExtension(obj, this)
      {
      }
};

typedef QoreQLayoutBase<myQStackedLayout, QoreAbstractQLayout> QoreQStackedLayoutImpl;

class QoreQStackedLayout : public QoreQStackedLayoutImpl
{
   public:
      DLLLOCAL QoreQStackedLayout(QoreObject *obj) : QoreQStackedLayoutImpl(new myQStackedLayout(obj))
      {
      }
      DLLLOCAL QoreQStackedLayout(QoreObject *obj, QWidget* parent) : QoreQStackedLayoutImpl(new myQStackedLayout(obj, parent))
      {
      }
      DLLLOCAL QoreQStackedLayout(QoreObject *obj, QLayout* parentLayout) : QoreQStackedLayoutImpl(new myQStackedLayout(obj, parentLayout))
      {
      }
};

typedef QoreQtQLayoutBase<QStackedLayout, QoreAbstractQLayout> QoreQtQStackedLayoutImpl;

class QoreQtQStackedLayout : public QoreQtQStackedLayoutImpl
{
   public:
      DLLLOCAL QoreQtQStackedLayout(QoreObject *obj, QStackedLayout *qstackedlayout) : QoreQtQStackedLayoutImpl(obj, qstackedlayout)
      {
      }
};

#endif // _QORE_QT_QC_QSTACKEDLAYOUT_H
