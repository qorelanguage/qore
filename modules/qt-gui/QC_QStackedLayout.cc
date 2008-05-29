/*
 QC_QStackedLayout.cc
 
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

#include <qore/Qore.h>

#include "QC_QLayout.h"
#include "QC_QStackedLayout.h"

int CID_QSTACKEDLAYOUT;
class QoreClass *QC_QStackedLayout = 0;

//QStackedLayout ()
//QStackedLayout ( QWidget * parent )
//QStackedLayout ( QLayout * parentLayout )
static void QSTACKEDLAYOUT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QSTACKEDLAYOUT, new QoreQStackedLayout(self));
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreAbstractQLayout *parentLayout = (QoreAbstractQLayout *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QLAYOUT, xsink);
      if (!parentLayout) {
         QoreAbstractQWidget *parent = (QoreAbstractQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink);
         if (!parent) {
            if (!xsink->isException())
               xsink->raiseException("QSTACKEDLAYOUT-CONSTRUCTOR-PARAM-ERROR", "QStackedLayout::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
            return;
         }
         ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
         self->setPrivate(CID_QSTACKEDLAYOUT, new QoreQStackedLayout(self, static_cast<QWidget *>(parent->getQWidget())));
         return;
      }
      ReferenceHolder<AbstractPrivateData> parentLayoutHolder(static_cast<AbstractPrivateData *>(parentLayout), xsink);
      self->setPrivate(CID_QSTACKEDLAYOUT, new QoreQStackedLayout(self, static_cast<QLayout *>(parentLayout->getQLayout())));
      return;
   }
   xsink->raiseException("QSTACKEDLAYOUT-CONSTRUCTOR-PARAM-ERROR", "QStackedLayout::constructor() does not know how to handle arguments of type '%s'", p->getTypeName());
}

static void QSTACKEDLAYOUT_copy(QoreObject *self, QoreObject *old, QoreQStackedLayout *qsl, ExceptionSink *xsink)
{
   xsink->raiseException("QSTACKEDLAYOUT-COPY-ERROR", "objects of this class cannot be copied");
}

//int addWidget ( QWidget * widget )
static AbstractQoreNode *QSTACKEDLAYOUT_addWidget(QoreObject *self, QoreQStackedLayout *qsl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *widget = (p && p->getType() == NT_OBJECT) ? (QoreAbstractQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSTACKEDLAYOUT-ADDWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QStackedLayout::addWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   int rc = qsl->qobj->addWidget(static_cast<QWidget *>(widget->getQWidget()));
   widget->setExternallyOwned();
   return new QoreBigIntNode(rc);
}

//virtual int count () const
static AbstractQoreNode *QSTACKEDLAYOUT_count(QoreObject *self, QoreQStackedLayout *qsl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qsl->qobj->count());
}

//int currentIndex () const
static AbstractQoreNode *QSTACKEDLAYOUT_currentIndex(QoreObject *self, QoreQStackedLayout *qsl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qsl->qobj->currentIndex());
}

//QWidget * currentWidget () const
static AbstractQoreNode *QSTACKEDLAYOUT_currentWidget(QoreObject *self, QoreQStackedLayout *qsl, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_qwidget(qsl->qobj->currentWidget());
}

//int insertWidget ( int index, QWidget * widget )
static AbstractQoreNode *QSTACKEDLAYOUT_insertWidget(QoreObject *self, QoreQStackedLayout *qsl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreAbstractQWidget *widget = (p && p->getType() == NT_OBJECT) ? (QoreAbstractQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSTACKEDLAYOUT-INSERTWIDGET-PARAM-ERROR", "expecting a QWidget object as second argument to QStackedLayout::insertWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreBigIntNode(qsl->qobj->insertWidget(index, static_cast<QWidget *>(widget->getQWidget())));
}

//QWidget * widget ( int index ) const
static AbstractQoreNode *QSTACKEDLAYOUT_widget(QoreObject *self, QoreQStackedLayout *qsl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return return_qwidget(qsl->qobj->widget(index));
}

//void setCurrentIndex ( int index )
static AbstractQoreNode *QSTACKEDLAYOUT_setCurrentIndex(QoreObject *self, QoreQStackedLayout *qsl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qsl->qobj->setCurrentIndex(index);
   return 0;
}

//void setCurrentWidget ( QWidget * widget )
static AbstractQoreNode *QSTACKEDLAYOUT_setCurrentWidget(QoreObject *self, QoreQStackedLayout *qsl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *widget = (p && p->getType() == NT_OBJECT) ? (QoreAbstractQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSTACKEDLAYOUT-SETCURRENTWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QStackedLayout::setCurrentWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qsl->qobj->setCurrentWidget(static_cast<QWidget *>(widget->getQWidget()));
   return 0;
}

QoreClass *initQStackedLayoutClass(QoreClass *qobject)
{
   QC_QStackedLayout = new QoreClass("QStackedLayout", QDOM_GUI);
   CID_QSTACKEDLAYOUT = QC_QStackedLayout->getID();

   QC_QStackedLayout->addBuiltinVirtualBaseClass(qobject);

   QC_QStackedLayout->setConstructor(QSTACKEDLAYOUT_constructor);
   QC_QStackedLayout->setCopy((q_copy_t)QSTACKEDLAYOUT_copy);

   QC_QStackedLayout->addMethod("addWidget",                   (q_method_t)QSTACKEDLAYOUT_addWidget);
   QC_QStackedLayout->addMethod("count",                       (q_method_t)QSTACKEDLAYOUT_count);
   QC_QStackedLayout->addMethod("currentIndex",                (q_method_t)QSTACKEDLAYOUT_currentIndex);
   QC_QStackedLayout->addMethod("currentWidget",               (q_method_t)QSTACKEDLAYOUT_currentWidget);
   QC_QStackedLayout->addMethod("insertWidget",                (q_method_t)QSTACKEDLAYOUT_insertWidget);
   QC_QStackedLayout->addMethod("widget",                      (q_method_t)QSTACKEDLAYOUT_widget);
   QC_QStackedLayout->addMethod("setCurrentIndex",             (q_method_t)QSTACKEDLAYOUT_setCurrentIndex);
   QC_QStackedLayout->addMethod("setCurrentWidget",            (q_method_t)QSTACKEDLAYOUT_setCurrentWidget);

   return QC_QStackedLayout;
}
