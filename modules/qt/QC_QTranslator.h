/*
 QC_QTranslator.h
 
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

#ifndef _QORE_QT_QC_QTRANSLATOR_H

#define _QORE_QT_QC_QTRANSLATOR_H

#include <QTranslator>
#include "QoreAbstractQObject.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QTRANSLATOR;
DLLLOCAL extern class QoreClass *QC_QTranslator;

DLLLOCAL class QoreClass *initQTranslatorClass(QoreClass *);

class myQTranslator : public QTranslator, public QoreQObjectExtension
{
#define QOREQTYPE QTranslator
#define MYQOREQTYPE myQTranslator
#include "qore-qt-metacode.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQTranslator(QoreObject *obj, QObject* parent = 0) : QTranslator(parent), QoreQObjectExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQTranslator : public QoreAbstractQObject
{
   public:
      QPointer<myQTranslator> qobj;

      DLLLOCAL QoreQTranslator(QoreObject *obj, QObject* parent = 0) : qobj(new myQTranslator(obj, parent))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      QORE_VIRTUAL_QOBJECT_METHODS
};

#endif // _QORE_QT_QC_QTRANSLATOR_H
