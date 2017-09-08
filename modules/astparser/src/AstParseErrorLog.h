/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AstParseErrorLog.h

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
*/

#ifndef _QLS_ASTPARSEERRORLOG_H
#define _QLS_ASTPARSEERRORLOG_H

#include <vector>

#include "ast/ASTParseLocation.h"
#include "ast/ASTParseError.h"

class AstParseErrorLog {
private:
    std::vector<ASTParseError*> errors;

public:
    AstParseErrorLog() {}
    ~AstParseErrorLog() {
        clear();
    }

    //! Clear all the reported errors.
    void clear() {
        for (size_t i = 0, count = errors.size(); i < count; i++)
            delete errors[i];
        errors.clear();
    }

    //! Get the count of reported errors.
    size_t getErrorCount() const { return errors.size(); }

    //! Report a new error.
    void reportError(const ASTParseLocation& loc, const char* str) {
        errors.push_back(new ASTParseError(loc, str));
    }

    //! Get a reported error.
    ASTParseError* getError(size_t index) {
        return (index >= errors.size()) ? nullptr : errors[index];
    }
};

#endif // _QLS_ASTPARSEERRORLOG_H
