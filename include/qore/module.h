/*
  module.h

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

#ifndef _QORE_MODULE_H

#define _QORE_MODULE_H

#define QORE_MODULE_API_MAJOR 0
#define QORE_MODULE_API_MINOR 0

// NOTE: do not use MODULE_INIT and MODULE_DELETE anymore, define 

// macros for defining module initialization and deletion functions
#if !defined(QORE_MONOLITHIC)

#if defined(__GNUC__)

#define MODULE_INIT(a) __attribute__((constructor)) void module_init()
#define MODULE_DELETE(a) __attribute__((destructor)) void module_delete()

#elif defined(__SUNPRO_CC)

void module_init();
#pragma init(module_init)
#define MODULE_INIT(a) void module_init(void)

void module_delete();
#pragma fini(module_delete)
#define MODULE_DELETE(a) void module_delete(void)

#else
#error "don't know how to initialize modules for this compiler!"
#endif

#else  // QORE_MONÒLITHIC

#define MODULE_INIT(a) void (a)(void)
#define MODULE_DELETE(a) void (a)(void)

#endif // !QORE_MONÒLITHIC

#endif // _QORE_MODULE_H
