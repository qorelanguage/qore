/*
 QC_QTextFrameFormat.cc
 
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

#include "QC_QTextFrameFormat.h"
#include "QC_QBrush.h"
#include "QC_QTextLength.h"

#include "qore-qt.h"

int CID_QTEXTFRAMEFORMAT;
class QoreClass *QC_QTextFrameFormat = 0;

//QTextFrameFormat ()
static void QTEXTFRAMEFORMAT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTFRAMEFORMAT, new QoreQTextFrameFormat());
   return;
}

static void QTEXTFRAMEFORMAT_copy(class QoreObject *self, class QoreObject *old, class QoreQTextFrameFormat *qtff, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTFRAMEFORMAT, new QoreQTextFrameFormat(*qtff));
}

//qreal border () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_border(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qtff->border());
}

//QBrush borderBrush () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_borderBrush(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qb = new QoreObject(QC_QBrush, getProgram());
   QoreQBrush *q_qb = new QoreQBrush(qtff->borderBrush());
   o_qb->setPrivate(CID_QBRUSH, q_qb);
   return o_qb;
}

//BorderStyle borderStyle () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_borderStyle(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtff->borderStyle());
}

//qreal bottomMargin () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_bottomMargin(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qtff->bottomMargin());
}

//QTextLength height () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_height(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qtl = new QoreObject(QC_QTextLength, getProgram());
   QoreQTextLength *q_qtl = new QoreQTextLength(qtff->height());
   o_qtl->setPrivate(CID_QTEXTLENGTH, q_qtl);
   return o_qtl;
}

//bool isValid () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_isValid(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtff->isValid());
}

//qreal leftMargin () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_leftMargin(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qtff->leftMargin());
}

//qreal margin () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_margin(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qtff->margin());
}

//qreal padding () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_padding(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qtff->padding());
}

//PageBreakFlags pageBreakPolicy () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_pageBreakPolicy(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtff->pageBreakPolicy());
}

//Position position () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_position(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtff->position());
}

//qreal rightMargin () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_rightMargin(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qtff->rightMargin());
}

//void setBorder ( qreal width )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setBorder(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal width = p ? p->getAsFloat() : 0.0;
   qtff->setBorder(width);
   return 0;
}

//void setBorderBrush ( const QBrush & brush )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setBorderBrush(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QBrush brush;
   if (get_qbrush(p, brush, xsink))
      return 0;
   qtff->setBorderBrush(brush);
   return 0;
}

//void setBorderStyle ( BorderStyle style )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setBorderStyle(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QTextFrameFormat::BorderStyle style = (QTextFrameFormat::BorderStyle)(p ? p->getAsInt() : 0);
   qtff->setBorderStyle(style);
   return 0;
}

//void setBottomMargin ( qreal margin )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setBottomMargin(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal margin = p ? p->getAsFloat() : 0.0;
   qtff->setBottomMargin(margin);
   return 0;
}

//void setHeight ( const QTextLength & height )
//void setHeight ( qreal height )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setHeight(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQTextLength *height = (QoreQTextLength *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QTEXTLENGTH, xsink);
      if (!height) {
         if (!xsink->isException())
            xsink->raiseException("QTEXTFRAMEFORMAT-SETHEIGHT-PARAM-ERROR", "QTextFrameFormat::setHeight() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return 0;
      }
      ReferenceHolder<QoreQTextLength> heightHolder(height, xsink);
      qtff->setHeight(*(static_cast<QTextLength *>(height)));
      return 0;
   }
   qreal height = p ? p->getAsFloat() : 0.0;
   qtff->setHeight(height);
   return 0;
}

//void setLeftMargin ( qreal margin )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setLeftMargin(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal margin = p ? p->getAsFloat() : 0.0;
   qtff->setLeftMargin(margin);
   return 0;
}

//void setMargin ( qreal margin )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setMargin(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal margin = p ? p->getAsFloat() : 0.0;
   qtff->setMargin(margin);
   return 0;
}

//void setPadding ( qreal width )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setPadding(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal width = p ? p->getAsFloat() : 0.0;
   qtff->setPadding(width);
   return 0;
}

//void setPageBreakPolicy ( PageBreakFlags policy )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setPageBreakPolicy(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QTextFrameFormat::PageBreakFlags policy = (QTextFrameFormat::PageBreakFlags)(p ? p->getAsInt() : 0);
   qtff->setPageBreakPolicy(policy);
   return 0;
}

//void setPosition ( Position policy )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setPosition(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QTextFrameFormat::Position policy = (QTextFrameFormat::Position)(p ? p->getAsInt() : 0);
   qtff->setPosition(policy);
   return 0;
}

//void setRightMargin ( qreal margin )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setRightMargin(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal margin = p ? p->getAsFloat() : 0.0;
   qtff->setRightMargin(margin);
   return 0;
}

//void setTopMargin ( qreal margin )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setTopMargin(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal margin = p ? p->getAsFloat() : 0.0;
   qtff->setTopMargin(margin);
   return 0;
}

//void setWidth ( const QTextLength & width )
//void setWidth ( qreal width )
static AbstractQoreNode *QTEXTFRAMEFORMAT_setWidth(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQTextLength *width = (QoreQTextLength *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QTEXTLENGTH, xsink);
      if (!width) {
         if (!xsink->isException())
            xsink->raiseException("QTEXTFRAMEFORMAT-SETWIDTH-PARAM-ERROR", "QTextFrameFormat::setWidth() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return 0;
      }
      ReferenceHolder<QoreQTextLength> widthHolder(width, xsink);
      qtff->setWidth(*(static_cast<QTextLength *>(width)));
      return 0;
   }
   qreal width = p ? p->getAsFloat() : 0.0;
   qtff->setWidth(width);
   return 0;
}

//qreal topMargin () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_topMargin(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode((double)qtff->topMargin());
}

//QTextLength width () const
static AbstractQoreNode *QTEXTFRAMEFORMAT_width(QoreObject *self, QoreQTextFrameFormat *qtff, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qtl = new QoreObject(QC_QTextLength, getProgram());
   QoreQTextLength *q_qtl = new QoreQTextLength(qtff->width());
   o_qtl->setPrivate(CID_QTEXTLENGTH, q_qtl);
   return o_qtl;
}

QoreClass *initQTextFrameFormatClass(QoreClass *qtextformat)
{
   QC_QTextFrameFormat = new QoreClass("QTextFrameFormat", QDOM_GUI);
   CID_QTEXTFRAMEFORMAT = QC_QTextFrameFormat->getID();

   QC_QTextFrameFormat->addBuiltinVirtualBaseClass(qtextformat);

   QC_QTextFrameFormat->setConstructor(QTEXTFRAMEFORMAT_constructor);
   QC_QTextFrameFormat->setCopy((q_copy_t)QTEXTFRAMEFORMAT_copy);

   QC_QTextFrameFormat->addMethod("border",                      (q_method_t)QTEXTFRAMEFORMAT_border);
   QC_QTextFrameFormat->addMethod("borderBrush",                 (q_method_t)QTEXTFRAMEFORMAT_borderBrush);
   QC_QTextFrameFormat->addMethod("borderStyle",                 (q_method_t)QTEXTFRAMEFORMAT_borderStyle);
   QC_QTextFrameFormat->addMethod("bottomMargin",                (q_method_t)QTEXTFRAMEFORMAT_bottomMargin);
   QC_QTextFrameFormat->addMethod("height",                      (q_method_t)QTEXTFRAMEFORMAT_height);
   QC_QTextFrameFormat->addMethod("isValid",                     (q_method_t)QTEXTFRAMEFORMAT_isValid);
   QC_QTextFrameFormat->addMethod("leftMargin",                  (q_method_t)QTEXTFRAMEFORMAT_leftMargin);
   QC_QTextFrameFormat->addMethod("margin",                      (q_method_t)QTEXTFRAMEFORMAT_margin);
   QC_QTextFrameFormat->addMethod("padding",                     (q_method_t)QTEXTFRAMEFORMAT_padding);
   QC_QTextFrameFormat->addMethod("pageBreakPolicy",             (q_method_t)QTEXTFRAMEFORMAT_pageBreakPolicy);
   QC_QTextFrameFormat->addMethod("position",                    (q_method_t)QTEXTFRAMEFORMAT_position);
   QC_QTextFrameFormat->addMethod("rightMargin",                 (q_method_t)QTEXTFRAMEFORMAT_rightMargin);
   QC_QTextFrameFormat->addMethod("setBorder",                   (q_method_t)QTEXTFRAMEFORMAT_setBorder);
   QC_QTextFrameFormat->addMethod("setBorderBrush",              (q_method_t)QTEXTFRAMEFORMAT_setBorderBrush);
   QC_QTextFrameFormat->addMethod("setBorderStyle",              (q_method_t)QTEXTFRAMEFORMAT_setBorderStyle);
   QC_QTextFrameFormat->addMethod("setBottomMargin",             (q_method_t)QTEXTFRAMEFORMAT_setBottomMargin);
   QC_QTextFrameFormat->addMethod("setHeight",                   (q_method_t)QTEXTFRAMEFORMAT_setHeight);
   QC_QTextFrameFormat->addMethod("setLeftMargin",               (q_method_t)QTEXTFRAMEFORMAT_setLeftMargin);
   QC_QTextFrameFormat->addMethod("setMargin",                   (q_method_t)QTEXTFRAMEFORMAT_setMargin);
   QC_QTextFrameFormat->addMethod("setPadding",                  (q_method_t)QTEXTFRAMEFORMAT_setPadding);
   QC_QTextFrameFormat->addMethod("setPageBreakPolicy",          (q_method_t)QTEXTFRAMEFORMAT_setPageBreakPolicy);
   QC_QTextFrameFormat->addMethod("setPosition",                 (q_method_t)QTEXTFRAMEFORMAT_setPosition);
   QC_QTextFrameFormat->addMethod("setRightMargin",              (q_method_t)QTEXTFRAMEFORMAT_setRightMargin);
   QC_QTextFrameFormat->addMethod("setTopMargin",                (q_method_t)QTEXTFRAMEFORMAT_setTopMargin);
   QC_QTextFrameFormat->addMethod("setWidth",                    (q_method_t)QTEXTFRAMEFORMAT_setWidth);
   QC_QTextFrameFormat->addMethod("topMargin",                   (q_method_t)QTEXTFRAMEFORMAT_topMargin);
   QC_QTextFrameFormat->addMethod("width",                       (q_method_t)QTEXTFRAMEFORMAT_width);

   return QC_QTextFrameFormat;
}
