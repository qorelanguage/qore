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
#include <qore/BuiltinFunctionList.h>
#include <qore/params.h>
#include <qore/ModuleManager.h>
#include <qore/Hash.h>

#include "ncurses-module.h"
#include "QC_Window.h"
#include "QC_Panel.h"

#include <curses.h>

#ifndef QORE_MONOLITHIC
char qore_module_name[] = "ncurses";
char qore_module_version[] = "0.1";
char qore_module_description[] = "ncurses class module";
char qore_module_author[] = "David Nichols";
char qore_module_url[] = "http://qore.sourceforge.net";
int qore_module_api_major = QORE_MODULE_API_MAJOR;
int qore_module_api_minor = QORE_MODULE_API_MINOR;
qore_module_init_t qore_module_init = ncurses_module_init;
qore_module_ns_init_t qore_module_ns_init = ncurses_module_ns_init;
qore_module_delete_t qore_module_delete = ncurses_module_delete;
#endif

class LockedObject lUpdate, lGetch;

static inline void init_colors()
{
   for (int i = 0; i < COLORS; i++)
      for (int j = 0; j < COLORS; j++)
	 init_pair(i * COLORS + j, i, j);
}

static inline void init_constants(class Namespace *ns)
{
   // colors
   ns->addConstant("COLOR_BLACK", new QoreNode((int64)COLOR_BLACK));
   ns->addConstant("COLOR_RED", new QoreNode((int64)COLOR_RED));
   ns->addConstant("COLOR_GREEN", new QoreNode((int64)COLOR_GREEN));
   ns->addConstant("COLOR_YELLOW", new QoreNode((int64)COLOR_YELLOW));
   ns->addConstant("COLOR_BLUE", new QoreNode((int64)COLOR_BLUE));
   ns->addConstant("COLOR_MAGENTA", new QoreNode((int64)COLOR_MAGENTA));
   ns->addConstant("COLOR_CYAN", new QoreNode((int64)COLOR_CYAN));
   ns->addConstant("COLOR_WHITE", new QoreNode((int64)COLOR_WHITE));

   // keys
   ns->addConstant("KEY_BREAK", new QoreNode((int64)KEY_BREAK));
   ns->addConstant("KEY_DOWN", new QoreNode((int64)KEY_DOWN));
   ns->addConstant("KEY_UP", new QoreNode((int64)KEY_UP));
   ns->addConstant("KEY_LEFT", new QoreNode((int64)KEY_LEFT));
   ns->addConstant("KEY_RIGHT", new QoreNode((int64)KEY_RIGHT));
   ns->addConstant("KEY_HOME", new QoreNode((int64)KEY_HOME));
   ns->addConstant("KEY_BACKSPACE", new QoreNode((int64)KEY_BACKSPACE));
   ns->addConstant("KEY_F0", new QoreNode((int64)KEY_F0));
   ns->addConstant("KEY_F1", new QoreNode((int64)KEY_F(1)));
   ns->addConstant("KEY_F2", new QoreNode((int64)KEY_F(2)));
   ns->addConstant("KEY_F3", new QoreNode((int64)KEY_F(3)));
   ns->addConstant("KEY_F4", new QoreNode((int64)KEY_F(4)));
   ns->addConstant("KEY_F5", new QoreNode((int64)KEY_F(5)));
   ns->addConstant("KEY_F6", new QoreNode((int64)KEY_F(6)));
   ns->addConstant("KEY_F7", new QoreNode((int64)KEY_F(7)));
   ns->addConstant("KEY_F8", new QoreNode((int64)KEY_F(8)));
   ns->addConstant("KEY_F9", new QoreNode((int64)KEY_F(9)));
   ns->addConstant("KEY_F10", new QoreNode((int64)KEY_F(10)));
   ns->addConstant("KEY_F11", new QoreNode((int64)KEY_F(11)));
   ns->addConstant("KEY_F12", new QoreNode((int64)KEY_F(12)));
   ns->addConstant("KEY_DL", new QoreNode((int64)KEY_DL));
   ns->addConstant("KEY_IL", new QoreNode((int64)KEY_IL));
   ns->addConstant("KEY_DC", new QoreNode((int64)KEY_DC));
   ns->addConstant("KEY_IC", new QoreNode((int64)KEY_IC));
   ns->addConstant("KEY_EIC", new QoreNode((int64)KEY_EIC));
   ns->addConstant("KEY_CLEAR", new QoreNode((int64)KEY_CLEAR));
   ns->addConstant("KEY_EOS", new QoreNode((int64)KEY_EOS));
   ns->addConstant("KEY_EOL", new QoreNode((int64)KEY_EOL));
   ns->addConstant("KEY_SF", new QoreNode((int64)KEY_SF));
   ns->addConstant("KEY_SR", new QoreNode((int64)KEY_SR));
   ns->addConstant("KEY_NPAGE", new QoreNode((int64)KEY_NPAGE));
   ns->addConstant("KEY_PPAGE", new QoreNode((int64)KEY_PPAGE));
   ns->addConstant("KEY_STAB", new QoreNode((int64)KEY_STAB));
   ns->addConstant("KEY_CTAB", new QoreNode((int64)KEY_CTAB));
   ns->addConstant("KEY_CATAB", new QoreNode((int64)KEY_CATAB));
   ns->addConstant("KEY_ENTER", new QoreNode((int64)KEY_ENTER));
   ns->addConstant("KEY_SRESET", new QoreNode((int64)KEY_SRESET));
   ns->addConstant("KEY_RESET", new QoreNode((int64)KEY_RESET));
   ns->addConstant("KEY_PRINT", new QoreNode((int64)KEY_PRINT));
   ns->addConstant("KEY_LL", new QoreNode((int64)KEY_LL));
   ns->addConstant("KEY_A1", new QoreNode((int64)KEY_A1));
   ns->addConstant("KEY_A3", new QoreNode((int64)KEY_A3));
   ns->addConstant("KEY_B2", new QoreNode((int64)KEY_B2));
   ns->addConstant("KEY_C1", new QoreNode((int64)KEY_C1));
   ns->addConstant("KEY_C3", new QoreNode((int64)KEY_C3));
   ns->addConstant("KEY_BTAB", new QoreNode((int64)KEY_BTAB));
   ns->addConstant("KEY_BEG", new QoreNode((int64)KEY_BEG));
   ns->addConstant("KEY_CANCEL", new QoreNode((int64)KEY_CANCEL));
   ns->addConstant("KEY_CLOSE", new QoreNode((int64)KEY_CLOSE));
   ns->addConstant("KEY_COMMAND", new QoreNode((int64)KEY_COMMAND));
   ns->addConstant("KEY_COPY", new QoreNode((int64)KEY_COPY));
   ns->addConstant("KEY_CREATE", new QoreNode((int64)KEY_CREATE));
   ns->addConstant("KEY_END", new QoreNode((int64)KEY_END));
   ns->addConstant("KEY_EXIT", new QoreNode((int64)KEY_EXIT));
   ns->addConstant("KEY_FIND", new QoreNode((int64)KEY_FIND));
   ns->addConstant("KEY_HELP", new QoreNode((int64)KEY_HELP));
   ns->addConstant("KEY_MARK", new QoreNode((int64)KEY_MARK));
   ns->addConstant("KEY_MESSAGE", new QoreNode((int64)KEY_MESSAGE));
   ns->addConstant("KEY_MOUSE", new QoreNode((int64)KEY_MOUSE));
   ns->addConstant("KEY_MOVE", new QoreNode((int64)KEY_MOVE));
   ns->addConstant("KEY_NEXT", new QoreNode((int64)KEY_NEXT));
   ns->addConstant("KEY_OPEN", new QoreNode((int64)KEY_OPEN));
   ns->addConstant("KEY_OPTIONS", new QoreNode((int64)KEY_OPTIONS));
   ns->addConstant("KEY_PREVIOUS", new QoreNode((int64)KEY_PREVIOUS));
   ns->addConstant("KEY_REDO", new QoreNode((int64)KEY_REDO));
   ns->addConstant("KEY_REFERENCE", new QoreNode((int64)KEY_REFERENCE));
   ns->addConstant("KEY_REFRESH", new QoreNode((int64)KEY_REFRESH));
   ns->addConstant("KEY_REPLACE", new QoreNode((int64)KEY_REPLACE));
#ifdef KEY_RESIZE
   ns->addConstant("KEY_RESIZE", new QoreNode((int64)KEY_RESIZE));
#endif
   ns->addConstant("KEY_RESTART", new QoreNode((int64)KEY_RESTART));
   ns->addConstant("KEY_RESUME", new QoreNode((int64)KEY_RESUME));
   ns->addConstant("KEY_SAVE", new QoreNode((int64)KEY_SAVE));
   ns->addConstant("KEY_SBEG", new QoreNode((int64)KEY_SBEG));
   ns->addConstant("KEY_SCANCEL", new QoreNode((int64)KEY_SCANCEL));
   ns->addConstant("KEY_SCOMMAND", new QoreNode((int64)KEY_SCOMMAND));
   ns->addConstant("KEY_SCOPY", new QoreNode((int64)KEY_SCOPY));
   ns->addConstant("KEY_SCREATE", new QoreNode((int64)KEY_SCREATE));
   ns->addConstant("KEY_SDC", new QoreNode((int64)KEY_SDC));
   ns->addConstant("KEY_SDL", new QoreNode((int64)KEY_SDL));
   ns->addConstant("KEY_SELECT", new QoreNode((int64)KEY_SELECT));
   ns->addConstant("KEY_SEND", new QoreNode((int64)KEY_SEND));
   ns->addConstant("KEY_SEOL", new QoreNode((int64)KEY_SEOL));
   ns->addConstant("KEY_SEXIT", new QoreNode((int64)KEY_SEXIT));
   ns->addConstant("KEY_SFIND", new QoreNode((int64)KEY_SFIND));
   ns->addConstant("KEY_SHELP", new QoreNode((int64)KEY_SHELP));
   ns->addConstant("KEY_SHOME", new QoreNode((int64)KEY_SHOME));
   ns->addConstant("KEY_SIC", new QoreNode((int64)KEY_SIC));
   ns->addConstant("KEY_SLEFT", new QoreNode((int64)KEY_SLEFT));
   ns->addConstant("KEY_SMESSAGE", new QoreNode((int64)KEY_SMESSAGE));
   ns->addConstant("KEY_SMOVE", new QoreNode((int64)KEY_SMOVE));
   ns->addConstant("KEY_SNEXT", new QoreNode((int64)KEY_SNEXT));
   ns->addConstant("KEY_SOPTIONS", new QoreNode((int64)KEY_SOPTIONS));
   ns->addConstant("KEY_SPREVIOUS", new QoreNode((int64)KEY_SPREVIOUS));
   ns->addConstant("KEY_SPRINT", new QoreNode((int64)KEY_SPRINT));
   ns->addConstant("KEY_SREDO", new QoreNode((int64)KEY_SREDO));
   ns->addConstant("KEY_SREPLACE", new QoreNode((int64)KEY_SREPLACE));
   ns->addConstant("KEY_SRIGHT", new QoreNode((int64)KEY_SRIGHT));
   ns->addConstant("KEY_SRSUME", new QoreNode((int64)KEY_SRSUME));
   ns->addConstant("KEY_SSAVE", new QoreNode((int64)KEY_SSAVE));
   ns->addConstant("KEY_SSUSPEND", new QoreNode((int64)KEY_SSUSPEND));
   ns->addConstant("KEY_SUNDO", new QoreNode((int64)KEY_SUNDO));
   ns->addConstant("KEY_SUSPEND", new QoreNode((int64)KEY_SUSPEND));
   ns->addConstant("KEY_UNDO", new QoreNode((int64)KEY_UNDO));

   // attributes
   ns->addConstant("A_NORMAL", new QoreNode((int64)A_NORMAL));           // Normal mode
   ns->addConstant("A_STANDOUT", new QoreNode((int64)A_STANDOUT));       // Best highlighting mode of the terminal
   ns->addConstant("A_UNDERLINE", new QoreNode((int64)A_UNDERLINE));     // Underline mode
   ns->addConstant("A_REVERSE", new QoreNode((int64)A_REVERSE));         // Reverse video
   ns->addConstant("A_BLINK", new QoreNode((int64)A_BLINK));             // Blinking
   ns->addConstant("A_DIM", new QoreNode((int64)A_DIM));                 // Half bright
   ns->addConstant("A_BOLD", new QoreNode((int64)A_BOLD));               // Extra bright or bold
   ns->addConstant("A_PROTECT", new QoreNode((int64)A_PROTECT));         // Protected mode
   ns->addConstant("A_INVIS", new QoreNode((int64)A_INVIS));             // Invisible mode
   ns->addConstant("A_ALTCHARSET", new QoreNode((int64)A_ALTCHARSET));   // Alternate character set
   ns->addConstant("A_CHARTEXT", new QoreNode((int64)A_ALTCHARSET));     // Bit-mask to extract a character

#ifdef NCURSES_MOUSE_VERSION
   // mouse events
   ns->addConstant("BUTTON1_PRESSED",        new QoreNode((int64)BUTTON1_PRESSED));
   ns->addConstant("BUTTON1_RELEASED",       new QoreNode((int64)BUTTON1_RELEASED));
   ns->addConstant("BUTTON1_CLICKED",        new QoreNode((int64)BUTTON1_CLICKED));
   ns->addConstant("BUTTON1_DOUBLE_CLICKED", new QoreNode((int64)BUTTON1_DOUBLE_CLICKED));
   ns->addConstant("BUTTON1_TRIPLE_CLICKED", new QoreNode((int64)BUTTON1_TRIPLE_CLICKED));
   ns->addConstant("BUTTON2_PRESSED",        new QoreNode((int64)BUTTON2_PRESSED));
   ns->addConstant("BUTTON2_RELEASED",       new QoreNode((int64)BUTTON2_RELEASED));
   ns->addConstant("BUTTON2_CLICKED",        new QoreNode((int64)BUTTON2_CLICKED));
   ns->addConstant("BUTTON2_DOUBLE_CLICKED", new QoreNode((int64)BUTTON2_DOUBLE_CLICKED));
   ns->addConstant("BUTTON2_TRIPLE_CLICKED", new QoreNode((int64)BUTTON2_TRIPLE_CLICKED));
   ns->addConstant("BUTTON3_PRESSED",        new QoreNode((int64)BUTTON3_PRESSED));
   ns->addConstant("BUTTON3_RELEASED",       new QoreNode((int64)BUTTON3_RELEASED));
   ns->addConstant("BUTTON3_CLICKED",        new QoreNode((int64)BUTTON3_CLICKED));
   ns->addConstant("BUTTON3_DOUBLE_CLICKED", new QoreNode((int64)BUTTON3_DOUBLE_CLICKED));
   ns->addConstant("BUTTON3_TRIPLE_CLICKED", new QoreNode((int64)BUTTON3_TRIPLE_CLICKED));
   ns->addConstant("BUTTON4_PRESSED",        new QoreNode((int64)BUTTON4_PRESSED));
   ns->addConstant("BUTTON4_RELEASED",       new QoreNode((int64)BUTTON4_RELEASED));
   ns->addConstant("BUTTON4_CLICKED",        new QoreNode((int64)BUTTON4_CLICKED));
   ns->addConstant("BUTTON4_DOUBLE_CLICKED", new QoreNode((int64)BUTTON4_DOUBLE_CLICKED));
   ns->addConstant("BUTTON4_TRIPLE_CLICKED", new QoreNode((int64)BUTTON4_TRIPLE_CLICKED));
   ns->addConstant("BUTTON_SHIFT",           new QoreNode((int64)BUTTON_SHIFT));
   ns->addConstant("BUTTON_CTRL",            new QoreNode((int64)BUTTON_CTRL));
   ns->addConstant("BUTTON_ALT",             new QoreNode((int64)BUTTON_ALT));
   ns->addConstant("ALL_MOUSE_EVENTS",       new QoreNode((int64)ALL_MOUSE_EVENTS));
   ns->addConstant("REPORT_MOUSE_POSITION",  new QoreNode((int64)REPORT_MOUSE_POSITION));
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

static class QoreNode *f_initscr(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return NULL;
}

static class QoreNode *f_printw(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   QoreString *str = q_sprintf(params, 0, 0, xsink); 

   int rc = printw(str->getBuffer());
   delete str;
   return new QoreNode((int64)rc);
}

static class QoreNode *f_refresh(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   lUpdate.lock();
   int rc = refresh();
   lUpdate.unlock();

   return new QoreNode((int64)rc);
}

static class QoreNode *f_doupdate(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   lUpdate.lock();
   int rc = doupdate();
   lUpdate.unlock();

   return new QoreNode((int64)rc);
}

static class QoreNode *f_getch(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   lGetch.lock();
   int rc = getch();
   lGetch.unlock();
   return new QoreNode((int64)rc);
}

static class QoreNode *f_endwin(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   q_nc_init.close();
   return NULL;
}

static class QoreNode *f_cbreak(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   return new QoreNode((int64)cbreak());
}

static class QoreNode *f_nocbreak(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   return new QoreNode((int64)nocbreak());
}

static class QoreNode *f_echo(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   return new QoreNode((int64)echo());
}

static class QoreNode *f_noecho(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   return new QoreNode((int64)noecho());
}

static class QoreNode *f_raw(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   return new QoreNode((int64)raw());
}

static class QoreNode *f_noraw(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();

   return new QoreNode((int64)noraw());
}

static class QoreNode *f_noqiflush(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   noqiflush();
   return NULL;
}

static class QoreNode *f_qiflush(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   qiflush();
   return NULL;
}

static class QoreNode *f_halfdelay(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   class QoreNode *p0 = get_param(params, 0);
   int d = p0 ? p0->getAsInt() : 0;
   if (!d)
      return NULL;

   return new QoreNode((int64)halfdelay(d));
}

static class QoreNode *f_curs_set(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   class QoreNode *p0 = get_param(params, 0);

   return new QoreNode((int64)curs_set(p0 ? p0->getAsInt() : 0));
}

static class QoreNode *f_def_prog_mode(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)def_prog_mode());
}

