/*
  opengl-glu.cc
  
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

#include "qore-opengl.h"

#include <glu.h>

/*
//void gluBeginCurve (GLUnurbs* nurb);
static AbstractQoreNode *f_gluBeginCurve(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   gluBeginCurve(nurb);
   return 0;
}
*/

/*
//void gluBeginPolygon (GLUtesselator* tess);
static AbstractQoreNode *f_gluBeginPolygon(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   gluBeginPolygon(tess);
   return 0;
}
*/

/*
//void gluBeginSurface (GLUnurbs* nurb);
static AbstractQoreNode *f_gluBeginSurface(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   gluBeginSurface(nurb);
   return 0;
}
*/

/*
//void gluBeginTrim (GLUnurbs* nurb);
static AbstractQoreNode *f_gluBeginTrim(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   gluBeginTrim(nurb);
   return 0;
}
*/

/*
//GLint gluBuild1DMipmapLevels (GLenum target, GLint internalFormat, GLsizei width, GLenum format, GLenum type, GLint level, GLint base, GLint max, const void *data);
static AbstractQoreNode *f_gluBuild1DMipmapLevels(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int internalFormat = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum format = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   int level = p ? p->getAsInt() : 0;
   p = get_param(params, 6);
   int base = p ? p->getAsInt() : 0;
   p = get_param(params, 7);
   int max = p ? p->getAsInt() : 0;
   p = get_param(params, 8);
   ??? void* data = p;
   return new QoreBigIntNode(gluBuild1DMipmapLevels(target, internalFormat, width, format, type, level, base, max, data));
}
*/

/*
//GLint gluBuild1DMipmaps (GLenum target, GLint internalFormat, GLsizei width, GLenum format, GLenum type, const void *data);
static AbstractQoreNode *f_gluBuild1DMipmaps(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int internalFormat = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum format = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   ??? void* data = p;
   return new QoreBigIntNode(gluBuild1DMipmaps(target, internalFormat, width, format, type, data));
}
*/

/*
//GLint gluBuild2DMipmapLevels (GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, GLint level, GLint base, GLint max, const void *data);
static AbstractQoreNode *f_gluBuild2DMipmapLevels(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int internalFormat = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLenum format = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   int level = p ? p->getAsInt() : 0;
   p = get_param(params, 7);
   int base = p ? p->getAsInt() : 0;
   p = get_param(params, 8);
   int max = p ? p->getAsInt() : 0;
   p = get_param(params, 9);
   ??? void* data = p;
   return new QoreBigIntNode(gluBuild2DMipmapLevels(target, internalFormat, width, height, format, type, level, base, max, data));
}
*/

/*
//GLint gluBuild2DMipmaps (GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);
static AbstractQoreNode *f_gluBuild2DMipmaps(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int internalFormat = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLenum format = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   ??? void* data = p;
   return new QoreBigIntNode(gluBuild2DMipmaps(target, internalFormat, width, height, format, type, data));
}
*/

/*
//GLint gluBuild3DMipmapLevels (GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLint level, GLint base, GLint max, const void *data);
static AbstractQoreNode *f_gluBuild3DMipmapLevels(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int internalFormat = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLsizei depth = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLenum format = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   int level = p ? p->getAsInt() : 0;
   p = get_param(params, 8);
   int base = p ? p->getAsInt() : 0;
   p = get_param(params, 9);
   int max = p ? p->getAsInt() : 0;
   p = get_param(params, 10);
   ??? void* data = p;
   return new QoreBigIntNode(gluBuild3DMipmapLevels(target, internalFormat, width, height, depth, format, type, level, base, max, data));
}
*/

/*
//GLint gluBuild3DMipmaps (GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *data);
static AbstractQoreNode *f_gluBuild3DMipmaps(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int internalFormat = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLsizei depth = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLenum format = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   ??? void* data = p;
   return new QoreBigIntNode(gluBuild3DMipmaps(target, internalFormat, width, height, depth, format, type, data));
}
*/

/*
//GLboolean gluCheckExtension (const GLubyte *extName, const GLubyte *extString);
static AbstractQoreNode *f_gluCheckExtension(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLubyte* extName = p;
   p = get_param(params, 1);
   ??? GLubyte* extString = p;
   return get_bool_node(gluCheckExtension(extName, extString));
}
*/

