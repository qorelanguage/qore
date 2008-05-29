/*
 QC_QGLFramebufferObject.cc
 
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

#include "qore-qt.h"

#include "QC_QGLFramebufferObject.h"
#include "QC_QSize.h"
#include "QC_QImage.h"

int CID_QGLFRAMEBUFFEROBJECT;
QoreClass *QC_QGLFramebufferObject = 0;

//QGLFramebufferObject ( const QSize & size, GLenum target = GL_TEXTURE_2D )
//QGLFramebufferObject ( const QSize & size, Attachment attachment, GLenum target = GL_TEXTURE_2D, GLenum internal_format = GL_RGBA8 )
//QGLFramebufferObject ( int width, int height, GLenum target = GL_TEXTURE_2D )
//QGLFramebufferObject ( int width, int height, Attachment attachment, GLenum target = GL_TEXTURE_2D, GLenum internal_format = GL_RGBA8 )
static void QGLFRAMEBUFFEROBJECT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQSize *size = (QoreQSize *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QSIZE, xsink);
      if (!size) {
	 if (!xsink->isException())
	    xsink->raiseException("QGLFRAMEBUFFEROBJECT-CONSTRUCTOR-PARAM-ERROR", "QGLFramebufferObject::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
	 return;
      }
      ReferenceHolder<AbstractPrivateData> sizeHolder(static_cast<AbstractPrivateData *>(size), xsink);
      p = get_param(params, 1);
      if (num_params(params) == 2) {
         GLenum target = !is_nothing(p) ? (GLenum)p->getAsInt() : GL_TEXTURE_2D;
         self->setPrivate(CID_QGLFRAMEBUFFEROBJECT, new QoreQGLFramebufferObject(*(static_cast<QSize *>(size)), target));
         return;
      }
      QGLFramebufferObject::Attachment attachment = (QGLFramebufferObject::Attachment)(p ? p->getAsInt() : 0);
      p = get_param(params, 2);
      GLenum target = !is_nothing(p) ? (GLenum)p->getAsInt() : GL_TEXTURE_2D;
      p = get_param(params, 3);
      GLenum internal_format = !is_nothing(p) ? (GLenum)p->getAsInt() : GL_RGBA8;
      self->setPrivate(CID_QGLFRAMEBUFFEROBJECT, new QoreQGLFramebufferObject(*(static_cast<QSize *>(size)), attachment, target, internal_format));
      return;
   }
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int height = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   if (num_params(params) == 3) {
      GLenum target = !is_nothing(p) ? (GLenum)p->getAsInt() : GL_TEXTURE_2D;
      self->setPrivate(CID_QGLFRAMEBUFFEROBJECT, new QoreQGLFramebufferObject(width, height, target));
      return;
   }

   QGLFramebufferObject::Attachment attachment = (QGLFramebufferObject::Attachment)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum target = !is_nothing(p) ? (GLenum)p->getAsInt() : GL_TEXTURE_2D;
   p = get_param(params, 4);
   GLenum internal_format = !is_nothing(p) ? (GLenum)p->getAsInt() : GL_RGBA8;
   self->setPrivate(CID_QGLFRAMEBUFFEROBJECT, new QoreQGLFramebufferObject(width, height, attachment, target, internal_format));
   return;
}

static void QGLFRAMEBUFFEROBJECT_copy(QoreObject *self, QoreObject *old, QoreQGLFramebufferObject *qglfo, ExceptionSink *xsink)
{
   xsink->raiseException("QGLFRAMEBUFFEROBJECT-COPY-ERROR", "objects of this class cannot be copied");
}

//Attachment attachment () const
static AbstractQoreNode *QGLFRAMEBUFFEROBJECT_attachment(QoreObject *self, QoreQGLFramebufferObject *qglfo, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglfo->attachment());
}

//bool bind ()
static AbstractQoreNode *QGLFRAMEBUFFEROBJECT_bind(QoreObject *self, QoreQGLFramebufferObject *qglfo, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglfo->bind());
}

//GLuint handle () const
static AbstractQoreNode *QGLFRAMEBUFFEROBJECT_handle(QoreObject *self, QoreQGLFramebufferObject *qglfo, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglfo->handle());
}

//bool isValid () const
static AbstractQoreNode *QGLFRAMEBUFFEROBJECT_isValid(QoreObject *self, QoreQGLFramebufferObject *qglfo, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglfo->isValid());
}

//bool release ()
static AbstractQoreNode *QGLFRAMEBUFFEROBJECT_release(QoreObject *self, QoreQGLFramebufferObject *qglfo, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglfo->release());
}

//QSize size () const
static AbstractQoreNode *QGLFRAMEBUFFEROBJECT_size(QoreObject *self, QoreQGLFramebufferObject *qglfo, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QSize, new QoreQSize(qglfo->size()));
}

//GLuint texture () const
static AbstractQoreNode *QGLFRAMEBUFFEROBJECT_texture(QoreObject *self, QoreQGLFramebufferObject *qglfo, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglfo->texture());
}

//QImage toImage () const
static AbstractQoreNode *QGLFRAMEBUFFEROBJECT_toImage(QoreObject *self, QoreQGLFramebufferObject *qglfo, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QImage, new QoreQImage(qglfo->toImage()));
}

static QoreClass *initQGLFramebufferObjectClass(QoreClass *qpaintdevice)
{
   QC_QGLFramebufferObject = new QoreClass("QGLFramebufferObject", QDOM_GUI);
   CID_QGLFRAMEBUFFEROBJECT = QC_QGLFramebufferObject->getID();

   QC_QGLFramebufferObject->addBuiltinVirtualBaseClass(qpaintdevice);

   QC_QGLFramebufferObject->setConstructor(QGLFRAMEBUFFEROBJECT_constructor);
   QC_QGLFramebufferObject->setCopy((q_copy_t)QGLFRAMEBUFFEROBJECT_copy);

   QC_QGLFramebufferObject->addMethod("attachment",                  (q_method_t)QGLFRAMEBUFFEROBJECT_attachment);
   QC_QGLFramebufferObject->addMethod("bind",                        (q_method_t)QGLFRAMEBUFFEROBJECT_bind);
   QC_QGLFramebufferObject->addMethod("handle",                      (q_method_t)QGLFRAMEBUFFEROBJECT_handle);
   QC_QGLFramebufferObject->addMethod("isValid",                     (q_method_t)QGLFRAMEBUFFEROBJECT_isValid);
   QC_QGLFramebufferObject->addMethod("release",                     (q_method_t)QGLFRAMEBUFFEROBJECT_release);
   QC_QGLFramebufferObject->addMethod("size",                        (q_method_t)QGLFRAMEBUFFEROBJECT_size);
   QC_QGLFramebufferObject->addMethod("texture",                     (q_method_t)QGLFRAMEBUFFEROBJECT_texture);
   QC_QGLFramebufferObject->addMethod("toImage",                     (q_method_t)QGLFRAMEBUFFEROBJECT_toImage);

   return QC_QGLFramebufferObject;
}

QoreNamespace *initQGLFramebufferObjectNS(QoreClass *qpaintdevice)
{
   QoreNamespace *ns = new QoreNamespace("QGLFramebufferObject");
   ns->addSystemClass(initQGLFramebufferObjectClass(qpaintdevice));

   // Attachment enum
   ns->addConstant("NoAttachment",             new QoreBigIntNode(QGLFramebufferObject::NoAttachment));
   ns->addConstant("CombinedDepthStencil",     new QoreBigIntNode(QGLFramebufferObject::CombinedDepthStencil));
   ns->addConstant("Depth",                    new QoreBigIntNode(QGLFramebufferObject::Depth));

   return ns;
}

//bool hasOpenGLFramebufferObjects ()
static AbstractQoreNode *f_QGLFramebufferObject_hasOpenGLFramebufferObjects(const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(QGLFramebufferObject::hasOpenGLFramebufferObjects());
}

void initQGLFramebufferObjectStaticFunctions()
{
   builtinFunctions.add("QGLFramebufferObject_hasOpenGLFramebufferObjects",  f_QGLFramebufferObject_hasOpenGLFramebufferObjects, QDOM_GUI);
}
