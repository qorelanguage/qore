#/*
  opengl-ext.cc
  
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

#include "qore-opengl.h"

#include <glext.h>

//void glActiveTextureARB (GLenum);
static AbstractQoreNode *f_glActiveTextureARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   glActiveTextureARB(arg0);
   return 0;
}

//void glClientActiveTextureARB (GLenum);
static AbstractQoreNode *f_glClientActiveTextureARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   glClientActiveTextureARB(arg0);
   return 0;
}

//void glMultiTexCoord1dARB (GLenum, GLdouble);
static AbstractQoreNode *f_glMultiTexCoord1dARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLdouble arg1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord1dARB(arg0, arg1);
   return 0;
}

/*
//void glMultiTexCoord1dvARB (GLenum, const GLdouble *);
static AbstractQoreNode *f_glMultiTexCoord1dvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLdouble* arg1 = p;
   glMultiTexCoord1dvARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord1fARB (GLenum, GLfloat);
static AbstractQoreNode *f_glMultiTexCoord1fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat arg1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord1fARB(arg0, arg1);
   return 0;
}

/*
//void glMultiTexCoord1fvARB (GLenum, const GLfloat *);
static AbstractQoreNode *f_glMultiTexCoord1fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLfloat* arg1 = p;
   glMultiTexCoord1fvARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord1iARB (GLenum, GLint);
static AbstractQoreNode *f_glMultiTexCoord1iARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   glMultiTexCoord1iARB(arg0, arg1);
   return 0;
}

/*
//void glMultiTexCoord1ivARB (GLenum, const GLint *);
static AbstractQoreNode *f_glMultiTexCoord1ivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* arg1 = p;
   glMultiTexCoord1ivARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord1sARB (GLenum, GLshort);
static AbstractQoreNode *f_glMultiTexCoord1sARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort arg1 = (GLshort)(p ? p->getAsInt() : 0);
   glMultiTexCoord1sARB(arg0, arg1);
   return 0;
}

/*
//void glMultiTexCoord1svARB (GLenum, const GLshort *);
static AbstractQoreNode *f_glMultiTexCoord1svARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLshort* arg1 = p;
   glMultiTexCoord1svARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord2dARB (GLenum, GLdouble, GLdouble);
static AbstractQoreNode *f_glMultiTexCoord2dARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLdouble arg1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble arg2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord2dARB(arg0, arg1, arg2);
   return 0;
}

/*
//void glMultiTexCoord2dvARB (GLenum, const GLdouble *);
static AbstractQoreNode *f_glMultiTexCoord2dvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLdouble* arg1 = p;
   glMultiTexCoord2dvARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord2fARB (GLenum, GLfloat, GLfloat);
static AbstractQoreNode *f_glMultiTexCoord2fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat arg1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat arg2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord2fARB(arg0, arg1, arg2);
   return 0;
}

/*
//void glMultiTexCoord2fvARB (GLenum, const GLfloat *);
static AbstractQoreNode *f_glMultiTexCoord2fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLfloat* arg1 = p;
   glMultiTexCoord2fvARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord2iARB (GLenum, GLint, GLint);
static AbstractQoreNode *f_glMultiTexCoord2iARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int arg2 = p ? p->getAsInt() : 0;
   glMultiTexCoord2iARB(arg0, arg1, arg2);
   return 0;
}

/*
//void glMultiTexCoord2ivARB (GLenum, const GLint *);
static AbstractQoreNode *f_glMultiTexCoord2ivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* arg1 = p;
   glMultiTexCoord2ivARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord2sARB (GLenum, GLshort, GLshort);
static AbstractQoreNode *f_glMultiTexCoord2sARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort arg1 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort arg2 = (GLshort)(p ? p->getAsInt() : 0);
   glMultiTexCoord2sARB(arg0, arg1, arg2);
   return 0;
}

/*
//void glMultiTexCoord2svARB (GLenum, const GLshort *);
static AbstractQoreNode *f_glMultiTexCoord2svARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLshort* arg1 = p;
   glMultiTexCoord2svARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord3dARB (GLenum, GLdouble, GLdouble, GLdouble);
static AbstractQoreNode *f_glMultiTexCoord3dARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLdouble arg1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble arg2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble arg3 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord3dARB(arg0, arg1, arg2, arg3);
   return 0;
}

/*
//void glMultiTexCoord3dvARB (GLenum, const GLdouble *);
static AbstractQoreNode *f_glMultiTexCoord3dvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLdouble* arg1 = p;
   glMultiTexCoord3dvARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord3fARB (GLenum, GLfloat, GLfloat, GLfloat);
static AbstractQoreNode *f_glMultiTexCoord3fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat arg1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat arg2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat arg3 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord3fARB(arg0, arg1, arg2, arg3);
   return 0;
}

/*
//void glMultiTexCoord3fvARB (GLenum, const GLfloat *);
static AbstractQoreNode *f_glMultiTexCoord3fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLfloat* arg1 = p;
   glMultiTexCoord3fvARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord3iARB (GLenum, GLint, GLint, GLint);
static AbstractQoreNode *f_glMultiTexCoord3iARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int arg2 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int arg3 = p ? p->getAsInt() : 0;
   glMultiTexCoord3iARB(arg0, arg1, arg2, arg3);
   return 0;
}

/*
//void glMultiTexCoord3ivARB (GLenum, const GLint *);
static AbstractQoreNode *f_glMultiTexCoord3ivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* arg1 = p;
   glMultiTexCoord3ivARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord3sARB (GLenum, GLshort, GLshort, GLshort);
static AbstractQoreNode *f_glMultiTexCoord3sARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort arg1 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort arg2 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLshort arg3 = (GLshort)(p ? p->getAsInt() : 0);
   glMultiTexCoord3sARB(arg0, arg1, arg2, arg3);
   return 0;
}

/*
//void glMultiTexCoord3svARB (GLenum, const GLshort *);
static AbstractQoreNode *f_glMultiTexCoord3svARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLshort* arg1 = p;
   glMultiTexCoord3svARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord4dARB (GLenum, GLdouble, GLdouble, GLdouble, GLdouble);
static AbstractQoreNode *f_glMultiTexCoord4dARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLdouble arg1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble arg2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble arg3 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLdouble arg4 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord4dARB(arg0, arg1, arg2, arg3, arg4);
   return 0;
}

/*
//void glMultiTexCoord4dvARB (GLenum, const GLdouble *);
static AbstractQoreNode *f_glMultiTexCoord4dvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLdouble* arg1 = p;
   glMultiTexCoord4dvARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord4fARB (GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
static AbstractQoreNode *f_glMultiTexCoord4fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat arg1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat arg2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat arg3 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLfloat arg4 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord4fARB(arg0, arg1, arg2, arg3, arg4);
   return 0;
}

/*
//void glMultiTexCoord4fvARB (GLenum, const GLfloat *);
static AbstractQoreNode *f_glMultiTexCoord4fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLfloat* arg1 = p;
   glMultiTexCoord4fvARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord4iARB (GLenum, GLint, GLint, GLint, GLint);
static AbstractQoreNode *f_glMultiTexCoord4iARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int arg2 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int arg3 = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int arg4 = p ? p->getAsInt() : 0;
   glMultiTexCoord4iARB(arg0, arg1, arg2, arg3, arg4);
   return 0;
}

/*
//void glMultiTexCoord4ivARB (GLenum, const GLint *);
static AbstractQoreNode *f_glMultiTexCoord4ivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* arg1 = p;
   glMultiTexCoord4ivARB(arg0, arg1);
   return 0;
}
*/

//void glMultiTexCoord4sARB (GLenum, GLshort, GLshort, GLshort, GLshort);
static AbstractQoreNode *f_glMultiTexCoord4sARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort arg1 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort arg2 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLshort arg3 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLshort arg4 = (GLshort)(p ? p->getAsInt() : 0);
   glMultiTexCoord4sARB(arg0, arg1, arg2, arg3, arg4);
   return 0;
}

/*
//void glMultiTexCoord4svARB (GLenum, const GLshort *);
static AbstractQoreNode *f_glMultiTexCoord4svARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLshort* arg1 = p;
   glMultiTexCoord4svARB(arg0, arg1);
   return 0;
}
*/

/*
//void glLoadTransposeMatrixfARB (const GLfloat *);
static AbstractQoreNode *f_glLoadTransposeMatrixfARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* arg0 = p;
   glLoadTransposeMatrixfARB(arg0);
   return 0;
}
*/

/*
//void glLoadTransposeMatrixdARB (const GLdouble *);
static AbstractQoreNode *f_glLoadTransposeMatrixdARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* arg0 = p;
   glLoadTransposeMatrixdARB(arg0);
   return 0;
}
*/

/*
//void glMultTransposeMatrixfARB (const GLfloat *);
static AbstractQoreNode *f_glMultTransposeMatrixfARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* arg0 = p;
   glMultTransposeMatrixfARB(arg0);
   return 0;
}
*/

/*
//void glMultTransposeMatrixdARB (const GLdouble *);
static AbstractQoreNode *f_glMultTransposeMatrixdARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* arg0 = p;
   glMultTransposeMatrixdARB(arg0);
   return 0;
}
*/

//void glSampleCoverageARB (GLclampf, GLboolean);
static AbstractQoreNode *f_glSampleCoverageARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLclampf arg0 = (GLclampf)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLboolean arg1 = (GLboolean)(p ? p->getAsBool() : false);
   glSampleCoverageARB(arg0, arg1);
   return 0;
}

#ifdef HAVE_GLSAMPLEPASSARB
//void glSamplePassARB (GLenum);
static AbstractQoreNode *f_glSamplePassARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   glSamplePassARB(arg0);
   return 0;
}
#endif

/*
//void glCompressedTexImage3DARB (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
static AbstractQoreNode *f_glCompressedTexImage3DARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei arg3 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLsizei arg4 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLsizei arg5 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   int arg6 = p ? p->getAsInt() : 0;
   p = get_param(params, 7);
   GLsizei arg7 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 8);
   ??? GLvoid* arg8 = p;
   glCompressedTexImage3DARB(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
   return 0;
}
*/

/*
//void glCompressedTexImage2DARB (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
static AbstractQoreNode *f_glCompressedTexImage2DARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei arg3 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLsizei arg4 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   int arg5 = p ? p->getAsInt() : 0;
   p = get_param(params, 6);
   GLsizei arg6 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   ??? GLvoid* arg7 = p;
   glCompressedTexImage2DARB(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
   return 0;
}
*/

/*
//void glCompressedTexImage1DARB (GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *);
static AbstractQoreNode *f_glCompressedTexImage1DARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei arg3 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   int arg4 = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   GLsizei arg5 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   ??? GLvoid* arg6 = p;
   glCompressedTexImage1DARB(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
   return 0;
}
*/

/*
//void glCompressedTexSubImage3DARB (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
static AbstractQoreNode *f_glCompressedTexSubImage3DARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int arg2 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int arg3 = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int arg4 = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   GLsizei arg5 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLsizei arg6 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   GLsizei arg7 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 8);
   GLenum arg8 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 9);
   GLsizei arg9 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 10);
   ??? GLvoid* arg10 = p;
   glCompressedTexSubImage3DARB(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
   return 0;
}
*/

/*
//void glCompressedTexSubImage2DARB (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
static AbstractQoreNode *f_glCompressedTexSubImage2DARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int arg2 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int arg3 = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   GLsizei arg4 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLsizei arg5 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLenum arg6 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   GLsizei arg7 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 8);
   ??? GLvoid* arg8 = p;
   glCompressedTexSubImage2DARB(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
   return 0;
}
*/

/*
//void glCompressedTexSubImage1DARB (GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *);
static AbstractQoreNode *f_glCompressedTexSubImage1DARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int arg2 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   GLsizei arg3 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLenum arg4 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLsizei arg5 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   ??? GLvoid* arg6 = p;
   glCompressedTexSubImage1DARB(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
   return 0;
}
*/

/*
//void glGetCompressedTexImageARB (GLenum, GLint, GLvoid *);
static AbstractQoreNode *f_glGetCompressedTexImageARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   ??? GLvoid* arg2 = p;
   glGetCompressedTexImageARB(arg0, arg1, arg2);
   return 0;
}
*/

/*
//void glWeightbvARB (GLint, const GLbyte *);
static AbstractQoreNode *f_glWeightbvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   ??? GLbyte* arg1 = p;
   glWeightbvARB(arg0, arg1);
   return 0;
}
*/

/*
//void glWeightsvARB (GLint, const GLshort *);
static AbstractQoreNode *f_glWeightsvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   ??? GLshort* arg1 = p;
   glWeightsvARB(arg0, arg1);
   return 0;
}
*/

/*
//void glWeightivARB (GLint, const GLint *);
static AbstractQoreNode *f_glWeightivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   ??? GLint* arg1 = p;
   glWeightivARB(arg0, arg1);
   return 0;
}
*/

/*
//void glWeightfvARB (GLint, const GLfloat *);
static AbstractQoreNode *f_glWeightfvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   ??? GLfloat* arg1 = p;
   glWeightfvARB(arg0, arg1);
   return 0;
}
*/

/*
//void glWeightdvARB (GLint, const GLdouble *);
static AbstractQoreNode *f_glWeightdvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   ??? GLdouble* arg1 = p;
   glWeightdvARB(arg0, arg1);
   return 0;
}
*/

/*
//void glWeightubvARB (GLint, const GLubyte *);
static AbstractQoreNode *f_glWeightubvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   ??? GLubyte* arg1 = p;
   glWeightubvARB(arg0, arg1);
   return 0;
}
*/

/*
//void glWeightusvARB (GLint, const GLushort *);
static AbstractQoreNode *f_glWeightusvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   ??? GLushort* arg1 = p;
   glWeightusvARB(arg0, arg1);
   return 0;
}
*/

/*
//void glWeightuivARB (GLint, const GLuint *);
static AbstractQoreNode *f_glWeightuivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   ??? GLuint* arg1 = p;
   glWeightuivARB(arg0, arg1);
   return 0;
}
*/

/*
//void glWeightPointerARB (GLint, GLenum, GLsizei, const GLvoid *);
static AbstractQoreNode *f_glWeightPointerARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizei arg2 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLvoid* arg3 = p;
   glWeightPointerARB(arg0, arg1, arg2, arg3);
   return 0;
}
*/

//void glVertexBlendARB (GLint);
static AbstractQoreNode *f_glVertexBlendARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   glVertexBlendARB(arg0);
   return 0;
}

//void glWindowPos2dARB (GLdouble, GLdouble);
static AbstractQoreNode *f_glWindowPos2dARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble arg0 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble arg1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glWindowPos2dARB(arg0, arg1);
   return 0;
}

/*
//void glWindowPos2dvARB (const GLdouble *);
static AbstractQoreNode *f_glWindowPos2dvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* arg0 = p;
   glWindowPos2dvARB(arg0);
   return 0;
}
*/

//void glWindowPos2fARB (GLfloat, GLfloat);
static AbstractQoreNode *f_glWindowPos2fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat arg0 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat arg1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glWindowPos2fARB(arg0, arg1);
   return 0;
}

/*
//void glWindowPos2fvARB (const GLfloat *);
static AbstractQoreNode *f_glWindowPos2fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* arg0 = p;
   glWindowPos2fvARB(arg0);
   return 0;
}
*/

//void glWindowPos2iARB (GLint, GLint);
static AbstractQoreNode *f_glWindowPos2iARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   glWindowPos2iARB(arg0, arg1);
   return 0;
}

/*
//void glWindowPos2ivARB (const GLint *);
static AbstractQoreNode *f_glWindowPos2ivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* arg0 = p;
   glWindowPos2ivARB(arg0);
   return 0;
}
*/

//void glWindowPos2sARB (GLshort, GLshort);
static AbstractQoreNode *f_glWindowPos2sARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort arg0 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort arg1 = (GLshort)(p ? p->getAsInt() : 0);
   glWindowPos2sARB(arg0, arg1);
   return 0;
}

/*
//void glWindowPos2svARB (const GLshort *);
static AbstractQoreNode *f_glWindowPos2svARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* arg0 = p;
   glWindowPos2svARB(arg0);
   return 0;
}
*/

//void glWindowPos3dARB (GLdouble, GLdouble, GLdouble);
static AbstractQoreNode *f_glWindowPos3dARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble arg0 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble arg1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble arg2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glWindowPos3dARB(arg0, arg1, arg2);
   return 0;
}

/*
//void glWindowPos3dvARB (const GLdouble *);
static AbstractQoreNode *f_glWindowPos3dvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* arg0 = p;
   glWindowPos3dvARB(arg0);
   return 0;
}
*/

//void glWindowPos3fARB (GLfloat, GLfloat, GLfloat);
static AbstractQoreNode *f_glWindowPos3fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat arg0 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat arg1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat arg2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glWindowPos3fARB(arg0, arg1, arg2);
   return 0;
}

/*
//void glWindowPos3fvARB (const GLfloat *);
static AbstractQoreNode *f_glWindowPos3fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* arg0 = p;
   glWindowPos3fvARB(arg0);
   return 0;
}
*/

//void glWindowPos3iARB (GLint, GLint, GLint);
static AbstractQoreNode *f_glWindowPos3iARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int arg2 = p ? p->getAsInt() : 0;
   glWindowPos3iARB(arg0, arg1, arg2);
   return 0;
}

/*
//void glWindowPos3ivARB (const GLint *);
static AbstractQoreNode *f_glWindowPos3ivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* arg0 = p;
   glWindowPos3ivARB(arg0);
   return 0;
}
*/

