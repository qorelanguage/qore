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

//void glShadeModel( GLenum mode )
static AbstractQoreNode *f_glShadeModel(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   glShadeModel(mode);
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
