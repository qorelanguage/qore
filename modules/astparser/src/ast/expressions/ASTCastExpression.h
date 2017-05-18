/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTCastExpression.h

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

#ifndef _QLS_AST_EXPRESSIONS_ASTCASTEXPRESSION_H
#define _QLS_AST_EXPRESSIONS_ASTCASTEXPRESSION_H

#include "ast/ASTExpression.h"
#include "ast/ASTName.h"

class ASTCastExpression : public ASTExpression {
public:
    //! (Class)type to which the expression should be cast.
    ASTName castType;

    //! Object expression which should be casted to a different type.
    ASTExpression::Ptr obj;

public:
    ASTCastExpression(const ASTName& ct, ASTExpression* oe) :
        ASTExpression(),
        castType(ct),
        obj(oe)
    {
        loc.firstLine = castType.loc.firstLine;
        loc.firstCol = castType.loc.firstCol;
        loc.lastLine = oe->loc.lastLine;
        loc.lastCol = oe->loc.lastCol;
    }

    virtual Kind getKind() const override {
        return Kind::AEK_Cast;
    }
};

#endif // _QLS_AST_EXPRESSIONS_ASTCASTEXPRESSION_H
