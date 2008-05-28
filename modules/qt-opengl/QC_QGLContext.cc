/*
 QC_QGLContext.cc
 
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

#include "../qt/qore-qt.h"

#include "QC_QGLContext.h"
#include "QC_QGLFormat.h"
#include "../qt/QC_QPixmap.h"
#include "../qt/QC_QImage.h"
#include "../qt/QC_QColor.h"
#include "../qt/QC_QPaintDevice.h"

int CID_QGLCONTEXT;
QoreClass *QC_QGLContext = 0;

//QGLContext ( const QGLFormat & format )
static void QGLCONTEXT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGLFormat *format = (p && p->getType() == NT_OBJECT) ? (QoreQGLFormat *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGLFORMAT, xsink) : 0;
   if (!format) {
      if (!xsink->isException())
         xsink->raiseException("QGLCONTEXT-CONSTRUCTOR-PARAM-ERROR", "expecting a QGLFormat object as first argument to QGLContext::constructor()");
      return;
   }
   ReferenceHolder<AbstractPrivateData> formatHolder(static_cast<AbstractPrivateData *>(format), xsink);
   self->setPrivate(CID_QGLCONTEXT, new QoreQGLContext(*(static_cast<QGLFormat *>(format))));
   return;
}

static void QGLCONTEXT_copy(QoreObject *self, QoreObject *old, QoreQGLContext *qglc, ExceptionSink *xsink)
{
   xsink->raiseException("QGLCONTEXT-COPY-ERROR", "objects of this class cannot be copied");
}

//GLuint bindTexture ( const QImage & image, GLenum target = GL_TEXTURE_2D, GLint format = GL_RGBA )
//GLuint bindTexture ( const QString & fileName )
//GLuint bindTexture ( const QPixmap & pixmap, GLenum target = GL_TEXTURE_2D, GLint format = GL_RGBA )
static AbstractQoreNode *QGLCONTEXT_bindTexture(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPixmap *pixmap = (QoreQPixmap *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPIXMAP, xsink);
      if (!pixmap) {
         QoreQImage *image = (QoreQImage *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QIMAGE, xsink);
         if (!image) {
            if (!xsink->isException())
               xsink->raiseException("QGLCONTEXT-BINDTEXTURE-PARAM-ERROR", "QGLContext::bindTexture() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> imageHolder(static_cast<AbstractPrivateData *>(image), xsink);
         p = get_param(params, 1);
         GLenum target = !is_nothing(p) ? (GLenum)p->getAsInt() : GL_TEXTURE_2D;
         p = get_param(params, 2);
         GLint format = !is_nothing(p) ? (GLint)p->getAsInt() : GL_RGBA;
         return new QoreBigIntNode(qglc->getQGLContext()->bindTexture(*(static_cast<QImage *>(image)), target, format));
      }
      ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
      p = get_param(params, 1);
      GLenum target = !is_nothing(p) ? (GLenum)p->getAsInt() : GL_TEXTURE_2D;
      p = get_param(params, 2);
      GLint format = !is_nothing(p) ? (GLint)p->getAsInt() : GL_RGBA;
      return new QoreBigIntNode(qglc->getQGLContext()->bindTexture(*(static_cast<QPixmap *>(pixmap)), target, format));
   }
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   return new QoreBigIntNode(qglc->getQGLContext()->bindTexture(fileName));
}

//virtual bool create ( const QGLContext * shareContext = 0 )
static AbstractQoreNode *QGLCONTEXT_create(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGLContext *shareContext = (p && p->getType() == NT_OBJECT) ? (QoreQGLContext *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGLCONTEXT, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> shareContextHolder(static_cast<AbstractPrivateData *>(shareContext), xsink);
   return get_bool_node(qglc->getQGLContext()->create(shareContext ? static_cast<QGLContext *>(shareContext) : 0));
}

//void deleteTexture ( GLuint id )
static AbstractQoreNode *QGLCONTEXT_deleteTexture(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLuint id = (GLuint)(p ? p->getAsBigInt() : 0);
   qglc->getQGLContext()->deleteTexture(id);
   return 0;
}

//QPaintDevice * device () const
static AbstractQoreNode *QGLCONTEXT_device(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   QPaintDevice *qpd_rv = qglc->getQGLContext()->device();
   if (!qpd_rv)
      return 0;
   return return_object(QC_QPaintDevice, new QoreQtQPaintDevice(qpd_rv));
}

//virtual void doneCurrent ()
static AbstractQoreNode *QGLCONTEXT_doneCurrent(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   qglc->getQGLContext()->doneCurrent();
   return 0;
}

//QGLFormat format () const
static AbstractQoreNode *QGLCONTEXT_format(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QGLFormat, new QoreQGLFormat(qglc->getQGLContext()->format()));
}

//void * getProcAddress ( const QString & proc ) const
static AbstractQoreNode *QGLCONTEXT_getProcAddress(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString proc;
   if (get_qstring(p, proc, xsink))
      return 0;
   qglc->getQGLContext()->getProcAddress(proc);
   return 0;
}

//bool isSharing () const
static AbstractQoreNode *QGLCONTEXT_isSharing(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglc->getQGLContext()->isSharing());
}

//bool isValid () const
static AbstractQoreNode *QGLCONTEXT_isValid(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglc->getQGLContext()->isValid());
}

//virtual void makeCurrent ()
static AbstractQoreNode *QGLCONTEXT_makeCurrent(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   qglc->getQGLContext()->makeCurrent();
   return 0;
}

//QColor overlayTransparentColor () const
static AbstractQoreNode *QGLCONTEXT_overlayTransparentColor(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QColor, new QoreQColor(qglc->getQGLContext()->overlayTransparentColor()));
}

//QGLFormat requestedFormat () const
static AbstractQoreNode *QGLCONTEXT_requestedFormat(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QGLFormat, new QoreQGLFormat(qglc->getQGLContext()->requestedFormat()));
}

//void reset ()
static AbstractQoreNode *QGLCONTEXT_reset(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   qglc->getQGLContext()->reset();
   return 0;
}

//void setFormat ( const QGLFormat & format )
static AbstractQoreNode *QGLCONTEXT_setFormat(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGLFormat *format = (p && p->getType() == NT_OBJECT) ? (QoreQGLFormat *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGLFORMAT, xsink) : 0;
   if (!format) {
      if (!xsink->isException())
         xsink->raiseException("QGLCONTEXT-SETFORMAT-PARAM-ERROR", "expecting a QGLFormat object as first argument to QGLContext::setFormat()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> formatHolder(static_cast<AbstractPrivateData *>(format), xsink);
   qglc->getQGLContext()->setFormat(*(static_cast<QGLFormat *>(format)));
   return 0;
}

//virtual void swapBuffers () const
static AbstractQoreNode *QGLCONTEXT_swapBuffers(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   qglc->getQGLContext()->swapBuffers();
   return 0;
}

//virtual bool chooseContext ( const QGLContext * shareContext = 0 )
static AbstractQoreNode *QGLCONTEXT_chooseContext(QoreObject *self, QoreAbstractQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGLContext *shareContext = (p && p->getType() == NT_OBJECT) ? (QoreQGLContext *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGLCONTEXT, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> shareContextHolder(static_cast<AbstractPrivateData *>(shareContext), xsink);
   return get_bool_node(qglc->parent_chooseContext(shareContext ? static_cast<QGLContext *>(shareContext) : 0));
}

#ifdef Q_WS_MAC
//virtual void * chooseMacVisual ( GDHandle handle )
/*
static AbstractQoreNode *QGLCONTEXT_chooseMacVisual(QoreObject *self, QoreQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GDHandle handle = (GDHandle)(p ? p->getAsInt() : 0);
   qglc->parent_chooseMacVisual(handle);
   return 0;
}
*/
#endif