//void glWindowPos3sARB (GLshort, GLshort, GLshort);
static AbstractQoreNode *f_glWindowPos3sARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort arg0 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort arg1 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort arg2 = (GLshort)(p ? p->getAsInt() : 0);
   glWindowPos3sARB(arg0, arg1, arg2);
   return 0;
}

/*
//void glWindowPos3svARB (const GLshort *);
static AbstractQoreNode *f_glWindowPos3svARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* arg0 = p;
   glWindowPos3svARB(arg0);
   return 0;
}
*/

/*
//void glGenQueriesARB (GLsizei n, GLuint *ids);
static AbstractQoreNode *f_glGenQueriesARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* ids = p;
   glGenQueriesARB(n, ids);
   return 0;
}
*/

/*
//void glDeleteQueriesARB (GLsizei n, const GLuint *ids);
static AbstractQoreNode *f_glDeleteQueriesARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* ids = p;
   glDeleteQueriesARB(n, ids);
   return 0;
}
*/

//GLboolean glIsQueryARB (GLuint id);
static AbstractQoreNode *f_glIsQueryARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned id = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsQueryARB(id));
}

//void glBeginQueryARB (GLenum target, GLuint id);
static AbstractQoreNode *f_glBeginQueryARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned id = p ? p->getAsBigInt() : 0;
   glBeginQueryARB(target, id);
   return 0;
}

//void glEndQueryARB (GLenum target);
static AbstractQoreNode *f_glEndQueryARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   glEndQueryARB(target);
   return 0;
}

/*
//void glGetQueryivARB (GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetQueryivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetQueryivARB(target, pname, params);
   return 0;
}
*/

/*
//void glGetQueryObjectivARB (GLuint id, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetQueryObjectivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned id = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetQueryObjectivARB(id, pname, params);
   return 0;
}
*/

/*
//void glGetQueryObjectuivARB (GLuint id, GLenum pname, GLuint *params);
static AbstractQoreNode *f_glGetQueryObjectuivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned id = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLuint* params = p;
   glGetQueryObjectuivARB(id, pname, params);
   return 0;
}
*/

//void glPointParameterfARB (GLenum pname, GLfloat param);
static AbstractQoreNode *f_glPointParameterfARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glPointParameterfARB(pname, param);
   return 0;
}

/*
//void glPointParameterfvARB (GLenum pname, const GLfloat *params);
static AbstractQoreNode *f_glPointParameterfvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLfloat* params = p;
   glPointParameterfvARB(pname, params);
   return 0;
}
*/

//void glBindProgramARB (GLenum target, GLuint program);
static AbstractQoreNode *f_glBindProgramARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned program = p ? p->getAsBigInt() : 0;
   glBindProgramARB(target, program);
   return 0;
}

/*
//void glDeleteProgramsARB (GLsizei n, const GLuint *programs);
static AbstractQoreNode *f_glDeleteProgramsARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* programs = p;
   glDeleteProgramsARB(n, programs);
   return 0;
}
*/

//void glGenProgramsARB (GLsizei n, GLuint *programs);
static AbstractQoreNode *f_glGenProgramsARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);

   GLuint programs[n];
   glGenProgramsARB(n, programs);

   if (n == 1)
      return new QoreBigIntNode(programs[0]);

   QoreListNode *l = new QoreListNode;
   for (int i = 0; i < n; ++i)
      l->push(new QoreBigIntNode(programs[i]));

   return l;
}

//GLboolean glIsProgramARB (GLuint program);
static AbstractQoreNode *f_glIsProgramARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsProgramARB(program));
}

//void glProgramEnvParameter4dARB (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
static AbstractQoreNode *f_glProgramEnvParameter4dARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 5);
   GLdouble w = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glProgramEnvParameter4dARB(target, index, x, y, z, w);
   return 0;
}

/*
//void glProgramEnvParameter4dvARB (GLenum target, GLuint index, const GLdouble *params);
static AbstractQoreNode *f_glProgramEnvParameter4dvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLdouble* params = p;
   glProgramEnvParameter4dvARB(target, index, params);
   return 0;
}
*/

//void glProgramEnvParameter4fARB (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
static AbstractQoreNode *f_glProgramEnvParameter4fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 5);
   GLfloat w = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glProgramEnvParameter4fARB(target, index, x, y, z, w);
   return 0;
}

/*
//void glProgramEnvParameter4fvARB (GLenum target, GLuint index, const GLfloat *params);
static AbstractQoreNode *f_glProgramEnvParameter4fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glProgramEnvParameter4fvARB(target, index, params);
   return 0;
}
*/

//void glProgramLocalParameter4dARB (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
static AbstractQoreNode *f_glProgramLocalParameter4dARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 5);
   GLdouble w = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glProgramLocalParameter4dARB(target, index, x, y, z, w);
   return 0;
}

/*
//void glProgramLocalParameter4dvARB (GLenum target, GLuint index, const GLdouble *params);
static AbstractQoreNode *f_glProgramLocalParameter4dvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLdouble* params = p;
   glProgramLocalParameter4dvARB(target, index, params);
   return 0;
}
*/

//void glProgramLocalParameter4fARB (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
static AbstractQoreNode *f_glProgramLocalParameter4fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 5);
   GLfloat w = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glProgramLocalParameter4fARB(target, index, x, y, z, w);
   return 0;
}

/*
//void glProgramLocalParameter4fvARB (GLenum target, GLuint index, const GLfloat *params);
static AbstractQoreNode *f_glProgramLocalParameter4fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glProgramLocalParameter4fvARB(target, index, params);
   return 0;
}
*/

/*
//void glGetProgramEnvParameterdvARB (GLenum target, GLuint index, GLdouble *params);
static AbstractQoreNode *f_glGetProgramEnvParameterdvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLdouble* params = p;
   glGetProgramEnvParameterdvARB(target, index, params);
   return 0;
}
*/

/*
//void glGetProgramEnvParameterfvARB (GLenum target, GLuint index, GLfloat *params);
static AbstractQoreNode *f_glGetProgramEnvParameterfvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetProgramEnvParameterfvARB(target, index, params);
   return 0;
}
*/

/*
//void glProgramEnvParameters4fvEXT (GLenum target, GLuint index, GLsizei count, const GLfloat *params);
static AbstractQoreNode *f_glProgramEnvParameters4fvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLfloat* params = p;
   glProgramEnvParameters4fvEXT(target, index, count, params);
   return 0;
}
*/

/*
//void glProgramLocalParameters4fvEXT (GLenum target, GLuint index, GLsizei count, const GLfloat *params);
static AbstractQoreNode *f_glProgramLocalParameters4fvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLfloat* params = p;
   glProgramLocalParameters4fvEXT(target, index, count, params);
   return 0;
}
*/

/*
//void glGetProgramLocalParameterdvARB (GLenum target, GLuint index, GLdouble *params);
static AbstractQoreNode *f_glGetProgramLocalParameterdvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLdouble* params = p;
   glGetProgramLocalParameterdvARB(target, index, params);
   return 0;
}
*/

/*
//void glGetProgramLocalParameterfvARB (GLenum target, GLuint index, GLfloat *params);
static AbstractQoreNode *f_glGetProgramLocalParameterfvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetProgramLocalParameterfvARB(target, index, params);
   return 0;
}
*/

//void glProgramStringARB (GLenum target, GLenum format, GLsizei len, const GLvoid *string);
static AbstractQoreNode *f_glProgramStringARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum format = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);

   if (!p || (p->getType() != NT_STRING && p->getType() != NT_BINARY)) {
      xsink->raiseException("GLPROGRAMSTRINGARG-ERROR", "no binary or string argument passed to glProgramStringARB() in the third argument (type passed=%s)", p ? p->getTypeName() : "NOTHING");
      return 0;
   }

   GLsizei len;
   const GLvoid *string;
   if (p->getType() == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(p);
      len = str->strlen();
      string = (const GLvoid *)str->getBuffer();
   }
   else {
      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(p);
      len = b->size();
      string = (const GLvoid *)b->getPtr();
   }

   glProgramStringARB(target, format, len, string);
   return 0;
}

/*
//void glGetProgramStringARB (GLenum target, GLenum pname, GLvoid *string);
static AbstractQoreNode *f_glGetProgramStringARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLvoid* string = p;
   glGetProgramStringARB(target, pname, string);
   return 0;
}
*/

/*
//void glGetProgramivARB (GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetProgramivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetProgramivARB(target, pname, params);
   return 0;
}
*/

//void glVertexAttrib1dARB (GLuint index, GLdouble x);
static AbstractQoreNode *f_glVertexAttrib1dARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib1dARB(index, x);
   return 0;
}

/*
//void glVertexAttrib1dvARB (GLuint index, const GLdouble *v);
static AbstractQoreNode *f_glVertexAttrib1dvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLdouble* v = p;
   glVertexAttrib1dvARB(index, v);
   return 0;
}
*/

//void glVertexAttrib1fARB (GLuint index, GLfloat x);
static AbstractQoreNode *f_glVertexAttrib1fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib1fARB(index, x);
   return 0;
}

/*
//void glVertexAttrib1fvARB (GLuint index, const GLfloat *v);
static AbstractQoreNode *f_glVertexAttrib1fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLfloat* v = p;
   glVertexAttrib1fvARB(index, v);
   return 0;
}
*/

//void glVertexAttrib1sARB (GLuint index, GLshort x);
static AbstractQoreNode *f_glVertexAttrib1sARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   glVertexAttrib1sARB(index, x);
   return 0;
}

/*
//void glVertexAttrib1svARB (GLuint index, const GLshort *v);
static AbstractQoreNode *f_glVertexAttrib1svARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glVertexAttrib1svARB(index, v);
   return 0;
}
*/

//void glVertexAttrib2dARB (GLuint index, GLdouble x, GLdouble y);
static AbstractQoreNode *f_glVertexAttrib2dARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib2dARB(index, x, y);
   return 0;
}

/*
//void glVertexAttrib2dvARB (GLuint index, const GLdouble *v);
static AbstractQoreNode *f_glVertexAttrib2dvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLdouble* v = p;
   glVertexAttrib2dvARB(index, v);
   return 0;
}
*/

//void glVertexAttrib2fARB (GLuint index, GLfloat x, GLfloat y);
static AbstractQoreNode *f_glVertexAttrib2fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib2fARB(index, x, y);
   return 0;
}

/*
//void glVertexAttrib2fvARB (GLuint index, const GLfloat *v);
static AbstractQoreNode *f_glVertexAttrib2fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLfloat* v = p;
   glVertexAttrib2fvARB(index, v);
   return 0;
}
*/

//void glVertexAttrib2sARB (GLuint index, GLshort x, GLshort y);
static AbstractQoreNode *f_glVertexAttrib2sARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   glVertexAttrib2sARB(index, x, y);
   return 0;
}

/*
//void glVertexAttrib2svARB (GLuint index, const GLshort *v);
static AbstractQoreNode *f_glVertexAttrib2svARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glVertexAttrib2svARB(index, v);
   return 0;
}
*/

//void glVertexAttrib3dARB (GLuint index, GLdouble x, GLdouble y, GLdouble z);
static AbstractQoreNode *f_glVertexAttrib3dARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib3dARB(index, x, y, z);
   return 0;
}

/*
//void glVertexAttrib3dvARB (GLuint index, const GLdouble *v);
static AbstractQoreNode *f_glVertexAttrib3dvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLdouble* v = p;
   glVertexAttrib3dvARB(index, v);
   return 0;
}
*/

//void glVertexAttrib3fARB (GLuint index, GLfloat x, GLfloat y, GLfloat z);
static AbstractQoreNode *f_glVertexAttrib3fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib3fARB(index, x, y, z);
   return 0;
}

/*
//void glVertexAttrib3fvARB (GLuint index, const GLfloat *v);
static AbstractQoreNode *f_glVertexAttrib3fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLfloat* v = p;
   glVertexAttrib3fvARB(index, v);
   return 0;
}
*/

//void glVertexAttrib3sARB (GLuint index, GLshort x, GLshort y, GLshort z);
static AbstractQoreNode *f_glVertexAttrib3sARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLshort z = (GLshort)(p ? p->getAsInt() : 0);
   glVertexAttrib3sARB(index, x, y, z);
   return 0;
}

/*
//void glVertexAttrib3svARB (GLuint index, const GLshort *v);
static AbstractQoreNode *f_glVertexAttrib3svARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glVertexAttrib3svARB(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4NbvARB (GLuint index, const GLbyte *v);
static AbstractQoreNode *f_glVertexAttrib4NbvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLbyte* v = p;
   glVertexAttrib4NbvARB(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4NivARB (GLuint index, const GLint *v);
static AbstractQoreNode *f_glVertexAttrib4NivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLint* v = p;
   glVertexAttrib4NivARB(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4NsvARB (GLuint index, const GLshort *v);
static AbstractQoreNode *f_glVertexAttrib4NsvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glVertexAttrib4NsvARB(index, v);
   return 0;
}
*/

//void glVertexAttrib4NubARB (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
static AbstractQoreNode *f_glVertexAttrib4NubARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLubyte x = (GLubyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLubyte y = (GLubyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLubyte z = (GLubyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLubyte w = (GLubyte)(p ? p->getAsInt() : 0);
   glVertexAttrib4NubARB(index, x, y, z, w);
   return 0;
}

/*
//void glVertexAttrib4NubvARB (GLuint index, const GLubyte *v);
static AbstractQoreNode *f_glVertexAttrib4NubvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLubyte* v = p;
   glVertexAttrib4NubvARB(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4NuivARB (GLuint index, const GLuint *v);
static AbstractQoreNode *f_glVertexAttrib4NuivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLuint* v = p;
   glVertexAttrib4NuivARB(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4NusvARB (GLuint index, const GLushort *v);
static AbstractQoreNode *f_glVertexAttrib4NusvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLushort* v = p;
   glVertexAttrib4NusvARB(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4bvARB (GLuint index, const GLbyte *v);
static AbstractQoreNode *f_glVertexAttrib4bvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLbyte* v = p;
   glVertexAttrib4bvARB(index, v);
   return 0;
}
*/

//void glVertexAttrib4dARB (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
static AbstractQoreNode *f_glVertexAttrib4dARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLdouble w = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib4dARB(index, x, y, z, w);
   return 0;
}

/*
//void glVertexAttrib4dvARB (GLuint index, const GLdouble *v);
static AbstractQoreNode *f_glVertexAttrib4dvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLdouble* v = p;
   glVertexAttrib4dvARB(index, v);
   return 0;
}
*/

//void glVertexAttrib4fARB (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
static AbstractQoreNode *f_glVertexAttrib4fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLfloat w = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib4fARB(index, x, y, z, w);
   return 0;
}

/*
//void glVertexAttrib4fvARB (GLuint index, const GLfloat *v);
static AbstractQoreNode *f_glVertexAttrib4fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLfloat* v = p;
   glVertexAttrib4fvARB(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4ivARB (GLuint index, const GLint *v);
static AbstractQoreNode *f_glVertexAttrib4ivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLint* v = p;
   glVertexAttrib4ivARB(index, v);
   return 0;
}
*/

//void glVertexAttrib4sARB (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
static AbstractQoreNode *f_glVertexAttrib4sARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLshort z = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLshort w = (GLshort)(p ? p->getAsInt() : 0);
   glVertexAttrib4sARB(index, x, y, z, w);
   return 0;
}

/*
//void glVertexAttrib4svARB (GLuint index, const GLshort *v);
static AbstractQoreNode *f_glVertexAttrib4svARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glVertexAttrib4svARB(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4ubvARB (GLuint index, const GLubyte *v);
static AbstractQoreNode *f_glVertexAttrib4ubvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLubyte* v = p;
   glVertexAttrib4ubvARB(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4uivARB (GLuint index, const GLuint *v);
static AbstractQoreNode *f_glVertexAttrib4uivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLuint* v = p;
   glVertexAttrib4uivARB(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4usvARB (GLuint index, const GLushort *v);
static AbstractQoreNode *f_glVertexAttrib4usvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLushort* v = p;
   glVertexAttrib4usvARB(index, v);
   return 0;
}
*/

/*
//void glVertexAttribPointerARB (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
static AbstractQoreNode *f_glVertexAttribPointerARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   int size = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLboolean normalized = (GLboolean)(p ? p->getAsBool() : false);
   p = get_param(params, 4);
   GLsizei stride = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   ??? GLvoid* pointer = p;
   glVertexAttribPointerARB(index, size, type, normalized, stride, pointer);
   return 0;
}
*/

//void glDisableVertexAttribArrayARB (GLuint index);
static AbstractQoreNode *f_glDisableVertexAttribArrayARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   glDisableVertexAttribArrayARB(index);
   return 0;
}

//void glEnableVertexAttribArrayARB (GLuint index);
static AbstractQoreNode *f_glEnableVertexAttribArrayARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   glEnableVertexAttribArrayARB(index);
   return 0;
}

/*
//void glGetVertexAttribPointervARB (GLuint index, GLenum pname, GLvoid **pointer);
static AbstractQoreNode *f_glGetVertexAttribPointervARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLvoid* *pointer arg2 = p;
   glGetVertexAttribPointervARB(index, pname, arg2);
   return 0;
}
*/

/*
//void glGetVertexAttribdvARB (GLuint index, GLenum pname, GLdouble *params);
static AbstractQoreNode *f_glGetVertexAttribdvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLdouble* params = p;
   glGetVertexAttribdvARB(index, pname, params);
   return 0;
}
*/

