/*
 QC_QGLFormat.cc
 
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

#include "QC_QGLFormat.h"

int CID_QGLFORMAT;
QoreClass *QC_QGLFormat = 0;

//QGLFormat ()
//QGLFormat ( QGL::FormatOptions options, int plane = 0 )
//QGLFormat ( const QGLFormat & other )
static void QGLFORMAT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QGLFORMAT, new QoreQGLFormat());
      return;
   }
   QGL::FormatOptions options = (QGL::FormatOptions)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int plane = !is_nothing(p) ? p->getAsInt() : 0;
   self->setPrivate(CID_QGLFORMAT, new QoreQGLFormat(options, plane));
   return;
}

static void QGLFORMAT_copy(QoreObject *self, QoreObject *old, QoreQGLFormat *qglf, ExceptionSink *xsink)
{
   xsink->raiseException("QGLFORMAT-COPY-ERROR", "objects of this class cannot be copied");
}

//bool accum () const
static AbstractQoreNode *QGLFORMAT_accum(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglf->accum());
}

//int accumBufferSize () const
static AbstractQoreNode *QGLFORMAT_accumBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglf->accumBufferSize());
}

//bool alpha () const
static AbstractQoreNode *QGLFORMAT_alpha(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglf->alpha());
}

//int alphaBufferSize () const
static AbstractQoreNode *QGLFORMAT_alphaBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglf->alphaBufferSize());
}

//int blueBufferSize () const
static AbstractQoreNode *QGLFORMAT_blueBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglf->blueBufferSize());
}

//bool depth () const
static AbstractQoreNode *QGLFORMAT_depth(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglf->depth());
}

//int depthBufferSize () const
static AbstractQoreNode *QGLFORMAT_depthBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglf->depthBufferSize());
}

//bool directRendering () const
static AbstractQoreNode *QGLFORMAT_directRendering(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglf->directRendering());
}

//bool doubleBuffer () const
static AbstractQoreNode *QGLFORMAT_doubleBuffer(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglf->doubleBuffer());
}

//int greenBufferSize () const
static AbstractQoreNode *QGLFORMAT_greenBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglf->greenBufferSize());
}

//bool hasOverlay () const
static AbstractQoreNode *QGLFORMAT_hasOverlay(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglf->hasOverlay());
}

//int plane () const
static AbstractQoreNode *QGLFORMAT_plane(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglf->plane());
}

//int redBufferSize () const
static AbstractQoreNode *QGLFORMAT_redBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglf->redBufferSize());
}

//bool rgba () const
static AbstractQoreNode *QGLFORMAT_rgba(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglf->rgba());
}

//bool sampleBuffers () const
static AbstractQoreNode *QGLFORMAT_sampleBuffers(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglf->sampleBuffers());
}

//int samples () const
static AbstractQoreNode *QGLFORMAT_samples(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglf->samples());
}

//void setAccum ( bool enable )
static AbstractQoreNode *QGLFORMAT_setAccum(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qglf->setAccum(enable);
   return 0;
}

//void setAccumBufferSize ( int size )
static AbstractQoreNode *QGLFORMAT_setAccumBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   qglf->setAccumBufferSize(size);
   return 0;
}

//void setAlpha ( bool enable )
static AbstractQoreNode *QGLFORMAT_setAlpha(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qglf->setAlpha(enable);
   return 0;
}

//void setAlphaBufferSize ( int size )
static AbstractQoreNode *QGLFORMAT_setAlphaBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   qglf->setAlphaBufferSize(size);
   return 0;
}

//void setBlueBufferSize ( int size )
static AbstractQoreNode *QGLFORMAT_setBlueBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   qglf->setBlueBufferSize(size);
   return 0;
}

//void setDepth ( bool enable )
static AbstractQoreNode *QGLFORMAT_setDepth(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qglf->setDepth(enable);
   return 0;
}

//void setDepthBufferSize ( int size )
static AbstractQoreNode *QGLFORMAT_setDepthBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   qglf->setDepthBufferSize(size);
   return 0;
}

//void setDirectRendering ( bool enable )
static AbstractQoreNode *QGLFORMAT_setDirectRendering(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qglf->setDirectRendering(enable);
   return 0;
}

//void setDoubleBuffer ( bool enable )
static AbstractQoreNode *QGLFORMAT_setDoubleBuffer(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qglf->setDoubleBuffer(enable);
   return 0;
}

//void setGreenBufferSize ( int size )
static AbstractQoreNode *QGLFORMAT_setGreenBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   qglf->setGreenBufferSize(size);
   return 0;
}

//void setOption ( QGL::FormatOptions opt )
static AbstractQoreNode *QGLFORMAT_setOption(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGL::FormatOptions opt = (QGL::FormatOptions)(p ? p->getAsInt() : 0);
   qglf->setOption(opt);
   return 0;
}

//void setOverlay ( bool enable )
static AbstractQoreNode *QGLFORMAT_setOverlay(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qglf->setOverlay(enable);
   return 0;
}

//void setPlane ( int plane )
static AbstractQoreNode *QGLFORMAT_setPlane(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int plane = p ? p->getAsInt() : 0;
   qglf->setPlane(plane);
   return 0;
}

//void setRedBufferSize ( int size )
static AbstractQoreNode *QGLFORMAT_setRedBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   qglf->setRedBufferSize(size);
   return 0;
}

//void setRgba ( bool enable )
static AbstractQoreNode *QGLFORMAT_setRgba(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qglf->setRgba(enable);
   return 0;
}

//void setSampleBuffers ( bool enable )
static AbstractQoreNode *QGLFORMAT_setSampleBuffers(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qglf->setSampleBuffers(enable);
   return 0;
}

//void setSamples ( int numSamples )
static AbstractQoreNode *QGLFORMAT_setSamples(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int numSamples = p ? p->getAsInt() : 0;
   qglf->setSamples(numSamples);
   return 0;
}

//void setStencil ( bool enable )
static AbstractQoreNode *QGLFORMAT_setStencil(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qglf->setStencil(enable);
   return 0;
}

//void setStencilBufferSize ( int size )
static AbstractQoreNode *QGLFORMAT_setStencilBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   qglf->setStencilBufferSize(size);
   return 0;
}

//void setStereo ( bool enable )
static AbstractQoreNode *QGLFORMAT_setStereo(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qglf->setStereo(enable);
   return 0;
}

//void setSwapInterval ( int interval )
static AbstractQoreNode *QGLFORMAT_setSwapInterval(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int interval = p ? p->getAsInt() : 0;
   qglf->setSwapInterval(interval);
   return 0;
}

//bool stencil () const
static AbstractQoreNode *QGLFORMAT_stencil(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglf->stencil());
}

//int stencilBufferSize () const
static AbstractQoreNode *QGLFORMAT_stencilBufferSize(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglf->stencilBufferSize());
}

//bool stereo () const
static AbstractQoreNode *QGLFORMAT_stereo(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qglf->stereo());
}

//int swapInterval () const
static AbstractQoreNode *QGLFORMAT_swapInterval(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qglf->swapInterval());
}

//bool testOption ( QGL::FormatOptions opt ) const
static AbstractQoreNode *QGLFORMAT_testOption(QoreObject *self, QoreQGLFormat *qglf, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QGL::FormatOptions opt = (QGL::FormatOptions)(p ? p->getAsInt() : 0);
   return get_bool_node(qglf->testOption(opt));
}

//QGLFormat defaultFormat ()
static AbstractQoreNode *f_QGLFormat_defaultFormat(const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QGLFormat, new QoreQGLFormat(QGLFormat::defaultFormat()));
}

//QGLFormat defaultOverlayFormat ()
static AbstractQoreNode *f_QGLFormat_defaultOverlayFormat(const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QGLFormat, new QoreQGLFormat(QGLFormat::defaultOverlayFormat()));
}

//bool hasOpenGL ()
static AbstractQoreNode *f_QGLFormat_hasOpenGL(const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(QGLFormat::hasOpenGL());
}

//bool hasOpenGLOverlays ()
static AbstractQoreNode *f_QGLFormat_hasOpenGLOverlays(const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(QGLFormat::hasOpenGLOverlays());
}

//OpenGLVersionFlags openGLVersionFlags ()
static AbstractQoreNode *f_QGLFormat_openGLVersionFlags(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(QGLFormat::openGLVersionFlags());
}

//void setDefaultFormat ( const QGLFormat & f )
static AbstractQoreNode *f_QGLFormat_setDefaultFormat(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGLFormat *f = (p && p->getType() == NT_OBJECT) ? (QoreQGLFormat *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGLFORMAT, xsink) : 0;
   if (!f) {
      if (!xsink->isException())
         xsink->raiseException("QGLFORMAT-SETDEFAULTFORMAT-PARAM-ERROR", "expecting a QGLFormat object as first argument to QGLFormat::setDefaultFormat()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> fHolder(static_cast<AbstractPrivateData *>(f), xsink);
   QGLFormat::setDefaultFormat(*(static_cast<QGLFormat *>(f)));
   return 0;
}

//void setDefaultOverlayFormat ( const QGLFormat & f )
static AbstractQoreNode *f_QGLFormat_setDefaultOverlayFormat(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQGLFormat *f = (p && p->getType() == NT_OBJECT) ? (QoreQGLFormat *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QGLFORMAT, xsink) : 0;
   if (!f) {
      if (!xsink->isException())
         xsink->raiseException("QGLFORMAT-SETDEFAULTOVERLAYFORMAT-PARAM-ERROR", "expecting a QGLFormat object as first argument to QGLFormat::setDefaultOverlayFormat()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> fHolder(static_cast<AbstractPrivateData *>(f), xsink);
   QGLFormat::setDefaultOverlayFormat(*(static_cast<QGLFormat *>(f)));
   return 0;
}

static QoreClass *initQGLFormatClass()
{
   QC_QGLFormat = new QoreClass("QGLFormat", QDOM_GUI);
   CID_QGLFORMAT = QC_QGLFormat->getID();

   QC_QGLFormat->setConstructor(QGLFORMAT_constructor);
   QC_QGLFormat->setCopy((q_copy_t)QGLFORMAT_copy);

   QC_QGLFormat->addMethod("accum",                       (q_method_t)QGLFORMAT_accum);
   QC_QGLFormat->addMethod("accumBufferSize",             (q_method_t)QGLFORMAT_accumBufferSize);
   QC_QGLFormat->addMethod("alpha",                       (q_method_t)QGLFORMAT_alpha);
   QC_QGLFormat->addMethod("alphaBufferSize",             (q_method_t)QGLFORMAT_alphaBufferSize);
   QC_QGLFormat->addMethod("blueBufferSize",              (q_method_t)QGLFORMAT_blueBufferSize);
   QC_QGLFormat->addMethod("depth",                       (q_method_t)QGLFORMAT_depth);
   QC_QGLFormat->addMethod("depthBufferSize",             (q_method_t)QGLFORMAT_depthBufferSize);
   QC_QGLFormat->addMethod("directRendering",             (q_method_t)QGLFORMAT_directRendering);
   QC_QGLFormat->addMethod("doubleBuffer",                (q_method_t)QGLFORMAT_doubleBuffer);
   QC_QGLFormat->addMethod("greenBufferSize",             (q_method_t)QGLFORMAT_greenBufferSize);
   QC_QGLFormat->addMethod("hasOverlay",                  (q_method_t)QGLFORMAT_hasOverlay);
   QC_QGLFormat->addMethod("plane",                       (q_method_t)QGLFORMAT_plane);
   QC_QGLFormat->addMethod("redBufferSize",               (q_method_t)QGLFORMAT_redBufferSize);
   QC_QGLFormat->addMethod("rgba",                        (q_method_t)QGLFORMAT_rgba);
   QC_QGLFormat->addMethod("sampleBuffers",               (q_method_t)QGLFORMAT_sampleBuffers);
   QC_QGLFormat->addMethod("samples",                     (q_method_t)QGLFORMAT_samples);
   QC_QGLFormat->addMethod("setAccum",                    (q_method_t)QGLFORMAT_setAccum);
   QC_QGLFormat->addMethod("setAccumBufferSize",          (q_method_t)QGLFORMAT_setAccumBufferSize);
   QC_QGLFormat->addMethod("setAlpha",                    (q_method_t)QGLFORMAT_setAlpha);
   QC_QGLFormat->addMethod("setAlphaBufferSize",          (q_method_t)QGLFORMAT_setAlphaBufferSize);
   QC_QGLFormat->addMethod("setBlueBufferSize",           (q_method_t)QGLFORMAT_setBlueBufferSize);
   QC_QGLFormat->addMethod("setDepth",                    (q_method_t)QGLFORMAT_setDepth);
   QC_QGLFormat->addMethod("setDepthBufferSize",          (q_method_t)QGLFORMAT_setDepthBufferSize);
   QC_QGLFormat->addMethod("setDirectRendering",          (q_method_t)QGLFORMAT_setDirectRendering);
   QC_QGLFormat->addMethod("setDoubleBuffer",             (q_method_t)QGLFORMAT_setDoubleBuffer);
   QC_QGLFormat->addMethod("setGreenBufferSize",          (q_method_t)QGLFORMAT_setGreenBufferSize);
   QC_QGLFormat->addMethod("setOption",                   (q_method_t)QGLFORMAT_setOption);
   QC_QGLFormat->addMethod("setOverlay",                  (q_method_t)QGLFORMAT_setOverlay);
   QC_QGLFormat->addMethod("setPlane",                    (q_method_t)QGLFORMAT_setPlane);
   QC_QGLFormat->addMethod("setRedBufferSize",            (q_method_t)QGLFORMAT_setRedBufferSize);
   QC_QGLFormat->addMethod("setRgba",                     (q_method_t)QGLFORMAT_setRgba);
   QC_QGLFormat->addMethod("setSampleBuffers",            (q_method_t)QGLFORMAT_setSampleBuffers);
   QC_QGLFormat->addMethod("setSamples",                  (q_method_t)QGLFORMAT_setSamples);
   QC_QGLFormat->addMethod("setStencil",                  (q_method_t)QGLFORMAT_setStencil);
   QC_QGLFormat->addMethod("setStencilBufferSize",        (q_method_t)QGLFORMAT_setStencilBufferSize);
   QC_QGLFormat->addMethod("setStereo",                   (q_method_t)QGLFORMAT_setStereo);
   QC_QGLFormat->addMethod("setSwapInterval",             (q_method_t)QGLFORMAT_setSwapInterval);
   QC_QGLFormat->addMethod("stencil",                     (q_method_t)QGLFORMAT_stencil);
   QC_QGLFormat->addMethod("stencilBufferSize",           (q_method_t)QGLFORMAT_stencilBufferSize);
   QC_QGLFormat->addMethod("stereo",                      (q_method_t)QGLFORMAT_stereo);
   QC_QGLFormat->addMethod("swapInterval",                (q_method_t)QGLFORMAT_swapInterval);
   QC_QGLFormat->addMethod("testOption",                  (q_method_t)QGLFORMAT_testOption);

   // static methods
   QC_QGLFormat->addStaticMethod("defaultFormat",                f_QGLFormat_defaultFormat);
   QC_QGLFormat->addStaticMethod("defaultOverlayFormat",         f_QGLFormat_defaultOverlayFormat);
   QC_QGLFormat->addStaticMethod("hasOpenGL",                    f_QGLFormat_hasOpenGL);
   QC_QGLFormat->addStaticMethod("hasOpenGLOverlays",            f_QGLFormat_hasOpenGLOverlays);
   QC_QGLFormat->addStaticMethod("openGLVersionFlags",           f_QGLFormat_openGLVersionFlags);
   QC_QGLFormat->addStaticMethod("setDefaultFormat",             f_QGLFormat_setDefaultFormat);
   QC_QGLFormat->addStaticMethod("setDefaultOverlayFormat",      f_QGLFormat_setDefaultOverlayFormat);

   return QC_QGLFormat;
}

QoreNamespace *initQGLNS()
{
   QoreNamespace *ns = new QoreNamespace("QGL");

   // FormatOption namespace
   ns->addConstant("DoubleBuffer",             new QoreBigIntNode(QGL::DoubleBuffer));
   ns->addConstant("DepthBuffer",              new QoreBigIntNode(QGL::DepthBuffer));
   ns->addConstant("Rgba",                     new QoreBigIntNode(QGL::Rgba));
   ns->addConstant("AlphaChannel",             new QoreBigIntNode(QGL::AlphaChannel));
   ns->addConstant("AccumBuffer",              new QoreBigIntNode(QGL::AccumBuffer));
   ns->addConstant("StencilBuffer",            new QoreBigIntNode(QGL::StencilBuffer));
   ns->addConstant("StereoBuffers",            new QoreBigIntNode(QGL::StereoBuffers));
   ns->addConstant("DirectRendering",          new QoreBigIntNode(QGL::DirectRendering));
   ns->addConstant("HasOverlay",               new QoreBigIntNode(QGL::HasOverlay));
   ns->addConstant("SampleBuffers",            new QoreBigIntNode(QGL::SampleBuffers));
   ns->addConstant("SingleBuffer",             new QoreBigIntNode(QGL::SingleBuffer));
   ns->addConstant("NoDepthBuffer",            new QoreBigIntNode(QGL::NoDepthBuffer));
   ns->addConstant("ColorIndex",               new QoreBigIntNode(QGL::ColorIndex));
   ns->addConstant("NoAlphaChannel",           new QoreBigIntNode(QGL::NoAlphaChannel));
   ns->addConstant("NoAccumBuffer",            new QoreBigIntNode(QGL::NoAccumBuffer));
   ns->addConstant("NoStencilBuffer",          new QoreBigIntNode(QGL::NoStencilBuffer));
   ns->addConstant("NoStereoBuffers",          new QoreBigIntNode(QGL::NoStereoBuffers));
   ns->addConstant("IndirectRendering",        new QoreBigIntNode(QGL::IndirectRendering));
   ns->addConstant("NoOverlay",                new QoreBigIntNode(QGL::NoOverlay));
   ns->addConstant("NoSampleBuffers",          new QoreBigIntNode(QGL::NoSampleBuffers));

   return ns;
}

QoreNamespace *initQGLFormatNS()
{
   QoreNamespace *ns = new QoreNamespace("QGLFormat");
   ns->addSystemClass(initQGLFormatClass());

   // OpenGLVersionFlag enum
   ns->addConstant("OpenGL_Version_None",      new QoreBigIntNode(QGLFormat::OpenGL_Version_None));
   ns->addConstant("OpenGL_Version_1_1",       new QoreBigIntNode(QGLFormat::OpenGL_Version_1_1));
   ns->addConstant("OpenGL_Version_1_2",       new QoreBigIntNode(QGLFormat::OpenGL_Version_1_2));
   ns->addConstant("OpenGL_Version_1_3",       new QoreBigIntNode(QGLFormat::OpenGL_Version_1_3));
   ns->addConstant("OpenGL_Version_1_4",       new QoreBigIntNode(QGLFormat::OpenGL_Version_1_4));
   ns->addConstant("OpenGL_Version_1_5",       new QoreBigIntNode(QGLFormat::OpenGL_Version_1_5));
   ns->addConstant("OpenGL_Version_2_0",       new QoreBigIntNode(QGLFormat::OpenGL_Version_2_0));
   ns->addConstant("OpenGL_Version_2_1",       new QoreBigIntNode(QGLFormat::OpenGL_Version_2_1));
   ns->addConstant("OpenGL_ES_Common_Version_1_0", new QoreBigIntNode(QGLFormat::OpenGL_ES_Common_Version_1_0));
   ns->addConstant("OpenGL_ES_CommonLite_Version_1_0", new QoreBigIntNode(QGLFormat::OpenGL_ES_CommonLite_Version_1_0));
   ns->addConstant("OpenGL_ES_Common_Version_1_1", new QoreBigIntNode(QGLFormat::OpenGL_ES_Common_Version_1_1));
   ns->addConstant("OpenGL_ES_CommonLite_Version_1_1", new QoreBigIntNode(QGLFormat::OpenGL_ES_CommonLite_Version_1_1));
   ns->addConstant("OpenGL_ES_Version_2_0",    new QoreBigIntNode(QGLFormat::OpenGL_ES_Version_2_0));

   return ns;
}
