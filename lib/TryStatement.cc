/*
 TryStatement.cc
 
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

#include <qore/Qore.h>
#include <qore/intern/TryStatement.h>
#include <qore/intern/StatementBlock.h>

TryStatement::TryStatement(int start_line, int end_line, class StatementBlock *t, class StatementBlock *c, char *p) : AbstractStatement(start_line, end_line)
{
   try_block = t;
   catch_block = c;
   param = p;
   //finally = f;
}

TryStatement::~TryStatement()
{
   if (param)
      free(param);
   if (try_block)
      delete try_block;
   if (catch_block)
      delete catch_block;
   //if (finally)
   //delete finally;
}

int TryStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink)
{
   AbstractQoreNode *trv = 0;
   
   QORE_TRACE("TryStatement::execImpl()");
   int rc = 0;
   if (try_block)
      rc = try_block->execImpl(&trv, xsink);
   
   /*
    // if thread_exit has been executed
    if (except == (QoreException *)1)
    return rc;
    */
   
   class QoreException *except = xsink->catchException();
   if (except)
   {
      printd(5, "TryStatement::execImpl() entering catch handler, e=%08p\n", except);
      
      if (catch_block)
      {
	 // save exception
	 catchSaveException(except);
	 
	 if (param)	 // instantiate exception information parameter
	    id->instantiate(except->makeExceptionObject());
	 
	 rc = catch_block->execImpl(&trv, xsink);
	 
	 // uninstantiate extra args
	 if (param)
	    id->uninstantiate(xsink);
      }
      else
	 rc = 0;
      
      // delete exception chain
      except->del(xsink);
   }
   /*
    if (finally)
    {
       printd(5, "TryStatement::exec() now executing finally block...\n");
       rc = finally->execImpl(return_value, xsink);
    }
    */
   if (trv) {
      if (*return_value) // NOTE: return value overridden in the catch block!
	 trv->deref(xsink);
      else
	 *return_value = trv;
   }

   return rc;
}

int TryStatement::parseInitImpl(LocalVar *oflag, int pflag)
{
   if (try_block)
      try_block->parseInitImpl(oflag, pflag);
   
   // prepare catch block and params
   if (param)
   {
      id = push_local_var(param);
      printd(3, "TryStatement::parseInitImpl() reg. local var %s (id=%08p)\n", param, id);
   }
   else
      id = 0;
   
   // initialize code block
   if (catch_block)
      catch_block->parseInitImpl(oflag, pflag | PF_RETHROW_OK);
   
   // pop local param from stack
   if (param)
      pop_local_var();

   return 0;
}

