/*
 QC_QPainterPath.cc
 
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

#include "QC_QPainterPath.h"
#include "QC_QPointF.h"
#include "QC_QPolygonF.h"
#include "QC_QRectF.h"
#include "QC_QRegion.h"
#include "QC_QFont.h"

#include "qore-qt.h"

int CID_QPAINTERPATH;
class QoreClass *QC_QPainterPath = 0;

//QPainterPath ()
//QPainterPath ( const QPointF & startPoint )
//QPainterPath ( const QPainterPath & path )
static void QPAINTERPATH_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QPAINTERPATH, new QoreQPainterPath());
      return;
   }
   QoreQPointF *startPoint = (p && p->type == NT_OBJECT) ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
   if (!startPoint) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTERPATH-CONSTRUCTOR-PARAM-ERROR", "this version of QPainterPath::constructor() expects an object derived from QPointF as the first argument", p->val.object->getClass()->getName());
      return;
   }
   ReferenceHolder<AbstractPrivateData> startPointHolder(static_cast<AbstractPrivateData *>(startPoint), xsink);
   self->setPrivate(CID_QPAINTERPATH, new QoreQPainterPath(*(static_cast<QPointF *>(startPoint))));
   return;
}

static void QPAINTERPATH_copy(class QoreObject *self, class QoreObject *old, class QoreQPainterPath *qpp, ExceptionSink *xsink)
{
   self->setPrivate(CID_QPAINTERPATH, new QoreQPainterPath(*qpp));
}

//void addEllipse ( const QRectF & boundingRectangle )
//void addEllipse ( qreal x, qreal y, qreal width, qreal height )
static QoreNode *QPAINTERPATH_addEllipse(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *boundingRectangle = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!boundingRectangle) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTERPATH-ADDELLIPSE-PARAM-ERROR", "QPainterPath::addEllipse() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> boundingRectangleHolder(static_cast<AbstractPrivateData *>(boundingRectangle), xsink);
      qpp->addEllipse(*(static_cast<QRectF *>(boundingRectangle)));
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal width = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal height = p ? p->getAsFloat() : 0.0;
   qpp->addEllipse(x, y, width, height);
   return 0;
}

//void addPath ( const QPainterPath & path )
static QoreNode *QPAINTERPATH_addPath(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainterPath *path = (p && p->type == NT_OBJECT) ? (QoreQPainterPath *)p->val.object->getReferencedPrivateData(CID_QPAINTERPATH, xsink) : 0;
   if (!path) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTERPATH-ADDPATH-PARAM-ERROR", "expecting a QPainterPath object as first argument to QPainterPath::addPath()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
   qpp->addPath(*(static_cast<QPainterPath *>(path)));
   return 0;
}

//void addPolygon ( const QPolygonF & polygon )
static QoreNode *QPAINTERPATH_addPolygon(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPolygonF *polygon = (p && p->type == NT_OBJECT) ? (QoreQPolygonF *)p->val.object->getReferencedPrivateData(CID_QPOLYGONF, xsink) : 0;
   if (!polygon) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTERPATH-ADDPOLYGON-PARAM-ERROR", "expecting a QPolygonF object as first argument to QPainterPath::addPolygon()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> polygonHolder(static_cast<AbstractPrivateData *>(polygon), xsink);
   qpp->addPolygon(*(static_cast<QPolygonF *>(polygon)));
   return 0;
}

//void addRect ( const QRectF & rectangle )
//void addRect ( qreal x, qreal y, qreal width, qreal height )
static QoreNode *QPAINTERPATH_addRect(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectangle = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectangle) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTERPATH-ADDRECT-PARAM-ERROR", "QPainterPath::addRect() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
      qpp->addRect(*(static_cast<QRectF *>(rectangle)));
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal width = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal height = p ? p->getAsFloat() : 0.0;
   qpp->addRect(x, y, width, height);
   return 0;
}

//void addRegion ( const QRegion & region )
static QoreNode *QPAINTERPATH_addRegion(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRegion *region = (p && p->type == NT_OBJECT) ? (QoreQRegion *)p->val.object->getReferencedPrivateData(CID_QREGION, xsink) : 0;
   if (!region) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTERPATH-ADDREGION-PARAM-ERROR", "expecting a QRegion object as first argument to QPainterPath::addRegion()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> regionHolder(static_cast<AbstractPrivateData *>(region), xsink);
   qpp->addRegion(*(static_cast<QRegion *>(region)));
   return 0;
}

//void addRoundRect ( const QRectF & r, int xRnd, int yRnd )
//void addRoundRect ( qreal x, qreal y, qreal w, qreal h, int xRnd, int yRnd )
//void addRoundRect ( const QRectF & rect, int roundness )
//void addRoundRect ( qreal x, qreal y, qreal w, qreal h, int roundness )
static QoreNode *QPAINTERPATH_addRoundRect(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rect = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rect) {
	 if (!xsink->isException())
	    xsink->raiseException("QPAINTERPATH-ADDROUNDRECT-PARAM-ERROR", "QPainterPath::addRoundRect() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> rHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      p = get_param(params, 1);
      if (num_params(params) > 2) {
	 int xRnd = p ? p->getAsInt() : 0;
	 p = get_param(params, 2);
	 int yRnd = p ? p->getAsInt() : 0;
	 qpp->addRoundRect(*(static_cast<QRectF *>(rect)), xRnd, yRnd);
      }
      else {
	 int roundness = p ? p->getAsInt() : 0;
	 qpp->addRoundRect(*(static_cast<QRectF *>(rect)), roundness);
      }
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal w = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal h = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   if (num_params(params) > 5) {
      int xRnd = p ? p->getAsInt() : 0;
      p = get_param(params, 5);
      int yRnd = p ? p->getAsInt() : 0;
      qpp->addRoundRect(x, y, w, h, xRnd, yRnd);
   }
   else {
      int roundness = p ? p->getAsInt() : 0;
      qpp->addRoundRect(x, y, w, h, roundness);
   }
   return 0;
}

//void addText ( const QPointF & point, const QFont & font, const QString & text )
//void addText ( qreal x, qreal y, const QFont & font, const QString & text )
static QoreNode *QPAINTERPATH_addText(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPointF *point = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (!point) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTERPATH-ADDTEXT-PARAM-ERROR", "QPainterPath::addText() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
      p = get_param(params, 1);
      QoreQFont *font = (p && p->type == NT_OBJECT) ? (QoreQFont *)p->val.object->getReferencedPrivateData(CID_QFONT, xsink) : 0;
      if (!font) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTERPATH-ADDTEXT-PARAM-ERROR", "this version of QPainterPath::addText() expects an object derived from QFont as the second argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> fontHolder(static_cast<AbstractPrivateData *>(font), xsink);
      p = get_param(params, 2);
      QString text;
      if (get_qstring(p, text, xsink))
	 return 0;
      qpp->addText(*(static_cast<QPointF *>(point)), *(static_cast<QFont *>(font)), text);
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   QoreQFont *font = (p && p->type == NT_OBJECT) ? (QoreQFont *)p->val.object->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTERPATH-ADDTEXT-PARAM-ERROR", "this version of QPainterPath::addText() expects an object derived from QFont as the third argument", p->val.object->getClass()->getName());
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> fontHolder(static_cast<AbstractPrivateData *>(font), xsink);
   p = get_param(params, 3);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   qpp->addText(x, y, *(static_cast<QFont *>(font)), text);
   return 0;
}

//qreal angleAtPercent ( qreal t ) const
static QoreNode *QPAINTERPATH_angleAtPercent(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal t = p ? p->getAsFloat() : 0.0;
   return new QoreNode((double)qpp->angleAtPercent(t));
}

//void arcMoveTo ( const QRectF & rectangle, qreal angle )
//void arcMoveTo ( qreal x, qreal y, qreal width, qreal height, qreal angle )
static QoreNode *QPAINTERPATH_arcMoveTo(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectangle = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectangle) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTERPATH-ARCMOVETO-PARAM-ERROR", "QPainterPath::arcMoveTo() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
      p = get_param(params, 1);
      qreal angle = p ? p->getAsFloat() : 0.0;
      qpp->arcMoveTo(*(static_cast<QRectF *>(rectangle)), angle);
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal width = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal height = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   qreal angle = p ? p->getAsFloat() : 0.0;
   qpp->arcMoveTo(x, y, width, height, angle);
   return 0;
}

//void arcTo ( const QRectF & rectangle, qreal startAngle, qreal sweepLength )
//void arcTo ( qreal x, qreal y, qreal width, qreal height, qreal startAngle, qreal sweepLength )
static QoreNode *QPAINTERPATH_arcTo(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   //printd(5, "QPainterPath::arcTo() p=%08p '%s' params=%d p=%d\n", p, p ? p->getTypeName() : "N/A", params ? params->size() : 0, p && p->type == NT_LIST ? (reinterpret_cast<QoreList *>(p))->size() : 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRectF *rectangle = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
      if (!rectangle) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTERPATH-ARCTO-PARAM-ERROR", "QPainterPath::arcTo() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
      p = get_param(params, 1);
      qreal startAngle = p ? p->getAsFloat() : 0.0;
      p = get_param(params, 2);
      qreal sweepLength = p ? p->getAsFloat() : 0.0;
      //printd(5, "QPainterPath::arcTo(%08p, %g, %g)\n", rectangle, startAngle, sweepLength);
      qpp->arcTo(*(static_cast<QRectF *>(rectangle)), startAngle, sweepLength);
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal width = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal height = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   qreal startAngle = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 5);
   qreal sweepLength = p ? p->getAsFloat() : 0.0;
   qpp->arcTo(x, y, width, height, startAngle, sweepLength);
   return 0;
}

//QRectF boundingRect () const
static QoreNode *QPAINTERPATH_boundingRect(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qrf = new QoreObject(QC_QRectF, getProgram());
   QoreQRectF *q_qrf = new QoreQRectF(qpp->boundingRect());
   o_qrf->setPrivate(CID_QRECTF, q_qrf);
   return new QoreNode(o_qrf);
}

//void closeSubpath ()
static QoreNode *QPAINTERPATH_closeSubpath(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   qpp->closeSubpath();
   return 0;
}

//void connectPath ( const QPainterPath & path )
static QoreNode *QPAINTERPATH_connectPath(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainterPath *path = (p && p->type == NT_OBJECT) ? (QoreQPainterPath *)p->val.object->getReferencedPrivateData(CID_QPAINTERPATH, xsink) : 0;
   if (!path) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTERPATH-CONNECTPATH-PARAM-ERROR", "expecting a QPainterPath object as first argument to QPainterPath::connectPath()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
   qpp->connectPath(*(static_cast<QPainterPath *>(path)));
   return 0;
}

//bool contains ( const QPointF & point ) const
//bool contains ( const QPainterPath & p ) const
//bool contains ( const QRectF & rectangle ) const
static QoreNode *QPAINTERPATH_contains(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQPainterPath *path = p ? (QoreQPainterPath *)p->val.object->getReferencedPrivateData(CID_QPAINTERPATH, xsink) : 0;
   if (*xsink)
      return 0;
   if (!path) {
      QoreQRectF *rectangle = p ? (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
      if (*xsink)
	 return 0;
      if (!rectangle) {
	 QoreQPointF *point = p ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
	 if (*xsink)
	    return 0;
	 if (!point) {
	    xsink->raiseException("QPAINTERPATH-CONTAINS-PARAM-ERROR", "QPainterPath::contains() expects a QRectF, QPointF, or a QPainterPath object as the first argument");
	    return 0;
	 }
	 ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
	 return new QoreNode(qpp->contains(*(static_cast<QPointF *>(point))));
      }
      ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
      return new QoreNode(qpp->contains(*(static_cast<QRectF *>(rectangle))));
   }
   ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(path), xsink);
   return new QoreNode(qpp->contains(*(static_cast<QPainterPath *>(path))));
}

//QRectF controlPointRect () const
static QoreNode *QPAINTERPATH_controlPointRect(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qrf = new QoreObject(QC_QRectF, getProgram());
   QoreQRectF *q_qrf = new QoreQRectF(qpp->controlPointRect());
   o_qrf->setPrivate(CID_QRECTF, q_qrf);
   return new QoreNode(o_qrf);
}

//void cubicTo ( const QPointF & c1, const QPointF & c2, const QPointF & endPoint )
//void cubicTo ( qreal c1X, qreal c1Y, qreal c2X, qreal c2Y, qreal endPointX, qreal endPointY )
static QoreNode *QPAINTERPATH_cubicTo(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPointF *c1 = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (!c1) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTERPATH-CUBICTO-PARAM-ERROR", "QPainterPath::cubicTo() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> c1Holder(static_cast<AbstractPrivateData *>(c1), xsink);
      p = get_param(params, 1);
      QoreQPointF *c2 = (p && p->type == NT_OBJECT) ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
      if (!c2) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTERPATH-CUBICTO-PARAM-ERROR", "this version of QPainterPath::cubicTo() expects an object derived from QPointF as the second argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> c2Holder(static_cast<AbstractPrivateData *>(c2), xsink);
      p = get_param(params, 2);
      QoreQPointF *endPoint = (p && p->type == NT_OBJECT) ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
      if (!endPoint) {
	 if (!xsink->isException())
	    xsink->raiseException("QPAINTERPATH-CUBICTO-PARAM-ERROR", "this version of QPainterPath::cubicTo() expects an object derived from QPointF as the third argument", p->val.object->getClass()->getName());
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> endPointHolder(static_cast<AbstractPrivateData *>(endPoint), xsink);
      qpp->cubicTo(*(static_cast<QPointF *>(c1)), *(static_cast<QPointF *>(c2)), *(static_cast<QPointF *>(endPoint)));
      return 0;
   }
   qreal c1X = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal c1Y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal c2X = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal c2Y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 4);
   qreal endPointX = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 5);
   qreal endPointY = p ? p->getAsFloat() : 0.0;
   qpp->cubicTo(c1X, c1Y, c2X, c2Y, endPointX, endPointY);
   return 0;
}

//QPointF currentPosition () const
static QoreNode *QPAINTERPATH_currentPosition(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qpf = new QoreObject(QC_QPointF, getProgram());
   QoreQPointF *q_qpf = new QoreQPointF(qpp->currentPosition());
   o_qpf->setPrivate(CID_QPOINTF, q_qpf);
   return new QoreNode(o_qpf);
}

/*
//const QPainterPath::Element & elementAt ( int index ) const
static QoreNode *QPAINTERPATH_elementAt(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   QPainterPath::Element element = qpp->elementAt(index);
}
*/

