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

typedef int ast_loc_t;

struct ASTParseLocation {
    ast_loc_t firstLine;
    ast_loc_t firstCol;
    ast_loc_t lastLine;
    ast_loc_t lastCol;

    // Needed for correct string and regex locations.
    ast_loc_t savedFirstLine = 0;
    ast_loc_t savedFirstCol = 0;

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

    //! Set first line and column with first argument and last line and column with second argument.
    void set(const ASTParseLocation& first, const ASTParseLocation& last) {
        firstLine = first.firstLine;
        firstCol  = first.firstCol;
        lastLine  = last.lastLine;
        lastCol   = last.lastCol;
    }

    //! Set first line and column.
    void setFirst(const ASTParseLocation& first) {
        firstLine = first.firstLine;
        firstCol  = first.firstCol;
    }

    //! Set last line and column.
    void setLast(const ASTParseLocation& last) {
        lastLine  = last.lastLine;
        lastCol   = last.lastCol;
    }

    //! Save first line and column to helper vars.
    void saveFirst() {
        savedFirstLine = firstLine;
        savedFirstCol = firstCol;
    }

    //! Restore first line and column from helper vars.
    void restoreFirst() {
        firstLine = savedFirstLine;
        firstCol = savedFirstCol;
    }

    //! Update according to flex parameters.
    void update(int lineno, int leng, const char* text) {
        firstLine = lastLine;
        firstCol = lastCol;
        if (firstLine == lineno)
            lastCol += leng;
        else {
            int col = 1;
            for (; (col <= leng) && (text[leng - col] != '\n'); ++col) {}
            lastCol = col;
            lastLine = lineno;
        }
    }

    //! Check whether the passed line and column are inside this location.
    bool inside(ast_loc_t line, ast_loc_t col) {
        if (line < firstLine) return false;
        if (line > lastLine) return false;
        if (firstLine == lastLine) {
            if (col < firstCol || col >= lastCol)
                return false;
            return true;
        }
        if (line == firstLine) {
            if (col < firstCol)
                return false;
            return true;
        }
        if (line == lastLine) {
            if (col >= lastCol)
                return false;
            return true;
        }
        return true;
    }
};

#endif // _QLS_AST_ASTPARSELOCATION_H
