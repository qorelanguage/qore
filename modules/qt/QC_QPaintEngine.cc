/*
 QC_QPaintEngine.cc
 
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

#include "QC_QPaintEngine.h"

int CID_QPAINTENGINE;
class QoreClass *QC_QPaintEngine = 0;

//QPaintEngine ( PaintEngineFeatures caps = 0 )
static void QPAINTENGINE_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("QPAINTENGINE-CONSTRUCTOR-ERROR", "QPaintEngine is an abstract class");
}

static void QPAINTENGINE_copy(class QoreObject *self, class QoreObject *old, class QoreQPaintEngine *qpe, ExceptionSink *xsink)
{
   xsink->raiseException("QPAINTENGINE-COPY-ERROR", "objects of this class cannot be copied");
}

//virtual bool begin ( QPaintDevice * pdev ) = 0
static QoreNode *QPAINTENGINE_begin(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   AbstractPrivateData *apd_pdev = (p && p->type == NT_OBJECT) ? p->val.object->getReferencedPrivateData(CID_QPAINTDEVICE, xsink) : 0;
   if (!apd_pdev) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTENGINE-BEGIN-PARAM-ERROR", "expecting a QPaintDevice object as first argument to QPaintEngine::begin()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> holder(apd_pdev, xsink);
   QoreAbstractQPaintDevice *pdev = dynamic_cast<QoreAbstractQPaintDevice *>(apd_pdev);
   assert(pdev);
   return new QoreNode(qpe->getQPaintEngine()->begin(pdev->getQPaintDevice()));
}

//virtual void drawEllipse ( const QRectF & rect )
//virtual void drawEllipse ( const QRect & rect )
static QoreNode *QPAINTENGINE_drawEllipse(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQRect *rect = p ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (*xsink)
      return 0;
   if (!rect) {
      QoreQRectF *rect = p ? (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
      if (*xsink)
	 return 0;
      if (!rect) {
	 if (!xsink->isException())
	    xsink->raiseException("QPAINTENGINE-DRAWELLIPSE-PARAM-ERROR", "QPaintEngine::drawEllipse() expects a QRectF or QRect as the sole argument");
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      qpe->getQPaintEngine()->drawEllipse(*(static_cast<QRectF *>(rect)));
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
   qpe->getQPaintEngine()->drawEllipse(*(static_cast<QRect *>(rect)));
   return 0;
}

//virtual void drawImage ( const QRectF & rectangle, const QImage & image, const QRectF & sr, Qt::ImageConversionFlags flags = Qt::AutoColor )
static QoreNode *QPAINTENGINE_drawImage(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRectF *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTENGINE-DRAWIMAGE-PARAM-ERROR", "expecting a QRectF object as first argument to QPaintEngine::drawImage()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
   p = get_param(params, 1);
   QoreQImage *image = (p && p->type == NT_OBJECT) ? (QoreQImage *)p->val.object->getReferencedPrivateData(CID_QIMAGE, xsink) : 0;
   if (!image) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTENGINE-DRAWIMAGE-PARAM-ERROR", "expecting a QImage object as second argument to QPaintEngine::drawImage()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> imageHolder(static_cast<AbstractPrivateData *>(image), xsink);
   p = get_param(params, 2);
   QoreQRectF *sr = (p && p->type == NT_OBJECT) ? (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
   if (!sr) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTENGINE-DRAWIMAGE-PARAM-ERROR", "expecting a QRectF object as third argument to QPaintEngine::drawImage()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> srHolder(static_cast<AbstractPrivateData *>(sr), xsink);
   p = get_param(params, 3);
   Qt::ImageConversionFlags flags = !is_nothing(p) ? (Qt::ImageConversionFlags)p->getAsInt() : Qt::AutoColor;
   qpe->getQPaintEngine()->drawImage(*(static_cast<QRectF *>(rectangle)), *(static_cast<QImage *>(image)), *(static_cast<QRectF *>(sr)), flags);
   return 0;
}

//virtual void drawLines ( const QLineF * lines, int lineCount )
//virtual void drawLines ( const QLine * lines, int lineCount )
static QoreNode *QPAINTENGINE_drawLines(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQLine *lines = p ? (QoreQLine *)p->val.object->getReferencedPrivateData(CID_QLINE, xsink) : 0;
   if (*xsink)
      return 0;
   if (!lines) {
      QoreQLineF *lines = p ? (QoreQLineF *)p->val.object->getReferencedPrivateData(CID_QLINEF, xsink) : 0;
      if (*xsink)
	 return 0;
      if (!lines) {
	 xsink->raiseException("QPAINTENGINE-DRAWLINES-PARAM-ERROR", "QPaintEngine::drawLines() expects a QLine or QLineF as the sole argument");
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> linesHolder(static_cast<AbstractPrivateData *>(lines), xsink);
      p = get_param(params, 1);
      int lineCount = p ? p->getAsInt() : 0;
      qpe->getQPaintEngine()->drawLines(static_cast<QLineF *>(lines), lineCount);
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> linesHolder(static_cast<AbstractPrivateData *>(lines), xsink);
   p = get_param(params, 1);
   int lineCount = p ? p->getAsInt() : 0;
   qpe->getQPaintEngine()->drawLines(static_cast<QLine *>(lines), lineCount);
   return 0;
}

//virtual void drawPath ( const QPainterPath & path )
static QoreNode *QPAINTENGINE_drawPath(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainterPath *path = (p && p->type == NT_OBJECT) ? (QoreQPainterPath *)p->val.object->getReferencedPrivateData(CID_QPAINTERPATH, xsink) : 0;
   if (!path) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTENGINE-DRAWPATH-PARAM-ERROR", "expecting a QPainterPath object as first argument to QPaintEngine::drawPath()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pathHolder(static_cast<AbstractPrivateData *>(path), xsink);
   qpe->getQPaintEngine()->drawPath(*(static_cast<QPainterPath *>(path)));
   return 0;
}

//virtual void drawPixmap ( const QRectF & r, const QPixmap & pm, const QRectF & sr ) = 0
static QoreNode *QPAINTENGINE_drawPixmap(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRectF *r = (p && p->type == NT_OBJECT) ? (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
   if (!r) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTENGINE-DRAWPIXMAP-PARAM-ERROR", "expecting a QRectF object as first argument to QPaintEngine::drawPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> rHolder(static_cast<AbstractPrivateData *>(r), xsink);
   p = get_param(params, 1);
   QoreQPixmap *pm = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pm) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTENGINE-DRAWPIXMAP-PARAM-ERROR", "expecting a QPixmap object as second argument to QPaintEngine::drawPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pmHolder(static_cast<AbstractPrivateData *>(pm), xsink);
   p = get_param(params, 2);
   QoreQRectF *sr = (p && p->type == NT_OBJECT) ? (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
   if (!sr) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTENGINE-DRAWPIXMAP-PARAM-ERROR", "expecting a QRectF object as third argument to QPaintEngine::drawPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> srHolder(static_cast<AbstractPrivateData *>(sr), xsink);
   qpe->getQPaintEngine()->drawPixmap(*(static_cast<QRectF *>(r)), *(static_cast<QPixmap *>(pm)), *(static_cast<QRectF *>(sr)));
   return 0;
}

/*
//virtual void drawPoints ( const QPointF * points, int pointCount )
//virtual void drawPoints ( const QPoint * points, int pointCount )
static QoreNode *QPAINTENGINE_drawPoints(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQPoint *points = p ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (*xsink)
      return 0;
   if (!points) {
      QoreQPointF *points = p ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
      if (*xsink)
	 return 0;
      if (!points) {
	 xsink->raiseException("QPAINTENGINE-DRAWPOINTS-PARAM-ERROR", "QPaintEngine::drawPoints() expects a QPoint or QPointF as the first argument");
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> pointsHolder(static_cast<AbstractPrivateData *>(points), xsink);
      p = get_param(params, 1);
      int pointCount = p ? p->getAsInt() : 0;
      qpe->getQPaintEngine()->drawPoints(static_cast<QPointF *>(points), pointCount);
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pointsHolder(static_cast<AbstractPrivateData *>(points), xsink);
   p = get_param(params, 1);
   int pointCount = p ? p->getAsInt() : 0;
   qpe->getQPaintEngine()->drawPoints(static_cast<QPoint *>(points), pointCount);
   return 0;
}
*/

