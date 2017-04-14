/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AstParserSupport.h

  parsing support functions and objects

  Qore Programming language

  Copyright (C) 2017 Qore Technologies, s.r.o.

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

#ifndef _QLS_AST_PARSER_SUPPORT_H
#define _QLS_AST_PARSER_SUPPORT_H

struct AstParserLocation {
public:
   int firstLine;
   int firstCol;
   int lastLine;
   int lastCol;
};

// private interface to bison/flex parser/scanner
typedef void *yyscan_t;
DLLLOCAL extern int astparse(yyscan_t astscanner);
DLLLOCAL extern struct yy_buffer_state *ast_scan_string(const char *, yyscan_t scanner);
DLLLOCAL int astlex_init(yyscan_t *scanner);
DLLLOCAL void astset_in(FILE *in_str, yyscan_t astscanner);
DLLLOCAL int astlex_destroy(yyscan_t astscanner);
DLLLOCAL void astset_lineno(int line_number, yyscan_t astscanner);

#endif // _QLS_AST_PARSER_SUPPORT_H
