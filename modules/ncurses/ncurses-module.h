/*
  modules/ncurses/ncurses.h

  Qore Programming Language

  Copyright (C) 2004, 2005, 2006, 2007 David Nichols

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

#ifndef _QORE_NCURSES_MODULE_H

#define _QORE_NCURSES_MODULE_H

class QoreString *ncurses_module_init();
void ncurses_module_ns_init(class QoreNamespace *rns, class QoreNamespace *qns);
void ncurses_module_delete();

#include <qore/LockedObject.h>

class q_nc_init_class {
   private:
      bool initialized;
      class LockedObject l;

      void init_intern();

   public:
      inline q_nc_init_class() : initialized(false) {}
      void close();
      inline void init()
      {
	 // the "init" variable can only be set once, so we check
	 // it outside the lock, and only if it's then do we grab the lock and 
	 // recheck and initialize if necessary.  This should avoid unnecessary
	 // locking and unlocking (cache syncs, etc) the vast majority of the 
	 // time and still remain thread-safe
	 if (initialized)
	    return;
	 init_intern();
      }
};

extern class q_nc_init_class q_nc_init;


#endif // _QORE_NCURSES_MODULE_H
