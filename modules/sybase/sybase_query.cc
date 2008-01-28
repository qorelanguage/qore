/*
  sybase_query.cc

  Sybase DB layer for QORE
  uses Sybase OpenClient C library

  Qore Programming language

  Copyright (C) 2007 Qore Technologies

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
#include <qore/minitest.hpp>

#include <assert.h>
#include <ctype.h>

#include "sybase_query.h"

// returns 0=OK, -1=error (exception raised)
int sybase_query::init(QoreString *cmd_text, const QoreListNode *args, ExceptionSink *xsink)
{
   m_cmd = cmd_text;

   const char* s = m_cmd->getBuffer();
   QoreString tmp;
   while (*s) {
      char ch = *s++;

      // skip double qouted strings
      if (ch == '"') {
	 for (;;) {
	    ch = *s++;
	    if (ch == '\\') {
	       ch = *s++;
	       continue;
	    }
	    if (ch == '"') {
	       goto next;
	    }
	 }
      }
      // skip single qouted strings
      if (ch == '\'') {
	 for (;;) {
	    ch = *s++;
	    if (ch == '\\') {
	       ch = *s++;
	       continue;
	    }
	    if (ch == '\'') {
	       goto next;
	    }
	 }
      }
      
      if (ch == '%') 
      {
	 int offset = s - m_cmd->getBuffer() - 1;
	 ch = *s++;
	 if (ch == 'v') 
	 {
	    param_list.push_back('v');
	    //param_list.resize(count + 1);
	    //param_list[count++].set(PN_VALUE);
	  
	    tmp.clear();
	    tmp.sprintf("@par%u", param_list.size());
	    m_cmd->replace(offset, 2, &tmp);
	    s = m_cmd->getBuffer() + offset + tmp.strlen();
	 } 
	 else if (ch == 'd') 
	 {
	    class QoreNode *v = args ? args->retrieve_entry(param_list.size()) : 0;
	    tmp.clear();
	    DBI_concat_numeric(&tmp, v);
	    m_cmd->replace(offset, 2, tmp.getBuffer());
	    s = m_cmd->getBuffer() + offset + tmp.strlen();

	    param_list.push_back('d');
	    //param_list.resize(count + 1);
	    //param_list[count++].set(PN_NUMERIC);
	 } 
	 else if (ch == 's') 
	 {
	    class QoreNode *v = args ? args->retrieve_entry(param_list.size()) : 0;
	    tmp.clear();
	    if (DBI_concat_string(&tmp, v, xsink))
	       return -1;
	    m_cmd->replace(offset, 2, tmp.getBuffer());
	    s = m_cmd->getBuffer() + offset + tmp.strlen();
	    
	    // mark it with a 'd' to ensure it gets skipped
	    param_list.push_back('d');
	    //param_list.resize(count + 1);
	    //param_list[count++].set(PN_NUMERIC);
	 } 
	 else
	 {
	    xsink->raiseException("DBI-EXEC-EXCEPTION", "Only %%v or %%d expected in parameter list");
	    return -1;
	 }
      } 
      // read placeholder name
      else if (ch == ':') 
      {
	 int offset = s - m_cmd->getBuffer() - 1;

	 const char* placeholder_start = s;
	 while (isalnum(*s) || *s == '_') ++s;
	 if (s == placeholder_start) {
	    xsink->raiseException("DBI-EXEC-EXCEPTION", "Placeholder name missing after ':'");
	    return -1;
	 }
	 m_cmd->replace(offset, 1, "@");

	 //param_list.resize(count + 1);
	 //param_list[count++].set(placeholder_start, s - placeholder_start);

	 // is this creating a std::string and then copying it and creating another in the vector?
	 placeholder_list.add(placeholder_start, s - placeholder_start);
      }
     next:
	 ;	 
   } // while

   //printd(5, "size=%d, m_cmd=%s\n", param_list.size(), m_cmd->getBuffer());
   return 0;
}

