#ifndef _QORE_MACHINE_MACROS_H

#define _QORE_MACHINE_MACROS_H

// no atomic support or stack guard for 64-bit sparc yet
#if !defined(__sparcv9)

#define STACK_DIRECTION_DOWN 1

#ifdef __GNUC__

#define HAVE_ATOMIC_MACROS

// borrowed from boost
inline int compare_and_swap(int *dest_, int compare_, int swap_) {
   __asm__ __volatile__( "cas %0, %2, %1" 
                         : "+m" (*dest_), "+r" (swap_) 
                         : "r" (compare_) 
                         : "memory" ); 
 
   return swap_; 
}

inline int atomic_fetch_and_add(int * pw, int dv) {
   for( ;; ) {
      int r = *pw;

      if (__builtin_expect((compare_and_swap(pw, r, r + dv) == r), 1)) {
	 return r;
      }
   }
}

static inline int atomic_dec(int *pw) {
   return !atomic_fetch_and_add(pw, -1);
}

static inline void atomic_inc(int *pw) {
    atomic_fetch_and_add(pw, 1);
}

#define HAVE_CHECK_STACK_POS

static inline size_t get_stack_pos() {
   size_t addr;
   __asm__("mov %%sp,%0" : "=r" (addr) );
   return addr;
}

#endif

#ifdef __SUNPRO_CC
#define HAVE_ATOMIC_MACROS

// routines are implemented in assembler
extern "C" int atomic_dec(int *pw);
extern "C" void atomic_inc(int *pw);

#define HAVE_CHECK_STACK_POS

extern "C" size_t get_stack_pos();

#endif

#endif

#endif
