/*
  modules/ncurses/QC_Panel.h

  Qore Programming Language

  Copyright (C) 2004, 2005, 2006 David Nichols

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

#ifndef _QORE_NCURSES_PANEL_H

#define _QORE_NCURSES_PANEL_H

#include <qore/Exception.h>

#include <curses.h>
#include <panel.h>

#include "QC_Window.h"

extern int CID_PANEL;
class QoreClass *initPanelClass();

class Panel : public Window {
   private:
      PANEL *panel;

   protected:
      ~Panel() 
      {
	 if (panel)
	 {
	    lUpdate.lock();
	    del_panel(panel);
	    lUpdate.unlock();
	 }
      }

   public:
      inline Panel(int lines, int cols, int y, int x, class ExceptionSink *xsink) : Window(lines, cols, y, x, xsink)
      {
	 if (!win)
	    panel = NULL;
	 else
	 {
	    lUpdate.lock();
	    panel = new_panel(win);
	    lUpdate.unlock();
	 }
      }

      inline void update()
      {
	 lUpdate.lock();
	 update_panels();
	 lUpdate.unlock();
      }

      inline void qrefresh()
      {
	 lUpdate.lock();
	 update_panels();
	 doupdate();
	 lUpdate.unlock();
      }

      inline int show()
      {
	 lUpdate.lock();
	 int rc = show_panel(panel);
	 lUpdate.unlock();
	 return rc;
      }

      inline int hide()
      {
	 lUpdate.lock();
	 int rc = hide_panel(panel);
	 lUpdate.unlock();
	 return rc;
      }

      inline int movePanel(int y, int x)
      {
	 lock();
	 int rc = move_panel(panel, y, x);
	 unlock();
	 return rc;
      }

#ifdef HAVE_WRESIZE
      inline int resize(int y, int x)
      {
	 if (Window::resize(y, x))
	    return -1;
	 lUpdate.lock();
	 int rc = replace_panel(panel, win);
	 lUpdate.unlock();
	 return rc;
      }
#endif

      inline int top()
      {
	 lUpdate.lock();
	 int rc = top_panel(panel);
	 lUpdate.unlock();
	 return rc;
      }

      inline int bottom()
      {
	 lUpdate.lock();
	 int rc = bottom_panel(panel);
	 lUpdate.unlock();
	 return rc;
      }

      inline void ref()
      {
	 ROreference();
      }

      inline void deref()
      {
	 if (ROdereference())
	    delete this;
      }
};

#endif // _QORE_NCURSES_PANEL_H
