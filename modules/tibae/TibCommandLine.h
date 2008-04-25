/*
  TibCommandLine.h

  TIBCO Active Enterprise integration to QORE

  Copyright 2003 - 2008 David Nichols

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

#ifndef _QORE_TIBAE_TIBCOMMANDLINE_H
#define _QORE_TIBAE_TIBCOMMANDLINE_H

class TibCommandLine {

   private:
      DLLLOCAL void add_intern(char *str);

   public:
      char **argv;
      int argc, alloc;

      DLLLOCAL TibCommandLine() : argv(0), argc(0), alloc(0)
      {
      }
      DLLLOCAL ~TibCommandLine();
      DLLLOCAL void add(const char *key, const char *val);
};


#endif
