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

struct ASTSymbolInfo : public ASTNode {
public:
    ASTSymbolInfo() : ASTNode(), kind(ASYK_None) {}
    ASTSymbolInfo(ASTSymbolKind k, ASTName* n) : ASTNode(n->loc), kind(k), name(n->name) {}
    ASTSymbolInfo(ASTSymbolKind k, const ASTParseLocation& nloc, std::string&& n) : ASTNode(nloc), kind(k), name(std::move(n)) {}
    ASTSymbolInfo(ASTSymbolKind k, const ASTParseLocation& nloc, const std::string& n) : ASTNode(nloc), kind(k), name(n) {}
    ASTSymbolInfo(const ASTSymbolInfo& si) : ASTNode(si.loc), kind(si.kind), name(si.name) {}
    ASTSymbolInfo(ASTSymbolInfo&& si) : ASTNode(si.loc), kind(si.kind), name(std::move(si.name)) {}
    ASTSymbolInfo& operator=(const ASTSymbolInfo& si) {
        loc = si.loc;
        kind = si.kind;
        name = si.name;
        return *this;
    }
    ASTSymbolInfo& operator=(ASTSymbolInfo&& si) {
        loc = si.loc;
        kind = si.kind;
        name = std::move(si.name);
        return *this;
    }

    //! Symbol's kind.
    ASTSymbolKind kind;

    //! Symbol's name.
    std::string name;
};

#endif // _QLS_AST_ASTSYMBOLINFO_H
