/*
 QC_QGLPixelBuffer.cc
 
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

#include "QC_QGLPixelBuffer.h"

qore_classid_t CID_QGLPIXELBUFFER;
QoreClass *QC_QGLPixelBuffer = 0;

//QGLPixelBuffer ( const QSize & size, const QGLFormat & format = QGLFormat::defaultFormat(), QGLWidget * shareWidget = 0 )
//QGLPixelBuffer ( int width, int height, const QGLFormat & format = QGLFormat::defaultFormat(), QGLWidget * shareWidget = 0 )
static void QGLPIXELBUFFER_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQSize *size = (QoreQSize *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!size) {
         if (!xsink->isException())
            xsink->raiseException("QGLPIXELBUFFER-CONSTRUCTOR-PARAM-ERROR", "QGLPixelBuffer::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
         return;
      }
      ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
      p = get_param(params, 1);
      QoreQGLFormat *format = (p && p->getType() == NT_OBJECT) ? (QoreQGLFormat *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGLFORMAT, xsink) : 0;
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> formatHolder(static_cast<AbstractPrivateData *>(format), xsink);
      p = get_param(params, 2);
      QoreQGLWidget *shareWidget = (p && p->getType() == NT_OBJECT) ? (QoreQGLWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGLWIDGET, xsink) : 0;
      if (*xsink)
	 return;
      ReferenceHolder<AbstractPrivateData> shareWidgetHolder(static_cast<AbstractPrivateData *>(shareWidget), xsink);
      self->setPrivate(CID_QGLPIXELBUFFER, new QoreQGLPixelBuffer(*(static_cast<QSize *>(size)), *(static_cast<QGLFormat *>(format)), shareWidget ? static_cast<QGLWidget *>(shareWidget->qobj) : 0));
      return;
   }
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQGLFormat *format = (p && p->getType() == NT_OBJECT) ? (QoreQGLFormat *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGLFORMAT, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> formatHolder(static_cast<AbstractPrivateData *>(format), xsink);
   p = get_param(params, 3);
   QoreQGLWidget *shareWidget = (p && p->getType() == NT_OBJECT) ? (QoreQGLWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGLWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> shareWidgetHolder(static_cast<AbstractPrivateData *>(shareWidget), xsink);
   self->setPrivate(CID_QGLPIXELBUFFER, new QoreQGLPixelBuffer(width, height, *(static_cast<QGLFormat *>(format)), shareWidget ? static_cast<QGLWidget *>(shareWidget->qobj) : 0));
   return;
}

static void QGLPIXELBUFFER_copy(QoreObject *self, QoreObject *old, QoreQGLPixelBuffer *qglpb, ExceptionSink *xsink)
{
   xsink->raiseException("QGLPIXELBUFFER-COPY-ERROR", "objects of this class cannot be copied");
}

//GLuint bindTexture ( const QImage & image, GLenum target = GL_TEXTURE_2D )
//GLuint bindTexture ( const QPixmap & pixmap, GLenum target = GL_TEXTURE_2D )
//GLuint bindTexture ( const QString & fileName )
static AbstractQoreNode *QGLPIXELBUFFER_bindTexture(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPixmap *pixmap = (QoreQPixmap *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPIXMAP, xsink);
      if (!pixmap) {
         QoreQImage *image = (QoreQImage *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QIMAGE, xsink);
         if (!image) {
            if (!xsink->isException())
               xsink->raiseException("QGLPIXELBUFFER-BINDTEXTURE-PARAM-ERROR", "QGLPixelBuffer::bindTexture() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> imageHolder(static_cast<AbstractPrivateData *>(image), xsink);
         p = get_param(params, 1);
         GLenum target = !is_nothing(p) ? (GLenum)p->getAsInt() : GL_TEXTURE_2D;
         return new QoreBigIntNode(qglpb->bindTexture(*(static_cast<QImage *>(image)), target));
      }
      ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
      p = get_param(params, 1);
      GLenum target = !is_nothing(p) ? (GLenum)p->getAsInt() : GL_TEXTURE_2D;
      return new QoreBigIntNode(qglpb->bindTexture(*(static_cast<QPixmap *>(pixmap)), target));
   }
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   return new QoreBigIntNode(qglpb->bindTexture(fileName));
}

//bool bindToDynamicTexture ( GLuint texture_id )
static AbstractQoreNode *QGLPIXELBUFFER_bindToDynamicTexture(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned texture_id = p ? p->getAsBigInt() : 0;
   return get_bool_node(qglpb->bindToDynamicTexture(texture_id));
}

//void deleteTexture ( GLuint texture_id )
static AbstractQoreNode *QGLPIXELBUFFER_deleteTexture(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned texture_id = p ? p->getAsBigInt() : 0;
   qglpb->deleteTexture(texture_id);
   return 0;
}

//bool doneCurrent ()
static AbstractQoreNode *QGLPIXELBUFFER_doneCurrent(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglpb->doneCurrent());
}

//QGLFormat format () const
static AbstractQoreNode *QGLPIXELBUFFER_format(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QGLFormat, new QoreQGLFormat(qglpb->format()));
}

//GLuint generateDynamicTexture () const
static AbstractQoreNode *QGLPIXELBUFFER_generateDynamicTexture(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglpb->generateDynamicTexture());
}

/*
//Qt::HANDLE handle () const
static AbstractQoreNode *QGLPIXELBUFFER_handle(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglpb->handle());
}
*/

