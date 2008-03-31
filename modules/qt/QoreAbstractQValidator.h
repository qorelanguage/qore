/*
 QoreAbstractQValidator.h
 
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

#ifndef _QORE_QT_QOREABSTRACTQVALIDATOR_H

#define _QORE_QT_QOREABSTRACTQVALIDATOR_H

#include "QoreAbstractQObject.h"
#include "QoreQtEventDispatcher.h"

#include <qore/ReferenceArgumentHelper.h>

class QoreAbstractQValidator : public QoreAbstractQObject
{
   public:
      DLLLOCAL virtual class QValidator *getQValidator() const = 0;
      DLLLOCAL virtual void fixup(QString &input) const = 0;
      DLLLOCAL virtual QValidator::State validate(QString &intput, int &pos) const = 0;
};

template<typename T, typename V>
class QoreQValidatorBase : public QoreQObjectBase<T, V>
{
   public:
      DLLLOCAL QoreQValidatorBase(T *qo) : QoreQObjectBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QValidator *getQValidator() const
      {
         return &(*this->qobj);
      }
      DLLLOCAL virtual void fixup(QString & input) const 
      {
	 this->qobj->fixup_parent(input);
      }
      DLLLOCAL virtual QValidator::State validate(QString & input, int & pos) const 
      {
	 return this->qobj->validate_parent(input, pos);
      }
};

template<typename T, typename V>
class QoreQtQValidatorCommonBase : public QoreQtQObjectBase<T, V>
{
   public:
      DLLLOCAL QoreQtQValidatorCommonBase(QoreObject *obj, T *qo) : QoreQtQObjectBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QValidator *getQValidator() const
      {
         return this->qobj;
      }

      DLLLOCAL virtual void fixup(QString & input) const 
      { 
	 this->qobj->fixup(input); 
      }
};

template<typename T, typename V>
class QoreQtQValidatorBase : public QoreQtQValidatorCommonBase<T, V>
{
   public:
      DLLLOCAL QoreQtQValidatorBase(QoreObject *obj, T *qo) : QoreQtQValidatorCommonBase<T, V>(obj, qo)
      {
      }
      DLLLOCAL virtual QValidator::State validate(QString & input, int & pos) const 
      {
	 return this->qobj->validate(input, pos); 
      }
};

class QoreQValidatorExtension : public QoreQObjectExtension
{
   protected:
      // event methods
      const QoreMethod *m_fixup, *m_validate;

   public:
      DLLLOCAL QoreQValidatorExtension(QoreObject *obj, QObject *qo) : QoreQObjectExtension(obj, qo)
      {
	 const QoreClass *qc = obj->getClass();
         m_fixup        = findMethod(qc, "fixup");
         m_validate     = findMethod(qc, "validate");
      }
};

#endif  // _QORE_QT_QOREABSTRACTQVALIDATOR_H
