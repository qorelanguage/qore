/*
 QC_QVBoxLayout.h
 
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

#ifndef _QORE_QC_QVBOXLAYOUT_H

#define _QORE_QC_QVBOXLAYOUT_H

#include "QoreAbstractQBoxLayout.h"

#include <QVBoxLayout>

DLLEXPORT extern int CID_QVBOXLAYOUT;

DLLLOCAL class QoreClass *initQVBoxLayoutClass();

class QoreQVBoxLayout : public QoreAbstractQBoxLayout
{
   public:
      QPointer<QVBoxLayout> qobj;

      DLLLOCAL QoreQVBoxLayout() : qobj(new QVBoxLayout)
      {
      }

      DLLLOCAL QoreQVBoxLayout(QWidget *parent) : qobj(new QVBoxLayout(parent))
      {
      }
      DLLLOCAL virtual void destructor(class ExceptionSink *xsink)
      {
	 //QObject::disconnect(qobj, SLOT(isDeleted()));
	 if (qobj && !qobj->parent())
	    //qobj->deleteLater();
	    //delete qobj;
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
