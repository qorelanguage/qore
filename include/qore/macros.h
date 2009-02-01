#ifndef _QORE_MACROS_H
#define _QORE_MACROS_H

// include the right assembly macro file for the current architecture

#ifdef __i386
#include <qore/macros-i386.h>
#endif // #ifdef __i386

#ifdef __x86_64
#include <qore/macros-x86_64.h>
#endif // #ifdef __x86_64

#ifdef __sparc
#include <qore/macros-sparc.h>
#endif // #ifdef __sparc

#if defined(__ppc) || defined(__ppc__)
#include <qore/macros-powerpc.h>
#endif // #ifdef __ppc

#ifdef __hppa
#include <qore/macros-parisc.h>
#endif // #ifdef __hppa

#ifdef __ia64
#include <qore/macros-ia64.h>
#endif // #ifdef __ia64

#endif // #ifndef _QORE_MACROS_H

