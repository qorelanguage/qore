/*
  TryStatement.cpp
 
  Qore Programming Language
 
  Copyright (C) 2003 - 2015 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>
#include <qore/intern/TryStatement.h>
#include <qore/intern/StatementBlock.h>

class CatchExceptionHelper {
private:
   QoreException* e;

public:
   DLLLOCAL CatchExceptionHelper(QoreException* n_e) : e(catchSwapException(n_e)) {
   }

   DLLLOCAL ~CatchExceptionHelper() {
      catchSwapException(e);
   }
};

TryStatement::TryStatement(int start_line, int end_line, class StatementBlock *t, class StatementBlock *c, char *p) : AbstractStatement(start_line, end_line) {
   try_block = t;
   catch_block = c;
   param = p;
   //finally = f;
}

TryStatement::~TryStatement() {
   if (param)
      free(param);
   if (try_block)
      delete try_block;
   if (catch_block)
      delete catch_block;
   //if (finally)
   //delete finally;
}

int TryStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   AbstractQoreNode *trv = 0;
   
   QORE_TRACE("TryStatement::execImpl()");
   int rc = 0;
   if (try_block)
      rc = try_block->execImpl(&trv, xsink);
   
   QoreException* except = xsink->catchException();
   if (except) {
      printd(5, "TryStatement::execImpl() entering catch handler, e=%p\n", except);
      
      if (catch_block) {
	 CatchExceptionHelper ceh(except);
	 
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

   if (trv) {
      if (*return_value) // NOTE: return value overridden in the catch block!
	 trv->deref(xsink);
      else
	 *return_value = trv;
   }

   return rc;
}

int TryStatement::parseInitImpl(LocalVar *oflag, int pflag) {
   // turn off top-level flag for statement vars
   pflag &= (~PF_TOP_LEVEL);

   if (try_block)
      try_block->parseInitImpl(oflag, pflag);
   
   // prepare catch block and params
   if (param) {
      // push as if the variable is already referenced so no warning will be emitted
      // in case the variable is not actually referenced in the catch block
      id = push_local_var(param, loc, 0, true, 1);
      printd(3, "TryStatement::parseInitImpl() reg. local var %s (id=%p)\n", param, id);
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

bool TryStatement::hasFinalReturn() const {
   // this works because try and rethrow both return true for hasFinalReturn
   // because throwing an exception trumpts any return statement
   return try_block && try_block->hasFinalReturn() && catch_block && catch_block->hasFinalReturn();
}
