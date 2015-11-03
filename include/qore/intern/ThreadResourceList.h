/*
  ThreadResourceList.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#ifndef _QORE_THREADRESOURCELIST_H

#define _QORE_THREADRESOURCELIST_H

class ThreadResourceList {
   private:
      class ThreadResourceNode *head;
   
      DLLLOCAL class ThreadResourceNode *find(AbstractThreadResource *atr);
      DLLLOCAL void removeIntern(class ThreadResourceNode *w);
      DLLLOCAL void setIntern(class ThreadResourceNode *n);

   public:
      DLLLOCAL ThreadResourceList();
      DLLLOCAL ~ThreadResourceList();
   
      DLLLOCAL void set(AbstractThreadResource *atr);
      //returns 0 if not already set, -1 if already set
      DLLLOCAL int setOnce(AbstractThreadResource *atr);
      // returns 0 if removed, -1 if not found
      DLLLOCAL int remove(AbstractThreadResource *atr);
      DLLLOCAL void purge(class ExceptionSink *xsink);
};



#endif