/*
//void glGetVertexAttribfvARB (GLuint index, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetVertexAttribfvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetVertexAttribfvARB(index, pname, params);
   return 0;
}
*/

/*
//void glGetVertexAttribivARB (GLuint index, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetVertexAttribivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetVertexAttribivARB(index, pname, params);
   return 0;
}
*/

//void glDeleteObjectARB (GLhandleARB obj);
static AbstractQoreNode *f_glDeleteObjectARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB obj = (GLhandleARB)(p ? p->getAsInt() : 0);
   glDeleteObjectARB(obj);
   return 0;
}

/*
//GLhandleARB glGetHandleARB (GLenum pname);
static AbstractQoreNode *f_glGetHandleARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   ??? return new QoreBigIntNode(glGetHandleARB(pname));
}
*/

//void glDetachObjectARB (GLhandleARB containerObj, GLhandleARB attachedObj);
static AbstractQoreNode *f_glDetachObjectARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB containerObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLhandleARB attachedObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   glDetachObjectARB(containerObj, attachedObj);
   return 0;
}

/*
//GLhandleARB glCreateShaderObjectARB (GLenum shaderType);
static AbstractQoreNode *f_glCreateShaderObjectARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum shaderType = (GLenum)(p ? p->getAsInt() : 0);
   ??? return new QoreBigIntNode(glCreateShaderObjectARB(shaderType));
}
*/

/*
//void glShaderSourceARB (GLhandleARB shaderObj, GLsizei count, const GLcharARB **string, const GLint *length);
static AbstractQoreNode *f_glShaderSourceARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB shaderObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLcharARB* *string arg2 = p;
   p = get_param(params, 3);
   ??? GLint* length = p;
   glShaderSourceARB(shaderObj, count, arg2, length);
   return 0;
}
*/

//void glCompileShaderARB (GLhandleARB shaderObj);
static AbstractQoreNode *f_glCompileShaderARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB shaderObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   glCompileShaderARB(shaderObj);
   return 0;
}

/*
//GLhandleARB glCreateProgramObjectARB (void);
static AbstractQoreNode *f_glCreateProgramObjectARB(const QoreListNode *params, ExceptionSink *xsink)
{
   ??? return new QoreBigIntNode(glCreateProgramObjectARB());
}
*/

//void glAttachObjectARB (GLhandleARB containerObj, GLhandleARB obj);
static AbstractQoreNode *f_glAttachObjectARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB containerObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLhandleARB obj = (GLhandleARB)(p ? p->getAsInt() : 0);
   glAttachObjectARB(containerObj, obj);
   return 0;
}

//void glLinkProgramARB (GLhandleARB programObj);
static AbstractQoreNode *f_glLinkProgramARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB programObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   glLinkProgramARB(programObj);
   return 0;
}

//void glUseProgramObjectARB (GLhandleARB programObj);
static AbstractQoreNode *f_glUseProgramObjectARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB programObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   glUseProgramObjectARB(programObj);
   return 0;
}

//void glValidateProgramARB (GLhandleARB programObj);
static AbstractQoreNode *f_glValidateProgramARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB programObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   glValidateProgramARB(programObj);
   return 0;
}

//void glUniform1fARB (GLint location, GLfloat v0);
static AbstractQoreNode *f_glUniform1fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLfloat v0 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glUniform1fARB(location, v0);
   return 0;
}

//void glUniform2fARB (GLint location, GLfloat v0, GLfloat v1);
static AbstractQoreNode *f_glUniform2fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLfloat v0 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat v1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glUniform2fARB(location, v0, v1);
   return 0;
}

//void glUniform3fARB (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
static AbstractQoreNode *f_glUniform3fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLfloat v0 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat v1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat v2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glUniform3fARB(location, v0, v1, v2);
   return 0;
}

//void glUniform4fARB (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
static AbstractQoreNode *f_glUniform4fARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLfloat v0 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat v1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat v2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLfloat v3 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glUniform4fARB(location, v0, v1, v2, v3);
   return 0;
}

//void glUniform1iARB (GLint location, GLint v0);
static AbstractQoreNode *f_glUniform1iARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int v0 = p ? p->getAsInt() : 0;
   glUniform1iARB(location, v0);
   return 0;
}

//void glUniform2iARB (GLint location, GLint v0, GLint v1);
static AbstractQoreNode *f_glUniform2iARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int v0 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int v1 = p ? p->getAsInt() : 0;
   glUniform2iARB(location, v0, v1);
   return 0;
}

//void glUniform3iARB (GLint location, GLint v0, GLint v1, GLint v2);
static AbstractQoreNode *f_glUniform3iARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int v0 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int v1 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int v2 = p ? p->getAsInt() : 0;
   glUniform3iARB(location, v0, v1, v2);
   return 0;
}

//void glUniform4iARB (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
static AbstractQoreNode *f_glUniform4iARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int v0 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int v1 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int v2 = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int v3 = p ? p->getAsInt() : 0;
   glUniform4iARB(location, v0, v1, v2, v3);
   return 0;
}

/*
//void glUniform1fvARB (GLint location, GLsizei count, const GLfloat *value);
static AbstractQoreNode *f_glUniform1fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* value = p;
   glUniform1fvARB(location, count, value);
   return 0;
}
*/

/*
//void glUniform2fvARB (GLint location, GLsizei count, const GLfloat *value);
static AbstractQoreNode *f_glUniform2fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* value = p;
   glUniform2fvARB(location, count, value);
   return 0;
}
*/

/*
//void glUniform3fvARB (GLint location, GLsizei count, const GLfloat *value);
static AbstractQoreNode *f_glUniform3fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* value = p;
   glUniform3fvARB(location, count, value);
   return 0;
}
*/

/*
//void glUniform4fvARB (GLint location, GLsizei count, const GLfloat *value);
static AbstractQoreNode *f_glUniform4fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* value = p;
   glUniform4fvARB(location, count, value);
   return 0;
}
*/

/*
//void glUniform1ivARB (GLint location, GLsizei count, const GLint *value);
static AbstractQoreNode *f_glUniform1ivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* value = p;
   glUniform1ivARB(location, count, value);
   return 0;
}
*/

/*
//void glUniform2ivARB (GLint location, GLsizei count, const GLint *value);
static AbstractQoreNode *f_glUniform2ivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* value = p;
   glUniform2ivARB(location, count, value);
   return 0;
}
*/

/*
//void glUniform3ivARB (GLint location, GLsizei count, const GLint *value);
static AbstractQoreNode *f_glUniform3ivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* value = p;
   glUniform3ivARB(location, count, value);
   return 0;
}
*/

/*
//void glUniform4ivARB (GLint location, GLsizei count, const GLint *value);
static AbstractQoreNode *f_glUniform4ivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* value = p;
   glUniform4ivARB(location, count, value);
   return 0;
}
*/

/*
//void glUniformMatrix2fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static AbstractQoreNode *f_glUniformMatrix2fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean transpose = (GLboolean)(p ? p->getAsBool() : false);
   p = get_param(params, 3);
   ??? GLfloat* value = p;
   glUniformMatrix2fvARB(location, count, transpose, value);
   return 0;
}
*/

/*
//void glUniformMatrix3fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static AbstractQoreNode *f_glUniformMatrix3fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean transpose = (GLboolean)(p ? p->getAsBool() : false);
   p = get_param(params, 3);
   ??? GLfloat* value = p;
   glUniformMatrix3fvARB(location, count, transpose, value);
   return 0;
}
*/

/*
//void glUniformMatrix4fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static AbstractQoreNode *f_glUniformMatrix4fvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean transpose = (GLboolean)(p ? p->getAsBool() : false);
   p = get_param(params, 3);
   ??? GLfloat* value = p;
   glUniformMatrix4fvARB(location, count, transpose, value);
   return 0;
}
*/

/*
//void glGetObjectParameterfvARB (GLhandleARB obj, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetObjectParameterfvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB obj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetObjectParameterfvARB(obj, pname, params);
   return 0;
}
*/

/*
//void glGetObjectParameterivARB (GLhandleARB obj, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetObjectParameterivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB obj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetObjectParameterivARB(obj, pname, params);
   return 0;
}
*/

/*
//void glGetInfoLogARB (GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);
static AbstractQoreNode *f_glGetInfoLogARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB obj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei maxLength = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLsizei* length = p;
   p = get_param(params, 3);
   ??? GLcharARB* infoLog = p;
   glGetInfoLogARB(obj, maxLength, length, infoLog);
   return 0;
}
*/

/*
//void glGetAttachedObjectsARB (GLhandleARB containerObj, GLsizei maxCount, GLsizei *count, GLhandleARB *obj);
static AbstractQoreNode *f_glGetAttachedObjectsARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB containerObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei maxCount = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLsizei* count = p;
   p = get_param(params, 3);
   ??? GLhandleARB* obj = p;
   glGetAttachedObjectsARB(containerObj, maxCount, count, obj);
   return 0;
}
*/

/*
//GLint glGetUniformLocationARB (GLhandleARB programObj, const GLcharARB *name);
static AbstractQoreNode *f_glGetUniformLocationARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB programObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLcharARB* name = p;
   return new QoreBigIntNode(glGetUniformLocationARB(programObj, name));
}
*/

/*
//void glGetActiveUniformARB (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
static AbstractQoreNode *f_glGetActiveUniformARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB programObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLsizei maxLength = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLsizei* length = p;
   p = get_param(params, 4);
   ??? GLint* size = p;
   p = get_param(params, 5);
   ??? GLenum* type = p;
   p = get_param(params, 6);
   ??? GLcharARB* name = p;
   glGetActiveUniformARB(programObj, index, maxLength, length, size, type, name);
   return 0;
}
*/

/*
//void glGetUniformfvARB (GLhandleARB programObj, GLint location, GLfloat *params);
static AbstractQoreNode *f_glGetUniformfvARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB programObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetUniformfvARB(programObj, location, params);
   return 0;
}
*/

/*
//void glGetUniformivARB (GLhandleARB programObj, GLint location, GLint *params);
static AbstractQoreNode *f_glGetUniformivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB programObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetUniformivARB(programObj, location, params);
   return 0;
}
*/

/*
//void glGetShaderSourceARB (GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *source);
static AbstractQoreNode *f_glGetShaderSourceARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB obj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei maxLength = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLsizei* length = p;
   p = get_param(params, 3);
   ??? GLcharARB* source = p;
   glGetShaderSourceARB(obj, maxLength, length, source);
   return 0;
}
*/

/*
//void glBindAttribLocationARB (GLhandleARB programObj, GLuint index, const GLcharARB *name);
static AbstractQoreNode *f_glBindAttribLocationARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB programObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLcharARB* name = p;
   glBindAttribLocationARB(programObj, index, name);
   return 0;
}
*/

/*
//void glGetActiveAttribARB (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
static AbstractQoreNode *f_glGetActiveAttribARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB programObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLsizei maxLength = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLsizei* length = p;
   p = get_param(params, 4);
   ??? GLint* size = p;
   p = get_param(params, 5);
   ??? GLenum* type = p;
   p = get_param(params, 6);
   ??? GLcharARB* name = p;
   glGetActiveAttribARB(programObj, index, maxLength, length, size, type, name);
   return 0;
}
*/

/*
//GLint glGetAttribLocationARB (GLhandleARB programObj, const GLcharARB *name);
static AbstractQoreNode *f_glGetAttribLocationARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLhandleARB programObj = (GLhandleARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLcharARB* name = p;
   return new QoreBigIntNode(glGetAttribLocationARB(programObj, name));
}
*/

//void glBindBufferARB (GLenum target, GLuint buffer);
static AbstractQoreNode *f_glBindBufferARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned buffer = p ? p->getAsBigInt() : 0;
   glBindBufferARB(target, buffer);
   return 0;
}

/*
//void glDeleteBuffersARB (GLsizei n, const GLuint *buffers);
static AbstractQoreNode *f_glDeleteBuffersARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* buffers = p;
   glDeleteBuffersARB(n, buffers);
   return 0;
}
*/

/*
//void glGenBuffersARB (GLsizei n, GLuint *buffers);
static AbstractQoreNode *f_glGenBuffersARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* buffers = p;
   glGenBuffersARB(n, buffers);
   return 0;
}
*/

//GLboolean glIsBufferARB (GLuint buffer);
static AbstractQoreNode *f_glIsBufferARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned buffer = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsBufferARB(buffer));
}

/*
//void glBufferDataARB (GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
static AbstractQoreNode *f_glBufferDataARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizeiptrARB size = (GLsizeiptrARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLvoid* data = p;
   p = get_param(params, 3);
   GLenum usage = (GLenum)(p ? p->getAsInt() : 0);
   glBufferDataARB(target, size, data, usage);
   return 0;
}
*/

/*
//void glBufferSubDataARB (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data);
static AbstractQoreNode *f_glBufferSubDataARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLintptrARB offset = (GLintptrARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizeiptrARB size = (GLsizeiptrARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLvoid* data = p;
   glBufferSubDataARB(target, offset, size, data);
   return 0;
}
*/

/*
//void glGetBufferSubDataARB (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid *data);
static AbstractQoreNode *f_glGetBufferSubDataARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLintptrARB offset = (GLintptrARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizeiptrARB size = (GLsizeiptrARB)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLvoid* data = p;
   glGetBufferSubDataARB(target, offset, size, data);
   return 0;
}
*/

/*
//GLvoid *glMapBufferARB (GLenum target, GLenum access);
static AbstractQoreNode *f_glMapBufferARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum access = (GLenum)(p ? p->getAsInt() : 0);
   ??? return new QoreBigIntNode(glMapBufferARB(target, access));
}
*/

//GLboolean glUnmapBufferARB (GLenum target);
static AbstractQoreNode *f_glUnmapBufferARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   return get_bool_node(glUnmapBufferARB(target));
}

/*
//void glGetBufferParameterivARB (GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetBufferParameterivARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetBufferParameterivARB(target, pname, params);
   return 0;
}
*/

/*
//void glGetBufferPointervARB (GLenum target, GLenum pname, GLvoid **params);
static AbstractQoreNode *f_glGetBufferPointervARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLvoid* *params arg2 = p;
   glGetBufferPointervARB(target, pname, arg2);
   return 0;
}
*/

/*
//void glDrawBuffersARB (GLsizei n, const GLenum *bufs);
static AbstractQoreNode *f_glDrawBuffersARB(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLenum* bufs = p;
   glDrawBuffersARB(n, bufs);
   return 0;
}
*/

//void glBlendColorEXT (GLclampf, GLclampf, GLclampf, GLclampf);
static AbstractQoreNode *f_glBlendColorEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLclampf arg0 = (GLclampf)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLclampf arg1 = (GLclampf)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLclampf arg2 = (GLclampf)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLclampf arg3 = (GLclampf)(p ? p->getAsFloat() : 0.0);
   glBlendColorEXT(arg0, arg1, arg2, arg3);
   return 0;
}

//void glBlendEquationEXT (GLenum);
static AbstractQoreNode *f_glBlendEquationEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   glBlendEquationEXT(arg0);
   return 0;
}

/*
//void glColorTableEXT (GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid *);
static AbstractQoreNode *f_glColorTableEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizei arg2 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum arg3 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLenum arg4 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   ??? GLvoid* arg5 = p;
   glColorTableEXT(arg0, arg1, arg2, arg3, arg4, arg5);
   return 0;
}
*/

/*
//void glColorSubTableEXT (GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
static AbstractQoreNode *f_glColorSubTableEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei arg1 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizei arg2 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum arg3 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLenum arg4 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   ??? GLvoid* arg5 = p;
   glColorSubTableEXT(arg0, arg1, arg2, arg3, arg4, arg5);
   return 0;
}
*/

/*
//void glGetColorTableEXT (GLenum, GLenum, GLenum, GLvoid *);
static AbstractQoreNode *f_glGetColorTableEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLvoid* arg3 = p;
   glGetColorTableEXT(arg0, arg1, arg2, arg3);
   return 0;
}
*/

/*
//void glGetColorTableParameterivEXT (GLenum, GLenum, GLint *);
static AbstractQoreNode *f_glGetColorTableParameterivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* arg2 = p;
   glGetColorTableParameterivEXT(arg0, arg1, arg2);
   return 0;
}
*/

/*
//void glGetColorTableParameterfvEXT (GLenum, GLenum, GLfloat *);
static AbstractQoreNode *f_glGetColorTableParameterfvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* arg2 = p;
   glGetColorTableParameterfvEXT(arg0, arg1, arg2);
   return 0;
}
*/

//void glLockArraysEXT (GLint, GLsizei);
static AbstractQoreNode *f_glLockArraysEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei arg1 = (GLsizei)(p ? p->getAsInt() : 0);
   glLockArraysEXT(arg0, arg1);
   return 0;
}

//void glUnlockArraysEXT (void);
static AbstractQoreNode *f_glUnlockArraysEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   glUnlockArraysEXT();
   return 0;
}

/*
//void glDrawRangeElementsEXT (GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid *);
static AbstractQoreNode *f_glDrawRangeElementsEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned arg1 = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned arg2 = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   GLsizei arg3 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLenum arg4 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   ??? GLvoid* arg5 = p;
   glDrawRangeElementsEXT(arg0, arg1, arg2, arg3, arg4, arg5);
   return 0;
}
*/