/*
#ifdef Q_WS_WIN
//virtual int choosePixelFormat ( void * dummyPfd, HDC pdc )
static AbstractQoreNode *QGLCONTEXT_choosePixelFormat(QoreObject *self, QoreQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? void* dummyPfd = p;
   p = get_param(params, 1);
   HDC pdc = (HDC)(p ? p->getAsInt() : 0);
   return new QoreBigIntNode(qglc->parent_choosePixelFormat(dummyPfd, pdc));
}
#endif
*/

/*
#ifdef Q_WS_X11
//virtual void * chooseVisual ()
static AbstractQoreNode *QGLCONTEXT_chooseVisual(QoreObject *self, QoreQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   qglc->parent_chooseVisual();
   return 0;
}
#endif
*/

//bool deviceIsPixmap () const
static AbstractQoreNode *QGLCONTEXT_deviceIsPixmap(QoreObject *self, QoreQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglc->parent_deviceIsPixmap());
}

//bool initialized () const
static AbstractQoreNode *QGLCONTEXT_initialized(QoreObject *self, QoreQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglc->parent_initialized());
}

//void setInitialized ( bool on )
static AbstractQoreNode *QGLCONTEXT_setInitialized(QoreObject *self, QoreQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool on = p ? p->getAsBool() : false;
   qglc->parent_setInitialized(on);
   return 0;
}