//bool isValid () const
static AbstractQoreNode *QGLPIXELBUFFER_isValid(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglpb->isValid());
}

//bool makeCurrent ()
static AbstractQoreNode *QGLPIXELBUFFER_makeCurrent(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglpb->makeCurrent());
}

//void releaseFromDynamicTexture ()
static AbstractQoreNode *QGLPIXELBUFFER_releaseFromDynamicTexture(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   qglpb->releaseFromDynamicTexture();
   return 0;
}

//QSize size () const
static AbstractQoreNode *QGLPIXELBUFFER_size(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QSize, new QoreQSize(qglpb->size()));
}

//QImage toImage () const
static AbstractQoreNode *QGLPIXELBUFFER_toImage(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QImage, new QoreQImage(qglpb->toImage()));
}

//void updateDynamicTexture ( GLuint texture_id ) const
static AbstractQoreNode *QGLPIXELBUFFER_updateDynamicTexture(QoreObject *self, QoreQGLPixelBuffer *qglpb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned texture_id = p ? p->getAsBigInt() : 0;
   qglpb->updateDynamicTexture(texture_id);
   return 0;
}

QoreClass *initQGLPixelBufferClass(QoreClass *qpaintdevice)
{
   QC_QGLPixelBuffer = new QoreClass("QGLPixelBuffer", QDOM_GUI);
   CID_QGLPIXELBUFFER = QC_QGLPixelBuffer->getID();

   QC_QGLPixelBuffer->setConstructor(QGLPIXELBUFFER_constructor);
   QC_QGLPixelBuffer->setCopy((q_copy_t)QGLPIXELBUFFER_copy);

   QC_QGLPixelBuffer->addBuiltinVirtualBaseClass(qpaintdevice);

   QC_QGLPixelBuffer->addMethod("bindTexture",                 (q_method_t)QGLPIXELBUFFER_bindTexture);
   QC_QGLPixelBuffer->addMethod("bindToDynamicTexture",        (q_method_t)QGLPIXELBUFFER_bindToDynamicTexture);
   QC_QGLPixelBuffer->addMethod("deleteTexture",               (q_method_t)QGLPIXELBUFFER_deleteTexture);
   QC_QGLPixelBuffer->addMethod("doneCurrent",                 (q_method_t)QGLPIXELBUFFER_doneCurrent);
   QC_QGLPixelBuffer->addMethod("format",                      (q_method_t)QGLPIXELBUFFER_format);
   QC_QGLPixelBuffer->addMethod("generateDynamicTexture",      (q_method_t)QGLPIXELBUFFER_generateDynamicTexture);
   //QC_QGLPixelBuffer->addMethod("handle",                      (q_method_t)QGLPIXELBUFFER_handle);
   QC_QGLPixelBuffer->addMethod("isValid",                     (q_method_t)QGLPIXELBUFFER_isValid);
   QC_QGLPixelBuffer->addMethod("makeCurrent",                 (q_method_t)QGLPIXELBUFFER_makeCurrent);
   QC_QGLPixelBuffer->addMethod("releaseFromDynamicTexture",   (q_method_t)QGLPIXELBUFFER_releaseFromDynamicTexture);
   QC_QGLPixelBuffer->addMethod("size",                        (q_method_t)QGLPIXELBUFFER_size);
   QC_QGLPixelBuffer->addMethod("toImage",                     (q_method_t)QGLPIXELBUFFER_toImage);
   QC_QGLPixelBuffer->addMethod("updateDynamicTexture",        (q_method_t)QGLPIXELBUFFER_updateDynamicTexture);

   return QC_QGLPixelBuffer;
}

//bool hasOpenGLPbuffers ()
static AbstractQoreNode *f_QGLPixelBuffer_hasOpenGLPbuffers(const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(QGLPixelBuffer::hasOpenGLPbuffers());
}

void initQGLPixelBufferStaticFunctions()
{
   builtinFunctions.add("QGLPixelBuffer_hasOpenGLPbuffers",            f_QGLPixelBuffer_hasOpenGLPbuffers);
}