//int elementCount () const
static QoreNode *QPAINTERPATH_elementCount(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpp->elementCount());
}

//Qt::FillRule fillRule () const
static QoreNode *QPAINTERPATH_fillRule(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpp->fillRule());
}

//QPainterPath intersected ( const QPainterPath & p ) const
static QoreNode *QPAINTERPATH_intersected(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainterPath *path = (p && p->type == NT_OBJECT) ? (QoreQPainterPath *)p->val.object->getReferencedPrivateData(CID_QPAINTERPATH, xsink) : 0;
   if (!path) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTERPATH-INTERSECTED-PARAM-ERROR", "expecting a QPainterPath object as first argument to QPainterPath::intersected()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(path), xsink);
   QoreObject *o_qpp = new QoreObject(self->getClass(CID_QPAINTERPATH), getProgram());
   QoreQPainterPath *q_qpp = new QoreQPainterPath(qpp->intersected(*(static_cast<QPainterPath *>(path))));
   o_qpp->setPrivate(CID_QPAINTERPATH, q_qpp);
   return new QoreNode(o_qpp);
}

//bool intersects ( const QRectF & rectangle ) const
//bool intersects ( const QPainterPath & p ) const
static QoreNode *QPAINTERPATH_intersects(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQPainterPath *path = p ? (QoreQPainterPath *)p->val.object->getReferencedPrivateData(CID_QPAINTERPATH, xsink) : 0;
   if (!path) {
      QoreQRectF *rectangle = p ? (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
      if (!rectangle) {
	 if (!xsink->isException())
	    xsink->raiseException("QPAINTERPATH-INTERSECTS-PARAM-ERROR", "QPainterPath::intersects() expects a QRectF or QPainterPath object as the first argument");
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
      return new QoreNode(qpp->intersects(*(static_cast<QRectF *>(rectangle))));
   }
   ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(path), xsink);
   return new QoreNode(qpp->intersects(*(static_cast<QPainterPath *>(path))));
}


//bool isEmpty () const
static QoreNode *QPAINTERPATH_isEmpty(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qpp->isEmpty());
}