/*
//void gluCylinder (GLUquadric* quad, GLdouble base, GLdouble top, GLdouble height, GLint slices, GLint stacks);
static AbstractQoreNode *f_gluCylinder(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUquadric* quad = p;
   p = get_param(params, 1);
   GLdouble base = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble top = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble height = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   int slices = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   int stacks = p ? p->getAsInt() : 0;
   gluCylinder(quad, base, top, height, slices, stacks);
   return 0;
}
*/

/*
//void gluDeleteNurbsRenderer (GLUnurbs* nurb);
static AbstractQoreNode *f_gluDeleteNurbsRenderer(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   gluDeleteNurbsRenderer(nurb);
   return 0;
}
*/

/*
//void gluDeleteQuadric (GLUquadric* quad);
static AbstractQoreNode *f_gluDeleteQuadric(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUquadric* quad = p;
   gluDeleteQuadric(quad);
   return 0;
}
*/

/*
//void gluDeleteTess (GLUtesselator* tess);
static AbstractQoreNode *f_gluDeleteTess(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   gluDeleteTess(tess);
   return 0;
}
*/

/*
//void gluDisk (GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops);
static AbstractQoreNode *f_gluDisk(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUquadric* quad = p;
   p = get_param(params, 1);
   GLdouble inner = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble outer = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   int slices = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int loops = p ? p->getAsInt() : 0;
   gluDisk(quad, inner, outer, slices, loops);
   return 0;
}
*/

/*
//void gluEndCurve (GLUnurbs* nurb);
static AbstractQoreNode *f_gluEndCurve(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   gluEndCurve(nurb);
   return 0;
}
*/

/*
//void gluEndPolygon (GLUtesselator* tess);
static AbstractQoreNode *f_gluEndPolygon(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   gluEndPolygon(tess);
   return 0;
}
*/

/*
//void gluEndSurface (GLUnurbs* nurb);
static AbstractQoreNode *f_gluEndSurface(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   gluEndSurface(nurb);
   return 0;
}
*/

/*
//void gluEndTrim (GLUnurbs* nurb);
static AbstractQoreNode *f_gluEndTrim(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   gluEndTrim(nurb);
   return 0;
}
*/

//const GLubyte * gluErrorString (GLenum error);
static AbstractQoreNode *f_gluErrorString(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum error = (GLenum)(p ? p->getAsInt() : 0);

   const GLubyte *str = gluErrorString(error);
   if (!str)
      return 0;
   return new QoreStringNode((const char *)str, QCS_ISO_8859_1);
}

/*
//void gluGetNurbsProperty (GLUnurbs* nurb, GLenum property, GLfloat* data);
static AbstractQoreNode *f_gluGetNurbsProperty(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   p = get_param(params, 1);
   GLenum property = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* data = p;
   gluGetNurbsProperty(nurb, property, data);
   return 0;
}
*/

//const GLubyte * gluGetString (GLenum name);
static AbstractQoreNode *f_gluGetString(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum name = (GLenum)(p ? p->getAsInt() : 0);

   const GLubyte *str = gluGetString(name);
   if (!str)
      return 0;
   return new QoreStringNode((const char *)str, QCS_ISO_8859_1);
}

/*
//void gluGetTessProperty (GLUtesselator* tess, GLenum which, GLdouble* data);
static AbstractQoreNode *f_gluGetTessProperty(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   p = get_param(params, 1);
   GLenum which = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLdouble* data = p;
   gluGetTessProperty(tess, which, data);
   return 0;
}
*/

/*
//void gluLoadSamplingMatrices (GLUnurbs* nurb, const GLfloat *model, const GLfloat *perspective, const GLint *view);
static AbstractQoreNode *f_gluLoadSamplingMatrices(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   p = get_param(params, 1);
   ??? GLfloat* model = p;
   p = get_param(params, 2);
   ??? GLfloat* perspective = p;
   p = get_param(params, 3);
   ??? GLint* view = p;
   gluLoadSamplingMatrices(nurb, model, perspective, view);
   return 0;
}
*/

