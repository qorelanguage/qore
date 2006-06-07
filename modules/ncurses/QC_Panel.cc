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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/thread.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>
#include <qore/Exception.h>
#include <qore/QoreLib.h>

#include "ncurses-module.h"
#include "QC_Panel.h"

int CID_PANEL;

//class LockedObject nc_panel_update_lock;

static inline void *getPanel(void *obj)
{
   ((Panel *)obj)->ROreference();
   return obj;
}

class QoreNode *PC_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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

   class Panel *p;

   qore_ncurses_init();
   if (self->setPrivate(CID_PANEL, p = new Panel(lines, columns, y, x, xsink), getPanel) || xsink->isEvent())
      p->deref();
   return NULL;
}

class QoreNode *PC_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class Panel *p = (Panel *)self->getAndClearPrivateData(CID_PANEL);
   if (p)
      p->deref();
   return NULL;
}

class QoreNode *PC_copy(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("PANEL-COPY-ERROR", "copying Panel objects is currently unsupported");
   return NULL;
}

class QoreNode *PC_keypad(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0 = get_param(params, 0);
   bool b = p0 ? p0->getAsBool() : false;

   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      int rc = p->keypad(b);
      p->deref();
      rv = new QoreNode((int64)rc);
   }
   else
   {
      alreadyDeleted(xsink, "Panel::keypad");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_addstr(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      int rc;
      class QoreNode *p0 = test_param(params, NT_STRING, 0);
      if (p0)
	 rc = p->qaddstr(p0->val.String->getBuffer());
      else
	 rc = 0;

      p->deref();
      rv = new QoreNode((int64)rc);
   }
   else
   {
      alreadyDeleted(xsink, "Panel::addstr");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_mvaddstr(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
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

      p->deref();
      rv = new QoreNode((int64)rc);
   }
   else
   {
      alreadyDeleted(xsink, "Panel::mvaddstr");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_printw(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      class QoreString *str = q_sprintf(params, 0, 0, xsink);
      rv = new QoreNode((int64)p->qaddstr(str->getBuffer()));
      delete str;

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::printw");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_mvprintw(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      class QoreString *str = q_sprintf(params, 0, 2, xsink);
      class QoreNode *p0 = get_param(params, 0);
      class QoreNode *p1 = get_param(params, 1);

      rv = new QoreNode((int64)p->qmvaddstr(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0, str->getBuffer()));
      delete str;
      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::mvprintw");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_refresh(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   if (p)
   {
      p->qrefresh();
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Panel::refresh");

   return NULL;
}

class QoreNode *PC_update(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   if (p)
   {
      p->update();
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Panel::update");

   return NULL;
}

class QoreNode *PC_getch(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->qgetch());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::getch");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_border(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
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

      rv = new QoreNode((int64)p->qborder(ls, rs, ts, bs, tl, tr, bl, br));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::border");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_setColor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)p->setColor(p0 ? p0->getAsInt() : COLOR_WHITE,
					   p1 ? p1->getAsInt() : COLOR_BLACK));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::setColor");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_setBackgroundColor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)p->setBackgroundColor(p0 ? p0->getAsInt() : COLOR_BLACK));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::setBackgroundColor");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_setForegroundColor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)p->setForegroundColor(p0 ? p0->getAsInt() : COLOR_WHITE));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::setForegroundColor");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_show(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->show());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::show");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_hide(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->hide());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::hide");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_movePanel(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)p->movePanel(p0 ? p0->getAsInt() : 0,
					    p1 ? p1->getAsInt() : 0));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::movePanel");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_move(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)p->qmove(p0 ? p0->getAsInt() : 0,
					p1 ? p1->getAsInt() : 0));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::move");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_top(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->top());
      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::top");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_bottom(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->bottom());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::bottom");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_getLines(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->getLines());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::getLines");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_getColumns(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->getColumns());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::getColumns");
      rv = NULL;
   }
   return rv;
}

#ifdef HAVE_WRESIZE
class QoreNode *PC_resize(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
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
      alreadyDeleted(xsink, "Panel::resize");
      rv = NULL;
   }
   return rv;
}
#endif

