/*
  ParserSupport.h

  parsing support functions and objects

  Qore Programming language

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

#ifndef _QORE_PARSER_SUPPORT_H

#define _QORE_PARSER_SUPPORT_H

#define HE_TAG_CONST        1
#define HE_TAG_SCOPED_CONST 2

class QoreParserLocation
{
   private:
      bool explicit_first;

   public:
      int first_line, last_line;

      // method defined in scanner.ll
      DLLLOCAL QoreParserLocation();
      // method defined in scanner.ll
      DLLLOCAL void updatePosition(int f);
      DLLLOCAL void setExplicitFirst(int f)
      {
	 first_line = f;
	 explicit_first = true;
      }
};

#define YYLTYPE struct QoreParserLocation

// private interface to bison/flex parser/scannertypedef void *yyscan_t;
typedef void *yyscan_t;
DLLLOCAL extern int yyparse(yyscan_t yyscanner);
DLLLOCAL extern struct yy_buffer_state *yy_scan_string(const char *, yyscan_t scanner);
DLLLOCAL int yylex_init(yyscan_t *scanner);
DLLLOCAL void yyset_in(FILE *in_str, yyscan_t yyscanner);
DLLLOCAL int yylex_destroy(yyscan_t yyscanner);
DLLLOCAL void yyset_lineno (int line_number ,yyscan_t yyscanner );

#endif // _QORE_PARSER_SUPPORT_H
