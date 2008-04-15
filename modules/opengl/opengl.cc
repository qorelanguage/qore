/*
  opengl.cc
  
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

static QoreStringNode *opengl_module_init();
static void opengl_module_ns_init(QoreNamespace *rns, QoreNamespace *qns);
static void opengl_module_delete();

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "opengl";
DLLEXPORT char qore_module_version[] = "0.1";
DLLEXPORT char qore_module_description[] = "OpenGL module";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://qore.sourceforge.net";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = opengl_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = opengl_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = opengl_module_delete;
#endif

QoreNamespace opengl_ns("OpenGL");

// GLuint glGenLists( GLsizei range )
static AbstractQoreNode *f_glGenLists(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLuint rc = glGenLists(p ? p->getAsBigInt() : 0);
   return new QoreBigIntNode(rc);
}

//void glNewList( GLuint list, GLenum mode )
static AbstractQoreNode *f_glNewList(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned list = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   glNewList(list, mode);
   return 0;
}

//void glDeleteLists( GLuint list, GLsizei range )
static AbstractQoreNode *f_glDeleteLists(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned list = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLsizei range = (GLsizei)(p ? p->getAsInt() : 0);
   glDeleteLists(list, range);
   return 0;
}

// void glBegin( GLenum mode )
static AbstractQoreNode *f_glBegin(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   glBegin(p ? p->getAsInt() : 0);
   return 0;
}

// void glEnd( void )
static AbstractQoreNode *f_glEnd(const QoreListNode *params, ExceptionSink *xsink)
{
   glEnd();
   return 0;
}

// GLenum glGetError( void )
static AbstractQoreNode *f_glGetError(const QoreListNode *params, ExceptionSink *xsink)
{
   GLenum rc = glGetError();
   return new QoreBigIntNode(rc);
}

//void glTexCoord1d ( GLdouble s )
static AbstractQoreNode *f_glTexCoord1d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble s = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glTexCoord1d(s);
   return 0;
}

//void glTexCoord1f ( GLfloat s )
static AbstractQoreNode *f_glTexCoord1f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat s = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glTexCoord1f(s);
   return 0;
}

//void glTexCoord1i ( GLint s )
static AbstractQoreNode *f_glTexCoord1i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint s = (GLint)(p ? p->getAsInt() : 0);
   glTexCoord1i(s);
   return 0;
}

//void glTexCoord1s ( GLshort s )
static AbstractQoreNode *f_glTexCoord1s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort s = (GLshort)(p ? p->getAsInt() : 0);
   glTexCoord1s(s);
   return 0;
}

//void glTexCoord2d ( GLdouble s, GLdouble t )
static AbstractQoreNode *f_glTexCoord2d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble s = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble t = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glTexCoord2d(s, t);
   return 0;
}

//void glTexCoord2f ( GLfloat s, GLfloat t )
static AbstractQoreNode *f_glTexCoord2f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat s = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat t = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glTexCoord2f(s, t);
   return 0;
}

//void glTexCoord2i ( GLint s, GLint t )
static AbstractQoreNode *f_glTexCoord2i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint s = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint t = (GLint)(p ? p->getAsInt() : 0);
   glTexCoord2i(s, t);
   return 0;
}

//void glTexCoord2s ( GLshort s, GLshort t )
static AbstractQoreNode *f_glTexCoord2s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort s = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort t = (GLshort)(p ? p->getAsInt() : 0);
   glTexCoord2s(s, t);
   return 0;
}

//void glTexCoord3d ( GLdouble s, GLdouble t, GLdouble r )
static AbstractQoreNode *f_glTexCoord3d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble s = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble t = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble r = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glTexCoord3d(s, t, r);
   return 0;
}

//void glTexCoord3f ( GLfloat s, GLfloat t, GLfloat r )
static AbstractQoreNode *f_glTexCoord3f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat s = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat t = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat r = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glTexCoord3f(s, t, r);
   return 0;
}

//void glTexCoord3i ( GLint s, GLint t, GLint r )
static AbstractQoreNode *f_glTexCoord3i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint s = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint t = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint r = (GLint)(p ? p->getAsInt() : 0);
   glTexCoord3i(s, t, r);
   return 0;
}

//void glTexCoord3s ( GLshort s, GLshort t, GLshort r )
static AbstractQoreNode *f_glTexCoord3s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort s = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort t = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort r = (GLshort)(p ? p->getAsInt() : 0);
   glTexCoord3s(s, t, r);
   return 0;
}

//void glTexCoord4d ( GLdouble s, GLdouble t, GLdouble r, GLdouble q )
static AbstractQoreNode *f_glTexCoord4d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble s = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble t = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble r = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble q = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glTexCoord4d(s, t, r, q);
   return 0;
}

//void glTexCoord4f ( GLfloat s, GLfloat t, GLfloat r, GLfloat q )
static AbstractQoreNode *f_glTexCoord4f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat s = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat t = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat r = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat q = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glTexCoord4f(s, t, r, q);
   return 0;
}

//void glTexCoord4i ( GLint s, GLint t, GLint r, GLint q )
static AbstractQoreNode *f_glTexCoord4i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint s = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint t = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint r = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint q = (GLint)(p ? p->getAsInt() : 0);
   glTexCoord4i(s, t, r, q);
   return 0;
}

//void glTexCoord4s ( GLshort s, GLshort t, GLshort r, GLshort q )
static AbstractQoreNode *f_glTexCoord4s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort s = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort t = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort r = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLshort q = (GLshort)(p ? p->getAsInt() : 0);
   glTexCoord4s(s, t, r, q);
   return 0;
}

//void glVertex2d( GLdouble x, GLdouble y )
static AbstractQoreNode *f_glVertex2d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glVertex2d(x, y);
   return 0;
}

//void glVertex2f( GLfloat x, GLfloat y )
static AbstractQoreNode *f_glVertex2f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glVertex2f(x, y);
   return 0;
}

//void glVertex2i( GLint x, GLint y )
static AbstractQoreNode *f_glVertex2i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   glVertex2i(x, y);
   return 0;
}

//void glVertex2s( GLshort x, GLshort y )
static AbstractQoreNode *f_glVertex2s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   glVertex2s(x, y);
   return 0;
}

//void glVertex3d( GLdouble x, GLdouble y, GLdouble z )
static AbstractQoreNode *f_glVertex3d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glVertex3d(x, y, z);
   return 0;
}

//void glVertex3f( GLfloat x, GLfloat y, GLfloat z )
static AbstractQoreNode *f_glVertex3f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glVertex3f(x, y, z);
   return 0;
}

//void glVertex3i( GLint x, GLint y, GLint z )
static AbstractQoreNode *f_glVertex3i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint z = (GLint)(p ? p->getAsInt() : 0);
   glVertex3i(x, y, z);
   return 0;
}

//void glVertex3s( GLshort x, GLshort y, GLshort z )
static AbstractQoreNode *f_glVertex3s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort z = (GLshort)(p ? p->getAsInt() : 0);
   glVertex3s(x, y, z);
   return 0;
}

//void glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
static AbstractQoreNode *f_glVertex4d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble w = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glVertex4d(x, y, z, w);
   return 0;
}

//void glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
static AbstractQoreNode *f_glVertex4f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat w = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glVertex4f(x, y, z, w);
   return 0;
}

//void glVertex4i( GLint x, GLint y, GLint z, GLint w )
static AbstractQoreNode *f_glVertex4i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint z = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint w = (GLint)(p ? p->getAsInt() : 0);
   glVertex4i(x, y, z, w);
   return 0;
}

//void glVertex4s( GLshort x, GLshort y, GLshort z, GLshort w )
static AbstractQoreNode *f_glVertex4s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort z = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLshort w = (GLshort)(p ? p->getAsInt() : 0);
   glVertex4s(x, y, z, w);
   return 0;
}

// void glEndList( void )
static AbstractQoreNode *f_glEndList(const QoreListNode *params, ExceptionSink *xsink)
{
   glEndList();
   return 0;
}

//void glClear( GLbitfield mask )
static AbstractQoreNode *f_glClear(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLbitfield mask = (GLbitfield)(p ? p->getAsInt() : 0);
   glClear(mask);
   return 0;
}

//void glPushAttrib( GLbitfield mask )
static AbstractQoreNode *f_glPushAttrib(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLbitfield mask = (GLbitfield)(p ? p->getAsInt() : 0);
   glPushAttrib(mask);
   return 0;
}

//void glPopAttrib( void )
static AbstractQoreNode *f_glPopAttrib(const QoreListNode *params, ExceptionSink *xsink)
{
   glPopAttrib();
   return 0;
}

//void glMatrixMode( GLenum mode )
static AbstractQoreNode *f_glMatrixMode(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   glMatrixMode(mode);
   return 0;
}

//void glPushMatrix( void )
static AbstractQoreNode *f_glPushMatrix(const QoreListNode *params, ExceptionSink *xsink)
{
   glPushMatrix();
   return 0;
}

//void glPopMatrix( void )
static AbstractQoreNode *f_glPopMatrix(const QoreListNode *params, ExceptionSink *xsink)
{
   glPopMatrix();
   return 0;
}

//void glLoadIdentity( void )
static AbstractQoreNode *f_glLoadIdentity(const QoreListNode *params, ExceptionSink *xsink)
{
   glLoadIdentity();
   return 0;
}

//void glFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
static AbstractQoreNode *f_glFrustum(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble left = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble right = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble bottom = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble top = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLdouble zNear = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 5);
   GLdouble zFar = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glFrustum(left, right, bottom, top, zNear, zFar);
   return 0;
}

//void glTranslated( GLdouble x, GLdouble y, GLdouble z )
static AbstractQoreNode *f_glTranslated(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glTranslated(x, y, z);
   return 0;
}

//void glTranslatef( GLfloat x, GLfloat y, GLfloat z )
static AbstractQoreNode *f_glTranslatef(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glTranslatef(x, y, z);
   return 0;
}

//void glViewport( GLint x, GLint y, GLsizei width, GLsizei height )
static AbstractQoreNode *f_glViewport(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   glViewport(x, y, width, height);
   return 0;
}

//void glEnable( GLenum cap )
static AbstractQoreNode *f_glEnable(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum cap = (GLenum)(p ? p->getAsInt() : 0);
   glEnable(cap);
   return 0;
}

//void glDisable( GLenum cap )
static AbstractQoreNode *f_glDisable(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum cap = (GLenum)(p ? p->getAsInt() : 0);
   glDisable(cap);
   return 0;
}

//void glBlendFunc( GLenum sfactor, GLenum dfactor )
static AbstractQoreNode *f_glBlendFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum sfactor = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum dfactor = (GLenum)(p ? p->getAsInt() : 0);
   glBlendFunc(sfactor, dfactor);
   return 0;
}

//void glBindTexture( GLenum target, GLuint texture )
static AbstractQoreNode *f_glBindTexture(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned texture = p ? p->getAsBigInt() : 0;
   glBindTexture(target, texture);
   return 0;
}

//void glScaled( GLdouble x, GLdouble y, GLdouble z )
static AbstractQoreNode *f_glScaled(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glScaled(x, y, z);
   return 0;
}

//void glScalef( GLfloat x, GLfloat y, GLfloat z )
static AbstractQoreNode *f_glScalef(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glScalef(x, y, z);
   return 0;
}

//void glColor3b( GLbyte red, GLbyte green, GLbyte blue )
static AbstractQoreNode *f_glColor3b(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLbyte red = (GLbyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLbyte green = (GLbyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLbyte blue = (GLbyte)(p ? p->getAsInt() : 0);
   glColor3b(red, green, blue);
   return 0;
}

//void glColor3d( GLdouble red, GLdouble green, GLdouble blue )
static AbstractQoreNode *f_glColor3d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble red = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble green = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble blue = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glColor3d(red, green, blue);
   return 0;
}

//void glColor3f( GLfloat red, GLfloat green, GLfloat blue )
static AbstractQoreNode *f_glColor3f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat red = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat green = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat blue = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glColor3f(red, green, blue);
   return 0;
}

//void glColor3i( GLint red, GLint green, GLint blue )
static AbstractQoreNode *f_glColor3i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint red = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint green = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint blue = (GLint)(p ? p->getAsInt() : 0);
   glColor3i(red, green, blue);
   return 0;
}

//void glColor3s( GLshort red, GLshort green, GLshort blue )
static AbstractQoreNode *f_glColor3s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort red = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort green = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort blue = (GLshort)(p ? p->getAsInt() : 0);
   glColor3s(red, green, blue);
   return 0;
}

//void glColor3ub( GLubyte red, GLubyte green, GLubyte blue )
static AbstractQoreNode *f_glColor3ub(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLubyte red = (GLubyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLubyte green = (GLubyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLubyte blue = (GLubyte)(p ? p->getAsInt() : 0);
   glColor3ub(red, green, blue);
   return 0;
}

//void glColor3ui( GLuint red, GLuint green, GLuint blue )
static AbstractQoreNode *f_glColor3ui(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned red = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned green = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned blue = p ? p->getAsBigInt() : 0;
   glColor3ui(red, green, blue);
   return 0;
}

//void glColor3us( GLushort red, GLushort green, GLushort blue )
static AbstractQoreNode *f_glColor3us(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLushort red = (GLushort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLushort green = (GLushort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLushort blue = (GLushort)(p ? p->getAsInt() : 0);
   glColor3us(red, green, blue);
   return 0;
}

//void glColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha )
static AbstractQoreNode *f_glColor4b(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLbyte red = (GLbyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLbyte green = (GLbyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLbyte blue = (GLbyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLbyte alpha = (GLbyte)(p ? p->getAsInt() : 0);
   glColor4b(red, green, blue, alpha);
   return 0;
}

//void glColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha )
static AbstractQoreNode *f_glColor4d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble red = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble green = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble blue = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble alpha = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glColor4d(red, green, blue, alpha);
   return 0;
}

//void glColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
static AbstractQoreNode *f_glColor4f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat red = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat green = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat blue = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat alpha = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glColor4f(red, green, blue, alpha);
   return 0;
}

//void glColor4i( GLint red, GLint green, GLint blue, GLint alpha )
static AbstractQoreNode *f_glColor4i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint red = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint green = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint blue = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint alpha = (GLint)(p ? p->getAsInt() : 0);
   glColor4i(red, green, blue, alpha);
   return 0;
}

//void glColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha )
static AbstractQoreNode *f_glColor4s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort red = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort green = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort blue = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLshort alpha = (GLshort)(p ? p->getAsInt() : 0);
   glColor4s(red, green, blue, alpha);
   return 0;
}

//void glColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
static AbstractQoreNode *f_glColor4ub(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLubyte red = (GLubyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLubyte green = (GLubyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLubyte blue = (GLubyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLubyte alpha = (GLubyte)(p ? p->getAsInt() : 0);
   glColor4ub(red, green, blue, alpha);
   return 0;
}

//void glColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha )
static AbstractQoreNode *f_glColor4ui(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned red = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned green = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned blue = p ? p->getAsBigInt() : 0;
   p = get_param(params, 3);
   unsigned alpha = p ? p->getAsBigInt() : 0;
   glColor4ui(red, green, blue, alpha);
   return 0;
}

//void glColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha )
static AbstractQoreNode *f_glColor4us(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLushort red = (GLushort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLushort green = (GLushort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLushort blue = (GLushort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLushort alpha = (GLushort)(p ? p->getAsInt() : 0);
   glColor4us(red, green, blue, alpha);
   return 0;
}

//void glCallList( GLuint list )
static AbstractQoreNode *f_glCallList(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned list = p ? p->getAsBigInt() : 0;
   glCallList(list);
   return 0;
}

//void glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z )
static AbstractQoreNode *f_glRotated(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble angle = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glRotated(angle, x, y, z);
   return 0;
}

//void glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
static AbstractQoreNode *f_glRotatef(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat angle = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glRotatef(angle, x, y, z);
   return 0;
}

//void glDepthFunc( GLenum func )
static AbstractQoreNode *f_glDepthFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum func = (GLenum)(p ? p->getAsInt() : 0);
   glDepthFunc(func);
   return 0;
}

//void glNormal3b( GLbyte nx, GLbyte ny, GLbyte nz )
static AbstractQoreNode *f_glNormal3b(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLbyte nx = (GLbyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLbyte ny = (GLbyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLbyte nz = (GLbyte)(p ? p->getAsInt() : 0);
   glNormal3b(nx, ny, nz);
   return 0;
}

//void glNormal3d( GLdouble nx, GLdouble ny, GLdouble nz )
static AbstractQoreNode *f_glNormal3d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble nx = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble ny = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble nz = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glNormal3d(nx, ny, nz);
   return 0;
}

//void glNormal3f( GLfloat nx, GLfloat ny, GLfloat nz )
static AbstractQoreNode *f_glNormal3f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat nx = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat ny = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat nz = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glNormal3f(nx, ny, nz);
   return 0;
}

//void glNormal3i( GLint nx, GLint ny, GLint nz )
static AbstractQoreNode *f_glNormal3i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint nx = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint ny = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint nz = (GLint)(p ? p->getAsInt() : 0);
   glNormal3i(nx, ny, nz);
   return 0;
}

//void glNormal3s( GLshort nx, GLshort ny, GLshort nz )
static AbstractQoreNode *f_glNormal3s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort nx = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort ny = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort nz = (GLshort)(p ? p->getAsInt() : 0);
   glNormal3s(nx, ny, nz);
   return 0;
}

//void glLightf( GLenum light, GLenum pname, GLfloat param )
static AbstractQoreNode *f_glLightf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum light = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glLightf(light, pname, param);
   return 0;
}

//void glLighti( GLenum light, GLenum pname, GLint param )
static AbstractQoreNode *f_glLighti(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum light = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint param = (GLint)(p ? p->getAsInt() : 0);
   glLighti(light, pname, param);
   return 0;
}

//void glMaterialf( GLenum face, GLenum pname, GLfloat param )
static AbstractQoreNode *f_glMaterialf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum face = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glMaterialf(face, pname, param);
   return 0;
}

//void glMateriali( GLenum face, GLenum pname, GLint param )
static AbstractQoreNode *f_glMateriali(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum face = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint param = (GLint)(p ? p->getAsInt() : 0);
   glMateriali(face, pname, param);
   return 0;
}

//void glClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
static AbstractQoreNode *f_glClearColor(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLclampf red = (GLclampf)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLclampf green = (GLclampf)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLclampf blue = (GLclampf)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLclampf alpha = (GLclampf)(p ? p->getAsFloat() : 0.0);
   glClearColor(red, green, blue, alpha);
   return 0;
}

static int get_num_light_args(GLenum name)
{
   switch (name)
   {
      case GL_AMBIENT:
      case GL_DIFFUSE:
      case GL_SPECULAR:
      case GL_POSITION:
	 return 4;

      case GL_SPOT_DIRECTION:
	 return 3;

      case GL_SPOT_EXPONENT:
      case GL_SPOT_CUTOFF:
      case GL_CONSTANT_ATTENUATION:
      case GL_LINEAR_ATTENUATION:
      case GL_QUADRATIC_ATTENUATION:
	 return 1;
   }
   return -1;
}

static AbstractQoreNode *f_glLightfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum light = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   const QoreListNode *l = test_list_param(params, 2);
   if (!l) {
      xsink->raiseException("GLLIGHTFV-ERROR", "expecing a list as third argument");
      return 0;
   }

   int num = get_num_light_args(pname);
   if (num == -1) {
      xsink->raiseException("GLLIGHTFV-ERROR", "unrecognized light parameter code %d", (int)pname);
      return 0;      
   }

   GLfloat a[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *p = l->retrieve_entry(i);
      a[i] = p ? p->getAsFloat() : 0.0;
   }

   glLightfv(light, pname, a);
   return 0;
}

static AbstractQoreNode *f_glLightiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum light = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   const QoreListNode *l = test_list_param(params, 2);
   if (!l) {
      xsink->raiseException("GLLIGHTFV-ERROR", "expecing a list as third argument");
      return 0;
   }

   int num = get_num_light_args(pname);
   if (num == -1) {
      xsink->raiseException("GLLIGHTFV-ERROR", "unrecognized light parameter code %d", (int)pname);
      return 0;      
   }

   GLint a[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *p = l->retrieve_entry(i);
      a[i] = p ? p->getAsBigInt() : 0;
   }

   glLightiv(light, pname, a);
   return 0;
}

static int get_num_material_args(GLenum name)
{
   switch (name)
   {
      case GL_AMBIENT:
      case GL_DIFFUSE:
      case GL_SPECULAR:
      case GL_EMISSION:
      case GL_AMBIENT_AND_DIFFUSE:
	 return 4;

      case GL_COLOR_INDEXES:
	 return 3;

      case GL_SHININESS:
	 return 1;
   }
   return -1;
}

static AbstractQoreNode *f_glMaterialfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum material = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   const QoreListNode *l = test_list_param(params, 2);
   if (!l) {
      xsink->raiseException("GLMATERIALFV-ERROR", "expecing a list as third argument");
      return 0;
   }

   int num = get_num_material_args(pname);
   if (num == -1) {
      xsink->raiseException("GLMATERIALFV-ERROR", "unrecognized material parameter code %d", (int)pname);
      return 0;      
   }

   GLfloat a[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *p = l->retrieve_entry(i);
      a[i] = p ? p->getAsFloat() : 0.0;
   }

   glMaterialfv(material, pname, a);
   return 0;
}

static AbstractQoreNode *f_glMaterialiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum material = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   const QoreListNode *l = test_list_param(params, 2);
   if (!l) {
      xsink->raiseException("GLMATERIALFV-ERROR", "expecing a list as third argument");
      return 0;
   }

   int num = get_num_material_args(pname);
   if (num == -1) {
      xsink->raiseException("GLMATERIALFV-ERROR", "unrecognized material parameter code %d", (int)pname);
      return 0;      
   }

   GLint a[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *p = l->retrieve_entry(i);
      a[i] = p ? p->getAsBigInt() : 0;
   }

   glMaterialiv(material, pname, a);
   return 0;
}

static int get_num_lightmodel_args(GLenum name)
{
   switch (name)
   {
      case GL_LIGHT_MODEL_AMBIENT:
	 return 4;

      case GL_LIGHT_MODEL_COLOR_CONTROL:
      case GL_LIGHT_MODEL_LOCAL_VIEWER:
      case GL_LIGHT_MODEL_TWO_SIDE:
	 return 1;
   }
   return -1;
}

static AbstractQoreNode *f_glLightModelfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   const QoreListNode *l = test_list_param(params, 1);
   if (!l) {
      xsink->raiseException("GLLIGHTMODELFV-ERROR", "expecing a list as second argument");
      return 0;
   }

   int num = get_num_lightmodel_args(pname);
   if (num == -1) {
      xsink->raiseException("GLLIGHTMODELFV-ERROR", "unrecognized lightmodel parameter code %d", (int)pname);
      return 0;      
   }

   GLfloat a[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *p = l->retrieve_entry(i);
      a[i] = p ? p->getAsFloat() : 0.0;
   }

   glLightModelfv(pname, a);
   return 0;
}

static AbstractQoreNode *f_glLightModeliv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   const QoreListNode *l = test_list_param(params, 1);
   if (!l) {
      xsink->raiseException("GLLIGHTMODELFV-ERROR", "expecing a list as second argument");
      return 0;
   }

   int num = get_num_lightmodel_args(pname);
   if (num == -1) {
      xsink->raiseException("GLLIGHTMODELFV-ERROR", "unrecognized lightmodel parameter code %d", (int)pname);
      return 0;
   }

   GLint a[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *p = l->retrieve_entry(i);
      a[i] = p ? p->getAsBigInt() : 0;
   }

   glLightModeliv(pname, a);
   return 0;
}

//void glLightModelf( GLenum pname, GLfloat param )
static AbstractQoreNode *f_glLightModelf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glLightModelf(pname, param);
   return 0;
}

//void glLightModeli( GLenum pname, GLint param )
static AbstractQoreNode *f_glLightModeli(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint param = (GLint)(p ? p->getAsInt() : 0);
   glLightModeli(pname, param);
   return 0;
}

//void glShadeModel( GLenum mode )
static AbstractQoreNode *f_glShadeModel(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   glShadeModel(mode);
   return 0;
}

//void glOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val )
static AbstractQoreNode *f_glOrtho(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble left = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble right = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble bottom = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble top = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLdouble near_val = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 5);
   GLdouble far_val = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glOrtho(left, right, bottom, top, near_val, far_val);
   return 0;
}

//void glHint( GLenum target, GLenum mode )
static AbstractQoreNode *f_glHint(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   glHint(target, mode);
   return 0;
}

//void glClearDepth( GLclampd depth )
static AbstractQoreNode *f_glClearDepth(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLclampd depth = (GLclampd)(p ? p->getAsInt() : 0);
   glClearDepth(depth);
   return 0;
}

//void glFlush( void )
static AbstractQoreNode *f_glFlush(const QoreListNode *params, ExceptionSink *xsink)
{
   glFlush();
   return 0;
}

static int get_num_var_size(GLenum name)
{
   switch (name)
   {
      case GL_ACCUM_ALPHA_BITS:
      case GL_ACCUM_BLUE_BITS:
      case GL_ACCUM_GREEN_BITS:
      case GL_ACCUM_RED_BITS:
      case GL_ACTIVE_TEXTURE_ARB:
      case GL_ALPHA_BIAS:
      case GL_ALPHA_BITS:
      case GL_ALPHA_SCALE:
      case GL_ALPHA_TEST:
      case GL_ALPHA_TEST_FUNC:
      case GL_ALPHA_TEST_REF:
      case GL_ATTRIB_STACK_DEPTH:
      case GL_AUTO_NORMAL:
      case GL_AUX_BUFFERS:
      case GL_BLEND:
      case GL_BLEND_DST:
      case GL_BLEND_EQUATION:
      case GL_BLEND_SRC:
      case GL_BLUE_BIAS:
      case GL_BLUE_BITS:
      case GL_BLUE_SCALE:
      case GL_CLIENT_ACTIVE_TEXTURE_ARB:
      case GL_CLIENT_ATTRIB_STACK_DEPTH:
      case GL_CLIP_PLANE0:
      case GL_CLIP_PLANE1:
      case GL_CLIP_PLANE2:
      case GL_CLIP_PLANE3:
      case GL_CLIP_PLANE4:
      case GL_CLIP_PLANE5:
      case GL_COLOR_ARRAY:
      case GL_COLOR_ARRAY_SIZE:
      case GL_COLOR_ARRAY_STRIDE:
      case GL_COLOR_ARRAY_TYPE:
      case GL_COLOR_LOGIC_OP:
      case GL_COLOR_MATERIAL:
      case GL_COLOR_MATERIAL_FACE:
      case GL_COLOR_MATERIAL_PARAMETER:
      case GL_COLOR_MATRIX_STACK_DEPTH:
      case GL_COLOR_TABLE:
      case GL_CONVOLUTION_1D:
      case GL_CONVOLUTION_2D:
      case GL_CULL_FACE:
      case GL_CULL_FACE_MODE:
      case GL_CURRENT_INDEX:
      case GL_CURRENT_RASTER_DISTANCE:
      case GL_CURRENT_RASTER_INDEX:
      case GL_CURRENT_RASTER_POSITION_VALID:
      case GL_DEPTH_BIAS:
      case GL_DEPTH_BITS:
      case GL_DEPTH_CLEAR_VALUE:
      case GL_DEPTH_FUNC:
      case GL_DEPTH_SCALE:
      case GL_DEPTH_TEST:
      case GL_DEPTH_WRITEMASK:
      case GL_DITHER:
      case GL_DOUBLEBUFFER:
      case GL_DRAW_BUFFER:
      case GL_EDGE_FLAG:
      case GL_EDGE_FLAG_ARRAY:
      case GL_EDGE_FLAG_ARRAY_STRIDE:
      case GL_FEEDBACK_BUFFER_SIZE:
      case GL_FEEDBACK_BUFFER_TYPE:
      case GL_FOG:
      case GL_FOG_DENSITY:
      case GL_FOG_END:
      case GL_FOG_HINT:
      case GL_FOG_INDEX:
      case GL_FOG_MODE:
      case GL_FOG_START:
      case GL_FRONT_FACE:
      case GL_GREEN_BIAS:
      case GL_GREEN_BITS:
      case GL_GREEN_SCALE:
      case GL_HISTOGRAM:
      case GL_INDEX_ARRAY:
      case GL_INDEX_ARRAY_STRIDE:
      case GL_INDEX_ARRAY_TYPE:
      case GL_INDEX_BITS:
      case GL_INDEX_CLEAR_VALUE:
      case GL_INDEX_LOGIC_OP:
      case GL_INDEX_MODE:
      case GL_INDEX_OFFSET:
      case GL_INDEX_SHIFT:
      case GL_INDEX_WRITEMASK:
      case GL_LIGHT0:
      case GL_LIGHT1:
      case GL_LIGHT2:
      case GL_LIGHT3:
      case GL_LIGHT4:
      case GL_LIGHT5:
      case GL_LIGHT6:
      case GL_LIGHT7:
      case GL_LIGHTING:
      case GL_LIGHT_MODEL_COLOR_CONTROL:
      case GL_LIGHT_MODEL_LOCAL_VIEWER:
      case GL_LIGHT_MODEL_TWO_SIDE:
      case GL_LINE_SMOOTH:
      case GL_LINE_SMOOTH_HINT:
      case GL_LINE_STIPPLE:
      case GL_LINE_STIPPLE_PATTERN:
      case GL_LINE_STIPPLE_REPEAT:
      case GL_LINE_WIDTH:
      case GL_LINE_WIDTH_GRANULARITY:
      case GL_LIST_BASE:
      case GL_LIST_INDEX:
      case GL_LIST_MODE:
      case GL_LOGIC_OP_MODE:
      case GL_MAP1_COLOR_4:
      case GL_MAP1_GRID_SEGMENTS:
      case GL_MAP1_INDEX:
      case GL_MAP1_NORMAL:
      case GL_MAP1_TEXTURE_COORD_1:
      case GL_MAP1_TEXTURE_COORD_2:
      case GL_MAP1_TEXTURE_COORD_3:
      case GL_MAP1_TEXTURE_COORD_4:
      case GL_MAP1_VERTEX_3:
      case GL_MAP1_VERTEX_4:
      case GL_MAP2_COLOR_4:
      case GL_MAP2_INDEX:
      case GL_MAP2_NORMAL:
      case GL_MAP2_TEXTURE_COORD_1:
      case GL_MAP2_TEXTURE_COORD_2:
      case GL_MAP2_TEXTURE_COORD_3:
      case GL_MAP2_TEXTURE_COORD_4:
      case GL_MAP2_VERTEX_3:
      case GL_MAP2_VERTEX_4:
      case GL_MAP_COLOR:
      case GL_MAP_STENCIL:
      case GL_MATRIX_MODE:
      case GL_MAX_3D_TEXTURE_SIZE:
      case GL_MAX_CLIENT_ATTRIB_STACK_DEPTH:
      case GL_MAX_ATTRIB_STACK_DEPTH:
      case GL_MAX_CLIP_PLANES:
      case GL_MAX_COLOR_MATRIX_STACK_DEPTH:
      case GL_MAX_ELEMENTS_INDICES:
      case GL_MAX_ELEMENTS_VERTICES:
      case GL_MAX_EVAL_ORDER:
      case GL_MAX_LIGHTS:
      case GL_MAX_LIST_NESTING:
      case GL_MAX_MODELVIEW_STACK_DEPTH:
      case GL_MAX_NAME_STACK_DEPTH:
      case GL_MAX_PIXEL_MAP_TABLE:
      case GL_MAX_PROJECTION_STACK_DEPTH:
      case GL_MAX_TEXTURE_SIZE:
      case GL_MAX_TEXTURE_STACK_DEPTH:
      case GL_MAX_TEXTURE_UNITS_ARB:
      case GL_MINMAX:
      case GL_MODELVIEW_STACK_DEPTH:
      case GL_NAME_STACK_DEPTH:
      case GL_NORMAL_ARRAY:
      case GL_NORMAL_ARRAY_STRIDE:
      case GL_NORMAL_ARRAY_TYPE:
      case GL_NORMALIZE:
      case GL_PACK_ALIGNMENT:
      case GL_PACK_IMAGE_HEIGHT:
      case GL_PACK_LSB_FIRST:
      case GL_PACK_ROW_LENGTH:
      case GL_PACK_SKIP_IMAGES:
      case GL_PACK_SKIP_PIXELS:
      case GL_PACK_SKIP_ROWS:
      case GL_PACK_SWAP_BYTES:
      case GL_PERSPECTIVE_CORRECTION_HINT:
      case GL_PIXEL_MAP_A_TO_A_SIZE:
      case GL_PIXEL_MAP_B_TO_B_SIZE:
      case GL_PIXEL_MAP_G_TO_G_SIZE:
      case GL_PIXEL_MAP_I_TO_A_SIZE:
      case GL_PIXEL_MAP_I_TO_B_SIZE:
      case GL_PIXEL_MAP_I_TO_G_SIZE:
      case GL_PIXEL_MAP_I_TO_I_SIZE:
      case GL_PIXEL_MAP_I_TO_R_SIZE:
      case GL_PIXEL_MAP_R_TO_R_SIZE:
      case GL_PIXEL_MAP_S_TO_S_SIZE:
      case GL_POINT_SIZE:
      case GL_POINT_SIZE_GRANULARITY:
      case GL_POINT_SMOOTH:
      case GL_POINT_SMOOTH_HINT:
      case GL_POLYGON_OFFSET_FACTOR:
      case GL_POLYGON_OFFSET_UNITS:
      case GL_POLYGON_OFFSET_FILL:
      case GL_POLYGON_OFFSET_LINE:
      case GL_POLYGON_OFFSET_POINT:
      case GL_POLYGON_SMOOTH:
      case GL_POLYGON_SMOOTH_HINT:
      case GL_POLYGON_STIPPLE:
      case GL_POST_COLOR_MATRIX_COLOR_TABLE:
      case GL_POST_COLOR_MATRIX_RED_BIAS:
      case GL_POST_COLOR_MATRIX_GREEN_BIAS:
      case GL_POST_COLOR_MATRIX_BLUE_BIAS:
      case GL_POST_COLOR_MATRIX_ALPHA_BIAS:
      case GL_POST_COLOR_MATRIX_RED_SCALE:
      case GL_POST_COLOR_MATRIX_GREEN_SCALE:
      case GL_POST_COLOR_MATRIX_BLUE_SCALE:
      case GL_POST_COLOR_MATRIX_ALPHA_SCALE:
      case GL_POST_CONVOLUTION_COLOR_TABLE:
      case GL_POST_CONVOLUTION_RED_BIAS:
      case GL_POST_CONVOLUTION_GREEN_BIAS:
      case GL_POST_CONVOLUTION_BLUE_BIAS:
      case GL_POST_CONVOLUTION_ALPHA_BIAS:
      case GL_POST_CONVOLUTION_RED_SCALE:
      case GL_POST_CONVOLUTION_GREEN_SCALE:
      case GL_POST_CONVOLUTION_BLUE_SCALE:
      case GL_POST_CONVOLUTION_ALPHA_SCALE:
      case GL_PROJECTION_STACK_DEPTH:
      case GL_READ_BUFFER:
      case GL_RED_BIAS:
      case GL_RED_BITS:
      case GL_RED_SCALE:
      case GL_RENDER_MODE:
      case GL_RESCALE_NORMAL:
      case GL_RGBA_MODE:
      case GL_SCISSOR_TEST:
      case GL_SELECTION_BUFFER_SIZE:
      case GL_SEPARABLE_2D:
      case GL_SHADE_MODEL:
#if GL_SMOOTH_LINE_WIDTH_GRANULARITY != GL_LINE_WIDTH_GRANULARITY
      case GL_SMOOTH_LINE_WIDTH_GRANULARITY:
#endif
#if GL_SMOOTH_POINT_SIZE_GRANULARITY != GL_POINT_SIZE_GRANULARITY
      case GL_SMOOTH_POINT_SIZE_GRANULARITY:
#endif
      case GL_STENCIL_BITS:
      case GL_STENCIL_CLEAR_VALUE:
      case GL_STENCIL_FAIL:
      case GL_STENCIL_FUNC:
      case GL_STENCIL_PASS_DEPTH_FAIL:
      case GL_STENCIL_PASS_DEPTH_PASS:
      case GL_STENCIL_REF:
      case GL_STENCIL_TEST:
      case GL_STENCIL_VALUE_MASK:
      case GL_STENCIL_WRITEMASK:
      case GL_STEREO:
      case GL_SUBPIXEL_BITS:
      case GL_TEXTURE_1D:
      case GL_TEXTURE_BINDING_1D:
      case GL_TEXTURE_2D:
      case GL_TEXTURE_BINDING_2D:
      case GL_TEXTURE_3D:
      case GL_TEXTURE_BINDING_3D:
      case GL_TEXTURE_COORD_ARRAY:
      case GL_TEXTURE_COORD_ARRAY_SIZE:
      case GL_TEXTURE_COORD_ARRAY_STRIDE:
      case GL_TEXTURE_COORD_ARRAY_TYPE:
      case GL_TEXTURE_GEN_Q:
      case GL_TEXTURE_GEN_R:
      case GL_TEXTURE_GEN_S:
      case GL_TEXTURE_GEN_T:
      case GL_TEXTURE_STACK_DEPTH:
      case GL_UNPACK_ALIGNMENT:
      case GL_UNPACK_IMAGE_HEIGHT:
      case GL_UNPACK_LSB_FIRST:
      case GL_UNPACK_ROW_LENGTH:
      case GL_UNPACK_SKIP_IMAGES:
      case GL_UNPACK_SKIP_PIXELS:
      case GL_UNPACK_SKIP_ROWS:
      case GL_UNPACK_SWAP_BYTES:
      case GL_VERTEX_ARRAY:
      case GL_VERTEX_ARRAY_SIZE:
      case GL_VERTEX_ARRAY_STRIDE:
      case GL_VERTEX_ARRAY_TYPE:
      case GL_ZOOM_X:
      case GL_ZOOM_Y:
      case GL_PROGRAM_ERROR_POSITION_ARB:
         return 1;

      case GL_ALIASED_POINT_SIZE_RANGE:
      case GL_ALIASED_LINE_WIDTH_RANGE:
      case GL_DEPTH_RANGE:
      case GL_LINE_WIDTH_RANGE:
      case GL_MAP1_GRID_DOMAIN:
      case GL_MAP2_GRID_SEGMENTS:
      case GL_MAX_VIEWPORT_DIMS:
      case GL_POINT_SIZE_RANGE:
      case GL_POLYGON_MODE:
#if GL_SMOOTH_LINE_WIDTH_RANGE != GL_LINE_WIDTH_RANGE
      case GL_SMOOTH_LINE_WIDTH_RANGE:
#endif
#if GL_SMOOTH_POINT_SIZE_RANGE != GL_POINT_SIZE_RANGE
      case GL_SMOOTH_POINT_SIZE_RANGE:
#endif
         return 2;

      case GL_CURRENT_NORMAL:
         return 3;

      case GL_ACCUM_CLEAR_VALUE:
      case GL_BLEND_COLOR:
      case GL_COLOR_CLEAR_VALUE:
      case GL_COLOR_WRITEMASK:
      case GL_CURRENT_COLOR:
      case GL_CURRENT_RASTER_COLOR:
      case GL_CURRENT_RASTER_POSITION:
      case GL_CURRENT_RASTER_TEXTURE_COORDS:
      case GL_CURRENT_TEXTURE_COORDS:
      case GL_FOG_COLOR:
      case GL_LIGHT_MODEL_AMBIENT:
      case GL_MAP2_GRID_DOMAIN:
      case GL_SCISSOR_BOX:
      case GL_VIEWPORT:
         return 4;

      case GL_COLOR_MATRIX:
      case GL_MODELVIEW_MATRIX:
      case GL_PROJECTION_MATRIX:
      case GL_TEXTURE_MATRIX:
         return 16;
   }
   return -1;
}


//void glGetBooleanv( GLenum pname, GLboolean *params )
static AbstractQoreNode *f_glGetBooleanv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   int num = get_num_var_size(pname);
   if (num == -1) {
      xsink->raiseException("GLGETBOOLEANV-ERROR", "unrecognized parameter code %d", (int)pname);
      return 0;
   }

   GLboolean parms[num];
   glGetBooleanv(pname, parms);

   if (num == 1)
      return get_bool_node(parms[0]);

   QoreListNode *l = new QoreListNode();
   if (num == 16) {
      for (int i = 0; i < 4; ++i) {
	 QoreListNode *m = new QoreListNode();
	 l->push(m);
	 for (int j = 0; j < 4; ++j)
	    m->push(get_bool_node(parms[i*4 + j]));
      }
   }
   else
      for (int i = 0; i < num; ++i)
	 l->push(get_bool_node(parms[i]));

   return l;
}

//void glGetDoublev( GLenum pname, GLdouble *params )
static AbstractQoreNode *f_glGetDoublev(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   int num = get_num_var_size(pname);
   if (num == -1) {
      xsink->raiseException("GLGETDOUBLEV-ERROR", "unrecognized parameter code %d", (int)pname);
      return 0;
   }

   GLdouble parms[num];
   glGetDoublev(pname, parms);

   if (num == 1)
      return new QoreFloatNode(parms[0]);

   QoreListNode *l = new QoreListNode();
   if (num == 16) {
      for (int i = 0; i < 4; ++i) {
	 QoreListNode *m = new QoreListNode();
	 l->push(m);
	 for (int j = 0; j < 4; ++j)
	    m->push(new QoreFloatNode(parms[i*4 + j]));
      }
   }
   else
      for (int i = 0; i < num; ++i)
	 l->push(new QoreFloatNode(parms[i]));

   return l;
}

//void glGetFloatv( GLenum pname, GLfloat *params )
static AbstractQoreNode *f_glGetFloatv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   int num = get_num_var_size(pname);
   if (num == -1) {
      xsink->raiseException("GLGETFLOATV-ERROR", "unrecognized parameter code %d", (int)pname);
      return 0;
   }

   GLfloat parms[num];
   glGetFloatv(pname, parms);

   if (num == 1)
      return new QoreFloatNode(parms[0]);

   QoreListNode *l = new QoreListNode();
   if (num == 16) {
      for (int i = 0; i < 4; ++i) {
	 QoreListNode *m = new QoreListNode();
	 l->push(m);
	 for (int j = 0; j < 4; ++j)
	    m->push(new QoreFloatNode(parms[i*4 + j]));
      }
   }
   else
      for (int i = 0; i < num; ++i)
	 l->push(new QoreFloatNode(parms[i]));

   return l;
}

//void glGetIntegerv( GLenum pname, GLint *params )
static AbstractQoreNode *f_glGetIntegerv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   int num = get_num_var_size(pname);
   if (num == -1) {
      xsink->raiseException("GLGETINTEGERV-ERROR", "unrecognized parameter code %d", (int)pname);
      return 0;
   }

   GLint parms[num];
   glGetIntegerv(pname, parms);

   if (num == 1)
      return new QoreBigIntNode(parms[0]);

   QoreListNode *l = new QoreListNode();
   if (num == 16) {
      for (int i = 0; i < 4; ++i) {
	 QoreListNode *m = new QoreListNode();
	 l->push(m);
	 for (int j = 0; j < 4; ++j)
	    m->push(new QoreBigIntNode(parms[i*4 + j]));
      }
   }
   else
      for (int i = 0; i < num; ++i)
	 l->push(new QoreBigIntNode(parms[i]));

   return l;
}

//void glTexEnvf( GLenum target, GLenum pname, GLfloat param )
static AbstractQoreNode *f_glTexEnvf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glTexEnvf(target, pname, param);
   return 0;
}

//void glTexEnvi( GLenum target, GLenum pname, GLint param )
static AbstractQoreNode *f_glTexEnvi(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint param = (GLint)(p ? p->getAsInt() : 0);
   glTexEnvi(target, pname, param);
   return 0;
}

static int get_num_texenv_args(GLenum pname)
{
   switch (pname) {
      case GL_MODULATE:
      case GL_DECAL:
      case GL_BLEND:
      case GL_REPLACE:
	 return 1;

      case GL_TEXTURE_ENV_COLOR:
	 return 4;
   }
   return -1;
}

//void glTexEnvfv( GLenum target, GLenum pname, const GLfloat *params )
static AbstractQoreNode *f_glTexEnvfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   const QoreListNode *l = test_list_param(params, 2);
   if (!l) {
      xsink->raiseException("GLTEXENVFV-ERROR", "expecing a list as third argument");
      return 0;
   }

   int num = get_num_texenv_args(pname);
   if (num == -1) {
      xsink->raiseException("GLTEXENVFV-ERROR", "unrecognized parameter code %d", (int)pname);
      return 0;      
   }

   GLfloat parms[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *p = l->retrieve_entry(i);
      parms[i] = p ? p->getAsFloat() : 0.0;
   }

   glTexEnvfv(target, pname, parms);
   return 0;
}

//void glTexEnviv( GLenum target, GLenum pname, const GLint *params )
static AbstractQoreNode *f_glTexEnviv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   const QoreListNode *l = test_list_param(params, 2);
   if (!l) {
      xsink->raiseException("GLTEXENVIV-ERROR", "expecing a list as third argument");
      return 0;
   }

   int num = get_num_texenv_args(pname);
   if (num == -1) {
      xsink->raiseException("GLTEXENVIV-ERROR", "unrecognized parameter code %d", (int)pname);
      return 0;      
   }

   GLint parms[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *p = l->retrieve_entry(i);
      parms[i] = p ? p->getAsInt() : 0;
   }

   glTexEnviv(target, pname, parms);
   return 0;
}

//void glAccum (GLenum op, GLfloat value);
static AbstractQoreNode *f_glAccum(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum op = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat value = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glAccum(op, value);
   return 0;
}

//void glAlphaFunc (GLenum func, GLclampf ref);
static AbstractQoreNode *f_glAlphaFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum func = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLclampf ref = (GLclampf)(p ? p->getAsFloat() : 0.0);
   glAlphaFunc(func, ref);
   return 0;
}

/*
//GLboolean glAreTexturesResident (GLsizei n, const GLuint *textures, GLboolean *residences);
static AbstractQoreNode *f_glAreTexturesResident(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* textures = p;
   p = get_param(params, 2);
   ??? GLboolean* residences = p;
   ??? return new QoreBigIntNode(glAreTexturesResident(n, textures, residences));
}
*/

