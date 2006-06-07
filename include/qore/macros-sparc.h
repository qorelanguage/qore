#ifndef _QORE_MACHINE_MACROS_H

#define _QORE_MACHINE_MACROS_H

#ifdef __GNUC__DISABLED_UNTESTED
#define HAVE_ATOMIC_MACROS

// the following routines have been borrowed from gasnet: http://gasnet.cs.berkeley.edu

static inline void atomic_inc(volatile int *v) 
{
   // requires the cas instruction in Sparc V9, and therefore gcc -mcpu=ultrasparc
   register volatile int *addr = (volatile int *)v;
   register int oldval;
   register int newval;
   __asm__ __volatile__ ( 
      "0:\t" 
      "membar #StoreLoad | #LoadLoad    \n\t" // complete all previous ops before next load
      "ld       [%2],%0 \n\t"    // oldval = *addr;
      "add      %0,1,%1 \n\t"    // newval = oldval + op;
      "cas      [%2],%0,%1 \n\t" // if (*addr == oldval) { *addr = newval; }  newval = *addr;
      "cmp      %0, %1 \n\t"     // check if newval == oldval (swap succeeded)
      "bne      0b \n\t"         // otherwise, try again
      "membar #StoreLoad | #StoreStore    \n\t" // complete previous cas store before all subsequent ops
      : "=&r"(oldval), "=&r"(newval)
      : "r" (addr) 
      : "memory");
}

static inline int atomic_dec(volatile int *v) 
{
   // requires the cas instruction in Sparc V9, and therefore gcc -mcpu=ultrasparc
   register volatile int *addr = (volatile int *)v;
   register int oldval;
   register int newval;
   __asm__ __volatile__ ( 
      "0:\t" 
      "membar #StoreLoad | #LoadLoad    \n\t" // complete all previous ops before next load
      "ld       [%2],%0 \n\t"    // oldval = *addr;
      "add      %0,-1,%1 \n\t"    // newval = oldval + op;
      "cas      [%2],%0,%1 \n\t" // if (*addr == oldval) { *addr = newval; }  newval = *addr;
      "cmp      %0, %1 \n\t"     // check if newval == oldval (swap succeeded)
      "bne      0b \n\t"         // otherwise, try again
      "membar #StoreLoad | #StoreStore    \n\t" // complete previous cas store before all subsequent ops
      : "=&r"(oldval), "=&r"(newval)
      : "r" (addr) 
      : "memory");
   return oldval;
}

#endif

#endif
