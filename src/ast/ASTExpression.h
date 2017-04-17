/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTExpression.h

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

#ifndef _QLS_AST_ASTEXPRESSION_H
#define _QLS_AST_ASTEXPRESSION_H

#include <memory>

#include "ASTNode.h"

class ASTExpression : public ASTNode {
public:
    //! Pointer type.
    using Ptr = std::unique_ptr<ASTExpression>;

public:
    enum class Kind { // AEK == (A)st (E)xpression (K)ind
        AEK_Access,             //!< Identifies an instance of \ref ASTAccessExpression.
        AEK_Assignment,         //!< Identifies an instance of \ref ASTAssignmentExpression.
        AEK_Backquote,          //!< Identifies an instance of \ref ASTBackquoteExpression.
        AEK_Binary,             //!< Identifies an instance of \ref ASTBinaryExpression.
        AEK_Call,               //!< Identifies an instance of \ref ASTCallExpression.
        AEK_Case,               //!< Identifies an instance of \ref ASTCaseExpression.
        AEK_Cast,               //!< Identifies an instance of \ref ASTCastExpression.
        AEK_Closure,            //!< Identifies an instance of \ref ASTClosureExpression.
        AEK_ConstrInit,         //!< Identifies an instance of \ref ASTConstrInitExpression.
        AEK_ContextMod,         //!< Identifies an instance of \ref ASTContextModExpression.
        AEK_ContextRow,         //!< Identifies an instance of \ref ASTContextRowExpression.
        AEK_Decl,               //!< Identifies an instance of \ref ASTDeclExpression.
        AEK_Find,               //!< Identifies an instance of \ref ASTFindExpression.
        AEK_Hash,               //!< Identifies an instance of \ref ASTHashExpression.
        AEK_HashElement,        //!< Identifies an instance of \ref ASTHashElementExpression.
        AEK_ImplicitArg,        //!< Identifies an instance of \ref ASTImplicitArgExpression.
        AEK_ImplicitElem,       //!< Identifies an instance of \ref ASTImplicitElemExpression.
        AEK_Index,              //!< Identifies an instance of \ref ASTIndexExpression.
        AEK_List,               //!< Identifies an instance of \ref ASTListExpression.
        AEK_Literal,            //!< Identifies an instance of \ref ASTLiteralExpression.
        AEK_Name,               //!< Identifies an instance of \ref ASTNameExpression.
        AEK_Regex,              //!< Identifies an instance of \ref ASTRegexExpression.
        AEK_RegexSubst,         //!< Identifies an instance of \ref ASTRegexSubstExpression.
        AEK_RegexTrans,         //!< Identifies an instance of \ref ASTRegexTransExpression.
        AEK_Returns,            //!< Identifies an instance of \ref ASTReturnsExpression.
        AEK_SwitchBody,         //!< Identifies an instance of \ref ASTSwitchBodyExpression.
        AEK_Ternary,            //!< Identifies an instance of \ref ASTTernaryExpression.
        AEK_Unary,              //!< Identifies an instance of \ref ASTUnaryExpression.
    };

public:
    ASTExpression() : ASTNode() {}
    ASTExpression(const ASTParseLocation& l) : ASTNode(l) {}

    virtual Kind getKind() const = 0;
};

#endif // _QLS_AST_ASTEXPRESSION_H
