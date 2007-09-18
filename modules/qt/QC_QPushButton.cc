/*
 QC_QPushButton.cc
 
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

#include <qore/Qore.h>

#include "QC_QPushButton.h"
#include "QC_QFont.h"
#include "QC_QWidget.h"
#include "QoreAbstractQWidget.h"
//#include "QC_QAbstractButton.h"

int CID_QPUSHBUTTON;

static void QPUSHBUTTON_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreQPushButton *pb = 0;
   int np = num_params(params);
   if (!np)
      pb = new QoreQPushButton(self);
   else if (np == 1)
   {
      QoreNode *p = get_param(params, 0);
      if (p && p->type == NT_OBJECT)
      {
	 QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
	 if (!parent)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QPUSHBUTTON-CONSTRUCTOR-ERROR", "expecting an object derived from QWidget as parameter to QPushButton::constructor(), object class passed: '%s' is not derived from QWidget", p->val.object->getClass()->getName());
	    return;
	 }
	 ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
	 pb = new QoreQPushButton(self, parent->getQWidget());
      }
      else if (p && p->type == NT_STRING)
	 pb = new QoreQPushButton(self, p->val.String->getBuffer());
   }
   else if (np == 2 || np == 3)
   {
      QoreNode *p = get_param(params, 0);
      if (p && p->type == NT_STRING)
      {
	 const char *name = p->val.String->getBuffer();
	 QoreNode *p = test_param(params, NT_OBJECT, 1);
	 QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
	 if (!parent)
	 {
	    if (!xsink->isException())
	       xsink->raiseException("QPUSHBUTTON-CONSTRUCTOR-ERROR", "expecting an object derived from QWidget as parameter to QPushButton::constructor() in second argument when first argument is a string and second argument is passed");
	    return;
	 }
	 ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
	 pb = new QoreQPushButton(self, name, parent->getQWidget());
      }
      else // FIXME: implement constructor version with icon
      {
	 xsink->raiseException("QPUSHBUTTON-CONSTRUCTOR-ERROR", "unexpected value type %s for QPushButton::constructor() with %d arguments", p ? p->type->getName() : "NOTHING", np);
	 return;
      }
   }
   else
   {
      xsink->raiseException("QPUSHBUTTON-CONSTRUCTOR-ERROR", "expecting 0-3 arguments to QPushButton::constructor(), got %d", np);
      return;
   }

   self->setPrivate(CID_QPUSHBUTTON, pb);
}

static void QPUSHBUTTON_copy(class Object *self, class Object *old, class QoreQPushButton *pb, ExceptionSink *xsink)
{
   xsink->raiseException("QPUSHBUTTON-COPY-ERROR", "objects of this class cannot be copied");
}

//bool autoDefault () const
static QoreNode *QPUSHBUTTON_autoDefault(Object *self, QoreQPushButton *qpb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qpb->qobj->autoDefault());
}

//bool isDefault () const
static QoreNode *QPUSHBUTTON_isDefault(Object *self, QoreQPushButton *qpb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qpb->qobj->isDefault());
}

//bool isFlat () const
static QoreNode *QPUSHBUTTON_isFlat(Object *self, QoreQPushButton *qpb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qpb->qobj->isFlat());
}

//QMenu * menu () const
//static QoreNode *QPUSHBUTTON_menu(Object *self, QoreQPushButton *qpb, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return qpb->qobj->menu();
//}

//void setAutoDefault ( bool )
static QoreNode *QPUSHBUTTON_setAutoDefault(Object *self, QoreQPushButton *qpb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : 0;
   qpb->qobj->setAutoDefault(b);
   return 0;
}

//void setDefault ( bool )
static QoreNode *QPUSHBUTTON_setDefault(Object *self, QoreQPushButton *qpb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : 0;
   qpb->qobj->setDefault(b);
   return 0;
}

//void setFlat ( bool )
static QoreNode *QPUSHBUTTON_setFlat(Object *self, QoreQPushButton *qpb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : 0;
   qpb->qobj->setFlat(b);
   return 0;
}

//void setMenu ( QMenu * menu )
//static QoreNode *QPUSHBUTTON_setMenu(Object *self, QoreQPushButton *qpb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QMenu* menu = p;
//   qpb->qobj->setMenu(menu);
//   return 0;
//}

// slots
//void showMenu()
static QoreNode *QPUSHBUTTON_showMenu(Object *self, QoreQPushButton *qpb, QoreNode *params, ExceptionSink *xsink)
{
   qpb->qobj->showMenu();
   return 0;
}


class QoreClass *initQPushButtonClass(class QoreClass *qwidget)
{
   tracein("initQPushButtonClass()");
   
   class QoreClass *QC_QPushButton = new QoreClass("QPushButton", QDOM_GUI);
   CID_QPUSHBUTTON = QC_QPushButton->getID();

   QC_QPushButton->addBuiltinVirtualBaseClass(qwidget);

   QC_QPushButton->setConstructor(QPUSHBUTTON_constructor);
   QC_QPushButton->setCopy((q_copy_t)QPUSHBUTTON_copy);

   QC_QPushButton->addMethod("autoDefault",                 (q_method_t)QPUSHBUTTON_autoDefault);
   QC_QPushButton->addMethod("isDefault",                   (q_method_t)QPUSHBUTTON_isDefault);
   QC_QPushButton->addMethod("isFlat",                      (q_method_t)QPUSHBUTTON_isFlat);
   //QC_QPushButton->addMethod("menu",                        (q_method_t)QPUSHBUTTON_menu);
   QC_QPushButton->addMethod("setAutoDefault",              (q_method_t)QPUSHBUTTON_setAutoDefault);
   QC_QPushButton->addMethod("setDefault",                  (q_method_t)QPUSHBUTTON_setDefault);
   QC_QPushButton->addMethod("setFlat",                     (q_method_t)QPUSHBUTTON_setFlat);
   //QC_QPushButton->addMethod("setMenu",                     (q_method_t)QPUSHBUTTON_setMenu);
   QC_QPushButton->addMethod("showMenu",                    (q_method_t)QPUSHBUTTON_showMenu);

   traceout("initQPushButtonClass()");
   return QC_QPushButton;
}
