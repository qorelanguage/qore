/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AstParserHolder.h

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

#ifndef _QLS_ASTPARSERHOLDER_H
#define _QLS_ASTPARSERHOLDER_H

#include <ostream>
#include <string>

#include "qore/Qore.h"

class AstParser;
class ASTParseError;
class ASTTree;

class AstParserHolder : public AbstractPrivateData {
private:
    AstParser* parser;

public:
    AstParserHolder();
    ~AstParserHolder();

    ASTTree* parseFile(const char* filename);
    ASTTree* parseFile(std::string& filename);

    ASTTree* parseString(const char* str);
    ASTTree* parseString(std::string& str);

    //! Get the count of reported errors.
    size_t getErrorCount() const;

    //! Get a reported error.
    ASTParseError* getError(unsigned int index);
};

#endif // _QLS_ASTPARSERHOLDER_H
