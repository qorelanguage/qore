/*
  modules/ncurses/ncurses.h

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

#ifndef _QORE_NCURSES_MODULE_H

#define _QORE_NCURSES_MODULE_H

char *ncurses_module_init();
void ncurses_module_ns_init(class Namespace *rns, class Namespace *qns);
void ncurses_module_delete();

void qore_ncurses_init();

#endif // _QORE_NCURSES_MODULE_H
