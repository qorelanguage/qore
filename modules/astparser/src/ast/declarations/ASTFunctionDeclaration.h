/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTFunctionDeclaration.h

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

#ifndef _QLS_AST_DECLARATIONS_ASTFUNCTIONDECLARATION_H
#define _QLS_AST_DECLARATIONS_ASTFUNCTIONDECLARATION_H

#include "ast/ASTDeclaration.h"
#include "ast/ASTExpression.h"
#include "ast/ASTModifiers.h"
#include "ast/ASTName.h"
#include "ast/expressions/ASTConstrInitExpression.h"
#include "ast/statements/ASTStatementBlock.h"

enum AFDKind {
    AFDK_Inline = 0,
    AFDK_Outofline = 1,
    AFDK_ScopedSub = 2,
    AFDK_Sub = 3,
};

class ASTFunctionDeclaration : public ASTDeclaration {
public:
    //! Function modifiers.
    ASTModifiers modifiers;

    //! Function name.
    ASTName name;

    //! Function kind.
    AFDKind afdKind;

    //! Return type.
    ASTExpression::Ptr returnType;

    //! Function parameters.
    ASTExpression::Ptr params;

    //! (Base class) initializations.
    ASTConstrInitExpression::Ptr inits;

    //! Function's body.
    ASTStatementBlock::Ptr body;

public:
    ASTFunctionDeclaration(ASTModifiers mods,
                           const ASTName& n,
                           AFDKind k,
                           ASTExpression* rt = nullptr,
                           ASTExpression* par = nullptr,
                           ASTConstrInitExpression* ci = nullptr,
                           ASTStatementBlock* stmts = nullptr) :
        ASTDeclaration(),
        modifiers(mods),
        name(n),
        afdKind(k),
        returnType(rt),
        params(par),
        inits(ci),
        body(stmts) {}

    ASTFunctionDeclaration(const ASTName& n,
                           AFDKind k,
                           ASTExpression* rt = nullptr,
                           ASTExpression* par = nullptr,
                           ASTConstrInitExpression* ci = nullptr,
                           ASTStatementBlock* stmts = nullptr) :
        ASTDeclaration(),
        name(n),
        afdKind(k),
        returnType(rt),
        params(par),
        inits(ci),
        body(stmts) {}

    virtual Kind getKind() const override {
        return Kind::ADK_Function;
    }
};

#endif // _QLS_AST_DECLARATIONS_ASTFUNCTIONDECLARATION_H
