/*
  QT_backquote.cc

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

#include <qore/Qore.h>
#include <qore/intern/QT_backquote.h>

#include <stdio.h>
#include <errno.h>

class QoreNode *backquote_Eval(class QoreNode *n, class ExceptionSink *xsink)
{
   return backquoteEval(n->val.c_str, xsink);
}

// read block size is 1K
#define READ_BLOCK 1024

class QoreNode *backquoteEval(const char *cmd, ExceptionSink *xsink)
{
   tracein("backquoteEval()");

   // execute command in a new process and read stdout in parent
   FILE *p = popen(cmd, "r");
   if (!p)
   {
      // could not fork or create pipe
      xsink->raiseException("BACKQUOTE-ERROR", strerror(errno));
      traceout("backquoteEval()");
      return NULL;
   }

   // allocate buffer for return value
   QoreString *s = new QoreString();

   // read in result string
   while (1)
   {
      char buf[READ_BLOCK];
      int size = fread(buf, 1, READ_BLOCK, p);

      // break if no data is available or an error occurred
      if (!size || size == -1)
         break;

      s->concat(buf, size);

      // break if there is no more data
      if (size != READ_BLOCK)
         break;
   }

   // wait for child process to terminate and close pipe
   pclose(p);

   QoreNode *rv;
   if (!s->strlen())
   {
      delete s;
      rv = NULL;
   }
   else
      rv = new QoreNode(s);
   traceout("backquoteEval()");
   return rv;
}