//void glSecondaryColor3bEXT (GLbyte, GLbyte, GLbyte);
static AbstractQoreNode *f_glSecondaryColor3bEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLbyte arg0 = (GLbyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLbyte arg1 = (GLbyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLbyte arg2 = (GLbyte)(p ? p->getAsInt() : 0);
   glSecondaryColor3bEXT(arg0, arg1, arg2);
   return 0;
}

/*
//void glSecondaryColor3bvEXT (const GLbyte *);
static AbstractQoreNode *f_glSecondaryColor3bvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLbyte* arg0 = p;
   glSecondaryColor3bvEXT(arg0);
   return 0;
}
*/

//void glSecondaryColor3dEXT (GLdouble, GLdouble, GLdouble);
static AbstractQoreNode *f_glSecondaryColor3dEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble arg0 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble arg1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble arg2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glSecondaryColor3dEXT(arg0, arg1, arg2);
   return 0;
}

/*
//void glSecondaryColor3dvEXT (const GLdouble *);
static AbstractQoreNode *f_glSecondaryColor3dvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* arg0 = p;
   glSecondaryColor3dvEXT(arg0);
   return 0;
}
*/

//void glSecondaryColor3fEXT (GLfloat, GLfloat, GLfloat);
static AbstractQoreNode *f_glSecondaryColor3fEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat arg0 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat arg1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat arg2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glSecondaryColor3fEXT(arg0, arg1, arg2);
   return 0;
}

/*
//void glSecondaryColor3fvEXT (const GLfloat *);
static AbstractQoreNode *f_glSecondaryColor3fvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* arg0 = p;
   glSecondaryColor3fvEXT(arg0);
   return 0;
}
*/

//void glSecondaryColor3iEXT (GLint, GLint, GLint);
static AbstractQoreNode *f_glSecondaryColor3iEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int arg2 = p ? p->getAsInt() : 0;
   glSecondaryColor3iEXT(arg0, arg1, arg2);
   return 0;
}

/*
//void glSecondaryColor3ivEXT (const GLint *);
static AbstractQoreNode *f_glSecondaryColor3ivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* arg0 = p;
   glSecondaryColor3ivEXT(arg0);
   return 0;
}
*/

//void glSecondaryColor3sEXT (GLshort, GLshort, GLshort);
static AbstractQoreNode *f_glSecondaryColor3sEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort arg0 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort arg1 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort arg2 = (GLshort)(p ? p->getAsInt() : 0);
   glSecondaryColor3sEXT(arg0, arg1, arg2);
   return 0;
}

/*
//void glSecondaryColor3svEXT (const GLshort *);
static AbstractQoreNode *f_glSecondaryColor3svEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* arg0 = p;
   glSecondaryColor3svEXT(arg0);
   return 0;
}
*/

//void glSecondaryColor3ubEXT (GLubyte, GLubyte, GLubyte);
static AbstractQoreNode *f_glSecondaryColor3ubEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLubyte arg0 = (GLubyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLubyte arg1 = (GLubyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLubyte arg2 = (GLubyte)(p ? p->getAsInt() : 0);
   glSecondaryColor3ubEXT(arg0, arg1, arg2);
   return 0;
}

/*
//void glSecondaryColor3ubvEXT (const GLubyte *);
static AbstractQoreNode *f_glSecondaryColor3ubvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLubyte* arg0 = p;
   glSecondaryColor3ubvEXT(arg0);
   return 0;
}
*/

//void glSecondaryColor3uiEXT (GLuint, GLuint, GLuint);
static AbstractQoreNode *f_glSecondaryColor3uiEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned arg0 = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned arg1 = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned arg2 = p ? p->getAsBigInt() : 0;
   glSecondaryColor3uiEXT(arg0, arg1, arg2);
   return 0;
}

/*
//void glSecondaryColor3uivEXT (const GLuint *);
static AbstractQoreNode *f_glSecondaryColor3uivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLuint* arg0 = p;
   glSecondaryColor3uivEXT(arg0);
   return 0;
}
*/

//void glSecondaryColor3usEXT (GLushort, GLushort, GLushort);
static AbstractQoreNode *f_glSecondaryColor3usEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLushort arg0 = (GLushort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLushort arg1 = (GLushort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLushort arg2 = (GLushort)(p ? p->getAsInt() : 0);
   glSecondaryColor3usEXT(arg0, arg1, arg2);
   return 0;
}

/*
//void glSecondaryColor3usvEXT (const GLushort *);
static AbstractQoreNode *f_glSecondaryColor3usvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLushort* arg0 = p;
   glSecondaryColor3usvEXT(arg0);
   return 0;
}
*/

/*
//void glSecondaryColorPointerEXT (GLint, GLenum, GLsizei, const GLvoid *);
static AbstractQoreNode *f_glSecondaryColorPointerEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int arg0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizei arg2 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLvoid* arg3 = p;
   glSecondaryColorPointerEXT(arg0, arg1, arg2, arg3);
   return 0;
}
*/

/*
//void glMultiDrawArraysEXT (GLenum, const GLint *, const GLsizei *, GLsizei);
static AbstractQoreNode *f_glMultiDrawArraysEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* arg1 = p;
   p = get_param(params, 2);
   ??? GLsizei* arg2 = p;
   p = get_param(params, 3);
   GLsizei arg3 = (GLsizei)(p ? p->getAsInt() : 0);
   glMultiDrawArraysEXT(arg0, arg1, arg2, arg3);
   return 0;
}
*/

/*
//void glMultiDrawElementsEXT (GLenum, const GLsizei *, GLenum, const GLvoid* *, GLsizei);
static AbstractQoreNode *f_glMultiDrawElementsEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLsizei* arg1 = p;
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLvoid** arg3 = p;
   p = get_param(params, 4);
   GLsizei arg4 = (GLsizei)(p ? p->getAsInt() : 0);
   glMultiDrawElementsEXT(arg0, arg1, arg2, arg3, arg4);
   return 0;
}
*/

//void glFogCoordfEXT (GLfloat);
static AbstractQoreNode *f_glFogCoordfEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat arg0 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glFogCoordfEXT(arg0);
   return 0;
}

/*
//void glFogCoordfvEXT (const GLfloat *);
static AbstractQoreNode *f_glFogCoordfvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* arg0 = p;
   glFogCoordfvEXT(arg0);
   return 0;
}
*/

//void glFogCoorddEXT (GLdouble);
static AbstractQoreNode *f_glFogCoorddEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble arg0 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glFogCoorddEXT(arg0);
   return 0;
}

/*
//void glFogCoorddvEXT (const GLdouble *);
static AbstractQoreNode *f_glFogCoorddvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* arg0 = p;
   glFogCoorddvEXT(arg0);
   return 0;
}
*/

/*
//void glFogCoordPointerEXT (GLenum, GLsizei, const GLvoid *);
static AbstractQoreNode *f_glFogCoordPointerEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei arg1 = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLvoid* arg2 = p;
   glFogCoordPointerEXT(arg0, arg1, arg2);
   return 0;
}
*/

//void glBlendFuncSeparateEXT (GLenum, GLenum, GLenum, GLenum);
static AbstractQoreNode *f_glBlendFuncSeparateEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum arg3 = (GLenum)(p ? p->getAsInt() : 0);
   glBlendFuncSeparateEXT(arg0, arg1, arg2, arg3);
   return 0;
}

//void glActiveStencilFaceEXT (GLenum face);
static AbstractQoreNode *f_glActiveStencilFaceEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum face = (GLenum)(p ? p->getAsInt() : 0);
   glActiveStencilFaceEXT(face);
   return 0;
}

//void glDepthBoundsEXT (GLclampd zmin, GLclampd zmax);
static AbstractQoreNode *f_glDepthBoundsEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLclampd zmin = (GLclampd)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLclampd zmax = (GLclampd)(p ? p->getAsInt() : 0);
   glDepthBoundsEXT(zmin, zmax);
   return 0;
}

//void glBlendEquationSeparateEXT (GLenum modeRGB, GLenum modeAlpha);
static AbstractQoreNode *f_glBlendEquationSeparateEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum modeRGB = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum modeAlpha = (GLenum)(p ? p->getAsInt() : 0);
   glBlendEquationSeparateEXT(modeRGB, modeAlpha);
   return 0;
}

//GLboolean glIsRenderbufferEXT (GLuint renderbuffer);
static AbstractQoreNode *f_glIsRenderbufferEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned renderbuffer = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsRenderbufferEXT(renderbuffer));
}

//void glBindRenderbufferEXT (GLenum target, GLuint renderbuffer);
static AbstractQoreNode *f_glBindRenderbufferEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned renderbuffer = p ? p->getAsBigInt() : 0;
   glBindRenderbufferEXT(target, renderbuffer);
   return 0;
}

/*
//void glDeleteRenderbuffersEXT (GLsizei n, const GLuint *renderbuffers);
static AbstractQoreNode *f_glDeleteRenderbuffersEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* renderbuffers = p;
   glDeleteRenderbuffersEXT(n, renderbuffers);
   return 0;
}
*/

/*
//void glGenRenderbuffersEXT (GLsizei n, GLuint *renderbuffers);
static AbstractQoreNode *f_glGenRenderbuffersEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* renderbuffers = p;
   glGenRenderbuffersEXT(n, renderbuffers);
   return 0;
}
*/

//void glRenderbufferStorageEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
static AbstractQoreNode *f_glRenderbufferStorageEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   glRenderbufferStorageEXT(target, internalformat, width, height);
   return 0;
}

/*
//void glGetRenderbufferParameterivEXT (GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetRenderbufferParameterivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetRenderbufferParameterivEXT(target, pname, params);
   return 0;
}
*/

//GLboolean glIsFramebufferEXT (GLuint framebuffer);
static AbstractQoreNode *f_glIsFramebufferEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned framebuffer = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsFramebufferEXT(framebuffer));
}

//void glBindFramebufferEXT (GLenum target, GLuint framebuffer);
static AbstractQoreNode *f_glBindFramebufferEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned framebuffer = p ? p->getAsBigInt() : 0;
   glBindFramebufferEXT(target, framebuffer);
   return 0;
}

/*
//void glDeleteFramebuffersEXT (GLsizei n, const GLuint *framebuffers);
static AbstractQoreNode *f_glDeleteFramebuffersEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* framebuffers = p;
   glDeleteFramebuffersEXT(n, framebuffers);
   return 0;
}
*/

//void glGenFramebuffersEXT (GLsizei n, GLuint *framebuffers);
static AbstractQoreNode *f_glGenFramebuffersEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);

   GLuint framebuffers[n];
   glGenFramebuffersEXT(n, framebuffers);

   if (n == 1)
      return new QoreBigIntNode(framebuffers[0]);

   QoreListNode *l = new QoreListNode;
   for (int i = 0; i < n; ++i)
      l->push(new QoreBigIntNode(framebuffers[i]));

   return l;
}

//GLenum glCheckFramebufferStatusEXT (GLenum target);
static AbstractQoreNode *f_glCheckFramebufferStatusEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   return new QoreBigIntNode(glCheckFramebufferStatusEXT(target));
}

//void glFramebufferTexture1DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
static AbstractQoreNode *f_glFramebufferTexture1DEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum attachment = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum textarget = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   unsigned texture = p ? p->getAsBigInt() : 0;
   p = get_param(params, 4);
   int level = p ? p->getAsInt() : 0;
   glFramebufferTexture1DEXT(target, attachment, textarget, texture, level);
   return 0;
}

//void glFramebufferTexture2DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
static AbstractQoreNode *f_glFramebufferTexture2DEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum attachment = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum textarget = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   unsigned texture = p ? p->getAsBigInt() : 0;
   p = get_param(params, 4);
   int level = p ? p->getAsInt() : 0;
   glFramebufferTexture2DEXT(target, attachment, textarget, texture, level);
   return 0;
}

//void glFramebufferTexture3DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
static AbstractQoreNode *f_glFramebufferTexture3DEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum attachment = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum textarget = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   unsigned texture = p ? p->getAsBigInt() : 0;
   p = get_param(params, 4);
   int level = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   int zoffset = p ? p->getAsInt() : 0;
   glFramebufferTexture3DEXT(target, attachment, textarget, texture, level, zoffset);
   return 0;
}

//void glFramebufferRenderbufferEXT (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
static AbstractQoreNode *f_glFramebufferRenderbufferEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum attachment = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum renderbuffertarget = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   unsigned renderbuffer = p ? p->getAsBigInt() : 0;
   glFramebufferRenderbufferEXT(target, attachment, renderbuffertarget, renderbuffer);
   return 0;
}

/*
//void glGetFramebufferAttachmentParameterivEXT (GLenum target, GLenum attachment, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetFramebufferAttachmentParameterivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum attachment = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLint* params = p;
   glGetFramebufferAttachmentParameterivEXT(target, attachment, pname, params);
   return 0;
}
*/

//void glGenerateMipmapEXT (GLenum target);
static AbstractQoreNode *f_glGenerateMipmapEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   glGenerateMipmapEXT(target);
   return 0;
}

//void glBlitFramebufferEXT (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
static AbstractQoreNode *f_glBlitFramebufferEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int srcX0 = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int srcY0 = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int srcX1 = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int srcY1 = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int dstX0 = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   int dstY0 = p ? p->getAsInt() : 0;
   p = get_param(params, 6);
   int dstX1 = p ? p->getAsInt() : 0;
   p = get_param(params, 7);
   int dstY1 = p ? p->getAsInt() : 0;
   p = get_param(params, 8);
   GLbitfield mask = (GLbitfield)(p ? p->getAsInt() : 0);
   p = get_param(params, 9);
   GLenum filter = (GLenum)(p ? p->getAsInt() : 0);
   glBlitFramebufferEXT(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
   return 0;
}

//void glRenderbufferStorageMultisampleEXT (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
static AbstractQoreNode *f_glRenderbufferStorageMultisampleEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei samples = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   glRenderbufferStorageMultisampleEXT(target, samples, internalformat, width, height);
   return 0;
}

//void glProgramParameteriEXT (GLuint program, GLenum pname, GLint value);
static AbstractQoreNode *f_glProgramParameteriEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   int value = p ? p->getAsInt() : 0;
   glProgramParameteriEXT(program, pname, value);
   return 0;
}

//void glFramebufferTextureEXT (GLenum target, GLenum attachment, GLuint texture, GLint level);
static AbstractQoreNode *f_glFramebufferTextureEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum attachment = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   unsigned texture = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   int level = p ? p->getAsInt() : 0;
   glFramebufferTextureEXT(target, attachment, texture, level);
   return 0;
}

//void glFramebufferTextureLayerEXT (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
static AbstractQoreNode *f_glFramebufferTextureLayerEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum attachment = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   unsigned texture = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   int level = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int layer = p ? p->getAsInt() : 0;
   glFramebufferTextureLayerEXT(target, attachment, texture, level, layer);
   return 0;
}

//void glFramebufferTextureFaceEXT (GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face);
static AbstractQoreNode *f_glFramebufferTextureFaceEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum attachment = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   unsigned texture = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   int level = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   GLenum face = (GLenum)(p ? p->getAsInt() : 0);
   glFramebufferTextureFaceEXT(target, attachment, texture, level, face);
   return 0;
}

#ifdef HAVE_GLBINDBUFFEREXT
//void glBindBufferRangeEXT (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
static AbstractQoreNode *f_glBindBufferRangeEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned buffer = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   GLintptr offset = (GLintptr)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLsizeiptr size = (GLsizeiptr)(p ? p->getAsInt() : 0);
   glBindBufferRangeEXT(target, index, buffer, offset, size);
   return 0;
}

//void glBindBufferOffsetEXT (GLenum target, GLuint index, GLuint buffer, GLintptr offset);
static AbstractQoreNode *f_glBindBufferOffsetEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned buffer = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   GLintptr offset = (GLintptr)(p ? p->getAsInt() : 0);
   glBindBufferOffsetEXT(target, index, buffer, offset);
   return 0;
}

//void glBindBufferBaseEXT (GLenum target, GLuint index, GLuint buffer);
static AbstractQoreNode *f_glBindBufferBaseEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned buffer = p ? p->getAsBigInt() : 0;
   glBindBufferBaseEXT(target, index, buffer);
   return 0;
}
#endif

#ifdef HAVE_GLTRANSFORMFEEDBACKEXT
//void glBeginTransformFeedbackEXT (GLenum primitiveMode);
static AbstractQoreNode *f_glBeginTransformFeedbackEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum primitiveMode = (GLenum)(p ? p->getAsInt() : 0);
   glBeginTransformFeedbackEXT(primitiveMode);
   return 0;
}

//void glEndTransformFeedbackEXT (void);
static AbstractQoreNode *f_glEndTransformFeedbackEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   glEndTransformFeedbackEXT();
   return 0;
}
#endif

/*
//void glTransformFeedbackVaryingsEXT (GLuint program, GLsizei count, const GLchar **varyings, GLenum bufferMode);
static AbstractQoreNode *f_glTransformFeedbackVaryingsEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLchar* *varyings arg2 = p;
   p = get_param(params, 3);
   GLenum bufferMode = (GLenum)(p ? p->getAsInt() : 0);
   glTransformFeedbackVaryingsEXT(program, count, arg2, bufferMode);
   return 0;
}
*/

/*
//void glGetTransformFeedbackVaryingEXT (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
static AbstractQoreNode *f_glGetTransformFeedbackVaryingEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLsizei bufSize = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLsizei* length = p;
   p = get_param(params, 4);
   ??? GLsizei* size = p;
   p = get_param(params, 5);
   ??? GLenum* type = p;
   p = get_param(params, 6);
   ??? GLchar* name = p;
   glGetTransformFeedbackVaryingEXT(program, index, bufSize, length, size, type, name);
   return 0;
}
*/

/*
//void glGetIntegerIndexedvEXT (GLenum param, GLuint index, GLint *values);
static AbstractQoreNode *f_glGetIntegerIndexedvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum param = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLint* values = p;
   glGetIntegerIndexedvEXT(param, index, values);
   return 0;
}
*/