//void glArrayElement (GLint i);
static AbstractQoreNode *f_glArrayElement(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint i = (GLint)(p ? p->getAsInt() : 0);
   glArrayElement(i);
   return 0;
}

/*
//void glBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
static AbstractQoreNode *f_glBitmap(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLfloat xorig = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat yorig = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLfloat xmove = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 5);
   GLfloat ymove = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 6);
   ??? GLubyte* bitmap = p;
   glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);
   return 0;
}
*/

//void glBlendColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
static AbstractQoreNode *f_glBlendColor(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLclampf red = (GLclampf)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLclampf green = (GLclampf)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLclampf blue = (GLclampf)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLclampf alpha = (GLclampf)(p ? p->getAsFloat() : 0.0);
   glBlendColor(red, green, blue, alpha);
   return 0;
}

//void glBlendEquation (GLenum mode);
static AbstractQoreNode *f_glBlendEquation(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   glBlendEquation(mode);
   return 0;
}

//void glBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha);
static AbstractQoreNode *f_glBlendEquationSeparate(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum modeRGB = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum modeAlpha = (GLenum)(p ? p->getAsInt() : 0);
   glBlendEquationSeparate(modeRGB, modeAlpha);
   return 0;
}

/*
//void glCallLists (GLsizei n, GLenum type, const GLvoid *lists);
static AbstractQoreNode *f_glCallLists(const QoreListNode *params, ExceptionSink *xsink)
{
   glCallLists();
   return 0;
}
*/

//void glClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
static AbstractQoreNode *f_glClearAccum(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat red = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat green = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat blue = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat alpha = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glClearAccum(red, green, blue, alpha);
   return 0;
}

//void glClearIndex (GLfloat c);
static AbstractQoreNode *f_glClearIndex(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat c = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glClearIndex(c);
   return 0;
}

//void glClearStencil (GLint s);
static AbstractQoreNode *f_glClearStencil(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint s = (GLint)(p ? p->getAsInt() : 0);
   glClearStencil(s);
   return 0;
}

/*
//void glClipPlane (GLenum plane, const GLdouble *equation);
static AbstractQoreNode *f_glClipPlane(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum plane = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLdouble* equation = p;
   glClipPlane(plane, equation);
   return 0;
}
*/

//void glColor3bv (const GLbyte *v);
static AbstractQoreNode *f_glColor3bv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR3BV-ERROR", "expecting a list as sole argument to glColor3bv()");
      return 0;
   }

   GLbyte v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glColor3bv(v);
   return 0;
}

//void glColor3dv (const GLdouble *v);
static AbstractQoreNode *f_glColor3dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR3DV-ERROR", "expecting a list as sole argument to glColor3dv()");
      return 0;
   }

   GLdouble v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsFloat() : 0.0;
   }

   glColor3dv(v);
   return 0;
}

//void glColor3fv (const GLfloat *v);
static AbstractQoreNode *f_glColor3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR3FV-ERROR", "expecting a list as sole argument to glColor3fv()");
      return 0;
   }

   GLfloat v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsFloat() : 0.0;
   }

   glColor3fv(v);
   return 0;
}

//void glColor3iv (const GLint *v);
static AbstractQoreNode *f_glColor3iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR3IV-ERROR", "expecting a list as sole argument to glColor3iv()");
      return 0;
   }

   GLint v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glColor3iv(v);
   return 0;
}

//void glColor3sv (const GLshort *v);
static AbstractQoreNode *f_glColor3sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR3SV-ERROR", "expecting a list as sole argument to glColor3sv()");
      return 0;
   }

   GLshort v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glColor3sv(v);
   return 0;
}

//void glColor3ubv (const GLubyte *v);
static AbstractQoreNode *f_glColor3ubv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR3UBV-ERROR", "expecting a list as sole argument to glColor3ubv()");
      return 0;
   }

   GLubyte v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glColor3ubv(v);
   return 0;
}

//void glColor3uiv (const GLuint *v);
static AbstractQoreNode *f_glColor3uiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR3UIV-ERROR", "expecting a list as sole argument to glColor3uiv()");
      return 0;
   }

   GLuint v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glColor3uiv(v);
   return 0;
}

//void glColor3usv (const GLushort *v);
static AbstractQoreNode *f_glColor3usv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR3USV-ERROR", "expecting a list as sole argument to glColor3usv()");
      return 0;
   }

   GLushort v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glColor3usv(v);
   return 0;
}

//void glColor4bv (const GLbyte *v);
static AbstractQoreNode *f_glColor4bv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR4BV-ERROR", "expecting a list as sole argument to glColor4bv()");
      return 0;
   }

   GLbyte v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glColor4bv(v);
   return 0;
}

//void glColor4dv (const GLdouble *v);
static AbstractQoreNode *f_glColor4dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR4DV-ERROR", "expecting a list as sole argument to glColor4dv()");
      return 0;
   }

   GLdouble v[4];
   for (int i = 0; i < 4; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsFloat() : 0.0;
   }

   glColor4dv(v);
   return 0;
}

//void glColor4fv (const GLfloat *v);
static AbstractQoreNode *f_glColor4fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR4FV-ERROR", "expecting a list as sole argument to glColor4Fv()");
      return 0;
   }

   GLfloat v[4];
   for (int i = 0; i < 4; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsFloat() : 0.0;
   }

   glColor4fv(v);
   return 0;
}

//void glColor4iv (const GLint *v);
static AbstractQoreNode *f_glColor4iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR4IV-ERROR", "expecting a list as sole argument to glColor4iv()");
      return 0;
   }

   GLint v[4];
   for (int i = 0; i < 4; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glColor4iv(v);
   return 0;
}

//void glColor4sv (const GLshort *v);
static AbstractQoreNode *f_glColor4sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR4SV-ERROR", "expecting a list as sole argument to glColor4sv()");
      return 0;
   }

   GLshort v[4];
   for (int i = 0; i < 4; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glColor4sv(v);
   return 0;
}

//void glColor4ubv (const GLubyte *v);
static AbstractQoreNode *f_glColor4ubv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR4UBV-ERROR", "expecting a list as sole argument to glColor4ubv()");
      return 0;
   }

   GLubyte v[4];
   for (int i = 0; i < 4; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glColor4ubv(v);
   return 0;
}

//void glColor4uiv (const GLuint *v);
static AbstractQoreNode *f_glColor4uiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR4UIV-ERROR", "expecting a list as sole argument to glColor4uiv()");
      return 0;
   }

   GLuint v[4];
   for (int i = 0; i < 4; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glColor4uiv(v);
   return 0;
}

//void glColor4usv (const GLushort *v);
static AbstractQoreNode *f_glColor4usv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLCOLOR4USV-ERROR", "expecting a list as sole argument to glColor4usv()");
      return 0;
   }

   GLushort v[4];
   for (int i = 0; i < 4; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glColor4usv(v);
   return 0;
}

//void glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
static AbstractQoreNode *f_glColorMask(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLboolean red = (GLboolean)(p ? p->getAsBool() : 0);
   p = get_param(params, 1);
   GLboolean green = (GLboolean)(p ? p->getAsBool() : 0);
   p = get_param(params, 2);
   GLboolean blue = (GLboolean)(p ? p->getAsBool() : 0);
   p = get_param(params, 3);
   GLboolean alpha = (GLboolean)(p ? p->getAsBool() : 0);
   glColorMask(red, green, blue, alpha);
   return 0;
}

//void glColorMaterial (GLenum face, GLenum mode);
static AbstractQoreNode *f_glColorMaterial(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum face = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   glColorMaterial(face, mode);
   return 0;
}

/*
//void glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
static AbstractQoreNode *f_glColorPointer(const QoreListNode *params, ExceptionSink *xsink)
{
   glColorPointer();
   return 0;
}
*/

 /*
//void glColorSubTable (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data);
static AbstractQoreNode *f_glColorSubTable(const QoreListNode *params, ExceptionSink *xsink)
{
   glColorSubTable();
   return 0;
}
*/

/*
//void glColorTable (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table);
static AbstractQoreNode *f_glColorTable(const QoreListNode *params, ExceptionSink *xsink)
{
   glColorTable();
   return 0;
}
*/

/*
//void glColorTableParameterfv (GLenum target, GLenum pname, const GLfloat *params);
static AbstractQoreNode *f_glColorTableParameterfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glColorTableParameterfv(target, pname, params);
   return 0;
}
*/

