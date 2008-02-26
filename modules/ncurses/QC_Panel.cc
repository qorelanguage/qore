
/*
  QC_Panel.cc

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
#include "QC_Panel.h"

int CID_PANEL;

//class QoreThreadLock nc_panel_update_lock;

void PC_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
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
   class Panel *p = new Panel(lines, columns, y, x, xsink);
   if (xsink->isException())
      p->deref();
   self->setPrivate(CID_PANEL, p);
}

static void PC_copy(class QoreObject *self, class QoreObject *old, class Panel *p, class ExceptionSink *xsink)
{
   xsink->raiseException("PANEL-COPY-ERROR", "copying Panel objects is currently unsupported");
}

AbstractQoreNode *PC_keypad(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   bool b = p0 ? p0->getAsBool() : false;

   AbstractQoreNode *rv;
   int rc = p->keypad(b);
   rv = new QoreBigIntNode(rc);
   return rv;
}

AbstractQoreNode *PC_addstr(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *rv;
   int rc;
   const QoreStringNode *p0 = test_string_param(params, 0);
   if (p0)
      rc = p->qaddstr(p0->getBuffer());
   else
      rc = 0;

   rv = new QoreBigIntNode(rc);
   return rv;
}

AbstractQoreNode *PC_mvaddstr(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *rv;
   int rc;
   const QoreStringNode *p2 = test_string_param(params, 2);
   if (p2)
   {
      const AbstractQoreNode *p0 = get_param(params, 0);
      const AbstractQoreNode *p1 = get_param(params, 1);
      rc = p->qmvaddstr(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0, p2->getBuffer());
   }
   else
      rc = 0;

   rv = new QoreBigIntNode(rc);
   return rv;
}

AbstractQoreNode *PC_printw(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_sprintf(params, 0, 0, xsink));
   return new QoreBigIntNode(p->qaddstr(str->getBuffer()));
}

AbstractQoreNode *PC_mvprintw(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_sprintf(params, 0, 2, xsink));
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);

   return new QoreBigIntNode(p->qmvaddstr(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0, str->getBuffer()));
}

AbstractQoreNode *PC_refresh(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   p->qrefresh();
   return NULL;
}

AbstractQoreNode *PC_update(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   p->update();
   return NULL;
}

AbstractQoreNode *PC_getch(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *rv;
   rv = new QoreBigIntNode(p->qgetch());
   return rv;
}

AbstractQoreNode *PC_border(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *rv;
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
   
   rv = new QoreBigIntNode(p->qborder(ls, rs, ts, bs, tl, tr, bl, br));
   return rv;
}

AbstractQoreNode *PC_setColor(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *rv;
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   rv = new QoreBigIntNode(p->setColor(p0 ? p0->getAsInt() : COLOR_WHITE,
					p1 ? p1->getAsInt() : COLOR_BLACK));
   return rv;
}

AbstractQoreNode *PC_setBackgroundColor(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *rv;
   const AbstractQoreNode *p0 = get_param(params, 0);
   rv = new QoreBigIntNode(p->setBackgroundColor(p0 ? p0->getAsInt() : COLOR_BLACK));
   return rv;
}

AbstractQoreNode *PC_setForegroundColor(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *rv;
   const AbstractQoreNode *p0 = get_param(params, 0);
   rv = new QoreBigIntNode(p->setForegroundColor(p0 ? p0->getAsInt() : COLOR_WHITE));
   return rv;
}

AbstractQoreNode *PC_show(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->show());
}

AbstractQoreNode *PC_hide(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->hide());
}

AbstractQoreNode *PC_movePanel(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(p->movePanel(p0 ? p0->getAsInt() : 0,
					   p1 ? p1->getAsInt() : 0));
}

AbstractQoreNode *PC_move(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(p->qmove(p0 ? p0->getAsInt() : 0,
				       p1 ? p1->getAsInt() : 0));
}

AbstractQoreNode *PC_top(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->top());
}

AbstractQoreNode *PC_bottom(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->bottom());
}

AbstractQoreNode *PC_getLines(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->getLines());
}

AbstractQoreNode *PC_getColumns(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->getColumns());
}

#ifdef HAVE_WRESIZE
AbstractQoreNode *PC_resize(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(p->resize(p0 ? p0->getAsInt() : 0,
					p1 ? p1->getAsInt() : 0));
}
#endif

AbstractQoreNode *PC_getY(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->getY());
}

AbstractQoreNode *PC_getX(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->getX());
}

AbstractQoreNode *PC_getBegY(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->getBegY());
}

AbstractQoreNode *PC_getBegX(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->getBegX());
}

AbstractQoreNode *PC_attrset(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(p->qattrset(p0 ? p0->getAsInt() : 0));
}

AbstractQoreNode *PC_attron(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(p->qattron(p0 ? p0->getAsInt() : 0));
}

AbstractQoreNode *PC_attroff(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(p->qattroff(p0 ? p0->getAsInt() : 0));
}

AbstractQoreNode *PC_scrollok(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(p->scrollok(p0 ? p0->getAsBool() : 0));
}


AbstractQoreNode *PC_idlok(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(p->idlok(p0 ? p0->getAsBool() : 0));
}

AbstractQoreNode *PC_clearok(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(p->clearok(p0 ? p0->getAsBool() : 0));
}

AbstractQoreNode *PC_idcok(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   p->idcok(p0 ? p0->getAsBool() : 0);
   return NULL;
}

AbstractQoreNode *PC_immedok(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   p->immedok(p0 ? p0->getAsBool() : 0);
   return NULL;
}

AbstractQoreNode *PC_erase(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   p->qerase();
   return NULL;
}

AbstractQoreNode *PC_clear(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   p->qclear();
   return NULL;
}

AbstractQoreNode *PC_setscrreg(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(p->qsetscrreg(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
}

AbstractQoreNode *PC_redraw(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->redraw());
}

AbstractQoreNode *PC_scroll(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->qscroll());
}

AbstractQoreNode *PC_scrl(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   return new QoreBigIntNode(p->qscrl(p0 ? p0->getAsInt() : 0));
}

AbstractQoreNode *PC_hline(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(p->qhline(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
}

AbstractQoreNode *PC_vline(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   return new QoreBigIntNode(p->qvline(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
}

AbstractQoreNode *PC_mvhline(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   const AbstractQoreNode *p2 = get_param(params, 2);
   const AbstractQoreNode *p3 = get_param(params, 3);
   return new QoreBigIntNode(p->qmvhline(p0 ? p0->getAsInt() : 0, 
					  p1 ? p1->getAsInt() : 0,
					  p2 ? p2->getAsInt() : 0,
					  p3 ? p3->getAsInt() : 0));
}

AbstractQoreNode *PC_mvvline(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   const AbstractQoreNode *p2 = get_param(params, 2);
   const AbstractQoreNode *p3 = get_param(params, 3);
   return new QoreBigIntNode(p->qmvvline(p0 ? p0->getAsInt() : 0, 
					  p1 ? p1->getAsInt() : 0,
					  p2 ? p2->getAsInt() : 0,
					  p3 ? p3->getAsInt() : 0));
}

AbstractQoreNode *PC_addch(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   if (!is_nothing(p0) && (p0->getType() != NT_STRING || (reinterpret_cast<const QoreStringNode *>(p0))->strlen()))
      return new QoreBigIntNode(p->qaddch(getChar(p0)));
   return 0;
}

AbstractQoreNode *PC_mvaddch(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   const AbstractQoreNode *p2 = get_param(params, 2);
   if (!is_nothing(p2) && (p2->getType() != NT_STRING || (reinterpret_cast<const QoreStringNode *>(p2))->strlen()))
      return new QoreBigIntNode(p->qmvaddch(p0 ? p0->getAsInt() : 0,
					    p1 ? p1->getAsInt() : 0,
					    getChar(p2)));
   return 0;
}

AbstractQoreNode *PC_clrtoeol(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   p->qclrtoeol();
   return NULL;
}

AbstractQoreNode *PC_clrtobot(class QoreObject *self, class Panel *p, const QoreListNode *params, ExceptionSink *xsink)
{
   p->qclrtobot();
   return NULL;
}

class QoreClass *initPanelClass()
{
   tracein("initPanelClass()");

   class QoreClass *QC_PANEL = new QoreClass("Panel", QDOM_TERMINAL_IO);
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