//void gluLookAt (GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY, GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ);
static AbstractQoreNode *f_gluLookAt(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble eyeX = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble eyeY = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble eyeZ = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble centerX = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLdouble centerY = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 5);
   GLdouble centerZ = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 6);
   GLdouble upX = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 7);
   GLdouble upY = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 8);
   GLdouble upZ = (GLdouble)(p ? p->getAsFloat() : 0.0);
   gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
   return 0;
}

/*
//GLUnurbs* gluNewNurbsRenderer (void);
static AbstractQoreNode *f_gluNewNurbsRenderer(const QoreListNode *params, ExceptionSink *xsink)
{
   ??? return new QoreBigIntNode(gluNewNurbsRenderer());
}
*/

/*
//GLUquadric* gluNewQuadric (void);
static AbstractQoreNode *f_gluNewQuadric(const QoreListNode *params, ExceptionSink *xsink)
{
   ??? return new QoreBigIntNode(gluNewQuadric());
}
*/

/*
//GLUtesselator* gluNewTess (void);
static AbstractQoreNode *f_gluNewTess(const QoreListNode *params, ExceptionSink *xsink)
{
   ??? return new QoreBigIntNode(gluNewTess());
}
*/

/*
//void gluNextContour (GLUtesselator* tess, GLenum type);
static AbstractQoreNode *f_gluNextContour(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   p = get_param(params, 1);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   gluNextContour(tess, type);
   return 0;
}
*/

/*
//void gluNurbsCallback (GLUnurbs* nurb, GLenum which, GLvoid (*CallBackFunc) ());
static AbstractQoreNode *f_gluNurbsCallback(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   p = get_param(params, 1);
   GLenum which = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLvoid (*CallBackFunc) () arg2 = p;
   gluNurbsCallback(nurb, which, arg2);
   return 0;
}
*/

/*
//void gluNurbsCallbackData (GLUnurbs* nurb, GLvoid* userData);
static AbstractQoreNode *f_gluNurbsCallbackData(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   p = get_param(params, 1);
   ??? GLvoid* userData = p;
   gluNurbsCallbackData(nurb, userData);
   return 0;
}
*/

/*
//void gluNurbsCallbackDataEXT (GLUnurbs* nurb, GLvoid* userData);
static AbstractQoreNode *f_gluNurbsCallbackDataEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   p = get_param(params, 1);
   ??? GLvoid* userData = p;
   gluNurbsCallbackDataEXT(nurb, userData);
   return 0;
}
*/

/*
//void gluNurbsCurve (GLUnurbs* nurb, GLint knotCount, GLfloat *knots, GLint stride, GLfloat *control, GLint order, GLenum type);
static AbstractQoreNode *f_gluNurbsCurve(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   p = get_param(params, 1);
   int knotCount = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   ??? GLfloat* knots = p;
   p = get_param(params, 3);
   int stride = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   ??? GLfloat* control = p;
   p = get_param(params, 5);
   int order = p ? p->getAsInt() : 0;
   p = get_param(params, 6);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   gluNurbsCurve(nurb, knotCount, knots, stride, control, order, type);
   return 0;
}
*/

/*
//void gluNurbsProperty (GLUnurbs* nurb, GLenum property, GLfloat value);
static AbstractQoreNode *f_gluNurbsProperty(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   p = get_param(params, 1);
   GLenum property = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLfloat value = (GLfloat)(p ? p->getAsFloat() : 0.0);
   gluNurbsProperty(nurb, property, value);
   return 0;
}
*/

/*
//void gluNurbsSurface (GLUnurbs* nurb, GLint sKnotCount, GLfloat* sKnots, GLint tKnotCount, GLfloat* tKnots, GLint sStride, GLint tStride, GLfloat* control, GLint sOrder, GLint tOrder, GLenum type);
static AbstractQoreNode *f_gluNurbsSurface(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   p = get_param(params, 1);
   int sKnotCount = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   ??? GLfloat* sKnots = p;
   p = get_param(params, 3);
   int tKnotCount = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   ??? GLfloat* tKnots = p;
   p = get_param(params, 5);
   int sStride = p ? p->getAsInt() : 0;
   p = get_param(params, 6);
   int tStride = p ? p->getAsInt() : 0;
   p = get_param(params, 7);
   ??? GLfloat* control = p;
   p = get_param(params, 8);
   int sOrder = p ? p->getAsInt() : 0;
   p = get_param(params, 9);
   int tOrder = p ? p->getAsInt() : 0;
   p = get_param(params, 10);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   gluNurbsSurface(nurb, sKnotCount, sKnots, tKnotCount, tKnots, sStride, tStride, control, sOrder, tOrder, type);
   return 0;
}
*/

