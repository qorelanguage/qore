#ifndef _QORE_MACHINE_MACROS_H

#define _QORE_MACHINE_MACROS_H

// we don't have code for 64-bit pa-risc yet
#if !defined(__LP64__)

#define STACK_DIRECTION_UP 1

#ifdef __GNUC__

/*
#define HAVE_ATOMIC_MACROS

static inline int atomic_dec(int *pw) {
}

static inline void atomic_inc(int *pw) {
}
*/

#define HAVE_CHECK_STACK_POS

static inline size_t get_stack_pos() {
   size_t addr;
   __asm__("copy %%sp,%0" : "=r" (addr) );
   return addr;
}

#endif

#ifdef __HP_aCC

//#define HAVE_ATOMIC_MACROS

// routines are implemented in assembler
//extern "C" int atomic_dec(int *pw);
//extern "C" void atomic_inc(int *pw);

#define HAVE_CHECK_STACK_POS

extern "C" size_t get_stack_pos();

#endif

#endif

#endif