//qreal length () const
static QoreNode *QPAINTERPATH_length(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((double)qpp->length());
}

//void lineTo ( const QPointF & endPoint )
//void lineTo ( qreal x, qreal y )
static QoreNode *QPAINTERPATH_lineTo(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPointF *endPoint = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (!endPoint) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTERPATH-LINETO-PARAM-ERROR", "QPainterPath::lineTo() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> endPointHolder(static_cast<AbstractPrivateData *>(endPoint), xsink);
      qpp->lineTo(*(static_cast<QPointF *>(endPoint)));
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   qpp->lineTo(x, y);
   return 0;
}

//void moveTo ( const QPointF & point )
//void moveTo ( qreal x, qreal y )
static QoreNode *QPAINTERPATH_moveTo(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPointF *point = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (!point) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTERPATH-MOVETO-PARAM-ERROR", "QPainterPath::moveTo() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
      qpp->moveTo(*(static_cast<QPointF *>(point)));
      return 0;
   }
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal y = p ? p->getAsFloat() : 0.0;
   qpp->moveTo(x, y);
   return 0;
}

//qreal percentAtLength ( qreal len ) const
static QoreNode *QPAINTERPATH_percentAtLength(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal len = p ? p->getAsFloat() : 0.0;
   return new QoreNode((double)qpp->percentAtLength(len));
}