/*
//void glGetBooleanIndexedvEXT (GLenum param, GLuint index, GLboolean *values);
static AbstractQoreNode *f_glGetBooleanIndexedvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum param = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLboolean* values = p;
   glGetBooleanIndexedvEXT(param, index, values);
   return 0;
}
*/

//void glUniformBufferEXT (GLuint program, GLint location, GLuint buffer);
static AbstractQoreNode *f_glUniformBufferEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   unsigned buffer = p ? p->getAsBigInt() : 0;
   glUniformBufferEXT(program, location, buffer);
   return 0;
}

//GLint glGetUniformBufferSizeEXT (GLuint program, GLint location);
static AbstractQoreNode *f_glGetUniformBufferSizeEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   int location = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(glGetUniformBufferSizeEXT(program, location));
}

/*
//GLintptr glGetUniformOffsetEXT (GLuint program, GLint location);
static AbstractQoreNode *f_glGetUniformOffsetEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   int location = p ? p->getAsInt() : 0;
   ??? return new QoreBigIntNode(glGetUniformOffsetEXT(program, location));
}
*/

//void glClearColorIiEXT ( GLint r, GLint g, GLint b, GLint a );
static AbstractQoreNode *f_glClearColorIiEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int r = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int g = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int b = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int a = p ? p->getAsInt() : 0;
   glClearColorIiEXT(r, g, b, a);
   return 0;
}

//void glClearColorIuiEXT ( GLuint r, GLuint g, GLuint b, GLuint a );
static AbstractQoreNode *f_glClearColorIuiEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned r = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned g = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned b = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   unsigned a = p ? p->getAsBigInt() : 0;
   glClearColorIuiEXT(r, g, b, a);
   return 0;
}

/*
//void glTexParameterIivEXT ( GLenum target, GLenum pname, GLint *params );
static AbstractQoreNode *f_glTexParameterIivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glTexParameterIivEXT(target, pname, params);
   return 0;
}
*/

/*
//void glTexParameterIuivEXT ( GLenum target, GLenum pname, GLuint *params );
static AbstractQoreNode *f_glTexParameterIuivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLuint* params = p;
   glTexParameterIuivEXT(target, pname, params);
   return 0;
}
*/

/*
//void glGetTexParameterIivEXT ( GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetTexParameterIivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetTexParameterIivEXT(target, pname, params);
   return 0;
}
*/

/*
//void glGetTexParameterIiuvEXT ( GLenum target, GLenum pname, GLuint *params);
static AbstractQoreNode *f_glGetTexParameterIiuvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLuint* params = p;
   glGetTexParameterIiuvEXT(target, pname, params);
   return 0;
}
*/

//void glVertexAttribI1iEXT (GLuint index, GLint x);
static AbstractQoreNode *f_glVertexAttribI1iEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   int x = p ? p->getAsInt() : 0;
   glVertexAttribI1iEXT(index, x);
   return 0;
}

//void glVertexAttribI2iEXT (GLuint index, GLint x, GLint y);
static AbstractQoreNode *f_glVertexAttribI2iEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int y = p ? p->getAsInt() : 0;
   glVertexAttribI2iEXT(index, x, y);
   return 0;
}

//void glVertexAttribI3iEXT (GLuint index, GLint x, GLint y, GLint z);
static AbstractQoreNode *f_glVertexAttribI3iEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int z = p ? p->getAsInt() : 0;
   glVertexAttribI3iEXT(index, x, y, z);
   return 0;
}

//void glVertexAttribI4iEXT (GLuint index, GLint x, GLint y, GLint z, GLint w);
static AbstractQoreNode *f_glVertexAttribI4iEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int z = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int w = p ? p->getAsInt() : 0;
   glVertexAttribI4iEXT(index, x, y, z, w);
   return 0;
}

//void glVertexAttribI1uiEXT (GLuint index, GLuint x);
static AbstractQoreNode *f_glVertexAttribI1uiEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned x = p ? p->getAsBigInt() : 0;
   glVertexAttribI1uiEXT(index, x);
   return 0;
}

//void glVertexAttribI2uiEXT (GLuint index, GLuint x, GLuint y);
static AbstractQoreNode *f_glVertexAttribI2uiEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned x = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned y = p ? p->getAsBigInt() : 0;
   glVertexAttribI2uiEXT(index, x, y);
   return 0;
}

//void glVertexAttribI3uiEXT (GLuint index, GLuint x, GLuint y, GLuint z);
static AbstractQoreNode *f_glVertexAttribI3uiEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned x = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned y = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   unsigned z = p ? p->getAsBigInt() : 0;
   glVertexAttribI3uiEXT(index, x, y, z);
   return 0;
}

//void glVertexAttribI4uiEXT (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
static AbstractQoreNode *f_glVertexAttribI4uiEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned x = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned y = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   unsigned z = p ? p->getAsBigInt() : 0;
   p = get_param(params, 4);
   unsigned w = p ? p->getAsBigInt() : 0;
   glVertexAttribI4uiEXT(index, x, y, z, w);
   return 0;
}

/*
//void glVertexAttribI1ivEXT (GLuint index, const GLint *v);
static AbstractQoreNode *f_glVertexAttribI1ivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLint* v = p;
   glVertexAttribI1ivEXT(index, v);
   return 0;
}
*/

/*
//void glVertexAttribI2ivEXT (GLuint index, const GLint *v);
static AbstractQoreNode *f_glVertexAttribI2ivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLint* v = p;
   glVertexAttribI2ivEXT(index, v);
   return 0;
}
*/

/*
//void glVertexAttribI3ivEXT (GLuint index, const GLint *v);
static AbstractQoreNode *f_glVertexAttribI3ivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLint* v = p;
   glVertexAttribI3ivEXT(index, v);
   return 0;
}
*/

/*
//void glVertexAttribI4ivEXT (GLuint index, const GLint *v);
static AbstractQoreNode *f_glVertexAttribI4ivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLint* v = p;
   glVertexAttribI4ivEXT(index, v);
   return 0;
}
*/

/*
//void glVertexAttribI1uivEXT (GLuint index, const GLuint *v);
static AbstractQoreNode *f_glVertexAttribI1uivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLuint* v = p;
   glVertexAttribI1uivEXT(index, v);
   return 0;
}
*/

/*
//void glVertexAttribI2uivEXT (GLuint index, const GLuint *v);
static AbstractQoreNode *f_glVertexAttribI2uivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLuint* v = p;
   glVertexAttribI2uivEXT(index, v);
   return 0;
}
*/

/*
//void glVertexAttribI3uivEXT (GLuint index, const GLuint *v);
static AbstractQoreNode *f_glVertexAttribI3uivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLuint* v = p;
   glVertexAttribI3uivEXT(index, v);
   return 0;
}
*/

/*
//void glVertexAttribI4uivEXT (GLuint index, const GLuint *v);
static AbstractQoreNode *f_glVertexAttribI4uivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLuint* v = p;
   glVertexAttribI4uivEXT(index, v);
   return 0;
}
*/

/*
//void glVertexAttribI4bvEXT (GLuint index, const GLbyte *v);
static AbstractQoreNode *f_glVertexAttribI4bvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLbyte* v = p;
   glVertexAttribI4bvEXT(index, v);
   return 0;
}
*/

/*
//void glVertexAttribI4svEXT (GLuint index, const GLshort *v);
static AbstractQoreNode *f_glVertexAttribI4svEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glVertexAttribI4svEXT(index, v);
   return 0;
}
*/

/*
//void glVertexAttribI4ubvEXT (GLuint index, const GLubyte *v);
static AbstractQoreNode *f_glVertexAttribI4ubvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLubyte* v = p;
   glVertexAttribI4ubvEXT(index, v);
   return 0;
}
*/

/*
//void glVertexAttribI4usvEXT (GLuint index, const GLushort *v);
static AbstractQoreNode *f_glVertexAttribI4usvEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLushort* v = p;
   glVertexAttribI4usvEXT(index, v);
   return 0;
}
*/

/*
//void glVertexAttribIPointerEXT (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
static AbstractQoreNode *f_glVertexAttribIPointerEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   int size = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei stride = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   ??? GLvoid* pointer = p;
   glVertexAttribIPointerEXT(index, size, type, stride, pointer);
   return 0;
}
*/

/*
//void glGetVertexAttribIivEXT (GLuint index, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetVertexAttribIivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetVertexAttribIivEXT(index, pname, params);
   return 0;
}
*/

/*
//void glGetVertexAttribIuivEXT (GLuint index, GLenum pname, GLuint *params);
static AbstractQoreNode *f_glGetVertexAttribIuivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLuint* params = p;
   glGetVertexAttribIuivEXT(index, pname, params);
   return 0;
}
*/

//void glUniform1uiEXT (GLint location, GLuint v0);
static AbstractQoreNode *f_glUniform1uiEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   unsigned v0 = p ? p->getAsBigInt() : 0;
   glUniform1uiEXT(location, v0);
   return 0;
}

//void glUniform2uiEXT (GLint location, GLuint v0, GLuint v1);
static AbstractQoreNode *f_glUniform2uiEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   unsigned v0 = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned v1 = p ? p->getAsBigInt() : 0;
   glUniform2uiEXT(location, v0, v1);
   return 0;
}

//void glUniform3uiEXT (GLint location, GLuint v0, GLuint v1, GLuint v2);
static AbstractQoreNode *f_glUniform3uiEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   unsigned v0 = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned v1 = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   unsigned v2 = p ? p->getAsBigInt() : 0;
   glUniform3uiEXT(location, v0, v1, v2);
   return 0;
}

//void glUniform4uiEXT (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
static AbstractQoreNode *f_glUniform4uiEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   unsigned v0 = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned v1 = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   unsigned v2 = p ? p->getAsBigInt() : 0;
   p = get_param(params, 4);
   unsigned v3 = p ? p->getAsBigInt() : 0;
   glUniform4uiEXT(location, v0, v1, v2, v3);
   return 0;
}

/*
//void glUniform1uivEXT (GLint location, GLsizei count, const GLuint *value);
static AbstractQoreNode *f_glUniform1uivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLuint* value = p;
   glUniform1uivEXT(location, count, value);
   return 0;
}
*/

/*
//void glUniform2uivEXT (GLint location, GLsizei count, const GLuint *value);
static AbstractQoreNode *f_glUniform2uivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLuint* value = p;
   glUniform2uivEXT(location, count, value);
   return 0;
}
*/

/*
//void glUniform3uivEXT (GLint location, GLsizei count, const GLuint *value);
static AbstractQoreNode *f_glUniform3uivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLuint* value = p;
   glUniform3uivEXT(location, count, value);
   return 0;
}
*/

/*
//void glUniform4uivEXT (GLint location, GLsizei count, const GLuint *value);
static AbstractQoreNode *f_glUniform4uivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLuint* value = p;
   glUniform4uivEXT(location, count, value);
   return 0;
}
*/

/*
//void glGetUniformuivEXT (GLuint program, GLint location, GLuint *params);
static AbstractQoreNode *f_glGetUniformuivEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   int location = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   ??? GLuint* params = p;
   glGetUniformuivEXT(program, location, params);
   return 0;
}
*/

/*
//void glBindFragDataLocationEXT (GLuint program, GLuint colorNumber, const GLchar *name);
static AbstractQoreNode *f_glBindFragDataLocationEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned colorNumber = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLchar* name = p;
   glBindFragDataLocationEXT(program, colorNumber, name);
   return 0;
}
*/

/*
//GLint glGetFragDataLocationEXT (GLuint program, const GLchar *name);
static AbstractQoreNode *f_glGetFragDataLocationEXT(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLchar* name = p;
   return new QoreBigIntNode(glGetFragDataLocationEXT(program, name));
}
*/

#ifdef HAVE_GL_APPLE_FUNCS
/*
//void glTextureRangeAPPLE (GLenum target, GLsizei length, const GLvoid *pointer);
static AbstractQoreNode *f_glTextureRangeAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei length = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLvoid* pointer = p;
   glTextureRangeAPPLE(target, length, pointer);
   return 0;
}
#endif
*/

/*
//void glGetTexParameterPointervAPPLE (GLenum target, GLenum pname, GLvoid **params);
static AbstractQoreNode *f_glGetTexParameterPointervAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLvoid* *params arg2 = p;
   glGetTexParameterPointervAPPLE(target, pname, arg2);
   return 0;
}
*/

/*
//void glVertexArrayRangeAPPLE (GLsizei length, const GLvoid *pointer);
static AbstractQoreNode *f_glVertexArrayRangeAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei length = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLvoid* pointer = p;
   glVertexArrayRangeAPPLE(length, pointer);
   return 0;
}
*/

/*
//void glFlushVertexArrayRangeAPPLE (GLsizei length, const GLvoid *pointer);
static AbstractQoreNode *f_glFlushVertexArrayRangeAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei length = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLvoid* pointer = p;
   glFlushVertexArrayRangeAPPLE(length, pointer);
   return 0;
}
*/

//void glVertexArrayParameteriAPPLE (GLenum pname, GLint param);
static AbstractQoreNode *f_glVertexArrayParameteriAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int param = p ? p->getAsInt() : 0;
   glVertexArrayParameteriAPPLE(pname, param);
   return 0;
}

//void glBindVertexArrayAPPLE (GLuint id);
static AbstractQoreNode *f_glBindVertexArrayAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned id = p ? p->getAsBigInt() : 0;
   glBindVertexArrayAPPLE(id);
   return 0;
}

/*
//void glDeleteVertexArraysAPPLE (GLsizei n, const GLuint *ids);
static AbstractQoreNode *f_glDeleteVertexArraysAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* ids = p;
   glDeleteVertexArraysAPPLE(n, ids);
   return 0;
}
*/

/*
//void glGenVertexArraysAPPLE (GLsizei n, GLuint *ids);
static AbstractQoreNode *f_glGenVertexArraysAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* ids = p;
   glGenVertexArraysAPPLE(n, ids);
   return 0;
}
*/

//GLboolean glIsVertexArrayAPPLE (GLuint id);
static AbstractQoreNode *f_glIsVertexArrayAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned id = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsVertexArrayAPPLE(id));
}

/*
//void glGenFencesAPPLE (GLsizei n, GLuint *fences);
static AbstractQoreNode *f_glGenFencesAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* fences = p;
   glGenFencesAPPLE(n, fences);
   return 0;
}
*/

/*
//void glDeleteFencesAPPLE (GLsizei n, const GLuint *fences);
static AbstractQoreNode *f_glDeleteFencesAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* fences = p;
   glDeleteFencesAPPLE(n, fences);
   return 0;
}
*/

//void glSetFenceAPPLE (GLuint fence);
static AbstractQoreNode *f_glSetFenceAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned fence = p ? p->getAsBigInt() : 0;
   glSetFenceAPPLE(fence);
   return 0;
}

//GLboolean glIsFenceAPPLE (GLuint fence);
static AbstractQoreNode *f_glIsFenceAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned fence = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsFenceAPPLE(fence));
}

//GLboolean glTestFenceAPPLE (GLuint fence);
static AbstractQoreNode *f_glTestFenceAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned fence = p ? p->getAsBigInt() : 0;
   return get_bool_node(glTestFenceAPPLE(fence));
}

//void glFinishFenceAPPLE (GLuint fence);
static AbstractQoreNode *f_glFinishFenceAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned fence = p ? p->getAsBigInt() : 0;
   glFinishFenceAPPLE(fence);
   return 0;
}

//GLboolean glTestObjectAPPLE (GLenum object, GLuint name);
static AbstractQoreNode *f_glTestObjectAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum object = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned name = p ? p->getAsBigInt() : 0;
   return get_bool_node(glTestObjectAPPLE(object, name));
}

//void glFinishObjectAPPLE (GLenum object, GLuint name);
static AbstractQoreNode *f_glFinishObjectAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum object = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned name = p ? p->getAsBigInt() : 0;
   glFinishObjectAPPLE(object, name);
   return 0;
}

/*
//void glElementPointerAPPLE (GLenum type, const GLvoid *pointer);
static AbstractQoreNode *f_glElementPointerAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLvoid* pointer = p;
   glElementPointerAPPLE(type, pointer);
   return 0;
}
*/

//void glDrawElementArrayAPPLE (GLenum mode, GLint first, GLsizei count);
static AbstractQoreNode *f_glDrawElementArrayAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int first = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   glDrawElementArrayAPPLE(mode, first, count);
   return 0;
}

//void glDrawRangeElementArrayAPPLE (GLenum mode, GLuint start, GLuint end, GLint first, GLsizei count);
static AbstractQoreNode *f_glDrawRangeElementArrayAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned start = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned end = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   int first = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   glDrawRangeElementArrayAPPLE(mode, start, end, first, count);
   return 0;
}

/*
//void glMultiDrawElementArrayAPPLE (GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount);
static AbstractQoreNode *f_glMultiDrawElementArrayAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* first = p;
   p = get_param(params, 2);
   ??? GLsizei* count = p;
   p = get_param(params, 3);
   GLsizei primcount = (GLsizei)(p ? p->getAsInt() : 0);
   glMultiDrawElementArrayAPPLE(mode, first, count, primcount);
   return 0;
}
*/

/*
//void glMultiDrawRangeElementArrayAPPLE (GLenum mode, GLuint start, GLuint end, const GLint *first, const GLsizei *count, GLsizei primcount);
static AbstractQoreNode *f_glMultiDrawRangeElementArrayAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned start = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned end = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   ??? GLint* first = p;
   p = get_param(params, 4);
   ??? GLsizei* count = p;
   p = get_param(params, 5);
   GLsizei primcount = (GLsizei)(p ? p->getAsInt() : 0);
   glMultiDrawRangeElementArrayAPPLE(mode, start, end, first, count, primcount);
   return 0;
}
*/

