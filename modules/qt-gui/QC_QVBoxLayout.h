/*
 QC_QVBoxLayout.h
 
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

#ifndef _QORE_QC_QVBOXLAYOUT_H

#define _QORE_QC_QVBOXLAYOUT_H

#include "QoreAbstractQBoxLayout.h"
#include "QC_QLayout.h"

#include <QVBoxLayout>

DLLEXPORT extern qore_classid_t CID_QVBOXLAYOUT;

DLLEXPORT class QoreClass *initQVBoxLayoutClass(class QoreClass *qboxlayout);

class myQVBoxLayout : public QVBoxLayout, public QoreQLayoutExtension
{
#define QOREQTYPE QVBoxLayout
#define MYQOREQTYPE myQVBoxLayout
#include "qore-qt-metacode.h"
#include "qore-qt-qlayout-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE
      DLLLOCAL myQVBoxLayout(QoreObject *obj) : QVBoxLayout(), QoreQLayoutExtension(obj, this)
      {
      }
      DLLLOCAL myQVBoxLayout(QoreObject *obj, QWidget *parent) : QVBoxLayout(parent), QoreQLayoutExtension(obj, this)
      {
      }
};

typedef QoreQBoxLayoutBase<myQVBoxLayout, QoreAbstractQBoxLayout> QoreQVBoxLayoutImpl;

class QoreQVBoxLayout : public QoreQVBoxLayoutImpl
{
   public:
      DLLLOCAL QoreQVBoxLayout(QoreObject *obj) : QoreQVBoxLayoutImpl(new myQVBoxLayout(obj))
      {
      }

      DLLLOCAL QoreQVBoxLayout(QoreObject *obj, QWidget *parent) : QoreQVBoxLayoutImpl(new myQVBoxLayout(obj, parent))
      {
      }
};

#endif
