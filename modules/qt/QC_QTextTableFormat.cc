/*
 QC_QTextTableFormat.cc
 
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

#include "QC_QTextTableFormat.h"

int CID_QTEXTTABLEFORMAT;
class QoreClass *QC_QTextTableFormat = 0;

//QTextTableFormat ()
static void QTEXTTABLEFORMAT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTTABLEFORMAT, new QoreQTextTableFormat());
}

static void QTEXTTABLEFORMAT_copy(class QoreObject *self, class QoreObject *old, class QoreQTextTableFormat *qttf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTTABLEFORMAT, new QoreQTextTableFormat(*qttf));
}

//Qt::Alignment alignment () const
static QoreNode *QTEXTTABLEFORMAT_alignment(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qttf->alignment());
}

//qreal cellPadding () const
static QoreNode *QTEXTTABLEFORMAT_cellPadding(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qttf->cellPadding());
}

//qreal cellSpacing () const
static QoreNode *QTEXTTABLEFORMAT_cellSpacing(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qttf->cellSpacing());
}

//void clearColumnWidthConstraints ()
static QoreNode *QTEXTTABLEFORMAT_clearColumnWidthConstraints(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
{
   qttf->clearColumnWidthConstraints();
   return 0;
}

////QVector<QTextLength> columnWidthConstraints () const
//static QoreNode *QTEXTTABLEFORMAT_columnWidthConstraints(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qttf->columnWidthConstraints());
//}

//int columns () const
static QoreNode *QTEXTTABLEFORMAT_columns(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qttf->columns());
}

//int headerRowCount () const
static QoreNode *QTEXTTABLEFORMAT_headerRowCount(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qttf->headerRowCount());
}

//bool isValid () const
static QoreNode *QTEXTTABLEFORMAT_isValid(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qttf->isValid());
}

//void setAlignment ( Qt::Alignment alignment )
static QoreNode *QTEXTTABLEFORMAT_setAlignment(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   qttf->setAlignment(alignment);
   return 0;
}

//void setCellPadding ( qreal padding )
static QoreNode *QTEXTTABLEFORMAT_setCellPadding(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal padding = p ? p->getAsFloat() : 0.0;
   qttf->setCellPadding(padding);
   return 0;
}

//void setCellSpacing ( qreal spacing )
static QoreNode *QTEXTTABLEFORMAT_setCellSpacing(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal spacing = p ? p->getAsFloat() : 0.0;
   qttf->setCellSpacing(spacing);
   return 0;
}

////void setColumnWidthConstraints ( const QVector<QTextLength> & constraints )
//static QoreNode *QTEXTTABLEFORMAT_setColumnWidthConstraints(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QVector<QTextLength> constraints = p;
//   qttf->setColumnWidthConstraints(constraints);
//   return 0;
//}

//void setHeaderRowCount ( int count )
static QoreNode *QTEXTTABLEFORMAT_setHeaderRowCount(QoreObject *self, QoreQTextTableFormat *qttf, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int count = p ? p->getAsInt() : 0;
   qttf->setHeaderRowCount(count);
   return 0;
}

QoreClass *initQTextTableFormatClass(QoreClass *qtextframeformat)
{
   QC_QTextTableFormat = new QoreClass("QTextTableFormat", QDOM_GUI);
   CID_QTEXTTABLEFORMAT = QC_QTextTableFormat->getID();

   QC_QTextTableFormat->addBuiltinVirtualBaseClass(qtextframeformat);

   QC_QTextTableFormat->setConstructor(QTEXTTABLEFORMAT_constructor);
   QC_QTextTableFormat->setCopy((q_copy_t)QTEXTTABLEFORMAT_copy);

   QC_QTextTableFormat->addMethod("alignment",                   (q_method_t)QTEXTTABLEFORMAT_alignment);
   QC_QTextTableFormat->addMethod("cellPadding",                 (q_method_t)QTEXTTABLEFORMAT_cellPadding);
   QC_QTextTableFormat->addMethod("cellSpacing",                 (q_method_t)QTEXTTABLEFORMAT_cellSpacing);
   QC_QTextTableFormat->addMethod("clearColumnWidthConstraints", (q_method_t)QTEXTTABLEFORMAT_clearColumnWidthConstraints);
   //QC_QTextTableFormat->addMethod("columnWidthConstraints",      (q_method_t)QTEXTTABLEFORMAT_columnWidthConstraints);
   QC_QTextTableFormat->addMethod("columns",                     (q_method_t)QTEXTTABLEFORMAT_columns);
   QC_QTextTableFormat->addMethod("headerRowCount",              (q_method_t)QTEXTTABLEFORMAT_headerRowCount);
   QC_QTextTableFormat->addMethod("isValid",                     (q_method_t)QTEXTTABLEFORMAT_isValid);
   QC_QTextTableFormat->addMethod("setAlignment",                (q_method_t)QTEXTTABLEFORMAT_setAlignment);
   QC_QTextTableFormat->addMethod("setCellPadding",              (q_method_t)QTEXTTABLEFORMAT_setCellPadding);
   QC_QTextTableFormat->addMethod("setCellSpacing",              (q_method_t)QTEXTTABLEFORMAT_setCellSpacing);
   //QC_QTextTableFormat->addMethod("setColumnWidthConstraints",   (q_method_t)QTEXTTABLEFORMAT_setColumnWidthConstraints);
   QC_QTextTableFormat->addMethod("setHeaderRowCount",           (q_method_t)QTEXTTABLEFORMAT_setHeaderRowCount);

   return QC_QTextTableFormat;
}
