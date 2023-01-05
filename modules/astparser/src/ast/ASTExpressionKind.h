/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTExpressionKind.h

  Qore AST Parser

  Copyright (C) 2023 Qore Technologies, s.r.o.

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

#ifndef _QLS_AST_ASTEXPRESSIONKIND_H
#define _QLS_AST_ASTEXPRESSIONKIND_H

//! Describes the kind of expression.
enum ASTExpressionKind {
    AEK__reserved = 0,      // don't use
    AEK_Access = 1,         //!< Identifies an instance of \ref ASTAccessExpression.
    AEK_Assignment = 2,     //!< Identifies an instance of \ref ASTAssignmentExpression.
    AEK_Backquote = 3,      //!< Identifies an instance of \ref ASTBackquoteExpression.
    AEK_Binary = 4,         //!< Identifies an instance of \ref ASTBinaryExpression.
    AEK_Call = 5,           //!< Identifies an instance of \ref ASTCallExpression.
    AEK_Case = 6,           //!< Identifies an instance of \ref ASTCaseExpression.
    AEK_Cast = 7,           //!< Identifies an instance of \ref ASTCastExpression.
    AEK_Closure = 8,        //!< Identifies an instance of \ref ASTClosureExpression.
    AEK_ConstrInit = 9,     //!< Identifies an instance of \ref ASTConstrInitExpression.
    AEK_ContextMod = 10,    //!< Identifies an instance of \ref ASTContextModExpression.
    AEK_ContextRow = 11,    //!< Identifies an instance of \ref ASTContextRowExpression.
    AEK_Decl = 12,          //!< Identifies an instance of \ref ASTDeclExpression.
    AEK_Find = 13,          //!< Identifies an instance of \ref ASTFindExpression.
    AEK_Hash = 14,          //!< Identifies an instance of \ref ASTHashExpression.
    AEK_HashElement = 15,   //!< Identifies an instance of \ref ASTHashElementExpression.
    AEK_ImplicitArg = 16,   //!< Identifies an instance of \ref ASTImplicitArgExpression.
    AEK_ImplicitElem = 17,  //!< Identifies an instance of \ref ASTImplicitElemExpression.
    AEK_Index = 18,         //!< Identifies an instance of \ref ASTIndexExpression.
    AEK_List = 19,          //!< Identifies an instance of \ref ASTListExpression.
    AEK_Literal = 20,       //!< Identifies an instance of \ref ASTLiteralExpression.
    AEK_Name = 21,          //!< Identifies an instance of \ref ASTNameExpression.
    AEK_Range = 22,         //!< Identifies an instance of \ref ASTRangeExpression.
    AEK_Regex = 23,         //!< Identifies an instance of \ref ASTRegexExpression.
    AEK_RegexSubst = 24,    //!< Identifies an instance of \ref ASTRegexSubstExpression.
    AEK_RegexTrans = 25,    //!< Identifies an instance of \ref ASTRegexTransExpression.
    AEK_Returns = 26,       //!< Identifies an instance of \ref ASTReturnsExpression.
    AEK_SwitchBody = 27,    //!< Identifies an instance of \ref ASTSwitchBodyExpression.
    AEK_Ternary = 28,       //!< Identifies an instance of \ref ASTTernaryExpression.
    AEK_Unary = 29,         //!< Identifies an instance of \ref ASTUnaryExpression.
    AEK_HashdeclHash = 30,  //!< Identifies an instance of \ref ASTHashdeclHashExpression.
};

#endif // _QLS_AST_ASTEXPRESSIONKIND_H
