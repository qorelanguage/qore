/*
  Sequence.h
  
  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#ifndef _QORE_SEQUENCE_H

#define _QORE_SEQUENCE_H

#include <qore/LockedObject.h>

class Sequence : public LockedObject
{
   private:
      int val;

   public:
      inline Sequence(int start = 0);
      inline int next();
      inline int getCurrent() { return val; }
};

inline Sequence::Sequence(int start)
{
   val = start;
}

inline int Sequence::next()
{
   lock();
   int rc = val++;
   unlock();
   return rc;
}

#endif
