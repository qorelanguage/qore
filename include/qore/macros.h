/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    Qore Programming Language

    Copyright (C) 2003 - 2024 Qore Technologies, s.r.o

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

#ifndef _QORE_MACROS_H
#define _QORE_MACROS_H

// include the right assembly macro file for the current architecture

#ifdef __x86_64
#include <qore/macros-x86_64.h>
#elif defined(__i386)
#include <qore/macros-i386.h>
#elif defined(__sparc)
#include <qore/macros-sparc.h>
#elif defined(__ppc64) || defined(__ppc64__) || defined(__powerpc64__) || defined(_ARCH_PPC64) || defined(__PPC64__)
#include <qore/macros-ppc64.h>
#elif defined(__ppc) || defined(__ppc__) || defined(__powerpc__) || defined(_ARCH_PPC) || defined(__PPC__)
#include <qore/macros-powerpc.h>
#elif defined(__hppa)
#include <qore/macros-parisc.h>
#elif defined(__ia64)
#include <qore/macros-ia64.h>
#elif defined(__aarch64__)
#include <qore/macros-aarch64.h>
#elif defined(__arm__)
#include <qore/macros-arm.h>
#elif defined(__riscv)
#include <qore/macros-riscv.h>
#elif (defined(__s390__) || defined(__s390x__) || defined(__zarch__))
#include <qore/macros-s390x.h>
#endif

#ifndef HAVE_CHECK_STACK_POS
#error need a solution to get the stack pointer with this compiler and on this CPU architecture
#endif

#endif // #ifndef _QORE_MACROS_H

