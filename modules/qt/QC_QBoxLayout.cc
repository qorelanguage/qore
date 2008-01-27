/*
 QC_QBoxLayout.cc
 
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

#include "QC_QBoxLayout.h"
#include "QC_QLayout.h"

int CID_QBOXLAYOUT;

static void QBOXLAYOUT_constructor(class QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int direction = p ? p->getAsInt() : 0;

   QoreQBoxLayout *qw;
   p = test_param(params, NT_OBJECT, 0);
   QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;

   if (!parent)
      qw = new QoreQBoxLayout(self, (QBoxLayout::Direction)direction);
   else 
   {
      ReferenceHolder<QoreAbstractQWidget> holder(parent, xsink);
      qw = new QoreQBoxLayout(self, (QBoxLayout::Direction)direction, parent->getQWidget());
   }

   self->setPrivate(CID_QBOXLAYOUT, qw);
}

static void QBOXLAYOUT_copy(class QoreObject *self, class QoreObject *old, class QoreQBoxLayout *ql, ExceptionSink *xsink)
{
   xsink->raiseException("QBOXLAYOUT-COPY-ERROR", "objects of this class cannot be copied");
}

//void addLayout ( QLayout * layout, int stretch = 0 )
static QoreNode *QBOXLAYOUT_addLayout(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQLayout *layout = (p && p->type == NT_OBJECT) ? (QoreAbstractQLayout *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QLAYOUT, xsink) : 0;
   if (!p || !layout)
   {
      if (!xsink->isException())
         xsink->raiseException("QBOXLAYOUT-ADDLAYOUT-PARAM-ERROR", "expecting a QLayout object as first argument to QBoxLayout::addLayout()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQLayout> holder(layout, xsink);
   p = get_param(params, 1);
   int stretch = p ? p->getAsInt() : 0;
   qbl->getQBoxLayout()->addLayout(layout->getQLayout(), stretch);
   return 0;
}

//void addSpacing ( int size )
static QoreNode *QBOXLAYOUT_addSpacing(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   qbl->getQBoxLayout()->addSpacing(size);
   return 0;
}

//void addStretch ( int stretch = 0 )
static QoreNode *QBOXLAYOUT_addStretch(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int stretch = p ? p->getAsInt() : 0;
   qbl->getQBoxLayout()->addStretch(stretch);
   return 0;
}

//void addStrut ( int size )
static QoreNode *QBOXLAYOUT_addStrut(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   qbl->getQBoxLayout()->addStrut(size);
   return 0;
}

//void addWidget ( QWidget * widget, int stretch = 0, Qt::Alignment alignment = 0 )
static QoreNode *QBOXLAYOUT_addWidget(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!p || !widget)
   {
      if (!xsink->isException())
         xsink->raiseException("QBOXLAYOUT-ADDWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QBoxLayout::addWidget()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(widget, xsink);
   p = get_param(params, 1);
   int stretch = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   //printd(5, "QBoxLayout::addWidget(%08x, %d, %d)\n", widget->getQWidget(), stretch, (int)alignment);
   qbl->getQBoxLayout()->addWidget(widget->getQWidget(), stretch, alignment);
   return 0;
}

//Direction direction () const
static QoreNode *QBOXLAYOUT_direction(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qbl->getQBoxLayout()->direction());
}

//void insertLayout ( int index, QLayout * layout, int stretch = 0 )
static QoreNode *QBOXLAYOUT_insertLayout(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreAbstractQLayout *layout = (p && p->type == NT_OBJECT) ? (QoreAbstractQLayout *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QLAYOUT, xsink) : 0;
   if (!p || !layout)
   {
      if (!xsink->isException())
         xsink->raiseException("QBOXLAYOUT-INSERTLAYOUT-PARAM-ERROR", "expecting a QLayout object as second argument to QBoxLayout::insertLayout()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQLayout> holder(layout, xsink);
   p = get_param(params, 2);
   int stretch = p ? p->getAsInt() : 0;
   qbl->getQBoxLayout()->insertLayout(index, layout->getQLayout(), stretch);
   return 0;
}

//void insertSpacing ( int index, int size )
static QoreNode *QBOXLAYOUT_insertSpacing(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int size = p ? p->getAsInt() : 0;
   qbl->getQBoxLayout()->insertSpacing(index, size);
   return 0;
}

//void insertStretch ( int index, int stretch = 0 )
static QoreNode *QBOXLAYOUT_insertStretch(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int stretch = p ? p->getAsInt() : 0;
   qbl->getQBoxLayout()->insertStretch(index, stretch);
   return 0;
}

//void insertWidget ( int index, QWidget * widget, int stretch = 0, Qt::Alignment alignment = 0 )
static QoreNode *QBOXLAYOUT_insertWidget(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QoreAbstractQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!p || !widget)
   {
      if (!xsink->isException())
         xsink->raiseException("QBOXLAYOUT-INSERTWIDGET-PARAM-ERROR", "expecting a QWidget object as second argument to QBoxLayout::insertWidget()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(widget, xsink);
   p = get_param(params, 2);
   int stretch = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   qbl->getQBoxLayout()->insertWidget(index, widget->getQWidget(), stretch, alignment);
   return 0;
}

//virtual void invalidate ()
static QoreNode *QBOXLAYOUT_invalidate(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   qbl->getQBoxLayout()->invalidate();
   return 0;
}

//void setDirection ( Direction direction )
static QoreNode *QBOXLAYOUT_setDirection(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QBoxLayout::Direction direction = (QBoxLayout::Direction)(p ? p->getAsInt() : 0);
   qbl->getQBoxLayout()->setDirection(direction);
   return 0;
}

//void setSpacing ( int spacing )
static QoreNode *QBOXLAYOUT_setSpacing(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int spacing = p ? p->getAsInt() : 0;
   qbl->getQBoxLayout()->setSpacing(spacing);
   return 0;
}

//bool setStretchFactor ( QWidget * widget, int stretch )
//bool setStretchFactor ( QLayout * layout, int stretch )
static QoreNode *QBOXLAYOUT_setStretchFactor(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 1);
   int stretch = p ? p->getAsInt() : 0;

   p = get_param(params, 0);
   QoreAbstractQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (p && !widget) {
      QoreAbstractQLayout *layout = (p && p->type == NT_OBJECT) ? (QoreAbstractQLayout *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QLAYOUT, xsink) : 0;
      if (layout) {
	 ReferenceHolder<QoreAbstractQLayout> holder(layout, xsink);
	 return new QoreNode(qbl->getQBoxLayout()->setStretchFactor(layout->getQLayout(), stretch));
      }
   }

   if (!widget)
   {
      if (!xsink->isException())
         xsink->raiseException("QBOXLAYOUT-SETSTRETCHFACTOR-PARAM-ERROR", "expecting a QWidget object as first argument to QBoxLayout::setStretchFactor()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(widget, xsink);
   return new QoreNode(qbl->getQBoxLayout()->setStretchFactor(widget->getQWidget(), stretch));
}

//int spacing () const
static QoreNode *QBOXLAYOUT_spacing(QoreObject *self, QoreAbstractQBoxLayout *qbl, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qbl->getQBoxLayout()->spacing());
}


class QoreClass *initQBoxLayoutClass(class QoreClass *qlayout)
{
   tracein("initQBoxLayoutClass()");
   
   class QoreClass *QC_QBoxLayout = new QoreClass("QBoxLayout", QDOM_GUI);
   CID_QBOXLAYOUT = QC_QBoxLayout->getID();

   QC_QBoxLayout->addBuiltinVirtualBaseClass(qlayout);

   QC_QBoxLayout->setConstructor(QBOXLAYOUT_constructor);
   QC_QBoxLayout->setCopy((q_copy_t)QBOXLAYOUT_copy);

   QC_QBoxLayout->addMethod("addLayout",                    (q_method_t)QBOXLAYOUT_addLayout);
   QC_QBoxLayout->addMethod("addSpacing",                   (q_method_t)QBOXLAYOUT_addSpacing);
   QC_QBoxLayout->addMethod("addStretch",                   (q_method_t)QBOXLAYOUT_addStretch);
   QC_QBoxLayout->addMethod("addStrut",                     (q_method_t)QBOXLAYOUT_addStrut);
   QC_QBoxLayout->addMethod("addWidget",                    (q_method_t)QBOXLAYOUT_addWidget);
   QC_QBoxLayout->addMethod("direction",                    (q_method_t)QBOXLAYOUT_direction);
   QC_QBoxLayout->addMethod("insertLayout",                 (q_method_t)QBOXLAYOUT_insertLayout);
   QC_QBoxLayout->addMethod("insertSpacing",                (q_method_t)QBOXLAYOUT_insertSpacing);
   QC_QBoxLayout->addMethod("insertStretch",                (q_method_t)QBOXLAYOUT_insertStretch);
   QC_QBoxLayout->addMethod("insertWidget",                 (q_method_t)QBOXLAYOUT_insertWidget);
   QC_QBoxLayout->addMethod("invalidate",                   (q_method_t)QBOXLAYOUT_invalidate);
   QC_QBoxLayout->addMethod("setDirection",                 (q_method_t)QBOXLAYOUT_setDirection);
   QC_QBoxLayout->addMethod("setSpacing",                   (q_method_t)QBOXLAYOUT_setSpacing);
   QC_QBoxLayout->addMethod("setStretchFactor",             (q_method_t)QBOXLAYOUT_setStretchFactor);
   QC_QBoxLayout->addMethod("spacing",                      (q_method_t)QBOXLAYOUT_spacing);

   traceout("initQBoxLayoutClass()");
   return QC_QBoxLayout;
}