/*
//virtual void drawPolygon ( const QPointF * points, int pointCount, PolygonDrawMode mode )
//virtual void drawPolygon ( const QPoint * points, int pointCount, PolygonDrawMode mode )
static QoreNode *QPAINTENGINE_drawPolygon(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQPoint *points = p ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!points) {
      QoreQPointF *points = p ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
      if (!points) {
	 xsink->raiseException("QPAINTENGINE-DRAWPOLYGON-PARAM-ERROR", "QPaintEngine::drawPolygon() expects a XXXXthe first argument" XXXX);
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> pointsHolder(static_cast<AbstractPrivateData *>(points), xsink);
      p = get_param(params, 1);
      int pointCount = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      QPaintEngine::PolygonDrawMode mode = (QPaintEngine::PolygonDrawMode)(p ? p->getAsInt() : 0);
      qpe->getQPaintEngine()->drawPolygon(static_cast<QPointF *>(points), pointCount, mode);
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pointsHolder(static_cast<AbstractPrivateData *>(points), xsink);
   p = get_param(params, 1);
   int pointCount = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QPaintEngine::PolygonDrawMode mode = (QPaintEngine::PolygonDrawMode)(p ? p->getAsInt() : 0);
   qpe->getQPaintEngine()->drawPolygon(static_cast<QPoint *>(points), pointCount, mode);
   return 0;
}
*/

