/*
  Qore.h

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

#ifndef _QORE_QORE_H

#define _QORE_QORE_H

#include <qore/config.h>
#include <qore/common.h>
#include <qore/List.h>
#include <qore/QoreProgram.h>
#include <qore/ModuleManager.h>

extern char qore_version_string[];
extern int qore_version_major;
extern int qore_version_minor;
extern int qore_version_sub;
extern int qore_build_number;
extern char qore_target_os[];
extern char qore_target_arch[];

void qore_init(char *def_charset = NULL, bool show_module_errors = false);
void qore_cleanup();

#endif  // _QORE_QORE_H