//void gluOrtho2D (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
static AbstractQoreNode *f_gluOrtho2D(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble left = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble right = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble bottom = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble top = (GLdouble)(p ? p->getAsFloat() : 0.0);
   gluOrtho2D(left, right, bottom, top);
   return 0;
}

/*
//void gluPartialDisk (GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops, GLdouble start, GLdouble sweep);
static AbstractQoreNode *f_gluPartialDisk(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUquadric* quad = p;
   p = get_param(params, 1);
   GLdouble inner = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble outer = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   int slices = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int loops = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   GLdouble start = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 6);
   GLdouble sweep = (GLdouble)(p ? p->getAsFloat() : 0.0);
   gluPartialDisk(quad, inner, outer, slices, loops, start, sweep);
   return 0;
}
*/

//void gluPerspective (GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
static AbstractQoreNode *f_gluPerspective(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble fovy = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble aspect = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble zNear = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble zFar = (GLdouble)(p ? p->getAsFloat() : 0.0);
   gluPerspective(fovy, aspect, zNear, zFar);
   return 0;
}

/*
//void gluPickMatrix (GLdouble x, GLdouble y, GLdouble delX, GLdouble delY, GLint *viewport);
static AbstractQoreNode *f_gluPickMatrix(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble delX = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble delY = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   ??? GLint* viewport = p;
   gluPickMatrix(x, y, delX, delY, viewport);
   return 0;
}
*/

/*
//GLint gluProject (GLdouble objX, GLdouble objY, GLdouble objZ, const GLdouble *model, const GLdouble *proj, const GLint *view, GLdouble* winX, GLdouble* winY, GLdouble* winZ);
static AbstractQoreNode *f_gluProject(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble objX = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble objY = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble objZ = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   ??? GLdouble* model = p;
   p = get_param(params, 4);
   ??? GLdouble* proj = p;
   p = get_param(params, 5);
   ??? GLint* view = p;
   p = get_param(params, 6);
   ??? GLdouble* winX = p;
   p = get_param(params, 7);
   ??? GLdouble* winY = p;
   p = get_param(params, 8);
   ??? GLdouble* winZ = p;
   return new QoreBigIntNode(gluProject(objX, objY, objZ, model, proj, view, winX, winY, winZ));
}
*/

/*
//void gluPwlCurve (GLUnurbs* nurb, GLint count, GLfloat* data, GLint stride, GLenum type);
static AbstractQoreNode *f_gluPwlCurve(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUnurbs* nurb = p;
   p = get_param(params, 1);
   int count = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   ??? GLfloat* data = p;
   p = get_param(params, 3);
   int stride = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   gluPwlCurve(nurb, count, data, stride, type);
   return 0;
}
*/

/*
//void gluQuadricCallback (GLUquadric* quad, GLenum which, GLvoid (*CallBackFunc) ());
static AbstractQoreNode *f_gluQuadricCallback(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUquadric* quad = p;
   p = get_param(params, 1);
   GLenum which = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLvoid (*CallBackFunc) () arg2 = p;
   gluQuadricCallback(quad, which, arg2);
   return 0;
}
*/

/*
//void gluQuadricDrawStyle (GLUquadric* quad, GLenum draw);
static AbstractQoreNode *f_gluQuadricDrawStyle(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUquadric* quad = p;
   p = get_param(params, 1);
   GLenum draw = (GLenum)(p ? p->getAsInt() : 0);
   gluQuadricDrawStyle(quad, draw);
   return 0;
}
*/

/*
//void gluQuadricNormals (GLUquadric* quad, GLenum normal);
static AbstractQoreNode *f_gluQuadricNormals(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUquadric* quad = p;
   p = get_param(params, 1);
   GLenum normal = (GLenum)(p ? p->getAsInt() : 0);
   gluQuadricNormals(quad, normal);
   return 0;
}
*/