//void glFlushRenderAPPLE (void);
static AbstractQoreNode *f_glFlushRenderAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   glFlushRenderAPPLE();
   return 0;
}

//void glFinishRenderAPPLE (void);
static AbstractQoreNode *f_glFinishRenderAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   glFinishRenderAPPLE();
   return 0;
}

//void glSwapAPPLE (void);
static AbstractQoreNode *f_glSwapAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   glSwapAPPLE();
   return 0;
}

//void glEnableVertexAttribAPPLE (GLuint index, GLenum pname);
static AbstractQoreNode *f_glEnableVertexAttribAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   glEnableVertexAttribAPPLE(index, pname);
   return 0;
}

//void glDisableVertexAttribAPPLE (GLuint index, GLenum pname);
static AbstractQoreNode *f_glDisableVertexAttribAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   glDisableVertexAttribAPPLE(index, pname);
   return 0;
}

//GLboolean glIsVertexAttribEnabledAPPLE (GLuint index, GLenum pname);
static AbstractQoreNode *f_glIsVertexAttribEnabledAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   return get_bool_node(glIsVertexAttribEnabledAPPLE(index, pname));
}

/*
//void glMapVertexAttrib1dAPPLE (GLuint index, GLuint size, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
static AbstractQoreNode *f_glMapVertexAttrib1dAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned size = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLdouble u1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble u2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   int stride = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   int order = p ? p->getAsInt() : 0;
   p = get_param(params, 6);
   ??? GLdouble* points = p;
   glMapVertexAttrib1dAPPLE(index, size, u1, u2, stride, order, points);
   return 0;
}
*/

/*
//void glMapVertexAttrib1fAPPLE (GLuint index, GLuint size, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
static AbstractQoreNode *f_glMapVertexAttrib1fAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned size = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLfloat u1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat u2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   int stride = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   int order = p ? p->getAsInt() : 0;
   p = get_param(params, 6);
   ??? GLfloat* points = p;
   glMapVertexAttrib1fAPPLE(index, size, u1, u2, stride, order, points);
   return 0;
}
*/

/*
//void glMapVertexAttrib2dAPPLE (GLuint index, GLuint size, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
static AbstractQoreNode *f_glMapVertexAttrib2dAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned size = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLdouble u1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble u2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   int ustride = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   int uorder = p ? p->getAsInt() : 0;
   p = get_param(params, 6);
   GLdouble v1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 7);
   GLdouble v2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 8);
   int vstride = p ? p->getAsInt() : 0;
   p = get_param(params, 9);
   int vorder = p ? p->getAsInt() : 0;
   p = get_param(params, 10);
   ??? GLdouble* points = p;
   glMapVertexAttrib2dAPPLE(index, size, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
   return 0;
}
*/

/*
//void glMapVertexAttrib2fAPPLE (GLuint index, GLuint size, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
static AbstractQoreNode *f_glMapVertexAttrib2fAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned size = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLfloat u1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat u2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   int ustride = p ? p->getAsInt() : 0;
   p = get_param(params, 5);
   int uorder = p ? p->getAsInt() : 0;
   p = get_param(params, 6);
   GLfloat v1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 7);
   GLfloat v2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 8);
   int vstride = p ? p->getAsInt() : 0;
   p = get_param(params, 9);
   int vorder = p ? p->getAsInt() : 0;
   p = get_param(params, 10);
   ??? GLfloat* points = p;
   glMapVertexAttrib2fAPPLE(index, size, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
   return 0;
}
*/

//void glBufferParameteriAPPLE (GLenum target, GLenum pname, GLint param);
static AbstractQoreNode *f_glBufferParameteriAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   int param = p ? p->getAsInt() : 0;
   glBufferParameteriAPPLE(target, pname, param);
   return 0;
}

//void glFlushMappedBufferRangeAPPLE (GLenum target, GLintptr offset, GLsizeiptr size);
static AbstractQoreNode *f_glFlushMappedBufferRangeAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLintptr offset = (GLintptr)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizeiptr size = (GLsizeiptr)(p ? p->getAsInt() : 0);
   glFlushMappedBufferRangeAPPLE(target, offset, size);
   return 0;
}

//GLenum glObjectPurgeableAPPLE (GLenum objectType, GLuint name, GLenum option);
static AbstractQoreNode *f_glObjectPurgeableAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum objectType = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned name = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLenum option = (GLenum)(p ? p->getAsInt() : 0);
   return new QoreBigIntNode(glObjectPurgeableAPPLE(objectType, name, option));
}

//GLenum glObjectUnpurgeableAPPLE (GLenum objectType, GLuint name, GLenum option);
static AbstractQoreNode *f_glObjectUnpurgeableAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum objectType = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned name = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLenum option = (GLenum)(p ? p->getAsInt() : 0);
   return new QoreBigIntNode(glObjectUnpurgeableAPPLE(objectType, name, option));
}

/*
//void glGetObjectParameterivAPPLE (GLenum objectType, GLuint name, GLenum pname, GLint* params);
static AbstractQoreNode *f_glGetObjectParameterivAPPLE(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum objectType = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned name = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLint* params = p;
   glGetObjectParameterivAPPLE(objectType, name, pname, params);
   return 0;
}
*/
#endif // HAVE_GL_APPLE_FUNCS

//void glPNTrianglesiATI (GLenum pname, GLint param);
static AbstractQoreNode *f_glPNTrianglesiATI(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int param = p ? p->getAsInt() : 0;
   glPNTrianglesiATI(pname, param);
   return 0;
}

//void glPNTrianglesfATI (GLenum pname, GLfloat param);
static AbstractQoreNode *f_glPNTrianglesfATI(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glPNTrianglesfATI(pname, param);
   return 0;
}

#ifdef HAVE_GLBLENDEQUATIONSEPARATEATI
//void glBlendEquationSeparateATI (GLenum equationRGB, GLenum equationAlpha);
static AbstractQoreNode *f_glBlendEquationSeparateATI(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum equationRGB = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum equationAlpha = (GLenum)(p ? p->getAsInt() : 0);
   glBlendEquationSeparateATI(equationRGB, equationAlpha);
   return 0;
}
#endif

//void glStencilOpSeparateATI (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
static AbstractQoreNode *f_glStencilOpSeparateATI(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum face = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum sfail = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum dpfail = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum dppass = (GLenum)(p ? p->getAsInt() : 0);
   glStencilOpSeparateATI(face, sfail, dpfail, dppass);
   return 0;
}

//void glStencilFuncSeparateATI (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask);
static AbstractQoreNode *f_glStencilFuncSeparateATI(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum frontfunc = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum backfunc = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   int ref = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   unsigned mask = p ? p->getAsBigInt() : 0;
   glStencilFuncSeparateATI(frontfunc, backfunc, ref, mask);
   return 0;
}

#ifdef HAVE_GLPNTRIANGLESATIX
//void glPNTrianglesiATIX (GLenum pname, GLint param);
static AbstractQoreNode *f_glPNTrianglesiATIX(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int param = p ? p->getAsInt() : 0;
   glPNTrianglesiATIX(pname, param);
   return 0;
}

//void glPNTrianglesfATIX (GLenum pname, GLfloat param);
static AbstractQoreNode *f_glPNTrianglesfATIX(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glPNTrianglesfATIX(pname, param);
   return 0;
}
#endif

/*
//void glCombinerParameterfvNV (GLenum, const GLfloat *);
static AbstractQoreNode *f_glCombinerParameterfvNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLfloat* arg1 = p;
   glCombinerParameterfvNV(arg0, arg1);
   return 0;
}
*/

//void glCombinerParameterfNV (GLenum, GLfloat);
static AbstractQoreNode *f_glCombinerParameterfNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat arg1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glCombinerParameterfNV(arg0, arg1);
   return 0;
}

/*
//void glCombinerParameterivNV (GLenum, const GLint *);
static AbstractQoreNode *f_glCombinerParameterivNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* arg1 = p;
   glCombinerParameterivNV(arg0, arg1);
   return 0;
}
*/

//void glCombinerParameteriNV (GLenum, GLint);
static AbstractQoreNode *f_glCombinerParameteriNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int arg1 = p ? p->getAsInt() : 0;
   glCombinerParameteriNV(arg0, arg1);
   return 0;
}

//void glCombinerInputNV (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum);
static AbstractQoreNode *f_glCombinerInputNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum arg3 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLenum arg4 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLenum arg5 = (GLenum)(p ? p->getAsInt() : 0);
   glCombinerInputNV(arg0, arg1, arg2, arg3, arg4, arg5);
   return 0;
}

//void glCombinerOutputNV (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLboolean, GLboolean, GLboolean);
static AbstractQoreNode *f_glCombinerOutputNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum arg3 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLenum arg4 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLenum arg5 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLenum arg6 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   GLboolean arg7 = (GLboolean)(p ? p->getAsBool() : false);
   p = get_param(params, 8);
   GLboolean arg8 = (GLboolean)(p ? p->getAsBool() : false);
   p = get_param(params, 9);
   GLboolean arg9 = (GLboolean)(p ? p->getAsBool() : false);
   glCombinerOutputNV(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
   return 0;
}

//void glFinalCombinerInputNV (GLenum, GLenum, GLenum, GLenum);
static AbstractQoreNode *f_glFinalCombinerInputNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum arg3 = (GLenum)(p ? p->getAsInt() : 0);
   glFinalCombinerInputNV(arg0, arg1, arg2, arg3);
   return 0;
}

/*
//void glGetCombinerInputParameterfvNV (GLenum, GLenum, GLenum, GLenum, GLfloat *);
static AbstractQoreNode *f_glGetCombinerInputParameterfvNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum arg3 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   ??? GLfloat* arg4 = p;
   glGetCombinerInputParameterfvNV(arg0, arg1, arg2, arg3, arg4);
   return 0;
}
*/

/*
//void glGetCombinerInputParameterivNV (GLenum, GLenum, GLenum, GLenum, GLint *);
static AbstractQoreNode *f_glGetCombinerInputParameterivNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum arg3 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   ??? GLint* arg4 = p;
   glGetCombinerInputParameterivNV(arg0, arg1, arg2, arg3, arg4);
   return 0;
}
*/

/*
//void glGetCombinerOutputParameterfvNV (GLenum, GLenum, GLenum, GLfloat *);
static AbstractQoreNode *f_glGetCombinerOutputParameterfvNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLfloat* arg3 = p;
   glGetCombinerOutputParameterfvNV(arg0, arg1, arg2, arg3);
   return 0;
}
*/

/*
//void glGetCombinerOutputParameterivNV (GLenum, GLenum, GLenum, GLint *);
static AbstractQoreNode *f_glGetCombinerOutputParameterivNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum arg2 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLint* arg3 = p;
   glGetCombinerOutputParameterivNV(arg0, arg1, arg2, arg3);
   return 0;
}
*/

/*
//void glGetFinalCombinerInputParameterfvNV (GLenum, GLenum, GLfloat *);
static AbstractQoreNode *f_glGetFinalCombinerInputParameterfvNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* arg2 = p;
   glGetFinalCombinerInputParameterfvNV(arg0, arg1, arg2);
   return 0;
}
*/

/*
//void glGetFinalCombinerInputParameterivNV (GLenum, GLenum, GLint *);
static AbstractQoreNode *f_glGetFinalCombinerInputParameterivNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* arg2 = p;
   glGetFinalCombinerInputParameterivNV(arg0, arg1, arg2);
   return 0;
}
*/

/*
//void glCombinerStageParameterfvNV (GLenum, GLenum, const GLfloat *);
static AbstractQoreNode *f_glCombinerStageParameterfvNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* arg2 = p;
   glCombinerStageParameterfvNV(arg0, arg1, arg2);
   return 0;
}
*/

/*
//void glGetCombinerStageParameterfvNV (GLenum, GLenum, GLfloat *);
static AbstractQoreNode *f_glGetCombinerStageParameterfvNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum arg0 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum arg1 = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* arg2 = p;
   glGetCombinerStageParameterfvNV(arg0, arg1, arg2);
   return 0;
}
*/

//void glPointParameteriNV (GLenum pname, GLint param);
static AbstractQoreNode *f_glPointParameteriNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int param = p ? p->getAsInt() : 0;
   glPointParameteriNV(pname, param);
   return 0;
}

/*
//void glPointParameterivNV (GLenum pname, const GLint *params);
static AbstractQoreNode *f_glPointParameterivNV(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* params = p;
   glPointParameterivNV(pname, params);
   return 0;
}
*/

