/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTName.h

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

#ifndef _QLS_AST_ASTNAME_H
#define _QLS_AST_ASTNAME_H

#include <string>

#include "ASTNode.h"

enum class ASTNameKind {
    ANK_Identifier,
    ANK_KWIdentifier,
    ANK_IdentOpenParen,
    ANK_ScopedRef,
    ANK_ScopedVref,
    ANK_SelfRef,
    ANK_SelfAndScopedRef,
    ANK_VarRef,
    ANK_QTypedef,
    ANK_UncQTypedef,
    ANK_ContextRef,
    ANK_ComplexContextRef,
    ANK_BaseClassCall,
    ANK_CastType,
    ANK_ClassString,
    ANK_ClassScopedRef,
    ANK_Namespace
};

//! Represents a name.
class ASTName : public ASTNode {
public:
    std::string name;
    ASTNameKind kind;

public:
    ASTName() : ASTNode() {}
    ASTName(ASTNameKind k) : ASTNode(), kind(k) {}
    ASTName(const ASTName& n, ASTNameKind k) : ASTNode(n.loc), name(n.name), kind(k) {}
    ASTName(const std::string& str, ASTNameKind k) : ASTNode(), name(str), kind(k) {}
    ASTName(const std::string* str, ASTNameKind k) : ASTNode(), kind(k) {
        if (str)
            name = *str;
    }
    ASTName(const char* str, ASTNameKind k) : ASTNode(), name(str), kind(k) {
        if (str)
            name = str;
    }

    virtual ASTNodeType getNodeType() {
        return ANT_Name;
    }
};

#endif // _QLS_AST_ASTNAME_H