//QPointF pointAtPercent ( qreal t ) const
static QoreNode *QPAINTERPATH_pointAtPercent(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal t = p ? p->getAsFloat() : 0.0;
   QoreObject *o_qpf = new QoreObject(QC_QPointF, getProgram());
   QoreQPointF *q_qpf = new QoreQPointF(qpp->pointAtPercent(t));
   o_qpf->setPrivate(CID_QPOINTF, q_qpf);
   return new QoreNode(o_qpf);
}

//void quadTo ( const QPointF & c, const QPointF & endPoint )
//void quadTo ( qreal cx, qreal cy, qreal endPointX, qreal endPointY )
static QoreNode *QPAINTERPATH_quadTo(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPointF *c = (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink);
      if (!c) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTERPATH-QUADTO-PARAM-ERROR", "QPainterPath::quadTo() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> cHolder(static_cast<AbstractPrivateData *>(c), xsink);
      p = get_param(params, 1);
      QoreQPointF *endPoint = (p && p->type == NT_OBJECT) ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
      if (!endPoint) {
         if (!xsink->isException())
            xsink->raiseException("QPAINTERPATH-QUADTO-PARAM-ERROR", "this version of QPainterPath::quadTo() expects an object derived from QPointF as the second argument", p->val.object->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> endPointHolder(static_cast<AbstractPrivateData *>(endPoint), xsink);
      qpp->quadTo(*(static_cast<QPointF *>(c)), *(static_cast<QPointF *>(endPoint)));
      return 0;
   }
   qreal cx = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   qreal cy = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal endPointX = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   qreal endPointY = p ? p->getAsFloat() : 0.0;
   qpp->quadTo(cx, cy, endPointX, endPointY);
   return 0;
}

//void setElementPositionAt ( int index, qreal x, qreal y )
static QoreNode *QPAINTERPATH_setElementPositionAt(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   qreal x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   qreal y = p ? p->getAsFloat() : 0.0;
   qpp->setElementPositionAt(index, x, y);
   return 0;
}

//void setFillRule ( Qt::FillRule fillRule )
static QoreNode *QPAINTERPATH_setFillRule(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::FillRule fillRule = (Qt::FillRule)(p ? p->getAsInt() : 0);
   qpp->setFillRule(fillRule);
   return 0;
}

//qreal slopeAtPercent ( qreal t ) const
static QoreNode *QPAINTERPATH_slopeAtPercent(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   qreal t = p ? p->getAsFloat() : 0.0;
   return new QoreNode((double)qpp->slopeAtPercent(t));
}

//QPainterPath subtracted ( const QPainterPath & p ) const
static QoreNode *QPAINTERPATH_subtracted(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainterPath *path = (p && p->type == NT_OBJECT) ? (QoreQPainterPath *)p->val.object->getReferencedPrivateData(CID_QPAINTERPATH, xsink) : 0;
   if (!path) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTERPATH-SUBTRACTED-PARAM-ERROR", "expecting a QPainterPath object as first argument to QPainterPath::subtracted()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(path), xsink);
   QoreObject *o_qpp = new QoreObject(self->getClass(CID_QPAINTERPATH), getProgram());
   QoreQPainterPath *q_qpp = new QoreQPainterPath(qpp->subtracted(*(static_cast<QPainterPath *>(path))));
   o_qpp->setPrivate(CID_QPAINTERPATH, q_qpp);
   return new QoreNode(o_qpp);
}

