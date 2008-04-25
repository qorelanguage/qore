/*
  glut.cc
  
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

#include "qore-glut.h"

#include <map>

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
DLLEXPORT char *qore_module_dependencies[] = { "opengl", 0 };
#endif

// type for mapping glut menu entries to call references
typedef std::map<int, ResolvedCallReferenceNode *> menu_map_t;

// menu map lock
static QoreThreadLock menu_map_lock;
// menu map:  glut menu entries to call references for callback functions
static menu_map_t menu_map;

// locks for callback functions
static QoreThreadLock 
#ifdef HAVE_GLUTWMCLOSE
   wmclose_lock,
#endif
   display_lock,
   reshape_lock,
   keyboard_lock,
   mouse_lock,
   motion_lock,
   passivemotion_lock,
   entry_lock,
   visibility_lock,
   idle_lock,
   timer_lock,
   menustate_lock,
   special_lock,
   spaceballmotion_lock,
   spaceballrotate_lock,
   spaceballbutton_lock,
   buttonbox_lock,
   dials_lock,
   tabletmotion_lock,
   tabletbutton_lock,
   menustatus_lock,
   overlaydisplay_lock,
   windowstatus_lock,
   keyboardup_lock,
   specialup_lock,
   joystick_lock;

// call references for callback functions
static ResolvedCallReferenceNode 
#ifdef HAVE_GLUTWMCLOSE
   *wmclose_ref = 0,
#endif
   *display_ref = 0,
   *reshape_ref = 0,
   *keyboard_ref = 0,
   *mouse_ref = 0,
   *motion_ref = 0,
   *passivemotion_ref = 0,
   *entry_ref = 0,
   *visibility_ref = 0,
   *idle_ref = 0,
   *timer_ref = 0,
   *menustate_ref = 0,
   *special_ref = 0,
   *spaceballmotion_ref = 0,
   *spaceballrotate_ref = 0,
   *spaceballbutton_ref = 0,
   *buttonbox_ref = 0,
   *dials_ref = 0,
   *tabletmotion_ref = 0,
   *tabletbutton_ref = 0,
   *menustatus_ref = 0,
   *overlaydisplay_ref = 0,
   *windowstatus_ref = 0,
   *keyboardup_ref = 0,
   *specialup_ref = 0,
   *joystick_ref = 0;

QoreNamespace glut_ns("Glut");


static void qore_glut_menu_callback(int value)
{
   int menu = glutGetMenu();
   AutoLocker al(&menu_map_lock);
   menu_map_t::iterator i = menu_map.find(menu);
   if (i == menu_map.end())
      return;

   ExceptionSink xsink;
   ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
   args->push(new QoreBigIntNode(value));
   discard(i->second->exec(*args, &xsink), &xsink);
}

static void qore_glut_reshape_func(int width, int height)
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

static void qore_glut_display_func()
{
   //printd(5, "display_func() display_ref=%08p\n", display_ref);
   AutoLocker al(&display_lock);
   if (display_ref) {
      ExceptionSink xsink;
      discard(display_ref->exec(0, &xsink), &xsink);
   }
}

static void qore_glut_keyboard_func(unsigned char key, int x, int y)
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

static void qore_glut_visibility_func(int state)
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

static void qore_glut_idle_func()
{
   //printd(5, "idle_func() idle_ref=%08p\n", idle_ref);
   AutoLocker al(&idle_lock);
   if (idle_ref) {
      ExceptionSink xsink;
      discard(idle_ref->exec(0, &xsink), &xsink);
   }
}

static void qore_glut_special_func(int key, int x, int y)
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

#ifdef HAVE_GLUTWMCLOSE
static void qore_glut_wmclose_func()
{
   AutoLocker al(&wmclose_lock);
   if (wmclose_ref) {
      ExceptionSink xsink;
      discard(wmclose_ref->exec(0, &xsink), &xsink);
   }
}
#endif

static void qore_glut_mouse_func(int button, int state, int x, int y)
{
   AutoLocker al(&mouse_lock);
   if (mouse_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(button));
      args->push(new QoreBigIntNode(state));
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      discard(mouse_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_motion_func(int x, int y)
{
   AutoLocker al(&motion_lock);
   if (motion_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      discard(motion_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_passivemotion_func(int x, int y)
{
   AutoLocker al(&passivemotion_lock);
   if (passivemotion_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      discard(passivemotion_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_entry_func(int state)
{
   AutoLocker al(&entry_lock);
   if (entry_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(state));
      discard(entry_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_timer_func(int value)
{
   AutoLocker al(&timer_lock);
   if (timer_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(value));
      discard(timer_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_menustate_func(int state)
{
   AutoLocker al(&menustate_lock);
   if (menustate_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(state));
      discard(menustate_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_spaceballmotion_func(int x, int y, int z)
{
   AutoLocker al(&spaceballmotion_lock);
   if (spaceballmotion_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      args->push(new QoreBigIntNode(z));
      discard(spaceballmotion_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_spaceballrotate_func(int x, int y, int z)
{
   AutoLocker al(&spaceballrotate_lock);
   if (spaceballrotate_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      args->push(new QoreBigIntNode(z));
      discard(spaceballrotate_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_spaceballbutton_func(int button, int state)
{
   AutoLocker al(&spaceballbutton_lock);
   if (spaceballbutton_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(button));
      args->push(new QoreBigIntNode(state));
      discard(spaceballbutton_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_buttonbox_func(int button, int state)
{
   AutoLocker al(&buttonbox_lock);
   if (buttonbox_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(button));
      args->push(new QoreBigIntNode(state));
      discard(buttonbox_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_dials_func(int dial, int value)
{
   AutoLocker al(&dials_lock);
   if (dials_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(dial));
      args->push(new QoreBigIntNode(value));
      discard(dials_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_tabletmotion_func(int x, int y)
{
   AutoLocker al(&tabletmotion_lock);
   if (tabletmotion_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      discard(tabletmotion_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_tabletbutton_func(int button, int state, int x, int y)
{
   AutoLocker al(&tabletbutton_lock);
   if (tabletbutton_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(button));
      args->push(new QoreBigIntNode(state));
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      discard(tabletbutton_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_menustatus_func(int status, int x, int y)
{
   AutoLocker al(&menustatus_lock);
   if (menustatus_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(status));
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      discard(menustatus_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_overlaydisplay_func()
{
   AutoLocker al(&overlaydisplay_lock);
   if (overlaydisplay_ref) {
      ExceptionSink xsink;
      discard(overlaydisplay_ref->exec(0, &xsink), &xsink);
   }
}

static void qore_glut_windowstatus_func(int state)
{
   AutoLocker al(&windowstatus_lock);
   if (windowstatus_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(state));
      discard(windowstatus_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_keyboardup_func(unsigned char key, int x, int y)
{
   AutoLocker al(&keyboardup_lock);
   if (keyboardup_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(key));
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      discard(keyboardup_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_specialup_func(int key, int x, int y)
{
   AutoLocker al(&specialup_lock);
   if (specialup_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(key));
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      discard(specialup_ref->exec(*args, &xsink), &xsink);
   }
}

static void qore_glut_joystick_func(unsigned int buttonMask, int x, int y, int z)
{
   AutoLocker al(&joystick_lock);
   if (joystick_ref) {
      ExceptionSink xsink;
      ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
      args->push(new QoreBigIntNode(buttonMask));
      args->push(new QoreBigIntNode(x));
      args->push(new QoreBigIntNode(y));
      args->push(new QoreBigIntNode(z));
      discard(joystick_ref->exec(*args, &xsink), &xsink);
   }
}

#ifdef HAVE_GLUTWMCLOSE
//void glutWMCloseFunc(void (*func)(void));
static AbstractQoreNode *f_glutWMCloseFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&wmclose_lock);
   glutWMCloseFunc(r ? qore_glut_wmclose_func : 0);
   if (wmclose_ref)
      wmclose_ref->deref(xsink);
   wmclose_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}
#endif

//void glutDisplayFunc(void (*func)(void));
static AbstractQoreNode *f_glutDisplayFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&display_lock);
   glutDisplayFunc(r ? qore_glut_display_func : 0);
   if (display_ref)
      display_ref->deref(xsink);
   display_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutReshapeFunc(void (*func)(int width, int height));
static AbstractQoreNode *f_glutReshapeFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&reshape_lock);
   glutReshapeFunc(r ? qore_glut_reshape_func : 0);
   if (reshape_ref)
      reshape_ref->deref(xsink);
   reshape_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutKeyboardFunc(void (*func)(unsigned char key, int x, int y));
static AbstractQoreNode *f_glutKeyboardFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&keyboard_lock);
   glutKeyboardFunc(r ? qore_glut_keyboard_func : 0);
   if (keyboard_ref)
      keyboard_ref->deref(xsink);
   keyboard_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutMouseFunc(void (*func)(int button, int state, int x, int y));
static AbstractQoreNode *f_glutMouseFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&mouse_lock);
   glutMouseFunc(r ? qore_glut_mouse_func : 0);
   if (mouse_ref)
      mouse_ref->deref(xsink);
   mouse_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutMotionFunc(void (*func)(int x, int y));
static AbstractQoreNode *f_glutMotionFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&motion_lock);
   glutMotionFunc(r ? qore_glut_motion_func : 0);
   if (motion_ref)
      motion_ref->deref(xsink);
   motion_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutPassiveMotionFunc(void (*func)(int x, int y));
static AbstractQoreNode *f_glutPassiveMotionFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&passivemotion_lock);
   glutPassiveMotionFunc(r ? qore_glut_passivemotion_func : 0);
   if (passivemotion_ref)
      passivemotion_ref->deref(xsink);
   passivemotion_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutEntryFunc(void (*func)(int state));
static AbstractQoreNode *f_glutEntryFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&entry_lock);
   glutEntryFunc(r ? qore_glut_entry_func : 0);
   if (entry_ref)
      entry_ref->deref(xsink);
   entry_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutVisibilityFunc(void (*func)(int state));
static AbstractQoreNode *f_glutVisibilityFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&visibility_lock);
   glutVisibilityFunc(r ? qore_glut_visibility_func : 0);
   if (visibility_ref)
      visibility_ref->deref(xsink);
   visibility_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutIdleFunc(void (*func)(void));
static AbstractQoreNode *f_glutIdleFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&idle_lock);
   glutIdleFunc(r ? qore_glut_idle_func : 0);
   if (idle_ref)
      idle_ref->deref(xsink);
   idle_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutTimerFunc(unsigned int millis, void (*func)(int value), int value);
static AbstractQoreNode *f_glutTimerFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   unsigned millis = p ? p->getAsBigInt() : 0;

   const ResolvedCallReferenceNode *r = test_funcref_param(params, 1);

   p = get_param(params, 2);
   int value = p ? p->getAsInt() : 0;

   AutoLocker al(&timer_lock);
   glutTimerFunc(millis, r ? qore_glut_timer_func : 0, value);
   if (timer_ref)
      timer_ref->deref(xsink);
   timer_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutMenuStateFunc(void (*func)(int state));
static AbstractQoreNode *f_glutMenuStateFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&menustate_lock);
   glutMenuStateFunc(r ? qore_glut_menustate_func : 0);
   if (menustate_ref)
      menustate_ref->deref(xsink);
   menustate_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutSpecialFunc(void (*func)(int key, int x, int y));
static AbstractQoreNode *f_glutSpecialFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&special_lock);
   glutSpecialFunc(r ? qore_glut_special_func : 0);
   if (special_ref)
      special_ref->deref(xsink);
   special_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutSpaceballMotionFunc(void (*func)(int x, int y, int z));
static AbstractQoreNode *f_glutSpaceballMotionFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&spaceballmotion_lock);
   glutSpaceballMotionFunc(r ? qore_glut_spaceballmotion_func : 0);
   if (spaceballmotion_ref)
      spaceballmotion_ref->deref(xsink);
   spaceballmotion_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutSpaceballRotateFunc(void (*func)(int x, int y, int z));
static AbstractQoreNode *f_glutSpaceballRotateFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&spaceballrotate_lock);
   glutSpaceballRotateFunc(r ? qore_glut_spaceballrotate_func : 0);
   if (spaceballrotate_ref)
      spaceballrotate_ref->deref(xsink);
   spaceballrotate_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutSpaceballButtonFunc(void (*func)(int button, int state));
static AbstractQoreNode *f_glutSpaceballButtonFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&spaceballbutton_lock);
   glutSpaceballButtonFunc(r ? qore_glut_spaceballbutton_func : 0);
   if (spaceballbutton_ref)
      spaceballbutton_ref->deref(xsink);
   spaceballbutton_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutButtonBoxFunc(void (*func)(int button, int state));
static AbstractQoreNode *f_glutButtonBoxFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&buttonbox_lock);
   glutButtonBoxFunc(r ? qore_glut_buttonbox_func : 0);
   if (buttonbox_ref)
      buttonbox_ref->deref(xsink);
   buttonbox_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutDialsFunc(void (*func)(int dial, int value));
static AbstractQoreNode *f_glutDialsFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&dials_lock);
   glutDialsFunc(r ? qore_glut_dials_func : 0);
   if (dials_ref)
      dials_ref->deref(xsink);
   dials_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutTabletMotionFunc(void (*func)(int x, int y));
static AbstractQoreNode *f_glutTabletMotionFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&tabletmotion_lock);
   glutTabletMotionFunc(r ? qore_glut_tabletmotion_func : 0);
   if (tabletmotion_ref)
      tabletmotion_ref->deref(xsink);
   tabletmotion_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutTabletButtonFunc(void (*func)(int button, int state, int x, int y));
static AbstractQoreNode *f_glutTabletButtonFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&tabletbutton_lock);
   glutTabletButtonFunc(r ? qore_glut_tabletbutton_func : 0);
   if (tabletbutton_ref)
      tabletbutton_ref->deref(xsink);
   tabletbutton_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutMenuStatusFunc(void (*func)(int status, int x, int y));
static AbstractQoreNode *f_glutMenuStatusFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&menustatus_lock);
   glutMenuStatusFunc(r ? qore_glut_menustatus_func : 0);
   if (menustatus_ref)
      menustatus_ref->deref(xsink);
   menustatus_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutOverlayDisplayFunc(void (*func)(void));
static AbstractQoreNode *f_glutOverlayDisplayFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&overlaydisplay_lock);
   glutOverlayDisplayFunc(r ? qore_glut_overlaydisplay_func : 0);
   if (overlaydisplay_ref)
      overlaydisplay_ref->deref(xsink);
   overlaydisplay_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutWindowStatusFunc(void (*func)(int state));
static AbstractQoreNode *f_glutWindowStatusFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&windowstatus_lock);
   glutWindowStatusFunc(r ? qore_glut_windowstatus_func : 0);
   if (windowstatus_ref)
      windowstatus_ref->deref(xsink);
   windowstatus_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutKeyboardUpFunc(void (*func)(unsigned char key, int x, int y));
static AbstractQoreNode *f_glutKeyboardUpFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&keyboardup_lock);
   glutKeyboardUpFunc(r ? qore_glut_keyboardup_func : 0);
   if (keyboardup_ref)
      keyboardup_ref->deref(xsink);
   keyboardup_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutSpecialUpFunc(void (*func)(int key, int x, int y));
static AbstractQoreNode *f_glutSpecialUpFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   AutoLocker al(&specialup_lock);
   glutSpecialUpFunc(r ? qore_glut_specialup_func : 0);
   if (specialup_ref)
      specialup_ref->deref(xsink);
   specialup_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutJoystickFunc(void (*func)(unsigned int buttonMask, int x, int y, int z), int pollInterval);
static AbstractQoreNode *f_glutJoystickFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   const AbstractQoreNode *p = get_param(params, 1);
   int pollInterval = p ? p->getAsInt() : 0;

   AutoLocker al(&joystick_lock);
   glutJoystickFunc(r ? qore_glut_joystick_func : 0, pollInterval);
   if (joystick_ref)
      joystick_ref->deref(xsink);
   joystick_ref = const_cast<ResolvedCallReferenceNode *>(r);
   if (r)
      r->ref();

   return 0;
}

//void glutForceJoystickFunc(void);
static AbstractQoreNode *f_glutForceJoystickFunc(const QoreListNode *params, ExceptionSink *xsink)
{
   glutForceJoystickFunc();
   return 0;
}

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

//int glutGet(GLenum state);
static AbstractQoreNode *f_glutGet(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum state = (GLenum)(p ? p->getAsInt() : 0);
   return new QoreBigIntNode(glutGet(state));
}

//void glutInitDisplayString(const char *string);
static AbstractQoreNode *f_glutInitDisplayString(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_STRING) {
      xsink->raiseException("GLUTINITDISPLAYSTRING-PARAM-ERROR", "expecting a string as first argument to glutInitDisplayString()");
      return 0;
   }
   const char *string = reinterpret_cast<const QoreStringNode *>(p)->getBuffer();
   glutInitDisplayString(string);
   return 0;
}


//int glutCreateSubWindow(int win, int x, int y, int width, int height);
static AbstractQoreNode *f_glutCreateSubWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int win = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int height = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(glutCreateSubWindow(win, x, y, width, height));
}

//void glutDestroyWindow(int win);
static AbstractQoreNode *f_glutDestroyWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int win = p ? p->getAsInt() : 0;
   glutDestroyWindow(win);
   return 0;
}

//int glutGetWindow(void);
static AbstractQoreNode *f_glutGetWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(glutGetWindow());
}

//void glutSetWindow(int win);
static AbstractQoreNode *f_glutSetWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int win = p ? p->getAsInt() : 0;
   glutSetWindow(win);
   return 0;
}

//void glutSetWindowTitle(const char *title);
static AbstractQoreNode *f_glutSetWindowTitle(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_STRING) {
      xsink->raiseException("GLUTSETWINDOWTITLE-PARAM-ERROR", "expecting a string as first argument to glutSetWindowTitle()");
      return 0;
   }
   const char *title = reinterpret_cast<const QoreStringNode *>(p)->getBuffer();
   glutSetWindowTitle(title);
   return 0;
}

//void glutSetIconTitle(const char *title);
static AbstractQoreNode *f_glutSetIconTitle(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_STRING) {
      xsink->raiseException("GLUTSETICONTITLE-PARAM-ERROR", "expecting a string as first argument to glutSetIconTitle()");
      return 0;
   }
   const char *title = reinterpret_cast<const QoreStringNode *>(p)->getBuffer();
   glutSetIconTitle(title);
   return 0;
}

//void glutPositionWindow(int x, int y);
static AbstractQoreNode *f_glutPositionWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   glutPositionWindow(x, y);
   return 0;
}

//void glutReshapeWindow(int width, int height);
static AbstractQoreNode *f_glutReshapeWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int height = p ? p->getAsInt() : 0;
   glutReshapeWindow(width, height);
   return 0;
}

//void glutPopWindow(void);
static AbstractQoreNode *f_glutPopWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   glutPopWindow();
   return 0;
}

//void glutPushWindow(void);
static AbstractQoreNode *f_glutPushWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   glutPushWindow();
   return 0;
}

//void glutIconifyWindow(void);
static AbstractQoreNode *f_glutIconifyWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   glutIconifyWindow();
   return 0;
}

//void glutShowWindow(void);
static AbstractQoreNode *f_glutShowWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   glutShowWindow();
   return 0;
}

//void glutHideWindow(void);
static AbstractQoreNode *f_glutHideWindow(const QoreListNode *params, ExceptionSink *xsink)
{
   glutHideWindow();
   return 0;
}

//void glutFullScreen(void);
static AbstractQoreNode *f_glutFullScreen(const QoreListNode *params, ExceptionSink *xsink)
{
   glutFullScreen();
   return 0;
}

//void glutSetCursor(int cursor);
static AbstractQoreNode *f_glutSetCursor(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int cursor = p ? p->getAsInt() : 0;
   glutSetCursor(cursor);
   return 0;
}

//void glutWarpPointer(int x, int y);
static AbstractQoreNode *f_glutWarpPointer(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   glutWarpPointer(x, y);
   return 0;
}

#ifdef HAVE_GLUTSURFACETEXTURE
//void glutSurfaceTexture(GLenum target, GLenum internalformat, int surfacewin);
static AbstractQoreNode *f_glutSurfaceTexture(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum target = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   GLenum internalformat = (GLenum)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   int surfacewin = p ? p->getAsInt() : 0;
   glutSurfaceTexture(target, internalformat, surfacewin);
   return 0;
}
#endif

#ifdef HAVE_GLUTCHECKLOOP
//void glutCheckLoop(void);
static AbstractQoreNode *f_glutCheckLoop(const QoreListNode *params, ExceptionSink *xsink)
{
   glutCheckLoop();
   return 0;
}
#endif

//void glutEstablishOverlay(void);
static AbstractQoreNode *f_glutEstablishOverlay(const QoreListNode *params, ExceptionSink *xsink)
{
   glutEstablishOverlay();
   return 0;
}

//void glutRemoveOverlay(void);
static AbstractQoreNode *f_glutRemoveOverlay(const QoreListNode *params, ExceptionSink *xsink)
{
   glutRemoveOverlay();
   return 0;
}

//void glutUseLayer(GLenum layer);
static AbstractQoreNode *f_glutUseLayer(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum layer = (GLenum)(p ? p->getAsInt() : 0);
   glutUseLayer(layer);
   return 0;
}

//void glutPostOverlayRedisplay(void);
static AbstractQoreNode *f_glutPostOverlayRedisplay(const QoreListNode *params, ExceptionSink *xsink)
{
   glutPostOverlayRedisplay();
   return 0;
}

//void glutPostWindowOverlayRedisplay(int win);
static AbstractQoreNode *f_glutPostWindowOverlayRedisplay(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int win = p ? p->getAsInt() : 0;
   glutPostWindowOverlayRedisplay(win);
   return 0;
}

//void glutShowOverlay(void);
static AbstractQoreNode *f_glutShowOverlay(const QoreListNode *params, ExceptionSink *xsink)
{
   glutShowOverlay();
   return 0;
}

//void glutHideOverlay(void);
static AbstractQoreNode *f_glutHideOverlay(const QoreListNode *params, ExceptionSink *xsink)
{
   glutHideOverlay();
   return 0;
}

//int glutCreateMenu(void (*)(int));
static AbstractQoreNode *f_glutCreateMenu(const QoreListNode *params, ExceptionSink *xsink)
{
   const ResolvedCallReferenceNode *r = test_funcref_param(params, 0);

   if (!r) {
      xsink->raiseException("GLUTCREATEMENU-ERROR", "a call reference for the menu callback was expected as the sole argument to glutCreateMenu()");
      return 0;
   }

   int rc = glutCreateMenu(qore_glut_menu_callback);

   {
      AutoLocker al(&menu_map_lock);
      assert(menu_map.find(rc) == menu_map.end());
      menu_map[rc] = r->refRefSelf();
   }

   return new QoreBigIntNode(rc);
}

//void glutDestroyMenu(int menu);
static AbstractQoreNode *f_glutDestroyMenu(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int menu = p ? p->getAsInt() : 0;

   AutoLocker al(&menu_map_lock);
   menu_map_t::iterator i = menu_map.find(menu);
   if (i != menu_map.end()) {
      i->second->deref(xsink);
      menu_map.erase(i);
   }

   glutDestroyMenu(menu);
   return 0;
}

//int glutGetMenu(void);
static AbstractQoreNode *f_glutGetMenu(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(glutGetMenu());
}

//void glutSetMenu(int menu);
static AbstractQoreNode *f_glutSetMenu(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int menu = p ? p->getAsInt() : 0;
   glutSetMenu(menu);
   return 0;
}

//void glutAddMenuEntry(const char *label, int value);
static AbstractQoreNode *f_glutAddMenuEntry(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_STRING) {
      xsink->raiseException("GLUTADDMENUENTRY-PARAM-ERROR", "expecting a string as first argument to glutAddMenuEntry()");
      return 0;
   }
   const char *label = reinterpret_cast<const QoreStringNode *>(p)->getBuffer();
   p = get_param(params, 1);
   int value = p ? p->getAsInt() : 0;
   glutAddMenuEntry(label, value);
   return 0;
}

//void glutAddSubMenu(const char *label, int submenu);
static AbstractQoreNode *f_glutAddSubMenu(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_STRING) {
      xsink->raiseException("GLUTADDSUBMENU-PARAM-ERROR", "expecting a string as first argument to glutAddSubMenu()");
      return 0;
   }
   const char *label = reinterpret_cast<const QoreStringNode *>(p)->getBuffer();
   p = get_param(params, 1);
   int submenu = p ? p->getAsInt() : 0;
   glutAddSubMenu(label, submenu);
   return 0;
}

//void glutChangeToMenuEntry(int item, const char *label, int value);
static AbstractQoreNode *f_glutChangeToMenuEntry(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int item = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   if (!p || p->getType() != NT_STRING) {
      xsink->raiseException("GLUTCHANGETOMENUENTRY-PARAM-ERROR", "expecting a string as second argument to glutChangeToMenuEntry()");
      return 0;
   }
   const char *label = reinterpret_cast<const QoreStringNode *>(p)->getBuffer();
   p = get_param(params, 2);
   int value = p ? p->getAsInt() : 0;
   glutChangeToMenuEntry(item, label, value);
   return 0;
}

//void glutChangeToSubMenu(int item, const char *label, int submenu);
static AbstractQoreNode *f_glutChangeToSubMenu(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int item = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   if (!p || p->getType() != NT_STRING) {
      xsink->raiseException("GLUTCHANGETOSUBMENU-PARAM-ERROR", "expecting a string as second argument to glutChangeToSubMenu()");
      return 0;
   }
   const char *label = reinterpret_cast<const QoreStringNode *>(p)->getBuffer();
   p = get_param(params, 2);
   int submenu = p ? p->getAsInt() : 0;
   glutChangeToSubMenu(item, label, submenu);
   return 0;
}

//void glutRemoveMenuItem(int item);
static AbstractQoreNode *f_glutRemoveMenuItem(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int item = p ? p->getAsInt() : 0;
   glutRemoveMenuItem(item);
   return 0;
}

//void glutAttachMenu(int button);
static AbstractQoreNode *f_glutAttachMenu(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int button = p ? p->getAsInt() : 0;
   glutAttachMenu(button);
   return 0;
}

//void glutDetachMenu(int button);
static AbstractQoreNode *f_glutDetachMenu(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int button = p ? p->getAsInt() : 0;
   glutDetachMenu(button);
   return 0;
}

//void glutSetColor(int, GLfloat red, GLfloat green, GLfloat blue);
static AbstractQoreNode *f_glutSetColor(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   GLfloat red = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLfloat green = (GLfloat)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 3);
   GLfloat blue = (GLfloat)(p ? p->getAsFloat() : 0.0);
   glutSetColor(x, red, green, blue);
   return 0;
}

//GLfloat glutGetColor(int ndx, int component);
static AbstractQoreNode *f_glutGetColor(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int ndx = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int component = p ? p->getAsInt() : 0;
   return new QoreFloatNode(glutGetColor(ndx, component));
}

//void glutCopyColormap(int win);
static AbstractQoreNode *f_glutCopyColormap(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int win = p ? p->getAsInt() : 0;
   glutCopyColormap(win);
   return 0;
}

//int glutDeviceGet(GLenum type);
static AbstractQoreNode *f_glutDeviceGet(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   return new QoreBigIntNode(glutDeviceGet(type));
}

//int glutExtensionSupported(const char *name);
static AbstractQoreNode *f_glutExtensionSupported(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_STRING) {
      xsink->raiseException("GLUTEXTENSIONSUPPORTED-PARAM-ERROR", "expecting a string as first argument to glutExtensionSupported()");
      return 0;
   }
   const char *name = reinterpret_cast<const QoreStringNode *>(p)->getBuffer();
   return new QoreBigIntNode(glutExtensionSupported(name));
}

//int glutGetModifiers(void);
static AbstractQoreNode *f_glutGetModifiers(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(glutGetModifiers());
}

//int glutLayerGet(GLenum type);
static AbstractQoreNode *f_glutLayerGet(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum type = (GLenum)(p ? p->getAsInt() : 0);
   return new QoreBigIntNode(glutLayerGet(type));
}

#ifdef HAVE_GLUTGETPROCADDRESS
//void * glutGetProcAddress(const char *procName);
static AbstractQoreNode *f_glutGetProcAddress(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_STRING) {
      xsink->raiseException("GLUTGETPROCADDRESS-PARAM-ERROR", "expecting a string as first argument to glutGetProcAddress()");
      return 0;
   }
   const char *procName = reinterpret_cast<const QoreStringNode *>(p)->getBuffer();
   glutGetProcAddress(procName);
   return 0;
}
#endif

//void glutBitmapCharacter(void *font, int character);
static AbstractQoreNode *f_glutBitmapCharacter(const QoreListNode *params, ExceptionSink *xsink)
{
   const GlutVoidPtrType *ptr = test_glutptr_param(params, 0);
   if (!ptr) {
      xsink->raiseException("GLUTBITMAPCHARACTER-ERROR", "expecting a font pointer argument as the first argument to glutBitmapCharacter()");
      return 0;
   }

   const AbstractQoreNode *p = get_param(params, 1);
   int character = p ? p->getAsInt() : 0;

   glutBitmapCharacter(ptr->getPtr(), character);
   return 0;
}

//int glutBitmapWidth(void *font, int character);
static AbstractQoreNode *f_glutBitmapWidth(const QoreListNode *params, ExceptionSink *xsink)
{
   const GlutVoidPtrType *ptr = test_glutptr_param(params, 0);
   if (!ptr) {
      xsink->raiseException("GLUTBITMAPWIDTH-ERROR", "expecting a font pointer argument as the first argument to glutBitmapWidth()");
      return 0;
   }

   const AbstractQoreNode *p = get_param(params, 1);
   int character = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(glutBitmapWidth(ptr->getPtr(), character));
}

//void glutStrokeCharacter(void *font, int character);
static AbstractQoreNode *f_glutStrokeCharacter(const QoreListNode *params, ExceptionSink *xsink)
{
   const GlutVoidPtrType *ptr = test_glutptr_param(params, 0);
   if (!ptr) {
      xsink->raiseException("GLUTSTROKECHARACTER-ERROR", "expecting a font pointer argument as the first argument to glutStrokeCharacter()");
      return 0;
   }

   const AbstractQoreNode *p = get_param(params, 1);
   int character = p ? p->getAsInt() : 0;

   glutStrokeCharacter(ptr->getPtr(), character);
   return 0;
}

//int glutStrokeWidth(void *font, int character);
static AbstractQoreNode *f_glutStrokeWidth(const QoreListNode *params, ExceptionSink *xsink)
{
   const GlutVoidPtrType *ptr = test_glutptr_param(params, 0);
   if (!ptr) {
      xsink->raiseException("GLUTSTROKEWIDTH-ERROR", "expecting a font pointer argument as the first argument to glutStrokeWidth()");
      return 0;
   }

   const AbstractQoreNode *p = get_param(params, 1);
   int character = p ? p->getAsInt() : 0;

   return new QoreBigIntNode(glutStrokeWidth(ptr->getPtr(), character));
}

//int glutBitmapLength(void *font, const unsigned char *string);
static AbstractQoreNode *f_glutBitmapLength(const QoreListNode *params, ExceptionSink *xsink)
{
   const GlutVoidPtrType *ptr = test_glutptr_param(params, 0);
   if (!ptr) {
      xsink->raiseException("GLUTBITMAPLENGTH-ERROR", "expecting a font pointer argument as the first argument to glutBitmapLength()");
      return 0;
   }

   const QoreStringNode *str = test_string_param(params, 1);
   if (!str) {
      xsink->raiseException("GLUTBITMAPLENGTH-ERROR", "expecting a string argument as the second argument to glutBitmapLength()");
      return 0;
   }

   return new QoreBigIntNode(glutBitmapLength(ptr->getPtr(), (const unsigned char *)str->getBuffer()));
}

//int glutStrokeLength(void *font, const unsigned char *string);
static AbstractQoreNode *f_glutStrokeLength(const QoreListNode *params, ExceptionSink *xsink)
{
   const GlutVoidPtrType *ptr = test_glutptr_param(params, 0);
   if (!ptr) {
      xsink->raiseException("GLUTSTROKELENGTH-ERROR", "expecting a font pointer argument as the first argument to glutStrokeLength()");
      return 0;
   }

   const QoreStringNode *str = test_string_param(params, 1);
   if (!str) {
      xsink->raiseException("GLUTSTROKELENGTH-ERROR", "expecting a string argument as the second argument to glutStrokeLength()");
      return 0;
   }

   return new QoreBigIntNode(glutStrokeLength(ptr->getPtr(), (const unsigned char *)str->getBuffer()));
}

//void glutWireSphere(GLdouble radius, GLint slices, GLint stacks);
static AbstractQoreNode *f_glutWireSphere(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble radius = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLint slices = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint stacks = (GLint)(p ? p->getAsInt() : 0);
   glutWireSphere(radius, slices, stacks);
   return 0;
}

//void glutSolidSphere(GLdouble radius, GLint slices, GLint stacks);
static AbstractQoreNode *f_glutSolidSphere(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble radius = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLint slices = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   GLint stacks = (GLint)(p ? p->getAsInt() : 0);
   glutSolidSphere(radius, slices, stacks);
   return 0;
}

//void glutWireCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
static AbstractQoreNode *f_glutWireCone(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble base = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble height = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLint slices = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint stacks = (GLint)(p ? p->getAsInt() : 0);
   glutWireCone(base, height, slices, stacks);
   return 0;
}

//void glutSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
static AbstractQoreNode *f_glutSolidCone(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble base = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble height = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLint slices = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint stacks = (GLint)(p ? p->getAsInt() : 0);
   glutSolidCone(base, height, slices, stacks);
   return 0;
}

//void glutWireCube(GLdouble size);
static AbstractQoreNode *f_glutWireCube(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble size = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glutWireCube(size);
   return 0;
}

//void glutSolidCube(GLdouble size);
static AbstractQoreNode *f_glutSolidCube(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble size = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glutSolidCube(size);
   return 0;
}

//void glutWireTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
static AbstractQoreNode *f_glutWireTorus(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble innerRadius = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble outerRadius = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLint sides = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint rings = (GLint)(p ? p->getAsInt() : 0);
   glutWireTorus(innerRadius, outerRadius, sides, rings);
   return 0;
}

//void glutSolidTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
static AbstractQoreNode *f_glutSolidTorus(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble innerRadius = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 1);
   GLdouble outerRadius = (GLdouble)(p ? p->getAsFloat() : 0.0);
   p = get_param(params, 2);
   GLint sides = (GLint)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   GLint rings = (GLint)(p ? p->getAsInt() : 0);
   glutSolidTorus(innerRadius, outerRadius, sides, rings);
   return 0;
}

//void glutWireDodecahedron(void);
static AbstractQoreNode *f_glutWireDodecahedron(const QoreListNode *params, ExceptionSink *xsink)
{
   glutWireDodecahedron();
   return 0;
}

//void glutSolidDodecahedron(void);
static AbstractQoreNode *f_glutSolidDodecahedron(const QoreListNode *params, ExceptionSink *xsink)
{
   glutSolidDodecahedron();
   return 0;
}

//void glutWireTeapot(GLdouble size);
static AbstractQoreNode *f_glutWireTeapot(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble size = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glutWireTeapot(size);
   return 0;
}

//void glutSolidTeapot(GLdouble size);
static AbstractQoreNode *f_glutSolidTeapot(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLdouble size = (GLdouble)(p ? p->getAsFloat() : 0.0);
   glutSolidTeapot(size);
   return 0;
}

//void glutWireOctahedron(void);
static AbstractQoreNode *f_glutWireOctahedron(const QoreListNode *params, ExceptionSink *xsink)
{
   glutWireOctahedron();
   return 0;
}

//void glutSolidOctahedron(void);
static AbstractQoreNode *f_glutSolidOctahedron(const QoreListNode *params, ExceptionSink *xsink)
{
   glutSolidOctahedron();
   return 0;
}

//void glutWireTetrahedron(void);
static AbstractQoreNode *f_glutWireTetrahedron(const QoreListNode *params, ExceptionSink *xsink)
{
   glutWireTetrahedron();
   return 0;
}

//void glutSolidTetrahedron(void);
static AbstractQoreNode *f_glutSolidTetrahedron(const QoreListNode *params, ExceptionSink *xsink)
{
   glutSolidTetrahedron();
   return 0;
}

//void glutWireIcosahedron(void);
static AbstractQoreNode *f_glutWireIcosahedron(const QoreListNode *params, ExceptionSink *xsink)
{
   glutWireIcosahedron();
   return 0;
}

//void glutSolidIcosahedron(void);
static AbstractQoreNode *f_glutSolidIcosahedron(const QoreListNode *params, ExceptionSink *xsink)
{
   glutSolidIcosahedron();
   return 0;
}

//int glutVideoResizeGet(GLenum param);
static AbstractQoreNode *f_glutVideoResizeGet(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum param = (GLenum)(p ? p->getAsInt() : 0);
   return new QoreBigIntNode(glutVideoResizeGet(param));
}

//void glutSetupVideoResizing(void);
static AbstractQoreNode *f_glutSetupVideoResizing(const QoreListNode *params, ExceptionSink *xsink)
{
   glutSetupVideoResizing();
   return 0;
}

//void glutStopVideoResizing(void);
static AbstractQoreNode *f_glutStopVideoResizing(const QoreListNode *params, ExceptionSink *xsink)
{
   glutStopVideoResizing();
   return 0;
}

//void glutVideoResize(int x, int y, int width, int height);
static AbstractQoreNode *f_glutVideoResize(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   glutVideoResize(x, y, width, height);
   return 0;
}

//void glutVideoPan(int x, int y, int width, int height);
static AbstractQoreNode *f_glutVideoPan(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int width = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int height = p ? p->getAsInt() : 0;
   glutVideoPan(x, y, width, height);
   return 0;
}

//void glutReportErrors(void);
static AbstractQoreNode *f_glutReportErrors(const QoreListNode *params, ExceptionSink *xsink)
{
   glutReportErrors();
   return 0;
}

//void glutIgnoreKeyRepeat(int ignore);
static AbstractQoreNode *f_glutIgnoreKeyRepeat(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int ignore = p ? p->getAsInt() : 0;
   glutIgnoreKeyRepeat(ignore);
   return 0;
}

//void glutSetKeyRepeat(int repeatMode);
static AbstractQoreNode *f_glutSetKeyRepeat(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int repeatMode = p ? p->getAsInt() : 0;
   glutSetKeyRepeat(repeatMode);
   return 0;
}

//void glutGameModeString(const char *string);
static AbstractQoreNode *f_glutGameModeString(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (!p || p->getType() != NT_STRING) {
      xsink->raiseException("GLUTGAMEMODESTRING-PARAM-ERROR", "expecting a string as first argument to glutGameModeString()");
      return 0;
   }
   const char *string = reinterpret_cast<const QoreStringNode *>(p)->getBuffer();
   glutGameModeString(string);
   return 0;
}

//int glutEnterGameMode(void);
static AbstractQoreNode *f_glutEnterGameMode(const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(glutEnterGameMode());
}

//void glutLeaveGameMode(void);
static AbstractQoreNode *f_glutLeaveGameMode(const QoreListNode *params, ExceptionSink *xsink)
{
   glutLeaveGameMode();
   return 0;
}

//int glutGameModeGet(GLenum mode);
static AbstractQoreNode *f_glutGameModeGet(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   GLenum mode = (GLenum)(p ? p->getAsInt() : 0);
   return new QoreBigIntNode(glutGameModeGet(mode));
}

static QoreStringNode *glut_module_init()
{
   builtinFunctions.add("glutInit",                     f_glutInit, QDOM_GUI);
   builtinFunctions.add("glutInitDisplayMode",          f_glutInitDisplayMode, QDOM_GUI);
   builtinFunctions.add("glutInitWindowSize",           f_glutInitWindowSize, QDOM_GUI);
   builtinFunctions.add("glutInitWindowPosition",       f_glutInitWindowPosition, QDOM_GUI);
   builtinFunctions.add("glutCreateWindow",             f_glutCreateWindow, QDOM_GUI);
   builtinFunctions.add("glutPostRedisplay",            f_glutPostRedisplay, QDOM_GUI);
   builtinFunctions.add("glutPostWindowRedisplay",      f_glutPostWindowRedisplay, QDOM_GUI);
   builtinFunctions.add("glutMainLoop",                 f_glutMainLoop, QDOM_GUI);
   builtinFunctions.add("glutSwapBuffers",              f_glutSwapBuffers, QDOM_GUI);
   builtinFunctions.add("glutGet",                      f_glutGet, QDOM_GUI);

   builtinFunctions.add("glutReshapeFunc",              f_glutReshapeFunc, QDOM_GUI);
   builtinFunctions.add("glutDisplayFunc",              f_glutDisplayFunc, QDOM_GUI);
   builtinFunctions.add("glutKeyboardFunc",             f_glutKeyboardFunc, QDOM_GUI);
   builtinFunctions.add("glutVisibilityFunc",           f_glutVisibilityFunc, QDOM_GUI);
   builtinFunctions.add("glutIdleFunc",                 f_glutIdleFunc, QDOM_GUI);
   builtinFunctions.add("glutSpecialFunc",              f_glutSpecialFunc, QDOM_GUI);

   builtinFunctions.add("glutInitDisplayString",        f_glutInitDisplayString, QDOM_GUI);
   builtinFunctions.add("glutCreateSubWindow",          f_glutCreateSubWindow, QDOM_GUI);
   builtinFunctions.add("glutDestroyWindow",            f_glutDestroyWindow, QDOM_GUI);
   builtinFunctions.add("glutGetWindow",                f_glutGetWindow, QDOM_GUI);
   builtinFunctions.add("glutSetWindow",                f_glutSetWindow, QDOM_GUI);
   builtinFunctions.add("glutSetWindowTitle",           f_glutSetWindowTitle, QDOM_GUI);
   builtinFunctions.add("glutSetIconTitle",             f_glutSetIconTitle, QDOM_GUI);
   builtinFunctions.add("glutPositionWindow",           f_glutPositionWindow, QDOM_GUI);
   builtinFunctions.add("glutReshapeWindow",            f_glutReshapeWindow, QDOM_GUI);
   builtinFunctions.add("glutPopWindow",                f_glutPopWindow, QDOM_GUI);
   builtinFunctions.add("glutPushWindow",               f_glutPushWindow, QDOM_GUI);
   builtinFunctions.add("glutIconifyWindow",            f_glutIconifyWindow, QDOM_GUI);
   builtinFunctions.add("glutShowWindow",               f_glutShowWindow, QDOM_GUI);
   builtinFunctions.add("glutHideWindow",               f_glutHideWindow, QDOM_GUI);
   builtinFunctions.add("glutFullScreen",               f_glutFullScreen, QDOM_GUI);
   builtinFunctions.add("glutSetCursor",                f_glutSetCursor, QDOM_GUI);
   builtinFunctions.add("glutWarpPointer",              f_glutWarpPointer, QDOM_GUI);
#ifdef HAVE_GLUTSURFACETEXTURE
   builtinFunctions.add("glutSurfaceTexture",           f_glutSurfaceTexture, QDOM_GUI);
#endif
#ifdef HAVE_GLUTWMCLOSE
   builtinFunctions.add("glutWMCloseFunc",              f_glutWMCloseFunc, QDOM_GUI);
#endif
#ifdef HAVE_GLUTCHECKLOOP
   builtinFunctions.add("glutCheckLoop",                f_glutCheckLoop, QDOM_GUI);
#endif
   builtinFunctions.add("glutEstablishOverlay",         f_glutEstablishOverlay, QDOM_GUI);
   builtinFunctions.add("glutRemoveOverlay",            f_glutRemoveOverlay, QDOM_GUI);
   builtinFunctions.add("glutUseLayer",                 f_glutUseLayer, QDOM_GUI);
   builtinFunctions.add("glutPostOverlayRedisplay",     f_glutPostOverlayRedisplay, QDOM_GUI);
   builtinFunctions.add("glutPostWindowOverlayRedisplay", f_glutPostWindowOverlayRedisplay, QDOM_GUI);
   builtinFunctions.add("glutShowOverlay",              f_glutShowOverlay, QDOM_GUI);
   builtinFunctions.add("glutHideOverlay",              f_glutHideOverlay, QDOM_GUI);
   builtinFunctions.add("glutCreateMenu",               f_glutCreateMenu, QDOM_GUI);
   builtinFunctions.add("glutDestroyMenu",              f_glutDestroyMenu, QDOM_GUI);
   builtinFunctions.add("glutGetMenu",                  f_glutGetMenu, QDOM_GUI);
   builtinFunctions.add("glutSetMenu",                  f_glutSetMenu, QDOM_GUI);
   builtinFunctions.add("glutAddMenuEntry",             f_glutAddMenuEntry, QDOM_GUI);
   builtinFunctions.add("glutAddSubMenu",               f_glutAddSubMenu, QDOM_GUI);
   builtinFunctions.add("glutChangeToMenuEntry",        f_glutChangeToMenuEntry, QDOM_GUI);
   builtinFunctions.add("glutChangeToSubMenu",          f_glutChangeToSubMenu, QDOM_GUI);
   builtinFunctions.add("glutRemoveMenuItem",           f_glutRemoveMenuItem, QDOM_GUI);
   builtinFunctions.add("glutAttachMenu",               f_glutAttachMenu, QDOM_GUI);
   builtinFunctions.add("glutDetachMenu",               f_glutDetachMenu, QDOM_GUI);
   builtinFunctions.add("glutDisplayFunc",              f_glutDisplayFunc, QDOM_GUI);
   builtinFunctions.add("glutReshapeFunc",              f_glutReshapeFunc, QDOM_GUI);
   builtinFunctions.add("glutKeyboardFunc",             f_glutKeyboardFunc, QDOM_GUI);
   builtinFunctions.add("glutMouseFunc",                f_glutMouseFunc, QDOM_GUI);
   builtinFunctions.add("glutMotionFunc",               f_glutMotionFunc, QDOM_GUI);
   builtinFunctions.add("glutPassiveMotionFunc",        f_glutPassiveMotionFunc, QDOM_GUI);
   builtinFunctions.add("glutEntryFunc",                f_glutEntryFunc, QDOM_GUI);
   builtinFunctions.add("glutVisibilityFunc",           f_glutVisibilityFunc, QDOM_GUI);
   builtinFunctions.add("glutIdleFunc",                 f_glutIdleFunc, QDOM_GUI);
   builtinFunctions.add("glutTimerFunc",                f_glutTimerFunc, QDOM_GUI);
   builtinFunctions.add("glutMenuStateFunc",            f_glutMenuStateFunc, QDOM_GUI);
   builtinFunctions.add("glutSpecialFunc",              f_glutSpecialFunc, QDOM_GUI);
   builtinFunctions.add("glutSpaceballMotionFunc",      f_glutSpaceballMotionFunc, QDOM_GUI);
   builtinFunctions.add("glutSpaceballRotateFunc",      f_glutSpaceballRotateFunc, QDOM_GUI);
   builtinFunctions.add("glutSpaceballButtonFunc",      f_glutSpaceballButtonFunc, QDOM_GUI);
   builtinFunctions.add("glutButtonBoxFunc",            f_glutButtonBoxFunc, QDOM_GUI);
   builtinFunctions.add("glutDialsFunc",                f_glutDialsFunc, QDOM_GUI);
   builtinFunctions.add("glutTabletMotionFunc",         f_glutTabletMotionFunc, QDOM_GUI);
   builtinFunctions.add("glutTabletButtonFunc",         f_glutTabletButtonFunc, QDOM_GUI);
   builtinFunctions.add("glutMenuStatusFunc",           f_glutMenuStatusFunc, QDOM_GUI);
   builtinFunctions.add("glutOverlayDisplayFunc",       f_glutOverlayDisplayFunc, QDOM_GUI);
   builtinFunctions.add("glutWindowStatusFunc",         f_glutWindowStatusFunc, QDOM_GUI);
   builtinFunctions.add("glutKeyboardUpFunc",           f_glutKeyboardUpFunc, QDOM_GUI);
   builtinFunctions.add("glutSpecialUpFunc",            f_glutSpecialUpFunc, QDOM_GUI);
   builtinFunctions.add("glutJoystickFunc",             f_glutJoystickFunc, QDOM_GUI);
   builtinFunctions.add("glutSetColor",                 f_glutSetColor, QDOM_GUI);
   builtinFunctions.add("glutGetColor",                 f_glutGetColor, QDOM_GUI);
   builtinFunctions.add("glutCopyColormap",             f_glutCopyColormap, QDOM_GUI);
   builtinFunctions.add("glutDeviceGet",                f_glutDeviceGet, QDOM_GUI);
   builtinFunctions.add("glutExtensionSupported",       f_glutExtensionSupported, QDOM_GUI);
   builtinFunctions.add("glutGetModifiers",             f_glutGetModifiers, QDOM_GUI);
   builtinFunctions.add("glutLayerGet",                 f_glutLayerGet, QDOM_GUI);
#ifdef HAVE_GLUTGETPROCADDRESS
   builtinFunctions.add("glutGetProcAddress",           f_glutGetProcAddress, QDOM_GUI);
#endif
   builtinFunctions.add("glutBitmapCharacter",          f_glutBitmapCharacter, QDOM_GUI);
   builtinFunctions.add("glutBitmapWidth",              f_glutBitmapWidth, QDOM_GUI);
   builtinFunctions.add("glutStrokeCharacter",          f_glutStrokeCharacter, QDOM_GUI);
   builtinFunctions.add("glutStrokeWidth",              f_glutStrokeWidth, QDOM_GUI);
   builtinFunctions.add("glutBitmapLength",             f_glutBitmapLength, QDOM_GUI);
   builtinFunctions.add("glutStrokeLength",             f_glutStrokeLength, QDOM_GUI);
   builtinFunctions.add("glutWireSphere",               f_glutWireSphere, QDOM_GUI);
   builtinFunctions.add("glutSolidSphere",              f_glutSolidSphere, QDOM_GUI);
   builtinFunctions.add("glutWireCone",                 f_glutWireCone, QDOM_GUI);
   builtinFunctions.add("glutSolidCone",                f_glutSolidCone, QDOM_GUI);
   builtinFunctions.add("glutWireCube",                 f_glutWireCube, QDOM_GUI);
   builtinFunctions.add("glutSolidCube",                f_glutSolidCube, QDOM_GUI);
   builtinFunctions.add("glutWireTorus",                f_glutWireTorus, QDOM_GUI);
   builtinFunctions.add("glutSolidTorus",               f_glutSolidTorus, QDOM_GUI);
   builtinFunctions.add("glutWireDodecahedron",         f_glutWireDodecahedron, QDOM_GUI);
   builtinFunctions.add("glutSolidDodecahedron",        f_glutSolidDodecahedron, QDOM_GUI);
   builtinFunctions.add("glutWireTeapot",               f_glutWireTeapot, QDOM_GUI);
   builtinFunctions.add("glutSolidTeapot",              f_glutSolidTeapot, QDOM_GUI);
   builtinFunctions.add("glutWireOctahedron",           f_glutWireOctahedron, QDOM_GUI);
   builtinFunctions.add("glutSolidOctahedron",          f_glutSolidOctahedron, QDOM_GUI);
   builtinFunctions.add("glutWireTetrahedron",          f_glutWireTetrahedron, QDOM_GUI);
   builtinFunctions.add("glutSolidTetrahedron",         f_glutSolidTetrahedron, QDOM_GUI);
   builtinFunctions.add("glutWireIcosahedron",          f_glutWireIcosahedron, QDOM_GUI);
   builtinFunctions.add("glutSolidIcosahedron",         f_glutSolidIcosahedron, QDOM_GUI);
   builtinFunctions.add("glutVideoResizeGet",           f_glutVideoResizeGet, QDOM_GUI);
   builtinFunctions.add("glutSetupVideoResizing",       f_glutSetupVideoResizing, QDOM_GUI);
   builtinFunctions.add("glutStopVideoResizing",        f_glutStopVideoResizing, QDOM_GUI);
   builtinFunctions.add("glutVideoResize",              f_glutVideoResize, QDOM_GUI);
   builtinFunctions.add("glutVideoPan",                 f_glutVideoPan, QDOM_GUI);
   builtinFunctions.add("glutReportErrors",             f_glutReportErrors, QDOM_GUI);
   builtinFunctions.add("glutIgnoreKeyRepeat",          f_glutIgnoreKeyRepeat, QDOM_GUI);
   builtinFunctions.add("glutSetKeyRepeat",             f_glutSetKeyRepeat, QDOM_GUI);
   builtinFunctions.add("glutForceJoystickFunc",        f_glutForceJoystickFunc, QDOM_GUI);
   builtinFunctions.add("glutGameModeString",           f_glutGameModeString, QDOM_GUI);
   builtinFunctions.add("glutEnterGameMode",            f_glutEnterGameMode, QDOM_GUI);
   builtinFunctions.add("glutLeaveGameMode",            f_glutLeaveGameMode, QDOM_GUI);
   builtinFunctions.add("glutGameModeGet",              f_glutGameModeGet, QDOM_GUI);

   addGlutVoidPtrType();
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