class QoreNode *PC_getY(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->getY());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::getY");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_getX(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->getX());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::getX");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_getBegY(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->getBegY());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::getBegY");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_getBegX(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->getBegX());

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::getBegX");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_attrset(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)p->qattrset(p0 ? p0->getAsInt() : 0));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::attrset");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_attron(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)p->qattron(p0 ? p0->getAsInt() : 0));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::attron");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_attroff(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)p->qattroff(p0 ? p0->getAsInt() : 0));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::attroff");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_scrollok(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)p->scrollok(p0 ? p0->getAsBool() : 0));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::scrollok");
      rv = NULL;
   }
   return rv;
}


class QoreNode *PC_idlok(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)p->idlok(p0 ? p0->getAsBool() : 0));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::idlok");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_clearok(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)p->clearok(p0 ? p0->getAsBool() : 0));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::clearok");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_idcok(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      p->idcok(p0 ? p0->getAsBool() : 0);

      p->deref();
   }
   else
      alreadyDeleted(xsink, "Panel::idcok");

   return NULL;
}

class QoreNode *PC_immedok(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      p->immedok(p0 ? p0->getAsBool() : 0);

      p->deref();
   }
   else
      alreadyDeleted(xsink, "Panel::immedok");

   return NULL;
}

class QoreNode *PC_erase(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   if (p)
   {
      p->qerase();
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Panel::erase");

   return NULL;
}

class QoreNode *PC_clear(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   if (p)
   {
      p->qclear();
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Panel::clear");

   return NULL;
}

class QoreNode *PC_setscrreg(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)p->qsetscrreg(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::setscrreg");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_redraw(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->redraw());
      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::redraw");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_scroll(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      rv = new QoreNode((int64)p->qscroll());
      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::scroll");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_scrl(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      rv = new QoreNode((int64)p->qscrl(p0 ? p0->getAsInt() : 0));

      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::scrl");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_hline(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)p->qhline(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::hline");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_vline(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      rv = new QoreNode((int64)p->qvline(p0 ? p0->getAsInt() : 0, p1 ? p1->getAsInt() : 0));
      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::vline");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_mvhline(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      QoreNode *p2 = get_param(params, 2);
      QoreNode *p3 = get_param(params, 3);
      rv = new QoreNode((int64)p->qmvhline(p0 ? p0->getAsInt() : 0, 
					   p1 ? p1->getAsInt() : 0,
					   p2 ? p2->getAsInt() : 0,
					   p3 ? p3->getAsInt() : 0));
      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::mvhline");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_mvvline(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      QoreNode *p2 = get_param(params, 2);
      QoreNode *p3 = get_param(params, 3);
      rv = new QoreNode((int64)p->qmvvline(p0 ? p0->getAsInt() : 0, 
					   p1 ? p1->getAsInt() : 0,
					   p2 ? p2->getAsInt() : 0,
					   p3 ? p3->getAsInt() : 0));
      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::mvvline");
      rv = NULL;
   }
   return rv;
}

static inline unsigned int getChar(QoreNode *p)
{
   if (p->type == NT_STRING)
      return p->val.String->getBuffer()[0];

   return p->getAsInt();
}

class QoreNode *PC_addch(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      if (!is_nothing(p0) && (p0->type != NT_STRING || p0->val.String->strlen()))
	 rv = new QoreNode((int64)p->qaddch(getChar(p0)));
      else
	 rv = NULL;
      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::addch");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_mvaddch(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   QoreNode *rv;
   if (p)
   {
      QoreNode *p0 = get_param(params, 0);
      QoreNode *p1 = get_param(params, 1);
      QoreNode *p2 = get_param(params, 2);
      if (!is_nothing(p2) && (p2->type != NT_STRING || p2->val.String->strlen()))
	 rv = new QoreNode((int64)p->qmvaddch(p0 ? p0->getAsInt() : 0,
					      p1 ? p1->getAsInt() : 0,
					      getChar(p2)));
      else
	 rv = NULL;
      p->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Panel::mvaddch");
      rv = NULL;
   }
   return rv;
}

class QoreNode *PC_clrtoeol(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   if (p)
   {
      p->qclrtoeol();
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Panel::clrtoeol");

   return NULL;
}

class QoreNode *PC_clrtobot(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   Panel *p = (Panel *)self->getReferencedPrivateData(CID_PANEL);
   if (p)
   {
      p->qclrtobot();
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Panel::clrtobot");

   return NULL;
}

class QoreClass *initPanelClass()
{
   tracein("initPanelClass()");

   class QoreClass *QC_PANEL = new QoreClass(strdup("Panel"));
   CID_PANEL = QC_PANEL->getID();
   QC_PANEL->addMethod("constructor",        PC_constructor);
   QC_PANEL->addMethod("destructor",         PC_destructor);
   QC_PANEL->addMethod("copy",               PC_copy);
   QC_PANEL->addMethod("keypad",             PC_keypad);
   QC_PANEL->addMethod("mvaddstr",           PC_mvaddstr);
   QC_PANEL->addMethod("mvprintw",           PC_mvprintw);
   QC_PANEL->addMethod("addstr",             PC_addstr);
   QC_PANEL->addMethod("printw",             PC_printw);
   QC_PANEL->addMethod("refresh",            PC_refresh);
   QC_PANEL->addMethod("update",             PC_update);
   QC_PANEL->addMethod("getch",              PC_getch);
   QC_PANEL->addMethod("border",             PC_border);
   QC_PANEL->addMethod("setBackgroundColor", PC_setBackgroundColor);
   QC_PANEL->addMethod("setForegroundColor", PC_setForegroundColor);
   QC_PANEL->addMethod("setColor",           PC_setColor);
   QC_PANEL->addMethod("getLines",           PC_getLines);
   QC_PANEL->addMethod("getColumns",         PC_getColumns);

   QC_PANEL->addMethod("show",               PC_show);
   QC_PANEL->addMethod("hide",               PC_hide);
   QC_PANEL->addMethod("top",                PC_top);
   QC_PANEL->addMethod("bottom",             PC_bottom);

#ifdef HAVE_WRESIZE
   QC_PANEL->addMethod("resize",             PC_resize);
#endif
   QC_PANEL->addMethod("getY",               PC_getY);
   QC_PANEL->addMethod("getX",               PC_getX);
   QC_PANEL->addMethod("getBegY",            PC_getBegY);
   QC_PANEL->addMethod("getBegX",            PC_getBegX);

   QC_PANEL->addMethod("attrset",            PC_attrset);
   QC_PANEL->addMethod("attron",             PC_attron);
   QC_PANEL->addMethod("attroff",            PC_attroff);
   QC_PANEL->addMethod("movePanel",          PC_movePanel);
   QC_PANEL->addMethod("move",               PC_move);
   QC_PANEL->addMethod("scrollok",           PC_scrollok);
   QC_PANEL->addMethod("idlok",              PC_idlok);
   QC_PANEL->addMethod("clearok",            PC_clearok);
   QC_PANEL->addMethod("idcok",              PC_idcok);
   QC_PANEL->addMethod("immedok",            PC_immedok);
   QC_PANEL->addMethod("erase",              PC_erase);
   QC_PANEL->addMethod("clear",              PC_clear);
   QC_PANEL->addMethod("redraw",             PC_redraw);
   QC_PANEL->addMethod("scroll",             PC_scroll);
   QC_PANEL->addMethod("scrl",               PC_scrl);
   QC_PANEL->addMethod("setscrreg",          PC_setscrreg);
   QC_PANEL->addMethod("hline",              PC_hline);
   QC_PANEL->addMethod("vline",              PC_vline);
   QC_PANEL->addMethod("mvhline",            PC_mvhline);
   QC_PANEL->addMethod("mvvline",            PC_mvvline);
   QC_PANEL->addMethod("addch",              PC_addch);
   QC_PANEL->addMethod("mvaddch",            PC_mvaddch);

   QC_PANEL->addMethod("clrtoeol",           PC_clrtoeol);
   QC_PANEL->addMethod("clrtobot",           PC_clrtobot);

   traceout("initPanelClass()");
   return QC_PANEL;
}