/*
//virtual void drawRects ( const QRectF * rects, int rectCount )
//virtual void drawRects ( const QRect * rects, int rectCount )
static QoreNode *QPAINTENGINE_drawRects(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRect *rects = (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink);
      if (!rects) {
         QoreQRectF *rects = (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink);
         if (!rects) {
            if (!xsink->isException())
               xsink->raiseException("QPAINTENGINE-DRAWRECTS-PARAM-ERROR", "QPaintEngine::drawRects() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> rectsHolder(static_cast<AbstractPrivateData *>(rects), xsink);
         p = get_param(params, 1);
         int rectCount = p ? p->getAsInt() : 0;
         qpe->getQPaintEngine()->drawRects(static_cast<QRectF *>(rects), rectCount);
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> rectsHolder(static_cast<AbstractPrivateData *>(rects), xsink);
      p = get_param(params, 1);
      int rectCount = p ? p->getAsInt() : 0;
      qpe->getQPaintEngine()->drawRects(static_cast<QRect *>(rects), rectCount);
      return 0;
   }
}
*/

/*
//virtual void drawTextItem ( const QPointF & p, const QTextItem & textItem )
static QoreNode *QPAINTENGINE_drawTextItem(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPointF *p = (p && p->type == NT_OBJECT) ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
   if (!p) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTENGINE-DRAWTEXTITEM-PARAM-ERROR", "expecting a QPointF object as first argument to QPaintEngine::drawTextItem()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(p), xsink);
   p = get_param(params, 1);
   ??? QTextItem textItem = p;
   qpe->getQPaintEngine()->drawTextItem(*(static_cast<QPointF *>(p)), textItem);
   return 0;
}
*/

//virtual void drawTiledPixmap ( const QRectF & rect, const QPixmap & pixmap, const QPointF & p )
static QoreNode *QPAINTENGINE_drawTiledPixmap(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRectF *rect = (p && p->type == NT_OBJECT) ? (QoreQRectF *)p->val.object->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
   if (!rect) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTENGINE-DRAWTILEDPIXMAP-PARAM-ERROR", "expecting a QRectF object as first argument to QPaintEngine::drawTiledPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
   p = get_param(params, 1);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTENGINE-DRAWTILEDPIXMAP-PARAM-ERROR", "expecting a QPixmap object as second argument to QPaintEngine::drawTiledPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   p = get_param(params, 2);
   QoreQPointF *point = (p && p->type == NT_OBJECT) ? (QoreQPointF *)p->val.object->getReferencedPrivateData(CID_QPOINTF, xsink) : 0;
   if (!point) {
      if (!xsink->isException())
         xsink->raiseException("QPAINTENGINE-DRAWTILEDPIXMAP-PARAM-ERROR", "expecting a QPointF object as third argument to QPaintEngine::drawTiledPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(point), xsink);
   qpe->getQPaintEngine()->drawTiledPixmap(*(static_cast<QRectF *>(rect)), *(static_cast<QPixmap *>(pixmap)), *(static_cast<QPointF *>(point)));
   return 0;
}

//virtual bool end () = 0
static QoreNode *QPAINTENGINE_end(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qpe->getQPaintEngine()->end());
}

//bool hasFeature ( PaintEngineFeatures feature ) const
static QoreNode *QPAINTENGINE_hasFeature(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPaintEngine::PaintEngineFeatures feature = (QPaintEngine::PaintEngineFeatures)(p ? p->getAsInt() : 0);
   return new QoreNode(qpe->getQPaintEngine()->hasFeature(feature));
}

//bool isActive () const
static QoreNode *QPAINTENGINE_isActive(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qpe->getQPaintEngine()->isActive());
}