/*
//void glColorTableParameteriv (GLenum target, GLenum pname, const GLint *params);
static AbstractQoreNode *f_glColorTableParameteriv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glColorTableParameteriv(target, pname, params);
   return 0;
}
*/

/*
//void glConvolutionFilter1D (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image);
static AbstractQoreNode *f_glConvolutionFilter1D(const QoreListNode *params, ExceptionSink *xsink)
{
   glConvolutionFilter1D();
   return 0;
}
*/

/*
//void glConvolutionFilter2D (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image);
static AbstractQoreNode *f_glConvolutionFilter2D(const QoreListNode *params, ExceptionSink *xsink)
{
   glConvolutionFilter2D();
   return 0;
}
*/

//void glConvolutionParameterf (GLenum target, GLenum pname, GLfloat params);
static AbstractQoreNode *f_glConvolutionParameterf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLfloat parms = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glConvolutionParameterf(target, pname, parms);
   return 0;
}

/*
//void glConvolutionParameterfv (GLenum target, GLenum pname, const GLfloat *params);
static AbstractQoreNode *f_glConvolutionParameterfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glConvolutionParameterfv(target, pname, params);
   return 0;
}
*/

//void glConvolutionParameteri (GLenum target, GLenum pname, GLint params);
static AbstractQoreNode *f_glConvolutionParameteri(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint parms = (GLint)(p ? p->getAsInt() : 0);
   glConvolutionParameteri(target, pname, parms);
   return 0;
}

/*
//void glConvolutionParameteriv (GLenum target, GLenum pname, const GLint *params);
static AbstractQoreNode *f_glConvolutionParameteriv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glConvolutionParameteriv(target, pname, params);
   return 0;
}
*/

//void glCopyColorSubTable (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width);
static AbstractQoreNode *f_glCopyColorSubTable(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei start = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   glCopyColorSubTable(target, start, x, y, width);
   return 0;
}

//void glCopyColorTable (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
static AbstractQoreNode *f_glCopyColorTable(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   glCopyColorTable(target, internalformat, x, y, width);
   return 0;
}

//void glCopyConvolutionFilter1D (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
static AbstractQoreNode *f_glCopyConvolutionFilter1D(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   glCopyConvolutionFilter1D(target, internalformat, x, y, width);
   return 0;
}

//void glCopyConvolutionFilter2D (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height);
static AbstractQoreNode *f_glCopyConvolutionFilter2D(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   glCopyConvolutionFilter2D(target, internalformat, x, y, width, height);
   return 0;
}

//void glCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
static AbstractQoreNode *f_glCopyPixels(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   glCopyPixels(x, y, width, height, type);
   return 0;
}

//void glCopyTexImage1D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
static AbstractQoreNode *f_glCopyTexImage1D(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint level = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLint border = (GLint)(p ? p->getAsInt() : 0);
   glCopyTexImage1D(target, level, internalformat, x, y, width, border);
   return 0;
}

//void glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
static AbstractQoreNode *f_glCopyTexImage2D(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint level = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   GLint border = (GLint)(p ? p->getAsInt() : 0);

   glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
   return 0;
}

//void glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
static AbstractQoreNode *f_glCopyTexSubImage1D(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint level = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint xoffset = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   glCopyTexSubImage1D(target, level, xoffset, x, y, width);
   return 0;
}

//void glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
static AbstractQoreNode *f_glCopyTexSubImage2D(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint level = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint xoffset = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint yoffset = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
   return 0;
}

//void glCopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
static AbstractQoreNode *f_glCopyTexSubImage3D(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint level = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint xoffset = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint yoffset = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint zoffset = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 8);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);
   return 0;
}

//void glCullFace (GLenum mode);
static AbstractQoreNode *f_glCullFace(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   glCullFace(mode);
   return 0;
}

/*
//void glDeleteTextures (GLsizei n, const GLuint *textures);
static AbstractQoreNode *f_glDeleteTextures(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* textures = p;
   glDeleteTextures(n, textures);
   return 0;
}
*/

//void glDepthMask (GLboolean flag);
static AbstractQoreNode *f_glDepthMask(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLboolean flag = (GLboolean)(p ? p->getAsBool() : 0);
   glDepthMask(flag);
   return 0;
}

//void glDepthRange (GLclampd zNear, GLclampd zFar);
static AbstractQoreNode *f_glDepthRange(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLclampd zNear = (GLclampd)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLclampd zFar = (GLclampd)(p ? p->getAsInt() : 0);
   glDepthRange(zNear, zFar);
   return 0;
}

//void glDisableClientState (GLenum array);
static AbstractQoreNode *f_glDisableClientState(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum array = (GLenum)(p ? p->getAsInt() : 0);
   glDisableClientState(array);
   return 0;
}

//void glDrawArrays (GLenum mode, GLint first, GLsizei count);
static AbstractQoreNode *f_glDrawArrays(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint first = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   glDrawArrays(mode, first, count);
   return 0;
}

//void glDrawBuffer (GLenum mode);
static AbstractQoreNode *f_glDrawBuffer(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   glDrawBuffer(mode);
   return 0;
}

/*
//void glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
static AbstractQoreNode *f_glDrawElements(const QoreListNode *params, ExceptionSink *xsink)
{
   glDrawElements();
   return 0;
}
*/

/*
//void glDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
static AbstractQoreNode *f_glDrawPixels(const QoreListNode *params, ExceptionSink *xsink)
{
   glDrawPixels();
   return 0;
}
*/

/*
//void glDrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);
static AbstractQoreNode *f_glDrawRangeElements(const QoreListNode *params, ExceptionSink *xsink)
{
   glDrawRangeElements();
   return 0;
}
*/

//void glEdgeFlag (GLboolean flag);
static AbstractQoreNode *f_glEdgeFlag(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLboolean flag = (GLboolean)(p ? p->getAsBool() : 0);
   glEdgeFlag(flag);
   return 0;
}

/*
//void glEdgeFlagPointer (GLsizei stride, const GLvoid *pointer);
static AbstractQoreNode *f_glEdgeFlagPointer(const QoreListNode *params, ExceptionSink *xsink)
{
   glEdgeFlagPointer();
   return 0;
}
*/

/*
//void glEdgeFlagv (const GLboolean *flag);
static AbstractQoreNode *f_glEdgeFlagv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLboolean* flag = p;
   glEdgeFlagv(flag);
   return 0;
}
*/

//void glEnableClientState (GLenum array);
static AbstractQoreNode *f_glEnableClientState(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum array = (GLenum)(p ? p->getAsInt() : 0);
   glEnableClientState(array);
   return 0;
}

//void glEvalCoord1d (GLdouble u);
static AbstractQoreNode *f_glEvalCoord1d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble u = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glEvalCoord1d(u);
   return 0;
}

/*
//void glEvalCoord1dv (const GLdouble *u);
static AbstractQoreNode *f_glEvalCoord1dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* u = p;
   glEvalCoord1dv(u);
   return 0;
}
*/

//void glEvalCoord1f (GLfloat u);
static AbstractQoreNode *f_glEvalCoord1f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat u = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glEvalCoord1f(u);
   return 0;
}

/*
//void glEvalCoord1fv (const GLfloat *u);
static AbstractQoreNode *f_glEvalCoord1fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* u = p;
   glEvalCoord1fv(u);
   return 0;
}
*/

//void glEvalCoord2d (GLdouble u, GLdouble v);
static AbstractQoreNode *f_glEvalCoord2d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble u = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble v = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glEvalCoord2d(u, v);
   return 0;
}

/*
//void glEvalCoord2dv (const GLdouble *u);
static AbstractQoreNode *f_glEvalCoord2dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* u = p;
   glEvalCoord2dv(u);
   return 0;
}
*/

//void glEvalCoord2f (GLfloat u, GLfloat v);
static AbstractQoreNode *f_glEvalCoord2f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat u = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat v = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glEvalCoord2f(u, v);
   return 0;
}

/*
//void glEvalCoord2fv (const GLfloat *u);
static AbstractQoreNode *f_glEvalCoord2fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* u = p;
   glEvalCoord2fv(u);
   return 0;
}
*/

//void glEvalMesh1 (GLenum mode, GLint i1, GLint i2);
static AbstractQoreNode *f_glEvalMesh1(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint i1 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint i2 = (GLint)(p ? p->getAsInt() : 0);
   glEvalMesh1(mode, i1, i2);
   return 0;
}

//void glEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
static AbstractQoreNode *f_glEvalMesh2(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint i1 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint i2 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint j1 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint j2 = (GLint)(p ? p->getAsInt() : 0);
   glEvalMesh2(mode, i1, i2, j1, j2);
   return 0;
}

//void glEvalPoint1 (GLint i);
static AbstractQoreNode *f_glEvalPoint1(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint i = (GLint)(p ? p->getAsInt() : 0);
   glEvalPoint1(i);
   return 0;
}

//void glEvalPoint2 (GLint i, GLint j);
static AbstractQoreNode *f_glEvalPoint2(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint i = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint j = (GLint)(p ? p->getAsInt() : 0);
   glEvalPoint2(i, j);
   return 0;
}

/*
//void glFeedbackBuffer (GLsizei size, GLenum type, GLfloat *buffer);
static AbstractQoreNode *f_glFeedbackBuffer(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei size = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* buffer = p;
   glFeedbackBuffer(size, type, buffer);
   return 0;
}
*/

//void glFinish (void);
static AbstractQoreNode *f_glFinish(const QoreListNode *params, ExceptionSink *xsink)
{
   glFinish();
   return 0;
}

//void glFogf (GLenum pname, GLfloat param);
static AbstractQoreNode *f_glFogf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glFogf(pname, param);
   return 0;
}

static int get_num_fog_args(GLenum pname)
{
   switch (pname) {
      case GL_FOG_COLOR:
	 return 4;

      case GL_FOG_INDEX:
      case GL_FOG_END:
      case GL_FOG_START:
      case GL_FOG_DENSITY:
      case GL_FOG_MODE:
	 return 1;
   }
   return -1;
}

//void glFogfv (GLenum pname, const GLfloat *params);
static AbstractQoreNode *f_glFogfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   const QoreListNode *l = test_list_param(params, 1);
   if (!l) {
      xsink->raiseException("GLFOGFV-ERROR", "expecing a list as second argument");
      return 0;
   }

   int num = get_num_fog_args(pname);
   if (num == -1) {
      xsink->raiseException("GLFOGFV-ERROR", "unrecognized parameter code %d", (int)pname);
      return 0;      
   }

   GLfloat parms[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *p = l->retrieve_entry(i);
      parms[i] = p ? p->getAsFloat() : 0.0;
   }

   glFogfv(pname, parms);
   return 0;
}

//void glFogi (GLenum pname, GLint param);
static AbstractQoreNode *f_glFogi(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint param = (GLint)(p ? p->getAsInt() : 0);
   glFogi(pname, param);
   return 0;
}

//void glFogiv (GLenum pname, const GLint *params);
static AbstractQoreNode *f_glFogiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   const QoreListNode *l = test_list_param(params, 1);
   if (!l) {
      xsink->raiseException("GLFOGIV-ERROR", "expecing a list as second argument");
      return 0;
   }

   int num = get_num_fog_args(pname);
   if (num == -1) {
      xsink->raiseException("GLFOGIV-ERROR", "unrecognized parameter code %d", (int)pname);
      return 0;      
   }

   GLint parms[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *p = l->retrieve_entry(i);
      parms[i] = p ? p->getAsInt() : 0.0;
   }

   glFogiv(pname, parms);
   return 0;
}

//void glFrontFace (GLenum mode);
static AbstractQoreNode *f_glFrontFace(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   glFrontFace(mode);
   return 0;
}

//void glGenTextures (GLsizei n, GLuint *textures);
static AbstractQoreNode *f_glGenTextures(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);

   GLuint textures[n];
   glGenTextures(n, textures);

   if (n == 1)
      return new QoreBigIntNode(textures[0]);

   QoreListNode *l = new QoreListNode();

   for (int i = 0; i < n; ++i)
      l->push(new QoreBigIntNode(textures[i]));

   return 0;
}

/*
//void glGetClipPlane (GLenum plane, GLdouble *equation);
static AbstractQoreNode *f_glGetClipPlane(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum plane = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLdouble* equation = p;
   glGetClipPlane(plane, equation);
   return 0;
}
*/

/*
//void glGetColorTable (GLenum target, GLenum format, GLenum type, GLvoid *table);
static AbstractQoreNode *f_glGetColorTable(const QoreListNode *params, ExceptionSink *xsink)
{
   glGetColorTable();
   return 0;
}
*/

/*
//void glGetColorTableParameterfv (GLenum target, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetColorTableParameterfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetColorTableParameterfv(target, pname, params);
   return 0;
}
*/

/*
//void glGetColorTableParameteriv (GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetColorTableParameteriv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetColorTableParameteriv(target, pname, params);
   return 0;
}
*/

/*
//void glGetConvolutionFilter (GLenum target, GLenum format, GLenum type, GLvoid *image);
static AbstractQoreNode *f_glGetConvolutionFilter(const QoreListNode *params, ExceptionSink *xsink)
{
   glGetConvolutionFilter();
   return 0;
}
*/

/*
//void glGetConvolutionParameterfv (GLenum target, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetConvolutionParameterfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetConvolutionParameterfv(target, pname, params);
   return 0;
}
*/

/*
//void glGetConvolutionParameteriv (GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetConvolutionParameteriv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetConvolutionParameteriv(target, pname, params);
   return 0;
}
*/

/*
//void glGetHistogram (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
static AbstractQoreNode *f_glGetHistogram(const QoreListNode *params, ExceptionSink *xsink)
{
   glGetHistogram();
   return 0;
}
*/

/*
//void glGetHistogramParameterfv (GLenum target, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetHistogramParameterfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetHistogramParameterfv(target, pname, params);
   return 0;
}
*/

/*
//void glGetHistogramParameteriv (GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetHistogramParameteriv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetHistogramParameteriv(target, pname, params);
   return 0;
}
*/

/*
//void glGetLightfv (GLenum light, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetLightfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum light = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetLightfv(light, pname, params);
   return 0;
}
*/

/*
//void glGetLightiv (GLenum light, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetLightiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum light = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetLightiv(light, pname, params);
   return 0;
}
*/

/*
//void glGetMapdv (GLenum target, GLenum query, GLdouble *v);
static AbstractQoreNode *f_glGetMapdv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum query = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLdouble* v = p;
   glGetMapdv(target, query, v);
   return 0;
}
*/

/*
//void glGetMapfv (GLenum target, GLenum query, GLfloat *v);
static AbstractQoreNode *f_glGetMapfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum query = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* v = p;
   glGetMapfv(target, query, v);
   return 0;
}
*/

/*
//void glGetMapiv (GLenum target, GLenum query, GLint *v);
static AbstractQoreNode *f_glGetMapiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum query = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* v = p;
   glGetMapiv(target, query, v);
   return 0;
}
*/

/*
//void glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetMaterialfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum face = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetMaterialfv(face, pname, params);
   return 0;
}
*/

/*
//void glGetMaterialiv (GLenum face, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetMaterialiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum face = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetMaterialiv(face, pname, params);
   return 0;
}
*/

/*
//void glGetMinmax (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values);
static AbstractQoreNode *f_glGetMinmax(const QoreListNode *params, ExceptionSink *xsink)
{
   glGetMinmax();
   return 0;
}
*/

/*
//void glGetMinmaxParameterfv (GLenum target, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetMinmaxParameterfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetMinmaxParameterfv(target, pname, params);
   return 0;
}
*/

/*
//void glGetMinmaxParameteriv (GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetMinmaxParameteriv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetMinmaxParameteriv(target, pname, params);
   return 0;
}
*/

/*
//void glGetPixelMapfv (GLenum map, GLfloat *values);
static AbstractQoreNode *f_glGetPixelMapfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum map = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLfloat* values = p;
   glGetPixelMapfv(map, values);
   return 0;
}
*/

/*
//void glGetPixelMapuiv (GLenum map, GLuint *values);
static AbstractQoreNode *f_glGetPixelMapuiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum map = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* values = p;
   glGetPixelMapuiv(map, values);
   return 0;
}
*/

/*
//void glGetPixelMapusv (GLenum map, GLushort *values);
static AbstractQoreNode *f_glGetPixelMapusv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum map = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLushort* values = p;
   glGetPixelMapusv(map, values);
   return 0;
}
*/

/*
//void glGetPointerv (GLenum pname, GLvoid* *params);
static AbstractQoreNode *f_glGetPointerv(const QoreListNode *params, ExceptionSink *xsink)
{
   glGetPointerv();
   return 0;
}
*/

/*
//void glGetPolygonStipple (GLubyte *mask);
static AbstractQoreNode *f_glGetPolygonStipple(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLubyte* mask = p;
   glGetPolygonStipple(mask);
   return 0;
}
*/

/*
//void glGetSeparableFilter (GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span);
static AbstractQoreNode *f_glGetSeparableFilter(const QoreListNode *params, ExceptionSink *xsink)
{
   glGetSeparableFilter();
   return 0;
}
*/

//const GLubyte * glGetString (GLenum name);
static AbstractQoreNode *f_glGetString(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum name = (GLenum)(p ? p->getAsInt() : 0);
   // the man page does not say what encoding the string is - but another man page says that
   // such strings are in latin1 encoding, so we'll assume that here too
   return new QoreStringNode((const char *)glGetString(name), QCS_ISO_8859_1);
}

/*
//void glGetTexEnvfv (GLenum target, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetTexEnvfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetTexEnvfv(target, pname, params);
   return 0;
}
*/

/*
//void glGetTexEnviv (GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetTexEnviv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetTexEnviv(target, pname, params);
   return 0;
}
*/

/*
//void glGetTexGendv (GLenum coord, GLenum pname, GLdouble *params);
static AbstractQoreNode *f_glGetTexGendv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum coord = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLdouble* params = p;
   glGetTexGendv(coord, pname, params);
   return 0;
}
*/

/*
//void glGetTexGenfv (GLenum coord, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetTexGenfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum coord = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetTexGenfv(coord, pname, params);
   return 0;
}
*/

/*
//void glGetTexGeniv (GLenum coord, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetTexGeniv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum coord = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetTexGeniv(coord, pname, params);
   return 0;
}
*/

/*
//void glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
static AbstractQoreNode *f_glGetTexImage(const QoreListNode *params, ExceptionSink *xsink)
{
   glGetTexImage();
   return 0;
}
*/

static int get_texlevel_num(GLenum pname)
{
   switch (pname) {
      case GL_TEXTURE_WIDTH:
      case GL_TEXTURE_HEIGHT:
      case GL_TEXTURE_DEPTH:
      case GL_TEXTURE_INTERNAL_FORMAT:
      case GL_TEXTURE_BORDER:
	 return 1;

      case GL_TEXTURE_RED_SIZE:
      case GL_TEXTURE_GREEN_SIZE:
      case GL_TEXTURE_BLUE_SIZE:
      case GL_TEXTURE_ALPHA_SIZE:
      case GL_TEXTURE_LUMINANCE_SIZE:
      case GL_TEXTURE_INTENSITY_SIZE:
	 return -1; // FIXME: don't know how to figure out the size yet
   }

   return -1;
}

//void glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetTexLevelParameterfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint level = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   int num = get_texlevel_num(pname);
   if (num == -1) {
      xsink->raiseException("GLGETTEXLEVELPARAMETERFV", "cannot determine result value size for parameter code %d", (int)pname);
      return 0;
   }

   GLfloat parms[num];
   glGetTexLevelParameterfv(target, level, pname, parms);

   if (num == 1)
      return new QoreFloatNode(parms[0]);

   QoreListNode *l = new QoreListNode();

   for (int i = 0; i < num; ++i)
      l->push(new QoreFloatNode(parms[i]));

   return l;
}

//void glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetTexLevelParameteriv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint level = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   int num = get_texlevel_num(pname);
   if (num == -1) {
      xsink->raiseException("GLGETTEXLEVELPARAMETERIV", "cannot determine result value size for parameter code %d", (int)pname);
      return 0;
   }

   GLint parms[num];
   glGetTexLevelParameteriv(target, level, pname, parms);

   if (num == 1)
      return new QoreBigIntNode(parms[0]);

   QoreListNode *l = new QoreListNode();

   for (int i = 0; i < num; ++i)
      l->push(new QoreBigIntNode(parms[i]));

   return l;
}

/*
//void glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetTexParameterfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetTexParameterfv(target, pname, params);
   return 0;
}
*/

/*
//void glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetTexParameteriv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetTexParameteriv(target, pname, params);
   return 0;
}
*/

//void glHistogram (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink);
static AbstractQoreNode *f_glHistogram(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLboolean sink = (GLboolean)(p ? p->getAsBool() : 0);
   glHistogram(target, width, internalformat, sink);
   return 0;
}

//void glIndexMask (GLuint mask);
static AbstractQoreNode *f_glIndexMask(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned mask = p ? p->getAsBigInt() : 0;
   glIndexMask(mask);
   return 0;
}

/*
//void glIndexPointer (GLenum type, GLsizei stride, const GLvoid *pointer);
static AbstractQoreNode *f_glIndexPointer(const QoreListNode *params, ExceptionSink *xsink)
{
   glIndexPointer();
   return 0;
}
*/

//void glIndexd (GLdouble c);
static AbstractQoreNode *f_glIndexd(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble c = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glIndexd(c);
   return 0;
}

/*
//void glIndexdv (const GLdouble *c);
static AbstractQoreNode *f_glIndexdv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* c = p;
   glIndexdv(c);
   return 0;
}
*/

//void glIndexf (GLfloat c);
static AbstractQoreNode *f_glIndexf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat c = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glIndexf(c);
   return 0;
}

/*
//void glIndexfv (const GLfloat *c);
static AbstractQoreNode *f_glIndexfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* c = p;
   glIndexfv(c);
   return 0;
}
*/

//void glIndexi (GLint c);
static AbstractQoreNode *f_glIndexi(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint c = (GLint)(p ? p->getAsInt() : 0);
   glIndexi(c);
   return 0;
}

/*
//void glIndexiv (const GLint *c);
static AbstractQoreNode *f_glIndexiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* c = p;
   glIndexiv(c);
   return 0;
}
*/

//void glIndexs (GLshort c);
static AbstractQoreNode *f_glIndexs(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort c = (GLshort)(p ? p->getAsInt() : 0);
   glIndexs(c);
   return 0;
}

/*
//void glIndexsv (const GLshort *c);
static AbstractQoreNode *f_glIndexsv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* c = p;
   glIndexsv(c);
   return 0;
}
*/

//void glIndexub (GLubyte c);
static AbstractQoreNode *f_glIndexub(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLubyte c = (GLubyte)(p ? p->getAsInt() : 0);
   glIndexub(c);
   return 0;
}

/*
//void glIndexubv (const GLubyte *c);
static AbstractQoreNode *f_glIndexubv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLubyte* c = p;
   glIndexubv(c);
   return 0;
}
*/

//void glInitNames (void);
static AbstractQoreNode *f_glInitNames(const QoreListNode *params, ExceptionSink *xsink)
{
   glInitNames();
   return 0;
}

/*
//void glInterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer);
static AbstractQoreNode *f_glInterleavedArrays(const QoreListNode *params, ExceptionSink *xsink)
{
   glInterleavedArrays();
   return 0;
}
*/

//GLboolean glIsEnabled (GLenum cap);
static AbstractQoreNode *f_glIsEnabled(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum cap = (GLenum)(p ? p->getAsInt() : 0);
   return get_bool_node(glIsEnabled(cap));
}

//GLboolean glIsList (GLuint list);
static AbstractQoreNode *f_glIsList(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned list = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsList(list));
}

//GLboolean glIsTexture (GLuint texture);
static AbstractQoreNode *f_glIsTexture(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned texture = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsTexture(texture));
}

//void glLineStipple (GLint factor, GLushort pattern);
static AbstractQoreNode *f_glLineStipple(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint factor = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLushort pattern = (GLushort)(p ? p->getAsInt() : 0);
   glLineStipple(factor, pattern);
   return 0;
}

//void glLineWidth (GLfloat width);
static AbstractQoreNode *f_glLineWidth(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat width = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glLineWidth(width);
   return 0;
}

//void glListBase (GLuint base);
static AbstractQoreNode *f_glListBase(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned base = p ? p->getAsBigInt() : 0;
   glListBase(base);
   return 0;
}

//void glLoadMatrixd (const GLdouble *m);
static AbstractQoreNode *f_glLoadMatrixd(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLLOADMATRIXD-ERROR", "expecting a list as the sole argument to glLoadMatrixd()");
      return 0;
   }
   
   GLdouble m[16];
   for (int i = 0; i < 16; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      m[i] = n ? n->getAsFloat() : 0;
   }

   glLoadMatrixd(m);
   return 0;
}
//void glLoadMatrixf (const GLfloat *m);
static AbstractQoreNode *f_glLoadMatrixf(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLLOADMATRIXF-ERROR", "expecting a list as the sole argument to glLoadMatrixf()");
      return 0;
   }
   
   GLfloat m[16];
   for (int i = 0; i < 16; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      m[i] = n ? n->getAsFloat() : 0;
   }

   glLoadMatrixf(m);
   return 0;
}

//void glLoadName (GLuint name);
static AbstractQoreNode *f_glLoadName(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned name = p ? p->getAsBigInt() : 0;
   glLoadName(name);
   return 0;
}

//void glLogicOp (GLenum opcode);
static AbstractQoreNode *f_glLogicOp(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum opcode = (GLenum)(p ? p->getAsInt() : 0);
   glLogicOp(opcode);
   return 0;
}

/*
//void glMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
static AbstractQoreNode *f_glMap1d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLdouble u1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble u2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLint stride = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint order = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   ??? GLdouble* points = p;
   glMap1d(target, u1, u2, stride, order, points);
   return 0;
}
*/

/*
//void glMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
static AbstractQoreNode *f_glMap1f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat u1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat u2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLint stride = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint order = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   ??? GLfloat* points = p;
   glMap1f(target, u1, u2, stride, order, points);
   return 0;
}
*/

/*
//void glMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
static AbstractQoreNode *f_glMap2d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLdouble u1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble u2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLint ustride = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint uorder = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLdouble v1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 6);
   GLdouble v2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 7);
   GLint vstride = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 8);
   GLint vorder = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 9);
   ??? GLdouble* points = p;
   glMap2d(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
   return 0;
}
*/

/*
//void glMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
static AbstractQoreNode *f_glMap2f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat u1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat u2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLint ustride = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint uorder = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLfloat v1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 6);
   GLfloat v2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 7);
   GLint vstride = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 8);
   GLint vorder = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 9);
   ??? GLfloat* points = p;
   glMap2f(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
   return 0;
}
*/

//void glMapGrid1d (GLint un, GLdouble u1, GLdouble u2);
static AbstractQoreNode *f_glMapGrid1d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint un = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLdouble u1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble u2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glMapGrid1d(un, u1, u2);
   return 0;
}

//void glMapGrid1f (GLint un, GLfloat u1, GLfloat u2);
static AbstractQoreNode *f_glMapGrid1f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint un = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat u1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat u2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glMapGrid1f(un, u1, u2);
   return 0;
}