//QPainterPath subtractedInverted ( const QPainterPath & p ) const
static QoreNode *QPAINTERPATH_subtractedInverted(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainterPath *path = (p && p->type == NT_OBJECT) ? (QoreQPainterPath *)p->val.object->getReferencedPrivateData(CID_QPAINTERPATH, xsink) : 0;
   if (!path) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTERPATH-SUBTRACTEDINVERTED-PARAM-ERROR", "expecting a QPainterPath object as first argument to QPainterPath::subtractedInverted()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(path), xsink);
   QoreObject *o_qpp = new QoreObject(self->getClass(CID_QPAINTERPATH), getProgram());
   QoreQPainterPath *q_qpp = new QoreQPainterPath(qpp->subtractedInverted(*(static_cast<QPainterPath *>(path))));
   o_qpp->setPrivate(CID_QPAINTERPATH, q_qpp);
   return new QoreNode(o_qpp);
}

/*
//QPolygonF toFillPolygon ( const QMatrix & matrix = QMatrix() ) const
//QPolygonF toFillPolygon ( const QTransform & matrix ) const
static QoreNode *QPAINTERPATH_toFillPolygon(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      QoreObject *o_qpf = new QoreObject(QC_QPolygonF, getProgram());
      QoreQPolygonF *q_qpf = new QoreQPolygonF(qpp->toFillPolygon());
      o_qpf->setPrivate(CID_QPOLYGONF, q_qpf);
      return new QoreNode(o_qpf);
   }
   if (p && p->type == NT_OBJECT) {
      QoreQMatrix *matrix = (QoreQMatrix *)p->val.object->getReferencedPrivateData(CID_QMATRIX, xsink);
      if (*xsink)
         return 0;
      ReferenceHolder<AbstractPrivateData> matrixHolder(static_cast<AbstractPrivateData *>(matrix), xsink);
      QoreObject *o_qpf = new QoreObject(QC_QPolygonF, getProgram());
      QoreQPolygonF *q_qpf = new QoreQPolygonF(qpp->toFillPolygon(*(static_cast<QMatrix *>(matrix))));
      o_qpf->setPrivate(CID_QPOLYGONF, q_qpf);
      return new QoreNode(o_qpf);
   }
   ??? QTransform matrix = p;
   QoreObject *o_qpf = new QoreObject(QC_QPolygonF, getProgram());
   QoreQPolygonF *q_qpf = new QoreQPolygonF(qpp->toFillPolygon(matrix));
   o_qpf->setPrivate(CID_QPOLYGONF, q_qpf);
   return new QoreNode(o_qpf);
}
*/