//QPaintDevice * paintDevice () const
static QoreNode *QPAINTENGINE_paintDevice(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QPaintDevice *qpd = qpe->getQPaintEngine()->paintDevice();

   if (!qpd)
      return 0;

   QoreObject *o = new QoreObject(QC_QPaintDevice, getProgram());
   o->setPrivate(CID_QPAINTDEVICE, new QoreQtQPaintDevice(qpd));
   return new QoreNode(o);
}

//QPainter * painter () const
static QoreNode *QPAINTENGINE_painter(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPainter, getProgram());
   QoreQPainter *q_qp = new QoreQPainter(qpe->getQPaintEngine()->painter());
   o_qp->setPrivate(CID_QPAINTER, q_qp);
   return new QoreNode(o_qp);
}

//void setActive ( bool state )
static QoreNode *QPAINTENGINE_setActive(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool state = p ? p->getAsBool() : false;
   qpe->getQPaintEngine()->setActive(state);
   return 0;
}

//virtual Type type () const = 0
static QoreNode *QPAINTENGINE_type(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qpe->getQPaintEngine()->type());
}

/*
//virtual void updateState ( const QPaintEngineState & state ) = 0
static QoreNode *QPAINTENGINE_updateState(QoreObject *self, QoreAbstractQPaintEngine *qpe, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   ??? QPaintEngineState state = p;
   qpe->getQPaintEngine()->updateState(state);
   return 0;
}
*/

QoreClass *initQPaintEngineClass()
{
   QC_QPaintEngine = new QoreClass("QPaintEngine", QDOM_GUI);
   CID_QPAINTENGINE = QC_QPaintEngine->getID();

   QC_QPaintEngine->setConstructor(QPAINTENGINE_constructor);
   QC_QPaintEngine->setCopy((q_copy_t)QPAINTENGINE_copy);

   QC_QPaintEngine->addMethod("begin",                       (q_method_t)QPAINTENGINE_begin);
   QC_QPaintEngine->addMethod("drawEllipse",                 (q_method_t)QPAINTENGINE_drawEllipse);
   QC_QPaintEngine->addMethod("drawImage",                   (q_method_t)QPAINTENGINE_drawImage);
   QC_QPaintEngine->addMethod("drawLines",                   (q_method_t)QPAINTENGINE_drawLines);
   QC_QPaintEngine->addMethod("drawPath",                    (q_method_t)QPAINTENGINE_drawPath);
   QC_QPaintEngine->addMethod("drawPixmap",                  (q_method_t)QPAINTENGINE_drawPixmap);
   //QC_QPaintEngine->addMethod("drawPoints",                  (q_method_t)QPAINTENGINE_drawPoints);
   //QC_QPaintEngine->addMethod("drawPolygon",                 (q_method_t)QPAINTENGINE_drawPolygon);
   //QC_QPaintEngine->addMethod("drawRects",                   (q_method_t)QPAINTENGINE_drawRects);
   //QC_QPaintEngine->addMethod("drawTextItem",                (q_method_t)QPAINTENGINE_drawTextItem);
   QC_QPaintEngine->addMethod("drawTiledPixmap",             (q_method_t)QPAINTENGINE_drawTiledPixmap);
   QC_QPaintEngine->addMethod("end",                         (q_method_t)QPAINTENGINE_end);
   QC_QPaintEngine->addMethod("hasFeature",                  (q_method_t)QPAINTENGINE_hasFeature);
   QC_QPaintEngine->addMethod("isActive",                    (q_method_t)QPAINTENGINE_isActive);
   QC_QPaintEngine->addMethod("paintDevice",                 (q_method_t)QPAINTENGINE_paintDevice);
   QC_QPaintEngine->addMethod("painter",                     (q_method_t)QPAINTENGINE_painter);
   QC_QPaintEngine->addMethod("setActive",                   (q_method_t)QPAINTENGINE_setActive);
   QC_QPaintEngine->addMethod("type",                        (q_method_t)QPAINTENGINE_type);
   //QC_QPaintEngine->addMethod("updateState",                 (q_method_t)QPAINTENGINE_updateState);

   return QC_QPaintEngine;
}