//void glMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
static AbstractQoreNode *f_glMapGrid2d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint un = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLdouble u1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble u2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLint vn = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLdouble v1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 5);
   GLdouble v2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glMapGrid2d(un, u1, u2, vn, v1, v2);
   return 0;
}

//void glMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
static AbstractQoreNode *f_glMapGrid2f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint un = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat u1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat u2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLint vn = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLfloat v1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 5);
   GLfloat v2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glMapGrid2f(un, u1, u2, vn, v1, v2);
   return 0;
}

//void glMinmax (GLenum target, GLenum internalformat, GLboolean sink);
static AbstractQoreNode *f_glMinmax(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean sink = (GLboolean)(p ? p->getAsBool() : 0);
   glMinmax(target, internalformat, sink);
   return 0;
}

//void glMultMatrixd (const GLdouble *m);
static AbstractQoreNode *f_glMultMatrixd(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLMULTMATRIXD-ERROR", "expecting a list as the sole argument to glMultMatrixd()");
      return 0;
   }
   
   GLdouble m[16];
   for (int i = 0; i < 16; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      m[i] = n ? n->getAsFloat() : 0;
   }

   glMultMatrixd(m);
   return 0;
}

//void glMultMatrixf (const GLfloat *m);
static AbstractQoreNode *f_glMultMatrixf(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLMULTMATRIXF-ERROR", "expecting a list as the sole argument to glMultMatrixf()");
      return 0;
   }
   
   GLfloat m[16];
   for (int i = 0; i < 16; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      m[i] = n ? n->getAsFloat() : 0;
   }

   glMultMatrixf(m);
   return 0;
}

//void glNormal3bv (const GLbyte *v);
static AbstractQoreNode *f_glNormal3bv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLNORMAL3BV-ERROR", "expecting a list as the sole argument to glNormal3bv()");
      return 0;
   }

   GLbyte v[3];

   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glNormal3bv(v);
   return 0;
}

//void glNormal3dv (const GLdouble *v);
static AbstractQoreNode *f_glNormal3dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLNORMAL3DV-ERROR", "expecting a list as the sole argument to glNormal3dv()");
      return 0;
   }

   GLdouble v[3];

   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsFloat() : 0;
   }

   glNormal3dv(v);
   return 0;
}

//void glNormal3fv (const GLfloat *v);
static AbstractQoreNode *f_glNormal3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLNORMAL3FV-ERROR", "expecting a list as the sole argument to glNormal3fv()");
      return 0;
   }

   GLfloat v[3];

   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsFloat() : 0;
   }

   glNormal3fv(v);
   return 0;
}

//void glNormal3iv (const GLint *v);
static AbstractQoreNode *f_glNormal3iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLNORMAL3IV-ERROR", "expecting a list as the sole argument to glNormal3iv()");
      return 0;
   }

   GLint v[3];

   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glNormal3iv(v);
   return 0;
}

//void glNormal3sv (const GLshort *v);
static AbstractQoreNode *f_glNormal3sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLNORMAL3SV-ERROR", "expecting a list as the sole argument to glNormal3sv()");
      return 0;
   }

   GLshort v[3];

   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glNormal3sv(v);
   return 0;
}

/*
//void glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer);
static AbstractQoreNode *f_glNormalPointer(const QoreListNode *params, ExceptionSink *xsink)
{
   glNormalPointer();
   return 0;
}
*/

//void glPassThrough (GLfloat token);
static AbstractQoreNode *f_glPassThrough(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat token = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glPassThrough(token);
   return 0;
}

/*
//void glPixelMapfv (GLenum map, GLint mapsize, const GLfloat *values);
static AbstractQoreNode *f_glPixelMapfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum map = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint mapsize = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* values = p;
   glPixelMapfv(map, mapsize, values);
   return 0;
}
*/

/*
//void glPixelMapuiv (GLenum map, GLint mapsize, const GLuint *values);
static AbstractQoreNode *f_glPixelMapuiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum map = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint mapsize = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLuint* values = p;
   glPixelMapuiv(map, mapsize, values);
   return 0;
}
*/

/*
//void glPixelMapusv (GLenum map, GLint mapsize, const GLushort *values);
static AbstractQoreNode *f_glPixelMapusv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum map = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint mapsize = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLushort* values = p;
   glPixelMapusv(map, mapsize, values);
   return 0;
}
*/

//void glPixelStoref (GLenum pname, GLfloat param);
static AbstractQoreNode *f_glPixelStoref(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glPixelStoref(pname, param);
   return 0;
}

//void glPixelStorei (GLenum pname, GLint param);
static AbstractQoreNode *f_glPixelStorei(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint param = (GLint)(p ? p->getAsInt() : 0);
   glPixelStorei(pname, param);
   return 0;
}

//void glPixelTransferf (GLenum pname, GLfloat param);
static AbstractQoreNode *f_glPixelTransferf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glPixelTransferf(pname, param);
   return 0;
}

//void glPixelTransferi (GLenum pname, GLint param);
static AbstractQoreNode *f_glPixelTransferi(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint param = (GLint)(p ? p->getAsInt() : 0);
   glPixelTransferi(pname, param);
   return 0;
}

//void glPixelZoom (GLfloat xfactor, GLfloat yfactor);
static AbstractQoreNode *f_glPixelZoom(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat xfactor = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat yfactor = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glPixelZoom(xfactor, yfactor);
   return 0;
}

//void glPointSize (GLfloat size);
static AbstractQoreNode *f_glPointSize(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat size = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glPointSize(size);
   return 0;
}

//void glPolygonMode (GLenum face, GLenum mode);
static AbstractQoreNode *f_glPolygonMode(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum face = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   glPolygonMode(face, mode);
   return 0;
}

//void glPolygonOffset (GLfloat factor, GLfloat units);
static AbstractQoreNode *f_glPolygonOffset(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat factor = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat units = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glPolygonOffset(factor, units);
   return 0;
}

/*
//void glPolygonStipple (const GLubyte *mask);
static AbstractQoreNode *f_glPolygonStipple(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLubyte* mask = p;
   glPolygonStipple(mask);
   return 0;
}
*/

//void glPopClientAttrib (void);
static AbstractQoreNode *f_glPopClientAttrib(const QoreListNode *params, ExceptionSink *xsink)
{
   glPopClientAttrib();
   return 0;
}

//void glPopName (void);
static AbstractQoreNode *f_glPopName(const QoreListNode *params, ExceptionSink *xsink)
{
   glPopName();
   return 0;
}

/*
//void glPrioritizeTextures (GLsizei n, const GLuint *textures, const GLclampf *priorities);
static AbstractQoreNode *f_glPrioritizeTextures(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* textures = p;
   p = get_param(params, 2);
   ??? GLclampf* priorities = p;
   glPrioritizeTextures(n, textures, priorities);
   return 0;
}
*/

//void glPushClientAttrib (GLbitfield mask);
static AbstractQoreNode *f_glPushClientAttrib(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLbitfield mask = (GLbitfield)(p ? p->getAsInt() : 0);
   glPushClientAttrib(mask);
   return 0;
}

//void glPushName (GLuint name);
static AbstractQoreNode *f_glPushName(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned name = p ? p->getAsBigInt() : 0;
   glPushName(name);
   return 0;
}

//void glRasterPos2d (GLdouble x, GLdouble y);
static AbstractQoreNode *f_glRasterPos2d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glRasterPos2d(x, y);
   return 0;
}

/*
//void glRasterPos2dv (const GLdouble *v);
static AbstractQoreNode *f_glRasterPos2dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* v = p;
   glRasterPos2dv(v);
   return 0;
}
*/

//void glRasterPos2f (GLfloat x, GLfloat y);
static AbstractQoreNode *f_glRasterPos2f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glRasterPos2f(x, y);
   return 0;
}

/*
//void glRasterPos2fv (const GLfloat *v);
static AbstractQoreNode *f_glRasterPos2fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* v = p;
   glRasterPos2fv(v);
   return 0;
}
*/

//void glRasterPos2i (GLint x, GLint y);
static AbstractQoreNode *f_glRasterPos2i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   glRasterPos2i(x, y);
   return 0;
}

/*
//void glRasterPos2iv (const GLint *v);
static AbstractQoreNode *f_glRasterPos2iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* v = p;
   glRasterPos2iv(v);
   return 0;
}
*/

//void glRasterPos2s (GLshort x, GLshort y);
static AbstractQoreNode *f_glRasterPos2s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   glRasterPos2s(x, y);
   return 0;
}

/*
//void glRasterPos2sv (const GLshort *v);
static AbstractQoreNode *f_glRasterPos2sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* v = p;
   glRasterPos2sv(v);
   return 0;
}
*/

//void glRasterPos3d (GLdouble x, GLdouble y, GLdouble z);
static AbstractQoreNode *f_glRasterPos3d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glRasterPos3d(x, y, z);
   return 0;
}

/*
//void glRasterPos3dv (const GLdouble *v);
static AbstractQoreNode *f_glRasterPos3dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* v = p;
   glRasterPos3dv(v);
   return 0;
}
*/

//void glRasterPos3f (GLfloat x, GLfloat y, GLfloat z);
static AbstractQoreNode *f_glRasterPos3f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glRasterPos3f(x, y, z);
   return 0;
}

/*
//void glRasterPos3fv (const GLfloat *v);
static AbstractQoreNode *f_glRasterPos3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* v = p;
   glRasterPos3fv(v);
   return 0;
}
*/

//void glRasterPos3i (GLint x, GLint y, GLint z);
static AbstractQoreNode *f_glRasterPos3i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint z = (GLint)(p ? p->getAsInt() : 0);
   glRasterPos3i(x, y, z);
   return 0;
}

/*
//void glRasterPos3iv (const GLint *v);
static AbstractQoreNode *f_glRasterPos3iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* v = p;
   glRasterPos3iv(v);
   return 0;
}
*/

//void glRasterPos3s (GLshort x, GLshort y, GLshort z);
static AbstractQoreNode *f_glRasterPos3s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort z = (GLshort)(p ? p->getAsInt() : 0);
   glRasterPos3s(x, y, z);
   return 0;
}

/*
//void glRasterPos3sv (const GLshort *v);
static AbstractQoreNode *f_glRasterPos3sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* v = p;
   glRasterPos3sv(v);
   return 0;
}
*/

//void glRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
static AbstractQoreNode *f_glRasterPos4d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble w = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glRasterPos4d(x, y, z, w);
   return 0;
}

/*
//void glRasterPos4dv (const GLdouble *v);
static AbstractQoreNode *f_glRasterPos4dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* v = p;
   glRasterPos4dv(v);
   return 0;
}
*/

//void glRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
static AbstractQoreNode *f_glRasterPos4f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat w = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glRasterPos4f(x, y, z, w);
   return 0;
}

/*
//void glRasterPos4fv (const GLfloat *v);
static AbstractQoreNode *f_glRasterPos4fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* v = p;
   glRasterPos4fv(v);
   return 0;
}
*/

//void glRasterPos4i (GLint x, GLint y, GLint z, GLint w);
static AbstractQoreNode *f_glRasterPos4i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint z = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint w = (GLint)(p ? p->getAsInt() : 0);
   glRasterPos4i(x, y, z, w);
   return 0;
}

/*
//void glRasterPos4iv (const GLint *v);
static AbstractQoreNode *f_glRasterPos4iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* v = p;
   glRasterPos4iv(v);
   return 0;
}
*/

//void glRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w);
static AbstractQoreNode *f_glRasterPos4s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort z = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLshort w = (GLshort)(p ? p->getAsInt() : 0);
   glRasterPos4s(x, y, z, w);
   return 0;
}

/*
//void glRasterPos4sv (const GLshort *v);
static AbstractQoreNode *f_glRasterPos4sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* v = p;
   glRasterPos4sv(v);
   return 0;
}
*/

//void glReadBuffer (GLenum mode);
static AbstractQoreNode *f_glReadBuffer(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   glReadBuffer(mode);
   return 0;
}

static int get_bits_per_pixel(GLenum type)
{
   switch (type) {
      case GL_FLOAT:
	 return sizeof(float) * 8;

      case GL_UNSIGNED_INT:
      case GL_INT:
      case GL_UNSIGNED_INT_8_8_8_8:
      case GL_UNSIGNED_INT_8_8_8_8_REV:
      case GL_UNSIGNED_INT_10_10_10_2:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
      case GL_UNSIGNED_INT_24_8_EXT:
      case GL_UNSIGNED_INT_S8_S8_8_8_NV:
      case GL_UNSIGNED_INT_8_8_S8_S8_REV_NV:
	 return 32;

      case GL_UNSIGNED_SHORT:
      case GL_SHORT:
      case GL_UNSIGNED_SHORT_5_6_5:
      case GL_UNSIGNED_SHORT_5_6_5_REV:
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
      case GL_UNSIGNED_SHORT_5_5_5_1:
      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
	 return 16;

      case GL_UNSIGNED_BYTE_2_3_3_REV:
      case GL_UNSIGNED_BYTE_3_3_2:
      case GL_UNSIGNED_BYTE:
      case GL_BYTE:
	 return 8;

      case GL_BITMAP:
	 return 1;
   }
   return -1;
}

//void glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
static AbstractQoreNode *f_glReadPixels(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLenum format = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);

   // get bits per pixel
   int bpp = get_bits_per_pixel(type);
   if (bpp == -1) {
      xsink->raiseException("GLREADPIXELS-ERROR", "cannot determine buffer size for unknown type code %d", (int)type);
      return 0;
   }

   size_t pixel_bits = x * y * bpp;
   size_t size = pixel_bits / 8;
   if (size * 8 != pixel_bits)
      ++size;
   void *pixels = malloc(size);
   if (!pixels) {
      xsink->outOfMemory();
      return 0;
   }

   printd(0, "glReadPixels() x=%d, y=%d, bpp=%d size=%d\n", x, y, bpp, size);

   glReadPixels(x, y, width, height, format, type, pixels);
   return new BinaryNode(pixels, size);
}

//void glRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
static AbstractQoreNode *f_glRectd(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble x1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble y1 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble x2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble y2 = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glRectd(x1, y1, x2, y2);
   return 0;
}

/*
//void glRectdv (const GLdouble *v1, const GLdouble *v2);
static AbstractQoreNode *f_glRectdv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* v1 = p;
   p = get_param(params, 1);
   ??? GLdouble* v2 = p;
   glRectdv(v1, v2);
   return 0;
}
*/

//void glRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
static AbstractQoreNode *f_glRectf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat x1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat y1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat x2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat y2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glRectf(x1, y1, x2, y2);
   return 0;
}

/*
//void glRectfv (const GLfloat *v1, const GLfloat *v2);
static AbstractQoreNode *f_glRectfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* v1 = p;
   p = get_param(params, 1);
   ??? GLfloat* v2 = p;
   glRectfv(v1, v2);
   return 0;
}
*/

//void glRecti (GLint x1, GLint y1, GLint x2, GLint y2);
static AbstractQoreNode *f_glRecti(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x1 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y1 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint x2 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint y2 = (GLint)(p ? p->getAsInt() : 0);
   glRecti(x1, y1, x2, y2);
   return 0;
}

/*
//void glRectiv (const GLint *v1, const GLint *v2);
static AbstractQoreNode *f_glRectiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* v1 = p;
   p = get_param(params, 1);
   ??? GLint* v2 = p;
   glRectiv(v1, v2);
   return 0;
}
*/

//void glRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2);
static AbstractQoreNode *f_glRects(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort x1 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort y1 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort x2 = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLshort y2 = (GLshort)(p ? p->getAsInt() : 0);
   glRects(x1, y1, x2, y2);
   return 0;
}

/*
//void glRectsv (const GLshort *v1, const GLshort *v2);
static AbstractQoreNode *f_glRectsv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* v1 = p;
   p = get_param(params, 1);
   ??? GLshort* v2 = p;
   glRectsv(v1, v2);
   return 0;
}
*/

/*
//GLint glRenderMode (GLenum mode);
static AbstractQoreNode *f_glRenderMode(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   ??? return new QoreBigIntNode(glRenderMode(mode));
}
*/

//void glResetHistogram (GLenum target);
static AbstractQoreNode *f_glResetHistogram(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   glResetHistogram(target);
   return 0;
}

//void glResetMinmax (GLenum target);
static AbstractQoreNode *f_glResetMinmax(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   glResetMinmax(target);
   return 0;
}

//void glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
static AbstractQoreNode *f_glScissor(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   glScissor(x, y, width, height);
   return 0;
}

/*
//void glSelectBuffer (GLsizei size, GLuint *buffer);
static AbstractQoreNode *f_glSelectBuffer(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei size = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* buffer = p;
   glSelectBuffer(size, buffer);
   return 0;
}
*/

/*
//void glSeparableFilter2D (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column);
static AbstractQoreNode *f_glSeparableFilter2D(const QoreListNode *params, ExceptionSink *xsink)
{
   glSeparableFilter2D();
   return 0;
}
*/

//void glStencilFunc (GLenum func, GLint ref, GLuint mask);
static AbstractQoreNode *f_glStencilFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum func = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint ref = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   unsigned mask = p ? p->getAsBigInt() : 0;
   glStencilFunc(func, ref, mask);
   return 0;
}

//void glStencilMask (GLuint mask);
static AbstractQoreNode *f_glStencilMask(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned mask = p ? p->getAsBigInt() : 0;
   glStencilMask(mask);
   return 0;
}

//void glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
static AbstractQoreNode *f_glStencilOp(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum fail = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum zfail = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum zpass = (GLenum)(p ? p->getAsInt() : 0);
   glStencilOp(fail, zfail, zpass);
   return 0;
}

/*
//void glTexCoord1dv (const GLdouble *v);
static AbstractQoreNode *f_glTexCoord1dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* v = p;
   glTexCoord1dv(v);
   return 0;
}
*/

/*
//void glTexCoord1fv (const GLfloat *v);
static AbstractQoreNode *f_glTexCoord1fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* v = p;
   glTexCoord1fv(v);
   return 0;
}
*/

/*
//void glTexCoord1iv (const GLint *v);
static AbstractQoreNode *f_glTexCoord1iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* v = p;
   glTexCoord1iv(v);
   return 0;
}
*/

/*
//void glTexCoord1sv (const GLshort *v);
static AbstractQoreNode *f_glTexCoord1sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* v = p;
   glTexCoord1sv(v);
   return 0;
}
*/

/*
//void glTexCoord2dv (const GLdouble *v);
static AbstractQoreNode *f_glTexCoord2dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* v = p;
   glTexCoord2dv(v);
   return 0;
}
*/

/*
//void glTexCoord2fv (const GLfloat *v);
static AbstractQoreNode *f_glTexCoord2fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* v = p;
   glTexCoord2fv(v);
   return 0;
}
*/

/*
//void glTexCoord2iv (const GLint *v);
static AbstractQoreNode *f_glTexCoord2iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* v = p;
   glTexCoord2iv(v);
   return 0;
}
*/

/*
//void glTexCoord2sv (const GLshort *v);
static AbstractQoreNode *f_glTexCoord2sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* v = p;
   glTexCoord2sv(v);
   return 0;
}
*/

/*
//void glTexCoord3dv (const GLdouble *v);
static AbstractQoreNode *f_glTexCoord3dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* v = p;
   glTexCoord3dv(v);
   return 0;
}
*/

/*
//void glTexCoord3fv (const GLfloat *v);
static AbstractQoreNode *f_glTexCoord3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* v = p;
   glTexCoord3fv(v);
   return 0;
}
*/

/*
//void glTexCoord3iv (const GLint *v);
static AbstractQoreNode *f_glTexCoord3iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* v = p;
   glTexCoord3iv(v);
   return 0;
}
*/

/*
//void glTexCoord3sv (const GLshort *v);
static AbstractQoreNode *f_glTexCoord3sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* v = p;
   glTexCoord3sv(v);
   return 0;
}
*/

/*
//void glTexCoord4dv (const GLdouble *v);
static AbstractQoreNode *f_glTexCoord4dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* v = p;
   glTexCoord4dv(v);
   return 0;
}
*/

/*
//void glTexCoord4fv (const GLfloat *v);
static AbstractQoreNode *f_glTexCoord4fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* v = p;
   glTexCoord4fv(v);
   return 0;
}
*/

/*
//void glTexCoord4iv (const GLint *v);
static AbstractQoreNode *f_glTexCoord4iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* v = p;
   glTexCoord4iv(v);
   return 0;
}
*/

/*
//void glTexCoord4sv (const GLshort *v);
static AbstractQoreNode *f_glTexCoord4sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* v = p;
   glTexCoord4sv(v);
   return 0;
}
*/

/*
//void glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
static AbstractQoreNode *f_glTexCoordPointer(const QoreListNode *params, ExceptionSink *xsink)
{
   glTexCoordPointer();
   return 0;
}
*/

//void glTexGend (GLenum coord, GLenum pname, GLdouble param);
static AbstractQoreNode *f_glTexGend(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum coord = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLdouble param = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glTexGend(coord, pname, param);
   return 0;
}

static int get_texgen_num(GLenum pname)
{
   switch (pname) {
      case GL_TEXTURE_GEN_MODE:
	 return 1;

      case GL_EYE_PLANE:
      case GL_OBJECT_PLANE:
	 return 4;
   }
   return -1;
}

//void glTexGendv (GLenum coord, GLenum pname, const GLdouble *params);
static AbstractQoreNode *f_glTexGendv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum coord = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   int num = get_texgen_num(pname);
   if (num == -1) {
      xsink->raiseException("GLTEXGENDV-ERROR", "cannot determine list argument length for glTexGendv() from unknown param code %d", (int)pname);
      return 0;
   }

   const QoreListNode *l = test_list_param(params, 2);
   if (!l) {
      xsink->raiseException("GLTEXGENDV-ERROR", "expecting a list as the third argument to glTexGendv()");
      return 0;
   }
   
   GLdouble parms[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      parms[i] = n ? n->getAsFloat() : 0;
   }

   glTexGendv(coord, pname, parms);
   return 0;
}

//void glTexGenf (GLenum coord, GLenum pname, GLfloat param);
static AbstractQoreNode *f_glTexGenf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum coord = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glTexGenf(coord, pname, param);
   return 0;
}

//void glTexGenfv (GLenum coord, GLenum pname, const GLfloat *params);
static AbstractQoreNode *f_glTexGenfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum coord = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   int num = get_texgen_num(pname);
   if (num == -1) {
      xsink->raiseException("GLTEXGENFV-ERROR", "cannot determine list argument length for glTexGenfv() from unknown param code %d", (int)pname);
      return 0;
   }

   const QoreListNode *l = test_list_param(params, 2);
   if (!l) {
      xsink->raiseException("GLTEXGENFV-ERROR", "expecting a list as the third argument to glTexGenfv()");
      return 0;
   }
   
   GLfloat parms[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      parms[i] = n ? n->getAsFloat() : 0;
   }

   glTexGenfv(coord, pname, parms);
   return 0;
}

//void glTexGeni (GLenum coord, GLenum pname, GLint param);
static AbstractQoreNode *f_glTexGeni(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum coord = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint param = (GLint)(p ? p->getAsInt() : 0);
   glTexGeni(coord, pname, param);
   return 0;
}

//void glTexGeniv (GLenum coord, GLenum pname, const GLint *params);
static AbstractQoreNode *f_glTexGeniv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum coord = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   int num = get_texgen_num(pname);
   if (num == -1) {
      xsink->raiseException("GLTEXGENIV-ERROR", "cannot determine list argument length for glTexGeniv() from unknown param code %d", (int)pname);
      return 0;
   }

   const QoreListNode *l = test_list_param(params, 2);
   if (!l) {
      xsink->raiseException("GLTEXGENIV-ERROR", "expecting a list as the third argument to glTexGeniv()");
      return 0;
   }
   
   GLint parms[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      parms[i] = n ? n->getAsInt() : 0;
   }

   glTexGeniv(coord, pname, parms);
   return 0;
}

//void glTexImage1D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
static AbstractQoreNode *f_glTexImage1D(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint level = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint border = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLenum format = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);

   // get bits per pixel
   int bpp = get_bits_per_pixel(type);
   if (bpp == -1) {
      xsink->raiseException("GLTEXIMAGE1D-ERROR", "cannot determine required buffer size for unknown type code %d", (int)type);
      return 0;
   }

   size_t pixel_bits = width * bpp;
   size_t size = pixel_bits / 8;
   if (size * 8 != pixel_bits)
      ++size;

   const BinaryNode *b = test_binary_param(params, 7);
   if (!b && get_param(params, 7)) {
      xsink->raiseException("GLTEXIMAGE1D-ERROR", "missing binary object for texture image as eighth argument");
      return 0;
   }
   
   if (b && b->size() < size) {
      xsink->raiseException("GLTEXTIMAGE1D-ERROR", "binary data passed has only %d byte%s, but %d bytes are required", b->size(), b->size() == 1 ? "" : "s", size);
      return 0;
   }

   const GLvoid *pixels = b ? b->getPtr() : 0;

   glTexImage1D(target, level, internalformat, width, border, format, type, pixels);
   return 0;
}

//void glTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
static AbstractQoreNode *f_glTexImage2D(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint level = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLint border = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLenum format = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);

   // get bits per pixel
   int bpp = get_bits_per_pixel(type);
   if (bpp == -1) {
      xsink->raiseException("GLTEXIMAGE2D-ERROR", "cannot determine required buffer size for unknown type code %d", (int)type);
      return 0;
   }

   size_t pixel_bits = width * height * bpp;
   size_t size = pixel_bits / 8;
   if (size * 8 != pixel_bits)
      ++size;

   const BinaryNode *b = test_binary_param(params, 8);
   if (!b && get_param(params, 8)) {
      xsink->raiseException("GLTEXIMAGE2D-ERROR", "missing binary object for texture image as ninth argument");
      return 0;
   }
   
   if (b && b->size() < size) {
      xsink->raiseException("GLTEXTIMAGE2D-ERROR", "binary data passed has only %d byte%s, but %d bytes are required", b->size(), b->size() == 1 ? "" : "s", size);
      return 0;
   }

   const GLvoid *pixels = b ? b->getPtr() : 0;

   glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
   return 0;
}

//void glTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
static AbstractQoreNode *f_glTexImage3D(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint level = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLsizei width = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLsizei height = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   GLsizei depth = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 6);
   GLint border = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 7);
   GLenum format = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 8);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);

   // get bits per pixel
   int bpp = get_bits_per_pixel(type);
   if (bpp == -1) {
      xsink->raiseException("GLTEXIMAGE3D-ERROR", "cannot determine required buffer size for unknown type code %d", (int)type);
      return 0;
   }

   size_t pixel_bits = width * height * depth * bpp;
   size_t size = pixel_bits / 8;
   if (size * 8 != pixel_bits)
      ++size;

   const BinaryNode *b = test_binary_param(params, 9);
   if (!b && get_param(params, 9)) {
      xsink->raiseException("GLTEXIMAGE3D-ERROR", "missing binary object for texture image as tenth argument");
      return 0;
   }
   
   if (b && b->size() < size) {
      xsink->raiseException("GLTEXTIMAGE3D-ERROR", "binary data passed has only %d byte%s, but %d bytes are required", b->size(), b->size() == 1 ? "" : "s", size);
      return 0;
   }

   const GLvoid *pixels = b ? b->getPtr() : 0;

   glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
   return 0;
}

//void glTexParameterf (GLenum target, GLenum pname, GLfloat param);
static AbstractQoreNode *f_glTexParameterf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glTexParameterf(target, pname, param);
   return 0;
}

