/*
  QC_Window.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/thread.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>
#include <qore/Exception.h>
#include <qore/QoreLib.h>

#include "ncurses-module.h"
#include "QC_Window.h"

int CID_WINDOW;

static inline void *getWindow(void *obj)
{
   ((Window *)obj)->ROreference();
   return obj;
}

class QoreNode *WC_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0, *p1, *p2, *p3;
   p0 = get_param(params, 0);
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);
   p3 = get_param(params, 3);
   int lines   = p0 ? p0->getAsInt() : 0;
   int columns = p1 ? p1->getAsInt() : 0;
   int y       = p2 ? p2->getAsInt() : 0;
   int x       = p3 ? p3->getAsInt() : 0;

   class Window *w;

   qore_ncurses_init();
   if (self->setPrivate(CID_WINDOW, w = new Window(lines, columns, y, x, xsink), getWindow) || xsink->isEvent())
      w->deref();
   return NULL;
}

class QoreNode *WC_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class Window *w = (Window *)self->getAndClearPrivateData(CID_WINDOW);
   if (w)
      w->deref();
   return NULL;
}

class QoreNode *WC_copy(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("WINDOW-COPY-ERROR", "copying Window objects is currently unsupported");
   return NULL;
}

class QoreNode *WC_keypad(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = get_param(params, 0);
   bool b = p0 ? p0->getAsBool() : false;

   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      int rc = w->keypad(b);
      w->deref();
      rv = new QoreNode((int64)rc);
   }
   else
   {
      alreadyDeleted(xsink, "Window::keypad");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_addstr(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      int rc;
      class QoreNode *p0 = test_param(params, NT_STRING, 0);
      if (p0)
	 rc = w->qaddstr(p0->val.String->getBuffer());
      else
	 rc = 0;

      w->deref();
      rv = new QoreNode((int64)rc);
   }
   else
   {
      alreadyDeleted(xsink, "Window::addstr");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_mvaddstr(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      int rc;
      class QoreNode *p2 = test_param(params, NT_STRING, 2);
      if (p2)
      {
	 class QoreNode *p0 = get_param(params, 0);
	 class QoreNode *p1 = get_param(params, 1);
	 rc = w->qmvaddstr(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0, p2->val.String->getBuffer());
      }
      else
	 rc = 0;

      w->deref();
      rv = new QoreNode((int64)rc);
   }
   else
   {
      alreadyDeleted(xsink, "Window::mvaddstr");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_printw(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      class QoreString *str = q_sprintf(params, 0, 0, xsink);
      rv = new QoreNode((int64)w->qaddstr(str->getBuffer()));
      delete str;

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::printw");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_mvprintw(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      class QoreString *str = q_sprintf(params, 0, 2, xsink);
      class QoreNode *p0 = get_param(params, 0);
      class QoreNode *p1 = get_param(params, 1);

      rv = new QoreNode((int64)w->qmvaddstr(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0, str->getBuffer()));
      delete str;
      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::mvprintw");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_refresh(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      rv = new QoreNode((int64)w->qrefresh());

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::refresh");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_getch(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      rv = new QoreNode((int64)w->qgetch());

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::getch");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_border(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      class QoreNode *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7;
      p0 = get_param(params, 0);
      p1 = get_param(params, 1);
      p2 = get_param(params, 2);
      p3 = get_param(params, 3);
      p4 = get_param(params, 4);
      p5 = get_param(params, 5);
      p6 = get_param(params, 6);
      p7 = get_param(params, 7);
      int ls = p0 ? p0->getAsInt() : 0;
      int rs = p1 ? p1->getAsInt() : 0;
      int ts = p2 ? p2->getAsInt() : 0;
      int bs = p3 ? p3->getAsInt() : 0;
      int tl = p4 ? p4->getAsInt() : 0;
      int tr = p5 ? p5->getAsInt() : 0;
      int bl = p6 ? p6->getAsInt() : 0;
      int br = p7 ? p7->getAsInt() : 0;

      rv = new QoreNode((int64)w->qborder(ls, rs, ts, bs, tl, tr, bl, br));

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::border");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_setColor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)w->setColor(p0 ? p0->getAsInt() : COLOR_WHITE,
					   p1 ? p1->getAsInt() : COLOR_BLACK));

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::setColor");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_setBackgroundColor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)w->setBackgroundColor(p0 ? p0->getAsInt() : COLOR_BLACK));

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::setBackgroundColor");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_setForegroundColor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)w->setForegroundColor(p0 ? p0->getAsInt() : COLOR_WHITE));

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::setForegroundColor");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_attrset(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)w->qattrset(p0 ? p0->getAsInt() : 0));

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::attrset");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_attron(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)w->qattron(p0 ? p0->getAsInt() : 0));

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::attron");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_attroff(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)w->qattroff(p0 ? p0->getAsInt() : 0));

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::attroff");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_moveWindow(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)w->moveWindow(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::moveWindow");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_move(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)w->qmove(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::move");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_scrollok(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)w->scrollok(p0 ? p0->getAsBool() : 0));

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::scrollok");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_idlok(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)w->idlok(p0 ? p0->getAsBool() : 0));

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::idlok");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_clearok(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)w->clearok(p0 ? p0->getAsBool() : 0));

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::clearok");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_idcok(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      w->idcok(p0 ? p0->getAsBool() : 0);

      w->deref();
   }
   else
      alreadyDeleted(xsink, "Window::idcok");

   return NULL;
}

class QoreNode *WC_immedok(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      w->immedok(p0 ? p0->getAsBool() : 0);

      w->deref();
   }
   else
      alreadyDeleted(xsink, "Window::immedok");

   return NULL;
}

class QoreNode *WC_erase(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   if (w)
   {
      w->qerase();
      w->deref();
   }
   else
      alreadyDeleted(xsink, "Window::erase");

   return NULL;
}

class QoreNode *WC_clear(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   if (w)
   {
      w->qclear();
      w->deref();
   }
   else
      alreadyDeleted(xsink, "Window::clear");

   return NULL;
}

class QoreNode *WC_setscrreg(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)w->qsetscrreg(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::setscrreg");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_redraw(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      rv = new QoreNode((int64)w->redraw());
      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::redraw");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_scroll(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      rv = new QoreNode((int64)w->qscroll());
      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::scroll");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_scrl(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)w->qscrl(p0 ? p0->getAsInt() : 0));

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::scrl");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_hline(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)w->qhline(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::hline");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_vline(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)w->qvline(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::vline");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_mvhline(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      QoreNode *p2 = get_param(params, 2);
      QoreNode *p3 = get_param(params, 3);
      rv = new QoreNode((int64)w->qmvhline(p0 ? p0->getAsInt() : 0, 
					   p1 ? p1->getAsInt() : 0,
					   p2 ? p2->getAsInt() : 0,
					   p3 ? p3->getAsInt() : 0));
      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::mvhline");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_mvvline(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      QoreNode *p2 = get_param(params, 2);
      QoreNode *p3 = get_param(params, 3);
      rv = new QoreNode((int64)w->qmvvline(p0 ? p0->getAsInt() : 0, 
					   p1 ? p1->getAsInt() : 0,
					   p2 ? p2->getAsInt() : 0,
					   p3 ? p3->getAsInt() : 0));
      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::mvvline");
      rv = NULL;
   }
   return rv;
}

static inline int getChar(QoreNode *p)
{
   if (p->type == NT_STRING)
      return p->val.String->getBuffer()[0];

   return p->getAsInt();
}

class QoreNode *WC_addch(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      if (!is_nothing(p0) && (p0->type != NT_STRING || p0->val.String->strlen()))
	 rv = new QoreNode((int64)w->qaddch(getChar(p0)));
      else
	 rv = NULL;
      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::addch");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_mvaddch(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      QoreNode *p2 = get_param(params, 2);
      if (!is_nothing(p2) && (p2->type != NT_STRING || p2->val.String->strlen()))
	 rv = new QoreNode((int64)w->qmvaddch(p0 ? p0->getAsInt() : 0,
					      p1 ? p1->getAsInt() : 0,
					      getChar(p2)));
      else
	 rv = NULL;
      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::mvaddch");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_clrtoeol(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   if (w)
   {
      w->qclrtoeol();
      w->deref();
   }
   else
      alreadyDeleted(xsink, "Window::clrtoeol");

   return NULL;
}

class QoreNode *WC_clrtobot(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   if (w)
   {
      w->qclrtobot();
      w->deref();
   }
   else
      alreadyDeleted(xsink, "Window::clrtobot");

   return NULL;
}

class QoreNode *WC_getLines(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      rv = new QoreNode((int64)w->getLines());

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::getLines");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_getColumns(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      rv = new QoreNode((int64)w->getColumns());

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::getColumns");
      rv = NULL;
   }
   return rv;
}

#ifdef HAVE_WRESIZE
class QoreNode *WC_resize(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *p = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)p->resize(p0 ? p0->getAsInt() : 0,
					 p1 ? p1->getAsInt() : 0));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::resize");
      rv = NULL;
   }
   return rv;
}
#endif

class QoreNode *WC_getY(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *p = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->getY());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::getY");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_getX(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *p = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->getX());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::getX");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_getBegY(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *p = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->getBegY());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::getY");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_getBegX(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *p = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->getBegX());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::getX");
      rv = NULL;
   }
   return rv;
}

class QoreNode *WC_nodelay(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Window *w = (Window *)self->getReferencedPrivateData(CID_WINDOW);
   QoreNode *rv;
   if (w)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)w->nodelay(p0 ? p0->getAsBool() : 0));

      w->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Window::nodelay");
      rv = NULL;
   }
   return rv;
}

class QoreClass *initWindowClass()
{
   tracein("initWindowClass()");

   class QoreClass *QC_WINDOW = new QoreClass(strdup("Window"));
   CID_WINDOW = QC_WINDOW->getID();
   QC_WINDOW->addMethod("constructor",        WC_constructor);
   QC_WINDOW->addMethod("destructor",         WC_destructor);
   QC_WINDOW->addMethod("copy",               WC_copy);
   QC_WINDOW->addMethod("keypad",             WC_keypad);
   QC_WINDOW->addMethod("mvaddstr",           WC_mvaddstr);
   QC_WINDOW->addMethod("mvprintw",           WC_mvprintw);
   QC_WINDOW->addMethod("addstr",             WC_addstr);
   QC_WINDOW->addMethod("printw",             WC_printw);
   QC_WINDOW->addMethod("refresh",            WC_refresh);
   QC_WINDOW->addMethod("getch",              WC_getch);
   QC_WINDOW->addMethod("border",             WC_border);
   QC_WINDOW->addMethod("setBackgroundColor", WC_setBackgroundColor);
   QC_WINDOW->addMethod("setForegroundColor", WC_setForegroundColor);
   QC_WINDOW->addMethod("setColor",           WC_setColor);
   QC_WINDOW->addMethod("getLines",           WC_getLines);
   QC_WINDOW->addMethod("getColumns",         WC_getColumns);
   QC_WINDOW->addMethod("attrset",            WC_attrset);
   QC_WINDOW->addMethod("attron",             WC_attron);
   QC_WINDOW->addMethod("attroff",            WC_attroff);
   QC_WINDOW->addMethod("moveWindow",         WC_moveWindow);
   QC_WINDOW->addMethod("move",               WC_move);
   QC_WINDOW->addMethod("scrollok",           WC_scrollok);
   QC_WINDOW->addMethod("idlok",              WC_idlok);
   QC_WINDOW->addMethod("clearok",            WC_clearok);
   QC_WINDOW->addMethod("idcok",              WC_idcok);
   QC_WINDOW->addMethod("immedok",            WC_immedok);
   QC_WINDOW->addMethod("erase",              WC_erase);
   QC_WINDOW->addMethod("clear",              WC_clear);
   QC_WINDOW->addMethod("redraw",             WC_redraw);
   QC_WINDOW->addMethod("scroll",             WC_scroll);
   QC_WINDOW->addMethod("scrl",               WC_scrl);
   QC_WINDOW->addMethod("setscrreg",          WC_setscrreg);
   QC_WINDOW->addMethod("hline",              WC_hline);
   QC_WINDOW->addMethod("vline",              WC_vline);
   QC_WINDOW->addMethod("mvhline",            WC_mvhline);
   QC_WINDOW->addMethod("mvvline",            WC_mvvline);
   QC_WINDOW->addMethod("addch",              WC_addch);
   QC_WINDOW->addMethod("mvaddch",            WC_mvaddch);
   QC_WINDOW->addMethod("clrtoeol",           WC_clrtoeol);
   QC_WINDOW->addMethod("clrtobot",           WC_clrtobot);
#ifdef HAVE_WRESIZE
   QC_WINDOW->addMethod("resize",             WC_resize);
#endif
   QC_WINDOW->addMethod("getY",               WC_getY);
   QC_WINDOW->addMethod("getX",               WC_getX);
   QC_WINDOW->addMethod("getBegY",            WC_getBegY);
   QC_WINDOW->addMethod("getBegX",            WC_getBegX);
   QC_WINDOW->addMethod("nodelay",            WC_nodelay);

   traceout("initWindowClass()");
   return QC_WINDOW;
}
