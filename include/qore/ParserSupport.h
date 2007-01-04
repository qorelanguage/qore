/*
  ParserSupport.h

  parsing support functions and objects

  Qore Programming language

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

#ifndef _QORE_PARSER_SUPPORT_H

#define _QORE_PARSER_SUPPORT_H

class QoreParserLocation
{
   private:
      bool explicit_first;
   public:
      int first_line;
      int last_line;

      DLLLOCAL QoreParserLocation() : explicit_first(false)
      {
      }
      DLLLOCAL void setExplicitFirst(int f)
      {
	 first_line = f;
	 explicit_first = true;
      }
      DLLLOCAL void setConditionalFirst(int f)
      {
	 if (!explicit_first)
	    first_line = f;
	 else
	    explicit_first = false;
      }
};

class QoreParserContext {
   public:
      void *scanner;
};

#endif // _QORE_PARSER_SUPPORT_H
