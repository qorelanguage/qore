/*
  macros-itanium.h

  assembly macros for the Itanium (IA-64) architecture

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

