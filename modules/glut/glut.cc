/*
  glut.cc
  
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

#include "qore-glut.h"

static QoreStringNode *glut_module_init();
static void glut_module_ns_init(QoreNamespace *rns, QoreNamespace *qns);
static void glut_module_delete();

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "glut";
DLLEXPORT char qore_module_version[] = "0.1";
DLLEXPORT char qore_module_description[] = "Glut module";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://qore.sourceforge.net";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = glut_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = glut_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = glut_module_delete;
#endif

static QoreThreadLock reshape_lock, display_lock, keyboard_lock, visibility_lock, idle_lock, special_lock;
static ResolvedFunctionReferenceNode *reshape_ref = 0, 
   *display_ref = 0, 
   *keyboard_ref = 0,
   *visibility_ref = 0,
   *idle_ref = 0,
   *special_ref = 0;

QoreNamespace glut_ns("Glut");

//void glutInit(int *argcp, char **argv)
static AbstractQoreNode *f_glutInit(const QoreListNode *params, ExceptionSink *xsink)
{
   static int argc = 1;
   static char *argv[] = { "gears.q", 0 };
   glutInit(&argc, argv);
   return 0;
}

//void glutInitDisplayMode(unsigned int mode);
static AbstractQoreNode *f_glutInitDisplayMode(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned int mode = (unsigned)(p ? p->getAsInt() : 0);
   glutInitDisplayMode(mode);
   return 0;
}

//void glutInitWindowSize(int width, int height);
static AbstractQoreNode *f_glutInitWindowSize(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int height = p ? p->getAsInt() : 0;
   glutInitWindowSize(width, height);
   return 0;
}

//void glutInitWindowPosition(int x, int y);
static AbstractQoreNode *f_glutInitWindowPosition(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   glutInitWindowPosition(x, y);
   return 0;
}

//int glutCreateWindow(char *name);
static AbstractQoreNode *f_glutCreateWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_STRING) {
      xsink->raiseException("GLUTCREATEWINDOW-PARAM-ERROR", "expecting a string as first argument to glutCreateWindow()");
      return 0;
   }
   const char *name = reinterpret_cast<const QoreStringNode *>(p)->getBuffer();
   return new QoreBigIntNode(glutCreateWindow(name));
}

//void glutPostRedisplay(void);
static AbstractQoreNode *f_glutPostRedisplay(const QoreListNode *params, ExceptionSink *xsink)
{
   glutPostRedisplay();
   return 0;
}

//void glutPostWindowRedisplay(int win);
static AbstractQoreNode *f_glutPostWindowRedisplay(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int win = p ? p->getAsInt() : 0;
   glutPostWindowRedisplay(win);
   return 0;
}

//void glutMainLoop(void);
static AbstractQoreNode *f_glutMainLoop(const QoreListNode *params, ExceptionSink *xsink)
{
   glutMainLoop();
   return 0;
}

//void glutSwapBuffers(void);
static AbstractQoreNode *f_glutSwapBuffers(const QoreListNode *params, ExceptionSink *xsink)
{
   glutSwapBuffers();
   return 0;
}

void reshape_func(int width, int height)
{
   //printd(5, "reshape_func(width=%d, height=%d) reshape_ref=%08p\n", width, height, reshape_ref);
   AutoLocker al(&reshape_lock);
   if (reshape_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(width));
      args->push(new QoreBigIntNode(height));
      discard(reshape_ref->exec(*args, &xsink), &xsink);
   }
}

void display_func()
{
   //printd(5, "display_func() display_ref=%08p\n", display_ref);
   AutoLocker al(&display_lock);
   if (display_ref) {
      ExceptionSink xsink;
      discard(display_ref->exec(0, &xsink), &xsink);
   }
}

void keyboard_func(unsigned char key, int x, int y)
{
   //printd(5, "keyboard_func(key=%d (%c), x=%d, y=%d) keyboard_ref=%08p\n", key, key, x, y, keyboard_ref);
   AutoLocker al(&keyboard_lock);
   if (keyboard_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(key));
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      discard(keyboard_ref->exec(*args, &xsink), &xsink);
   }
}

void visibility_func(int state)
{
   //printd(5, "visibility_func(state=%d) visibility_ref=%08p\n", state, visibility_ref);
   AutoLocker al(&visibility_lock);
   if (visibility_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(state));
      discard(visibility_ref->exec(*args, &xsink), &xsink);
   }
}

void idle_func()
{
   //printd(5, "idle_func() idle_ref=%08p\n", idle_ref);
   AutoLocker al(&idle_lock);
   if (idle_ref) {
      ExceptionSink xsink;
      discard(idle_ref->exec(0, &xsink), &xsink);
   }
}

void special_func(int key, int x, int y)
{
   //printd(5, "special_func(key=%d, x=%d, y=%d) special_ref=%08p\n", key, x, y, special_ref);
   AutoLocker al(&special_lock);
   if (special_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(key));
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      discard(special_ref->exec(*args, &xsink), &xsink);
   }
}

static AbstractQoreNode *f_glutReshapeFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedFunctionReferenceNode *r = test_funcref_param(params, 0);

   //printd(5, "glutReshapeFunc() params=%08p (%d) r=%08p (%d)\n", params, params ? params->needs_eval() : -1, r, r ? r->needs_eval() : -1);

   AutoLocker al(&reshape_lock);
   glutReshapeFunc(r ? reshape_func : 0);
   if (reshape_ref)
      reshape_ref->deref(xsink);
   reshape_ref = const_cast<ResolvedFunctionReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

static AbstractQoreNode *f_glutDisplayFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedFunctionReferenceNode *r = test_funcref_param(params, 0);
   AutoLocker al(&display_lock);
   glutDisplayFunc(r ? display_func : 0);
   if (display_ref)
      display_ref->deref(xsink);
   display_ref = const_cast<ResolvedFunctionReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

static AbstractQoreNode *f_glutKeyboardFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedFunctionReferenceNode *r = test_funcref_param(params, 0);
   AutoLocker al(&keyboard_lock);
   glutKeyboardFunc(r ? keyboard_func : 0);
   if (keyboard_ref)
      keyboard_ref->deref(xsink);
   keyboard_ref = const_cast<ResolvedFunctionReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

static AbstractQoreNode *f_glutVisibilityFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedFunctionReferenceNode *r = test_funcref_param(params, 0);
   AutoLocker al(&visibility_lock);
   glutVisibilityFunc(r ? visibility_func : 0);
   if (visibility_ref)
      visibility_ref->deref(xsink);
   visibility_ref = const_cast<ResolvedFunctionReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

static AbstractQoreNode *f_glutIdleFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedFunctionReferenceNode *r = test_funcref_param(params, 0);
   AutoLocker al(&idle_lock);
   glutIdleFunc(r ? idle_func : 0);
   if (idle_ref)
      idle_ref->deref(xsink);
   idle_ref = const_cast<ResolvedFunctionReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

static AbstractQoreNode *f_glutSpecialFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedFunctionReferenceNode *r = test_funcref_param(params, 0);
   AutoLocker al(&special_lock);
   glutSpecialFunc(r ? special_func : 0);
   if (special_ref)
      special_ref->deref(xsink);
   special_ref = const_cast<ResolvedFunctionReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

static QoreStringNode *glut_module_init()
{
   builtinFunctions.add("glutInit",                     f_glutInit);
   builtinFunctions.add("glutInitDisplayMode",          f_glutInitDisplayMode);
   builtinFunctions.add("glutInitWindowSize",           f_glutInitWindowSize);
   builtinFunctions.add("glutInitWindowPosition",       f_glutInitWindowPosition);
   builtinFunctions.add("glutCreateWindow",             f_glutCreateWindow);
   builtinFunctions.add("glutPostRedisplay",            f_glutPostRedisplay);
   builtinFunctions.add("glutPostWindowRedisplay",      f_glutPostWindowRedisplay);
   builtinFunctions.add("glutMainLoop",                 f_glutMainLoop);
   builtinFunctions.add("glutSwapBuffers",              f_glutSwapBuffers);

   builtinFunctions.add("glutReshapeFunc",              f_glutReshapeFunc);
   builtinFunctions.add("glutDisplayFunc",              f_glutDisplayFunc);
   builtinFunctions.add("glutKeyboardFunc",             f_glutKeyboardFunc);
   builtinFunctions.add("glutVisibilityFunc",           f_glutVisibilityFunc);
   builtinFunctions.add("glutIdleFunc",                 f_glutIdleFunc);
   builtinFunctions.add("glutSpecialFunc",              f_glutSpecialFunc);

   addGlutConstants();

   return 0;
}

static void glut_module_ns_init(QoreNamespace *rns, QoreNamespace *qns)
{
   qns->addInitialNamespace(glut_ns.copy());
}

static void glut_module_delete()
{
}
