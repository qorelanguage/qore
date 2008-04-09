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
      a[i] = p ? p->getAsBigInt() : 0.0;
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
      a[i] = p ? p->getAsBigInt() : 0.0;
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
      a[i] = p ? p->getAsBigInt() : 0.0;
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

   QoreListNode *l = new QoreListNode();
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

   QoreListNode *l = new QoreListNode();
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

   QoreListNode *l = new QoreListNode();
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

   QoreListNode *l = new QoreListNode();
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
      parms[i] = p ? p->getAsInt() : 0.0;
   }

   glTexEnviv(target, pname, parms);
   return 0;
}

static QoreStringNode *opengl_module_init()
{
   builtinFunctions.add("glGenLists",                   f_glGenLists);
   builtinFunctions.add("glDeleteLists",                f_glDeleteLists);
   builtinFunctions.add("glNewList",                    f_glNewList);
   builtinFunctions.add("glEndList",                    f_glEndList);
   builtinFunctions.add("glBegin",                      f_glBegin);
   builtinFunctions.add("glEnd",                        f_glEnd);
   builtinFunctions.add("glGetError",                   f_glGetError);

   builtinFunctions.add("glTexCoord1d",                 f_glTexCoord1d);
   builtinFunctions.add("glTexCoord1f",                 f_glTexCoord1f);
   builtinFunctions.add("glTexCoord1i",                 f_glTexCoord1i);
   builtinFunctions.add("glTexCoord1s",                 f_glTexCoord1s);
   builtinFunctions.add("glTexCoord2d",                 f_glTexCoord2d);
   builtinFunctions.add("glTexCoord2f",                 f_glTexCoord2f);
   builtinFunctions.add("glTexCoord2i",                 f_glTexCoord2i);
   builtinFunctions.add("glTexCoord2s",                 f_glTexCoord2s);
   builtinFunctions.add("glTexCoord3d",                 f_glTexCoord3d);
   builtinFunctions.add("glTexCoord3f",                 f_glTexCoord3f);
   builtinFunctions.add("glTexCoord3i",                 f_glTexCoord3i);
   builtinFunctions.add("glTexCoord3s",                 f_glTexCoord3s);
   builtinFunctions.add("glTexCoord4d",                 f_glTexCoord4d);
   builtinFunctions.add("glTexCoord4f",                 f_glTexCoord4f);
   builtinFunctions.add("glTexCoord4i",                 f_glTexCoord4i);
   builtinFunctions.add("glTexCoord4s",                 f_glTexCoord4s);

   builtinFunctions.add("glVertex2d",                   f_glVertex2d);
   builtinFunctions.add("glVertex2f",                   f_glVertex2f);
   builtinFunctions.add("glVertex2i",                   f_glVertex2i);
   builtinFunctions.add("glVertex2s",                   f_glVertex2s);
   builtinFunctions.add("glVertex3d",                   f_glVertex3d);
   builtinFunctions.add("glVertex3f",                   f_glVertex3f);
   builtinFunctions.add("glVertex3i",                   f_glVertex3i);
   builtinFunctions.add("glVertex3s",                   f_glVertex3s);
   builtinFunctions.add("glVertex4d",                   f_glVertex4d);
   builtinFunctions.add("glVertex4f",                   f_glVertex4f);
   builtinFunctions.add("glVertex4i",                   f_glVertex4i);
   builtinFunctions.add("glVertex4s",                   f_glVertex4s);

   builtinFunctions.add("glClear",                      f_glClear);
   builtinFunctions.add("glClearColor",                 f_glClearColor);
   builtinFunctions.add("glPushAttrib",                 f_glPushAttrib);
   builtinFunctions.add("glPopAttrib",                  f_glPopAttrib);
   builtinFunctions.add("glMatrixMode",                 f_glMatrixMode);
   builtinFunctions.add("glPushMatrix",                 f_glPushMatrix);
   builtinFunctions.add("glPopMatrix",                  f_glPopMatrix);
   builtinFunctions.add("glLoadIdentity",               f_glLoadIdentity);
   builtinFunctions.add("glFrustum",                    f_glFrustum);
   builtinFunctions.add("glTranslated",                 f_glTranslated);
   builtinFunctions.add("glTranslatef",                 f_glTranslatef);
   builtinFunctions.add("glViewport",                   f_glViewport);
   builtinFunctions.add("glEnable",                     f_glEnable);
   builtinFunctions.add("glDisable",                    f_glDisable);
   builtinFunctions.add("glBlendFunc",                  f_glBlendFunc);
   builtinFunctions.add("glBindTexture",                f_glBindTexture);

   builtinFunctions.add("glScaled",                     f_glScaled);
   builtinFunctions.add("glScalef",                     f_glScalef);

   builtinFunctions.add("glColor3b",                    f_glColor3b);
   builtinFunctions.add("glColor3d",                    f_glColor3d);
   builtinFunctions.add("glColor3f",                    f_glColor3f);
   builtinFunctions.add("glColor3i",                    f_glColor3i);
   builtinFunctions.add("glColor3s",                    f_glColor3s);
   builtinFunctions.add("glColor3ub",                   f_glColor3ub);
   builtinFunctions.add("glColor3ui",                   f_glColor3ui);
   builtinFunctions.add("glColor3us",                   f_glColor3us);
   builtinFunctions.add("glColor4b",                    f_glColor4b);
   builtinFunctions.add("glColor4d",                    f_glColor4d);
   builtinFunctions.add("glColor4f",                    f_glColor4f);
   builtinFunctions.add("glColor4i",                    f_glColor4i);
   builtinFunctions.add("glColor4s",                    f_glColor4s);
   builtinFunctions.add("glColor4ub",                   f_glColor4ub);
   builtinFunctions.add("glColor4ui",                   f_glColor4ui);
   builtinFunctions.add("glColor4us",                   f_glColor4us);
   builtinFunctions.add("glCallList",                   f_glCallList);
   builtinFunctions.add("glRotated",                    f_glRotated);
   builtinFunctions.add("glRotatef",                    f_glRotatef);
   builtinFunctions.add("glDepthFunc",                  f_glDepthFunc);

   builtinFunctions.add("glNormal3b",                   f_glNormal3b);
   builtinFunctions.add("glNormal3d",                   f_glNormal3d);
   builtinFunctions.add("glNormal3f",                   f_glNormal3f);
   builtinFunctions.add("glNormal3i",                   f_glNormal3i);
   builtinFunctions.add("glNormal3s",                   f_glNormal3s);

   builtinFunctions.add("glLightf",                     f_glLightf);
   builtinFunctions.add("glLighti",                     f_glLighti);
   builtinFunctions.add("glLightfv",                    f_glLightfv);
   builtinFunctions.add("glLightiv",                    f_glLightiv);

   builtinFunctions.add("glMaterialf",                  f_glMaterialf);
   builtinFunctions.add("glMateriali",                  f_glMateriali);
   builtinFunctions.add("glMaterialfv",                 f_glMaterialfv);
   builtinFunctions.add("glMaterialiv",                 f_glMaterialiv);

   builtinFunctions.add("glShadeModel",                 f_glShadeModel);

   builtinFunctions.add("glOrtho",                      f_glOrtho);
   builtinFunctions.add("glHint",                       f_glHint);

   builtinFunctions.add("glClearDepth",                 f_glClearDepth);
   builtinFunctions.add("glFlush",                      f_glFlush);

   builtinFunctions.add("glLightModelf",                f_glLightModelf);
   builtinFunctions.add("glLightModeli",                f_glLightModeli);
   builtinFunctions.add("glLightModelfv",               f_glLightModelfv);
   builtinFunctions.add("glLightModeliv",               f_glLightModeliv);

   builtinFunctions.add("glGetBooleanv",                f_glGetBooleanv);
   builtinFunctions.add("glGetDoublev",                 f_glGetDoublev);
   builtinFunctions.add("glGetFloatv",                  f_glGetFloatv);
   builtinFunctions.add("glGetIntegerv",                f_glGetIntegerv);

   builtinFunctions.add("glTexEnvf",                    f_glTexEnvf);
   builtinFunctions.add("glTexEnvi",                    f_glTexEnvi);
   builtinFunctions.add("glTexEnvfv",                   f_glTexEnvfv);
   builtinFunctions.add("glTexEnviv",                   f_glTexEnviv);

   addOpenGLConstants();

   return 0;
}

static void opengl_module_ns_init(QoreNamespace *rns, QoreNamespace *qns)
{
   qns->addInitialNamespace(opengl_ns.copy());
}

static void opengl_module_delete()
{
}

