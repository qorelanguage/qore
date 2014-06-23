/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  macros-itanium.h

  assembly macros for the Itanium (IA-64) architecture

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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

#define STACK_DIRECTION_DOWN 1

#ifdef __GNUC__
#ifdef __LP64__

#define HAVE_ATOMIC_MACROS
#define HAVE_CHECK_STACK_POS

// 64-bit IA-64 atomic operations borrowed from linux
#define ia64_cmpxchg4_acq(ptr, new, old) ({                              \
   unsigned long ia64_intri_res;                                         \
   asm volatile ("mov ar.ccv=%0;;" :: "rO"(old));                        \
   asm volatile ("cmpxchg4.acq %0=[%1],%2,ar.ccv":                       \
		 "=r"(ia64_intri_res) : "r"(ptr), "r"(new) : "memory");  \
   (int)ia64_intri_res;							 \
})

static __inline__ int ia64_atomic_add (int i, volatile int *v) {
   int old, vnew;

   do {
      old = *v;
      vnew = old + i;
   } while (ia64_cmpxchg4_acq(v, vnew, old) != old);
   return vnew;
}

static __inline__ int ia64_atomic_sub (int i, volatile int *v) {
   int old, vnew;

   do {
      old = *v;
      vnew = old - i;
   } while (ia64_cmpxchg4_acq(v, vnew, old) != old);
   return vnew;
}

static inline void atomic_inc(volatile int *a) {
   ia64_atomic_add(1, a);
}

// returns 1 when counter reaches zero, 0 if not
static inline int atomic_dec(volatile int *a) {
   return !ia64_atomic_sub(1, a);
}

static inline size_t get_stack_pos() {
  size_t addr;
  asm volatile ("mov %0=sp" : "=r" (addr));
  return addr;
}

static inline size_t get_rse_bsp() {
  size_t addr;
  asm volatile ("mov %0=ar.bsp" : "=r" (addr));
  return addr;
}

#endif  // #ifdef __LP64__
#endif  // #ifdef __GNUC__

#ifdef __HP_aCC
#ifdef __LP64__

#define HAVE_ATOMIC_MACROS
#define HAVE_CHECK_STACK_POS

// these routines are implemented in assembler
extern "C" void atomic_inc(int *v);
extern "C" int atomic_dec(int *v);
extern "C" size_t get_stack_pos();
extern "C" size_t get_rse_bsp(); // get ia64 Register Stack Engine backing store pointer

#endif  // #ifdef __LP64__
#endif  // #ifdef __HP_aCC

#endif  // #ifndef _QORE_MACHINE_MACROS_H

