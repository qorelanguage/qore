/*
 QC_QTextLayout.cc
 
  Qore Programming Language

 Copyright (C) 2003 - 2008 David Nichols

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

#include "QC_QTextLayout.h"
#include "QC_QTextCharFormat.h"

qore_classid_t CID_QTEXTLAYOUT;
QoreClass *QC_QTextLayout = 0;

static int get_format_range(QTextLayout::FormatRange &fr, const QoreHashNode *h, ExceptionSink *xsink)
{
   const AbstractQoreNode *n = h->getKeyValue("format");
   if (n && n->getType() == NT_OBJECT) {
      const QoreObject *o = reinterpret_cast<const QoreObject *>(n);
      QoreQTextCharFormat *format = (QoreQTextCharFormat *)o->getReferencedPrivateData(CID_QTEXTCHARFORMAT, xsink);
      if (*xsink)
	 return -1;
      ReferenceHolder<AbstractPrivateData> holder(format, xsink);
      fr.format = *format;
   }
   
   n = h->getKeyValue("length");
   if (!is_nothing(n))
      fr.length = n->getAsInt();

   n = h->getKeyValue("start");
   if (!is_nothing(n))
      fr.start = n->getAsInt();

   return 0;
}

//QTextLayout ()
//QTextLayout ( const QString & text )
//QTextLayout ( const QString & text, const QFont & font, QPaintDevice * paintdevice = 0 )
static void QTEXTLAYOUT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QTEXTLAYOUT, new QoreQTextLayout());
      return;
   }
   QString text;
   if (get_qstring(p, text, xsink))
      return;

   if (num_params(params) == 1) {
      self->setPrivate(CID_QTEXTLAYOUT, new QoreQTextLayout(text));
      return;
   }

   p = get_param(params, 1);
   QoreQFont *font = (p && p->getType() == NT_OBJECT) ? (QoreQFont *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTLAYOUT-CONSTRUCTOR-PARAM-ERROR", "this version of QTextLayout::constructor() expects an object derived from QFont as the second argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
      return;
   }
   ReferenceHolder<AbstractPrivateData> fontHolder(static_cast<AbstractPrivateData *>(font), xsink);
   p = get_param(params, 2);
   QoreAbstractQPaintDeviceData *paintdevice = (p && p->getType() == NT_OBJECT) ? (QoreAbstractQPaintDeviceData *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTDEVICE, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> paintdeviceHolder(static_cast<AbstractPrivateData *>(paintdevice), xsink);
   self->setPrivate(CID_QTEXTLAYOUT, new QoreQTextLayout(text, *(static_cast<QFont *>(font)), paintdevice ? paintdevice->getQPaintDevice() : 0));
}

static void QTEXTLAYOUT_copy(QoreObject *self, QoreObject *old, QoreQTextLayout *qtl, ExceptionSink *xsink)
{
   xsink->raiseException("QTEXTLAYOUT-COPY-ERROR", "objects of this class cannot be copied");
}

//QList<FormatRange> additionalFormats () const
static AbstractQoreNode *QTEXTLAYOUT_additionalFormats(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   QList<QTextLayout::FormatRange> fr = qtl->additionalFormats();
   
   QoreListNode *l = new QoreListNode();
   for (QList<QTextLayout::FormatRange>::iterator i = fr.begin(), e = fr.end(); i != e; ++i) {
      QoreHashNode *h = new QoreHashNode();

      h->setKeyValue("format", return_object(QC_QTextCharFormat, new QoreQTextCharFormat(i->format)), 0);
      h->setKeyValue("length", new QoreBigIntNode(i->length), 0);
      h->setKeyValue("start", new QoreBigIntNode(i->start), 0);
      l->push(h);
   }

   return l;
}

//void beginLayout ()
static AbstractQoreNode *QTEXTLAYOUT_beginLayout(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   qtl->beginLayout();
   return 0;
}

//QRectF boundingRect () const
static AbstractQoreNode *QTEXTLAYOUT_boundingRect(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QRectF, new QoreQRectF(qtl->boundingRect()));
}

//bool cacheEnabled () const
static AbstractQoreNode *QTEXTLAYOUT_cacheEnabled(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qtl->cacheEnabled());
}

//void clearAdditionalFormats ()
static AbstractQoreNode *QTEXTLAYOUT_clearAdditionalFormats(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   qtl->clearAdditionalFormats();
   return 0;
}

//QTextLine createLine ()
static AbstractQoreNode *QTEXTLAYOUT_createLine(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QTextLine, new QoreQTextLine(qtl->createLine()));
}

//void draw ( QPainter * p, const QPointF & pos, const QVector<FormatRange> & selections = QVector<FormatRange> (), const QRectF & clip = QRectF() ) const
static AbstractQoreNode *QTEXTLAYOUT_draw(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPainter *painter = (p && p->getType() == NT_OBJECT) ? (QoreQPainter *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTLAYOUT-DRAW-PARAM-ERROR", "expecting a QPainter object as first argument to QTextLayout::draw()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 1);
   QoreQPointF *pos = (p && p->getType() == NT_OBJECT) ? (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTLAYOUT-DRAW-PARAM-ERROR", "expecting a QPointF object as second argument to QTextLayout::draw()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> posHolder(static_cast<AbstractPrivateData *>(pos), xsink);

   QVector<QTextLayout::FormatRange> selections;

   const QoreListNode *l = test_list_param(params, 2);
   if (l) {
      ConstListIterator li_selections(l);
      while (li_selections.next()) {
	 const AbstractQoreNode *n = li_selections.getValue();
	 if (!n || n->getType() != NT_HASH) {
	    xsink->raiseException("QTEXTLAYOUT-DRAW-PARAM-ERROR", "selections list element is not a FormatRange hash (type encountered: '%s')", n ? n->getTypeName() : "NOTHING");
	    return 0;
	 }

	 QTextLayout::FormatRange fr;
	 if (get_format_range(fr, reinterpret_cast<const QoreHashNode *>(n), xsink))
	    return 0;

	 selections.push_back(fr);
      }
   }
   else
      selections = QVector<QTextLayout::FormatRange>();

   p = get_param(params, 3);
   QoreQRectF *clip = (p && p->getType() == NT_OBJECT) ? (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> clipHolder(static_cast<AbstractPrivateData *>(clip), xsink);
   qtl->draw(painter->getQPainter(), *(static_cast<QPointF *>(pos)), selections, clip ? *(static_cast<QRectF *>(clip)) : QRectF());
   return 0;
}

//void drawCursor ( QPainter * painter, const QPointF & position, int cursorPosition, int width ) const
//void drawCursor ( QPainter * painter, const QPointF & position, int cursorPosition ) const
static AbstractQoreNode *QTEXTLAYOUT_drawCursor(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   if (!o) {
      xsink->raiseException("QTEXTLAYOUT-DRAWCURSOR-PARAM-ERROR", "QTextLayout::drawCursor() expects an object derived from QPainter as the first argument");
      return 0;
   }
   QoreQPainter *painter = (QoreQPainter *)o->getReferencedPrivateData(CID_QPAINTER, xsink);
   if (!painter) {
      if (!xsink->isException())
	 xsink->raiseException("QTEXTLAYOUT-DRAWCURSOR-PARAM-ERROR", "QTextLayout::drawCursor() does not know how to handle arguments of class '%s' as passed as the first argument", o->getClassName());
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);

   o = test_object_param(params, 1);
   QoreQPointF *position = o ? (QoreQPointF *)o->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
   if (!position) {
      if (!xsink->isException())
	 xsink->raiseException("QTEXTLAYOUT-DRAWCURSOR-PARAM-ERROR", "this version of QTextLayout::drawCursor() expects an object derived from QPointF as the second argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
   const AbstractQoreNode *p = get_param(params, 2);
   int cursorPosition = p ? p->getAsInt() : 0;

   p = get_param(params, 3);
   if (is_nothing(p)) {
      qtl->drawCursor(static_cast<QPainter *>(painter->getQPainter()), *(static_cast<QPointF *>(position)), cursorPosition);
      return 0;
   }
   int width = p->getAsInt();
   qtl->drawCursor(static_cast<QPainter *>(painter->getQPainter()), *(static_cast<QPointF *>(position)), cursorPosition, width);
   return 0;
}

//void endLayout ()
static AbstractQoreNode *QTEXTLAYOUT_endLayout(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   qtl->endLayout();
   return 0;
}

//QFont font () const
static AbstractQoreNode *QTEXTLAYOUT_font(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QFont, new QoreQFont(qtl->font()));
}

//bool isValidCursorPosition ( int pos ) const
static AbstractQoreNode *QTEXTLAYOUT_isValidCursorPosition(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int pos = p ? p->getAsInt() : 0;
   return get_bool_node(qtl->isValidCursorPosition(pos));
}

//QTextLine lineAt ( int i ) const
static AbstractQoreNode *QTEXTLAYOUT_lineAt(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int i = p ? p->getAsInt() : 0;
   return return_object(QC_QTextLine, new QoreQTextLine(qtl->lineAt(i)));
}

//int lineCount () const
static AbstractQoreNode *QTEXTLAYOUT_lineCount(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->lineCount());
}

//QTextLine lineForTextPosition ( int pos ) const
static AbstractQoreNode *QTEXTLAYOUT_lineForTextPosition(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int pos = p ? p->getAsInt() : 0;
   return return_object(QC_QTextLine, new QoreQTextLine(qtl->lineForTextPosition(pos)));
}

//qreal maximumWidth () const
static AbstractQoreNode *QTEXTLAYOUT_maximumWidth(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qtl->maximumWidth());
}

//qreal minimumWidth () const
static AbstractQoreNode *QTEXTLAYOUT_minimumWidth(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreFloatNode(qtl->minimumWidth());
}

//int nextCursorPosition ( int oldPos, CursorMode mode = SkipCharacters ) const
static AbstractQoreNode *QTEXTLAYOUT_nextCursorPosition(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int oldPos = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QTextLayout::CursorMode mode = !is_nothing(p) ? (QTextLayout::CursorMode)p->getAsInt() : QTextLayout::SkipCharacters;
   return new QoreBigIntNode(qtl->nextCursorPosition(oldPos, mode));
}

//QPointF position () const
static AbstractQoreNode *QTEXTLAYOUT_position(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPointF, new QoreQPointF(qtl->position()));
}

//int preeditAreaPosition () const
static AbstractQoreNode *QTEXTLAYOUT_preeditAreaPosition(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtl->preeditAreaPosition());
}

//QString preeditAreaText () const
static AbstractQoreNode *QTEXTLAYOUT_preeditAreaText(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qtl->preeditAreaText().toUtf8().data(), QCS_UTF8);
}

//int previousCursorPosition ( int oldPos, CursorMode mode = SkipCharacters ) const
static AbstractQoreNode *QTEXTLAYOUT_previousCursorPosition(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int oldPos = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QTextLayout::CursorMode mode = !is_nothing(p) ? (QTextLayout::CursorMode)p->getAsInt() : QTextLayout::SkipCharacters;
   return new QoreBigIntNode(qtl->previousCursorPosition(oldPos, mode));
}

//void setAdditionalFormats ( const QList<FormatRange> & formatList )
static AbstractQoreNode *QTEXTLAYOUT_setAdditionalFormats(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("QTEXTLAYOUT-SETADDITIONALFORMATS-PARAM-ERROR", "expecting a list as sole argument to QTextLayout::setAdditionalFormats()"); 
      return 0;
   }
   QList<QTextLayout::FormatRange> formatList;
   ConstListIterator li_formatList(l);
   while (li_formatList.next()) {
      const AbstractQoreNode *n = li_formatList.getValue();

      if (!n || n->getType() != NT_HASH) {
	 xsink->raiseException("QTEXTLAYOUT-SETADDITIONALFORMATS-PARAM-ERROR", "formatList element is not a FormatRange hash (type encountered: '%s')", n ? n->getTypeName() : "NOTHING");
	 return 0;
      }

      QTextLayout::FormatRange fr;
      if (get_format_range(fr, reinterpret_cast<const QoreHashNode *>(n), xsink))
	 return 0;

      formatList.push_back(fr);
   }

   qtl->setAdditionalFormats(formatList);
   return 0;
}

//void setCacheEnabled ( bool enable )
static AbstractQoreNode *QTEXTLAYOUT_setCacheEnabled(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qtl->setCacheEnabled(enable);
   return 0;
}

//void setFont ( const QFont & font )
static AbstractQoreNode *QTEXTLAYOUT_setFont(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQFont *font = (p && p->getType() == NT_OBJECT) ? (QoreQFont *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTLAYOUT-SETFONT-PARAM-ERROR", "expecting a QFont object as first argument to QTextLayout::setFont()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> fontHolder(static_cast<AbstractPrivateData *>(font), xsink);
   qtl->setFont(*(static_cast<QFont *>(font)));
   return 0;
}

//void setPosition ( const QPointF & p )
static AbstractQoreNode *QTEXTLAYOUT_setPosition(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPointF *point = (p && p->getType() == NT_OBJECT) ? (QoreQPointF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
   if (!point) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTLAYOUT-SETPOSITION-PARAM-ERROR", "expecting a QPointF object as first argument to QTextLayout::setPosition()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(point), xsink);
   qtl->setPosition(*(static_cast<QPointF *>(point)));
   return 0;
}

//void setPreeditArea ( int position, const QString & text )
static AbstractQoreNode *QTEXTLAYOUT_setPreeditArea(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int position = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qtl->setPreeditArea(position, text);
   return 0;
}

//void setText ( const QString & string )
static AbstractQoreNode *QTEXTLAYOUT_setText(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString string;
   if (get_qstring(p, string, xsink))
      return 0;
   qtl->setText(string);
   return 0;
}

//void setTextOption ( const QTextOption & option )
static AbstractQoreNode *QTEXTLAYOUT_setTextOption(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQTextOption *option = (p && p->getType() == NT_OBJECT) ? (QoreQTextOption *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QTEXTOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QTEXTLAYOUT-SETTEXTOPTION-PARAM-ERROR", "expecting a QTextOption object as first argument to QTextLayout::setTextOption()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   qtl->setTextOption(*(static_cast<QTextOption *>(option)));
   return 0;
}

//QString text () const
static AbstractQoreNode *QTEXTLAYOUT_text(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qtl->text().toUtf8().data(), QCS_UTF8);
}

//QTextOption textOption () const
static AbstractQoreNode *QTEXTLAYOUT_textOption(QoreObject *self, QoreQTextLayout *qtl, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QTextOption, new QoreQTextOption(qtl->textOption()));
}

static QoreClass *initQTextLayoutClass()
{
   QC_QTextLayout = new QoreClass("QTextLayout", QDOM_GUI);
   CID_QTEXTLAYOUT = QC_QTextLayout->getID();

   QC_QTextLayout->setConstructor(QTEXTLAYOUT_constructor);
   QC_QTextLayout->setCopy((q_copy_t)QTEXTLAYOUT_copy);

   QC_QTextLayout->addMethod("additionalFormats",           (q_method_t)QTEXTLAYOUT_additionalFormats);
   QC_QTextLayout->addMethod("beginLayout",                 (q_method_t)QTEXTLAYOUT_beginLayout);
   QC_QTextLayout->addMethod("boundingRect",                (q_method_t)QTEXTLAYOUT_boundingRect);
   QC_QTextLayout->addMethod("cacheEnabled",                (q_method_t)QTEXTLAYOUT_cacheEnabled);
   QC_QTextLayout->addMethod("clearAdditionalFormats",      (q_method_t)QTEXTLAYOUT_clearAdditionalFormats);
   QC_QTextLayout->addMethod("createLine",                  (q_method_t)QTEXTLAYOUT_createLine);
   QC_QTextLayout->addMethod("draw",                        (q_method_t)QTEXTLAYOUT_draw);
   QC_QTextLayout->addMethod("drawCursor",                  (q_method_t)QTEXTLAYOUT_drawCursor);
   QC_QTextLayout->addMethod("endLayout",                   (q_method_t)QTEXTLAYOUT_endLayout);
   QC_QTextLayout->addMethod("font",                        (q_method_t)QTEXTLAYOUT_font);
   QC_QTextLayout->addMethod("isValidCursorPosition",       (q_method_t)QTEXTLAYOUT_isValidCursorPosition);
   QC_QTextLayout->addMethod("lineAt",                      (q_method_t)QTEXTLAYOUT_lineAt);
   QC_QTextLayout->addMethod("lineCount",                   (q_method_t)QTEXTLAYOUT_lineCount);
   QC_QTextLayout->addMethod("lineForTextPosition",         (q_method_t)QTEXTLAYOUT_lineForTextPosition);
   QC_QTextLayout->addMethod("maximumWidth",                (q_method_t)QTEXTLAYOUT_maximumWidth);
   QC_QTextLayout->addMethod("minimumWidth",                (q_method_t)QTEXTLAYOUT_minimumWidth);
   QC_QTextLayout->addMethod("nextCursorPosition",          (q_method_t)QTEXTLAYOUT_nextCursorPosition);
   QC_QTextLayout->addMethod("position",                    (q_method_t)QTEXTLAYOUT_position);
   QC_QTextLayout->addMethod("preeditAreaPosition",         (q_method_t)QTEXTLAYOUT_preeditAreaPosition);
   QC_QTextLayout->addMethod("preeditAreaText",             (q_method_t)QTEXTLAYOUT_preeditAreaText);
   QC_QTextLayout->addMethod("previousCursorPosition",      (q_method_t)QTEXTLAYOUT_previousCursorPosition);
   QC_QTextLayout->addMethod("setAdditionalFormats",        (q_method_t)QTEXTLAYOUT_setAdditionalFormats);
   QC_QTextLayout->addMethod("setCacheEnabled",             (q_method_t)QTEXTLAYOUT_setCacheEnabled);
   QC_QTextLayout->addMethod("setFont",                     (q_method_t)QTEXTLAYOUT_setFont);
   QC_QTextLayout->addMethod("setPosition",                 (q_method_t)QTEXTLAYOUT_setPosition);
   QC_QTextLayout->addMethod("setPreeditArea",              (q_method_t)QTEXTLAYOUT_setPreeditArea);
   QC_QTextLayout->addMethod("setText",                     (q_method_t)QTEXTLAYOUT_setText);
   QC_QTextLayout->addMethod("setTextOption",               (q_method_t)QTEXTLAYOUT_setTextOption);
   QC_QTextLayout->addMethod("text",                        (q_method_t)QTEXTLAYOUT_text);
   QC_QTextLayout->addMethod("textOption",                  (q_method_t)QTEXTLAYOUT_textOption);

   return QC_QTextLayout;
}

QoreNamespace *initQTextLayoutNS()
{
   QoreNamespace *ns = new QoreNamespace("QTextLayout");
   ns->addSystemClass(initQTextLayoutClass());

   return ns;
}
