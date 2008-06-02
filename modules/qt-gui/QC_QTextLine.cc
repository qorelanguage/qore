/*
 QC_QTextLine.cc
 
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

#include "qore-qt-gui.h"

#include "QC_QTextLine.h"
#include "QC_QPainter.h"
#include "QC_QPointF.h"
#include "QC_QRectF.h"

qore_classid_t CID_QTEXTLINE;
QoreClass *QC_QTextLine = 0;

//QTextLine ()
static void QTEXTLINE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTEXTLINE, new QoreQTextLine());
   return;
}

static void QTEXTLINE_copy(QoreObject *self, QoreObject *old, QoreQTextLine *qtl, ExceptionSink *xsink)
{
   xsink->raiseException("QTEXTLINE-COPY-ERROR", "objects of this class cannot be copied");
}

//qreal ascent () const
static AbstractQoreNode *QTEXTLINE_ascent(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qtl->getQTextLine()->ascent());
}

//qreal cursorToX ( int * cursorPos, Edge edge = Leading ) const
//qreal cursorToX ( int cursorPos, Edge edge = Leading ) const
static AbstractQoreNode *QTEXTLINE_cursorToX(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_REFERENCE) {
      const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(p);
      AutoVLock vl(xsink);
      ReferenceHelper ref(r, vl, xsink);
      if (!ref)
	 return 0;

      const AbstractQoreNode *v = ref.getValue();

      int cursorPos = v ? v->getAsInt() : 0;
      int save = cursorPos;

      p = get_param(params, 1);
      QTextLine::Edge edge = !is_nothing(p) ? (QTextLine::Edge)p->getAsInt() : QTextLine::Leading;
      qreal rv = qtl->getQTextLine()->cursorToX(&cursorPos, edge);

      if (cursorPos != save && ref.assign(new QoreBigIntNode(cursorPos), xsink))
	 return 0;  // there was an exception

      return new QoreFloatNode(rv);
   }
   int cursorPos = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QTextLine::Edge edge = !is_nothing(p) ? (QTextLine::Edge)p->getAsInt() : QTextLine::Leading;
   return new QoreFloatNode(qtl->getQTextLine()->cursorToX(cursorPos, edge));
}

//qreal descent () const
static AbstractQoreNode *QTEXTLINE_descent(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qtl->getQTextLine()->descent());
}

//void draw ( QPainter * painter, const QPointF & position, const QTextLayout::FormatRange * selection = 0 ) const
static AbstractQoreNode *QTEXTLINE_draw(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPainter *painter = (p && p->getType() == NT_OBJECT) ? (QoreQPainter *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTLINE-DRAW-PARAM-ERROR", "expecting a QPainter object as first argument to QTextLine::draw()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 1);
   QoreQPointF *position = (p && p->getType() == NT_OBJECT) ? (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
   if (!position) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTLINE-DRAW-PARAM-ERROR", "expecting a QPointF object as second argument to QTextLine::draw()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
   qtl->getQTextLine()->draw(static_cast<QPainter *>(painter->getQPainter()), *(static_cast<QPointF *>(position)));
   return 0;
}

//qreal height () const
static AbstractQoreNode *QTEXTLINE_height(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qtl->getQTextLine()->height());
}

//bool isValid () const
static AbstractQoreNode *QTEXTLINE_isValid(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qtl->getQTextLine()->isValid());
}

//int lineNumber () const
static AbstractQoreNode *QTEXTLINE_lineNumber(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->getQTextLine()->lineNumber());
}

//QRectF naturalTextRect () const
static AbstractQoreNode *QTEXTLINE_naturalTextRect(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QRectF, new QoreQRectF(qtl->getQTextLine()->naturalTextRect()));
}

//qreal naturalTextWidth () const
static AbstractQoreNode *QTEXTLINE_naturalTextWidth(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qtl->getQTextLine()->naturalTextWidth());
}

//QPointF position () const
static AbstractQoreNode *QTEXTLINE_position(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qtl->getQTextLine()->position()));
}

//QRectF rect () const
static AbstractQoreNode *QTEXTLINE_rect(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QRectF, new QoreQRectF(qtl->getQTextLine()->rect()));
}

//void setLineWidth ( qreal width )
static AbstractQoreNode *QTEXTLINE_setLineWidth(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal width = p ? p->getAsFloat() : 0.0;
   qtl->getQTextLine()->setLineWidth(width);
   return 0;
}

//void setNumColumns ( int numColumns )
//void setNumColumns ( int numColumns, qreal alignmentWidth )
static AbstractQoreNode *QTEXTLINE_setNumColumns(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int numColumns = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   qreal alignmentWidth = p ? p->getAsFloat() : 0.0;
   qtl->getQTextLine()->setNumColumns(numColumns, alignmentWidth);
   return 0;
}

//void setPosition ( const QPointF & pos )
static AbstractQoreNode *QTEXTLINE_setPosition(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPointF *pos = (p && p->getType() == NT_OBJECT) ? (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTLINE-SETPOSITION-PARAM-ERROR", "expecting a QPointF object as first argument to QTextLine::setPosition()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> posHolder(static_cast<AbstractPrivateData *>(pos), xsink);
   qtl->getQTextLine()->setPosition(*(static_cast<QPointF *>(pos)));
   return 0;
}

//int textLength () const
static AbstractQoreNode *QTEXTLINE_textLength(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->getQTextLine()->textLength());
}

//int textStart () const
static AbstractQoreNode *QTEXTLINE_textStart(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->getQTextLine()->textStart());
}

//qreal width () const
static AbstractQoreNode *QTEXTLINE_width(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qtl->getQTextLine()->width());
}

//qreal x () const
static AbstractQoreNode *QTEXTLINE_x(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qtl->getQTextLine()->x());
}

//int xToCursor ( qreal x, CursorPosition cpos = CursorBetweenCharacters ) const
static AbstractQoreNode *QTEXTLINE_xToCursor(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   QTextLine::CursorPosition cpos = !is_nothing(p) ? (QTextLine::CursorPosition)p->getAsInt() : QTextLine::CursorBetweenCharacters;
   return new QoreBigIntNode(qtl->getQTextLine()->xToCursor(x, cpos));
}

//qreal y () const
static AbstractQoreNode *QTEXTLINE_y(QoreObject *self, QoreQTextLine *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qtl->getQTextLine()->y());
}

static QoreClass *initQTextLineClass()
{
   QC_QTextLine = new QoreClass("QTextLine", QDOM_GUI);
   CID_QTEXTLINE = QC_QTextLine->getID();

   QC_QTextLine->setConstructor(QTEXTLINE_constructor);
   QC_QTextLine->setCopy((q_copy_t)QTEXTLINE_copy);

   QC_QTextLine->addMethod("ascent",                      (q_method_t)QTEXTLINE_ascent);
   QC_QTextLine->addMethod("cursorToX",                   (q_method_t)QTEXTLINE_cursorToX);
   QC_QTextLine->addMethod("descent",                     (q_method_t)QTEXTLINE_descent);
   QC_QTextLine->addMethod("draw",                        (q_method_t)QTEXTLINE_draw);
   QC_QTextLine->addMethod("height",                      (q_method_t)QTEXTLINE_height);
   QC_QTextLine->addMethod("isValid",                     (q_method_t)QTEXTLINE_isValid);
   QC_QTextLine->addMethod("lineNumber",                  (q_method_t)QTEXTLINE_lineNumber);
   QC_QTextLine->addMethod("naturalTextRect",             (q_method_t)QTEXTLINE_naturalTextRect);
   QC_QTextLine->addMethod("naturalTextWidth",            (q_method_t)QTEXTLINE_naturalTextWidth);
   QC_QTextLine->addMethod("position",                    (q_method_t)QTEXTLINE_position);
   QC_QTextLine->addMethod("rect",                        (q_method_t)QTEXTLINE_rect);
   QC_QTextLine->addMethod("setLineWidth",                (q_method_t)QTEXTLINE_setLineWidth);
   QC_QTextLine->addMethod("setNumColumns",               (q_method_t)QTEXTLINE_setNumColumns);
   QC_QTextLine->addMethod("setPosition",                 (q_method_t)QTEXTLINE_setPosition);
   QC_QTextLine->addMethod("textLength",                  (q_method_t)QTEXTLINE_textLength);
   QC_QTextLine->addMethod("textStart",                   (q_method_t)QTEXTLINE_textStart);
   QC_QTextLine->addMethod("width",                       (q_method_t)QTEXTLINE_width);
   QC_QTextLine->addMethod("x",                           (q_method_t)QTEXTLINE_x);
   QC_QTextLine->addMethod("xToCursor",                   (q_method_t)QTEXTLINE_xToCursor);
   QC_QTextLine->addMethod("y",                           (q_method_t)QTEXTLINE_y);

   return QC_QTextLine;
}

QoreNamespace *initQTextLineNS()
{
   QoreNamespace *ns = new QoreNamespace("QTextLine");
   ns->addSystemClass(initQTextLineClass());

   // CursorPosition enum
   ns->addConstant("CursorBetweenCharacters",  new QoreBigIntNode(QTextLine::CursorBetweenCharacters));
   ns->addConstant("CursorOnCharacter",        new QoreBigIntNode(QTextLine::CursorOnCharacter));

   // Egde enum
   ns->addConstant("Leading",                  new QoreBigIntNode(QTextLine::Leading));
   ns->addConstant("Trailing",                 new QoreBigIntNode(QTextLine::Trailing));

   return ns;
}
