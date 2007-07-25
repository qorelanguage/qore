/*
 QC_QPushButton.h
 
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

#ifndef _QORE_QC_QPUSHBUTTON_H

#define _QORE_QC_QPUSHBUTTON_H

#include "QoreAbstractQWidget.h"

#include <QPushButton>

DLLEXPORT extern int CID_QPUSHBUTTON;

DLLLOCAL class QoreClass *initQPushButtonClass(class QoreClass *parent);

class QoreQPushButton : public QoreAbstractQWidget
{
   public:
      QPointer<QPushButton> qobj;
   
      DLLLOCAL QoreQPushButton(const char *str, QWidget *parent = 0) : qobj(new QPushButton(str, parent))
      {
      }
      DLLLOCAL QoreQPushButton(QWidget *parent = 0) : qobj(new QPushButton(parent))
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
};


#endif