/*
//void gluQuadricOrientation (GLUquadric* quad, GLenum orientation);
static AbstractQoreNode *f_gluQuadricOrientation(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUquadric* quad = p;
   p = get_param(params, 1);
   GLenum orientation = (GLenum)(p ? p->getAsInt() : 0);
   gluQuadricOrientation(quad, orientation);
   return 0;
}
*/

/*
//void gluQuadricTexture (GLUquadric* quad, GLboolean texture);
static AbstractQoreNode *f_gluQuadricTexture(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUquadric* quad = p;
   p = get_param(params, 1);
   GLboolean texture = (GLboolean)(p ? p->getAsBool() : false);
   gluQuadricTexture(quad, texture);
   return 0;
}
*/

/*
//GLint gluScaleImage (GLenum format, GLsizei wIn, GLsizei hIn, GLenum typeIn, const void *dataIn, GLsizei wOut, GLsizei hOut, GLenum typeOut, GLvoid* dataOut);
static AbstractQoreNode *f_gluScaleImage(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum format = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei wIn = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizei hIn = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum typeIn = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   ??? void* dataIn = p;
   p = get_param(params, 5);
   GLsizei wOut = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLsizei hOut = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   GLenum typeOut = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 8);
   ??? GLvoid* dataOut = p;
   return new QoreBigIntNode(gluScaleImage(format, wIn, hIn, typeIn, dataIn, wOut, hOut, typeOut, dataOut));
}
*/

/*
//void gluSphere (GLUquadric* quad, GLdouble radius, GLint slices, GLint stacks);
static AbstractQoreNode *f_gluSphere(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUquadric* quad = p;
   p = get_param(params, 1);
   GLdouble radius = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   int slices = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int stacks = p ? p->getAsInt() : 0;
   gluSphere(quad, radius, slices, stacks);
   return 0;
}
*/

/*
//void gluTessBeginContour (GLUtesselator* tess);
static AbstractQoreNode *f_gluTessBeginContour(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   gluTessBeginContour(tess);
   return 0;
}
*/

/*
//void gluTessBeginPolygon (GLUtesselator* tess, GLvoid* data);
static AbstractQoreNode *f_gluTessBeginPolygon(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   p = get_param(params, 1);
   ??? GLvoid* data = p;
   gluTessBeginPolygon(tess, data);
   return 0;
}
*/

/*
//void gluTessCallback (GLUtesselator* tess, GLenum which, GLvoid (*CallBackFunc) ());
static AbstractQoreNode *f_gluTessCallback(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   p = get_param(params, 1);
   GLenum which = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLvoid (*CallBackFunc) () arg2 = p;
   gluTessCallback(tess, which, arg2);
   return 0;
}
*/

/*
//void gluTessEndContour (GLUtesselator* tess);
static AbstractQoreNode *f_gluTessEndContour(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   gluTessEndContour(tess);
   return 0;
}
*/

/*
//void gluTessEndPolygon (GLUtesselator* tess);
static AbstractQoreNode *f_gluTessEndPolygon(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   gluTessEndPolygon(tess);
   return 0;
}
*/

/*
//void gluTessNormal (GLUtesselator* tess, GLdouble valueX, GLdouble valueY, GLdouble valueZ);
static AbstractQoreNode *f_gluTessNormal(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   p = get_param(params, 1);
   GLdouble valueX = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble valueY = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble valueZ = (GLdouble)(p ? p->getAsFloat() : 0.0);
   gluTessNormal(tess, valueX, valueY, valueZ);
   return 0;
}
*/

/*
//void gluTessProperty (GLUtesselator* tess, GLenum which, GLdouble data);
static AbstractQoreNode *f_gluTessProperty(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   p = get_param(params, 1);
   GLenum which = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLdouble data = (GLdouble)(p ? p->getAsFloat() : 0.0);
   gluTessProperty(tess, which, data);
   return 0;
}
*/

/*
//void gluTessVertex (GLUtesselator* tess, GLdouble *location, GLvoid* data);
static AbstractQoreNode *f_gluTessVertex(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLUtesselator* tess = p;
   p = get_param(params, 1);
   ??? GLdouble* location = p;
   p = get_param(params, 2);
   ??? GLvoid* data = p;
   gluTessVertex(tess, location, data);
   return 0;
}
*/

