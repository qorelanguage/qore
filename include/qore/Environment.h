/*
  Environment.h

  Qore Programming Language

  Copyright (C) 2003,2004,2005 David Nichols

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

#ifndef _QORE_ENVIRONMENT_H

#define _QORE_ENVIRONMENT_H

class Environment {
   private:

   public:
      DLLEXPORT Environment();
      DLLEXPORT ~Environment();

      DLLEXPORT int set(const char *name, const char *value, int overwrite = 1);
      DLLEXPORT class QoreString *get(const char *name);
      // appends value to string, returns 0 for OK, -1 for not found
      DLLEXPORT int get(const char *name, class QoreString *str);
      DLLEXPORT int unset(const char *name);
      DLLEXPORT bool valueExists(const char* name); // true if exists and is not empty
};

DLLEXPORT extern Environment Env;

#endif
