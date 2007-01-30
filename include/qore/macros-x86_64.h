/*
  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#define HAVE_ATOMIC_MACROS

// returns 1 when counter reaches zero, 0 if not
static inline int atomic_dec(volatile int *a)
{
   unsigned char rc;

   __asm(
        "lock; decl %0; sete %1"
        : "=m" (*a), "=qm" (rc)
	: "m" (*a) : "memory"
      );
   return rc != 0;
}

static inline void atomic_inc(volatile int *a)
{
   __asm(
        "lock; incl %0"
        : "=m" (*a)
   );
}

#endif