/*
//GLint gluUnProject (GLdouble winX, GLdouble winY, GLdouble winZ, const GLdouble *model, const GLdouble *proj, const GLint *view, GLdouble* objX, GLdouble* objY, GLdouble* objZ);
static AbstractQoreNode *f_gluUnProject(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble winX = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble winY = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble winZ = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   ??? GLdouble* model = p;
   p = get_param(params, 4);
   ??? GLdouble* proj = p;
   p = get_param(params, 5);
   ??? GLint* view = p;
   p = get_param(params, 6);
   ??? GLdouble* objX = p;
   p = get_param(params, 7);
   ??? GLdouble* objY = p;
   p = get_param(params, 8);
   ??? GLdouble* objZ = p;
   return new QoreBigIntNode(gluUnProject(winX, winY, winZ, model, proj, view, objX, objY, objZ));
}
*/

/*
//GLint gluUnProject4 (GLdouble winX, GLdouble winY, GLdouble winZ, GLdouble clipW, const GLdouble *model, const GLdouble *proj, const GLint *view, GLdouble nearPlane, GLdouble farPlane, GLdouble* objX, GLdouble* objY, GLdouble* objZ, GLdouble* objW);
static AbstractQoreNode *f_gluUnProject4(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble winX = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble winY = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble winZ = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble clipW = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   ??? GLdouble* model = p;
   p = get_param(params, 5);
   ??? GLdouble* proj = p;
   p = get_param(params, 6);
   ??? GLint* view = p;
   p = get_param(params, 7);
   GLdouble nearPlane = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 8);
   GLdouble farPlane = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 9);
   ??? GLdouble* objX = p;
   p = get_param(params, 10);
   ??? GLdouble* objY = p;
   p = get_param(params, 11);
   ??? GLdouble* objZ = p;
   p = get_param(params, 12);
   ??? GLdouble* objW = p;
   return new QoreBigIntNode(gluUnProject4(winX, winY, winZ, clipW, model, proj, view, nearPlane, farPlane, objX, objY, objZ, objW));
}
*/

