/*
 QC_QValidator.h
 
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

#ifndef _QORE_QT_QC_QVALIDATOR_H

#define _QORE_QT_QC_QVALIDATOR_H

#include <QValidator>
#include "QoreAbstractQValidator.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QVALIDATOR;
DLLLOCAL extern class QoreClass *QC_QValidator;

DLLLOCAL class QoreClass *initQValidatorClass(QoreClass *);

class myQValidator : public QValidator, public QoreQValidatorExtension
{
#define QOREQTYPE QValidator
#include "qore-qt-metacode.h"
#include "qore-qt-qvalidator-methods.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQValidator(Object *obj, QObject* parent) : QValidator(parent), QoreQValidatorExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQValidator : public QoreAbstractQValidator
{
   public:
      QPointer<myQValidator> qobj;

      DLLLOCAL QoreQValidator(Object *obj, QObject* parent) : qobj(new myQValidator(obj, parent))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QValidator *getQValidator() const 
      { 
	 return static_cast<QValidator *>(&(*qobj)); 
      }

      QORE_VIRTUAL_QVALIDATOR_METHODS
};

class QoreQtQValidator : public QoreAbstractQValidator
{
   public:
      Object *qore_obj;
      QPointer<QValidator> qobj;

      DLLLOCAL QoreQtQValidator(Object *obj, QValidator *qv) : qore_obj(obj), qobj(qv)
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QValidator *getQValidator() const 
      { 
	 return static_cast<QValidator *>(&(*qobj)); 
      }
#include "qore-qt-static-qvalidator-methods.h"
};

#endif // _QORE_QT_QC_QVALIDATOR_H
