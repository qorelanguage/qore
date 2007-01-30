/*
  BinaryObject.h

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

#ifndef _QORE_BINARYOBJECT_H

#define _QORE_BINARYOBJECT_H

#include <string.h>
#include <stdlib.h>

class BinaryObject {
   private:
      void *ptr;
      int len;

   public:
      inline BinaryObject(void *p = NULL, int size = 0)
      {
	 ptr = p;
	 len = size;
      }

      inline ~BinaryObject()
      {
	 if (ptr)
	    free(ptr);
      }

      // returns 0 = equal, 1 = not equal
      inline int compare(class BinaryObject *obj) const
      {
	 // if the sizes are not equal, then the objects can't be equal
	 if (len != obj->len)
	    return 1;

	 // if both are zero, then they are equal
	 if (!len)
	    return 0;

	 return memcmp(ptr, obj->ptr, len);
      }

      inline int size() const
      {
	 return len;
      }

      inline class BinaryObject *copy() const
      {
	 if (!len)
	    return new BinaryObject();

	 void *np = malloc(len);
	 memcpy(np, ptr, len);
	 return new BinaryObject(np, len);
      }

      inline void *getPtr() const
      {
	 return ptr;
      }

      inline void append(void *nptr, int size)
      {
	 ptr = realloc(ptr, len + size);
	 memcpy((char *)ptr + len, nptr, size);
	 len += size;
      }

      inline void append(class BinaryObject *b)
      {
	 append(b->ptr, b->len);
      }
};

#endif // _QORE_BINARYOBJECT_H
