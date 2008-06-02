/*
 QC_QGLWidget.cc
 
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

#include "QC_QGLWidget.h"
#include "QC_QGLContext.h"
#include "QC_QGLFormat.h"
#include "QC_QGLColormap.h"
#include "QC_QWidget.h"
#include "QC_QImage.h"
#include "QC_QColor.h"
#include "QC_QPixmap.h"
#include "QC_QFont.h"

int CID_QGLWIDGET;
QoreClass *QC_QGLWidget = 0;

//QGLWidget ( QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0 )
//QGLWidget ( QGLContext * context, QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0 )
//QGLWidget ( const QGLFormat & format, QWidget * parent = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0 )
static void QGLWIDGET_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QGLWIDGET, new QoreQGLWidget(self));
      return;
   }

   int offset = 1;

   QoreQGLContext *context = 0;
   QoreQGLFormat *format = 0;

   const QoreObject *o = p->getType() == NT_OBJECT ? reinterpret_cast<const QoreObject *>(p) : 0;
   context = o ? (QoreQGLContext *)o->getReferencedPrivateData(CID_QGLCONTEXT, xsink) : 0;
   if (!context && !*xsink) {
      format = o ? (QoreQGLFormat *)o->getReferencedPrivateData(CID_QGLFORMAT, xsink) : 0;
      if (!format) {
	 offset = 0;
      }
   }

   // assign holder variables
   ReferenceHolder<AbstractPrivateData> formatHolder(static_cast<AbstractPrivateData *>(format), xsink);
   ReferenceHolder<AbstractPrivateData> contextHolder(static_cast<AbstractPrivateData *>(context), xsink);

   // return if an exception occured
   if (*xsink)
      return;

   // process the rest of the common parameters
   p = get_param(params, offset++);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);

   p = get_param(params, offset++);
   QoreQGLWidget *shareWidget = (p && p->getType() == NT_OBJECT) ? (QoreQGLWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGLWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> shareWidgetHolder(static_cast<AbstractPrivateData *>(shareWidget), xsink);

   p = get_param(params, offset);
   Qt::WindowFlags f = (Qt::WindowFlags)(!is_nothing(p) ? p->getAsInt() : 0);

   if (context)
      self->setPrivate(CID_QGLWIDGET, new QoreQGLWidget(self, context, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, shareWidget ? static_cast<QGLWidget *>(shareWidget->qobj) : 0, f));
   else if (format)
      self->setPrivate(CID_QGLWIDGET, new QoreQGLWidget(self, *format, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, shareWidget ? static_cast<QGLWidget *>(shareWidget->qobj) : 0, f));
   else
      self->setPrivate(CID_QGLWIDGET, new QoreQGLWidget(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0, shareWidget ? static_cast<QGLWidget *>(shareWidget->qobj) : 0, f));
}

static void QGLWIDGET_copy(QoreObject *self, QoreObject *old, QoreQGLWidget *qglw, ExceptionSink *xsink)
{
   xsink->raiseException("QGLWIDGET-COPY-ERROR", "objects of this class cannot be copied");
}

//GLuint bindTexture ( const QImage & image, GLenum target = GL_TEXTURE_2D, GLint format = GL_RGBA )
//GLuint bindTexture ( const QPixmap & pixmap, GLenum target = GL_TEXTURE_2D, GLint format = GL_RGBA )
//GLuint bindTexture ( const QString & fileName )
static AbstractQoreNode *QGLWIDGET_bindTexture(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPixmap *pixmap = (QoreQPixmap *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPIXMAP, xsink);
      if (!pixmap) {
         QoreQImage *image = (QoreQImage *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QIMAGE, xsink);
         if (!image) {
            if (!xsink->isException())
               xsink->raiseException("QGLWIDGET-BINDTEXTURE-PARAM-ERROR", "QGLWidget::bindTexture() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> imageHolder(static_cast<AbstractPrivateData *>(image), xsink);
         p = get_param(params, 1);
         GLenum target = !is_nothing(p) ? (GLenum)p->getAsInt() : GL_TEXTURE_2D;
         p = get_param(params, 2);
         GLint format = !is_nothing(p) ? (GLint)p->getAsInt() : GL_RGBA;
         return new QoreBigIntNode(qglw->qobj->bindTexture(*(static_cast<QImage *>(image)), target, format));
      }
      ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
      p = get_param(params, 1);
      GLenum target = !is_nothing(p) ? (GLenum)p->getAsInt() : GL_TEXTURE_2D;
      p = get_param(params, 2);
      GLint format = !is_nothing(p) ? (GLint)p->getAsInt() : GL_RGBA;
      return new QoreBigIntNode(qglw->qobj->bindTexture(*(static_cast<QPixmap *>(pixmap)), target, format));
   }
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   return new QoreBigIntNode(qglw->qobj->bindTexture(fileName));
}

//const QGLColormap & colormap () const
static AbstractQoreNode *QGLWIDGET_colormap(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QGLColormap, new QoreQGLColormap(qglw->qobj->colormap()));
}

//const QGLContext * context () const
static AbstractQoreNode *QGLWIDGET_context(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QGLContext, new QoreQtQGLContext(const_cast<QGLContext *>(qglw->qobj->context())));
}

//void deleteTexture ( GLuint id )
static AbstractQoreNode *QGLWIDGET_deleteTexture(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLuint id = (GLuint)(p ? p->getAsInt() : 0);
   qglw->qobj->deleteTexture(id);
   return 0;
}

//void doneCurrent ()
static AbstractQoreNode *QGLWIDGET_doneCurrent(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   qglw->qobj->doneCurrent();
   return 0;
}

//bool doubleBuffer () const
static AbstractQoreNode *QGLWIDGET_doubleBuffer(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglw->qobj->doubleBuffer());
}

//QGLFormat format () const
static AbstractQoreNode *QGLWIDGET_format(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QGLFormat, new QoreQGLFormat(qglw->qobj->format()));
}

//QImage grabFrameBuffer ( bool withAlpha = false )
static AbstractQoreNode *QGLWIDGET_grabFrameBuffer(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool withAlpha = p ? p->getAsBool() : false;
   return return_object(QC_QImage, new QoreQImage(qglw->qobj->grabFrameBuffer(withAlpha)));
}

//bool isSharing () const
static AbstractQoreNode *QGLWIDGET_isSharing(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglw->qobj->isSharing());
}

//bool isValid () const
static AbstractQoreNode *QGLWIDGET_isValid(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglw->qobj->isValid());
}

//void makeCurrent ()
static AbstractQoreNode *QGLWIDGET_makeCurrent(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   if (qglw->qobj)
      qglw->qobj->makeCurrent();
   return 0;
}

//void makeOverlayCurrent ()
static AbstractQoreNode *QGLWIDGET_makeOverlayCurrent(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   if (qglw->qobj)
      qglw->qobj->makeOverlayCurrent();
   return 0;
}

//const QGLContext * overlayContext () const
static AbstractQoreNode *QGLWIDGET_overlayContext(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QGLContext, new QoreQtQGLContext(const_cast<QGLContext *>(qglw->qobj->overlayContext())));
}

//void qglClearColor ( const QColor & c ) const
static AbstractQoreNode *QGLWIDGET_qglClearColor(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQColor *c = (p && p->getType() == NT_OBJECT) ? (QoreQColor *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
   if (!c) {
      if (!xsink->isException())
         xsink->raiseException("QGLWIDGET-QGLCLEARCOLOR-PARAM-ERROR", "expecting a QColor object as first argument to QGLWidget::qglClearColor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> cHolder(static_cast<AbstractPrivateData *>(c), xsink);
   qglw->qobj->qglClearColor(*(static_cast<QColor *>(c)));
   return 0;
}

//void qglColor ( const QColor & c ) const
static AbstractQoreNode *QGLWIDGET_qglColor(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQColor *c = (p && p->getType() == NT_OBJECT) ? (QoreQColor *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
   if (!c) {
      if (!xsink->isException())
         xsink->raiseException("QGLWIDGET-QGLCOLOR-PARAM-ERROR", "expecting a QColor object as first argument to QGLWidget::qglColor()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> cHolder(static_cast<AbstractPrivateData *>(c), xsink);
   qglw->qobj->qglColor(*(static_cast<QColor *>(c)));
   return 0;
}

//QPixmap renderPixmap ( int w = 0, int h = 0, bool useContext = false )
static AbstractQoreNode *QGLWIDGET_renderPixmap(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int w = !is_nothing(p) ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int h = !is_nothing(p) ? p->getAsInt() : 0;
   p = get_param(params, 2);
   bool useContext = p ? p->getAsBool() : false;
   return return_object(QC_QPixmap, new QoreQPixmap(qglw->qobj->renderPixmap(w, h, useContext)));
}

//void renderText ( int x, int y, const QString & str, const QFont & font = QFont(), int listBase = 2000 )
//void renderText ( double x, double y, double z, const QString & str, const QFont & font = QFont(), int listBase = 2000 )
static AbstractQoreNode *QGLWIDGET_renderText(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_INT) {
      int x = p ? p->getAsInt() : 0;
      p = get_param(params, 1);
      int y = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      QString str;
      if (get_qstring(p, str, xsink))
         return 0;
      p = get_param(params, 3);
      QoreQFont *font = (p && p->getType() == NT_OBJECT) ? (QoreQFont *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QFONT, xsink) : 0;
      if (*xsink)
         return 0;
      ReferenceHolder<AbstractPrivateData> fontHolder(static_cast<AbstractPrivateData *>(font), xsink);
      p = get_param(params, 4);
      int listBase = !is_nothing(p) ? p->getAsInt() : 2000;
      qglw->qobj->renderText(x, y, str, font ? *(static_cast<QFont *>(font)) : QFont(), listBase);
      return 0;
   }
   double x = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 1);
   double y = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 2);
   double z = p ? p->getAsFloat() : 0.0;
   p = get_param(params, 3);
   QString str;
   if (get_qstring(p, str, xsink))
      return 0;
   p = get_param(params, 4);
   QoreQFont *font = (p && p->getType() == NT_OBJECT) ? (QoreQFont *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> fontHolder(static_cast<AbstractPrivateData *>(font), xsink);
   p = get_param(params, 5);
   int listBase = !is_nothing(p) ? p->getAsInt() : 2000;
   qglw->qobj->renderText(x, y, z, str, font ? *(static_cast<QFont *>(font)) : QFont(), listBase);
   return 0;
}

//void setColormap ( const QGLColormap & cmap )
static AbstractQoreNode *QGLWIDGET_setColormap(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGLColormap *cmap = (p && p->getType() == NT_OBJECT) ? (QoreQGLColormap *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGLCOLORMAP, xsink) : 0;
   if (!cmap) {
      if (!xsink->isException())
         xsink->raiseException("QGLWIDGET-SETCOLORMAP-PARAM-ERROR", "expecting a QGLColormap object as first argument to QGLWidget::setColormap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> cmapHolder(static_cast<AbstractPrivateData *>(cmap), xsink);
   qglw->qobj->setColormap(*(static_cast<QGLColormap *>(cmap)));
   return 0;
}

//void setMouseTracking ( bool enable )
static AbstractQoreNode *QGLWIDGET_setMouseTracking(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qglw->qobj->setMouseTracking(enable);
   return 0;
}

//void swapBuffers ()
static AbstractQoreNode *QGLWIDGET_swapBuffers(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   if (qglw->qobj)
      qglw->qobj->swapBuffers();
   return 0;
}

//virtual void updateGL ()
static AbstractQoreNode *QGLWIDGET_updateGL(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   if (qglw->qobj)
      qglw->qobj->updateGL();
   return 0;
}

//virtual void updateOverlayGL ()
static AbstractQoreNode *QGLWIDGET_updateOverlayGL(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   if (qglw->qobj)
      qglw->qobj->updateOverlayGL();
   return 0;
}

//bool autoBufferSwap () const
static AbstractQoreNode *QGLWIDGET_autoBufferSwap(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglw->autoBufferSwap());
}

//virtual void glDraw ()
static AbstractQoreNode *QGLWIDGET_glDraw(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   if (qglw->qobj)
      qglw->glDraw();
   return 0;
}

//virtual void glInit ()
static AbstractQoreNode *QGLWIDGET_glInit(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   if (qglw->qobj)
      qglw->glInit();
   return 0;
}

//virtual void initializeGL ()
static AbstractQoreNode *QGLWIDGET_initializeGL(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   if (qglw->qobj)
      qglw->initializeGL();
   return 0;
}

//virtual void initializeOverlayGL ()
static AbstractQoreNode *QGLWIDGET_initializeOverlayGL(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   if (qglw->qobj)
      qglw->initializeOverlayGL();
   return 0;
}

//virtual void paintGL ()
static AbstractQoreNode *QGLWIDGET_paintGL(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   if (qglw->qobj)
      qglw->paintGL();
   return 0;
}

//virtual void paintOverlayGL ()
static AbstractQoreNode *QGLWIDGET_paintOverlayGL(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   if (qglw->qobj)
      qglw->paintOverlayGL();
   return 0;
}

//virtual void resizeGL ( int width, int height )
static AbstractQoreNode *QGLWIDGET_resizeGL(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int height = p ? p->getAsInt() : 0;
   qglw->resizeGL(width, height);
   return 0;
}

//virtual void resizeOverlayGL ( int width, int height )
static AbstractQoreNode *QGLWIDGET_resizeOverlayGL(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int height = p ? p->getAsInt() : 0;
   qglw->resizeOverlayGL(width, height);
   return 0;
}

//void setAutoBufferSwap ( bool on )
static AbstractQoreNode *QGLWIDGET_setAutoBufferSwap(QoreObject *self, QoreQGLWidget *qglw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool on = p ? p->getAsBool() : false;
   qglw->setAutoBufferSwap(on);
   return 0;
}

QoreClass *initQGLWidgetClass(QoreClass *qwidget)
{
   QC_QGLWidget = new QoreClass("QGLWidget", QDOM_GUI);
   CID_QGLWIDGET = QC_QGLWidget->getID();

   QC_QGLWidget->addBuiltinVirtualBaseClass(qwidget);

   QC_QGLWidget->setConstructor(QGLWIDGET_constructor);
   QC_QGLWidget->setCopy((q_copy_t)QGLWIDGET_copy);

   QC_QGLWidget->addMethod("bindTexture",                 (q_method_t)QGLWIDGET_bindTexture);
   QC_QGLWidget->addMethod("colormap",                    (q_method_t)QGLWIDGET_colormap);
   QC_QGLWidget->addMethod("context",                     (q_method_t)QGLWIDGET_context);
   QC_QGLWidget->addMethod("deleteTexture",               (q_method_t)QGLWIDGET_deleteTexture);
   QC_QGLWidget->addMethod("doneCurrent",                 (q_method_t)QGLWIDGET_doneCurrent);
   QC_QGLWidget->addMethod("doubleBuffer",                (q_method_t)QGLWIDGET_doubleBuffer);
   QC_QGLWidget->addMethod("format",                      (q_method_t)QGLWIDGET_format);
   QC_QGLWidget->addMethod("grabFrameBuffer",             (q_method_t)QGLWIDGET_grabFrameBuffer);
   QC_QGLWidget->addMethod("isSharing",                   (q_method_t)QGLWIDGET_isSharing);
   QC_QGLWidget->addMethod("isValid",                     (q_method_t)QGLWIDGET_isValid);
   QC_QGLWidget->addMethod("makeCurrent",                 (q_method_t)QGLWIDGET_makeCurrent);
   QC_QGLWidget->addMethod("makeOverlayCurrent",          (q_method_t)QGLWIDGET_makeOverlayCurrent);
   QC_QGLWidget->addMethod("overlayContext",              (q_method_t)QGLWIDGET_overlayContext);
   QC_QGLWidget->addMethod("qglClearColor",               (q_method_t)QGLWIDGET_qglClearColor);
   QC_QGLWidget->addMethod("qglColor",                    (q_method_t)QGLWIDGET_qglColor);
   QC_QGLWidget->addMethod("renderPixmap",                (q_method_t)QGLWIDGET_renderPixmap);
   QC_QGLWidget->addMethod("renderText",                  (q_method_t)QGLWIDGET_renderText);
   QC_QGLWidget->addMethod("setColormap",                 (q_method_t)QGLWIDGET_setColormap);
   QC_QGLWidget->addMethod("setMouseTracking",            (q_method_t)QGLWIDGET_setMouseTracking);
   QC_QGLWidget->addMethod("swapBuffers",                 (q_method_t)QGLWIDGET_swapBuffers);
   QC_QGLWidget->addMethod("updateGL",                    (q_method_t)QGLWIDGET_updateGL);
   QC_QGLWidget->addMethod("updateOverlayGL",             (q_method_t)QGLWIDGET_updateOverlayGL);

   // private functions
   QC_QGLWidget->addMethod("autoBufferSwap",              (q_method_t)QGLWIDGET_autoBufferSwap, true);
   QC_QGLWidget->addMethod("glDraw",                      (q_method_t)QGLWIDGET_glDraw, true);
   QC_QGLWidget->addMethod("glInit",                      (q_method_t)QGLWIDGET_glInit, true);
   QC_QGLWidget->addMethod("initializeGL",                (q_method_t)QGLWIDGET_initializeGL, true);
   QC_QGLWidget->addMethod("initializeOverlayGL",         (q_method_t)QGLWIDGET_initializeOverlayGL, true);
   QC_QGLWidget->addMethod("paintGL",                     (q_method_t)QGLWIDGET_paintGL, true);
   QC_QGLWidget->addMethod("paintOverlayGL",              (q_method_t)QGLWIDGET_paintOverlayGL, true);
   QC_QGLWidget->addMethod("resizeGL",                    (q_method_t)QGLWIDGET_resizeGL, true);
   QC_QGLWidget->addMethod("resizeOverlayGL",             (q_method_t)QGLWIDGET_resizeOverlayGL, true);
   QC_QGLWidget->addMethod("setAutoBufferSwap",           (q_method_t)QGLWIDGET_setAutoBufferSwap, true);

   return QC_QGLWidget;
}