/*
//QList<QPolygonF> toFillPolygons ( const QMatrix & matrix = QMatrix() ) const
//QList<QPolygonF> toFillPolygons ( const QTransform & matrix ) const
static QoreNode *QPAINTERPATH_toFillPolygons(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      ??? return new QoreNode((int64)qpp->toFillPolygons());
   }
   if (p && p->type == NT_OBJECT) {
      QoreQMatrix *matrix = (QoreQMatrix *)p->val.object->getReferencedPrivateData(CID_QMATRIX, xsink);
      if (*xsink)
         return 0;
      ReferenceHolder<AbstractPrivateData> matrixHolder(static_cast<AbstractPrivateData *>(matrix), xsink);
      ??? return new QoreNode((int64)qpp->toFillPolygons(*(static_cast<QMatrix *>(matrix))));
   }
   ??? QTransform matrix = p;
   ??? return new QoreNode((int64)qpp->toFillPolygons(matrix));
}
*/

//QPainterPath toReversed () const
static QoreNode *QPAINTERPATH_toReversed(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qpp = new QoreObject(self->getClass(CID_QPAINTERPATH), getProgram());
   QoreQPainterPath *q_qpp = new QoreQPainterPath(qpp->toReversed());
   o_qpp->setPrivate(CID_QPAINTERPATH, q_qpp);
   return new QoreNode(o_qpp);
}

