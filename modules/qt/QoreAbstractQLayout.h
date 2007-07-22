/*
 QoreAbstractQLayout.h
 
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

#ifndef _QORE_QOREABSTRACTQLAYOUT_H

#define _QORE_QOREABSTRACTQLAYOUT_H

#include "QoreAbstractQObject.h"
#include "QoreAbstractQWidget.h"

// abstract class ID
DLLLOCAL extern int CID_QLAYOUT;
extern int CID_QWIDGET;

class QoreAbstractQLayout : public QoreAbstractQObject
{
   public:
      DLLLOCAL virtual QLayout *getQLayout() const = 0;
};

// template functions for inherited methods - QLayout is an abstract class
template<typename T>
QoreNode *QLAYOUT_addWidget(class Object *self, T *ql, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *widget = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateDataFromMetaClass(CID_QWIDGET, xsink) : 0;
   if (!widget)
   {
      if (!xsink->isException())
	 xsink->raiseException("QLAYOUT-ADDWIDGET-ERROR", "expecting an object derived from QWidget as the only argument to QLayout::addWidget()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(widget, xsink);
   ql->qobj->addWidget(widget->getQWidget());
   return 0;
}

#endif
