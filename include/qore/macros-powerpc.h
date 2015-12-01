/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

#ifndef _QORE_CONFIG_MACHINE_MACROS_H

#define _QORE_CONFIG_MACHINE_MACROS_H

// only have support for 32-bit ppc for now
#ifndef __ppc64

#define HAVE_ATOMIC_MACROS

static inline int atomic_inc(int *v) {
   int t;

   asm volatile(
                "1:     lwarx   %0,0,%1\n"
                "	addic   %0,%0,1\n"
                "	stwcx.  %0,0,%1\n"
                "	bne-    1b"
                : "=&r" (t)
                : "r" (v)
                : "cc", "memory");
   return t;
}

static inline int atomic_dec(int *v) {
   int t;

   asm volatile(
                "1:     lwarx   %0,0,%1\n"
                "	addic   %0,%0,-1\n"
                "	stwcx.  %0,0,%1\n"
                "	bne-    1b"
                : "=&r" (t)
                : "r" (v)
                : "cc", "memory");
   return !t;
}

#define HAVE_CHECK_STACK_POS
#define STACK_DIRECTION_DOWN 1

static inline size_t get_stack_pos() {
   size_t addr;
   __asm("mr %0, r1" : "=r" (addr) );
   return addr;
}

#endif

#endif
