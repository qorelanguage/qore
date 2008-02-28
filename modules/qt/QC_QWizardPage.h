/*
 QC_QWizardPage.h
 
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

#ifndef _QORE_QT_QC_QWIZARDPAGE_H

#define _QORE_QT_QC_QWIZARDPAGE_H

#include <QWizardPage>
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QWIZARDPAGE;
DLLLOCAL extern class QoreClass *QC_QWizardPage;

DLLLOCAL class QoreClass *initQWizardPageClass(QoreClass *);

class myQWizardPage : public QWizardPage, public QoreQWidgetExtension
{
      friend class QoreQWizardPage;

#define QOREQTYPE QWizardPage
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQWizardPage(QoreObject *obj, QWidget* parent = 0) : QWizardPage(parent), QoreQWidgetExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreAbstractQWizardPage : public QoreAbstractQWidget
{
   public:
      DLLLOCAL virtual QWizardPage *getQWizardPage() const = 0;
      DLLLOCAL virtual QVariant field ( const QString & name ) const = 0;
      DLLLOCAL virtual void registerField ( const QString & name, QWidget * widget, const char * property = 0, const char * changedSignal = 0 ) = 0;
      DLLLOCAL virtual void setField ( const QString & name, const QVariant & value ) = 0;
      DLLLOCAL virtual QWizard * wizard () const = 0;
};

class QoreQWizardPage : public QoreAbstractQWizardPage
{
   public:
      QPointer<myQWizardPage> qobj;

      DLLLOCAL QoreQWizardPage(QoreObject *obj, QWidget* parent = 0) : qobj(new myQWizardPage(obj, parent))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QWidget *getQWidget() const
      {
         return static_cast<QWidget *>(&(*qobj));
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
         return static_cast<QPaintDevice *>(&(*qobj));
      }
      DLLLOCAL virtual QWizardPage *getQWizardPage() const
      {
	 return static_cast<QWizardPage *>(&(*qobj));
      }

      DLLLOCAL virtual QVariant field ( const QString & name ) const
      {
	 return qobj->field(name);
      }
      DLLLOCAL virtual void registerField ( const QString & name, QWidget * widget, const char * property = 0, const char * changedSignal = 0 )
      {
	 qobj->registerField(name, widget, property, changedSignal);
      }
      DLLLOCAL virtual void setField ( const QString & name, const QVariant & value )
      {
	 qobj->setField(name, value);
      }
      DLLLOCAL virtual QWizard * wizard () const
      {
	 return qobj->wizard();
      }

      QORE_VIRTUAL_QWIDGET_METHODS
};

class QoreQtQWizardPage : public QoreAbstractQWizardPage
{
   public:
      QoreObject *qore_obj;
      QPointer<QWizardPage> qobj;

      DLLLOCAL QoreQtQWizardPage(QoreObject *obj, QWizardPage *qwp) : qore_obj(obj), qobj(qwp)
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QWidget *getQWidget() const
      {
         return static_cast<QWidget *>(&(*qobj));
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
         return static_cast<QPaintDevice *>(&(*qobj));
      }
      DLLLOCAL virtual QWizardPage *getQWizardPage() const
      {
	 return static_cast<QWizardPage *>(&(*qobj));
      }

      // the following methods can never be called because they are protected
      DLLLOCAL virtual QVariant field ( const QString & name ) const
      {
	 return QVariant();
      }
      DLLLOCAL virtual void registerField ( const QString & name, QWidget * widget, const char * property = 0, const char * changedSignal = 0 )
      {
      }
      DLLLOCAL virtual void setField ( const QString & name, const QVariant & value )
      {
      }
      DLLLOCAL virtual QWizard * wizard () const
      {
	 return 0;
      }


#include "qore-qt-static-qwidget-methods.h"
};

#endif // _QORE_QT_QC_QWIZARDPAGE_H
