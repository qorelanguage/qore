/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTHoverInfo.h

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

#ifndef _QLS_AST_ASTHOVERINFO_H
#define _QLS_AST_ASTHOVERINFO_H

#include "ast/ASTSymbolInfo.h"

enum ASTHoverInfoKind {
    AHIK_None = 0,

    // Expressions.
    AHIK_AccessVariable,
    AHIK_AccessMember,
    AHIK_AssignmentLeft,
    AHIK_AssignmentRight,
    AHIK_BinaryLeft,
    AHIK_BinaryRight,
    AHIK_CallTarget,
    AHIK_CallArgs,
    AHIK_CaseExpr,
    AHIK_CastType,
    AHIK_CastObject,
    AHIK_FindData,
    AHIK_HashValue,
    AHIK_IndexVariable,
    AHIK_IndexIndex,
    AHIK_ListElement,
    AHIK_ReturnsType,
    AHIK_TernaryCond,
    AHIK_TernaryTrue,
    AHIK_TernaryFalse,
    AHIK_Unary,

    // Declarations.
    AHIK_ClassDeclName,
    AHIK_ConstantDeclName,
    AHIK_FuncDeclName,
    AHIK_FuncReturnType,
    AHIK_NamespaceDeclName,
    AHIK_SuperclassDeclName,
    AHIK_VarDeclName,
    AHIK_VarDeclTypeName,

    // Statements.
    AHIK_ContextStmtName,
    AHIK_ContextStmtData,
    AHIK_DoWhileStmtCond,
    AHIK_ExprStmtExpr,
    AHIK_ForeachStmtSrc,
    AHIK_ForeachStmtVal,
    AHIK_ForStmtCond,
    AHIK_ForStmtInit,
    AHIK_ForStmtIter,
    AHIK_IfStmtCond,
    AHIK_ReturnStmtVal,
    AHIK_SummarizeStmtName,
    AHIK_SummarizeStmtData,
    AHIK_SwitchStmtVar,
    AHIK_ThrowStmtExpr,
    AHIK_TryStmtCatchVar,
    AHIK_WhileStmtCond,
    /*AHIK_,
    AHIK_,
    AHIK_,
    AHIK_,
    AHIK_,
    AHIK_,
    AHIK_,*/
};

struct ASTHoverInfo : public ASTSymbolInfo {
public:
    ASTHoverInfo() : ASTSymbolInfo(), hiKind(AHIK_None) {}
    ASTHoverInfo(ASTHoverInfoKind hik, ASTSymbolKind k, const ASTParseLocation& nloc, const std::string& n) :
        ASTSymbolInfo(k, nloc, n), hiKind(hik) {}

    ASTHoverInfo(const ASTHoverInfo& hi) : ASTSymbolInfo(hi.kind, hi.loc, hi.name), hiKind(hi.hiKind) {}
    ASTHoverInfo(ASTHoverInfo&& hi) : ASTSymbolInfo(hi.kind, hi.loc, std::move(hi.name)), hiKind(hi.hiKind) {
        hi.kind = ASYK_None;
        hi.hiKind = AHIK_None;
    }

    ASTHoverInfo& operator=(const ASTHoverInfo& hi) {
        loc = hi.loc;
        kind = hi.kind;
        name = hi.name;
        hiKind = hi.hiKind;
        return *this;
    }
    ASTHoverInfo& operator=(ASTHoverInfo&& hi) {
        loc = hi.loc;
        kind = hi.kind;
        name = std::move(hi.name);
        hiKind = hi.hiKind;
        hi.kind = ASYK_None;
        hi.hiKind = AHIK_None;
        return *this;
    }

    ASTHoverInfoKind hiKind;
};

#endif // _QLS_AST_ASTHOVERINFO_H
