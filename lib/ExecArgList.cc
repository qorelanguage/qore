/*
 ExecArgList.cc
 
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

#include <qore/config.h>
#include <qore/ExecArgList.h>

#include <string.h>
#include <stdlib.h>

#define ARG_BLOCK 10

char *ExecArgList::getString(char *start, int size)
{
   char *str = (char *)malloc(sizeof(char) * (size + 1));
   strncpy(str, start, size);
   str[size] = '\0';
   //printd(5, "ExecArgList::getString() %d: %s\n", len, str);
   return str;
}

void ExecArgList::addArg(char *str)
{
   // resize args
   if (len == allocated)
   {
      allocated += ARG_BLOCK;
      arg = (char **)realloc(arg, sizeof(char *) * allocated);
   }
   arg[len] = str;
   len++;
}

ExecArgList::ExecArgList(char *str)
{
   allocated = 0;
   len = 0;
   arg = NULL;
   char *start = str, *p = str;
   int quote = 0;
   
   while (*p)
   {
      if (start == p && !quote && (*p == '\'' || *p == '\"'))
      {
	 quote = *p;
	 start = p + 1;
	 continue;
      }
      p++;
      if (quote && (*p == '\'' || *p == '\"') && *p == quote)
      {
	 // move characters down to quote position
	 memmove(p, p + 1, strlen(p));
	 quote = 0;
	 p--;
	 continue;
      }
      else if (!quote && *p == ' ')
      {
	 addArg(getString(start, p - start));
	 start = p + 1;
      }
   }
   if (*start)
      addArg(getString(start, strlen(start)));
   // terminate list
   addArg(NULL);
}

ExecArgList::~ExecArgList()
{
   if (arg)
   {
      for (int i = 0; i < len; i++)
	 if (arg[i])
	    free(arg[i]);
      free(arg);
   }
}

char *ExecArgList::getFile()
{
   if (len)
      return arg[0];
   return NULL;
}

char **ExecArgList::getArgs()
{
   return arg;
}
