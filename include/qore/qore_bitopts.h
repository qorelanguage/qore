/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_bit_opts.h
 
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

#ifndef _QORE_BITOPTS_H

#define _QORE_BITOPTS_H

#include <qore/common.h>

static inline int64 i8LSB(int64 i);
static inline int   i4LSB(int i);
static inline short i2LSB(short i);

static inline int64 LSBi8(int64 i);
static inline int   LSBi4(int i);
static inline short LSBi2(short i);

static inline int64 i8MSB(int64 i);
static inline int64 MSBi8(int64 i);

//! used to swap byte order of 8-byte values
union qore_i8_u {
   char buf[8];
   int64 i;
   double f;

   DLLLOCAL void swap(char *ibuf) {
      buf[7] = ibuf[0];
      buf[6] = ibuf[1];
      buf[5] = ibuf[2];
      buf[4] = ibuf[3];
      buf[3] = ibuf[4];
      buf[2] = ibuf[5];
      buf[1] = ibuf[6];
      buf[0] = ibuf[7];
   }
};

//! swaps byte order of 8-byte integer values
static inline int64 swapi8(int64 i) { 
   char *ibuf = (char *)&i;
   qore_i8_u i8;
   i8.swap(ibuf);
   return i8.i;
}

//! swaps byte order of 8-byte floating-point values
static inline double swapf8(double f) { 
   char *ibuf = (char *)&f;
   qore_i8_u f8;
   f8.swap(ibuf);
   return f8.f;
}

//! used to swap byte order of 4-byte values
union qore_i4_u {
   char buf[4];
   int i;
   float f;

   DLLLOCAL void swap(char *ibuf) {
      buf[3] = ibuf[0];
      buf[2] = ibuf[1];
      buf[1] = ibuf[2];
      buf[0] = ibuf[3];
   }
};

//! swaps byte order of 8-byte integer values
static inline int swapi4(int i) { 
   char *ibuf = (char *)&i;
   qore_i4_u i4;
   i4.swap(ibuf);
   return i4.i;
}

//! swaps byte order of 4-byte floating-point values
static inline float swapf4(float f) { 
   char *ibuf = (char *)&f;
   qore_i4_u f4;
   f4.swap(ibuf);
   return f4.f;
}

//! used to swap 2-byte integers
union qore_i2_u {
   char buf[2];
   short i;

   DLLLOCAL void swap(char *ibuf) {
      buf[1] = ibuf[0];
      buf[0] = ibuf[1];
   }
};

//! swaps byte order of 2-byte integer values
static inline short swapi2(short i) { 
   char *ibuf = (char *)&i;
   qore_i2_u i2;
   i2.swap(ibuf);
   return i2.i;
}

#ifdef WORDS_BIGENDIAN
static inline int64 i8LSB(int64 i) {
   return swapi8(i);
}

static inline int i4LSB(int i) {
   return swapi4(i);
}

static inline short i2LSB(short i) {
   return swapi2(i);
}

static inline int64 LSBi8(int64 i) {
   return swapi8(i);
}

static inline int LSBi4(int i) {
   return swapi4(i);
}

static inline short LSBi2(short i) { 
   return swapi2(i);
}

static inline int64 i8MSB(int64 i) { return i; }
static inline int64 MSBi8(int64 i) { return i; }

static inline double f8LSB(double f) {
   return swapf8(f);
}

static inline float f4LSB(float f) {
   return swapf4(f);
}

static inline double LSBf8(double f) { 
   return swapf8(f);
}

static inline float LSBf4(float f) {
   return swapf4(f);
}

static inline double f8MSB(double f) { return f; }
static inline double MSBf8(double f) { return f; }
static inline float f4MSB(float f)   { return f; }
static inline float MSBf4(float f)   { return f; }

#else  // definitions for little endian machines below

static inline int64 i8LSB(int64 i) { return i; }
static inline int   i4LSB(int i)   { return i; }
static inline short i2LSB(short i) { return i; }

static inline int64 LSBi8(int64 i) { return i; }
static inline int   LSBi4(int i)   { return i; }
static inline short LSBi2(short i) { return i; }

static inline int64 i8MSB(int64 i) { 
   return swapi8(i);
}

static inline int64 MSBi8(int64 i) { 
   return swapi8(i);
}

static inline double f8LSB(double f) { return f; }
static inline float  f4LSB(float f)  { return f; }

static inline double LSBf8(double f) { return f; }
static inline float  LSBf4(float f)  { return f; }

static inline double f8MSB(double f) { 
   return swapf8(f);
}

static inline double MSBf8(double f) { 
   return swapf8(f);
}

static inline float f4MSB(float f) { 
   return swapf4(f);
}

static inline float MSBf4(float f) { 
   return swapf4(f);
}

#endif

#endif