/*
//QList<QPolygonF> toSubpathPolygons ( const QMatrix & matrix = QMatrix() ) const
//QList<QPolygonF> toSubpathPolygons ( const QTransform & matrix ) const
static QoreNode *QPAINTERPATH_toSubpathPolygons(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      ??? return new QoreNode((int64)qpp->toSubpathPolygons());
   }
   if (p && p->type == NT_OBJECT) {
      QoreQMatrix *matrix = (QoreQMatrix *)p->val.object->getReferencedPrivateData(CID_QMATRIX, xsink);
      if (*xsink)
         return 0;
      ReferenceHolder<AbstractPrivateData> matrixHolder(static_cast<AbstractPrivateData *>(matrix), xsink);
      ??? return new QoreNode((int64)qpp->toSubpathPolygons(*(static_cast<QMatrix *>(matrix))));
   }
   ??? QTransform matrix = p;
   ??? return new QoreNode((int64)qpp->toSubpathPolygons(matrix));
}
*/

//QPainterPath united ( const QPainterPath & p ) const
static QoreNode *QPAINTERPATH_united(QoreObject *self, QoreQPainterPath *qpp, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainterPath *path = (p && p->type == NT_OBJECT) ? (QoreQPainterPath *)p->val.object->getReferencedPrivateData(CID_QPAINTERPATH, xsink) : 0;
   if (!path) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTERPATH-UNITED-PARAM-ERROR", "expecting a QPainterPath object as first argument to QPainterPath::united()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(path), xsink);
   QoreObject *o_qpp = new QoreObject(self->getClass(CID_QPAINTERPATH), getProgram());
   QoreQPainterPath *q_qpp = new QoreQPainterPath(qpp->united(*(static_cast<QPainterPath *>(path))));
   o_qpp->setPrivate(CID_QPAINTERPATH, q_qpp);
   return new QoreNode(o_qpp);
}

