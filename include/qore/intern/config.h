/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  config.h

  Qore Programming Language

  Copyright (C) 2005 - 2013 David Nichols

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

#ifndef QORE_CONFIG_H

#define QORE_CONFIG_H

#ifdef __CYGWIN__
#define __int64 long long
#endif

#ifdef HAVE_UNIX_CONFIG_H
#include <qore/intern/unix-config.h>
#else
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <qore/intern/win32-config.h>
#else
#error no configuration file for this build
#endif
#endif

#endif