void initOpenGLExt()
{
   builtinFunctions.add("glActiveTextureARB",           f_glActiveTextureARB, QDOM_GUI);
   builtinFunctions.add("glClientActiveTextureARB",     f_glClientActiveTextureARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord1dARB",         f_glMultiTexCoord1dARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord1dvARB",        f_glMultiTexCoord1dvARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord1fARB",         f_glMultiTexCoord1fARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord1fvARB",        f_glMultiTexCoord1fvARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord1iARB",         f_glMultiTexCoord1iARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord1ivARB",        f_glMultiTexCoord1ivARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord1sARB",         f_glMultiTexCoord1sARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord1svARB",        f_glMultiTexCoord1svARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord2dARB",         f_glMultiTexCoord2dARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord2dvARB",        f_glMultiTexCoord2dvARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord2fARB",         f_glMultiTexCoord2fARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord2fvARB",        f_glMultiTexCoord2fvARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord2iARB",         f_glMultiTexCoord2iARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord2ivARB",        f_glMultiTexCoord2ivARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord2sARB",         f_glMultiTexCoord2sARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord2svARB",        f_glMultiTexCoord2svARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord3dARB",         f_glMultiTexCoord3dARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord3dvARB",        f_glMultiTexCoord3dvARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord3fARB",         f_glMultiTexCoord3fARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord3fvARB",        f_glMultiTexCoord3fvARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord3iARB",         f_glMultiTexCoord3iARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord3ivARB",        f_glMultiTexCoord3ivARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord3sARB",         f_glMultiTexCoord3sARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord3svARB",        f_glMultiTexCoord3svARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord4dARB",         f_glMultiTexCoord4dARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord4dvARB",        f_glMultiTexCoord4dvARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord4fARB",         f_glMultiTexCoord4fARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord4fvARB",        f_glMultiTexCoord4fvARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord4iARB",         f_glMultiTexCoord4iARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord4ivARB",        f_glMultiTexCoord4ivARB, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord4sARB",         f_glMultiTexCoord4sARB, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord4svARB",        f_glMultiTexCoord4svARB, QDOM_GUI);
   //builtinFunctions.add("glLoadTransposeMatrixfARB",    f_glLoadTransposeMatrixfARB, QDOM_GUI);
   //builtinFunctions.add("glLoadTransposeMatrixdARB",    f_glLoadTransposeMatrixdARB, QDOM_GUI);
   //builtinFunctions.add("glMultTransposeMatrixfARB",    f_glMultTransposeMatrixfARB, QDOM_GUI);
   //builtinFunctions.add("glMultTransposeMatrixdARB",    f_glMultTransposeMatrixdARB, QDOM_GUI);
   builtinFunctions.add("glSampleCoverageARB",          f_glSampleCoverageARB, QDOM_GUI);
#ifdef HAVE_GLSAMPLEPASSARB
   builtinFunctions.add("glSamplePassARB",              f_glSamplePassARB, QDOM_GUI);
#endif
   //builtinFunctions.add("glCompressedTexImage3DARB",    f_glCompressedTexImage3DARB, QDOM_GUI);
   //builtinFunctions.add("glCompressedTexImage2DARB",    f_glCompressedTexImage2DARB, QDOM_GUI);
   //builtinFunctions.add("glCompressedTexImage1DARB",    f_glCompressedTexImage1DARB, QDOM_GUI);
   //builtinFunctions.add("glCompressedTexSubImage3DARB", f_glCompressedTexSubImage3DARB, QDOM_GUI);
   //builtinFunctions.add("glCompressedTexSubImage2DARB", f_glCompressedTexSubImage2DARB, QDOM_GUI);
   //builtinFunctions.add("glCompressedTexSubImage1DARB", f_glCompressedTexSubImage1DARB, QDOM_GUI);
   //builtinFunctions.add("glGetCompressedTexImageARB",   f_glGetCompressedTexImageARB, QDOM_GUI);
   //builtinFunctions.add("glWeightbvARB",                f_glWeightbvARB, QDOM_GUI);
   //builtinFunctions.add("glWeightsvARB",                f_glWeightsvARB, QDOM_GUI);
   //builtinFunctions.add("glWeightivARB",                f_glWeightivARB, QDOM_GUI);
   //builtinFunctions.add("glWeightfvARB",                f_glWeightfvARB, QDOM_GUI);
   //builtinFunctions.add("glWeightdvARB",                f_glWeightdvARB, QDOM_GUI);
   //builtinFunctions.add("glWeightubvARB",               f_glWeightubvARB, QDOM_GUI);
   //builtinFunctions.add("glWeightusvARB",               f_glWeightusvARB, QDOM_GUI);
   //builtinFunctions.add("glWeightuivARB",               f_glWeightuivARB, QDOM_GUI);
   //builtinFunctions.add("glWeightPointerARB",           f_glWeightPointerARB, QDOM_GUI);
   builtinFunctions.add("glVertexBlendARB",             f_glVertexBlendARB, QDOM_GUI);
   builtinFunctions.add("glWindowPos2dARB",             f_glWindowPos2dARB, QDOM_GUI);
   //builtinFunctions.add("glWindowPos2dvARB",            f_glWindowPos2dvARB, QDOM_GUI);
   builtinFunctions.add("glWindowPos2fARB",             f_glWindowPos2fARB, QDOM_GUI);
   //builtinFunctions.add("glWindowPos2fvARB",            f_glWindowPos2fvARB, QDOM_GUI);
   builtinFunctions.add("glWindowPos2iARB",             f_glWindowPos2iARB, QDOM_GUI);
   //builtinFunctions.add("glWindowPos2ivARB",            f_glWindowPos2ivARB, QDOM_GUI);
   builtinFunctions.add("glWindowPos2sARB",             f_glWindowPos2sARB, QDOM_GUI);
   //builtinFunctions.add("glWindowPos2svARB",            f_glWindowPos2svARB, QDOM_GUI);
   builtinFunctions.add("glWindowPos3dARB",             f_glWindowPos3dARB, QDOM_GUI);
   //builtinFunctions.add("glWindowPos3dvARB",            f_glWindowPos3dvARB, QDOM_GUI);
   builtinFunctions.add("glWindowPos3fARB",             f_glWindowPos3fARB, QDOM_GUI);
   //builtinFunctions.add("glWindowPos3fvARB",            f_glWindowPos3fvARB, QDOM_GUI);
   builtinFunctions.add("glWindowPos3iARB",             f_glWindowPos3iARB, QDOM_GUI);
   //builtinFunctions.add("glWindowPos3ivARB",            f_glWindowPos3ivARB, QDOM_GUI);
   builtinFunctions.add("glWindowPos3sARB",             f_glWindowPos3sARB, QDOM_GUI);
   //builtinFunctions.add("glWindowPos3svARB",            f_glWindowPos3svARB, QDOM_GUI);
   //builtinFunctions.add("glGenQueriesARB",              f_glGenQueriesARB, QDOM_GUI);
   //builtinFunctions.add("glDeleteQueriesARB",           f_glDeleteQueriesARB, QDOM_GUI);
   builtinFunctions.add("glIsQueryARB",                 f_glIsQueryARB, QDOM_GUI);
   builtinFunctions.add("glBeginQueryARB",              f_glBeginQueryARB, QDOM_GUI);
   builtinFunctions.add("glEndQueryARB",                f_glEndQueryARB, QDOM_GUI);
   //builtinFunctions.add("glGetQueryivARB",              f_glGetQueryivARB, QDOM_GUI);
   //builtinFunctions.add("glGetQueryObjectivARB",        f_glGetQueryObjectivARB, QDOM_GUI);
   //builtinFunctions.add("glGetQueryObjectuivARB",       f_glGetQueryObjectuivARB, QDOM_GUI);
   builtinFunctions.add("glPointParameterfARB",         f_glPointParameterfARB, QDOM_GUI);
   //builtinFunctions.add("glPointParameterfvARB",        f_glPointParameterfvARB, QDOM_GUI);
   builtinFunctions.add("glBindProgramARB",             f_glBindProgramARB, QDOM_GUI);
   //builtinFunctions.add("glDeleteProgramsARB",          f_glDeleteProgramsARB, QDOM_GUI);
   builtinFunctions.add("glGenProgramsARB",             f_glGenProgramsARB, QDOM_GUI);
   builtinFunctions.add("glIsProgramARB",               f_glIsProgramARB, QDOM_GUI);
   builtinFunctions.add("glProgramEnvParameter4dARB",   f_glProgramEnvParameter4dARB, QDOM_GUI);
   //builtinFunctions.add("glProgramEnvParameter4dvARB",  f_glProgramEnvParameter4dvARB, QDOM_GUI);
   builtinFunctions.add("glProgramEnvParameter4fARB",   f_glProgramEnvParameter4fARB, QDOM_GUI);
   //builtinFunctions.add("glProgramEnvParameter4fvARB",  f_glProgramEnvParameter4fvARB, QDOM_GUI);
   builtinFunctions.add("glProgramLocalParameter4dARB", f_glProgramLocalParameter4dARB, QDOM_GUI);
   //builtinFunctions.add("glProgramLocalParameter4dvARB", f_glProgramLocalParameter4dvARB, QDOM_GUI);
   builtinFunctions.add("glProgramLocalParameter4fARB", f_glProgramLocalParameter4fARB, QDOM_GUI);
   //builtinFunctions.add("glProgramLocalParameter4fvARB", f_glProgramLocalParameter4fvARB, QDOM_GUI);
   //builtinFunctions.add("glGetProgramEnvParameterdvARB", f_glGetProgramEnvParameterdvARB, QDOM_GUI);
   //builtinFunctions.add("glGetProgramEnvParameterfvARB", f_glGetProgramEnvParameterfvARB, QDOM_GUI);
   //builtinFunctions.add("glProgramEnvParameters4fvEXT", f_glProgramEnvParameters4fvEXT, QDOM_GUI);
   //builtinFunctions.add("glProgramLocalParameters4fvEXT", f_glProgramLocalParameters4fvEXT, QDOM_GUI);
   //builtinFunctions.add("glGetProgramLocalParameterdvARB", f_glGetProgramLocalParameterdvARB, QDOM_GUI);
   //builtinFunctions.add("glGetProgramLocalParameterfvARB", f_glGetProgramLocalParameterfvARB, QDOM_GUI);
   builtinFunctions.add("glProgramStringARB",           f_glProgramStringARB, QDOM_GUI);
   //builtinFunctions.add("glGetProgramStringARB",        f_glGetProgramStringARB, QDOM_GUI);
   //builtinFunctions.add("glGetProgramivARB",            f_glGetProgramivARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib1dARB",          f_glVertexAttrib1dARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib1dvARB",         f_glVertexAttrib1dvARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib1fARB",          f_glVertexAttrib1fARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib1fvARB",         f_glVertexAttrib1fvARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib1sARB",          f_glVertexAttrib1sARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib1svARB",         f_glVertexAttrib1svARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib2dARB",          f_glVertexAttrib2dARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib2dvARB",         f_glVertexAttrib2dvARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib2fARB",          f_glVertexAttrib2fARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib2fvARB",         f_glVertexAttrib2fvARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib2sARB",          f_glVertexAttrib2sARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib2svARB",         f_glVertexAttrib2svARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib3dARB",          f_glVertexAttrib3dARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib3dvARB",         f_glVertexAttrib3dvARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib3fARB",          f_glVertexAttrib3fARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib3fvARB",         f_glVertexAttrib3fvARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib3sARB",          f_glVertexAttrib3sARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib3svARB",         f_glVertexAttrib3svARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4NbvARB",        f_glVertexAttrib4NbvARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4NivARB",        f_glVertexAttrib4NivARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4NsvARB",        f_glVertexAttrib4NsvARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib4NubARB",        f_glVertexAttrib4NubARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4NubvARB",       f_glVertexAttrib4NubvARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4NuivARB",       f_glVertexAttrib4NuivARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4NusvARB",       f_glVertexAttrib4NusvARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4bvARB",         f_glVertexAttrib4bvARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib4dARB",          f_glVertexAttrib4dARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4dvARB",         f_glVertexAttrib4dvARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib4fARB",          f_glVertexAttrib4fARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4fvARB",         f_glVertexAttrib4fvARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4ivARB",         f_glVertexAttrib4ivARB, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib4sARB",          f_glVertexAttrib4sARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4svARB",         f_glVertexAttrib4svARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4ubvARB",        f_glVertexAttrib4ubvARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4uivARB",        f_glVertexAttrib4uivARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4usvARB",        f_glVertexAttrib4usvARB, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribPointerARB",     f_glVertexAttribPointerARB, QDOM_GUI);
   builtinFunctions.add("glDisableVertexAttribArrayARB", f_glDisableVertexAttribArrayARB, QDOM_GUI);
   builtinFunctions.add("glEnableVertexAttribArrayARB", f_glEnableVertexAttribArrayARB, QDOM_GUI);
   //builtinFunctions.add("glGetVertexAttribPointervARB", f_glGetVertexAttribPointervARB, QDOM_GUI);
   //builtinFunctions.add("glGetVertexAttribdvARB",       f_glGetVertexAttribdvARB, QDOM_GUI);
   //builtinFunctions.add("glGetVertexAttribfvARB",       f_glGetVertexAttribfvARB, QDOM_GUI);
   //builtinFunctions.add("glGetVertexAttribivARB",       f_glGetVertexAttribivARB, QDOM_GUI);
   builtinFunctions.add("glDeleteObjectARB",            f_glDeleteObjectARB, QDOM_GUI);
   //builtinFunctions.add("glGetHandleARB",               f_glGetHandleARB, QDOM_GUI);
   builtinFunctions.add("glDetachObjectARB",            f_glDetachObjectARB, QDOM_GUI);
   //builtinFunctions.add("glCreateShaderObjectARB",      f_glCreateShaderObjectARB, QDOM_GUI);
   //builtinFunctions.add("glShaderSourceARB",            f_glShaderSourceARB, QDOM_GUI);
   builtinFunctions.add("glCompileShaderARB",           f_glCompileShaderARB, QDOM_GUI);
   //builtinFunctions.add("glCreateProgramObjectARB",     f_glCreateProgramObjectARB, QDOM_GUI);
   builtinFunctions.add("glAttachObjectARB",            f_glAttachObjectARB, QDOM_GUI);
   builtinFunctions.add("glLinkProgramARB",             f_glLinkProgramARB, QDOM_GUI);
   builtinFunctions.add("glUseProgramObjectARB",        f_glUseProgramObjectARB, QDOM_GUI);
   builtinFunctions.add("glValidateProgramARB",         f_glValidateProgramARB, QDOM_GUI);
   builtinFunctions.add("glUniform1fARB",               f_glUniform1fARB, QDOM_GUI);
   builtinFunctions.add("glUniform2fARB",               f_glUniform2fARB, QDOM_GUI);
   builtinFunctions.add("glUniform3fARB",               f_glUniform3fARB, QDOM_GUI);
   builtinFunctions.add("glUniform4fARB",               f_glUniform4fARB, QDOM_GUI);
   builtinFunctions.add("glUniform1iARB",               f_glUniform1iARB, QDOM_GUI);
   builtinFunctions.add("glUniform2iARB",               f_glUniform2iARB, QDOM_GUI);
   builtinFunctions.add("glUniform3iARB",               f_glUniform3iARB, QDOM_GUI);
   builtinFunctions.add("glUniform4iARB",               f_glUniform4iARB, QDOM_GUI);
   //builtinFunctions.add("glUniform1fvARB",              f_glUniform1fvARB, QDOM_GUI);
   //builtinFunctions.add("glUniform2fvARB",              f_glUniform2fvARB, QDOM_GUI);
   //builtinFunctions.add("glUniform3fvARB",              f_glUniform3fvARB, QDOM_GUI);
   //builtinFunctions.add("glUniform4fvARB",              f_glUniform4fvARB, QDOM_GUI);
   //builtinFunctions.add("glUniform1ivARB",              f_glUniform1ivARB, QDOM_GUI);
   //builtinFunctions.add("glUniform2ivARB",              f_glUniform2ivARB, QDOM_GUI);
   //builtinFunctions.add("glUniform3ivARB",              f_glUniform3ivARB, QDOM_GUI);
   //builtinFunctions.add("glUniform4ivARB",              f_glUniform4ivARB, QDOM_GUI);
   //builtinFunctions.add("glUniformMatrix2fvARB",        f_glUniformMatrix2fvARB, QDOM_GUI);
   //builtinFunctions.add("glUniformMatrix3fvARB",        f_glUniformMatrix3fvARB, QDOM_GUI);
   //builtinFunctions.add("glUniformMatrix4fvARB",        f_glUniformMatrix4fvARB, QDOM_GUI);
   //builtinFunctions.add("glGetObjectParameterfvARB",    f_glGetObjectParameterfvARB, QDOM_GUI);
   //builtinFunctions.add("glGetObjectParameterivARB",    f_glGetObjectParameterivARB, QDOM_GUI);
   //builtinFunctions.add("glGetInfoLogARB",              f_glGetInfoLogARB, QDOM_GUI);
   //builtinFunctions.add("glGetAttachedObjectsARB",      f_glGetAttachedObjectsARB, QDOM_GUI);
   //builtinFunctions.add("glGetUniformLocationARB",      f_glGetUniformLocationARB, QDOM_GUI);
   //builtinFunctions.add("glGetActiveUniformARB",        f_glGetActiveUniformARB, QDOM_GUI);
   //builtinFunctions.add("glGetUniformfvARB",            f_glGetUniformfvARB, QDOM_GUI);
   //builtinFunctions.add("glGetUniformivARB",            f_glGetUniformivARB, QDOM_GUI);
   //builtinFunctions.add("glGetShaderSourceARB",         f_glGetShaderSourceARB, QDOM_GUI);
   //builtinFunctions.add("glBindAttribLocationARB",      f_glBindAttribLocationARB, QDOM_GUI);
   //builtinFunctions.add("glGetActiveAttribARB",         f_glGetActiveAttribARB, QDOM_GUI);
   //builtinFunctions.add("glGetAttribLocationARB",       f_glGetAttribLocationARB, QDOM_GUI);
   builtinFunctions.add("glBindBufferARB",              f_glBindBufferARB, QDOM_GUI);
   //builtinFunctions.add("glDeleteBuffersARB",           f_glDeleteBuffersARB, QDOM_GUI);
   //builtinFunctions.add("glGenBuffersARB",              f_glGenBuffersARB, QDOM_GUI);
   builtinFunctions.add("glIsBufferARB",                f_glIsBufferARB, QDOM_GUI);
   //builtinFunctions.add("glBufferDataARB",              f_glBufferDataARB, QDOM_GUI);
   //builtinFunctions.add("glBufferSubDataARB",           f_glBufferSubDataARB, QDOM_GUI);
   //builtinFunctions.add("glGetBufferSubDataARB",        f_glGetBufferSubDataARB, QDOM_GUI);
   //builtinFunctions.add("glMapBufferARB",               f_glMapBufferARB, QDOM_GUI);
   builtinFunctions.add("glUnmapBufferARB",             f_glUnmapBufferARB, QDOM_GUI);
   //builtinFunctions.add("glGetBufferParameterivARB",    f_glGetBufferParameterivARB, QDOM_GUI);
   //builtinFunctions.add("glGetBufferPointervARB",       f_glGetBufferPointervARB, QDOM_GUI);
   //builtinFunctions.add("glDrawBuffersARB",             f_glDrawBuffersARB, QDOM_GUI);
   builtinFunctions.add("glBlendColorEXT",              f_glBlendColorEXT, QDOM_GUI);
   builtinFunctions.add("glBlendEquationEXT",           f_glBlendEquationEXT, QDOM_GUI);
   //builtinFunctions.add("glColorTableEXT",              f_glColorTableEXT, QDOM_GUI);
   //builtinFunctions.add("glColorSubTableEXT",           f_glColorSubTableEXT, QDOM_GUI);
   //builtinFunctions.add("glGetColorTableEXT",           f_glGetColorTableEXT, QDOM_GUI);
   //builtinFunctions.add("glGetColorTableParameterivEXT", f_glGetColorTableParameterivEXT, QDOM_GUI);
   //builtinFunctions.add("glGetColorTableParameterfvEXT", f_glGetColorTableParameterfvEXT, QDOM_GUI);
   builtinFunctions.add("glLockArraysEXT",              f_glLockArraysEXT, QDOM_GUI);
   builtinFunctions.add("glUnlockArraysEXT",            f_glUnlockArraysEXT, QDOM_GUI);
   //builtinFunctions.add("glDrawRangeElementsEXT",       f_glDrawRangeElementsEXT, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3bEXT",        f_glSecondaryColor3bEXT, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3bvEXT",       f_glSecondaryColor3bvEXT, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3dEXT",        f_glSecondaryColor3dEXT, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3dvEXT",       f_glSecondaryColor3dvEXT, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3fEXT",        f_glSecondaryColor3fEXT, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3fvEXT",       f_glSecondaryColor3fvEXT, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3iEXT",        f_glSecondaryColor3iEXT, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3ivEXT",       f_glSecondaryColor3ivEXT, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3sEXT",        f_glSecondaryColor3sEXT, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3svEXT",       f_glSecondaryColor3svEXT, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3ubEXT",       f_glSecondaryColor3ubEXT, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3ubvEXT",      f_glSecondaryColor3ubvEXT, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3uiEXT",       f_glSecondaryColor3uiEXT, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3uivEXT",      f_glSecondaryColor3uivEXT, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3usEXT",       f_glSecondaryColor3usEXT, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3usvEXT",      f_glSecondaryColor3usvEXT, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColorPointerEXT",   f_glSecondaryColorPointerEXT, QDOM_GUI);
   //builtinFunctions.add("glMultiDrawArraysEXT",         f_glMultiDrawArraysEXT, QDOM_GUI);
   //builtinFunctions.add("glMultiDrawElementsEXT",       f_glMultiDrawElementsEXT, QDOM_GUI);
   builtinFunctions.add("glFogCoordfEXT",               f_glFogCoordfEXT, QDOM_GUI);
   //builtinFunctions.add("glFogCoordfvEXT",              f_glFogCoordfvEXT, QDOM_GUI);
   builtinFunctions.add("glFogCoorddEXT",               f_glFogCoorddEXT, QDOM_GUI);
   //builtinFunctions.add("glFogCoorddvEXT",              f_glFogCoorddvEXT, QDOM_GUI);
   //builtinFunctions.add("glFogCoordPointerEXT",         f_glFogCoordPointerEXT, QDOM_GUI);
   builtinFunctions.add("glBlendFuncSeparateEXT",       f_glBlendFuncSeparateEXT, QDOM_GUI);
   builtinFunctions.add("glActiveStencilFaceEXT",       f_glActiveStencilFaceEXT, QDOM_GUI);
   builtinFunctions.add("glDepthBoundsEXT",             f_glDepthBoundsEXT, QDOM_GUI);
   builtinFunctions.add("glBlendEquationSeparateEXT",   f_glBlendEquationSeparateEXT, QDOM_GUI);
   builtinFunctions.add("glIsRenderbufferEXT",          f_glIsRenderbufferEXT, QDOM_GUI);
   builtinFunctions.add("glBindRenderbufferEXT",        f_glBindRenderbufferEXT, QDOM_GUI);
   //builtinFunctions.add("glDeleteRenderbuffersEXT",     f_glDeleteRenderbuffersEXT, QDOM_GUI);
   //builtinFunctions.add("glGenRenderbuffersEXT",        f_glGenRenderbuffersEXT, QDOM_GUI);
   builtinFunctions.add("glRenderbufferStorageEXT",     f_glRenderbufferStorageEXT, QDOM_GUI);
   //builtinFunctions.add("glGetRenderbufferParameterivEXT", f_glGetRenderbufferParameterivEXT, QDOM_GUI);
   builtinFunctions.add("glIsFramebufferEXT",           f_glIsFramebufferEXT, QDOM_GUI);
   builtinFunctions.add("glBindFramebufferEXT",         f_glBindFramebufferEXT, QDOM_GUI);
   //builtinFunctions.add("glDeleteFramebuffersEXT",      f_glDeleteFramebuffersEXT, QDOM_GUI);
   builtinFunctions.add("glGenFramebuffersEXT",         f_glGenFramebuffersEXT, QDOM_GUI);
   builtinFunctions.add("glCheckFramebufferStatusEXT",  f_glCheckFramebufferStatusEXT, QDOM_GUI);
   builtinFunctions.add("glFramebufferTexture1DEXT",    f_glFramebufferTexture1DEXT, QDOM_GUI);
   builtinFunctions.add("glFramebufferTexture2DEXT",    f_glFramebufferTexture2DEXT, QDOM_GUI);
   builtinFunctions.add("glFramebufferTexture3DEXT",    f_glFramebufferTexture3DEXT, QDOM_GUI);
   builtinFunctions.add("glFramebufferRenderbufferEXT", f_glFramebufferRenderbufferEXT, QDOM_GUI);
   //builtinFunctions.add("glGetFramebufferAttachmentParameterivEXT", f_glGetFramebufferAttachmentParameterivEXT, QDOM_GUI);
   builtinFunctions.add("glGenerateMipmapEXT",          f_glGenerateMipmapEXT, QDOM_GUI);
   builtinFunctions.add("glBlitFramebufferEXT",         f_glBlitFramebufferEXT, QDOM_GUI);
   builtinFunctions.add("glRenderbufferStorageMultisampleEXT", f_glRenderbufferStorageMultisampleEXT, QDOM_GUI);
   builtinFunctions.add("glProgramParameteriEXT",       f_glProgramParameteriEXT, QDOM_GUI);
   builtinFunctions.add("glFramebufferTextureEXT",      f_glFramebufferTextureEXT, QDOM_GUI);
   builtinFunctions.add("glFramebufferTextureLayerEXT", f_glFramebufferTextureLayerEXT, QDOM_GUI);
   builtinFunctions.add("glFramebufferTextureFaceEXT",  f_glFramebufferTextureFaceEXT, QDOM_GUI);
#ifdef HAVE_GLBINDBUFFEREXT
   builtinFunctions.add("glBindBufferRangeEXT",         f_glBindBufferRangeEXT, QDOM_GUI);
   builtinFunctions.add("glBindBufferOffsetEXT",        f_glBindBufferOffsetEXT, QDOM_GUI);
   builtinFunctions.add("glBindBufferBaseEXT",          f_glBindBufferBaseEXT, QDOM_GUI);
#endif
#ifdef HAVE_GLTRANSFORMFEEDBACKEXT
   builtinFunctions.add("glBeginTransformFeedbackEXT",  f_glBeginTransformFeedbackEXT, QDOM_GUI);
   builtinFunctions.add("glEndTransformFeedbackEXT",    f_glEndTransformFeedbackEXT, QDOM_GUI);
#endif
   //builtinFunctions.add("glTransformFeedbackVaryingsEXT", f_glTransformFeedbackVaryingsEXT, QDOM_GUI);
   //builtinFunctions.add("glGetTransformFeedbackVaryingEXT", f_glGetTransformFeedbackVaryingEXT, QDOM_GUI);
   //builtinFunctions.add("glGetIntegerIndexedvEXT",      f_glGetIntegerIndexedvEXT, QDOM_GUI);
   //builtinFunctions.add("glGetBooleanIndexedvEXT",      f_glGetBooleanIndexedvEXT, QDOM_GUI);
   builtinFunctions.add("glUniformBufferEXT",           f_glUniformBufferEXT, QDOM_GUI);
   builtinFunctions.add("glGetUniformBufferSizeEXT",    f_glGetUniformBufferSizeEXT, QDOM_GUI);
   //builtinFunctions.add("glGetUniformOffsetEXT",        f_glGetUniformOffsetEXT, QDOM_GUI);
   builtinFunctions.add("glClearColorIiEXT",            f_glClearColorIiEXT, QDOM_GUI);
   builtinFunctions.add("glClearColorIuiEXT",           f_glClearColorIuiEXT, QDOM_GUI);
   //builtinFunctions.add("glTexParameterIivEXT",         f_glTexParameterIivEXT, QDOM_GUI);
   //builtinFunctions.add("glTexParameterIuivEXT",        f_glTexParameterIuivEXT, QDOM_GUI);
   //builtinFunctions.add("glGetTexParameterIivEXT",      f_glGetTexParameterIivEXT, QDOM_GUI);
   //builtinFunctions.add("glGetTexParameterIiuvEXT",     f_glGetTexParameterIiuvEXT, QDOM_GUI);
   builtinFunctions.add("glVertexAttribI1iEXT",         f_glVertexAttribI1iEXT, QDOM_GUI);
   builtinFunctions.add("glVertexAttribI2iEXT",         f_glVertexAttribI2iEXT, QDOM_GUI);
   builtinFunctions.add("glVertexAttribI3iEXT",         f_glVertexAttribI3iEXT, QDOM_GUI);
   builtinFunctions.add("glVertexAttribI4iEXT",         f_glVertexAttribI4iEXT, QDOM_GUI);
   builtinFunctions.add("glVertexAttribI1uiEXT",        f_glVertexAttribI1uiEXT, QDOM_GUI);
   builtinFunctions.add("glVertexAttribI2uiEXT",        f_glVertexAttribI2uiEXT, QDOM_GUI);
   builtinFunctions.add("glVertexAttribI3uiEXT",        f_glVertexAttribI3uiEXT, QDOM_GUI);
   builtinFunctions.add("glVertexAttribI4uiEXT",        f_glVertexAttribI4uiEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribI1ivEXT",        f_glVertexAttribI1ivEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribI2ivEXT",        f_glVertexAttribI2ivEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribI3ivEXT",        f_glVertexAttribI3ivEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribI4ivEXT",        f_glVertexAttribI4ivEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribI1uivEXT",       f_glVertexAttribI1uivEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribI2uivEXT",       f_glVertexAttribI2uivEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribI3uivEXT",       f_glVertexAttribI3uivEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribI4uivEXT",       f_glVertexAttribI4uivEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribI4bvEXT",        f_glVertexAttribI4bvEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribI4svEXT",        f_glVertexAttribI4svEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribI4ubvEXT",       f_glVertexAttribI4ubvEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribI4usvEXT",       f_glVertexAttribI4usvEXT, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribIPointerEXT",    f_glVertexAttribIPointerEXT, QDOM_GUI);
   //builtinFunctions.add("glGetVertexAttribIivEXT",      f_glGetVertexAttribIivEXT, QDOM_GUI);
   //builtinFunctions.add("glGetVertexAttribIuivEXT",     f_glGetVertexAttribIuivEXT, QDOM_GUI);
   builtinFunctions.add("glUniform1uiEXT",              f_glUniform1uiEXT, QDOM_GUI);
   builtinFunctions.add("glUniform2uiEXT",              f_glUniform2uiEXT, QDOM_GUI);
   builtinFunctions.add("glUniform3uiEXT",              f_glUniform3uiEXT, QDOM_GUI);
   builtinFunctions.add("glUniform4uiEXT",              f_glUniform4uiEXT, QDOM_GUI);
   //builtinFunctions.add("glUniform1uivEXT",             f_glUniform1uivEXT, QDOM_GUI);
   //builtinFunctions.add("glUniform2uivEXT",             f_glUniform2uivEXT, QDOM_GUI);
   //builtinFunctions.add("glUniform3uivEXT",             f_glUniform3uivEXT, QDOM_GUI);
   //builtinFunctions.add("glUniform4uivEXT",             f_glUniform4uivEXT, QDOM_GUI);
   //builtinFunctions.add("glGetUniformuivEXT",           f_glGetUniformuivEXT, QDOM_GUI);
   //builtinFunctions.add("glBindFragDataLocationEXT",    f_glBindFragDataLocationEXT, QDOM_GUI);
   //builtinFunctions.add("glGetFragDataLocationEXT",     f_glGetFragDataLocationEXT, QDOM_GUI);
#ifdef HAVE_GL_APPLE_FUNCS
   //builtinFunctions.add("glTextureRangeAPPLE",          f_glTextureRangeAPPLE, QDOM_GUI);
   //builtinFunctions.add("glGetTexParameterPointervAPPLE", f_glGetTexParameterPointervAPPLE, QDOM_GUI);
   //builtinFunctions.add("glVertexArrayRangeAPPLE",      f_glVertexArrayRangeAPPLE, QDOM_GUI);
   //builtinFunctions.add("glFlushVertexArrayRangeAPPLE", f_glFlushVertexArrayRangeAPPLE, QDOM_GUI);
   builtinFunctions.add("glVertexArrayParameteriAPPLE", f_glVertexArrayParameteriAPPLE, QDOM_GUI);
   builtinFunctions.add("glBindVertexArrayAPPLE",       f_glBindVertexArrayAPPLE, QDOM_GUI);
   //builtinFunctions.add("glDeleteVertexArraysAPPLE",    f_glDeleteVertexArraysAPPLE, QDOM_GUI);
   //builtinFunctions.add("glGenVertexArraysAPPLE",       f_glGenVertexArraysAPPLE, QDOM_GUI);
   builtinFunctions.add("glIsVertexArrayAPPLE",         f_glIsVertexArrayAPPLE, QDOM_GUI);
   //builtinFunctions.add("glGenFencesAPPLE",             f_glGenFencesAPPLE, QDOM_GUI);
   //builtinFunctions.add("glDeleteFencesAPPLE",          f_glDeleteFencesAPPLE, QDOM_GUI);
   builtinFunctions.add("glSetFenceAPPLE",              f_glSetFenceAPPLE, QDOM_GUI);
   builtinFunctions.add("glIsFenceAPPLE",               f_glIsFenceAPPLE, QDOM_GUI);
   builtinFunctions.add("glTestFenceAPPLE",             f_glTestFenceAPPLE, QDOM_GUI);
   builtinFunctions.add("glFinishFenceAPPLE",           f_glFinishFenceAPPLE, QDOM_GUI);
   builtinFunctions.add("glTestObjectAPPLE",            f_glTestObjectAPPLE, QDOM_GUI);
   builtinFunctions.add("glFinishObjectAPPLE",          f_glFinishObjectAPPLE, QDOM_GUI);
   //builtinFunctions.add("glElementPointerAPPLE",        f_glElementPointerAPPLE, QDOM_GUI);
   builtinFunctions.add("glDrawElementArrayAPPLE",      f_glDrawElementArrayAPPLE, QDOM_GUI);
   builtinFunctions.add("glDrawRangeElementArrayAPPLE", f_glDrawRangeElementArrayAPPLE, QDOM_GUI);
   //builtinFunctions.add("glMultiDrawElementArrayAPPLE", f_glMultiDrawElementArrayAPPLE, QDOM_GUI);
   //builtinFunctions.add("glMultiDrawRangeElementArrayAPPLE", f_glMultiDrawRangeElementArrayAPPLE, QDOM_GUI);
   builtinFunctions.add("glFlushRenderAPPLE",           f_glFlushRenderAPPLE, QDOM_GUI);
   builtinFunctions.add("glFinishRenderAPPLE",          f_glFinishRenderAPPLE, QDOM_GUI);
   builtinFunctions.add("glSwapAPPLE",                  f_glSwapAPPLE, QDOM_GUI);
   builtinFunctions.add("glEnableVertexAttribAPPLE",    f_glEnableVertexAttribAPPLE, QDOM_GUI);
   builtinFunctions.add("glDisableVertexAttribAPPLE",   f_glDisableVertexAttribAPPLE, QDOM_GUI);
   builtinFunctions.add("glIsVertexAttribEnabledAPPLE", f_glIsVertexAttribEnabledAPPLE, QDOM_GUI);
   //builtinFunctions.add("glMapVertexAttrib1dAPPLE",     f_glMapVertexAttrib1dAPPLE, QDOM_GUI);
   //builtinFunctions.add("glMapVertexAttrib1fAPPLE",     f_glMapVertexAttrib1fAPPLE, QDOM_GUI);
   //builtinFunctions.add("glMapVertexAttrib2dAPPLE",     f_glMapVertexAttrib2dAPPLE, QDOM_GUI);
   //builtinFunctions.add("glMapVertexAttrib2fAPPLE",     f_glMapVertexAttrib2fAPPLE, QDOM_GUI);
   builtinFunctions.add("glBufferParameteriAPPLE",      f_glBufferParameteriAPPLE, QDOM_GUI);
   builtinFunctions.add("glFlushMappedBufferRangeAPPLE", f_glFlushMappedBufferRangeAPPLE, QDOM_GUI);
   builtinFunctions.add("glObjectPurgeableAPPLE",       f_glObjectPurgeableAPPLE, QDOM_GUI);
   builtinFunctions.add("glObjectUnpurgeableAPPLE",     f_glObjectUnpurgeableAPPLE, QDOM_GUI);
   //builtinFunctions.add("glGetObjectParameterivAPPLE",  f_glGetObjectParameterivAPPLE, QDOM_GUI);
#endif
   builtinFunctions.add("glPNTrianglesiATI",            f_glPNTrianglesiATI, QDOM_GUI);
   builtinFunctions.add("glPNTrianglesfATI",            f_glPNTrianglesfATI, QDOM_GUI);
#ifdef HAVE_GLBLENDEQUATIONSEPARATEATI
   builtinFunctions.add("glBlendEquationSeparateATI",   f_glBlendEquationSeparateATI, QDOM_GUI);
#endif
   builtinFunctions.add("glStencilOpSeparateATI",       f_glStencilOpSeparateATI, QDOM_GUI);
   builtinFunctions.add("glStencilFuncSeparateATI",     f_glStencilFuncSeparateATI, QDOM_GUI);
#ifdef HAVE_GLPNTRIANGLESATI
   builtinFunctions.add("glPNTrianglesiATIX",           f_glPNTrianglesiATIX, QDOM_GUI);
   builtinFunctions.add("glPNTrianglesfATIX",           f_glPNTrianglesfATIX, QDOM_GUI);
#endif
   //builtinFunctions.add("glCombinerParameterfvNV",      f_glCombinerParameterfvNV, QDOM_GUI);
   builtinFunctions.add("glCombinerParameterfNV",       f_glCombinerParameterfNV, QDOM_GUI);
   //builtinFunctions.add("glCombinerParameterivNV",      f_glCombinerParameterivNV, QDOM_GUI);
   builtinFunctions.add("glCombinerParameteriNV",       f_glCombinerParameteriNV, QDOM_GUI);
   builtinFunctions.add("glCombinerInputNV",            f_glCombinerInputNV, QDOM_GUI);
   builtinFunctions.add("glCombinerOutputNV",           f_glCombinerOutputNV, QDOM_GUI);
   builtinFunctions.add("glFinalCombinerInputNV",       f_glFinalCombinerInputNV, QDOM_GUI);
   //builtinFunctions.add("glGetCombinerInputParameterfvNV", f_glGetCombinerInputParameterfvNV, QDOM_GUI);
   //builtinFunctions.add("glGetCombinerInputParameterivNV", f_glGetCombinerInputParameterivNV, QDOM_GUI);
   //builtinFunctions.add("glGetCombinerOutputParameterfvNV", f_glGetCombinerOutputParameterfvNV, QDOM_GUI);
   //builtinFunctions.add("glGetCombinerOutputParameterivNV", f_glGetCombinerOutputParameterivNV, QDOM_GUI);
   //builtinFunctions.add("glGetFinalCombinerInputParameterfvNV", f_glGetFinalCombinerInputParameterfvNV, QDOM_GUI);
   //builtinFunctions.add("glGetFinalCombinerInputParameterivNV", f_glGetFinalCombinerInputParameterivNV, QDOM_GUI);
   //builtinFunctions.add("glCombinerStageParameterfvNV", f_glCombinerStageParameterfvNV, QDOM_GUI);
   //builtinFunctions.add("glGetCombinerStageParameterfvNV", f_glGetCombinerStageParameterfvNV, QDOM_GUI);
   builtinFunctions.add("glPointParameteriNV",          f_glPointParameteriNV, QDOM_GUI);
   //builtinFunctions.add("glPointParameterivNV",         f_glPointParameterivNV, QDOM_GUI);
}