//void setWindowCreated ( bool on )
static AbstractQoreNode *QGLCONTEXT_setWindowCreated(QoreObject *self, QoreQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool on = p ? p->getAsBool() : false;
   qglc->parent_setWindowCreated(on);
   return 0;
}

//bool windowCreated () const
static AbstractQoreNode *QGLCONTEXT_windowCreated(QoreObject *self, QoreQGLContext *qglc, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglc->parent_windowCreated());
}

QoreClass *initQGLContextClass()
{
   QC_QGLContext = new QoreClass("QGLContext", QDOM_GUI);
   CID_QGLCONTEXT = QC_QGLContext->getID();

   QC_QGLContext->setConstructor(QGLCONTEXT_constructor);
   QC_QGLContext->setCopy((q_copy_t)QGLCONTEXT_copy);

   QC_QGLContext->addMethod("bindTexture",                 (q_method_t)QGLCONTEXT_bindTexture);
   QC_QGLContext->addMethod("create",                      (q_method_t)QGLCONTEXT_create);
   QC_QGLContext->addMethod("deleteTexture",               (q_method_t)QGLCONTEXT_deleteTexture);
   QC_QGLContext->addMethod("device",                      (q_method_t)QGLCONTEXT_device);
   QC_QGLContext->addMethod("doneCurrent",                 (q_method_t)QGLCONTEXT_doneCurrent);
   QC_QGLContext->addMethod("format",                      (q_method_t)QGLCONTEXT_format);
   QC_QGLContext->addMethod("getProcAddress",              (q_method_t)QGLCONTEXT_getProcAddress);
   QC_QGLContext->addMethod("isSharing",                   (q_method_t)QGLCONTEXT_isSharing);
   QC_QGLContext->addMethod("isValid",                     (q_method_t)QGLCONTEXT_isValid);
   QC_QGLContext->addMethod("makeCurrent",                 (q_method_t)QGLCONTEXT_makeCurrent);
   QC_QGLContext->addMethod("overlayTransparentColor",     (q_method_t)QGLCONTEXT_overlayTransparentColor);
   QC_QGLContext->addMethod("requestedFormat",             (q_method_t)QGLCONTEXT_requestedFormat);
   QC_QGLContext->addMethod("reset",                       (q_method_t)QGLCONTEXT_reset);
   QC_QGLContext->addMethod("setFormat",                   (q_method_t)QGLCONTEXT_setFormat);
   QC_QGLContext->addMethod("swapBuffers",                 (q_method_t)QGLCONTEXT_swapBuffers);

   // private methods
   QC_QGLContext->addMethod("chooseContext",               (q_method_t)QGLCONTEXT_chooseContext, true);
#ifdef Q_WS_MAC
   //QC_QGLContext->addMethod("chooseMacVisual",             (q_method_t)QGLCONTEXT_chooseMacVisual, true);
#endif
#ifdef Q_WS_WIN
   //QC_QGLContext->addMethod("choosePixelFormat",           (q_method_t)QGLCONTEXT_choosePixelFormat, true);
#endif
#ifdef Q_WS_X11
   //QC_QGLContext->addMethod("chooseVisual",                (q_method_t)QGLCONTEXT_chooseVisual, true);
#endif
   QC_QGLContext->addMethod("deviceIsPixmap",              (q_method_t)QGLCONTEXT_deviceIsPixmap, true);
   QC_QGLContext->addMethod("initialized",                 (q_method_t)QGLCONTEXT_initialized, true);
   QC_QGLContext->addMethod("setInitialized",              (q_method_t)QGLCONTEXT_setInitialized, true);
   QC_QGLContext->addMethod("setWindowCreated",            (q_method_t)QGLCONTEXT_setWindowCreated, true);
   QC_QGLContext->addMethod("windowCreated",               (q_method_t)QGLCONTEXT_windowCreated, true);

   return QC_QGLContext;
}
