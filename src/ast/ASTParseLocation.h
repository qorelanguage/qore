/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTParseLocation.h

  Qore AST Parser

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

#ifndef _QLS_AST_ASTPARSELOCATION_H
#define _QLS_AST_ASTPARSELOCATION_H

struct ASTParseLocation {
    typedef int ast_loc_t;

    ast_loc_t firstLine;
    ast_loc_t firstCol;
    ast_loc_t lastLine;
    ast_loc_t lastCol;

    ASTParseLocation() :
        firstLine(0),
        firstCol(0),
        lastLine(0),
        lastCol(0) {}

    ASTParseLocation(const ASTParseLocation& loc) :
        firstLine(loc.firstLine),
        firstCol(loc.firstCol),
        lastLine(loc.lastLine),
        lastCol(loc.lastCol) {}

    ASTParseLocation(ast_loc_t fline, ast_loc_t fcol, ast_loc_t lline, ast_loc_t lcol) :
        firstLine(fline),
        firstCol(fcol),
        lastLine(lline),
        lastCol(lcol) {}
};

#endif // _QLS_AST_ASTPARSELOCATION_H
