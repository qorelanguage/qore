/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTIndexExpression.h

  Qore AST Parser

  Copyright (C) 2023 - 2024 Qore Technologies, s.r.o.

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

#ifndef _QLS_AST_EXPRESSIONS_ASTINDEXEXPRESSION_H
#define _QLS_AST_EXPRESSIONS_ASTINDEXEXPRESSION_H

#include "ast/ASTExpression.h"

class ASTIndexExpression : public ASTExpression {
public:
    enum class IndexKind {
        AIE_SquareBrackets = 0,
        AIE_CurlyBrackets = 1,
    };

public:
    //! Variable being accessed.
    ASTExpression::Ptr variable;

    //! Index.
    ASTExpression::Ptr index;

    //! Type of indexing used.
    IndexKind indexKind;

public:
    ASTIndexExpression(ASTExpression* var, ASTExpression* ie, IndexKind ik) :
        ASTExpression(),
        variable(var),
        index(ie),
        indexKind(ik)
    {
        loc.firstLine = var->loc.firstLine;
        loc.firstCol = var->loc.firstCol;
        loc.lastLine = ie->loc.lastLine;
        loc.lastCol = ie->loc.lastCol;
    }

    virtual ASTExpressionKind getKind() const override {
        return ASTExpressionKind::AEK_Index;
    }
};

#endif // _QLS_AST_EXPRESSIONS_ASTINDEXEXPRESSION_H
