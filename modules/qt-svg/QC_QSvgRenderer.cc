/*
 QC_QSvgRenderer.cc
 
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

#include "QC_QSvgRenderer.h"
#include "QC_QObject.h"
#include "QC_QRectF.h"
#include "QC_QSize.h"
#include "QC_QMatrix.h"
#include "QC_QRect.h"
#include "QC_QPainter.h"

int CID_QSVGRENDERER;
class QoreClass *QC_QSvgRenderer = 0;

//QSvgRenderer ( QObject * parent = 0 )
//QSvgRenderer ( const QString & filename, QObject * parent = 0 )
//QSvgRenderer ( const QByteArray & contents, QObject * parent = 0 )
static void QSVGRENDERER_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QSVGRENDERER, new QoreQSvgRenderer(self));
      return;
   }

   if (p->getType() == NT_STRING) {
      QString filename;
      if (get_qstring(p, filename, xsink))
	 return;

      p = get_param(params, 1);
      QoreAbstractQObject *parent = (p && p->getType() == NT_OBJECT) ? (QoreAbstractQObject *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QSVGRENDERER, new QoreQSvgRenderer(self, filename, parent ? parent->getQObject() : 0));
      return;      
   }

   QByteArray contents;
   if (!get_qbytearray(p, contents, xsink, true)) {
      p = get_param(params, 1);
      QoreAbstractQObject *parent = (p && p->getType() == NT_OBJECT) ? (QoreAbstractQObject *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QSVGRENDERER, new QoreQSvgRenderer(self, contents, parent ? parent->getQObject() : 0));
   }

   if (*xsink)
      return;

   xsink->raiseException("QSVGRENDERER-CONSTRUCTOR-ERROR", "expecting either a QByteArray (or equivalent) or a string or a QObject as the first argument to QSvgRenderer::constructor()");
}

static void QSVGRENDERER_copy(QoreObject *self, QoreObject *old, QoreQSvgRenderer *qsvgr, ExceptionSink *xsink)
{
   xsink->raiseException("QSVGRENDERER-COPY-ERROR", "objects of this class cannot be copied");
}

//bool animated () const
static AbstractQoreNode *QSVGRENDERER_animated(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qsvgr->qobj->animated());
}

//QRectF boundsOnElement ( const QString & id ) const
static AbstractQoreNode *QSVGRENDERER_boundsOnElement(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString id;
   if (get_qstring(p, id, xsink))
      return 0;
   return return_object(QC_QRectF, new QoreQRectF(qsvgr->qobj->boundsOnElement(id)));
}

//QSize defaultSize () const
static AbstractQoreNode *QSVGRENDERER_defaultSize(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QSize, new QoreQSize(qsvgr->qobj->defaultSize()));
}

//bool elementExists ( const QString & id ) const
static AbstractQoreNode *QSVGRENDERER_elementExists(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString id;
   if (get_qstring(p, id, xsink))
      return 0;
   return get_bool_node(qsvgr->qobj->elementExists(id));
}

//int framesPerSecond () const
static AbstractQoreNode *QSVGRENDERER_framesPerSecond(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qsvgr->qobj->framesPerSecond());
}

//bool isValid () const
static AbstractQoreNode *QSVGRENDERER_isValid(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qsvgr->qobj->isValid());
}

//QMatrix matrixForElement ( const QString & id ) const
static AbstractQoreNode *QSVGRENDERER_matrixForElement(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString id;
   if (get_qstring(p, id, xsink))
      return 0;
   return return_object(QC_QMatrix, new QoreQMatrix(qsvgr->qobj->matrixForElement(id)));
}

//void setFramesPerSecond ( int num )
static AbstractQoreNode *QSVGRENDERER_setFramesPerSecond(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int num = p ? p->getAsInt() : 0;
   qsvgr->qobj->setFramesPerSecond(num);
   return 0;
}

//void setViewBox ( const QRect & viewbox )
//void setViewBox ( const QRectF & viewbox )
static AbstractQoreNode *QSVGRENDERER_setViewBox(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_OBJECT) {
      xsink->raiseException("QSVGRENDERER-SETVIEWBOX-PARAM-ERROR", "QSvgRenderer::setViewBox() does not know how to handle arguments of type '%s' as passed as the first argument", p ? p->getTypeName() : "NOTHING");
      return 0;
   }

   QoreQRectF *viewbox = (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink);
   if (!viewbox) {
      QoreQRect *viewbox = (QoreQRect *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECT, xsink);
      if (!viewbox) {
	 if (!xsink->isException())
	    xsink->raiseException("QSVGRENDERER-SETVIEWBOX-PARAM-ERROR", "QSvgRenderer::setViewBox() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> viewboxHolder(static_cast<AbstractPrivateData *>(viewbox), xsink);
      qsvgr->qobj->setViewBox(*(static_cast<QRect *>(viewbox)));
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> viewboxHolder(static_cast<AbstractPrivateData *>(viewbox), xsink);
   qsvgr->qobj->setViewBox(*(static_cast<QRectF *>(viewbox)));
   return 0;
}

//QRect viewBox () const
static AbstractQoreNode *QSVGRENDERER_viewBox(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QRect, new QoreQRect(qsvgr->qobj->viewBox()));
}

//QRectF viewBoxF () const
static AbstractQoreNode *QSVGRENDERER_viewBoxF(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QRectF, new QoreQRectF(qsvgr->qobj->viewBoxF()));
}

//bool load ( const QString & filename )
//bool load ( const QByteArray & contents )
static AbstractQoreNode *QSVGRENDERER_load(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString filename;
   if (get_qstring(p, filename, xsink))
      return 0;
   return get_bool_node(qsvgr->qobj->load(filename));
}

//void render ( QPainter * painter )
//void render ( QPainter * painter, const QRectF & bounds )
//void render ( QPainter * painter, const QString & elementId, const QRectF & bounds = QRectF() )
static AbstractQoreNode *QSVGRENDERER_render(QoreObject *self, QoreQSvgRenderer *qsvgr, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_OBJECT) {
      xsink->raiseException("QSVGRENDERER-RENDER-PARAM-ERROR", "QSvgRenderer::render() does not know how to handle arguments of type '%s' as passed as the first argument (expecing an object derived from QPainter)", p ? p->getTypeName() : "NOTHING");
      return 0;
   }
   QoreQPainter *painter = (QoreQPainter *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTER, xsink);
   if (!painter) {
      if (!xsink->isException())
	 xsink->raiseException("QSVGRENDERER-RENDER-PARAM-ERROR", "QSvgRenderer::render() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   if (num_params(params) == 1) {
      qsvgr->qobj->render(painter->getQPainter());
      return 0;
   }

   p = get_param(params, 1);
   if (p && p->getType() == NT_OBJECT) {
      const QoreObject *o = reinterpret_cast<const QoreObject *>(p);
      QoreQRectF *bounds = (QoreQRectF *)o->getReferencedPrivateData(CID_QRECTF, xsink);
      if (*xsink)
	 return 0;
      
      if (!bounds) {
	 xsink->raiseException("QSVGRENDERER-RENDER-PARAM-ERROR", "QSvgRenderer::render() does not know how to handle arguments of class '%s' as passed as the second argument", o->getClassName());
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> bounds_holder(bounds, xsink);
      qsvgr->qobj->render(painter->getQPainter(), *bounds);
      return 0;
   }

   QString elementId;
   if (get_qstring(p, elementId, xsink))
      return 0;

   p = get_param(params, 2);
   QoreQRectF *bounds = (p && p->getType() == NT_OBJECT) ? (QoreQRectF *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECTF, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> boundsHolder(static_cast<AbstractPrivateData *>(bounds), xsink);
   qsvgr->qobj->render(painter->getQPainter(), elementId, *(static_cast<QRectF *>(bounds)));
   return 0;
}

QoreClass *initQSvgRendererClass(QoreClass *qobject)
{
   QC_QSvgRenderer = new QoreClass("QSvgRenderer", QDOM_GUI);
   CID_QSVGRENDERER = QC_QSvgRenderer->getID();

   QC_QSvgRenderer->addBuiltinVirtualBaseClass(qobject);

   QC_QSvgRenderer->setConstructor(QSVGRENDERER_constructor);
   QC_QSvgRenderer->setCopy((q_copy_t)QSVGRENDERER_copy);

   QC_QSvgRenderer->addMethod("animated",                    (q_method_t)QSVGRENDERER_animated);
   QC_QSvgRenderer->addMethod("boundsOnElement",             (q_method_t)QSVGRENDERER_boundsOnElement);
   QC_QSvgRenderer->addMethod("defaultSize",                 (q_method_t)QSVGRENDERER_defaultSize);
   QC_QSvgRenderer->addMethod("elementExists",               (q_method_t)QSVGRENDERER_elementExists);
   QC_QSvgRenderer->addMethod("framesPerSecond",             (q_method_t)QSVGRENDERER_framesPerSecond);
   QC_QSvgRenderer->addMethod("isValid",                     (q_method_t)QSVGRENDERER_isValid);
   QC_QSvgRenderer->addMethod("matrixForElement",            (q_method_t)QSVGRENDERER_matrixForElement);
   QC_QSvgRenderer->addMethod("setFramesPerSecond",          (q_method_t)QSVGRENDERER_setFramesPerSecond);
   QC_QSvgRenderer->addMethod("setViewBox",                  (q_method_t)QSVGRENDERER_setViewBox);
   QC_QSvgRenderer->addMethod("viewBox",                     (q_method_t)QSVGRENDERER_viewBox);
   QC_QSvgRenderer->addMethod("viewBoxF",                    (q_method_t)QSVGRENDERER_viewBoxF);
   QC_QSvgRenderer->addMethod("load",                        (q_method_t)QSVGRENDERER_load);
   QC_QSvgRenderer->addMethod("render",                      (q_method_t)QSVGRENDERER_render);

   return QC_QSvgRenderer;
}