int get_tex_param_num(GLenum pname)
{
   switch (pname) {
      case GL_TEXTURE_MAG_FILTER:
      case GL_TEXTURE_MIN_FILTER:
      case GL_TEXTURE_MIN_LOD:
      case GL_TEXTURE_MAX_LOD:
      case GL_TEXTURE_BASE_LEVEL:
      case GL_TEXTURE_MAX_LEVEL:
      case GL_TEXTURE_WRAP_S:
      case GL_TEXTURE_WRAP_T:
      case GL_TEXTURE_WRAP_R:
      case GL_TEXTURE_PRIORITY:
	 return 1;

      case GL_TEXTURE_BORDER_COLOR:
	 return 4;
   }
   return -1;
}

//void glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
static AbstractQoreNode *f_glTexParameterfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);

   int num = get_tex_param_num(pname);

   if (num == -1) {
      xsink->raiseException("GLTEXPARAMETERFV-ERROR", "cannot determine list size from unknown parameter code %d passed to glTexParameterfv()", (int)pname);
      return 0;
   }

   const QoreListNode *l = test_list_param(params, 2);
   if (!l) {
      xsink->raiseException("GLTEXPARAMETERFV-ERROR", "expecting a list as the third argument to glTexParameterfv()");
      return 0;
   }
   
   GLfloat parms[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      parms[i] = n ? n->getAsFloat() : 0;
   }

   glTexParameterfv(target, pname, parms);
   return 0;
}

//void glTexParameteri (GLenum target, GLenum pname, GLint param);
static AbstractQoreNode *f_glTexParameteri(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint param = (GLint)(p ? p->getAsInt() : 0);
   glTexParameteri(target, pname, param);
   return 0;
}

//void glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
static AbstractQoreNode *f_glTexParameteriv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   int num = get_tex_param_num(pname);

   if (num == -1) {
      xsink->raiseException("GLTEXPARAMETERIV-ERROR", "cannot determine list size from unknown parameter code %d passed to glTexParameteriv()", (int)pname);
      return 0;
   }

   const QoreListNode *l = test_list_param(params, 2);
   if (!l) {
      xsink->raiseException("GLTEXPARAMETERIV-ERROR", "expecting a list as the third argument to glTexParameteriv()");
      return 0;
   }
   
   GLint parms[num];
   for (int i = 0; i < num; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      parms[i] = n ? n->getAsInt() : 0;
   }

   glTexParameteriv(target, pname, parms);
   return 0;
}

/*
//void glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
static AbstractQoreNode *f_glTexSubImage1D(const QoreListNode *params, ExceptionSink *xsink)
{
   glTexSubImage1D();
   return 0;
}
*/

/*
//void glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
static AbstractQoreNode *f_glTexSubImage2D(const QoreListNode *params, ExceptionSink *xsink)
{
   glTexSubImage2D();
   return 0;
}
*/

/*
//void glTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
static AbstractQoreNode *f_glTexSubImage3D(const QoreListNode *params, ExceptionSink *xsink)
{
   glTexSubImage3D();
   return 0;
}
*/

//void glVertex2dv (const GLdouble *v);
static AbstractQoreNode *f_glVertex2dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLVERTEX2DV-ERROR", "expecting a list as sole argument to glVertex2dv()");
      return 0;
   }

   GLdouble v[2];
   for (int i = 0; i < 2; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsFloat() : 0;
   }

   glVertex2dv(v);
   return 0;
}

//void glVertex2fv (const GLfloat *v);
static AbstractQoreNode *f_glVertex2fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLVERTEX2FV-ERROR", "expecting a list as sole argument to glVertex2fv()");
      return 0;
   }

   GLfloat v[2];
   for (int i = 0; i < 2; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsFloat() : 0;
   }

   glVertex2fv(v);
   return 0;
}

//void glVertex2iv (const GLint *v);
static AbstractQoreNode *f_glVertex2iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLVERTEX2IV-ERROR", "expecting a list as sole argument to glVertex2iv()");
      return 0;
   }

   GLint v[2];
   for (int i = 0; i < 2; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glVertex2iv(v);
   return 0;
}

//void glVertex2sv (const GLshort *v);
static AbstractQoreNode *f_glVertex2sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLVERTEX2SV-ERROR", "expecting a list as sole argument to glVertex2sv()");
      return 0;
   }

   GLshort v[2];
   for (int i = 0; i < 2; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glVertex2sv(v);
   return 0;
}

//void glVertex3dv (const GLdouble *v);
static AbstractQoreNode *f_glVertex3dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLVERTEX3DV-ERROR", "expecting a list as sole argument to glVertex3dv()");
      return 0;
   }

   GLdouble v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsFloat() : 0;
   }

   glVertex3dv(v);
   return 0;
}

//void glVertex3fv (const GLfloat *v);
static AbstractQoreNode *f_glVertex3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLVERTEX3FV-ERROR", "expecting a list as sole argument to glVertex3fv()");
      return 0;
   }

   GLfloat v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsFloat() : 0;
   }

   glVertex3fv(v);
   return 0;
}

//void glVertex3iv (const GLint *v);
static AbstractQoreNode *f_glVertex3iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLVERTEX3IV-ERROR", "expecting a list as sole argument to glVertex3iv()");
      return 0;
   }

   GLint v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glVertex3iv(v);
   return 0;
}

//void glVertex3sv (const GLshort *v);
static AbstractQoreNode *f_glVertex3sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLVERTEX3SV-ERROR", "expecting a list as sole argument to glVertex3sv()");
      return 0;
   }

   GLshort v[3];
   for (int i = 0; i < 3; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glVertex3sv(v);
   return 0;
}

//void glVertex4dv (const GLdouble *v);
static AbstractQoreNode *f_glVertex4dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLVERTEX4DV-ERROR", "expecting a list as sole argument to glVertex4dv()");
      return 0;
   }

   GLdouble v[4];
   for (int i = 0; i < 4; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsFloat() : 0;
   }

   glVertex4dv(v);
   return 0;
}

//void glVertex4fv (const GLfloat *v);
static AbstractQoreNode *f_glVertex4fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLVERTEX4FV-ERROR", "expecting a list as sole argument to glVertex4fv()");
      return 0;
   }

   GLfloat v[4];
   for (int i = 0; i < 4; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsFloat() : 0;
   }

   glVertex4fv(v);
   return 0;
}

//void glVertex4iv (const GLint *v);
static AbstractQoreNode *f_glVertex4iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLVERTEX4IV-ERROR", "expecting a list as sole argument to glVertex4iv()");
      return 0;
   }

   GLint v[4];
   for (int i = 0; i < 4; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glVertex4iv(v);
   return 0;
}

//void glVertex4sv (const GLshort *v);
static AbstractQoreNode *f_glVertex4sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreListNode *l = test_list_param(params, 0);
   if (!l) {
      xsink->raiseException("GLVERTEX4SV-ERROR", "expecting a list as sole argument to glVertex4sv()");
      return 0;
   }

   GLshort v[4];
   for (int i = 0; i < 4; ++i) {
      const AbstractQoreNode *n = l->retrieve_entry(i);
      v[i] = n ? n->getAsInt() : 0;
   }

   glVertex4sv(v);
   return 0;
}

/*
//void glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
static AbstractQoreNode *f_glVertexPointer(const QoreListNode *params, ExceptionSink *xsink)
{
   glVertexPointer();
   return 0;
}
*/

//void glSampleCoverage (GLclampf value, GLboolean invert);
static AbstractQoreNode *f_glSampleCoverage(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLclampf value = (GLclampf)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLboolean invert = (GLboolean)(p ? p->getAsBool() : 0);
   glSampleCoverage(value, invert);
   return 0;
}

#ifdef HAVE_GLSAMPLEPASS
//void glSamplePass (GLenum pass);
static AbstractQoreNode *f_glSamplePass(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pass = (GLenum)(p ? p->getAsInt() : 0);
   glSamplePass(pass);
   return 0;
}
#endif

/*
//void glLoadTransposeMatrixf (const GLfloat *m);
static AbstractQoreNode *f_glLoadTransposeMatrixf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* m = p;
   glLoadTransposeMatrixf(m);
   return 0;
}
*/

/*
//void glLoadTransposeMatrixd (const GLdouble *m);
static AbstractQoreNode *f_glLoadTransposeMatrixd(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* m = p;
   glLoadTransposeMatrixd(m);
   return 0;
}
*/

/*
//void glMultTransposeMatrixf (const GLfloat *m);
static AbstractQoreNode *f_glMultTransposeMatrixf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* m = p;
   glMultTransposeMatrixf(m);
   return 0;
}
*/

/*
//void glMultTransposeMatrixd (const GLdouble *m);
static AbstractQoreNode *f_glMultTransposeMatrixd(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* m = p;
   glMultTransposeMatrixd(m);
   return 0;
}
*/

/*
//void glCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
static AbstractQoreNode *f_glCompressedTexImage3D(const QoreListNode *params, ExceptionSink *xsink)
{
   glCompressedTexImage3D();
   return 0;
}
*/

/*
//void glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
static AbstractQoreNode *f_glCompressedTexImage2D(const QoreListNode *params, ExceptionSink *xsink)
{
   glCompressedTexImage2D();
   return 0;
}
*/

/*
//void glCompressedTexImage1D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
static AbstractQoreNode *f_glCompressedTexImage1D(const QoreListNode *params, ExceptionSink *xsink)
{
   glCompressedTexImage1D();
   return 0;
}
*/

/*
//void glCompressedTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
static AbstractQoreNode *f_glCompressedTexSubImage3D(const QoreListNode *params, ExceptionSink *xsink)
{
   glCompressedTexSubImage3D();
   return 0;
}
*/

/*
//void glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
static AbstractQoreNode *f_glCompressedTexSubImage2D(const QoreListNode *params, ExceptionSink *xsink)
{
   glCompressedTexSubImage2D();
   return 0;
}
*/

/*
//void glCompressedTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
static AbstractQoreNode *f_glCompressedTexSubImage1D(const QoreListNode *params, ExceptionSink *xsink)
{
   glCompressedTexSubImage1D();
   return 0;
}
*/

/*
//void glGetCompressedTexImage (GLenum target, GLint lod, GLvoid *img);
static AbstractQoreNode *f_glGetCompressedTexImage(const QoreListNode *params, ExceptionSink *xsink)
{
   glGetCompressedTexImage();
   return 0;
}
*/

//void glActiveTexture (GLenum texture);
static AbstractQoreNode *f_glActiveTexture(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum texture = (GLenum)(p ? p->getAsInt() : 0);
   glActiveTexture(texture);
   return 0;
}

//void glClientActiveTexture (GLenum texture);
static AbstractQoreNode *f_glClientActiveTexture(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum texture = (GLenum)(p ? p->getAsInt() : 0);
   glClientActiveTexture(texture);
   return 0;
}

//void glMultiTexCoord1d (GLenum target, GLdouble s);
static AbstractQoreNode *f_glMultiTexCoord1d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLdouble s = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord1d(target, s);
   return 0;
}

/*
//void glMultiTexCoord1dv (GLenum target, const GLdouble *v);
static AbstractQoreNode *f_glMultiTexCoord1dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLdouble* v = p;
   glMultiTexCoord1dv(target, v);
   return 0;
}
*/

//void glMultiTexCoord1f (GLenum target, GLfloat s);
static AbstractQoreNode *f_glMultiTexCoord1f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat s = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord1f(target, s);
   return 0;
}

/*
//void glMultiTexCoord1fv (GLenum target, const GLfloat *v);
static AbstractQoreNode *f_glMultiTexCoord1fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLfloat* v = p;
   glMultiTexCoord1fv(target, v);
   return 0;
}
*/

//void glMultiTexCoord1i (GLenum target, GLint s);
static AbstractQoreNode *f_glMultiTexCoord1i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint s = (GLint)(p ? p->getAsInt() : 0);
   glMultiTexCoord1i(target, s);
   return 0;
}

/*
//void glMultiTexCoord1iv (GLenum target, const GLint *v);
static AbstractQoreNode *f_glMultiTexCoord1iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* v = p;
   glMultiTexCoord1iv(target, v);
   return 0;
}
*/

//void glMultiTexCoord1s (GLenum target, GLshort s);
static AbstractQoreNode *f_glMultiTexCoord1s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort s = (GLshort)(p ? p->getAsInt() : 0);
   glMultiTexCoord1s(target, s);
   return 0;
}

/*
//void glMultiTexCoord1sv (GLenum target, const GLshort *v);
static AbstractQoreNode *f_glMultiTexCoord1sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glMultiTexCoord1sv(target, v);
   return 0;
}
*/

//void glMultiTexCoord2d (GLenum target, GLdouble s, GLdouble t);
static AbstractQoreNode *f_glMultiTexCoord2d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLdouble s = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble t = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord2d(target, s, t);
   return 0;
}

/*
//void glMultiTexCoord2dv (GLenum target, const GLdouble *v);
static AbstractQoreNode *f_glMultiTexCoord2dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLdouble* v = p;
   glMultiTexCoord2dv(target, v);
   return 0;
}
*/

//void glMultiTexCoord2f (GLenum target, GLfloat s, GLfloat t);
static AbstractQoreNode *f_glMultiTexCoord2f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat s = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat t = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord2f(target, s, t);
   return 0;
}

/*
//void glMultiTexCoord2fv (GLenum target, const GLfloat *v);
static AbstractQoreNode *f_glMultiTexCoord2fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLfloat* v = p;
   glMultiTexCoord2fv(target, v);
   return 0;
}
*/

//void glMultiTexCoord2i (GLenum target, GLint s, GLint t);
static AbstractQoreNode *f_glMultiTexCoord2i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint s = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint t = (GLint)(p ? p->getAsInt() : 0);
   glMultiTexCoord2i(target, s, t);
   return 0;
}

/*
//void glMultiTexCoord2iv (GLenum target, const GLint *v);
static AbstractQoreNode *f_glMultiTexCoord2iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* v = p;
   glMultiTexCoord2iv(target, v);
   return 0;
}
*/

//void glMultiTexCoord2s (GLenum target, GLshort s, GLshort t);
static AbstractQoreNode *f_glMultiTexCoord2s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort s = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort t = (GLshort)(p ? p->getAsInt() : 0);
   glMultiTexCoord2s(target, s, t);
   return 0;
}

/*
//void glMultiTexCoord2sv (GLenum target, const GLshort *v);
static AbstractQoreNode *f_glMultiTexCoord2sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glMultiTexCoord2sv(target, v);
   return 0;
}
*/

//void glMultiTexCoord3d (GLenum target, GLdouble s, GLdouble t, GLdouble r);
static AbstractQoreNode *f_glMultiTexCoord3d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLdouble s = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble t = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble r = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord3d(target, s, t, r);
   return 0;
}

/*
//void glMultiTexCoord3dv (GLenum target, const GLdouble *v);
static AbstractQoreNode *f_glMultiTexCoord3dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLdouble* v = p;
   glMultiTexCoord3dv(target, v);
   return 0;
}
*/

//void glMultiTexCoord3f (GLenum target, GLfloat s, GLfloat t, GLfloat r);
static AbstractQoreNode *f_glMultiTexCoord3f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat s = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat t = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat r = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord3f(target, s, t, r);
   return 0;
}

/*
//void glMultiTexCoord3fv (GLenum target, const GLfloat *v);
static AbstractQoreNode *f_glMultiTexCoord3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLfloat* v = p;
   glMultiTexCoord3fv(target, v);
   return 0;
}
*/

//void glMultiTexCoord3i (GLenum target, GLint s, GLint t, GLint r);
static AbstractQoreNode *f_glMultiTexCoord3i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint s = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint t = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint r = (GLint)(p ? p->getAsInt() : 0);
   glMultiTexCoord3i(target, s, t, r);
   return 0;
}

/*
//void glMultiTexCoord3iv (GLenum target, const GLint *v);
static AbstractQoreNode *f_glMultiTexCoord3iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* v = p;
   glMultiTexCoord3iv(target, v);
   return 0;
}
*/

//void glMultiTexCoord3s (GLenum target, GLshort s, GLshort t, GLshort r);
static AbstractQoreNode *f_glMultiTexCoord3s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort s = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort t = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLshort r = (GLshort)(p ? p->getAsInt() : 0);
   glMultiTexCoord3s(target, s, t, r);
   return 0;
}

/*
//void glMultiTexCoord3sv (GLenum target, const GLshort *v);
static AbstractQoreNode *f_glMultiTexCoord3sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glMultiTexCoord3sv(target, v);
   return 0;
}
*/

//void glMultiTexCoord4d (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
static AbstractQoreNode *f_glMultiTexCoord4d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLdouble s = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble t = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble r = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLdouble q = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord4d(target, s, t, r, q);
   return 0;
}

/*
//void glMultiTexCoord4dv (GLenum target, const GLdouble *v);
static AbstractQoreNode *f_glMultiTexCoord4dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLdouble* v = p;
   glMultiTexCoord4dv(target, v);
   return 0;
}
*/

//void glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
static AbstractQoreNode *f_glMultiTexCoord4f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat s = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat t = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat r = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLfloat q = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glMultiTexCoord4f(target, s, t, r, q);
   return 0;
}

/*
//void glMultiTexCoord4fv (GLenum target, const GLfloat *v);
static AbstractQoreNode *f_glMultiTexCoord4fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLfloat* v = p;
   glMultiTexCoord4fv(target, v);
   return 0;
}
*/

//void glMultiTexCoord4i (GLenum target, GLint, GLint s, GLint t, GLint r);
static AbstractQoreNode *f_glMultiTexCoord4i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint glint = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint s = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint t = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint r = (GLint)(p ? p->getAsInt() : 0);
   glMultiTexCoord4i(target, glint, s, t, r);
   return 0;
}

/*
//void glMultiTexCoord4iv (GLenum target, const GLint *v);
static AbstractQoreNode *f_glMultiTexCoord4iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* v = p;
   glMultiTexCoord4iv(target, v);
   return 0;
}
*/

//void glMultiTexCoord4s (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
static AbstractQoreNode *f_glMultiTexCoord4s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort s = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort t = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLshort r = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLshort q = (GLshort)(p ? p->getAsInt() : 0);
   glMultiTexCoord4s(target, s, t, r, q);
   return 0;
}

/*
//void glMultiTexCoord4sv (GLenum target, const GLshort *v);
static AbstractQoreNode *f_glMultiTexCoord4sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glMultiTexCoord4sv(target, v);
   return 0;
}
*/

//void glFogCoordf (GLfloat coord);
static AbstractQoreNode *f_glFogCoordf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat coord = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glFogCoordf(coord);
   return 0;
}

/*
//void glFogCoordfv (const GLfloat *coord);
static AbstractQoreNode *f_glFogCoordfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* coord = p;
   glFogCoordfv(coord);
   return 0;
}
*/

//void glFogCoordd (GLdouble coord);
static AbstractQoreNode *f_glFogCoordd(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble coord = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glFogCoordd(coord);
   return 0;
}

/*
//void glFogCoorddv (const GLdouble * coord);
static AbstractQoreNode *f_glFogCoorddv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* coord = p;
   glFogCoorddv(coord);
   return 0;
}
*/

/*
//void glFogCoordPointer (GLenum type, GLsizei stride, const GLvoid *pointer);
static AbstractQoreNode *f_glFogCoordPointer(const QoreListNode *params, ExceptionSink *xsink)
{
   glFogCoordPointer();
   return 0;
}
*/

//void glSecondaryColor3b (GLbyte red, GLbyte green, GLbyte blue);
static AbstractQoreNode *f_glSecondaryColor3b(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLbyte red = (GLbyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLbyte green = (GLbyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLbyte blue = (GLbyte)(p ? p->getAsInt() : 0);
   glSecondaryColor3b(red, green, blue);
   return 0;
}

/*
//void glSecondaryColor3bv (const GLbyte *v);
static AbstractQoreNode *f_glSecondaryColor3bv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLbyte* v = p;
   glSecondaryColor3bv(v);
   return 0;
}
*/

//void glSecondaryColor3d (GLdouble red, GLdouble green, GLdouble blue);
static AbstractQoreNode *f_glSecondaryColor3d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble red = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble green = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble blue = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glSecondaryColor3d(red, green, blue);
   return 0;
}

/*
//void glSecondaryColor3dv (const GLdouble *v);
static AbstractQoreNode *f_glSecondaryColor3dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* v = p;
   glSecondaryColor3dv(v);
   return 0;
}
*/

//void glSecondaryColor3f (GLfloat red, GLfloat green, GLfloat blue);
static AbstractQoreNode *f_glSecondaryColor3f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat red = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat green = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat blue = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glSecondaryColor3f(red, green, blue);
   return 0;
}

/*
//void glSecondaryColor3fv (const GLfloat *v);
static AbstractQoreNode *f_glSecondaryColor3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* v = p;
   glSecondaryColor3fv(v);
   return 0;
}
*/

//void glSecondaryColor3i (GLint red, GLint green, GLint blue);
static AbstractQoreNode *f_glSecondaryColor3i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint red = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint green = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint blue = (GLint)(p ? p->getAsInt() : 0);
   glSecondaryColor3i(red, green, blue);
   return 0;
}

/*
//void glSecondaryColor3iv (const GLint *v);
static AbstractQoreNode *f_glSecondaryColor3iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* v = p;
   glSecondaryColor3iv(v);
   return 0;
}
*/

//void glSecondaryColor3s (GLshort red, GLshort green, GLshort blue);
static AbstractQoreNode *f_glSecondaryColor3s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort red = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort green = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort blue = (GLshort)(p ? p->getAsInt() : 0);
   glSecondaryColor3s(red, green, blue);
   return 0;
}

/*
//void glSecondaryColor3sv (const GLshort *v);
static AbstractQoreNode *f_glSecondaryColor3sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* v = p;
   glSecondaryColor3sv(v);
   return 0;
}
*/

//void glSecondaryColor3ub (GLubyte red, GLubyte green, GLubyte blue);
static AbstractQoreNode *f_glSecondaryColor3ub(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLubyte red = (GLubyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLubyte green = (GLubyte)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLubyte blue = (GLubyte)(p ? p->getAsInt() : 0);
   glSecondaryColor3ub(red, green, blue);
   return 0;
}

/*
//void glSecondaryColor3ubv (const GLubyte *v);
static AbstractQoreNode *f_glSecondaryColor3ubv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLubyte* v = p;
   glSecondaryColor3ubv(v);
   return 0;
}
*/

//void glSecondaryColor3ui (GLuint red, GLuint green, GLuint blue);
static AbstractQoreNode *f_glSecondaryColor3ui(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned red = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned green = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   unsigned blue = p ? p->getAsBigInt() : 0;
   glSecondaryColor3ui(red, green, blue);
   return 0;
}

/*
//void glSecondaryColor3uiv (const GLuint *v);
static AbstractQoreNode *f_glSecondaryColor3uiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLuint* v = p;
   glSecondaryColor3uiv(v);
   return 0;
}
*/

//void glSecondaryColor3us (GLushort red, GLushort green, GLushort blue);
static AbstractQoreNode *f_glSecondaryColor3us(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLushort red = (GLushort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLushort green = (GLushort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLushort blue = (GLushort)(p ? p->getAsInt() : 0);
   glSecondaryColor3us(red, green, blue);
   return 0;
}

/*
//void glSecondaryColor3usv (const GLushort *v);
static AbstractQoreNode *f_glSecondaryColor3usv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLushort* v = p;
   glSecondaryColor3usv(v);
   return 0;
}
*/

 /*
//void glSecondaryColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
static AbstractQoreNode *f_glSecondaryColorPointer(const QoreListNode *params, ExceptionSink *xsink)
{
   glSecondaryColorPointer();
   return 0;
}
 */

//void glPointParameterf (GLenum pname, GLfloat param);
static AbstractQoreNode *f_glPointParameterf(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat param = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glPointParameterf(pname, param);
   return 0;
}

/*
//void glPointParameterfv (GLenum pname, const GLfloat *params);
static AbstractQoreNode *f_glPointParameterfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLfloat* params = p;
   glPointParameterfv(pname, params);
   return 0;
}
*/

//void glPointParameteri (GLenum pname, GLint param);
static AbstractQoreNode *f_glPointParameteri(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint param = (GLint)(p ? p->getAsInt() : 0);
   glPointParameteri(pname, param);
   return 0;
}

/*
//void glPointParameteriv (GLenum pname, const GLint *params);
static AbstractQoreNode *f_glPointParameteriv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* params = p;
   glPointParameteriv(pname, params);
   return 0;
}
*/

//void glBlendFuncSeparate (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
static AbstractQoreNode *f_glBlendFuncSeparate(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum srcRGB = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum dstRGB = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum srcAlpha = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum dstAlpha = (GLenum)(p ? p->getAsInt() : 0);
   glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
   return 0;
}

/*
//void glMultiDrawArrays (GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount);
static AbstractQoreNode *f_glMultiDrawArrays(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLint* first = p;
   p = get_param(params, 2);
   ??? GLsizei* count = p;
   p = get_param(params, 3);
   GLsizei primcount = (GLsizei)(p ? p->getAsInt() : 0);
   glMultiDrawArrays(mode, first, count, primcount);
   return 0;
}
*/

/*
//void glMultiDrawElements (GLenum mode, const GLsizei *count, GLenum type, const GLvoid* *indices, GLsizei primcount);
static AbstractQoreNode *f_glMultiDrawElements(const QoreListNode *params, ExceptionSink *xsink)
{
   glMultiDrawElements();
   return 0;
}
*/

//void glWindowPos2d (GLdouble x, GLdouble y);
static AbstractQoreNode *f_glWindowPos2d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glWindowPos2d(x, y);
   return 0;
}

/*
//void glWindowPos2dv (const GLdouble *v);
static AbstractQoreNode *f_glWindowPos2dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* v = p;
   glWindowPos2dv(v);
   return 0;
}
*/

//void glWindowPos2f (GLfloat x, GLfloat y);
static AbstractQoreNode *f_glWindowPos2f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glWindowPos2f(x, y);
   return 0;
}

/*
//void glWindowPos2fv (const GLfloat *v);
static AbstractQoreNode *f_glWindowPos2fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* v = p;
   glWindowPos2fv(v);
   return 0;
}
*/

//void glWindowPos2i (GLint x, GLint y);
static AbstractQoreNode *f_glWindowPos2i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   glWindowPos2i(x, y);
   return 0;
}

/*
//void glWindowPos2iv (const GLint *v);
static AbstractQoreNode *f_glWindowPos2iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* v = p;
   glWindowPos2iv(v);
   return 0;
}
*/

//void glWindowPos2s (GLshort x, GLshort y);
static AbstractQoreNode *f_glWindowPos2s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   glWindowPos2s(x, y);
   return 0;
}

/*
//void glWindowPos2sv (const GLshort *v);
static AbstractQoreNode *f_glWindowPos2sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* v = p;
   glWindowPos2sv(v);
   return 0;
}
*/

//void glWindowPos3d (GLdouble x, GLdouble y, GLdouble z);
static AbstractQoreNode *f_glWindowPos3d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glWindowPos3d(x, y, z);
   return 0;
}

/*
//void glWindowPos3dv (const GLdouble *v);
static AbstractQoreNode *f_glWindowPos3dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLdouble* v = p;
   glWindowPos3dv(v);
   return 0;
}
*/

//void glWindowPos3f (GLfloat x, GLfloat y, GLfloat z);
static AbstractQoreNode *f_glWindowPos3f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glWindowPos3f(x, y, z);
   return 0;
}

