/*
  support.cc

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

#include <qore/config.h>
#include <qore/support.h>
#include <qore/QoreString.h>
#include <qore/thread.h>
#include <qore/QoreProgram.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef DEBUG
int qore_trace = 0;
int debug = 0;
#include <qore/thread.h>
extern int threads_initialized;
#endif

// function to use to exit the program
void leave(int rc)
{
   exit(rc);
}

int printe(const char *fmt, ...)
{
   va_list args;
   QoreString buf;

   while (true)
   {
      va_start(args, fmt);
      int rc = buf.vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }
		    
   fputs(buf.getBuffer(), stderr);
   fflush(stderr);
   return 0;
}

#ifdef DEBUG
int printd(int level, const char *fmt, ...)
{
   if (level > debug)
      return 0;

   va_list args;
   QoreString buf;

   while (true)
   {
      va_start(args, fmt);
      int rc = buf.vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }

   if (threads_initialized) fprintf(stderr, "TID %d: ", gettid());
   fputs(buf.getBuffer(), stderr);
   fflush(stderr);
   return 0;
}

void trace_function(int code, char *funcname)
{
   if (!qore_trace)
      return;
   if (code == TRACE_IN)
      printe("TID %d: %s entered\n", threads_initialized ? gettid() : 0, funcname);
   else
      printe("TID %d: %s exited\n", threads_initialized ? gettid() : 0, funcname);
}
#endif

char *remove_trailing_newlines(char *str)
{
   int i = strlen(str);
   while (i && (str[i - 1] == '\n'))
      str[--i] = '\0';
   return str;
}

char *remove_trailing_blanks(char *str)
{
   int i = strlen(str);
   while (i && (str[--i] == ' '))
      str[i] = '\0';
   return str;
}

#ifdef DEBUG
void showCallStack()
{
   List *callStack = getCallStack();
   printf("terminated at %s:%d\n", get_pgm_file(), get_pgm_counter());
   if (callStack && callStack->size())
   {
      printe("call stack:\n");
      for (int i = 0; i < callStack->size(); i++)
      {
         class Hash *h = callStack->retrieve_entry(i)->val.hash;
         printe(" %2d: %s() (%s line %d, %s)\n", i + 1,
                h->getKeyValue("function")->val.String->getBuffer(),
                h->getKeyValue("file")->val.String->getBuffer(),
                (int)h->getKeyValue("line")->val.intval,
                h->getKeyValue("type")->val.String->getBuffer());
      }
   }
}

void run_time_error(const char *fmt, ...)
{
   va_list args;
   QoreString buf;

   while (true)
   {
      va_start(args, fmt);
      int rc = buf.vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }

   printe("run-time error in line %d of file \"%s\": ", get_pgm_counter(), get_pgm_file());
   fputs(buf.getBuffer(), stderr);
   fputc('\n', stderr);
   showCallStack();
   fflush(stderr);
   abort();
   //exit(1);
}
#endif

void parse_error(const char *fmt, ...)
{
   printd(5, "parse_error(\"%s\", ...) called\n", fmt);

   class QoreString *desc = new QoreString();
   while (true)
   {
      va_list args;
      va_start(args, fmt);
      int rc = desc->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }
   getProgram()->makeParseException(desc);
}

// returns 1 for success
static inline int tryIncludeDir(class QoreString *dir, char *file)
{
   // make fully-justified path
   if (dir->strlen() && dir->getBuffer()[dir->strlen() - 1] != '/')
      dir->concat('/');
   dir->concat(file);
   struct stat sb;
   //printd(5, "trying \"%s\"\n", dir->getBuffer());
   return !stat(dir->getBuffer(), &sb);
}

// FIXME: this could be a lot more efficient
class QoreString *findFileInEnvPath(char *file, char *varname)
{
   //printd(5, "findFileInEnvPath(file=%s var=%s)\n", file, varname);

   // if the file is an absolute path, then return it
   if (file[0] == '/')
      return new QoreString(file);

   // get path from environment
   char *idir = getenv(varname);

   // if path is empty, return null
   if (!idir)
      return NULL;

   // duplicate string for invasive searches
   QoreString plist(idir);
   idir = plist.getBuffer();
   //printd(5, "findFileInEnvPath() %s=%s\n", varname, idir);

   // try each directory
   while (char *p = strchr(idir, ':'))
   {
      if (p != idir)
      {
	 *p = '\0';
	 QoreString *str = new QoreString(idir);
	 if (tryIncludeDir(str, file))
	    return str;
	 delete str;
      }
      idir = p + 1;
   }

   // try last directory
   if (idir[0])
   {
      QoreString *str = new QoreString(idir);
      if (tryIncludeDir(str, file))
	 return str;
      delete str;
   }

   return NULL;
}
