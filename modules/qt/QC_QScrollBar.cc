/*
 QC_QScrollBar.cc
 
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

#include "QC_QScrollBar.h"

int CID_QSCROLLBAR;
class QoreClass *QC_QScrollBar = 0;

//QScrollBar ( QWidget * parent = 0 )
//QScrollBar ( Qt::Orientation orientation, QWidget * parent = 0 )
static void QSCROLLBAR_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QSCROLLBAR, new QoreQScrollBar(self));
      return;
   }
   if (p && p->type == NT_OBJECT) {
      QoreQWidget *parent = (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (!parent) {
         if (!xsink->isException())
            xsink->raiseException("QSCROLLBAR-QSCROLLBAR-PARAM-ERROR", "QScrollBar::QScrollBar() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return;
      }
      ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);
      self->setPrivate(CID_QSCROLLBAR, new QoreQScrollBar(self, static_cast<QWidget *>(parent->getQWidget())));
      return;
   }
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = test_param(params, NT_OBJECT, 1);
   QoreQWidget *parent = p ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent && p) {
      if (!xsink->isException())
         xsink->raiseException("QSCROLLBAR-QSCROLLBAR-PARAM-ERROR", "this version of QScrollBar::QScrollBar() expects either NOTHING or an object derived from QWidget as the second argument");
      return;
   }
   ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);
   self->setPrivate(CID_QSCROLLBAR, new QoreQScrollBar(self, orientation, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QSCROLLBAR_copy(class Object *self, class Object *old, class QoreQScrollBar *qsb, ExceptionSink *xsink)
{
   xsink->raiseException("QSCROLLBAR-COPY-ERROR", "objects of this class cannot be copied");
}

////void initStyleOption ( QStyleOptionSlider * option ) const
//static QoreNode *QSCROLLBAR_initStyleOption(Object *self, QoreQScrollBar *qsb, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QStyleOptionSlider* option = p;
//   qsb->qobj->initStyleOption(option);
//   return 0;
//}

QoreClass *initQScrollBarClass(QoreClass *qabstractslider)
{
   QC_QScrollBar = new QoreClass("QScrollBar", QDOM_GUI);
   CID_QSCROLLBAR = QC_QScrollBar->getID();

   QC_QScrollBar->addBuiltinVirtualBaseClass(qabstractslider);

   QC_QScrollBar->setConstructor(QSCROLLBAR_constructor);
   QC_QScrollBar->setCopy((q_copy_t)QSCROLLBAR_copy);

   // private methods
   //QC_QScrollBar->addMethod("initStyleOption",             (q_method_t)QSCROLLBAR_initStyleOption, true);

   return QC_QScrollBar;
}