/*
//void glWindowPos3fv (const GLfloat *v);
static AbstractQoreNode *f_glWindowPos3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLfloat* v = p;
   glWindowPos3fv(v);
   return 0;
}
*/

//void glWindowPos3i (GLint x, GLint y, GLint z);
static AbstractQoreNode *f_glWindowPos3i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint x = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint y = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint z = (GLint)(p ? p->getAsInt() : 0);
   glWindowPos3i(x, y, z);
   return 0;
}

/*
//void glWindowPos3iv (const GLint *v);
static AbstractQoreNode *f_glWindowPos3iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLint* v = p;
   glWindowPos3iv(v);
   return 0;
}
*/

//void glWindowPos3s (GLshort x, GLshort y, GLshort z);
static AbstractQoreNode *f_glWindowPos3s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort z = (GLshort)(p ? p->getAsInt() : 0);
   glWindowPos3s(x, y, z);
   return 0;
}

/*
//void glWindowPos3sv (const GLshort *v);
static AbstractQoreNode *f_glWindowPos3sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   ??? GLshort* v = p;
   glWindowPos3sv(v);
   return 0;
}
*/

/*
//void glGenQueries (GLsizei n, GLuint *ids);
static AbstractQoreNode *f_glGenQueries(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* ids = p;
   glGenQueries(n, ids);
   return 0;
}
*/

/*
//void glDeleteQueries (GLsizei n, const GLuint *ids);
static AbstractQoreNode *f_glDeleteQueries(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* ids = p;
   glDeleteQueries(n, ids);
   return 0;
}
*/

//GLboolean glIsQuery (GLuint id);
static AbstractQoreNode *f_glIsQuery(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned id = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsQuery(id));
}

//void glBeginQuery (GLenum target, GLuint id);
static AbstractQoreNode *f_glBeginQuery(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned id = p ? p->getAsBigInt() : 0;
   glBeginQuery(target, id);
   return 0;
}

//void glEndQuery (GLenum target);
static AbstractQoreNode *f_glEndQuery(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   glEndQuery(target);
   return 0;
}

/*
//void glGetQueryiv (GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetQueryiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetQueryiv(target, pname, params);
   return 0;
}
*/

/*
//void glGetQueryObjectiv (GLuint id, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetQueryObjectiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned id = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetQueryObjectiv(id, pname, params);
   return 0;
}
*/

/*
//void glGetQueryObjectuiv (GLuint id, GLenum pname, GLuint *params);
static AbstractQoreNode *f_glGetQueryObjectuiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned id = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLuint* params = p;
   glGetQueryObjectuiv(id, pname, params);
   return 0;
}
*/

//void glBindBuffer (GLenum target, GLuint buffer);
static AbstractQoreNode *f_glBindBuffer(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned buffer = p ? p->getAsBigInt() : 0;
   glBindBuffer(target, buffer);
   return 0;
}

/*
//void glDeleteBuffers (GLsizei n, const GLuint *buffers);
static AbstractQoreNode *f_glDeleteBuffers(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* buffers = p;
   glDeleteBuffers(n, buffers);
   return 0;
}
*/

/*
//void glGenBuffers (GLsizei n, GLuint *buffers);
static AbstractQoreNode *f_glGenBuffers(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLuint* buffers = p;
   glGenBuffers(n, buffers);
   return 0;
}
*/

//GLboolean glIsBuffer (GLuint buffer);
static AbstractQoreNode *f_glIsBuffer(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned buffer = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsBuffer(buffer));
}

/*
//void glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
static AbstractQoreNode *f_glBufferData(const QoreListNode *params, ExceptionSink *xsink)
{
   glBufferData();
   return 0;
}
*/

/*
//void glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
static AbstractQoreNode *f_glBufferSubData(const QoreListNode *params, ExceptionSink *xsink)
{
   glBufferSubData();
   return 0;
}
*/

/*
//void glGetBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid *data);
static AbstractQoreNode *f_glGetBufferSubData(const QoreListNode *params, ExceptionSink *xsink)
{
   glGetBufferSubData();
   return 0;
}
*/

/*
//GLvoid * glMapBuffer (GLenum target, GLenum access);
static AbstractQoreNode *f_glMapBuffer(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum access = (GLenum)(p ? p->getAsInt() : 0);
   ??? return new QoreBigIntNode(glMapBuffer(target, access));
}
*/

/*
//GLboolean glUnmapBuffer (GLenum target);
static AbstractQoreNode *f_glUnmapBuffer(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   ??? return new QoreBigIntNode(glUnmapBuffer(target));
}
*/

/*
//void glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetBufferParameteriv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetBufferParameteriv(target, pname, params);
   return 0;
}
*/

 /*
//void glGetBufferPointerv (GLenum target, GLenum pname, GLvoid **params);
static AbstractQoreNode *f_glGetBufferPointerv(const QoreListNode *params, ExceptionSink *xsink)
{
   glGetBufferPointerv();
   return 0;
}
 */

/*
//void glDrawBuffers (GLsizei n, const GLenum *bufs);
static AbstractQoreNode *f_glDrawBuffers(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLsizei n = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   ??? GLenum* bufs = p;
   glDrawBuffers(n, bufs);
   return 0;
}
*/

//void glVertexAttrib1d (GLuint index, GLdouble x);
static AbstractQoreNode *f_glVertexAttrib1d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib1d(index, x);
   return 0;
}

/*
//void glVertexAttrib1dv (GLuint index, const GLdouble *v);
static AbstractQoreNode *f_glVertexAttrib1dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLdouble* v = p;
   glVertexAttrib1dv(index, v);
   return 0;
}
*/

//void glVertexAttrib1f (GLuint index, GLfloat x);
static AbstractQoreNode *f_glVertexAttrib1f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib1f(index, x);
   return 0;
}

/*
//void glVertexAttrib1fv (GLuint index, const GLfloat *v);
static AbstractQoreNode *f_glVertexAttrib1fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLfloat* v = p;
   glVertexAttrib1fv(index, v);
   return 0;
}
*/

//void glVertexAttrib1s (GLuint index, GLshort x);
static AbstractQoreNode *f_glVertexAttrib1s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   glVertexAttrib1s(index, x);
   return 0;
}

/*
//void glVertexAttrib1sv (GLuint index, const GLshort *v);
static AbstractQoreNode *f_glVertexAttrib1sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glVertexAttrib1sv(index, v);
   return 0;
}
*/

//void glVertexAttrib2d (GLuint index, GLdouble x, GLdouble y);
static AbstractQoreNode *f_glVertexAttrib2d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib2d(index, x, y);
   return 0;
}

/*
//void glVertexAttrib2dv (GLuint index, const GLdouble *v);
static AbstractQoreNode *f_glVertexAttrib2dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLdouble* v = p;
   glVertexAttrib2dv(index, v);
   return 0;
}
*/

//void glVertexAttrib2f (GLuint index, GLfloat x, GLfloat y);
static AbstractQoreNode *f_glVertexAttrib2f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib2f(index, x, y);
   return 0;
}

/*
//void glVertexAttrib2fv (GLuint index, const GLfloat *v);
static AbstractQoreNode *f_glVertexAttrib2fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLfloat* v = p;
   glVertexAttrib2fv(index, v);
   return 0;
}
*/

//void glVertexAttrib2s (GLuint index, GLshort x, GLshort y);
static AbstractQoreNode *f_glVertexAttrib2s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   glVertexAttrib2s(index, x, y);
   return 0;
}

/*
//void glVertexAttrib2sv (GLuint index, const GLshort *v);
static AbstractQoreNode *f_glVertexAttrib2sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glVertexAttrib2sv(index, v);
   return 0;
}
*/

//void glVertexAttrib3d (GLuint index, GLdouble x, GLdouble y, GLdouble z);
static AbstractQoreNode *f_glVertexAttrib3d(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLdouble x = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLdouble y = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLdouble z = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib3d(index, x, y, z);
   return 0;
}

/*
//void glVertexAttrib3dv (GLuint index, const GLdouble *v);
static AbstractQoreNode *f_glVertexAttrib3dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLdouble* v = p;
   glVertexAttrib3dv(index, v);
   return 0;
}
*/

//void glVertexAttrib3f (GLuint index, GLfloat x, GLfloat y, GLfloat z);
static AbstractQoreNode *f_glVertexAttrib3f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLfloat x = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat y = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat z = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glVertexAttrib3f(index, x, y, z);
   return 0;
}

/*
//void glVertexAttrib3fv (GLuint index, const GLfloat *v);
static AbstractQoreNode *f_glVertexAttrib3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLfloat* v = p;
   glVertexAttrib3fv(index, v);
   return 0;
}
*/

//void glVertexAttrib3s (GLuint index, GLshort x, GLshort y, GLshort z);
static AbstractQoreNode *f_glVertexAttrib3s(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLshort x = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLshort y = (GLshort)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLshort z = (GLshort)(p ? p->getAsInt() : 0);
   glVertexAttrib3s(index, x, y, z);
   return 0;
}

/*
//void glVertexAttrib3sv (GLuint index, const GLshort *v);
static AbstractQoreNode *f_glVertexAttrib3sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glVertexAttrib3sv(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4Nbv (GLuint index, const GLbyte *v);
static AbstractQoreNode *f_glVertexAttrib4Nbv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLbyte* v = p;
   glVertexAttrib4Nbv(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4Niv (GLuint index, const GLint *v);
static AbstractQoreNode *f_glVertexAttrib4Niv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLint* v = p;
   glVertexAttrib4Niv(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4Nsv (GLuint index, const GLshort *v);
static AbstractQoreNode *f_glVertexAttrib4Nsv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glVertexAttrib4Nsv(index, v);
   return 0;
}
*/

//void glVertexAttrib4Nub (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
static AbstractQoreNode *f_glVertexAttrib4Nub(const QoreListNode *params, ExceptionSink *xsink)
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
   glVertexAttrib4Nub(index, x, y, z, w);
   return 0;
}

/*
//void glVertexAttrib4Nubv (GLuint index, const GLubyte *v);
static AbstractQoreNode *f_glVertexAttrib4Nubv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLubyte* v = p;
   glVertexAttrib4Nubv(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4Nuiv (GLuint index, const GLuint *v);
static AbstractQoreNode *f_glVertexAttrib4Nuiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLuint* v = p;
   glVertexAttrib4Nuiv(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4Nusv (GLuint index, const GLushort *v);
static AbstractQoreNode *f_glVertexAttrib4Nusv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLushort* v = p;
   glVertexAttrib4Nusv(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4bv (GLuint index, const GLbyte *v);
static AbstractQoreNode *f_glVertexAttrib4bv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLbyte* v = p;
   glVertexAttrib4bv(index, v);
   return 0;
}
*/

//void glVertexAttrib4d (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
static AbstractQoreNode *f_glVertexAttrib4d(const QoreListNode *params, ExceptionSink *xsink)
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
   glVertexAttrib4d(index, x, y, z, w);
   return 0;
}

/*
//void glVertexAttrib4dv (GLuint index, const GLdouble *v);
static AbstractQoreNode *f_glVertexAttrib4dv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLdouble* v = p;
   glVertexAttrib4dv(index, v);
   return 0;
}
*/

//void glVertexAttrib4f (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
static AbstractQoreNode *f_glVertexAttrib4f(const QoreListNode *params, ExceptionSink *xsink)
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
   glVertexAttrib4f(index, x, y, z, w);
   return 0;
}

/*
//void glVertexAttrib4fv (GLuint index, const GLfloat *v);
static AbstractQoreNode *f_glVertexAttrib4fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLfloat* v = p;
   glVertexAttrib4fv(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4iv (GLuint index, const GLint *v);
static AbstractQoreNode *f_glVertexAttrib4iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLint* v = p;
   glVertexAttrib4iv(index, v);
   return 0;
}
*/

//void glVertexAttrib4s (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
static AbstractQoreNode *f_glVertexAttrib4s(const QoreListNode *params, ExceptionSink *xsink)
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
   glVertexAttrib4s(index, x, y, z, w);
   return 0;
}

/*
//void glVertexAttrib4sv (GLuint index, const GLshort *v);
static AbstractQoreNode *f_glVertexAttrib4sv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLshort* v = p;
   glVertexAttrib4sv(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4ubv (GLuint index, const GLubyte *v);
static AbstractQoreNode *f_glVertexAttrib4ubv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLubyte* v = p;
   glVertexAttrib4ubv(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4uiv (GLuint index, const GLuint *v);
static AbstractQoreNode *f_glVertexAttrib4uiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLuint* v = p;
   glVertexAttrib4uiv(index, v);
   return 0;
}
*/

/*
//void glVertexAttrib4usv (GLuint index, const GLushort *v);
static AbstractQoreNode *f_glVertexAttrib4usv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLushort* v = p;
   glVertexAttrib4usv(index, v);
   return 0;
}
*/

 /*
//void glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
static AbstractQoreNode *f_glVertexAttribPointer(const QoreListNode *params, ExceptionSink *xsink)
{
   glVertexAttribPointer();
   return 0;
}
 */

//void glEnableVertexAttribArray (GLuint index);
static AbstractQoreNode *f_glEnableVertexAttribArray(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   glEnableVertexAttribArray(index);
   return 0;
}

//void glDisableVertexAttribArray (GLuint index);
static AbstractQoreNode *f_glDisableVertexAttribArray(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   glDisableVertexAttribArray(index);
   return 0;
}

/*
//void glGetVertexAttribdv (GLuint index, GLenum pname, GLdouble *params);
static AbstractQoreNode *f_glGetVertexAttribdv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLdouble* params = p;
   glGetVertexAttribdv(index, pname, params);
   return 0;
}
*/

/*
//void glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat *params);
static AbstractQoreNode *f_glGetVertexAttribfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetVertexAttribfv(index, pname, params);
   return 0;
}
*/

/*
//void glGetVertexAttribiv (GLuint index, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetVertexAttribiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetVertexAttribiv(index, pname, params);
   return 0;
}
*/

 /*
//void glGetVertexAttribPointerv (GLuint index, GLenum pname, GLvoid* *pointer);
static AbstractQoreNode *f_glGetVertexAttribPointerv(const QoreListNode *params, ExceptionSink *xsink)
{
   glGetVertexAttribPointerv();
   return 0;
}
 */

//void glDeleteShader (GLuint shader);
static AbstractQoreNode *f_glDeleteShader(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned shader = p ? p->getAsBigInt() : 0;
   glDeleteShader(shader);
   return 0;
}

//void glDetachShader (GLuint program, GLuint shader);
static AbstractQoreNode *f_glDetachShader(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned shader = p ? p->getAsBigInt() : 0;
   glDetachShader(program, shader);
   return 0;
}

//GLuint glCreateShader (GLenum type);
static AbstractQoreNode *f_glCreateShader(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   return new QoreBigIntNode(glCreateShader(type));
}

/*
//void glShaderSource (GLuint shader, GLsizei count, const GLchar* *string, const GLint *length);
static AbstractQoreNode *f_glShaderSource(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned shader = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLchar** string = p;
   p = get_param(params, 3);
   ??? GLint* length = p;
   glShaderSource(shader, count, string, length);
   return 0;
}
*/

//void glCompileShader (GLuint shader);
static AbstractQoreNode *f_glCompileShader(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned shader = p ? p->getAsBigInt() : 0;
   glCompileShader(shader);
   return 0;
}

//GLuint glCreateProgram (void);
static AbstractQoreNode *f_glCreateProgram(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(glCreateProgram());
}

//void glAttachShader (GLuint program, GLuint shader);
static AbstractQoreNode *f_glAttachShader(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned shader = p ? p->getAsBigInt() : 0;
   glAttachShader(program, shader);
   return 0;
}

//void glLinkProgram (GLuint program);
static AbstractQoreNode *f_glLinkProgram(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   glLinkProgram(program);
   return 0;
}

//void glUseProgram (GLuint program);
static AbstractQoreNode *f_glUseProgram(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   glUseProgram(program);
   return 0;
}

//void glDeleteProgram (GLuint program);
static AbstractQoreNode *f_glDeleteProgram(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   glDeleteProgram(program);
   return 0;
}

//void glValidateProgram (GLuint program);
static AbstractQoreNode *f_glValidateProgram(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   glValidateProgram(program);
   return 0;
}

//void glUniform1f (GLint location, GLfloat v0);
static AbstractQoreNode *f_glUniform1f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat v0 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glUniform1f(location, v0);
   return 0;
}

//void glUniform2f (GLint location, GLfloat v0, GLfloat v1);
static AbstractQoreNode *f_glUniform2f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat v0 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat v1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glUniform2f(location, v0, v1);
   return 0;
}

//void glUniform3f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
static AbstractQoreNode *f_glUniform3f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat v0 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat v1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat v2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glUniform3f(location, v0, v1, v2);
   return 0;
}

//void glUniform4f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
static AbstractQoreNode *f_glUniform4f(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLfloat v0 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat v1 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat v2 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 4);
   GLfloat v3 = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glUniform4f(location, v0, v1, v2, v3);
   return 0;
}

//void glUniform1i (GLint location, GLint v0);
static AbstractQoreNode *f_glUniform1i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint v0 = (GLint)(p ? p->getAsInt() : 0);
   glUniform1i(location, v0);
   return 0;
}

//void glUniform2i (GLint location, GLint v0, GLint v1);
static AbstractQoreNode *f_glUniform2i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint v0 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint v1 = (GLint)(p ? p->getAsInt() : 0);
   glUniform2i(location, v0, v1);
   return 0;
}

//void glUniform3i (GLint location, GLint v0, GLint v1, GLint v2);
static AbstractQoreNode *f_glUniform3i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint v0 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint v1 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint v2 = (GLint)(p ? p->getAsInt() : 0);
   glUniform3i(location, v0, v1, v2);
   return 0;
}

//void glUniform4i (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
static AbstractQoreNode *f_glUniform4i(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLint v0 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint v1 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint v2 = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   GLint v3 = (GLint)(p ? p->getAsInt() : 0);
   glUniform4i(location, v0, v1, v2, v3);
   return 0;
}

/*
//void glUniform1fv (GLint location, GLsizei count, const GLfloat *value);
static AbstractQoreNode *f_glUniform1fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* value = p;
   glUniform1fv(location, count, value);
   return 0;
}
*/

/*
//void glUniform2fv (GLint location, GLsizei count, const GLfloat *value);
static AbstractQoreNode *f_glUniform2fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* value = p;
   glUniform2fv(location, count, value);
   return 0;
}
*/

/*
//void glUniform3fv (GLint location, GLsizei count, const GLfloat *value);
static AbstractQoreNode *f_glUniform3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* value = p;
   glUniform3fv(location, count, value);
   return 0;
}
*/

/*
//void glUniform4fv (GLint location, GLsizei count, const GLfloat *value);
static AbstractQoreNode *f_glUniform4fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* value = p;
   glUniform4fv(location, count, value);
   return 0;
}
*/

/*
//void glUniform1iv (GLint location, GLsizei count, const GLint *value);
static AbstractQoreNode *f_glUniform1iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* value = p;
   glUniform1iv(location, count, value);
   return 0;
}
*/

/*
//void glUniform2iv (GLint location, GLsizei count, const GLint *value);
static AbstractQoreNode *f_glUniform2iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* value = p;
   glUniform2iv(location, count, value);
   return 0;
}
*/

/*
//void glUniform3iv (GLint location, GLsizei count, const GLint *value);
static AbstractQoreNode *f_glUniform3iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* value = p;
   glUniform3iv(location, count, value);
   return 0;
}
*/

/*
//void glUniform4iv (GLint location, GLsizei count, const GLint *value);
static AbstractQoreNode *f_glUniform4iv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* value = p;
   glUniform4iv(location, count, value);
   return 0;
}
*/

/*
//void glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static AbstractQoreNode *f_glUniformMatrix2fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean transpose = (GLboolean)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLfloat* value = p;
   glUniformMatrix2fv(location, count, transpose, value);
   return 0;
}
*/

/*
//void glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static AbstractQoreNode *f_glUniformMatrix3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean transpose = (GLboolean)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLfloat* value = p;
   glUniformMatrix3fv(location, count, transpose, value);
   return 0;
}
*/

/*
//void glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static AbstractQoreNode *f_glUniformMatrix4fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean transpose = (GLboolean)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLfloat* value = p;
   glUniformMatrix4fv(location, count, transpose, value);
   return 0;
}
*/

//GLboolean glIsShader (GLuint shader);
static AbstractQoreNode *f_glIsShader(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned shader = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsShader(shader));
}

//GLboolean glIsProgram (GLuint program);
static AbstractQoreNode *f_glIsProgram(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   return get_bool_node(glIsProgram(program));
}

/*
//void glGetShaderiv (GLuint shader, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetShaderiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned shader = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetShaderiv(shader, pname, params);
   return 0;
}
*/

/*
//void glGetProgramiv (GLuint program, GLenum pname, GLint *params);
static AbstractQoreNode *f_glGetProgramiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLenum pname = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetProgramiv(program, pname, params);
   return 0;
}
*/

/*
//void glGetAttachedShaders (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
static AbstractQoreNode *f_glGetAttachedShaders(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLsizei maxCount = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLsizei* count = p;
   p = get_param(params, 3);
   ??? GLuint* shaders = p;
   glGetAttachedShaders(program, maxCount, count, shaders);
   return 0;
}
*/

/*
//void glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
static AbstractQoreNode *f_glGetShaderInfoLog(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned shader = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLsizei bufSize = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLsizei* length = p;
   p = get_param(params, 3);
   ??? GLchar* infoLog = p;
   glGetShaderInfoLog(shader, bufSize, length, infoLog);
   return 0;
}
*/

/*
//void glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
static AbstractQoreNode *f_glGetProgramInfoLog(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLsizei bufSize = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLsizei* length = p;
   p = get_param(params, 3);
   ??? GLchar* infoLog = p;
   glGetProgramInfoLog(program, bufSize, length, infoLog);
   return 0;
}
*/

/*
//GLint glGetUniformLocation (GLuint program, const GLchar *name);
static AbstractQoreNode *f_glGetUniformLocation(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLchar* name = p;
   ??? return new QoreBigIntNode(glGetUniformLocation(program, name));
}
*/

/*
//void glGetActiveUniform (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
static AbstractQoreNode *f_glGetActiveUniform(const QoreListNode *params, ExceptionSink *xsink)
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
   ??? GLint* size = p;
   p = get_param(params, 5);
   ??? GLenum* type = p;
   p = get_param(params, 6);
   ??? GLchar* name = p;
   glGetActiveUniform(program, index, bufSize, length, size, type, name);
   return 0;
}
*/

/*
//void glGetUniformfv (GLuint program, GLint location, GLfloat *params);
static AbstractQoreNode *f_glGetUniformfv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLfloat* params = p;
   glGetUniformfv(program, location, params);
   return 0;
}
*/

/*
//void glGetUniformiv (GLuint program, GLint location, GLint *params);
static AbstractQoreNode *f_glGetUniformiv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLint* params = p;
   glGetUniformiv(program, location, params);
   return 0;
}
*/

/*
//void glGetShaderSource (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
static AbstractQoreNode *f_glGetShaderSource(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned shader = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   GLsizei bufSize = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   ??? GLsizei* length = p;
   p = get_param(params, 3);
   ??? GLchar* source = p;
   glGetShaderSource(shader, bufSize, length, source);
   return 0;
}
*/

/*
//void glBindAttribLocation (GLuint program, GLuint index, const GLchar *name);
static AbstractQoreNode *f_glBindAttribLocation(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   unsigned index = p ? p->getAsBigInt() : 0;
   p = get_param(params, 2);
   ??? GLchar* name = p;
   glBindAttribLocation(program, index, name);
   return 0;
}
*/

/*
//void glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
static AbstractQoreNode *f_glGetActiveAttrib(const QoreListNode *params, ExceptionSink *xsink)
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
   ??? GLint* size = p;
   p = get_param(params, 5);
   ??? GLenum* type = p;
   p = get_param(params, 6);
   ??? GLchar* name = p;
   glGetActiveAttrib(program, index, bufSize, length, size, type, name);
   return 0;
}
*/

/*
//GLint glGetAttribLocation (GLuint program, const GLchar *name);
static AbstractQoreNode *f_glGetAttribLocation(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned program = p ? p->getAsBigInt() : 0;
   p = get_param(params, 1);
   ??? GLchar* name = p;
   ??? return new QoreBigIntNode(glGetAttribLocation(program, name));
}
*/

//void glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask);
static AbstractQoreNode *f_glStencilFuncSeparate(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum face = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum func = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint ref = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   unsigned mask = p ? p->getAsBigInt() : 0;
   glStencilFuncSeparate(face, func, ref, mask);
   return 0;
}

//void glStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
static AbstractQoreNode *f_glStencilOpSeparate(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum face = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum fail = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLenum zfail = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLenum zpass = (GLenum)(p ? p->getAsInt() : 0);
   glStencilOpSeparate(face, fail, zfail, zpass);
   return 0;
}

//void glStencilMaskSeparate (GLenum face, GLuint mask);
static AbstractQoreNode *f_glStencilMaskSeparate(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum face = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   unsigned mask = p ? p->getAsBigInt() : 0;
   glStencilMaskSeparate(face, mask);
   return 0;
}

/*
//void glUniformMatrix2x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static AbstractQoreNode *f_glUniformMatrix2x3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean transpose = (GLboolean)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLfloat* value = p;
   glUniformMatrix2x3fv(location, count, transpose, value);
   return 0;
}
*/

/*
//void glUniformMatrix3x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static AbstractQoreNode *f_glUniformMatrix3x2fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean transpose = (GLboolean)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLfloat* value = p;
   glUniformMatrix3x2fv(location, count, transpose, value);
   return 0;
}
*/

/*
//void glUniformMatrix2x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static AbstractQoreNode *f_glUniformMatrix2x4fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean transpose = (GLboolean)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLfloat* value = p;
   glUniformMatrix2x4fv(location, count, transpose, value);
   return 0;
}
*/

/*
//void glUniformMatrix4x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static AbstractQoreNode *f_glUniformMatrix4x2fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean transpose = (GLboolean)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLfloat* value = p;
   glUniformMatrix4x2fv(location, count, transpose, value);
   return 0;
}
*/

/*
//void glUniformMatrix3x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static AbstractQoreNode *f_glUniformMatrix3x4fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean transpose = (GLboolean)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLfloat* value = p;
   glUniformMatrix3x4fv(location, count, transpose, value);
   return 0;
}
*/

/*
//void glUniformMatrix4x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static AbstractQoreNode *f_glUniformMatrix4x3fv(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLint location = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLsizei count = (GLsizei)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLboolean transpose = (GLboolean)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   ??? GLfloat* value = p;
   glUniformMatrix4x3fv(location, count, transpose, value);
   return 0;
}
*/

