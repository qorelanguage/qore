/*
 QC_QValidator.h
 
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

#ifndef _QORE_QT_QC_QVALIDATOR_H

#define _QORE_QT_QC_QVALIDATOR_H

#include <QValidator>
#include "QoreAbstractQValidator.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QVALIDATOR;
DLLLOCAL extern class QoreClass *QC_QValidator;

DLLLOCAL class QoreClass *initQValidatorClass(QoreClass *);

class myQValidator : public QValidator, public QoreQValidatorExtension
{
#define QOREQTYPE QValidator
#define MYQOREQTYPE myQValidator
#include "qore-qt-metacode.h"
#include "qore-qt-qvalidator-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQValidator(QoreObject *obj, QObject* parent) : QValidator(parent), QoreQValidatorExtension(obj, this)
      {
         
      }
};

typedef QoreQValidatorBase<myQValidator, QoreAbstractQValidator> QoreQValidatorImpl;

class QoreQValidator : public QoreQValidatorImpl
{
   public:
      DLLLOCAL QoreQValidator(QoreObject *obj, QObject* parent) : QoreQValidatorImpl(new myQValidator(obj, parent))
      {
      }
};

typedef QoreQtQValidatorCommonBase<QValidator, QoreAbstractQValidator> QoreQtQValidatorImpl;

class QoreQtQValidator : public QoreQtQValidatorImpl
{
   public:
      DLLLOCAL QoreQtQValidator(QoreObject *obj, QValidator *qv) : QoreQtQValidatorImpl(obj, qv)
      {
      }

      DLLLOCAL virtual QValidator::State validate(QString & input, int & pos) const 
      {
	 return QValidator::Invalid;
      }
};

#endif // _QORE_QT_QC_QVALIDATOR_H