void initOpenGLU()
{
   //builtinFunctions.add("gluBeginCurve",                f_gluBeginCurve, QDOM_GUI);
   //builtinFunctions.add("gluBeginPolygon",              f_gluBeginPolygon, QDOM_GUI);
   //builtinFunctions.add("gluBeginSurface",              f_gluBeginSurface, QDOM_GUI);
   //builtinFunctions.add("gluBeginTrim",                 f_gluBeginTrim, QDOM_GUI);
   //builtinFunctions.add("gluBuild1DMipmapLevels",       f_gluBuild1DMipmapLevels, QDOM_GUI);
   //builtinFunctions.add("gluBuild1DMipmaps",            f_gluBuild1DMipmaps, QDOM_GUI);
   //builtinFunctions.add("gluBuild2DMipmapLevels",       f_gluBuild2DMipmapLevels, QDOM_GUI);
   //builtinFunctions.add("gluBuild2DMipmaps",            f_gluBuild2DMipmaps, QDOM_GUI);
   //builtinFunctions.add("gluBuild3DMipmapLevels",       f_gluBuild3DMipmapLevels, QDOM_GUI);
   //builtinFunctions.add("gluBuild3DMipmaps",            f_gluBuild3DMipmaps, QDOM_GUI);
   //builtinFunctions.add("gluCheckExtension",            f_gluCheckExtension, QDOM_GUI);
   //builtinFunctions.add("gluCylinder",                  f_gluCylinder, QDOM_GUI);
   //builtinFunctions.add("gluDeleteNurbsRenderer",       f_gluDeleteNurbsRenderer, QDOM_GUI);
   //builtinFunctions.add("gluDeleteQuadric",             f_gluDeleteQuadric, QDOM_GUI);
   //builtinFunctions.add("gluDeleteTess",                f_gluDeleteTess, QDOM_GUI);
   //builtinFunctions.add("gluDisk",                      f_gluDisk, QDOM_GUI);
   //builtinFunctions.add("gluEndCurve",                  f_gluEndCurve, QDOM_GUI);
   //builtinFunctions.add("gluEndPolygon",                f_gluEndPolygon, QDOM_GUI);
   //builtinFunctions.add("gluEndSurface",                f_gluEndSurface, QDOM_GUI);
   //builtinFunctions.add("gluEndTrim",                   f_gluEndTrim, QDOM_GUI);
   builtinFunctions.add("gluErrorString",               f_gluErrorString, QDOM_GUI);
   //builtinFunctions.add("gluGetNurbsProperty",          f_gluGetNurbsProperty, QDOM_GUI);
   builtinFunctions.add("gluGetString",                 f_gluGetString, QDOM_GUI);
   //builtinFunctions.add("gluGetTessProperty",           f_gluGetTessProperty, QDOM_GUI);
   //builtinFunctions.add("gluLoadSamplingMatrices",      f_gluLoadSamplingMatrices, QDOM_GUI);
   builtinFunctions.add("gluLookAt",                    f_gluLookAt, QDOM_GUI);
   //builtinFunctions.add("gluNewNurbsRenderer",          f_gluNewNurbsRenderer, QDOM_GUI);
   //builtinFunctions.add("gluNewQuadric",                f_gluNewQuadric, QDOM_GUI);
   //builtinFunctions.add("gluNewTess",                   f_gluNewTess, QDOM_GUI);
   //builtinFunctions.add("gluNextContour",               f_gluNextContour, QDOM_GUI);
   //builtinFunctions.add("gluNurbsCallback",             f_gluNurbsCallback, QDOM_GUI);
   //builtinFunctions.add("gluNurbsCallbackData",         f_gluNurbsCallbackData, QDOM_GUI);
   //builtinFunctions.add("gluNurbsCallbackDataEXT",      f_gluNurbsCallbackDataEXT, QDOM_GUI);
   //builtinFunctions.add("gluNurbsCurve",                f_gluNurbsCurve, QDOM_GUI);
   //builtinFunctions.add("gluNurbsProperty",             f_gluNurbsProperty, QDOM_GUI);
   //builtinFunctions.add("gluNurbsSurface",              f_gluNurbsSurface, QDOM_GUI);
   builtinFunctions.add("gluOrtho2D",                   f_gluOrtho2D, QDOM_GUI);
   //builtinFunctions.add("gluPartialDisk",               f_gluPartialDisk, QDOM_GUI);
   builtinFunctions.add("gluPerspective",               f_gluPerspective, QDOM_GUI);
   //builtinFunctions.add("gluPickMatrix",                f_gluPickMatrix, QDOM_GUI);
   //builtinFunctions.add("gluProject",                   f_gluProject, QDOM_GUI);
   //builtinFunctions.add("gluPwlCurve",                  f_gluPwlCurve, QDOM_GUI);
   //builtinFunctions.add("gluQuadricCallback",           f_gluQuadricCallback, QDOM_GUI);
   //builtinFunctions.add("gluQuadricDrawStyle",          f_gluQuadricDrawStyle, QDOM_GUI);
   //builtinFunctions.add("gluQuadricNormals",            f_gluQuadricNormals, QDOM_GUI);
   //builtinFunctions.add("gluQuadricOrientation",        f_gluQuadricOrientation, QDOM_GUI);
   //builtinFunctions.add("gluQuadricTexture",            f_gluQuadricTexture, QDOM_GUI);
   //builtinFunctions.add("gluScaleImage",                f_gluScaleImage, QDOM_GUI);
   //builtinFunctions.add("gluSphere",                    f_gluSphere, QDOM_GUI);
   //builtinFunctions.add("gluTessBeginContour",          f_gluTessBeginContour, QDOM_GUI);
   //builtinFunctions.add("gluTessBeginPolygon",          f_gluTessBeginPolygon, QDOM_GUI);
   //builtinFunctions.add("gluTessCallback",              f_gluTessCallback, QDOM_GUI);
   //builtinFunctions.add("gluTessEndContour",            f_gluTessEndContour, QDOM_GUI);
   //builtinFunctions.add("gluTessEndPolygon",            f_gluTessEndPolygon, QDOM_GUI);
   //builtinFunctions.add("gluTessNormal",                f_gluTessNormal, QDOM_GUI);
   //builtinFunctions.add("gluTessProperty",              f_gluTessProperty, QDOM_GUI);
   //builtinFunctions.add("gluTessVertex",                f_gluTessVertex, QDOM_GUI);
   //builtinFunctions.add("gluUnProject",                 f_gluUnProject, QDOM_GUI);
   //builtinFunctions.add("gluUnProject4",                f_gluUnProject4, QDOM_GUI);
}
