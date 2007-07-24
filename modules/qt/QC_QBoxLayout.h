/*
 QC_QBoxLayout.h
 
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

#ifndef _QORE_QC_QBOXLAYOUT_H

#define _QORE_QC_QBOXLAYOUT_H

#include "QoreAbstractQBoxLayout.h"

#include <QBoxLayout>

DLLEXPORT extern int CID_QBOXLAYOUT;

DLLLOCAL class QoreClass *initQBoxLayoutClass();

class QoreQBoxLayout : public QoreAbstractQBoxLayout
{
   public:
      QPointer<QBoxLayout> qobj;

      DLLLOCAL QoreQBoxLayout(QBoxLayout::Direction dir) : qobj(new QBoxLayout(dir))
      {
      }

      DLLLOCAL QoreQBoxLayout(QBoxLayout::Direction dir, QWidget *parent) : qobj(new QBoxLayout(dir, parent))
      {
      }
      DLLLOCAL virtual void destructor(class ExceptionSink *xsink)
      {
	 //QObject::disconnect(qobj, SLOT(isDeleted()));
	 if (qobj && !qobj->parent())
	    //delete qobj;
	    //qobj->deleteLater();
	    ;
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

};

#endif