static class QoreNode *f_reset_prog_mode(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)reset_prog_mode());
}

static class QoreNode *f_def_shell_mode(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)def_shell_mode());
}

static class QoreNode *f_reset_shell_mode(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)reset_shell_mode());
}

static class QoreNode *f_beep(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)beep());
}

static class QoreNode *f_flash(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)flash());
}

static class QoreNode *f_has_colors(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)has_colors());
}

static class QoreNode *f_get_color_pair(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   class QoreNode *p0 = get_param(params, 0);
   class QoreNode *p1 = get_param(params, 1);
   int fg = p0 ? p0->getAsInt() : 0;
   int bg = p1 ? p1->getAsInt() : 0;

   return new QoreNode((int64)COLOR_PAIR(fg * COLORS + bg));
}

static class QoreNode *f_num_colors(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)COLORS);
}

static class QoreNode *f_nl(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)nl());
}

static class QoreNode *f_nonl(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)nonl());
}

static class QoreNode *f_getLines(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)LINES);
}

static class QoreNode *f_getColumns(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)COLS);
}

// alternate character set functions - these cannot be constants because they are terminal-dependent
// so to initialize them as constants would mean calling initscr() in the module_init function

static class QoreNode *f_ACS_ULCORNER(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_ULCORNER);
}

