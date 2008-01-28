/*
  modules/ncurses/ncurses.cc

  ncurses class

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006

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
#include "QC_Panel.h"

#include <curses.h>

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "ncurses";
DLLEXPORT char qore_module_version[] = "0.1";
DLLEXPORT char qore_module_description[] = "ncurses class module";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://qore.sourceforge.net";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = ncurses_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = ncurses_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = ncurses_module_delete;
#endif

class LockedObject lUpdate, lGetch;

static inline void init_colors()
{
   for (int i = 0; i < COLORS; i++)
      for (int j = 0; j < COLORS; j++)
	 init_pair(i * COLORS + j, i, j);
}

static inline void init_constants(class QoreNamespace *ns)
{
   // colors
   ns->addConstant("COLOR_BLACK", new QoreBigIntNode(COLOR_BLACK));
   ns->addConstant("COLOR_RED", new QoreBigIntNode(COLOR_RED));
   ns->addConstant("COLOR_GREEN", new QoreBigIntNode(COLOR_GREEN));
   ns->addConstant("COLOR_YELLOW", new QoreBigIntNode(COLOR_YELLOW));
   ns->addConstant("COLOR_BLUE", new QoreBigIntNode(COLOR_BLUE));
   ns->addConstant("COLOR_MAGENTA", new QoreBigIntNode(COLOR_MAGENTA));
   ns->addConstant("COLOR_CYAN", new QoreBigIntNode(COLOR_CYAN));
   ns->addConstant("COLOR_WHITE", new QoreBigIntNode(COLOR_WHITE));

   // keys
   ns->addConstant("KEY_BREAK", new QoreBigIntNode(KEY_BREAK));
   ns->addConstant("KEY_DOWN", new QoreBigIntNode(KEY_DOWN));
   ns->addConstant("KEY_UP", new QoreBigIntNode(KEY_UP));
   ns->addConstant("KEY_LEFT", new QoreBigIntNode(KEY_LEFT));
   ns->addConstant("KEY_RIGHT", new QoreBigIntNode(KEY_RIGHT));
   ns->addConstant("KEY_HOME", new QoreBigIntNode(KEY_HOME));
   ns->addConstant("KEY_BACKSPACE", new QoreBigIntNode(KEY_BACKSPACE));
   ns->addConstant("KEY_F0", new QoreBigIntNode(KEY_F0));
   ns->addConstant("KEY_F1", new QoreBigIntNode(KEY_F(1)));
   ns->addConstant("KEY_F2", new QoreBigIntNode(KEY_F(2)));
   ns->addConstant("KEY_F3", new QoreBigIntNode(KEY_F(3)));
   ns->addConstant("KEY_F4", new QoreBigIntNode(KEY_F(4)));
   ns->addConstant("KEY_F5", new QoreBigIntNode(KEY_F(5)));
   ns->addConstant("KEY_F6", new QoreBigIntNode(KEY_F(6)));
   ns->addConstant("KEY_F7", new QoreBigIntNode(KEY_F(7)));
   ns->addConstant("KEY_F8", new QoreBigIntNode(KEY_F(8)));
   ns->addConstant("KEY_F9", new QoreBigIntNode(KEY_F(9)));
   ns->addConstant("KEY_F10", new QoreBigIntNode(KEY_F(10)));
   ns->addConstant("KEY_F11", new QoreBigIntNode(KEY_F(11)));
   ns->addConstant("KEY_F12", new QoreBigIntNode(KEY_F(12)));
   ns->addConstant("KEY_DL", new QoreBigIntNode(KEY_DL));
   ns->addConstant("KEY_IL", new QoreBigIntNode(KEY_IL));
   ns->addConstant("KEY_DC", new QoreBigIntNode(KEY_DC));
   ns->addConstant("KEY_IC", new QoreBigIntNode(KEY_IC));
   ns->addConstant("KEY_EIC", new QoreBigIntNode(KEY_EIC));
   ns->addConstant("KEY_CLEAR", new QoreBigIntNode(KEY_CLEAR));
   ns->addConstant("KEY_EOS", new QoreBigIntNode(KEY_EOS));
   ns->addConstant("KEY_EOL", new QoreBigIntNode(KEY_EOL));
   ns->addConstant("KEY_SF", new QoreBigIntNode(KEY_SF));
   ns->addConstant("KEY_SR", new QoreBigIntNode(KEY_SR));
   ns->addConstant("KEY_NPAGE", new QoreBigIntNode(KEY_NPAGE));
   ns->addConstant("KEY_PPAGE", new QoreBigIntNode(KEY_PPAGE));
   ns->addConstant("KEY_STAB", new QoreBigIntNode(KEY_STAB));
   ns->addConstant("KEY_CTAB", new QoreBigIntNode(KEY_CTAB));
   ns->addConstant("KEY_CATAB", new QoreBigIntNode(KEY_CATAB));
   ns->addConstant("KEY_ENTER", new QoreBigIntNode(KEY_ENTER));
   ns->addConstant("KEY_SRESET", new QoreBigIntNode(KEY_SRESET));
   ns->addConstant("KEY_RESET", new QoreBigIntNode(KEY_RESET));
   ns->addConstant("KEY_PRINT", new QoreBigIntNode(KEY_PRINT));
   ns->addConstant("KEY_LL", new QoreBigIntNode(KEY_LL));
   ns->addConstant("KEY_A1", new QoreBigIntNode(KEY_A1));
   ns->addConstant("KEY_A3", new QoreBigIntNode(KEY_A3));
   ns->addConstant("KEY_B2", new QoreBigIntNode(KEY_B2));
   ns->addConstant("KEY_C1", new QoreBigIntNode(KEY_C1));
   ns->addConstant("KEY_C3", new QoreBigIntNode(KEY_C3));
   ns->addConstant("KEY_BTAB", new QoreBigIntNode(KEY_BTAB));
   ns->addConstant("KEY_BEG", new QoreBigIntNode(KEY_BEG));
   ns->addConstant("KEY_CANCEL", new QoreBigIntNode(KEY_CANCEL));
   ns->addConstant("KEY_CLOSE", new QoreBigIntNode(KEY_CLOSE));
   ns->addConstant("KEY_COMMAND", new QoreBigIntNode(KEY_COMMAND));
   ns->addConstant("KEY_COPY", new QoreBigIntNode(KEY_COPY));
   ns->addConstant("KEY_CREATE", new QoreBigIntNode(KEY_CREATE));
   ns->addConstant("KEY_END", new QoreBigIntNode(KEY_END));
   ns->addConstant("KEY_EXIT", new QoreBigIntNode(KEY_EXIT));
   ns->addConstant("KEY_FIND", new QoreBigIntNode(KEY_FIND));
   ns->addConstant("KEY_HELP", new QoreBigIntNode(KEY_HELP));
   ns->addConstant("KEY_MARK", new QoreBigIntNode(KEY_MARK));
   ns->addConstant("KEY_MESSAGE", new QoreBigIntNode(KEY_MESSAGE));
   ns->addConstant("KEY_MOUSE", new QoreBigIntNode(KEY_MOUSE));
   ns->addConstant("KEY_MOVE", new QoreBigIntNode(KEY_MOVE));
   ns->addConstant("KEY_NEXT", new QoreBigIntNode(KEY_NEXT));
   ns->addConstant("KEY_OPEN", new QoreBigIntNode(KEY_OPEN));
   ns->addConstant("KEY_OPTIONS", new QoreBigIntNode(KEY_OPTIONS));
   ns->addConstant("KEY_PREVIOUS", new QoreBigIntNode(KEY_PREVIOUS));
   ns->addConstant("KEY_REDO", new QoreBigIntNode(KEY_REDO));
   ns->addConstant("KEY_REFERENCE", new QoreBigIntNode(KEY_REFERENCE));
   ns->addConstant("KEY_REFRESH", new QoreBigIntNode(KEY_REFRESH));
   ns->addConstant("KEY_REPLACE", new QoreBigIntNode(KEY_REPLACE));
#ifdef KEY_RESIZE
   ns->addConstant("KEY_RESIZE", new QoreBigIntNode(KEY_RESIZE));
#endif
   ns->addConstant("KEY_RESTART", new QoreBigIntNode(KEY_RESTART));
   ns->addConstant("KEY_RESUME", new QoreBigIntNode(KEY_RESUME));
   ns->addConstant("KEY_SAVE", new QoreBigIntNode(KEY_SAVE));
   ns->addConstant("KEY_SBEG", new QoreBigIntNode(KEY_SBEG));
   ns->addConstant("KEY_SCANCEL", new QoreBigIntNode(KEY_SCANCEL));
   ns->addConstant("KEY_SCOMMAND", new QoreBigIntNode(KEY_SCOMMAND));
   ns->addConstant("KEY_SCOPY", new QoreBigIntNode(KEY_SCOPY));
   ns->addConstant("KEY_SCREATE", new QoreBigIntNode(KEY_SCREATE));
   ns->addConstant("KEY_SDC", new QoreBigIntNode(KEY_SDC));
   ns->addConstant("KEY_SDL", new QoreBigIntNode(KEY_SDL));
   ns->addConstant("KEY_SELECT", new QoreBigIntNode(KEY_SELECT));
   ns->addConstant("KEY_SEND", new QoreBigIntNode(KEY_SEND));
   ns->addConstant("KEY_SEOL", new QoreBigIntNode(KEY_SEOL));
   ns->addConstant("KEY_SEXIT", new QoreBigIntNode(KEY_SEXIT));
   ns->addConstant("KEY_SFIND", new QoreBigIntNode(KEY_SFIND));
   ns->addConstant("KEY_SHELP", new QoreBigIntNode(KEY_SHELP));
   ns->addConstant("KEY_SHOME", new QoreBigIntNode(KEY_SHOME));
   ns->addConstant("KEY_SIC", new QoreBigIntNode(KEY_SIC));
   ns->addConstant("KEY_SLEFT", new QoreBigIntNode(KEY_SLEFT));
   ns->addConstant("KEY_SMESSAGE", new QoreBigIntNode(KEY_SMESSAGE));
   ns->addConstant("KEY_SMOVE", new QoreBigIntNode(KEY_SMOVE));
   ns->addConstant("KEY_SNEXT", new QoreBigIntNode(KEY_SNEXT));
   ns->addConstant("KEY_SOPTIONS", new QoreBigIntNode(KEY_SOPTIONS));
   ns->addConstant("KEY_SPREVIOUS", new QoreBigIntNode(KEY_SPREVIOUS));
   ns->addConstant("KEY_SPRINT", new QoreBigIntNode(KEY_SPRINT));
   ns->addConstant("KEY_SREDO", new QoreBigIntNode(KEY_SREDO));
   ns->addConstant("KEY_SREPLACE", new QoreBigIntNode(KEY_SREPLACE));
   ns->addConstant("KEY_SRIGHT", new QoreBigIntNode(KEY_SRIGHT));
   ns->addConstant("KEY_SRSUME", new QoreBigIntNode(KEY_SRSUME));
   ns->addConstant("KEY_SSAVE", new QoreBigIntNode(KEY_SSAVE));
   ns->addConstant("KEY_SSUSPEND", new QoreBigIntNode(KEY_SSUSPEND));
   ns->addConstant("KEY_SUNDO", new QoreBigIntNode(KEY_SUNDO));
   ns->addConstant("KEY_SUSPEND", new QoreBigIntNode(KEY_SUSPEND));
   ns->addConstant("KEY_UNDO", new QoreBigIntNode(KEY_UNDO));

   // attributes
   ns->addConstant("A_NORMAL", new QoreBigIntNode(A_NORMAL));           // Normal mode
   ns->addConstant("A_STANDOUT", new QoreBigIntNode(A_STANDOUT));       // Best highlighting mode of the terminal
   ns->addConstant("A_UNDERLINE", new QoreBigIntNode(A_UNDERLINE));     // Underline mode
   ns->addConstant("A_REVERSE", new QoreBigIntNode(A_REVERSE));         // Reverse video
   ns->addConstant("A_BLINK", new QoreBigIntNode(A_BLINK));             // Blinking
   ns->addConstant("A_DIM", new QoreBigIntNode(A_DIM));                 // Half bright
   ns->addConstant("A_BOLD", new QoreBigIntNode(A_BOLD));               // Extra bright or bold
   ns->addConstant("A_PROTECT", new QoreBigIntNode(A_PROTECT));         // Protected mode
   ns->addConstant("A_INVIS", new QoreBigIntNode(A_INVIS));             // Invisible mode
   ns->addConstant("A_ALTCHARSET", new QoreBigIntNode(A_ALTCHARSET));   // Alternate character set
   ns->addConstant("A_CHARTEXT", new QoreBigIntNode(A_ALTCHARSET));     // Bit-mask to extract a character

#ifdef NCURSES_MOUSE_VERSION
   // mouse events
   ns->addConstant("BUTTON1_PRESSED",        new QoreBigIntNode(BUTTON1_PRESSED));
   ns->addConstant("BUTTON1_RELEASED",       new QoreBigIntNode(BUTTON1_RELEASED));
   ns->addConstant("BUTTON1_CLICKED",        new QoreBigIntNode(BUTTON1_CLICKED));
   ns->addConstant("BUTTON1_DOUBLE_CLICKED", new QoreBigIntNode(BUTTON1_DOUBLE_CLICKED));
   ns->addConstant("BUTTON1_TRIPLE_CLICKED", new QoreBigIntNode(BUTTON1_TRIPLE_CLICKED));
   ns->addConstant("BUTTON2_PRESSED",        new QoreBigIntNode(BUTTON2_PRESSED));
   ns->addConstant("BUTTON2_RELEASED",       new QoreBigIntNode(BUTTON2_RELEASED));
   ns->addConstant("BUTTON2_CLICKED",        new QoreBigIntNode(BUTTON2_CLICKED));
   ns->addConstant("BUTTON2_DOUBLE_CLICKED", new QoreBigIntNode(BUTTON2_DOUBLE_CLICKED));
   ns->addConstant("BUTTON2_TRIPLE_CLICKED", new QoreBigIntNode(BUTTON2_TRIPLE_CLICKED));
   ns->addConstant("BUTTON3_PRESSED",        new QoreBigIntNode(BUTTON3_PRESSED));
   ns->addConstant("BUTTON3_RELEASED",       new QoreBigIntNode(BUTTON3_RELEASED));
   ns->addConstant("BUTTON3_CLICKED",        new QoreBigIntNode(BUTTON3_CLICKED));
   ns->addConstant("BUTTON3_DOUBLE_CLICKED", new QoreBigIntNode(BUTTON3_DOUBLE_CLICKED));
   ns->addConstant("BUTTON3_TRIPLE_CLICKED", new QoreBigIntNode(BUTTON3_TRIPLE_CLICKED));
   ns->addConstant("BUTTON4_PRESSED",        new QoreBigIntNode(BUTTON4_PRESSED));
   ns->addConstant("BUTTON4_RELEASED",       new QoreBigIntNode(BUTTON4_RELEASED));
   ns->addConstant("BUTTON4_CLICKED",        new QoreBigIntNode(BUTTON4_CLICKED));
   ns->addConstant("BUTTON4_DOUBLE_CLICKED", new QoreBigIntNode(BUTTON4_DOUBLE_CLICKED));
   ns->addConstant("BUTTON4_TRIPLE_CLICKED", new QoreBigIntNode(BUTTON4_TRIPLE_CLICKED));
   ns->addConstant("BUTTON_SHIFT",           new QoreBigIntNode(BUTTON_SHIFT));
   ns->addConstant("BUTTON_CTRL",            new QoreBigIntNode(BUTTON_CTRL));
   ns->addConstant("BUTTON_ALT",             new QoreBigIntNode(BUTTON_ALT));
   ns->addConstant("ALL_MOUSE_EVENTS",       new QoreBigIntNode(ALL_MOUSE_EVENTS));
   ns->addConstant("REPORT_MOUSE_POSITION",  new QoreBigIntNode(REPORT_MOUSE_POSITION));
#endif // NCURSES_MOUSE_VERSION
}

class q_nc_init_class q_nc_init;

void q_nc_init_class::init_intern()
{
   l.lock();
   if (!initialized)
   {
      initscr();
      start_color();
      init_colors();
      cbreak();
      keypad(stdscr, true);
      noecho();
      initialized = true;
   }
   l.unlock();
}

void q_nc_init_class::close()
{
   l.lock();
   if (initialized)
   {
      endwin();
      initialized = false;
   }
   l.unlock();
}

static class QoreNode *f_initscr(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return NULL;
}

static class QoreNode *f_printw(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   TempQoreStringNode str(q_sprintf(params, 0, 0, xsink)); 
   // note: need cast for solaris curses
   int rc = printw((char *)str->getBuffer());
   return new QoreBigIntNode(rc);
}

static class QoreNode *f_refresh(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   lUpdate.lock();
   int rc = refresh();
   lUpdate.unlock();

   return new QoreBigIntNode(rc);
}

static class QoreNode *f_doupdate(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   lUpdate.lock();
   int rc = doupdate();
   lUpdate.unlock();

   return new QoreBigIntNode(rc);
}

static class QoreNode *f_getch(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   lGetch.lock();
   int rc = getch();
   lGetch.unlock();
   return new QoreBigIntNode(rc);
}

static class QoreNode *f_endwin(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   q_nc_init.close();
   return NULL;
}

static class QoreNode *f_cbreak(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   return new QoreBigIntNode(cbreak());
}

static class QoreNode *f_nocbreak(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   return new QoreBigIntNode(nocbreak());
}

static class QoreNode *f_echo(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   return new QoreBigIntNode(echo());
}

static class QoreNode *f_noecho(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   return new QoreBigIntNode(noecho());
}

static class QoreNode *f_raw(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   return new QoreBigIntNode(raw());
}

static class QoreNode *f_noraw(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   return new QoreBigIntNode(noraw());
}

static class QoreNode *f_noqiflush(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   noqiflush();
   return NULL;
}

static class QoreNode *f_qiflush(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   qiflush();
   return NULL;
}

static class QoreNode *f_halfdelay(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   class QoreNode *p0 = get_param(params, 0);
   int d = p0 ? p0->getAsInt() : 0;
   if (!d)
      return NULL;

   return new QoreBigIntNode(halfdelay(d));
}

static class QoreNode *f_curs_set(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   class QoreNode *p0 = get_param(params, 0);

   return new QoreBigIntNode(curs_set(p0 ? p0->getAsInt() : 0));
}

static class QoreNode *f_def_prog_mode(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(def_prog_mode());
}

static class QoreNode *f_reset_prog_mode(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(reset_prog_mode());
}

static class QoreNode *f_def_shell_mode(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(def_shell_mode());
}

static class QoreNode *f_reset_shell_mode(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(reset_shell_mode());
}

static class QoreNode *f_beep(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(beep());
}

static class QoreNode *f_flash(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(flash());
}

static class QoreNode *f_has_colors(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(has_colors());
}

static class QoreNode *f_get_color_pair(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   class QoreNode *p0 = get_param(params, 0);
   class QoreNode *p1 = get_param(params, 1);
   int fg = p0 ? p0->getAsInt() : 0;
   int bg = p1 ? p1->getAsInt() : 0;

   return new QoreBigIntNode(COLOR_PAIR(fg * COLORS + bg));
}

static class QoreNode *f_num_colors(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(COLORS);
}

static class QoreNode *f_nl(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(nl());
}

static class QoreNode *f_nonl(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(nonl());
}

static class QoreNode *f_getLines(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(LINES);
}

static class QoreNode *f_getColumns(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(COLS);
}

// alternate character set functions - these cannot be constants because they are terminal-dependent
// so to initialize them as constants would mean calling initscr() in the module_init function

static class QoreNode *f_ACS_ULCORNER(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_ULCORNER);
}

static class QoreNode *f_ACS_LLCORNER(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_LLCORNER);
}

static class QoreNode *f_ACS_URCORNER(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_URCORNER);
}

static class QoreNode *f_ACS_LRCORNER(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_LRCORNER);
}

static class QoreNode *f_ACS_LTEE(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_LTEE);
}

static class QoreNode *f_ACS_RTEE(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_RTEE);
}

static class QoreNode *f_ACS_BTEE(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_BTEE);
}

static class QoreNode *f_ACS_TTEE(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_TTEE);
}

static class QoreNode *f_ACS_HLINE(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_HLINE);
}

static class QoreNode *f_ACS_VLINE(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_VLINE);
}

static class QoreNode *f_ACS_PLUS(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_PLUS);
}

static class QoreNode *f_ACS_S1(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_S1);
}

static class QoreNode *f_ACS_S9(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_S9);
}

static class QoreNode *f_ACS_DIAMOND(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_DIAMOND);
}

static class QoreNode *f_ACS_CKBOARD(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_CKBOARD);
}

static class QoreNode *f_ACS_DEGREE(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_DEGREE);
}

static class QoreNode *f_ACS_PLMINUS(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_PLMINUS);
}

static class QoreNode *f_ACS_BULLET(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_BULLET);
}

static class QoreNode *f_ACS_LARROW(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_LARROW);
}

static class QoreNode *f_ACS_RARROW(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_RARROW);
}

static class QoreNode *f_ACS_DARROW(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_DARROW);
}

static class QoreNode *f_ACS_UARROW(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_UARROW);
}

static class QoreNode *f_ACS_BOARD(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_BOARD);
}

static class QoreNode *f_ACS_LANTERN(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_LANTERN);
}

static class QoreNode *f_ACS_BLOCK(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreBigIntNode(ACS_BLOCK);
}

#ifdef NCURSES_MOUSE_VERSION
static class QoreNode *f_mousemask(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   class QoreNode *p0 = get_param(params, 0);
   int d = p0 ? p0->getAsInt() : 0;

   return new QoreBigIntNode(mousemask(d, NULL));
}

static QoreNode *f_getmouse(const QoreListNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   MEVENT event;
   
   if (getmouse(&event))
      return NULL;

   class QoreHashNode *h = new QoreHashNode();
   h->setKeyValue("id",     new QoreBigIntNode(event.id), NULL);
   h->setKeyValue("x",      new QoreBigIntNode(event.x), NULL);
   h->setKeyValue("y",      new QoreBigIntNode(event.y), NULL);
   h->setKeyValue("z",      new QoreBigIntNode(event.z), NULL);
   h->setKeyValue("bstate", new QoreBigIntNode(event.bstate), NULL);

   return h;
}
#endif // NCURSES_MOUSE_VERSION

static class QoreNamespace *NCNS;

static void init_namespace()
{
   NCNS = new QoreNamespace("NCurses");
   NCNS->addSystemClass(initWindowClass());
   NCNS->addSystemClass(initPanelClass());
   init_constants(NCNS);
}

class QoreStringNode *ncurses_module_init()
{
   tracein("ncurses_module_init()");

   init_namespace();

   builtinFunctions.add("initscr",          f_initscr);
   builtinFunctions.add("printw",           f_printw);
   builtinFunctions.add("refresh",          f_refresh);
   builtinFunctions.add("doupdate",         f_doupdate);
   builtinFunctions.add("getch",            f_getch);
   builtinFunctions.add("endwin",           f_endwin);
   builtinFunctions.add("cbreak",           f_cbreak);
   builtinFunctions.add("nocbreak",         f_nocbreak);
   builtinFunctions.add("echo",             f_echo);
   builtinFunctions.add("noecho",           f_noecho);
   builtinFunctions.add("raw",              f_raw);
   builtinFunctions.add("noraw",            f_noraw);
   builtinFunctions.add("noqiflush",        f_noqiflush);
   builtinFunctions.add("qiflush",          f_qiflush);
   builtinFunctions.add("halfdelay",        f_halfdelay);
   builtinFunctions.add("curs_set",         f_curs_set);
   builtinFunctions.add("def_prog_mode",    f_def_prog_mode);
   builtinFunctions.add("reset_prog_mode",  f_reset_prog_mode);
   builtinFunctions.add("def_shell_mode",   f_def_shell_mode);
   builtinFunctions.add("reset_shell_mode", f_reset_shell_mode);
   builtinFunctions.add("beep",             f_beep);
   builtinFunctions.add("flash",            f_flash);
   builtinFunctions.add("has_colors",       f_has_colors);
   builtinFunctions.add("get_color_pair",   f_get_color_pair);
   builtinFunctions.add("num_colors",       f_num_colors);
   builtinFunctions.add("nl",               f_nl);
   builtinFunctions.add("nonl",             f_nonl);
   builtinFunctions.add("getLines",         f_getLines);
   builtinFunctions.add("getColumns",       f_getColumns);

   // alternate character set functions
   builtinFunctions.add("ACS_ULCORNER", f_ACS_ULCORNER);
   builtinFunctions.add("ACS_LLCORNER", f_ACS_LLCORNER);
   builtinFunctions.add("ACS_URCORNER", f_ACS_URCORNER);
   builtinFunctions.add("ACS_LRCORNER", f_ACS_LRCORNER);
   builtinFunctions.add("ACS_LTEE", f_ACS_LTEE);
   builtinFunctions.add("ACS_RTEE", f_ACS_RTEE);
   builtinFunctions.add("ACS_BTEE", f_ACS_BTEE);
   builtinFunctions.add("ACS_TTEE", f_ACS_TTEE);
   builtinFunctions.add("ACS_HLINE", f_ACS_HLINE);
   builtinFunctions.add("ACS_VLINE", f_ACS_VLINE);
   builtinFunctions.add("ACS_PLUS", f_ACS_PLUS);
   builtinFunctions.add("ACS_S1", f_ACS_S1);
   builtinFunctions.add("ACS_S9", f_ACS_S9);
   builtinFunctions.add("ACS_DIAMOND", f_ACS_DIAMOND);
   builtinFunctions.add("ACS_CKBOARD", f_ACS_CKBOARD);
   builtinFunctions.add("ACS_DEGREE", f_ACS_DEGREE);
   builtinFunctions.add("ACS_PLMINUS", f_ACS_PLMINUS);
   builtinFunctions.add("ACS_BULLET", f_ACS_BULLET);
   builtinFunctions.add("ACS_LARROW", f_ACS_LARROW);
   builtinFunctions.add("ACS_RARROW", f_ACS_RARROW);
   builtinFunctions.add("ACS_DARROW", f_ACS_DARROW);
   builtinFunctions.add("ACS_UARROW", f_ACS_UARROW);
   builtinFunctions.add("ACS_BOARD", f_ACS_BOARD);
   builtinFunctions.add("ACS_LANTERN", f_ACS_LANTERN);
   builtinFunctions.add("ACS_BLOCK", f_ACS_BLOCK);

#ifdef NCURSES_MOUSE_VERSION
   builtinFunctions.add("mousemask", f_mousemask);
   builtinFunctions.add("getmouse",  f_getmouse);
#endif // NCURSES_MOUSE_VERSION

   traceout("ncurses_module_init()");
   return 0;
}

void ncurses_module_ns_init(class QoreNamespace *rns, class QoreNamespace *qns)
{
   qns->addInitialNamespace(NCNS->copy());
}

void ncurses_module_delete()
{
   tracein("ncurses_module_delete()");
   q_nc_init.close();
   delete NCNS;
   traceout("ncurses_module_delete()");
}
