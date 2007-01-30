/*
 TryStatement.cc
 
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
#include <qore/TryStatement.h>

TryStatement::TryStatement(class StatementBlock *t, class StatementBlock *c, char *p)
{
   try_block = t;
   catch_block = c;
   param = p;
   /*
    finally = f;
    */
}

TryStatement::~TryStatement()
{
   if (param)
      free(param);
   if (try_block)
      delete try_block;
   if (catch_block)
      delete catch_block;
   /*
    if (finally)
    delete finally;
    */
}

// only executed by Statement::exec()
int TryStatement::exec(class QoreNode **return_value, class ExceptionSink *xsink)
{
   QoreNode *trv = NULL;
   
   tracein("TryStatement::exec()");
   int rc = 0;
   if (try_block)
      rc = try_block->exec(&trv, xsink);
   
   /*
    // if thread_exit has been executed
    if (except == (Exception *)1)
    return rc;
    */
   
   class Exception *except = xsink->catchException();
   if (except)
   {
      printd(5, "TryStatement::exec() entering catch handler, e=%08p\n", except);
      
      if (catch_block)
      {
	 // save exception
	 catchSaveException(except);
	 
	 if (param)	 // instantiate exception information parameter
	    instantiateLVar(id, except->makeExceptionObject());
	 
	 rc = catch_block->exec(&trv, xsink);
	 
	 // uninstantiate extra args
	 if (param)
	    uninstantiateLVar(xsink);
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
       rc = finally->exec(return_value, xsink);
    }
    */
   if (trv)
      if (*return_value) // NOTE: double return! (maybe should raise an exception here?)
	 trv->deref(xsink);
      else
	 *return_value = trv;
   traceout("TryStatement::exec()");
   return rc;
}

void TryStatement::parseInit(lvh_t oflag, int pflag)
{
   if (try_block)
      try_block->parseInit(oflag, pflag);
   
   // prepare catch block and params
   if (param)
   {
      id = push_local_var(param);
      printd(3, "TryStatement::parseInit() reg. local var %s (id=%08p)\n", param, id);
   }
   else
      id = NULL;
   
   // initialize code block
   if (catch_block)
      catch_block->parseInit(oflag, pflag | PF_RETHROW_OK);
   
   // pop local param from stack
   if (param)
      pop_local_var();
}

