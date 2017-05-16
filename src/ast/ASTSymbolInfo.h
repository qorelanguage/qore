/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTSymbolInfo.h

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

#ifndef _QLS_AST_ASTSYMBOLINFO_H
#define _QLS_AST_ASTSYMBOLINFO_H

#include "ast/ASTName.h"
#include "ast/ASTNode.h"

//! Describes the kind of symbol.
enum ASTSymbolKind {
    ASYK_None = 0,
    ASYK_File = 1,
    ASYK_Module = 2,
    ASYK_Namespace = 3,
    ASYK_Package = 4,
    ASYK_Class = 5,
    ASYK_Method = 6,
    ASYK_Property = 7,
    ASYK_Field = 8,
    ASYK_Constructor = 9,
    ASYK_Enum = 10,
    ASYK_Interface = 11,
    ASYK_Function = 12,
    ASYK_Variable = 13,
    ASYK_Constant = 14,
    ASYK_String = 15,
    ASYK_Number = 16,
    ASYK_Boolean = 17,
    ASYK_Array = 18,
};

//! Describes in what situation a symbol was used.
enum ASTSymbolUsageKind {
    ASUK_None = 0,

    // Declarations.
    ASUK_ClassDeclName = 100,
    ASUK_ConstantDeclName = 101,
    ASUK_FuncDeclName = 102,
    ASUK_FuncReturnType = 103,
    ASUK_NamespaceDeclName = 104,
    ASUK_SuperclassDeclName = 105,
    ASUK_VarDeclName = 106,
    ASUK_VarDeclTypeName = 107,

    // Expressions.
    ASUK_AccessVariable = 200,
    ASUK_AccessMember = 201,
    ASUK_AssignmentLeft = 202,
    ASUK_AssignmentRight = 203,
    ASUK_BinaryLeft = 204,
    ASUK_BinaryRight = 205,
    ASUK_CallTarget = 206,
    ASUK_CallArgs = 207,
    ASUK_CaseExpr = 208,
    ASUK_CastType = 209,
    ASUK_CastObject = 210,
    ASUK_FindData = 211,
    ASUK_HashValue = 212,
    ASUK_IndexVariable = 213,
    ASUK_IndexIndex = 214,
    ASUK_ListElement = 215,
    ASUK_ReturnsType = 216,
    ASUK_TernaryCond = 217,
    ASUK_TernaryTrue = 218,
    ASUK_TernaryFalse = 219,
    ASUK_Unary = 220,

    // Statements.
    ASUK_ContextStmtName = 300,
    ASUK_ContextStmtData = 301,
    ASUK_DoWhileStmtCond = 302,
    ASUK_ExprStmtExpr = 303,
    ASUK_ForeachStmtSrc = 304,
    ASUK_ForeachStmtVal = 305,
    ASUK_ForStmtCond = 306,
    ASUK_ForStmtInit = 307,
    ASUK_ForStmtIter = 308,
    ASUK_IfStmtCond = 309,
    ASUK_ReturnStmtVal = 310,
    ASUK_SummarizeStmtName = 311,
    ASUK_SummarizeStmtData = 312,
    ASUK_SwitchStmtVar = 313,
    ASUK_ThrowStmtExpr = 314,
    ASUK_TryStmtCatchVar = 315,
    ASUK_WhileStmtCond = 316,
};

class ASTSymbolInfo : public ASTNode {
public:
    ASTSymbolInfo() : ASTNode(), kind(ASYK_None), usage(ASUK_None) {}

    ASTSymbolInfo(ASTSymbolKind k, ASTSymbolUsageKind u, ASTName* n) :
        ASTNode(n->loc),
        kind(k),
        usage(u),
        name(n->name) {}

    ASTSymbolInfo(ASTSymbolKind k, ASTSymbolUsageKind u, const ASTParseLocation& nloc, std::string&& n) :
        ASTNode(nloc),
        kind(k),
        usage(u),
        name(std::move(n)) {}

    ASTSymbolInfo(ASTSymbolKind k, ASTSymbolUsageKind u, const ASTParseLocation& nloc, const std::string& n) :
        ASTNode(nloc),
        kind(k),
        usage(u),
        name(n) {}

    ASTSymbolInfo(ASTSymbolKind k, ASTName* n) :
        ASTNode(n->loc),
        kind(k),
        usage(ASUK_None),
        name(n->name) {}

    ASTSymbolInfo(ASTSymbolKind k, const ASTParseLocation& nloc, std::string&& n) :
        ASTNode(nloc),
        kind(k),
        usage(ASUK_None),
        name(std::move(n)) {}

    ASTSymbolInfo(ASTSymbolKind k, const ASTParseLocation& nloc, const std::string& n) :
        ASTNode(nloc),
        kind(k),
        usage(ASUK_None),
        name(n) {}

    //! Copy constructor.
    ASTSymbolInfo(const ASTSymbolInfo& si) : ASTNode(si.loc), kind(si.kind), usage(si.usage), name(si.name) {}

    //! Move constructor.
    ASTSymbolInfo(ASTSymbolInfo&& si) : ASTNode(si.loc), kind(si.kind), usage(si.usage), name(std::move(si.name)) {
        si.kind = ASYK_None;
        si.usage = ASUK_None;
    }

    //! Assignment operator.
    ASTSymbolInfo& operator=(const ASTSymbolInfo& si) {
        loc = si.loc;
        kind = si.kind;
        usage = si.usage;
        name = si.name;
        return *this;
    }

    //! Move assignment operator.
    ASTSymbolInfo& operator=(ASTSymbolInfo&& si) {
        loc = si.loc;
        kind = si.kind;
        usage = si.usage;
        name = std::move(si.name);
        si.kind = ASYK_None;
        si.usage = ASUK_None;
        return *this;
    }

    //! Symbol's kind.
    ASTSymbolKind kind;

    //! Symbol's usage kind.
    ASTSymbolUsageKind usage;

    //! Symbol's name.
    std::string name;
};

#endif // _QLS_AST_ASTSYMBOLINFO_H
