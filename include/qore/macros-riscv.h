/* -*- mode: c++; indent-tabs-mode: nil -*- */
#ifndef _QORE_MACHINE_MACROS_H

#define _QORE_MACHINE_MACROS_H

#define STACK_DIRECTION_DOWN 1

#ifdef __GNUC__

#define HAVE_CHECK_STACK_POS

static inline size_t get_stack_pos()
{
    size_t addr;
    __asm__("mv %0, sp" : "=r" (addr));
    return addr;
}

#endif

#endif
