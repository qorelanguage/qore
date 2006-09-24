/*
  QoreWarnings.h

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

#ifndef _QORE_QOREWARNINGS_H

#define _QORE_QOREWARNINGS_H

// warnings - must correspond with the string order in QoreProgram.cc
#define QP_WARN_WARNING_MASK_UNCHANGED   (1 << 0)
#define QP_WARN_DUPLICATE_LOCAL_VARS     (1 << 1)
#define QP_WARN_UNKNOWN_WARNING          (1 << 2)
#define QP_WARN_UNDECLARED_VAR           (1 << 3)
#define QP_WARN_DUPLICATE_GLOBAL_VARS    (1 << 4)
#define QP_WARN_UNREACHABLE_CODE         (1 << 5)

// defined in QoreProgram.cc
extern const char *qore_warnings[];
int get_warning_code(char *str);

#endif // _QORE_QOREWARNINGS_H
