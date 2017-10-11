/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTHashElementExpression.h

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

#ifndef _QLS_AST_EXPRESSIONS_ASTHASHELEMENTEXPRESSION_H
#define _QLS_AST_EXPRESSIONS_ASTHASHELEMENTEXPRESSION_H

#include "ast/ASTExpression.h"

class ASTHashElementExpression : public ASTExpression {
public:
    //! Key expression.
    ASTExpression::Ptr key;

    //! Value expression.
    ASTExpression::Ptr value;

public:
    ASTHashElementExpression(ASTExpression* k, ASTExpression* val) :
        ASTExpression(),
        key(k),
        value(val)
    {
        loc.firstLine = k->loc.firstLine;
        loc.firstCol = k->loc.firstCol;
        loc.lastLine = val->loc.lastLine;
        loc.lastCol = val->loc.lastCol;
    }

    virtual ASTExpressionKind getKind() const override {
        return ASTExpressionKind::AEK_HashElement;
    }
};

#endif // _QLS_AST_EXPRESSIONS_ASTHASHELEMENTEXPRESSION_H
