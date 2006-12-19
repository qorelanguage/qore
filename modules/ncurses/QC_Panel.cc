/*
  QC_Panel.cc

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

#include <qore/Qore.h>

#include "ncurses-module.h"
#include "QC_Panel.h"

int CID_PANEL;

//class LockedObject nc_panel_update_lock;

void PC_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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

   // ensure that the curses library has been initialized
   q_nc_init.init();
   class Panel *p = new Panel(lines, columns, y, x, xsink);
   if (xsink->isException())
      p->deref();
   self->setPrivate(CID_PANEL, p);
}

static void PC_copy(class Object *self, class Object *old, class Panel *p, class ExceptionSink *xsink)
{
   xsink->raiseException("PANEL-COPY-ERROR", "copying Panel objects is currently unsupported");
}

class QoreNode *PC_keypad(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = get_param(params, 0);
   bool b = p0 ? p0->getAsBool() : false;

   QoreNode *rv;
   int rc = p->keypad(b);
   rv = new QoreNode((int64)rc);
   return rv;
}

class QoreNode *PC_addstr(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   int rc;
   class QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (p0)
      rc = p->qaddstr(p0->val.String->getBuffer());
   else
      rc = 0;

   rv = new QoreNode((int64)rc);
   return rv;
}

class QoreNode *PC_mvaddstr(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   int rc;
   class QoreNode *p2 = test_param(params, NT_STRING, 2);
   if (p2)
   {
      class QoreNode *p0 = get_param(params, 0);
      class QoreNode *p1 = get_param(params, 1);
      rc = p->qmvaddstr(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0, p2->val.String->getBuffer());
   }
   else
      rc = 0;

   rv = new QoreNode((int64)rc);
   return rv;
}

class QoreNode *PC_printw(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   class QoreString *str = q_sprintf(params, 0, 0, xsink);
   rv = new QoreNode((int64)p->qaddstr(str->getBuffer()));
   delete str;
   return rv;
}

class QoreNode *PC_mvprintw(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   class QoreString *str = q_sprintf(params, 0, 2, xsink);
   class QoreNode *p0 = get_param(params, 0);
   class QoreNode *p1 = get_param(params, 1);

   rv = new QoreNode((int64)p->qmvaddstr(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0, str->getBuffer()));
   delete str;
   return rv;
}

class QoreNode *PC_refresh(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   p->qrefresh();
   return NULL;
}

class QoreNode *PC_update(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   p->update();
   return NULL;
}

class QoreNode *PC_getch(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   rv = new QoreNode((int64)p->qgetch());
   return rv;
}

class QoreNode *PC_border(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
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
   
   rv = new QoreNode((int64)p->qborder(ls, rs, ts, bs, tl, tr, bl, br));
   return rv;
}

class QoreNode *PC_setColor(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   QoreNode *p0 = get_param(params, 0);
   QoreNode *p1 = get_param(params, 1);
   rv = new QoreNode((int64)p->setColor(p0 ? p0->getAsInt() : COLOR_WHITE,
					p1 ? p1->getAsInt() : COLOR_BLACK));
   return rv;
}

class QoreNode *PC_setBackgroundColor(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   QoreNode *p0 = get_param(params, 0);
   rv = new QoreNode((int64)p->setBackgroundColor(p0 ? p0->getAsInt() : COLOR_BLACK));
   return rv;
}

class QoreNode *PC_setForegroundColor(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   QoreNode *p0 = get_param(params, 0);
   rv = new QoreNode((int64)p->setForegroundColor(p0 ? p0->getAsInt() : COLOR_WHITE));
   return rv;
}

class QoreNode *PC_show(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)p->show());
}

class QoreNode *PC_hide(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)p->hide());
}

class QoreNode *PC_movePanel(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   QoreNode *p1 = get_param(params, 1);
   return new QoreNode((int64)p->movePanel(p0 ? p0->getAsInt() : 0,
					   p1 ? p1->getAsInt() : 0));
}

class QoreNode *PC_move(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   QoreNode *p1 = get_param(params, 1);
   return new QoreNode((int64)p->qmove(p0 ? p0->getAsInt() : 0,
				       p1 ? p1->getAsInt() : 0));
}

class QoreNode *PC_top(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)p->top());
}

class QoreNode *PC_bottom(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)p->bottom());
}

class QoreNode *PC_getLines(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)p->getLines());
}

class QoreNode *PC_getColumns(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)p->getColumns());
}

#ifdef HAVE_WRESIZE
class QoreNode *PC_resize(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   QoreNode *p1 = get_param(params, 1);
   return new QoreNode((int64)p->resize(p0 ? p0->getAsInt() : 0,
					p1 ? p1->getAsInt() : 0));
}
#endif

class QoreNode *PC_getY(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)p->getY());
}

class QoreNode *PC_getX(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)p->getX());
}

class QoreNode *PC_getBegY(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)p->getBegY());
}

class QoreNode *PC_getBegX(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)p->getBegX());
}

class QoreNode *PC_attrset(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   return new QoreNode((int64)p->qattrset(p0 ? p0->getAsInt() : 0));
}

class QoreNode *PC_attron(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   return new QoreNode((int64)p->qattron(p0 ? p0->getAsInt() : 0));
}

class QoreNode *PC_attroff(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   return new QoreNode((int64)p->qattroff(p0 ? p0->getAsInt() : 0));
}

class QoreNode *PC_scrollok(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   return new QoreNode((int64)p->scrollok(p0 ? p0->getAsBool() : 0));
}


class QoreNode *PC_idlok(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   return new QoreNode((int64)p->idlok(p0 ? p0->getAsBool() : 0));
}

class QoreNode *PC_clearok(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   return new QoreNode((int64)p->clearok(p0 ? p0->getAsBool() : 0));
}

class QoreNode *PC_idcok(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   p->idcok(p0 ? p0->getAsBool() : 0);
   return NULL;
}

class QoreNode *PC_immedok(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   p->immedok(p0 ? p0->getAsBool() : 0);
   return NULL;
}

class QoreNode *PC_erase(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   p->qerase();
   return NULL;
}

class QoreNode *PC_clear(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   p->qclear();
   return NULL;
}

class QoreNode *PC_setscrreg(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   QoreNode *p1 = get_param(params, 1);
   return new QoreNode((int64)p->qsetscrreg(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
}

class QoreNode *PC_redraw(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)p->redraw());
}

class QoreNode *PC_scroll(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)p->qscroll());
}

class QoreNode *PC_scrl(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   return new QoreNode((int64)p->qscrl(p0 ? p0->getAsInt() : 0));
}

class QoreNode *PC_hline(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   QoreNode *p1 = get_param(params, 1);
   return new QoreNode((int64)p->qhline(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
}

class QoreNode *PC_vline(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   QoreNode *p1 = get_param(params, 1);
   return new QoreNode((int64)p->qvline(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
}

class QoreNode *PC_mvhline(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   QoreNode *p1 = get_param(params, 1);
   QoreNode *p2 = get_param(params, 2);
   QoreNode *p3 = get_param(params, 3);
   return new QoreNode((int64)p->qmvhline(p0 ? p0->getAsInt() : 0, 
					  p1 ? p1->getAsInt() : 0,
					  p2 ? p2->getAsInt() : 0,
					  p3 ? p3->getAsInt() : 0));
}

class QoreNode *PC_mvvline(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   QoreNode *p1 = get_param(params, 1);
   QoreNode *p2 = get_param(params, 2);
   QoreNode *p3 = get_param(params, 3);
   return new QoreNode((int64)p->qmvvline(p0 ? p0->getAsInt() : 0, 
					  p1 ? p1->getAsInt() : 0,
					  p2 ? p2->getAsInt() : 0,
					  p3 ? p3->getAsInt() : 0));
}

class QoreNode *PC_addch(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   QoreNode *p0 = get_param(params, 0);
   if (!is_nothing(p0) && (p0->type != NT_STRING || p0->val.String->strlen()))
      rv = new QoreNode((int64)p->qaddch(getChar(p0)));
   else
      rv = NULL;
   return rv;
}

class QoreNode *PC_mvaddch(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   QoreNode *p0 = get_param(params, 0);
   QoreNode *p1 = get_param(params, 1);
   QoreNode *p2 = get_param(params, 2);
   if (!is_nothing(p2) && (p2->type != NT_STRING || p2->val.String->strlen()))
      rv = new QoreNode((int64)p->qmvaddch(p0 ? p0->getAsInt() : 0,
					   p1 ? p1->getAsInt() : 0,
					   getChar(p2)));
   else
      rv = NULL;
   return rv;
}

class QoreNode *PC_clrtoeol(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   p->qclrtoeol();
   return NULL;
}

class QoreNode *PC_clrtobot(class Object *self, class Panel *p, class QoreNode *params, ExceptionSink *xsink)
{
   p->qclrtobot();
   return NULL;
}

class QoreClass *initPanelClass()
{
   tracein("initPanelClass()");

   class QoreClass *QC_PANEL = new QoreClass(strdup("Panel"));
   CID_PANEL = QC_PANEL->getID();
   QC_PANEL->setConstructor(PC_constructor);
   QC_PANEL->setCopy((q_copy_t)PC_copy);
   QC_PANEL->addMethod("keypad",             (q_method_t)PC_keypad);
   QC_PANEL->addMethod("mvaddstr",           (q_method_t)PC_mvaddstr);
   QC_PANEL->addMethod("mvprintw",           (q_method_t)PC_mvprintw);
   QC_PANEL->addMethod("addstr",             (q_method_t)PC_addstr);
   QC_PANEL->addMethod("printw",             (q_method_t)PC_printw);
   QC_PANEL->addMethod("refresh",            (q_method_t)PC_refresh);
   QC_PANEL->addMethod("update",             (q_method_t)PC_update);
   QC_PANEL->addMethod("getch",              (q_method_t)PC_getch);
   QC_PANEL->addMethod("border",             (q_method_t)PC_border);
   QC_PANEL->addMethod("setBackgroundColor", (q_method_t)PC_setBackgroundColor);
   QC_PANEL->addMethod("setForegroundColor", (q_method_t)PC_setForegroundColor);
   QC_PANEL->addMethod("setColor",           (q_method_t)PC_setColor);
   QC_PANEL->addMethod("getLines",           (q_method_t)PC_getLines);
   QC_PANEL->addMethod("getColumns",         (q_method_t)PC_getColumns);

   QC_PANEL->addMethod("show",               (q_method_t)PC_show);
   QC_PANEL->addMethod("hide",               (q_method_t)PC_hide);
   QC_PANEL->addMethod("top",                (q_method_t)PC_top);
   QC_PANEL->addMethod("bottom",             (q_method_t)PC_bottom);

#ifdef HAVE_WRESIZE
   QC_PANEL->addMethod("resize",             (q_method_t)PC_resize);
#endif
   QC_PANEL->addMethod("getY",               (q_method_t)PC_getY);
   QC_PANEL->addMethod("getX",               (q_method_t)PC_getX);
   QC_PANEL->addMethod("getBegY",            (q_method_t)PC_getBegY);
   QC_PANEL->addMethod("getBegX",            (q_method_t)PC_getBegX);

   QC_PANEL->addMethod("attrset",            (q_method_t)PC_attrset);
   QC_PANEL->addMethod("attron",             (q_method_t)PC_attron);
   QC_PANEL->addMethod("attroff",            (q_method_t)PC_attroff);
   QC_PANEL->addMethod("movePanel",          (q_method_t)PC_movePanel);
   QC_PANEL->addMethod("move",               (q_method_t)PC_move);
   QC_PANEL->addMethod("scrollok",           (q_method_t)PC_scrollok);
   QC_PANEL->addMethod("idlok",              (q_method_t)PC_idlok);
   QC_PANEL->addMethod("clearok",            (q_method_t)PC_clearok);
   QC_PANEL->addMethod("idcok",              (q_method_t)PC_idcok);
   QC_PANEL->addMethod("immedok",            (q_method_t)PC_immedok);
   QC_PANEL->addMethod("erase",              (q_method_t)PC_erase);
   QC_PANEL->addMethod("clear",              (q_method_t)PC_clear);
   QC_PANEL->addMethod("redraw",             (q_method_t)PC_redraw);
   QC_PANEL->addMethod("scroll",             (q_method_t)PC_scroll);
   QC_PANEL->addMethod("scrl",               (q_method_t)PC_scrl);
   QC_PANEL->addMethod("setscrreg",          (q_method_t)PC_setscrreg);
   QC_PANEL->addMethod("hline",              (q_method_t)PC_hline);
   QC_PANEL->addMethod("vline",              (q_method_t)PC_vline);
   QC_PANEL->addMethod("mvhline",            (q_method_t)PC_mvhline);
   QC_PANEL->addMethod("mvvline",            (q_method_t)PC_mvvline);
   QC_PANEL->addMethod("addch",              (q_method_t)PC_addch);
   QC_PANEL->addMethod("mvaddch",            (q_method_t)PC_mvaddch);

   QC_PANEL->addMethod("clrtoeol",           (q_method_t)PC_clrtoeol);
   QC_PANEL->addMethod("clrtobot",           (q_method_t)PC_clrtobot);

   traceout("initPanelClass()");
   return QC_PANEL;
}
