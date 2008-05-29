/*
 QC_QIntValidator.h
 
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

#ifndef _QORE_QT_QC_QINTVALIDATOR_H

#define _QORE_QT_QC_QINTVALIDATOR_H

#include <QIntValidator>
#include "QoreAbstractQValidator.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QINTVALIDATOR;
DLLEXPORT extern class QoreClass *QC_QIntValidator;

DLLEXPORT class QoreClass *initQIntValidatorClass(QoreClass *);

class myQIntValidator : public QIntValidator, public QoreQValidatorExtension
{
  private:
      const QoreMethod *m_setRange;

      void qdv_init(const QoreClass *qc)
      {
         m_setRange = findMethod(qc, "setRange");
      }

#define QOREQTYPE QIntValidator
#define MYQOREQTYPE myQIntValidator
#include "qore-qt-metacode.h"
#include "qore-qt-qvalidator-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQIntValidator(QoreObject *obj, QObject* parent) : QIntValidator(parent), QoreQValidatorExtension(obj, this)
      {
         
         qdv_init(obj->getClass());
      }
      DLLLOCAL myQIntValidator(QoreObject *obj, int minimum, int maximum, QObject* parent) : QIntValidator(minimum, maximum, parent), QoreQValidatorExtension(obj, this)
      {
         
         qdv_init(obj->getClass());
      }
 
      DLLLOCAL void parent_setRange(int bottom, int top)
      {
         QIntValidator::setRange(bottom, top);
      }
      DLLLOCAL virtual void setRange(int bottom, int top)
      {
         if (!m_setRange) {
            QIntValidator::setRange(bottom, top);
            return;
         }
         QoreListNode *args = new QoreListNode();
         args->push(new QoreBigIntNode(bottom));
         args->push(new QoreBigIntNode(top));

         ExceptionSink xsink;
         discard(dispatch_event_intern(qore_obj, m_setRange, args, &xsink), &xsink);
      }
};

typedef QoreQValidatorBase<myQIntValidator, QoreAbstractQValidator> QoreQIntValidatorImpl;

class QoreQIntValidator : public QoreQIntValidatorImpl
{
   public:
      DLLLOCAL QoreQIntValidator(QoreObject *obj, QObject* parent) : QoreQIntValidatorImpl(new myQIntValidator(obj, parent))
      {
      }
      DLLLOCAL QoreQIntValidator(QoreObject *obj, int minimum, int maximum, QObject* parent) : QoreQIntValidatorImpl(new myQIntValidator(obj, minimum, maximum, parent))
      {
      }
      DLLLOCAL void setRange(int bottom, int top)
      {
         qobj->parent_setRange(bottom, top);
      }
};

#endif // _QORE_QT_QC_QINTVALIDATOR_H
