/*
  QC_Window.cc

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

#include "ncurses-module.h"
#include "QC_Window.h"

int CID_WINDOW;

void WC_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0, *p1, *p2, *p3;
   p0 = get_param(params, 0);
   p1 = get_param(params, 1);
   p2 = get_param(params, 2);
   p3 = get_param(params, 3);
   int lines   = p0 ? p0->getAsInt() : 0;
   int columns = p1 ? p1->getAsInt() : 0;
   int y       = p2 ? p2->getAsInt() : 0;
   int x       = p3 ? p3->getAsInt() : 0;

   // ensure that the curses library has been initialized
   q_nc_init.init();
   class Window *w = new Window(lines, columns, y, x, xsink);
   if (xsink->isException())
      w->deref();
   self->setPrivate(CID_WINDOW, w);      
}

static void WC_copy(class QoreObject *self, class QoreObject *old, class Window *w, class ExceptionSink *xsink)
{
   xsink->raiseException("WINDOW-COPY-ERROR", "copying Window objects is currently unsupported");
}

class AbstractQoreNode *WC_keypad(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   bool b = p0 ? p0->getAsBool() : false;

   return new QoreBigIntNode(w->keypad(b));
}

class AbstractQoreNode *WC_addstr(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   int rc;
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (p0)
      rc = w->qaddstr(p0->getBuffer());
   else
      rc = 0;
   
   return new QoreBigIntNode(rc);
}

class AbstractQoreNode *WC_mvaddstr(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   int rc;
   const QoreStringNode *p2 = test_string_param(params, 2);
   if (p2)
   {
      const AbstractQoreNode *p0 = get_param(params, 0);
      const AbstractQoreNode *p1 = get_param(params, 1);
      rc = w->qmvaddstr(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0, p2->getBuffer());
   }
   else
      rc = 0;
   
   return new QoreBigIntNode(rc);
}

class AbstractQoreNode *WC_printw(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   TempQoreStringNode str(q_sprintf(params, 0, 0, xsink));
   return new QoreBigIntNode(w->qaddstr(str->getBuffer()));
}

class AbstractQoreNode *WC_mvprintw(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   TempQoreStringNode str(q_sprintf(params, 0, 2, xsink));
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);

   return new QoreBigIntNode(w->qmvaddstr(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0, str->getBuffer()));
}

class AbstractQoreNode *WC_refresh(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(w->qrefresh());
}

class AbstractQoreNode *WC_getch(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(w->qgetch());
}

class AbstractQoreNode *WC_border(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7;
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

   return new QoreBigIntNode(w->qborder(ls, rs, ts, bs, tl, tr, bl, br));
}

class AbstractQoreNode *WC_setColor(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(w->setColor(p0 ? p0->getAsInt() : COLOR_WHITE,
					  p1 ? p1->getAsInt() : COLOR_BLACK));
}

class AbstractQoreNode *WC_setBackgroundColor(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(w->setBackgroundColor(p0 ? p0->getAsInt() : COLOR_BLACK));
}

class AbstractQoreNode *WC_setForegroundColor(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(w->setForegroundColor(p0 ? p0->getAsInt() : COLOR_WHITE));
}

class AbstractQoreNode *WC_attrset(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(w->qattrset(p0 ? p0->getAsInt() : 0));
}

class AbstractQoreNode *WC_attron(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(w->qattron(p0 ? p0->getAsInt() : 0));
}

class AbstractQoreNode *WC_attroff(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(w->qattroff(p0 ? p0->getAsInt() : 0));
}

class AbstractQoreNode *WC_moveWindow(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(w->moveWindow(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
}

class AbstractQoreNode *WC_move(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(w->qmove(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
}

class AbstractQoreNode *WC_scrollok(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(w->scrollok(p0 ? p0->getAsBool() : 0));
}

class AbstractQoreNode *WC_idlok(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(w->idlok(p0 ? p0->getAsBool() : 0));
}

class AbstractQoreNode *WC_clearok(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(w->clearok(p0 ? p0->getAsBool() : 0));
}

class AbstractQoreNode *WC_idcok(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   w->idcok(p0 ? p0->getAsBool() : 0);
   return NULL;
}

class AbstractQoreNode *WC_immedok(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   w->immedok(p0 ? p0->getAsBool() : 0);
   return NULL;
}

class AbstractQoreNode *WC_erase(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   w->qerase();
   return NULL;
}

class AbstractQoreNode *WC_clear(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   w->qclear();
   return NULL;
}

class AbstractQoreNode *WC_setscrreg(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(w->qsetscrreg(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
}

class AbstractQoreNode *WC_redraw(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(w->redraw());
}

class AbstractQoreNode *WC_scroll(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(w->qscroll());
}

class AbstractQoreNode *WC_scrl(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(w->qscrl(p0 ? p0->getAsInt() : 0));
}

class AbstractQoreNode *WC_hline(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(w->qhline(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
}

class AbstractQoreNode *WC_vline(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(w->qvline(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
}

class AbstractQoreNode *WC_mvhline(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   const AbstractQoreNode *p2 = get_param(params, 2);
   const AbstractQoreNode *p3 = get_param(params, 3);
   return new QoreBigIntNode(w->qmvhline(p0 ? p0->getAsInt() : 0, 
					  p1 ? p1->getAsInt() : 0,
					  p2 ? p2->getAsInt() : 0,
					  p3 ? p3->getAsInt() : 0));
}

class AbstractQoreNode *WC_mvvline(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   const AbstractQoreNode *p2 = get_param(params, 2);
   const AbstractQoreNode *p3 = get_param(params, 3);
   return new QoreBigIntNode(w->qmvvline(p0 ? p0->getAsInt() : 0, 
					  p1 ? p1->getAsInt() : 0,
					  p2 ? p2->getAsInt() : 0,
					  p3 ? p3->getAsInt() : 0));
}

class AbstractQoreNode *WC_addch(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   
   if (!is_nothing(p0) && (p0->getType() != NT_STRING || (reinterpret_cast<const QoreStringNode *>(p0))->strlen()))
      return new QoreBigIntNode(w->qaddch(getChar(p0)));
   return 0;
}

class AbstractQoreNode *WC_mvaddch(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   const AbstractQoreNode *p2 = get_param(params, 2);
   if (!is_nothing(p2) && (p2->getType() != NT_STRING || (reinterpret_cast<const QoreStringNode *>(p2))->strlen()))
      return new QoreBigIntNode(w->qmvaddch(p0 ? p0->getAsInt() : 0,
					    p1 ? p1->getAsInt() : 0,
					    getChar(p2)));
   return 0;
}

class AbstractQoreNode *WC_clrtoeol(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   w->qclrtoeol();
   return NULL;
}

class AbstractQoreNode *WC_clrtobot(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   w->qclrtobot();
   return NULL;
}

class AbstractQoreNode *WC_getLines(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(w->getLines());
}

class AbstractQoreNode *WC_getColumns(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(w->getColumns());
}

#ifdef HAVE_WRESIZE
class AbstractQoreNode *WC_resize(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(w->resize(p0 ? p0->getAsInt() : 0,
				       p1 ? p1->getAsInt() : 0));
}
#endif

class AbstractQoreNode *WC_getY(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(w->getY());
}

class AbstractQoreNode *WC_getX(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(w->getX());
}

class AbstractQoreNode *WC_getBegY(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(w->getBegY());
}

class AbstractQoreNode *WC_getBegX(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(w->getBegX());
}

class AbstractQoreNode *WC_nodelay(class QoreObject *self, class Window *w, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(w->nodelay(p0 ? p0->getAsBool() : 0));
}

class QoreClass *initWindowClass()
{
   tracein("initWindowClass()");

   class QoreClass *QC_WINDOW = new QoreClass("Window");
   CID_WINDOW = QC_WINDOW->getID();
   QC_WINDOW->setConstructor(WC_constructor);
   QC_WINDOW->setCopy((q_copy_t)WC_copy);
   QC_WINDOW->addMethod("keypad",             (q_method_t)WC_keypad);
   QC_WINDOW->addMethod("mvaddstr",           (q_method_t)WC_mvaddstr);
   QC_WINDOW->addMethod("mvprintw",           (q_method_t)WC_mvprintw);
   QC_WINDOW->addMethod("addstr",             (q_method_t)WC_addstr);
   QC_WINDOW->addMethod("printw",             (q_method_t)WC_printw);
   QC_WINDOW->addMethod("refresh",            (q_method_t)WC_refresh);
   QC_WINDOW->addMethod("getch",              (q_method_t)WC_getch);
   QC_WINDOW->addMethod("border",             (q_method_t)WC_border);
   QC_WINDOW->addMethod("setBackgroundColor", (q_method_t)WC_setBackgroundColor);
   QC_WINDOW->addMethod("setForegroundColor", (q_method_t)WC_setForegroundColor);
   QC_WINDOW->addMethod("setColor",           (q_method_t)WC_setColor);
   QC_WINDOW->addMethod("getLines",           (q_method_t)WC_getLines);
   QC_WINDOW->addMethod("getColumns",         (q_method_t)WC_getColumns);
   QC_WINDOW->addMethod("attrset",            (q_method_t)WC_attrset);
   QC_WINDOW->addMethod("attron",             (q_method_t)WC_attron);
   QC_WINDOW->addMethod("attroff",            (q_method_t)WC_attroff);
   QC_WINDOW->addMethod("moveWindow",         (q_method_t)WC_moveWindow);
   QC_WINDOW->addMethod("move",               (q_method_t)WC_move);
   QC_WINDOW->addMethod("scrollok",           (q_method_t)WC_scrollok);
   QC_WINDOW->addMethod("idlok",              (q_method_t)WC_idlok);
   QC_WINDOW->addMethod("clearok",            (q_method_t)WC_clearok);
   QC_WINDOW->addMethod("idcok",              (q_method_t)WC_idcok);
   QC_WINDOW->addMethod("immedok",            (q_method_t)WC_immedok);
   QC_WINDOW->addMethod("erase",              (q_method_t)WC_erase);
   QC_WINDOW->addMethod("clear",              (q_method_t)WC_clear);
   QC_WINDOW->addMethod("redraw",             (q_method_t)WC_redraw);
   QC_WINDOW->addMethod("scroll",             (q_method_t)WC_scroll);
   QC_WINDOW->addMethod("scrl",               (q_method_t)WC_scrl);
   QC_WINDOW->addMethod("setscrreg",          (q_method_t)WC_setscrreg);
   QC_WINDOW->addMethod("hline",              (q_method_t)WC_hline);
   QC_WINDOW->addMethod("vline",              (q_method_t)WC_vline);
   QC_WINDOW->addMethod("mvhline",            (q_method_t)WC_mvhline);
   QC_WINDOW->addMethod("mvvline",            (q_method_t)WC_mvvline);
   QC_WINDOW->addMethod("addch",              (q_method_t)WC_addch);
   QC_WINDOW->addMethod("mvaddch",            (q_method_t)WC_mvaddch);
   QC_WINDOW->addMethod("clrtoeol",           (q_method_t)WC_clrtoeol);
   QC_WINDOW->addMethod("clrtobot",           (q_method_t)WC_clrtobot);
#ifdef HAVE_WRESIZE
   QC_WINDOW->addMethod("resize",             (q_method_t)WC_resize);
#endif
   QC_WINDOW->addMethod("getY",               (q_method_t)WC_getY);
   QC_WINDOW->addMethod("getX",               (q_method_t)WC_getX);
   QC_WINDOW->addMethod("getBegY",            (q_method_t)WC_getBegY);
   QC_WINDOW->addMethod("getBegX",            (q_method_t)WC_getBegX);
   QC_WINDOW->addMethod("nodelay",            (q_method_t)WC_nodelay);

   traceout("initWindowClass()");
   return QC_WINDOW;
}
