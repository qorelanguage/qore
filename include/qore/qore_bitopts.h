/*
 qore_bit_opts.h
 
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

static inline double f8MSB(double f8)
{
   int64 val = i8MSB(*((int64 *)&f8));
   return *((double *)&val);
}

static inline double MSBf8(double f8)
{
   int64 val = MSBi8(*((int64 *)&f8));
   return *((double *)&val);
}

static inline float f4MSB(float f4)
{
   int val = htonl(*((int *)&f4));
   return *((float *)&val);
}

static inline float MSBf4(float f4)
{
   int val = ntohl(*((int *)&f4));
   return *((float *)&val);
}

static inline int64 swapi8(int64 i)
{ 
   char obuf[8];
   char *ibuf = (char *)&i;
   obuf[7] = ibuf[0];
   obuf[6] = ibuf[1];
   obuf[5] = ibuf[2];
   obuf[4] = ibuf[3];
   obuf[3] = ibuf[4];
   obuf[2] = ibuf[5];
   obuf[1] = ibuf[6];
   obuf[0] = ibuf[7];
   
   return *((int64 *)obuf);
}

static inline int swapi4(int i)
{ 
   char obuf[4];
   char *ibuf = (char *)&i;
   obuf[3] = ibuf[0];
   obuf[2] = ibuf[1];
   obuf[1] = ibuf[2];
   obuf[0] = ibuf[3];
   
   return *((int *)obuf);
}

static inline short swapi2(short i)
{ 
   char obuf[2];
   char *ibuf = (char *)&i;
   obuf[1] = ibuf[0];
   obuf[0] = ibuf[1];
   
   return *((short *)obuf);
}

#ifdef WORDS_BIGENDIAN
static inline int64 i8LSB(int64 i)
{
   return swapi8(i);
}

static inline int i4LSB(int i)
{
   return swapi4(i);
}

static inline short i2LSB(short i)
{
   return swapi2(i);
}

static inline int64 LSBi8(int64 i)
{ 
   return swapi8(i);
}

static inline int LSBi4(int i)
{
   return swapi4(i);
}

static inline short LSBi2(short i)
{ 
   return swapi2(i);
}

static inline int64 i8MSB(int64 i) { return i; }
static inline int64 MSBi8(int64 i) { return i; }

#else  // definitions for little endian machines below

static inline int64 i8LSB(int64 i) { return i; }
static inline int   i4LSB(int i)   { return i; }
static inline short i2LSB(short i) { return i; }

static inline int64 LSBi8(int64 i) { return i; }
static inline int   LSBi4(int i)   { return i; }
static inline short LSBi2(short i) { return i; }

static inline int64 i8MSB(int64 i)
{ 
   return swapi8(i);
}

static inline int64 MSBi8(int64 i) 
{ 
   return swapi8(i);
}

#endif

#endif
