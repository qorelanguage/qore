/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  Qore Programming Language

  Copyright (C) 2003 - 2022 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_MACHINE_MACROS_H

#define _QORE_MACHINE_MACROS_H

// we need at least 22K of stack guard on sparc for background threads
#define QORE_STACK_GUARD (1024 * 22)

// no atomic support or stack guard for 64-bit sparc yet
#if !defined(__sparcv9)

#define STACK_DIRECTION_DOWN 1

#ifdef __GNUC__

#define HAVE_CHECK_STACK_POS

static inline size_t get_stack_pos() {
   size_t addr;
   __asm__("mov %%sp,%0" : "=r" (addr) );
   return addr;
}

#endif

#ifdef __SUNPRO_CC

#define HAVE_CHECK_STACK_POS

extern "C" size_t get_stack_pos();

#endif

#endif

#endif
