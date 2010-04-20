/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
 qore_bit_opts.h
 
 Qore Programming Language
 
 Copyright 2003 - 2010 David Nichols
 
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

#ifndef _QORE_BITOPTS_H

#define _QORE_BITOPTS_H

#include <qore/common.h>

#include <arpa/inet.h>

static inline int64 i8LSB(int64 i);
static inline int   i4LSB(int i);
static inline short i2LSB(short i);

static inline int64 LSBi8(int64 i);
static inline int   LSBi4(int i);
static inline short LSBi2(short i);

static inline int64 i8MSB(int64 i);
static inline int64 MSBi8(int64 i);

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

static inline int64 swapi8(int64 i) { 
   char *ibuf = (char *)&i;
   qore_i8_u i8;
   i8.swap(ibuf);
   return i8.i;
}

static inline double swapf8(double f) { 
   char *ibuf = (char *)&f;
   qore_i8_u f8;
   f8.swap(ibuf);
   return f8.f;
}

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

static inline int swapi4(int i) { 
   char *ibuf = (char *)&i;
   qore_i4_u i4;
   i4.swap(ibuf);
   return i4.i;
}

static inline float swapf4(float f) { 
   char *ibuf = (char *)&f;
   qore_i4_u f4;
   f4.swap(ibuf);
   return f4.f;
}

union qore_i2_u {
   char buf[4];
   int i;

   DLLLOCAL void swap(char *ibuf) {
      buf[1] = ibuf[0];
      buf[0] = ibuf[1];
   }
};

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
