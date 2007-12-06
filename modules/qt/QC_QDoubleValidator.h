/*
 QC_QDoubleValidator.h
 
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

#ifndef _QORE_QT_QC_QDOUBLEVALIDATOR_H

#define _QORE_QT_QC_QDOUBLEVALIDATOR_H

#include <QDoubleValidator>
#include "QoreAbstractQValidator.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QDOUBLEVALIDATOR;
DLLLOCAL extern class QoreClass *QC_QDoubleValidator;

DLLLOCAL class QoreClass *initQDoubleValidatorClass(QoreClass *);

class myQDoubleValidator : public QDoubleValidator, public QoreQValidatorExtension
{
  private:
      QoreMethod *m_setRange;

      void qdv_init(QoreClass *qc)
      {
	 m_setRange = findMethod(qc, "setRange");
      }

#define QOREQTYPE QDoubleValidator
#include "qore-qt-metacode.h"
#include "qore-qt-qvalidator-methods.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQDoubleValidator(QoreObject *obj, QObject* parent) : QDoubleValidator(parent), QoreQValidatorExtension(obj->getClass())
      {
         init(obj);
	 qdv_init(obj->getClass());
      }
      DLLLOCAL myQDoubleValidator(QoreObject *obj, double bottom, double top, int decimals, QObject* parent) : QDoubleValidator(bottom, top, decimals, parent), QoreQValidatorExtension(obj->getClass())
      {
         init(obj);
	 qdv_init(obj->getClass());
      }

      DLLLOCAL void parent_setRange(double minimum, double maximum, int decimals = 0)
      {
	 QDoubleValidator::setRange(minimum, maximum, decimals);
      }

      DLLLOCAL virtual void setRange(double minimum, double maximum, int decimals = 0)
      {
	 if (!m_setRange) {
	    QDoubleValidator::setRange(minimum, maximum, decimals);
	    return;
	 }
	 QoreList *args = new QoreList();
	 args->push(new QoreNode(minimum));
	 args->push(new QoreNode(maximum));
	 args->push(new QoreNode((int64)decimals));

	 ExceptionSink xsink;
	 discard(dispatch_event_intern(qore_obj, m_setRange, args, &xsink), &xsink);
      }
};

class QoreQDoubleValidator : public QoreAbstractQValidator
{
   public:
      QPointer<myQDoubleValidator> qobj;

      DLLLOCAL QoreQDoubleValidator(QoreObject *obj, QObject* parent) : qobj(new myQDoubleValidator(obj, parent))
      {
      }
      DLLLOCAL QoreQDoubleValidator(QoreObject *obj, double bottom, double top, int decimals, QObject* parent) : qobj(new myQDoubleValidator(obj, bottom, top, decimals, parent))
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
      DLLLOCAL void setRange(double minimum, double maximum, int decimals = 0)
      {
	 qobj->parent_setRange(minimum, maximum, decimals);
      }
      QORE_VIRTUAL_QVALIDATOR_METHODS
};

#endif // _QORE_QT_QC_QDOUBLEVALIDATOR_H
