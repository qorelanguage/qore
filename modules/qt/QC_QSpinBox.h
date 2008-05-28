/*
 QC_QSpinBox.h
 
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

#ifndef _QORE_QT_QC_QSPINBOX_H

#define _QORE_QT_QC_QSPINBOX_H

#include <QSpinBox>
#include "QoreAbstractQAbstractSpinBox.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QSPINBOX;
DLLEXPORT extern class QoreClass *QC_QSpinBox;

DLLEXPORT class QoreClass *initQSpinBoxClass(QoreClass *);

class myQSpinBox : public QSpinBox, public QoreQWidgetExtension
{
#define QOREQTYPE QSpinBox
#define MYQOREQTYPE myQSpinBox
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   private:
      const QoreMethod *m_textFromValue, *m_valueFromText;

      void qspinbox_init(const QoreClass *qc) 
      {
	 m_textFromValue = findMethod(qc, "textFromValue");
	 m_valueFromText = findMethod(qc, "valueFromText");
      }

   protected:

      virtual QString textFromValue ( int value ) const 
      {
	 if (!m_textFromValue)
	    return QSpinBox::textFromValue(value);

	 QoreListNode *args = new QoreListNode();
	 args->push(new QoreBigIntNode(value));

	 return dispatch_event_qstring(qore_obj, m_textFromValue, args);
      }
      virtual int valueFromText ( const QString & text ) const
      {
	 if (!m_valueFromText)
	    return QSpinBox::valueFromText(text);

	 QoreListNode *args = new QoreListNode();
	 args->push(new QoreStringNode(text.toUtf8().data(), QCS_UTF8));

	 return dispatch_event_int(qore_obj, m_valueFromText, args);
      }

   public:
      DLLLOCAL myQSpinBox(QoreObject *obj, QWidget* parent = 0) : QSpinBox(parent), QoreQWidgetExtension(obj, this)
      {
         
	 qspinbox_init(obj->getClass());
      }

      QString parent_textFromValue ( int value ) const 
      {
	 return QSpinBox::textFromValue(value);
      }

      int parent_valueFromText ( const QString & text ) const
      {
	 return QSpinBox::valueFromText(text);
      }
};

typedef QoreQAbstractSpinBoxBase<myQSpinBox, QoreAbstractQAbstractSpinBox> QoreQSpinBoxImpl;

class QoreQSpinBox : public QoreQSpinBoxImpl
{
   public:
      DLLLOCAL QoreQSpinBox(QoreObject *obj, QWidget* parent = 0) : QoreQSpinBoxImpl(new myQSpinBox(obj, parent))
      {
      }
};

#endif // _QORE_QT_QC_QSPINBOX_H
