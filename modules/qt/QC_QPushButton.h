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

#include <qore/Qore.h>

#include <QPushButton>

DLLEXPORT extern int CID_QPUSHBUTTON;

DLLLOCAL class QoreClass *initQPushButtonClass();

class QoreQPushButton : public AbstractPrivateData, public QPushButton
{
public:
   DLLLOCAL QoreQPushButton(const char *str) : QPushButton(str)
   {
   }
   DLLLOCAL QoreQPushButton() : QPushButton()
   {
   }

   DLLLOCAL virtual void deref(class ExceptionSink *xsink) 
   {
      if (ROdereference())
      {
	 delete this;
      }
   }

   DLLLOCAL virtual void destructor(class ExceptionSink *xsink) 
   {
   }
};


#endif
