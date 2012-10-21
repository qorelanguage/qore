/*
 WhileStatement.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2012 David Nichols
 
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
#include <qore/intern/WhileStatement.h>
#include <qore/intern/StatementBlock.h>

WhileStatement::WhileStatement(int start_line, int end_line, AbstractQoreNode *c, class StatementBlock *cd) : AbstractStatement(start_line, end_line) {
   cond = c;
   code = cd;
   lvars = 0;
}

WhileStatement::~WhileStatement() {
   cond->deref(0);
   delete code;
   delete lvars;
}

int WhileStatement::execImpl(AbstractQoreNode **return_value, ExceptionSink *xsink) {
   int rc = 0;
   
   // instantiate local variables
   LVListInstantiator lvi(lvars, xsink);
   
   while (cond->boolEval(xsink) && !xsink->isEvent()) {
      if (code && (((rc = code->execImpl(return_value, xsink)) == RC_BREAK) || xsink->isEvent())) {
	 rc = 0;
	 break;
      }
      if (rc == RC_RETURN)
	 break;
      else if (rc == RC_CONTINUE)
	 rc = 0;
   }

   return rc;
}

int WhileStatement::parseInitImpl(LocalVar *oflag, int pflag) {
   int lvids = 0;
   
   // turn off top-level flag for statement vars
   pflag &= (~PF_TOP_LEVEL);

   if (cond) {
      const QoreTypeInfo *argTypeInfo = 0;
      cond = cond->parseInit(oflag, pflag, lvids, argTypeInfo);
   }
   if (code)
      code->parseInitImpl(oflag, pflag);
   
   // save local variables
   if (lvids)
      lvars = new LVList(lvids);

   return 0;
}