static QoreStringNode *opengl_module_init()
{
   builtinFunctions.add("glGenLists",                   f_glGenLists, QDOM_GUI);
   builtinFunctions.add("glDeleteLists",                f_glDeleteLists, QDOM_GUI);
   builtinFunctions.add("glNewList",                    f_glNewList, QDOM_GUI);
   builtinFunctions.add("glEndList",                    f_glEndList, QDOM_GUI);
   builtinFunctions.add("glBegin",                      f_glBegin, QDOM_GUI);
   builtinFunctions.add("glEnd",                        f_glEnd, QDOM_GUI);
   builtinFunctions.add("glGetError",                   f_glGetError, QDOM_GUI);

   builtinFunctions.add("glTexCoord1d",                 f_glTexCoord1d, QDOM_GUI);
   builtinFunctions.add("glTexCoord1f",                 f_glTexCoord1f, QDOM_GUI);
   builtinFunctions.add("glTexCoord1i",                 f_glTexCoord1i, QDOM_GUI);
   builtinFunctions.add("glTexCoord1s",                 f_glTexCoord1s, QDOM_GUI);
   builtinFunctions.add("glTexCoord2d",                 f_glTexCoord2d, QDOM_GUI);
   builtinFunctions.add("glTexCoord2f",                 f_glTexCoord2f, QDOM_GUI);
   builtinFunctions.add("glTexCoord2i",                 f_glTexCoord2i, QDOM_GUI);
   builtinFunctions.add("glTexCoord2s",                 f_glTexCoord2s, QDOM_GUI);
   builtinFunctions.add("glTexCoord3d",                 f_glTexCoord3d, QDOM_GUI);
   builtinFunctions.add("glTexCoord3f",                 f_glTexCoord3f, QDOM_GUI);
   builtinFunctions.add("glTexCoord3i",                 f_glTexCoord3i, QDOM_GUI);
   builtinFunctions.add("glTexCoord3s",                 f_glTexCoord3s, QDOM_GUI);
   builtinFunctions.add("glTexCoord4d",                 f_glTexCoord4d, QDOM_GUI);
   builtinFunctions.add("glTexCoord4f",                 f_glTexCoord4f, QDOM_GUI);
   builtinFunctions.add("glTexCoord4i",                 f_glTexCoord4i, QDOM_GUI);
   builtinFunctions.add("glTexCoord4s",                 f_glTexCoord4s, QDOM_GUI);

   builtinFunctions.add("glVertex2d",                   f_glVertex2d, QDOM_GUI);
   builtinFunctions.add("glVertex2f",                   f_glVertex2f, QDOM_GUI);
   builtinFunctions.add("glVertex2i",                   f_glVertex2i, QDOM_GUI);
   builtinFunctions.add("glVertex2s",                   f_glVertex2s, QDOM_GUI);
   builtinFunctions.add("glVertex3d",                   f_glVertex3d, QDOM_GUI);
   builtinFunctions.add("glVertex3f",                   f_glVertex3f, QDOM_GUI);
   builtinFunctions.add("glVertex3i",                   f_glVertex3i, QDOM_GUI);
   builtinFunctions.add("glVertex3s",                   f_glVertex3s, QDOM_GUI);
   builtinFunctions.add("glVertex4d",                   f_glVertex4d, QDOM_GUI);
   builtinFunctions.add("glVertex4f",                   f_glVertex4f, QDOM_GUI);
   builtinFunctions.add("glVertex4i",                   f_glVertex4i, QDOM_GUI);
   builtinFunctions.add("glVertex4s",                   f_glVertex4s, QDOM_GUI);

   builtinFunctions.add("glClear",                      f_glClear, QDOM_GUI);
   builtinFunctions.add("glClearColor",                 f_glClearColor, QDOM_GUI);
   builtinFunctions.add("glPushAttrib",                 f_glPushAttrib, QDOM_GUI);
   builtinFunctions.add("glPopAttrib",                  f_glPopAttrib, QDOM_GUI);
   builtinFunctions.add("glMatrixMode",                 f_glMatrixMode, QDOM_GUI);
   builtinFunctions.add("glPushMatrix",                 f_glPushMatrix, QDOM_GUI);
   builtinFunctions.add("glPopMatrix",                  f_glPopMatrix, QDOM_GUI);
   builtinFunctions.add("glLoadIdentity",               f_glLoadIdentity, QDOM_GUI);
   builtinFunctions.add("glFrustum",                    f_glFrustum, QDOM_GUI);
   builtinFunctions.add("glTranslated",                 f_glTranslated, QDOM_GUI);
   builtinFunctions.add("glTranslatef",                 f_glTranslatef, QDOM_GUI);
   builtinFunctions.add("glViewport",                   f_glViewport, QDOM_GUI);
   builtinFunctions.add("glEnable",                     f_glEnable, QDOM_GUI);
   builtinFunctions.add("glDisable",                    f_glDisable, QDOM_GUI);
   builtinFunctions.add("glBlendFunc",                  f_glBlendFunc, QDOM_GUI);
   builtinFunctions.add("glBindTexture",                f_glBindTexture, QDOM_GUI);

   builtinFunctions.add("glScaled",                     f_glScaled, QDOM_GUI);
   builtinFunctions.add("glScalef",                     f_glScalef, QDOM_GUI);

   builtinFunctions.add("glColor3b",                    f_glColor3b, QDOM_GUI);
   builtinFunctions.add("glColor3d",                    f_glColor3d, QDOM_GUI);
   builtinFunctions.add("glColor3f",                    f_glColor3f, QDOM_GUI);
   builtinFunctions.add("glColor3i",                    f_glColor3i, QDOM_GUI);
   builtinFunctions.add("glColor3s",                    f_glColor3s, QDOM_GUI);
   builtinFunctions.add("glColor3ub",                   f_glColor3ub, QDOM_GUI);
   builtinFunctions.add("glColor3ui",                   f_glColor3ui, QDOM_GUI);
   builtinFunctions.add("glColor3us",                   f_glColor3us, QDOM_GUI);
   builtinFunctions.add("glColor4b",                    f_glColor4b, QDOM_GUI);
   builtinFunctions.add("glColor4d",                    f_glColor4d, QDOM_GUI);
   builtinFunctions.add("glColor4f",                    f_glColor4f, QDOM_GUI);
   builtinFunctions.add("glColor4i",                    f_glColor4i, QDOM_GUI);
   builtinFunctions.add("glColor4s",                    f_glColor4s, QDOM_GUI);
   builtinFunctions.add("glColor4ub",                   f_glColor4ub, QDOM_GUI);
   builtinFunctions.add("glColor4ui",                   f_glColor4ui, QDOM_GUI);
   builtinFunctions.add("glColor4us",                   f_glColor4us, QDOM_GUI);
   builtinFunctions.add("glCallList",                   f_glCallList, QDOM_GUI);
   builtinFunctions.add("glRotated",                    f_glRotated, QDOM_GUI);
   builtinFunctions.add("glRotatef",                    f_glRotatef, QDOM_GUI);
   builtinFunctions.add("glDepthFunc",                  f_glDepthFunc, QDOM_GUI);

   builtinFunctions.add("glNormal3b",                   f_glNormal3b, QDOM_GUI);
   builtinFunctions.add("glNormal3d",                   f_glNormal3d, QDOM_GUI);
   builtinFunctions.add("glNormal3f",                   f_glNormal3f, QDOM_GUI);
   builtinFunctions.add("glNormal3i",                   f_glNormal3i, QDOM_GUI);
   builtinFunctions.add("glNormal3s",                   f_glNormal3s, QDOM_GUI);

   builtinFunctions.add("glLightf",                     f_glLightf, QDOM_GUI);
   builtinFunctions.add("glLighti",                     f_glLighti, QDOM_GUI);
   builtinFunctions.add("glLightfv",                    f_glLightfv, QDOM_GUI);
   builtinFunctions.add("glLightiv",                    f_glLightiv, QDOM_GUI);

   builtinFunctions.add("glMaterialf",                  f_glMaterialf, QDOM_GUI);
   builtinFunctions.add("glMateriali",                  f_glMateriali, QDOM_GUI);
   builtinFunctions.add("glMaterialfv",                 f_glMaterialfv, QDOM_GUI);
   builtinFunctions.add("glMaterialiv",                 f_glMaterialiv, QDOM_GUI);

   builtinFunctions.add("glShadeModel",                 f_glShadeModel, QDOM_GUI);

   builtinFunctions.add("glOrtho",                      f_glOrtho, QDOM_GUI);
   builtinFunctions.add("glHint",                       f_glHint, QDOM_GUI);

   builtinFunctions.add("glClearDepth",                 f_glClearDepth, QDOM_GUI);
   builtinFunctions.add("glFlush",                      f_glFlush, QDOM_GUI);

   builtinFunctions.add("glLightModelf",                f_glLightModelf, QDOM_GUI);
   builtinFunctions.add("glLightModeli",                f_glLightModeli, QDOM_GUI);
   builtinFunctions.add("glLightModelfv",               f_glLightModelfv, QDOM_GUI);
   builtinFunctions.add("glLightModeliv",               f_glLightModeliv, QDOM_GUI);

   builtinFunctions.add("glGetBooleanv",                f_glGetBooleanv, QDOM_GUI);
   builtinFunctions.add("glGetDoublev",                 f_glGetDoublev, QDOM_GUI);
   builtinFunctions.add("glGetFloatv",                  f_glGetFloatv, QDOM_GUI);
   builtinFunctions.add("glGetIntegerv",                f_glGetIntegerv, QDOM_GUI);

   builtinFunctions.add("glTexEnvf",                    f_glTexEnvf, QDOM_GUI);
   builtinFunctions.add("glTexEnvi",                    f_glTexEnvi, QDOM_GUI);
   builtinFunctions.add("glTexEnvfv",                   f_glTexEnvfv, QDOM_GUI);
   builtinFunctions.add("glTexEnviv",                   f_glTexEnviv, QDOM_GUI);

   builtinFunctions.add("glAccum",                      f_glAccum, QDOM_GUI);
   builtinFunctions.add("glAlphaFunc",                  f_glAlphaFunc, QDOM_GUI);
   //builtinFunctions.add("glAreTexturesResident",        f_glAreTexturesResident, QDOM_GUI);
   builtinFunctions.add("glArrayElement",               f_glArrayElement, QDOM_GUI);
   //builtinFunctions.add("glBitmap",                     f_glBitmap, QDOM_GUI);
   builtinFunctions.add("glBlendColor",                 f_glBlendColor, QDOM_GUI);
   builtinFunctions.add("glBlendEquation",              f_glBlendEquation, QDOM_GUI);
   builtinFunctions.add("glBlendEquationSeparate",      f_glBlendEquationSeparate, QDOM_GUI);
   //builtinFunctions.add("glCallLists",                  f_glCallLists, QDOM_GUI);
   builtinFunctions.add("glClearAccum",                 f_glClearAccum, QDOM_GUI);
   builtinFunctions.add("glClearIndex",                 f_glClearIndex, QDOM_GUI);
   builtinFunctions.add("glClearStencil",               f_glClearStencil, QDOM_GUI);
   //builtinFunctions.add("glClipPlane",                  f_glClipPlane, QDOM_GUI);
   builtinFunctions.add("glColor3bv",                   f_glColor3bv, QDOM_GUI);
   builtinFunctions.add("glColor3dv",                   f_glColor3dv, QDOM_GUI);
   builtinFunctions.add("glColor3fv",                   f_glColor3fv, QDOM_GUI);
   builtinFunctions.add("glColor3iv",                   f_glColor3iv, QDOM_GUI);
   builtinFunctions.add("glColor3sv",                   f_glColor3sv, QDOM_GUI);
   builtinFunctions.add("glColor3ubv",                  f_glColor3ubv, QDOM_GUI);
   builtinFunctions.add("glColor3uiv",                  f_glColor3uiv, QDOM_GUI);
   builtinFunctions.add("glColor3usv",                  f_glColor3usv, QDOM_GUI);
   builtinFunctions.add("glColor4bv",                   f_glColor4bv, QDOM_GUI);
   builtinFunctions.add("glColor4dv",                   f_glColor4dv, QDOM_GUI);
   builtinFunctions.add("glColor4fv",                   f_glColor4fv, QDOM_GUI);
   builtinFunctions.add("glColor4iv",                   f_glColor4iv, QDOM_GUI);
   builtinFunctions.add("glColor4sv",                   f_glColor4sv, QDOM_GUI);
   builtinFunctions.add("glColor4ubv",                  f_glColor4ubv, QDOM_GUI);
   builtinFunctions.add("glColor4uiv",                  f_glColor4uiv, QDOM_GUI);
   builtinFunctions.add("glColor4usv",                  f_glColor4usv, QDOM_GUI);
   builtinFunctions.add("glColorMask",                  f_glColorMask, QDOM_GUI);
   builtinFunctions.add("glColorMaterial",              f_glColorMaterial, QDOM_GUI);
   //builtinFunctions.add("glColorPointer",               f_glColorPointer, QDOM_GUI);
   //builtinFunctions.add("glColorSubTable",              f_glColorSubTable, QDOM_GUI);
   //builtinFunctions.add("glColorTable",                 f_glColorTable, QDOM_GUI);
   //builtinFunctions.add("glColorTableParameterfv",      f_glColorTableParameterfv, QDOM_GUI);
   //builtinFunctions.add("glColorTableParameteriv",      f_glColorTableParameteriv, QDOM_GUI);
   //builtinFunctions.add("glConvolutionFilter1D",        f_glConvolutionFilter1D, QDOM_GUI);
   //builtinFunctions.add("glConvolutionFilter2D",        f_glConvolutionFilter2D, QDOM_GUI);
   builtinFunctions.add("glConvolutionParameterf",      f_glConvolutionParameterf, QDOM_GUI);
   //builtinFunctions.add("glConvolutionParameterfv",     f_glConvolutionParameterfv, QDOM_GUI);
   builtinFunctions.add("glConvolutionParameteri",      f_glConvolutionParameteri, QDOM_GUI);
   //builtinFunctions.add("glConvolutionParameteriv",     f_glConvolutionParameteriv, QDOM_GUI);
   builtinFunctions.add("glCopyColorSubTable",          f_glCopyColorSubTable, QDOM_GUI);
   builtinFunctions.add("glCopyColorTable",             f_glCopyColorTable, QDOM_GUI);
   builtinFunctions.add("glCopyConvolutionFilter1D",    f_glCopyConvolutionFilter1D, QDOM_GUI);
   builtinFunctions.add("glCopyConvolutionFilter2D",    f_glCopyConvolutionFilter2D, QDOM_GUI);
   builtinFunctions.add("glCopyPixels",                 f_glCopyPixels, QDOM_GUI);
   builtinFunctions.add("glCopyTexImage1D",             f_glCopyTexImage1D, QDOM_GUI);
   builtinFunctions.add("glCopyTexImage2D",             f_glCopyTexImage2D, QDOM_GUI);
   builtinFunctions.add("glCopyTexSubImage1D",          f_glCopyTexSubImage1D, QDOM_GUI);
   builtinFunctions.add("glCopyTexSubImage2D",          f_glCopyTexSubImage2D, QDOM_GUI);
   builtinFunctions.add("glCopyTexSubImage3D",          f_glCopyTexSubImage3D, QDOM_GUI);
   builtinFunctions.add("glCullFace",                   f_glCullFace, QDOM_GUI);
   //builtinFunctions.add("glDeleteTextures",             f_glDeleteTextures, QDOM_GUI);
   builtinFunctions.add("glDepthMask",                  f_glDepthMask, QDOM_GUI);
   builtinFunctions.add("glDepthRange",                 f_glDepthRange, QDOM_GUI);
   builtinFunctions.add("glDisableClientState",         f_glDisableClientState, QDOM_GUI);
   builtinFunctions.add("glDrawArrays",                 f_glDrawArrays, QDOM_GUI);
   builtinFunctions.add("glDrawBuffer",                 f_glDrawBuffer, QDOM_GUI);
   //builtinFunctions.add("glDrawElements",               f_glDrawElements, QDOM_GUI);
   //builtinFunctions.add("glDrawPixels",                 f_glDrawPixels, QDOM_GUI);
   //builtinFunctions.add("glDrawRangeElements",          f_glDrawRangeElements, QDOM_GUI);
   builtinFunctions.add("glEdgeFlag",                   f_glEdgeFlag, QDOM_GUI);
   //builtinFunctions.add("glEdgeFlagPointer",            f_glEdgeFlagPointer, QDOM_GUI);
   //builtinFunctions.add("glEdgeFlagv",                  f_glEdgeFlagv, QDOM_GUI);
   builtinFunctions.add("glEnableClientState",          f_glEnableClientState, QDOM_GUI);
   builtinFunctions.add("glEvalCoord1d",                f_glEvalCoord1d, QDOM_GUI);
   //builtinFunctions.add("glEvalCoord1dv",               f_glEvalCoord1dv, QDOM_GUI);
   builtinFunctions.add("glEvalCoord1f",                f_glEvalCoord1f, QDOM_GUI);
   //builtinFunctions.add("glEvalCoord1fv",               f_glEvalCoord1fv, QDOM_GUI);
   builtinFunctions.add("glEvalCoord2d",                f_glEvalCoord2d, QDOM_GUI);
   //builtinFunctions.add("glEvalCoord2dv",               f_glEvalCoord2dv, QDOM_GUI);
   builtinFunctions.add("glEvalCoord2f",                f_glEvalCoord2f, QDOM_GUI);
   //builtinFunctions.add("glEvalCoord2fv",               f_glEvalCoord2fv, QDOM_GUI);
   builtinFunctions.add("glEvalMesh1",                  f_glEvalMesh1, QDOM_GUI);
   builtinFunctions.add("glEvalMesh2",                  f_glEvalMesh2, QDOM_GUI);
   builtinFunctions.add("glEvalPoint1",                 f_glEvalPoint1, QDOM_GUI);
   builtinFunctions.add("glEvalPoint2",                 f_glEvalPoint2, QDOM_GUI);
   //builtinFunctions.add("glFeedbackBuffer",             f_glFeedbackBuffer, QDOM_GUI);
   builtinFunctions.add("glFinish",                     f_glFinish, QDOM_GUI);
   builtinFunctions.add("glFogf",                       f_glFogf, QDOM_GUI);
   builtinFunctions.add("glFogfv",                      f_glFogfv, QDOM_GUI);
   builtinFunctions.add("glFogi",                       f_glFogi, QDOM_GUI);
   builtinFunctions.add("glFogiv",                      f_glFogiv, QDOM_GUI);
   builtinFunctions.add("glFrontFace",                  f_glFrontFace, QDOM_GUI);
   builtinFunctions.add("glGenTextures",                f_glGenTextures, QDOM_GUI);
   //builtinFunctions.add("glGetClipPlane",               f_glGetClipPlane, QDOM_GUI);
   //builtinFunctions.add("glGetColorTable",              f_glGetColorTable, QDOM_GUI);
   //builtinFunctions.add("glGetColorTableParameterfv",   f_glGetColorTableParameterfv, QDOM_GUI);
   //builtinFunctions.add("glGetColorTableParameteriv",   f_glGetColorTableParameteriv, QDOM_GUI);
   //builtinFunctions.add("glGetConvolutionFilter",       f_glGetConvolutionFilter, QDOM_GUI);
   //builtinFunctions.add("glGetConvolutionParameterfv",  f_glGetConvolutionParameterfv, QDOM_GUI);
   //builtinFunctions.add("glGetConvolutionParameteriv",  f_glGetConvolutionParameteriv, QDOM_GUI);
   //builtinFunctions.add("glGetHistogram",               f_glGetHistogram, QDOM_GUI);
   //builtinFunctions.add("glGetHistogramParameterfv",    f_glGetHistogramParameterfv, QDOM_GUI);
   //builtinFunctions.add("glGetHistogramParameteriv",    f_glGetHistogramParameteriv, QDOM_GUI);
   //builtinFunctions.add("glGetLightfv",                 f_glGetLightfv, QDOM_GUI);
   //builtinFunctions.add("glGetLightiv",                 f_glGetLightiv, QDOM_GUI);
   //builtinFunctions.add("glGetMapdv",                   f_glGetMapdv, QDOM_GUI);
   //builtinFunctions.add("glGetMapfv",                   f_glGetMapfv, QDOM_GUI);
   //builtinFunctions.add("glGetMapiv",                   f_glGetMapiv, QDOM_GUI);
   //builtinFunctions.add("glGetMaterialfv",              f_glGetMaterialfv, QDOM_GUI);
   //builtinFunctions.add("glGetMaterialiv",              f_glGetMaterialiv, QDOM_GUI);
   //builtinFunctions.add("glGetMinmax",                  f_glGetMinmax, QDOM_GUI);
   //builtinFunctions.add("glGetMinmaxParameterfv",       f_glGetMinmaxParameterfv, QDOM_GUI);
   //builtinFunctions.add("glGetMinmaxParameteriv",       f_glGetMinmaxParameteriv, QDOM_GUI);
   //builtinFunctions.add("glGetPixelMapfv",              f_glGetPixelMapfv, QDOM_GUI);
   //builtinFunctions.add("glGetPixelMapuiv",             f_glGetPixelMapuiv, QDOM_GUI);
   //builtinFunctions.add("glGetPixelMapusv",             f_glGetPixelMapusv, QDOM_GUI);
   //builtinFunctions.add("glGetPointerv",                f_glGetPointerv, QDOM_GUI);
   //builtinFunctions.add("glGetPolygonStipple",          f_glGetPolygonStipple, QDOM_GUI);
   //builtinFunctions.add("glGetSeparableFilter",         f_glGetSeparableFilter, QDOM_GUI);
   builtinFunctions.add("glGetString",                  f_glGetString, QDOM_GUI);
   //builtinFunctions.add("glGetTexEnvfv",                f_glGetTexEnvfv, QDOM_GUI);
   //builtinFunctions.add("glGetTexEnviv",                f_glGetTexEnviv, QDOM_GUI);
   //builtinFunctions.add("glGetTexGendv",                f_glGetTexGendv, QDOM_GUI);
   //builtinFunctions.add("glGetTexGenfv",                f_glGetTexGenfv, QDOM_GUI);
   //builtinFunctions.add("glGetTexGeniv",                f_glGetTexGeniv, QDOM_GUI);
   //builtinFunctions.add("glGetTexImage",                f_glGetTexImage, QDOM_GUI);
   builtinFunctions.add("glGetTexLevelParameterfv",     f_glGetTexLevelParameterfv, QDOM_GUI);
   builtinFunctions.add("glGetTexLevelParameteriv",     f_glGetTexLevelParameteriv, QDOM_GUI);
   //builtinFunctions.add("glGetTexParameterfv",          f_glGetTexParameterfv, QDOM_GUI);
   //builtinFunctions.add("glGetTexParameteriv",          f_glGetTexParameteriv, QDOM_GUI);
   builtinFunctions.add("glHistogram",                  f_glHistogram, QDOM_GUI);
   builtinFunctions.add("glIndexMask",                  f_glIndexMask, QDOM_GUI);
   //builtinFunctions.add("glIndexPointer",               f_glIndexPointer, QDOM_GUI);
   builtinFunctions.add("glIndexd",                     f_glIndexd, QDOM_GUI);
   //builtinFunctions.add("glIndexdv",                    f_glIndexdv, QDOM_GUI);
   builtinFunctions.add("glIndexf",                     f_glIndexf, QDOM_GUI);
   //builtinFunctions.add("glIndexfv",                    f_glIndexfv, QDOM_GUI);
   builtinFunctions.add("glIndexi",                     f_glIndexi, QDOM_GUI);
   //builtinFunctions.add("glIndexiv",                    f_glIndexiv, QDOM_GUI);
   builtinFunctions.add("glIndexs",                     f_glIndexs, QDOM_GUI);
   //builtinFunctions.add("glIndexsv",                    f_glIndexsv, QDOM_GUI);
   builtinFunctions.add("glIndexub",                    f_glIndexub, QDOM_GUI);
   //builtinFunctions.add("glIndexubv",                   f_glIndexubv, QDOM_GUI);
   builtinFunctions.add("glInitNames",                  f_glInitNames, QDOM_GUI);
   //builtinFunctions.add("glInterleavedArrays",          f_glInterleavedArrays, QDOM_GUI);
   builtinFunctions.add("glIsEnabled",                  f_glIsEnabled, QDOM_GUI);
   builtinFunctions.add("glIsList",                     f_glIsList, QDOM_GUI);
   builtinFunctions.add("glIsTexture",                  f_glIsTexture, QDOM_GUI);
   builtinFunctions.add("glLineStipple",                f_glLineStipple, QDOM_GUI);
   builtinFunctions.add("glLineWidth",                  f_glLineWidth, QDOM_GUI);
   builtinFunctions.add("glListBase",                   f_glListBase, QDOM_GUI);
   builtinFunctions.add("glLoadMatrixd",                f_glLoadMatrixd, QDOM_GUI);
   builtinFunctions.add("glLoadMatrixf",                f_glLoadMatrixf, QDOM_GUI);
   builtinFunctions.add("glLoadName",                   f_glLoadName, QDOM_GUI);
   builtinFunctions.add("glLogicOp",                    f_glLogicOp, QDOM_GUI);
   //builtinFunctions.add("glMap1d",                      f_glMap1d, QDOM_GUI);
   //builtinFunctions.add("glMap1f",                      f_glMap1f, QDOM_GUI);
   //builtinFunctions.add("glMap2d",                      f_glMap2d, QDOM_GUI);
   //builtinFunctions.add("glMap2f",                      f_glMap2f, QDOM_GUI);
   builtinFunctions.add("glMapGrid1d",                  f_glMapGrid1d, QDOM_GUI);
   builtinFunctions.add("glMapGrid1f",                  f_glMapGrid1f, QDOM_GUI);
   builtinFunctions.add("glMapGrid2d",                  f_glMapGrid2d, QDOM_GUI);
   builtinFunctions.add("glMapGrid2f",                  f_glMapGrid2f, QDOM_GUI);
   builtinFunctions.add("glMinmax",                     f_glMinmax, QDOM_GUI);
   builtinFunctions.add("glMultMatrixd",                f_glMultMatrixd, QDOM_GUI);
   builtinFunctions.add("glMultMatrixf",                f_glMultMatrixf, QDOM_GUI);
   builtinFunctions.add("glNormal3bv",                  f_glNormal3bv, QDOM_GUI);
   builtinFunctions.add("glNormal3dv",                  f_glNormal3dv, QDOM_GUI);
   builtinFunctions.add("glNormal3fv",                  f_glNormal3fv, QDOM_GUI);
   builtinFunctions.add("glNormal3iv",                  f_glNormal3iv, QDOM_GUI);
   builtinFunctions.add("glNormal3sv",                  f_glNormal3sv, QDOM_GUI);
   //builtinFunctions.add("glNormalPointer",              f_glNormalPointer, QDOM_GUI);
   builtinFunctions.add("glPassThrough",                f_glPassThrough, QDOM_GUI);
   //builtinFunctions.add("glPixelMapfv",                 f_glPixelMapfv, QDOM_GUI);
   //builtinFunctions.add("glPixelMapuiv",                f_glPixelMapuiv, QDOM_GUI);
   //builtinFunctions.add("glPixelMapusv",                f_glPixelMapusv, QDOM_GUI);
   builtinFunctions.add("glPixelStoref",                f_glPixelStoref, QDOM_GUI);
   builtinFunctions.add("glPixelStorei",                f_glPixelStorei, QDOM_GUI);
   builtinFunctions.add("glPixelTransferf",             f_glPixelTransferf, QDOM_GUI);
   builtinFunctions.add("glPixelTransferi",             f_glPixelTransferi, QDOM_GUI);
   builtinFunctions.add("glPixelZoom",                  f_glPixelZoom, QDOM_GUI);
   builtinFunctions.add("glPointSize",                  f_glPointSize, QDOM_GUI);
   builtinFunctions.add("glPolygonMode",                f_glPolygonMode, QDOM_GUI);
   builtinFunctions.add("glPolygonOffset",              f_glPolygonOffset, QDOM_GUI);
   //builtinFunctions.add("glPolygonStipple",             f_glPolygonStipple, QDOM_GUI);
   builtinFunctions.add("glPopClientAttrib",            f_glPopClientAttrib, QDOM_GUI);
   builtinFunctions.add("glPopName",                    f_glPopName, QDOM_GUI);
   //builtinFunctions.add("glPrioritizeTextures",         f_glPrioritizeTextures, QDOM_GUI);
   builtinFunctions.add("glPushClientAttrib",           f_glPushClientAttrib, QDOM_GUI);
   builtinFunctions.add("glPushName",                   f_glPushName, QDOM_GUI);
   builtinFunctions.add("glRasterPos2d",                f_glRasterPos2d, QDOM_GUI);
   //builtinFunctions.add("glRasterPos2dv",               f_glRasterPos2dv, QDOM_GUI);
   builtinFunctions.add("glRasterPos2f",                f_glRasterPos2f, QDOM_GUI);
   //builtinFunctions.add("glRasterPos2fv",               f_glRasterPos2fv, QDOM_GUI);
   builtinFunctions.add("glRasterPos2i",                f_glRasterPos2i, QDOM_GUI);
   //builtinFunctions.add("glRasterPos2iv",               f_glRasterPos2iv, QDOM_GUI);
   builtinFunctions.add("glRasterPos2s",                f_glRasterPos2s, QDOM_GUI);
   //builtinFunctions.add("glRasterPos2sv",               f_glRasterPos2sv, QDOM_GUI);
   builtinFunctions.add("glRasterPos3d",                f_glRasterPos3d, QDOM_GUI);
   //builtinFunctions.add("glRasterPos3dv",               f_glRasterPos3dv, QDOM_GUI);
   builtinFunctions.add("glRasterPos3f",                f_glRasterPos3f, QDOM_GUI);
   //builtinFunctions.add("glRasterPos3fv",               f_glRasterPos3fv, QDOM_GUI);
   builtinFunctions.add("glRasterPos3i",                f_glRasterPos3i, QDOM_GUI);
   //builtinFunctions.add("glRasterPos3iv",               f_glRasterPos3iv, QDOM_GUI);
   builtinFunctions.add("glRasterPos3s",                f_glRasterPos3s, QDOM_GUI);
   //builtinFunctions.add("glRasterPos3sv",               f_glRasterPos3sv, QDOM_GUI);
   builtinFunctions.add("glRasterPos4d",                f_glRasterPos4d, QDOM_GUI);
   //builtinFunctions.add("glRasterPos4dv",               f_glRasterPos4dv, QDOM_GUI);
   builtinFunctions.add("glRasterPos4f",                f_glRasterPos4f, QDOM_GUI);
   //builtinFunctions.add("glRasterPos4fv",               f_glRasterPos4fv, QDOM_GUI);
   builtinFunctions.add("glRasterPos4i",                f_glRasterPos4i, QDOM_GUI);
   //builtinFunctions.add("glRasterPos4iv",               f_glRasterPos4iv, QDOM_GUI);
   builtinFunctions.add("glRasterPos4s",                f_glRasterPos4s, QDOM_GUI);
   //builtinFunctions.add("glRasterPos4sv",               f_glRasterPos4sv, QDOM_GUI);
   builtinFunctions.add("glReadBuffer",                 f_glReadBuffer, QDOM_GUI);
   builtinFunctions.add("glReadPixels",                 f_glReadPixels, QDOM_GUI);
   builtinFunctions.add("glRectd",                      f_glRectd, QDOM_GUI);
   //builtinFunctions.add("glRectdv",                     f_glRectdv, QDOM_GUI);
   builtinFunctions.add("glRectf",                      f_glRectf, QDOM_GUI);
   //builtinFunctions.add("glRectfv",                     f_glRectfv, QDOM_GUI);
   builtinFunctions.add("glRecti",                      f_glRecti, QDOM_GUI);
   //builtinFunctions.add("glRectiv",                     f_glRectiv, QDOM_GUI);
   builtinFunctions.add("glRects",                      f_glRects, QDOM_GUI);
   //builtinFunctions.add("glRectsv",                     f_glRectsv, QDOM_GUI);
   //builtinFunctions.add("glRenderMode",                 f_glRenderMode, QDOM_GUI);
   builtinFunctions.add("glResetHistogram",             f_glResetHistogram, QDOM_GUI);
   builtinFunctions.add("glResetMinmax",                f_glResetMinmax, QDOM_GUI);
   builtinFunctions.add("glScissor",                    f_glScissor, QDOM_GUI);
   //builtinFunctions.add("glSelectBuffer",               f_glSelectBuffer, QDOM_GUI);
   //builtinFunctions.add("glSeparableFilter2D",          f_glSeparableFilter2D, QDOM_GUI);
   builtinFunctions.add("glStencilFunc",                f_glStencilFunc, QDOM_GUI);
   builtinFunctions.add("glStencilMask",                f_glStencilMask, QDOM_GUI);
   builtinFunctions.add("glStencilOp",                  f_glStencilOp, QDOM_GUI);
   //builtinFunctions.add("glTexCoord1dv",                f_glTexCoord1dv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord1fv",                f_glTexCoord1fv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord1iv",                f_glTexCoord1iv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord1sv",                f_glTexCoord1sv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord2dv",                f_glTexCoord2dv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord2fv",                f_glTexCoord2fv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord2iv",                f_glTexCoord2iv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord2sv",                f_glTexCoord2sv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord3dv",                f_glTexCoord3dv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord3fv",                f_glTexCoord3fv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord3iv",                f_glTexCoord3iv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord3sv",                f_glTexCoord3sv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord4dv",                f_glTexCoord4dv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord4fv",                f_glTexCoord4fv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord4iv",                f_glTexCoord4iv, QDOM_GUI);
   //builtinFunctions.add("glTexCoord4sv",                f_glTexCoord4sv, QDOM_GUI);
   //builtinFunctions.add("glTexCoordPointer",            f_glTexCoordPointer, QDOM_GUI);
   builtinFunctions.add("glTexGend",                    f_glTexGend, QDOM_GUI);
   builtinFunctions.add("glTexGendv",                   f_glTexGendv, QDOM_GUI);
   builtinFunctions.add("glTexGenf",                    f_glTexGenf, QDOM_GUI);
   builtinFunctions.add("glTexGenfv",                   f_glTexGenfv, QDOM_GUI);
   builtinFunctions.add("glTexGeni",                    f_glTexGeni, QDOM_GUI);
   builtinFunctions.add("glTexGeniv",                   f_glTexGeniv, QDOM_GUI);
   builtinFunctions.add("glTexImage1D",                 f_glTexImage1D, QDOM_GUI);
   builtinFunctions.add("glTexImage2D",                 f_glTexImage2D, QDOM_GUI);
   builtinFunctions.add("glTexImage3D",                 f_glTexImage3D, QDOM_GUI);
   builtinFunctions.add("glTexParameterf",              f_glTexParameterf, QDOM_GUI);
   builtinFunctions.add("glTexParameterfv",             f_glTexParameterfv, QDOM_GUI);
   builtinFunctions.add("glTexParameteri",              f_glTexParameteri, QDOM_GUI);
   builtinFunctions.add("glTexParameteriv",             f_glTexParameteriv, QDOM_GUI);
   //builtinFunctions.add("glTexSubImage1D",              f_glTexSubImage1D, QDOM_GUI);
   //builtinFunctions.add("glTexSubImage2D",              f_glTexSubImage2D, QDOM_GUI);
   //builtinFunctions.add("glTexSubImage3D",              f_glTexSubImage3D, QDOM_GUI);
   builtinFunctions.add("glVertex2dv",                  f_glVertex2dv, QDOM_GUI);
   builtinFunctions.add("glVertex2fv",                  f_glVertex2fv, QDOM_GUI);
   builtinFunctions.add("glVertex2iv",                  f_glVertex2iv, QDOM_GUI);
   builtinFunctions.add("glVertex2sv",                  f_glVertex2sv, QDOM_GUI);
   builtinFunctions.add("glVertex3dv",                  f_glVertex3dv, QDOM_GUI);
   builtinFunctions.add("glVertex3fv",                  f_glVertex3fv, QDOM_GUI);
   builtinFunctions.add("glVertex3iv",                  f_glVertex3iv, QDOM_GUI);
   builtinFunctions.add("glVertex3sv",                  f_glVertex3sv, QDOM_GUI);
   builtinFunctions.add("glVertex4dv",                  f_glVertex4dv, QDOM_GUI);
   builtinFunctions.add("glVertex4fv",                  f_glVertex4fv, QDOM_GUI);
   builtinFunctions.add("glVertex4iv",                  f_glVertex4iv, QDOM_GUI);
   builtinFunctions.add("glVertex4sv",                  f_glVertex4sv, QDOM_GUI);
   //builtinFunctions.add("glVertexPointer",              f_glVertexPointer, QDOM_GUI);
   builtinFunctions.add("glSampleCoverage",             f_glSampleCoverage, QDOM_GUI);
#ifdef HAVE_GLSAMPLEPASS
   builtinFunctions.add("glSamplePass",                 f_glSamplePass, QDOM_GUI);
#endif
   //builtinFunctions.add("glLoadTransposeMatrixf",       f_glLoadTransposeMatrixf, QDOM_GUI);
   //builtinFunctions.add("glLoadTransposeMatrixd",       f_glLoadTransposeMatrixd, QDOM_GUI);
   //builtinFunctions.add("glMultTransposeMatrixf",       f_glMultTransposeMatrixf, QDOM_GUI);
   //builtinFunctions.add("glMultTransposeMatrixd",       f_glMultTransposeMatrixd, QDOM_GUI);
   //builtinFunctions.add("glCompressedTexImage3D",       f_glCompressedTexImage3D, QDOM_GUI);
   //builtinFunctions.add("glCompressedTexImage2D",       f_glCompressedTexImage2D, QDOM_GUI);
   //builtinFunctions.add("glCompressedTexImage1D",       f_glCompressedTexImage1D, QDOM_GUI);
   //builtinFunctions.add("glCompressedTexSubImage3D",    f_glCompressedTexSubImage3D, QDOM_GUI);
   //builtinFunctions.add("glCompressedTexSubImage2D",    f_glCompressedTexSubImage2D, QDOM_GUI);
   //builtinFunctions.add("glCompressedTexSubImage1D",    f_glCompressedTexSubImage1D, QDOM_GUI);
   //builtinFunctions.add("glGetCompressedTexImage",      f_glGetCompressedTexImage, QDOM_GUI);
   builtinFunctions.add("glActiveTexture",              f_glActiveTexture, QDOM_GUI);
   builtinFunctions.add("glClientActiveTexture",        f_glClientActiveTexture, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord1d",            f_glMultiTexCoord1d, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord1dv",           f_glMultiTexCoord1dv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord1f",            f_glMultiTexCoord1f, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord1fv",           f_glMultiTexCoord1fv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord1i",            f_glMultiTexCoord1i, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord1iv",           f_glMultiTexCoord1iv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord1s",            f_glMultiTexCoord1s, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord1sv",           f_glMultiTexCoord1sv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord2d",            f_glMultiTexCoord2d, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord2dv",           f_glMultiTexCoord2dv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord2f",            f_glMultiTexCoord2f, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord2fv",           f_glMultiTexCoord2fv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord2i",            f_glMultiTexCoord2i, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord2iv",           f_glMultiTexCoord2iv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord2s",            f_glMultiTexCoord2s, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord2sv",           f_glMultiTexCoord2sv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord3d",            f_glMultiTexCoord3d, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord3dv",           f_glMultiTexCoord3dv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord3f",            f_glMultiTexCoord3f, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord3fv",           f_glMultiTexCoord3fv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord3i",            f_glMultiTexCoord3i, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord3iv",           f_glMultiTexCoord3iv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord3s",            f_glMultiTexCoord3s, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord3sv",           f_glMultiTexCoord3sv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord4d",            f_glMultiTexCoord4d, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord4dv",           f_glMultiTexCoord4dv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord4f",            f_glMultiTexCoord4f, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord4fv",           f_glMultiTexCoord4fv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord4i",            f_glMultiTexCoord4i, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord4iv",           f_glMultiTexCoord4iv, QDOM_GUI);
   builtinFunctions.add("glMultiTexCoord4s",            f_glMultiTexCoord4s, QDOM_GUI);
   //builtinFunctions.add("glMultiTexCoord4sv",           f_glMultiTexCoord4sv, QDOM_GUI);
   builtinFunctions.add("glFogCoordf",                  f_glFogCoordf, QDOM_GUI);
   //builtinFunctions.add("glFogCoordfv",                 f_glFogCoordfv, QDOM_GUI);
   builtinFunctions.add("glFogCoordd",                  f_glFogCoordd, QDOM_GUI);
   //builtinFunctions.add("glFogCoorddv",                 f_glFogCoorddv, QDOM_GUI);
   //builtinFunctions.add("glFogCoordPointer",            f_glFogCoordPointer, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3b",           f_glSecondaryColor3b, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3bv",          f_glSecondaryColor3bv, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3d",           f_glSecondaryColor3d, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3dv",          f_glSecondaryColor3dv, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3f",           f_glSecondaryColor3f, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3fv",          f_glSecondaryColor3fv, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3i",           f_glSecondaryColor3i, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3iv",          f_glSecondaryColor3iv, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3s",           f_glSecondaryColor3s, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3sv",          f_glSecondaryColor3sv, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3ub",          f_glSecondaryColor3ub, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3ubv",         f_glSecondaryColor3ubv, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3ui",          f_glSecondaryColor3ui, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3uiv",         f_glSecondaryColor3uiv, QDOM_GUI);
   builtinFunctions.add("glSecondaryColor3us",          f_glSecondaryColor3us, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColor3usv",         f_glSecondaryColor3usv, QDOM_GUI);
   //builtinFunctions.add("glSecondaryColorPointer",      f_glSecondaryColorPointer, QDOM_GUI);
   builtinFunctions.add("glPointParameterf",            f_glPointParameterf, QDOM_GUI);
   //builtinFunctions.add("glPointParameterfv",           f_glPointParameterfv, QDOM_GUI);
   builtinFunctions.add("glPointParameteri",            f_glPointParameteri, QDOM_GUI);
   //builtinFunctions.add("glPointParameteriv",           f_glPointParameteriv, QDOM_GUI);
   builtinFunctions.add("glBlendFuncSeparate",          f_glBlendFuncSeparate, QDOM_GUI);
   //builtinFunctions.add("glMultiDrawArrays",            f_glMultiDrawArrays, QDOM_GUI);
   //builtinFunctions.add("glMultiDrawElements",          f_glMultiDrawElements, QDOM_GUI);
   builtinFunctions.add("glWindowPos2d",                f_glWindowPos2d, QDOM_GUI);
   //builtinFunctions.add("glWindowPos2dv",               f_glWindowPos2dv, QDOM_GUI);
   builtinFunctions.add("glWindowPos2f",                f_glWindowPos2f, QDOM_GUI);
   //builtinFunctions.add("glWindowPos2fv",               f_glWindowPos2fv, QDOM_GUI);
   builtinFunctions.add("glWindowPos2i",                f_glWindowPos2i, QDOM_GUI);
   //builtinFunctions.add("glWindowPos2iv",               f_glWindowPos2iv, QDOM_GUI);
   builtinFunctions.add("glWindowPos2s",                f_glWindowPos2s, QDOM_GUI);
   //builtinFunctions.add("glWindowPos2sv",               f_glWindowPos2sv, QDOM_GUI);
   builtinFunctions.add("glWindowPos3d",                f_glWindowPos3d, QDOM_GUI);
   //builtinFunctions.add("glWindowPos3dv",               f_glWindowPos3dv, QDOM_GUI);
   builtinFunctions.add("glWindowPos3f",                f_glWindowPos3f, QDOM_GUI);
   //builtinFunctions.add("glWindowPos3fv",               f_glWindowPos3fv, QDOM_GUI);
   builtinFunctions.add("glWindowPos3i",                f_glWindowPos3i, QDOM_GUI);
   //builtinFunctions.add("glWindowPos3iv",               f_glWindowPos3iv, QDOM_GUI);
   builtinFunctions.add("glWindowPos3s",                f_glWindowPos3s, QDOM_GUI);
   //builtinFunctions.add("glWindowPos3sv",               f_glWindowPos3sv, QDOM_GUI);
   //builtinFunctions.add("glGenQueries",                 f_glGenQueries, QDOM_GUI);
   //builtinFunctions.add("glDeleteQueries",              f_glDeleteQueries, QDOM_GUI);
   builtinFunctions.add("glIsQuery",                    f_glIsQuery, QDOM_GUI);
   builtinFunctions.add("glBeginQuery",                 f_glBeginQuery, QDOM_GUI);
   builtinFunctions.add("glEndQuery",                   f_glEndQuery, QDOM_GUI);
   //builtinFunctions.add("glGetQueryiv",                 f_glGetQueryiv, QDOM_GUI);
   //builtinFunctions.add("glGetQueryObjectiv",           f_glGetQueryObjectiv, QDOM_GUI);
   //builtinFunctions.add("glGetQueryObjectuiv",          f_glGetQueryObjectuiv, QDOM_GUI);
   builtinFunctions.add("glBindBuffer",                 f_glBindBuffer, QDOM_GUI);
   //builtinFunctions.add("glDeleteBuffers",              f_glDeleteBuffers, QDOM_GUI);
   //builtinFunctions.add("glGenBuffers",                 f_glGenBuffers, QDOM_GUI);
   builtinFunctions.add("glIsBuffer",                   f_glIsBuffer, QDOM_GUI);
   //builtinFunctions.add("glBufferData",                 f_glBufferData, QDOM_GUI);
   //builtinFunctions.add("glBufferSubData",              f_glBufferSubData, QDOM_GUI);
   //builtinFunctions.add("glGetBufferSubData",           f_glGetBufferSubData, QDOM_GUI);
   //builtinFunctions.add("glMapBuffer",                  f_glMapBuffer, QDOM_GUI);
   //builtinFunctions.add("glUnmapBuffer",                f_glUnmapBuffer, QDOM_GUI);
   //builtinFunctions.add("glGetBufferParameteriv",       f_glGetBufferParameteriv, QDOM_GUI);
   //builtinFunctions.add("glGetBufferPointerv",          f_glGetBufferPointerv, QDOM_GUI);
   //builtinFunctions.add("glDrawBuffers",                f_glDrawBuffers, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib1d",             f_glVertexAttrib1d, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib1dv",            f_glVertexAttrib1dv, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib1f",             f_glVertexAttrib1f, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib1fv",            f_glVertexAttrib1fv, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib1s",             f_glVertexAttrib1s, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib1sv",            f_glVertexAttrib1sv, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib2d",             f_glVertexAttrib2d, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib2dv",            f_glVertexAttrib2dv, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib2f",             f_glVertexAttrib2f, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib2fv",            f_glVertexAttrib2fv, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib2s",             f_glVertexAttrib2s, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib2sv",            f_glVertexAttrib2sv, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib3d",             f_glVertexAttrib3d, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib3dv",            f_glVertexAttrib3dv, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib3f",             f_glVertexAttrib3f, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib3fv",            f_glVertexAttrib3fv, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib3s",             f_glVertexAttrib3s, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib3sv",            f_glVertexAttrib3sv, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4Nbv",           f_glVertexAttrib4Nbv, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4Niv",           f_glVertexAttrib4Niv, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4Nsv",           f_glVertexAttrib4Nsv, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib4Nub",           f_glVertexAttrib4Nub, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4Nubv",          f_glVertexAttrib4Nubv, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4Nuiv",          f_glVertexAttrib4Nuiv, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4Nusv",          f_glVertexAttrib4Nusv, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4bv",            f_glVertexAttrib4bv, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib4d",             f_glVertexAttrib4d, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4dv",            f_glVertexAttrib4dv, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib4f",             f_glVertexAttrib4f, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4fv",            f_glVertexAttrib4fv, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4iv",            f_glVertexAttrib4iv, QDOM_GUI);
   builtinFunctions.add("glVertexAttrib4s",             f_glVertexAttrib4s, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4sv",            f_glVertexAttrib4sv, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4ubv",           f_glVertexAttrib4ubv, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4uiv",           f_glVertexAttrib4uiv, QDOM_GUI);
   //builtinFunctions.add("glVertexAttrib4usv",           f_glVertexAttrib4usv, QDOM_GUI);
   //builtinFunctions.add("glVertexAttribPointer",        f_glVertexAttribPointer, QDOM_GUI);
   builtinFunctions.add("glEnableVertexAttribArray",    f_glEnableVertexAttribArray, QDOM_GUI);
   builtinFunctions.add("glDisableVertexAttribArray",   f_glDisableVertexAttribArray, QDOM_GUI);
   //builtinFunctions.add("glGetVertexAttribdv",          f_glGetVertexAttribdv, QDOM_GUI);
   //builtinFunctions.add("glGetVertexAttribfv",          f_glGetVertexAttribfv, QDOM_GUI);
   //builtinFunctions.add("glGetVertexAttribiv",          f_glGetVertexAttribiv, QDOM_GUI);
   //builtinFunctions.add("glGetVertexAttribPointerv",    f_glGetVertexAttribPointerv, QDOM_GUI);
   builtinFunctions.add("glDeleteShader",               f_glDeleteShader, QDOM_GUI);
   builtinFunctions.add("glDetachShader",               f_glDetachShader, QDOM_GUI);
   builtinFunctions.add("glCreateShader",               f_glCreateShader, QDOM_GUI);
   //builtinFunctions.add("glShaderSource",               f_glShaderSource, QDOM_GUI);
   builtinFunctions.add("glCompileShader",              f_glCompileShader, QDOM_GUI);
   builtinFunctions.add("glCreateProgram",              f_glCreateProgram, QDOM_GUI);
   builtinFunctions.add("glAttachShader",               f_glAttachShader, QDOM_GUI);
   builtinFunctions.add("glLinkProgram",                f_glLinkProgram, QDOM_GUI);
   builtinFunctions.add("glUseProgram",                 f_glUseProgram, QDOM_GUI);
   builtinFunctions.add("glDeleteProgram",              f_glDeleteProgram, QDOM_GUI);
   builtinFunctions.add("glValidateProgram",            f_glValidateProgram, QDOM_GUI);
   builtinFunctions.add("glUniform1f",                  f_glUniform1f, QDOM_GUI);
   builtinFunctions.add("glUniform2f",                  f_glUniform2f, QDOM_GUI);
   builtinFunctions.add("glUniform3f",                  f_glUniform3f, QDOM_GUI);
   builtinFunctions.add("glUniform4f",                  f_glUniform4f, QDOM_GUI);
   builtinFunctions.add("glUniform1i",                  f_glUniform1i, QDOM_GUI);
   builtinFunctions.add("glUniform2i",                  f_glUniform2i, QDOM_GUI);
   builtinFunctions.add("glUniform3i",                  f_glUniform3i, QDOM_GUI);
   builtinFunctions.add("glUniform4i",                  f_glUniform4i, QDOM_GUI);
   //builtinFunctions.add("glUniform1fv",                 f_glUniform1fv, QDOM_GUI);
   //builtinFunctions.add("glUniform2fv",                 f_glUniform2fv, QDOM_GUI);
   //builtinFunctions.add("glUniform3fv",                 f_glUniform3fv, QDOM_GUI);
   //builtinFunctions.add("glUniform4fv",                 f_glUniform4fv, QDOM_GUI);
   //builtinFunctions.add("glUniform1iv",                 f_glUniform1iv, QDOM_GUI);
   //builtinFunctions.add("glUniform2iv",                 f_glUniform2iv, QDOM_GUI);
   //builtinFunctions.add("glUniform3iv",                 f_glUniform3iv, QDOM_GUI);
   //builtinFunctions.add("glUniform4iv",                 f_glUniform4iv, QDOM_GUI);
   //builtinFunctions.add("glUniformMatrix2fv",           f_glUniformMatrix2fv, QDOM_GUI);
   //builtinFunctions.add("glUniformMatrix3fv",           f_glUniformMatrix3fv, QDOM_GUI);
   //builtinFunctions.add("glUniformMatrix4fv",           f_glUniformMatrix4fv, QDOM_GUI);
   builtinFunctions.add("glIsShader",                   f_glIsShader, QDOM_GUI);
   builtinFunctions.add("glIsProgram",                  f_glIsProgram, QDOM_GUI);
   //builtinFunctions.add("glGetShaderiv",                f_glGetShaderiv, QDOM_GUI);
   //builtinFunctions.add("glGetProgramiv",               f_glGetProgramiv, QDOM_GUI);
   //builtinFunctions.add("glGetAttachedShaders",         f_glGetAttachedShaders, QDOM_GUI);
   //builtinFunctions.add("glGetShaderInfoLog",           f_glGetShaderInfoLog, QDOM_GUI);
   //builtinFunctions.add("glGetProgramInfoLog",          f_glGetProgramInfoLog, QDOM_GUI);
   //builtinFunctions.add("glGetUniformLocation",         f_glGetUniformLocation, QDOM_GUI);
   //builtinFunctions.add("glGetActiveUniform",           f_glGetActiveUniform, QDOM_GUI);
   //builtinFunctions.add("glGetUniformfv",               f_glGetUniformfv, QDOM_GUI);
   //builtinFunctions.add("glGetUniformiv",               f_glGetUniformiv, QDOM_GUI);
   //builtinFunctions.add("glGetShaderSource",            f_glGetShaderSource, QDOM_GUI);
   //builtinFunctions.add("glBindAttribLocation",         f_glBindAttribLocation, QDOM_GUI);
   //builtinFunctions.add("glGetActiveAttrib",            f_glGetActiveAttrib, QDOM_GUI);
   //builtinFunctions.add("glGetAttribLocation",          f_glGetAttribLocation, QDOM_GUI);
   builtinFunctions.add("glStencilFuncSeparate",        f_glStencilFuncSeparate, QDOM_GUI);
   builtinFunctions.add("glStencilOpSeparate",          f_glStencilOpSeparate, QDOM_GUI);
   builtinFunctions.add("glStencilMaskSeparate",        f_glStencilMaskSeparate, QDOM_GUI);
   //builtinFunctions.add("glUniformMatrix2x3fv",         f_glUniformMatrix2x3fv, QDOM_GUI);
   //builtinFunctions.add("glUniformMatrix3x2fv",         f_glUniformMatrix3x2fv, QDOM_GUI);
   //builtinFunctions.add("glUniformMatrix2x4fv",         f_glUniformMatrix2x4fv, QDOM_GUI);
   //builtinFunctions.add("glUniformMatrix4x2fv",         f_glUniformMatrix4x2fv, QDOM_GUI);
   //builtinFunctions.add("glUniformMatrix3x4fv",         f_glUniformMatrix3x4fv, QDOM_GUI);
   //builtinFunctions.add("glUniformMatrix4x3fv",         f_glUniformMatrix4x3fv, QDOM_GUI);

   initOpenGLExt();
   initOpenGLU();

   addOpenGLConstants();
   addOpenGLExtConstants();

   return 0;
}

static void opengl_module_ns_init(QoreNamespace *rns, QoreNamespace *qns)
{
   qns->addInitialNamespace(opengl_ns.copy());
}

static void opengl_module_delete()
{
}