QoreClass *initQPainterPathClass()
{
   QC_QPainterPath = new QoreClass("QPainterPath", QDOM_GUI);
   CID_QPAINTERPATH = QC_QPainterPath->getID();

   QC_QPainterPath->setConstructor(QPAINTERPATH_constructor);
   QC_QPainterPath->setCopy((q_copy_t)QPAINTERPATH_copy);

   QC_QPainterPath->addMethod("addEllipse",                  (q_method_t)QPAINTERPATH_addEllipse);
   QC_QPainterPath->addMethod("addPath",                     (q_method_t)QPAINTERPATH_addPath);
   QC_QPainterPath->addMethod("addPolygon",                  (q_method_t)QPAINTERPATH_addPolygon);
   QC_QPainterPath->addMethod("addRect",                     (q_method_t)QPAINTERPATH_addRect);
   QC_QPainterPath->addMethod("addRegion",                   (q_method_t)QPAINTERPATH_addRegion);
   QC_QPainterPath->addMethod("addRoundRect",                (q_method_t)QPAINTERPATH_addRoundRect);
   QC_QPainterPath->addMethod("addText",                     (q_method_t)QPAINTERPATH_addText);
   QC_QPainterPath->addMethod("angleAtPercent",              (q_method_t)QPAINTERPATH_angleAtPercent);
   QC_QPainterPath->addMethod("arcMoveTo",                   (q_method_t)QPAINTERPATH_arcMoveTo);
   QC_QPainterPath->addMethod("arcTo",                       (q_method_t)QPAINTERPATH_arcTo);
   QC_QPainterPath->addMethod("boundingRect",                (q_method_t)QPAINTERPATH_boundingRect);
   QC_QPainterPath->addMethod("closeSubpath",                (q_method_t)QPAINTERPATH_closeSubpath);
   QC_QPainterPath->addMethod("connectPath",                 (q_method_t)QPAINTERPATH_connectPath);
   QC_QPainterPath->addMethod("contains",                    (q_method_t)QPAINTERPATH_contains);
   QC_QPainterPath->addMethod("controlPointRect",            (q_method_t)QPAINTERPATH_controlPointRect);
   QC_QPainterPath->addMethod("cubicTo",                     (q_method_t)QPAINTERPATH_cubicTo);
   QC_QPainterPath->addMethod("currentPosition",             (q_method_t)QPAINTERPATH_currentPosition);
   //QC_QPainterPath->addMethod("elementAt",                   (q_method_t)QPAINTERPATH_elementAt);
   QC_QPainterPath->addMethod("elementCount",                (q_method_t)QPAINTERPATH_elementCount);
   QC_QPainterPath->addMethod("fillRule",                    (q_method_t)QPAINTERPATH_fillRule);
   QC_QPainterPath->addMethod("intersected",                 (q_method_t)QPAINTERPATH_intersected);
   QC_QPainterPath->addMethod("intersects",                  (q_method_t)QPAINTERPATH_intersects);
   QC_QPainterPath->addMethod("isEmpty",                     (q_method_t)QPAINTERPATH_isEmpty);
   QC_QPainterPath->addMethod("length",                      (q_method_t)QPAINTERPATH_length);
   QC_QPainterPath->addMethod("lineTo",                      (q_method_t)QPAINTERPATH_lineTo);
   QC_QPainterPath->addMethod("moveTo",                      (q_method_t)QPAINTERPATH_moveTo);
   QC_QPainterPath->addMethod("percentAtLength",             (q_method_t)QPAINTERPATH_percentAtLength);
   QC_QPainterPath->addMethod("pointAtPercent",              (q_method_t)QPAINTERPATH_pointAtPercent);
   QC_QPainterPath->addMethod("quadTo",                      (q_method_t)QPAINTERPATH_quadTo);
   QC_QPainterPath->addMethod("setElementPositionAt",        (q_method_t)QPAINTERPATH_setElementPositionAt);
   QC_QPainterPath->addMethod("setFillRule",                 (q_method_t)QPAINTERPATH_setFillRule);
   QC_QPainterPath->addMethod("slopeAtPercent",              (q_method_t)QPAINTERPATH_slopeAtPercent);
   QC_QPainterPath->addMethod("subtracted",                  (q_method_t)QPAINTERPATH_subtracted);
   QC_QPainterPath->addMethod("subtractedInverted",          (q_method_t)QPAINTERPATH_subtractedInverted);
   //QC_QPainterPath->addMethod("toFillPolygon",               (q_method_t)QPAINTERPATH_toFillPolygon);
   //QC_QPainterPath->addMethod("toFillPolygons",              (q_method_t)QPAINTERPATH_toFillPolygons);
   QC_QPainterPath->addMethod("toReversed",                  (q_method_t)QPAINTERPATH_toReversed);
   //QC_QPainterPath->addMethod("toSubpathPolygons",           (q_method_t)QPAINTERPATH_toSubpathPolygons);
   QC_QPainterPath->addMethod("united",                      (q_method_t)QPAINTERPATH_united);

   return QC_QPainterPath;
}
