/*
  TibCommandLine.cc

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

#include <Qore.h>

#include "TibCommandLine.h"

#include <stdlib.h>

void TibCommandLine::add_intern(char *str)
{
   // resize array
   if (argc == alloc)
   {
      alloc += 10;
      argv = (char **)realloc(argv, sizeof(char *) * alloc);
   }
   argv[argc] = str;
   //printd(5, "add_intern %d='%s'\n", argc, str);
   argc++;
}

TibCommandLine::~TibCommandLine()
{
   if (argv)
   {
      // delete every other string starting with index 2
      for (int i = 2; i < argc; i += 2)
	 free(argv[i]);
      free(argv);
   }
}

void TibCommandLine::add(const char *key, const char *val)
{	 
   if (!argc)
      // add a dummy entry for the program name
      add_intern("dummy");
   QoreString str;
   add_intern("-system:clientVar");
   str.sprintf("%s=%s", key, val);
   add_intern(str.giveBuffer());
}
