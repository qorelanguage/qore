/*
  modules/ncurses/QC_Window.h

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

#ifndef _QORE_NCURSES_WINDOW_H

#define _QORE_NCURSES_WINDOW_H

#include <qore/ReferenceObject.h>
#include <qore/Exception.h>
#include <qore/LockedObject.h>

#include <curses.h>

extern class LockedObject lUpdate, lGetch;

extern int CID_Window;
class QoreClass *initWindowClass();

class Window : public ReferenceObject, public LockedObject {
   private:

   protected:
      WINDOW *win;
      int bg, fg;

      ~Window() 
      {
	 if (win)
	    delwin(win);
      }
      inline int setColor()
      {
	 //wattroff(win, A_COLOR);
	 lock();
	 int rc = wattrset(win, COLOR_PAIR(fg * COLORS + bg));
	 unlock();
	 return rc;
      }

   public:
      inline Window(int lines, int cols, int y, int x, class ExceptionSink *xsink)
      {
	 fg = COLOR_WHITE;
	 bg = COLOR_BLACK;
	 win = newwin(lines, cols, y, x);
	 if (!win)
	    xsink->raiseException("WINDOW-CREATION-ERROR", "cannot create window with lines=%d, cols=%d, x=%d, y=%d", lines, cols, x, y);
	 else
	 {
	    if (!lines)
	       getmaxyx(win, lines, cols);
	    ::keypad(win, true);
	 }
      }

      inline int keypad(bool b)
      {
	 lock();
	 int rc = ::keypad(win, b);
	 unlock();
	 return rc;
      }

      inline int qaddstr(char *str)
      {
	 lock();
	 int rc = waddstr(win, str);
	 unlock();
	 return rc;
      }

      inline int qmvaddstr(int y, int x, char *str)
      {
	 lock();
	 int rc = mvwaddstr(win, y, x, str);
	 unlock();
	 return rc;
      }

      inline int qrefresh()
      {
	 lUpdate.lock();
	 int rc = wrefresh(win);
	 lUpdate.unlock();
	 return rc;
      }

      inline int qgetch()
      {
	 lGetch.lock();
	 int rc = wgetch(win);
	 lGetch.unlock();
	 return rc;
      }

#ifdef HAVE_WRESIZE
      inline int resize(int y, int x)
      {
	 lock();
	 int rc = wresize(win, y, x);
	 unlock();
	 return rc;
      }
#endif

      inline int qborder(int ls, int rs, int ts, int bs, int tl, int tr, int bl, int br)
      {
	 lock();
	 int rc = wborder(win, ls, rs, ts, bs, tl, tr, bl, br);
	 unlock();
	 return rc;
      }

      inline int setColor(int fgc, int bgc)
      {
	 fg = fgc;
	 bg = bgc;
	 return setColor();
      }

      inline int setForegroundColor(int color)
      {
	 fg = color;
	 return setColor();
      }

      inline int setBackgroundColor(int color)
      {
	 bg = color;
	 return setColor();
      }

      inline int qattrset(int attr)
      {
	 lock();
	 int rc = wattrset(win, attr);
	 unlock();
	 return rc;
      }

      inline int qattron(int attr)
      {
	 lock();
	 int rc = wattron(win, attr);
	 unlock();
	 return rc;
      }

      inline int qattroff(int attr)
      {
	 lock();
	 int rc = wattroff(win, attr);
	 unlock();
	 return rc;
      }

      inline int getLines()
      {
	 int y, x;
	 getmaxyx(win, y, x);
	 return y;
      }

      inline int getColumns()
      {
	 int y, x;
	 getmaxyx(win, y, x);
	 return x;
      }

      inline int getBegX()
      {
	 int y, x;
	 getbegyx(win, y, x);
	 return x;
      }

      inline int getBegY()
      {
	 int y, x;
	 getbegyx(win, y, x);
	 return y;
      }

      inline int getX()
      {
	 int y, x;
	 getyx(win, y, x);
	 return x;
      }

      inline int getY()
      {
	 int y, x;
	 getyx(win, y, x);
	 return y;
      }

      inline int qmove(int y, int x)
      {
	 lock();
	 int rc = wmove(win, y, x);
	 unlock();
	 return rc;
      }

      inline int moveWindow(int y, int x)
      {
	 lock();
	 int rc = mvwin(win, y, x);
	 unlock();
	 return rc;
      }

      inline int scrollok(bool b)
      {
	 lock();
	 int rc = ::scrollok(win, b);
	 unlock();
	 return rc;
      }

      inline int idlok(bool b)
      {
	 lock();
	 int rc = ::idlok(win, b);
	 unlock();
	 return rc;
      }

      inline int clearok(bool b)
      {
	 lock();
	 int rc = ::clearok(win, b);
	 unlock();
	 return rc;
      }

      void idcok(bool b)
      {
	 lock();
	 ::idcok(win, b);
	 unlock();
      }

      void immedok(bool b)
      {
	 lock();
	 ::immedok(win, b);
	 unlock();
      }
      
      int qerase()
      {
	 lock();
	 int rc = werase(win);
	 unlock();
	 return rc;
      }
      
      int qclear()
      {
	 lock();
	 int rc = wclear(win);
	 unlock();
	 return rc;
      }

      inline int qsetscrreg(int top, int bottom)
      {
	 lock();
	 int rc = wsetscrreg(win, top, bottom);
	 unlock();
	 return rc;
      }

      int redraw()
      {
	 lock();
	 int rc = redrawwin(win);
	 unlock();
	 return rc;
      }
      
      int qscroll()
      {
	 lock();
	 int rc = scroll(win);
	 unlock();
	 return rc;
      }

      int qscrl(int i)
      {
	 lock();
	 int rc = wscrl(win, i);
	 unlock();
	 return rc;
      }

      int qaddch(int c)
      {
	 lock();
	 int rc = waddch(win, c);
	 unlock();
	 return rc;
      }

      int qmvaddch(int y, int x, int c)
      {
	 lock();
	 int rc = mvwaddch(win, y, x, c);
	 unlock();
	 return rc;
      }

      int qhline(int c, int n)
      {
	 lock();
	 int rc = whline(win, c, n);
	 unlock();
	 return rc;
      }

      int qvline(int c, int n)
      {
	 lock();
	 int rc = wvline(win, c, n);
	 unlock();
	 return rc;
      }

      int qmvhline(int y, int x, int c, int n)
      {
	 lock();
	 int rc = mvwhline(win, y, x, c, n);
	 unlock();
	 return rc;
      }

      int qmvvline(int y, int x, int c, int n)
      {
	 lock();
	 int rc = mvwvline(win, y, x, c, n);
	 unlock();
	 return rc;
      }

      int qclrtoeol()
      {
	 lock();
	 int rc = wclrtoeol(win);
	 unlock();
	 return rc;
      }

      int qclrtobot()
      {
	 lock();
	 int rc = wclrtobot(win);
	 unlock();
	 return rc;
      }

      int nodelay(bool b)
      {
	 lock();
	 int rc = ::nodelay(win, b);
	 unlock();
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

static inline int getChar(QoreNode *p)
{
   if (p->type == NT_STRING)
      return p->val.String->getBuffer()[0];

   return p->getAsInt();
}

#endif // _QORE_NCURSES_WINDOW_H
