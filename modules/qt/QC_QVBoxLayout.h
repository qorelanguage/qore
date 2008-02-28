/*
 QC_QVBoxLayout.h
 
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

#ifndef _QORE_QC_QVBOXLAYOUT_H

#define _QORE_QC_QVBOXLAYOUT_H

#include "QoreAbstractQBoxLayout.h"

#include <QVBoxLayout>

DLLEXPORT extern qore_classid_t CID_QVBOXLAYOUT;

DLLLOCAL class QoreClass *initQVBoxLayoutClass(class QoreClass *qboxlayout);

class myQVBoxLayout : public QVBoxLayout, public QoreQObjectExtension
{
#define QOREQTYPE QVBoxLayout
#include "qore-qt-metacode.h"
#undef QOREQTYPE
      DLLLOCAL myQVBoxLayout(QoreObject *obj) : QVBoxLayout(), QoreQObjectExtension(obj->getClass())
      {
	 init(obj);
      }
      DLLLOCAL myQVBoxLayout(QoreObject *obj, QWidget *parent) : QVBoxLayout(parent), QoreQObjectExtension(obj->getClass())
      {
	 init(obj);
      }
};

class QoreQVBoxLayout : public QoreAbstractQBoxLayout
{
   public:
      QPointer<myQVBoxLayout> qobj;

      DLLLOCAL QoreQVBoxLayout(QoreObject *obj) : qobj(new myQVBoxLayout(obj))
      {
      }

      DLLLOCAL QoreQVBoxLayout(QoreObject *obj, QWidget *parent) : qobj(new myQVBoxLayout(obj, parent))
      {
      }
      DLLLOCAL virtual QObject *getQObject() const
      {
	 return static_cast<QObject *>(&(*qobj));
      }

      DLLLOCAL virtual QLayout *getQLayout() const
      {
	 return static_cast<QLayout *>(&(*qobj));
      }

      DLLLOCAL virtual QBoxLayout *getQBoxLayout() const
      {
	 return static_cast<QBoxLayout *>(&(*qobj));
      }
      QORE_VIRTUAL_QOBJECT_METHODS
};

#endif
