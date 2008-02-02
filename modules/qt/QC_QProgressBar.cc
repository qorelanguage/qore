/*
 QC_QProgressBar.cc
 
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

#include "QC_QProgressBar.h"
#include "QC_QWidget.h"

#include "qore-qt.h"

int CID_QPROGRESSBAR;
class QoreClass *QC_QProgressBar = 0;

//QProgressBar ( QWidget * parent = 0 )
static void QPROGRESSBAR_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QPROGRESSBAR, new QoreQProgressBar(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QPROGRESSBAR_copy(class QoreObject *self, class QoreObject *old, class QoreQProgressBar *qpb, ExceptionSink *xsink)
{
   xsink->raiseException("QPROGRESSBAR-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::Alignment alignment () const
static AbstractQoreNode *QPROGRESSBAR_alignment(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpb->qobj->alignment());
}

//QString format () const
static AbstractQoreNode *QPROGRESSBAR_format(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qpb->qobj->format().toUtf8().data(), QCS_UTF8);
}

//bool invertedAppearance ()
static AbstractQoreNode *QPROGRESSBAR_invertedAppearance(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qpb->qobj->invertedAppearance());
}

//bool isTextVisible () const
static AbstractQoreNode *QPROGRESSBAR_isTextVisible(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qpb->qobj->isTextVisible());
}

//int maximum () const
static AbstractQoreNode *QPROGRESSBAR_maximum(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpb->qobj->maximum());
}

//int minimum () const
static AbstractQoreNode *QPROGRESSBAR_minimum(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpb->qobj->minimum());
}

//Qt::Orientation orientation () const
static AbstractQoreNode *QPROGRESSBAR_orientation(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpb->qobj->orientation());
}

//void setAlignment ( Qt::Alignment alignment )
static AbstractQoreNode *QPROGRESSBAR_setAlignment(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   qpb->qobj->setAlignment(alignment);
   return 0;
}

//void setFormat ( const QString & format )
static AbstractQoreNode *QPROGRESSBAR_setFormat(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QString format;
   if (get_qstring(p, format, xsink))
      return 0;
   qpb->qobj->setFormat(format);
   return 0;
}

//void setInvertedAppearance ( bool invert )
static AbstractQoreNode *QPROGRESSBAR_setInvertedAppearance(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool invert = p ? p->getAsBool() : false;
   qpb->qobj->setInvertedAppearance(invert);
   return 0;
}

//void setTextDirection ( QProgressBar::Direction textDirection )
static AbstractQoreNode *QPROGRESSBAR_setTextDirection(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QProgressBar::Direction textDirection = (QProgressBar::Direction)(p ? p->getAsInt() : 0);
   qpb->qobj->setTextDirection(textDirection);
   return 0;
}

//void setTextVisible ( bool visible )
static AbstractQoreNode *QPROGRESSBAR_setTextVisible(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool visible = p ? p->getAsBool() : false;
   qpb->qobj->setTextVisible(visible);
   return 0;
}

//virtual QString text () const
static AbstractQoreNode *QPROGRESSBAR_text(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qpb->qobj->text().toUtf8().data(), QCS_UTF8);
}

//QProgressBar::Direction textDirection ()
static AbstractQoreNode *QPROGRESSBAR_textDirection(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpb->qobj->textDirection());
}

//int value () const
static AbstractQoreNode *QPROGRESSBAR_value(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qpb->qobj->value());
}

//void reset ()
static AbstractQoreNode *QPROGRESSBAR_reset(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   qpb->qobj->reset();
   return 0;
}

//void setMaximum ( int maximum )
static AbstractQoreNode *QPROGRESSBAR_setMaximum(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int maximum = p ? p->getAsInt() : 0;
   qpb->qobj->setMaximum(maximum);
   return 0;
}

//void setMinimum ( int minimum )
static AbstractQoreNode *QPROGRESSBAR_setMinimum(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int minimum = p ? p->getAsInt() : 0;
   qpb->qobj->setMinimum(minimum);
   return 0;
}

//void setOrientation ( Qt::Orientation )
static AbstractQoreNode *QPROGRESSBAR_setOrientation(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   qpb->qobj->setOrientation(orientation);
   return 0;
}

//void setRange ( int minimum, int maximum )
static AbstractQoreNode *QPROGRESSBAR_setRange(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int minimum = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int maximum = p ? p->getAsInt() : 0;
   qpb->qobj->setRange(minimum, maximum);
   return 0;
}

//void setValue ( int value )
static AbstractQoreNode *QPROGRESSBAR_setValue(QoreObject *self, QoreQProgressBar *qpb, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int value = p ? p->getAsInt() : 0;
   qpb->qobj->setValue(value);
   return 0;
}

QoreClass *initQProgressBarClass(QoreClass *qwidget)
{
   QC_QProgressBar = new QoreClass("QProgressBar", QDOM_GUI);
   CID_QPROGRESSBAR = QC_QProgressBar->getID();

   QC_QProgressBar->addBuiltinVirtualBaseClass(qwidget);

   QC_QProgressBar->setConstructor(QPROGRESSBAR_constructor);
   QC_QProgressBar->setCopy((q_copy_t)QPROGRESSBAR_copy);

   QC_QProgressBar->addMethod("alignment",                   (q_method_t)QPROGRESSBAR_alignment);
   QC_QProgressBar->addMethod("format",                      (q_method_t)QPROGRESSBAR_format);
   QC_QProgressBar->addMethod("invertedAppearance",          (q_method_t)QPROGRESSBAR_invertedAppearance);
   QC_QProgressBar->addMethod("isTextVisible",               (q_method_t)QPROGRESSBAR_isTextVisible);
   QC_QProgressBar->addMethod("maximum",                     (q_method_t)QPROGRESSBAR_maximum);
   QC_QProgressBar->addMethod("minimum",                     (q_method_t)QPROGRESSBAR_minimum);
   QC_QProgressBar->addMethod("orientation",                 (q_method_t)QPROGRESSBAR_orientation);
   QC_QProgressBar->addMethod("setAlignment",                (q_method_t)QPROGRESSBAR_setAlignment);
   QC_QProgressBar->addMethod("setFormat",                   (q_method_t)QPROGRESSBAR_setFormat);
   QC_QProgressBar->addMethod("setInvertedAppearance",       (q_method_t)QPROGRESSBAR_setInvertedAppearance);
   QC_QProgressBar->addMethod("setTextDirection",            (q_method_t)QPROGRESSBAR_setTextDirection);
   QC_QProgressBar->addMethod("setTextVisible",              (q_method_t)QPROGRESSBAR_setTextVisible);
   QC_QProgressBar->addMethod("text",                        (q_method_t)QPROGRESSBAR_text);
   QC_QProgressBar->addMethod("textDirection",               (q_method_t)QPROGRESSBAR_textDirection);
   QC_QProgressBar->addMethod("value",                       (q_method_t)QPROGRESSBAR_value);
   QC_QProgressBar->addMethod("reset",                       (q_method_t)QPROGRESSBAR_reset);
   QC_QProgressBar->addMethod("setMaximum",                  (q_method_t)QPROGRESSBAR_setMaximum);
   QC_QProgressBar->addMethod("setMinimum",                  (q_method_t)QPROGRESSBAR_setMinimum);
   QC_QProgressBar->addMethod("setOrientation",              (q_method_t)QPROGRESSBAR_setOrientation);
   QC_QProgressBar->addMethod("setRange",                    (q_method_t)QPROGRESSBAR_setRange);
   QC_QProgressBar->addMethod("setValue",                    (q_method_t)QPROGRESSBAR_setValue);

   return QC_QProgressBar;
}
