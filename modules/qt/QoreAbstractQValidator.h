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

#include <qore/LVarInstantiatorHelper.h>

class QoreAbstractQValidator : public QoreAbstractQObject
{
   public:
      DLLLOCAL virtual class QValidator *getQValidator() const = 0;
      DLLLOCAL virtual void fixup(QString &input) const = 0;
      DLLLOCAL virtual QValidator::State validate(QString &intput, int &pos) const = 0;
};

class QoreQValidatorExtension : public QoreQObjectExtension
{
   protected:
      // event methods
      const QoreMethod *m_fixup, *m_validate;

   public:
      DLLLOCAL QoreQValidatorExtension(const QoreClass *qc) : QoreQObjectExtension(qc)
      {
         m_fixup        = findMethod(qc, "fixup");
         m_validate     = findMethod(qc, "validate");
      }
};

#define QORE_VIRTUAL_QVALIDATOR_METHODS QORE_VIRTUAL_QOBJECT_METHODS \
   DLLLOCAL virtual void fixup(QString & input) const { qobj->fixup_parent(input); } \
   DLLLOCAL virtual QValidator::State validate(QString & input, int & pos) const {return qobj->validate_parent(input, pos); }

#endif  // _QORE_QT_QOREABSTRACTQVALIDATOR_H