static class QoreNode *f_ACS_LLCORNER(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_LLCORNER);
}

static class QoreNode *f_ACS_URCORNER(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_URCORNER);
}

static class QoreNode *f_ACS_LRCORNER(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_LRCORNER);
}

static class QoreNode *f_ACS_LTEE(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_LTEE);
}

static class QoreNode *f_ACS_RTEE(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_RTEE);
}

static class QoreNode *f_ACS_BTEE(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_BTEE);
}

static class QoreNode *f_ACS_TTEE(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_TTEE);
}

static class QoreNode *f_ACS_HLINE(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_HLINE);
}

static class QoreNode *f_ACS_VLINE(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_VLINE);
}

static class QoreNode *f_ACS_PLUS(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_PLUS);
}

static class QoreNode *f_ACS_S1(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_S1);
}

static class QoreNode *f_ACS_S9(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_S9);
}

static class QoreNode *f_ACS_DIAMOND(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_DIAMOND);
}

static class QoreNode *f_ACS_CKBOARD(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_CKBOARD);
}

static class QoreNode *f_ACS_DEGREE(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_DEGREE);
}

static class QoreNode *f_ACS_PLMINUS(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_PLMINUS);
}

static class QoreNode *f_ACS_BULLET(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_BULLET);
}

static class QoreNode *f_ACS_LARROW(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_LARROW);
}

static class QoreNode *f_ACS_RARROW(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_RARROW);
}

static class QoreNode *f_ACS_DARROW(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_DARROW);
}

static class QoreNode *f_ACS_UARROW(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_UARROW);
}

static class QoreNode *f_ACS_BOARD(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_BOARD);
}

static class QoreNode *f_ACS_LANTERN(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_LANTERN);
}

static class QoreNode *f_ACS_BLOCK(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   return new QoreNode((int64)ACS_BLOCK);
}

#ifdef NCURSES_MOUSE_VERSION
static class QoreNode *f_mousemask(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   class QoreNode *p0 = get_param(params, 0);
   int d = p0 ? p0->getAsInt() : 0;

   return new QoreNode((int64)mousemask(d, NULL));
}

static class QoreNode *f_getmouse(class QoreNode *params, class ExceptionSink *xsink)
{
   q_nc_init.init();
   MEVENT event;
   
   if (getmouse(&event))
      return NULL;

   class Hash *h = new Hash();
   h->setKeyValue("id",     new QoreNode((int64)event.id), NULL);
   h->setKeyValue("x",      new QoreNode((int64)event.x), NULL);
   h->setKeyValue("y",      new QoreNode((int64)event.y), NULL);
   h->setKeyValue("z",      new QoreNode((int64)event.z), NULL);
   h->setKeyValue("bstate", new QoreNode((int64)event.bstate), NULL);

   return new QoreNode(h);
}
#endif // NCURSES_MOUSE_VERSION

class QoreString *ncurses_module_init()
{
   tracein("ncurses_module_init()");

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
   return NULL;
}

void ncurses_module_ns_init(class Namespace *rns, class Namespace *qns)
{
   class Namespace *NCNS = new Namespace("NCurses");
   NCNS->addSystemClass(initWindowClass());
   NCNS->addSystemClass(initPanelClass());
   init_constants(NCNS);

   qns->addInitialNamespace(NCNS);
}

void ncurses_module_delete()
{
   tracein("ncurses_module_delete()");
   q_nc_init.close();
   traceout("ncurses_module_delete()");
}
