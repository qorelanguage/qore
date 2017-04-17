/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTDeclaration.h

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

#ifndef _QLS_AST_DECLARATIONS_ASTDECLARATION_H
#define _QLS_AST_DECLARATIONS_ASTDECLARATION_H

#include <memory>
#include <utility>
#include <vector>

#include "ast/ASTDeclaration.h"
#include "ast/ASTExpression.h"
#include "ast/ASTModifiers.h"
#include "ast/ASTName.h"
#include "ast/ASTStatement.h"

class ASTNamespaceDeclaration : public ASTDeclaration {
public:
    //! Namespace modifiers.
    ASTModifiers modifiers;

    //! Declarations inside the namespace.
    std::vector<ASTDeclaration*> declarations;

public:
    ASTNamespaceDeclaration(ASTModifiers mods, std::vector<ASTDeclaration*>* decls = nullptr) :
        ASTDeclaration(),
        modifiers(mods)
    {
        if (decls)
            declarations.swap(*decls);
    }

    virtual ~ASTNamespaceDeclaration() {
        for (unsigned int i = 0, count = declarations.size(); i < count; i++)
            delete declarations[i];
        declarations.clear();
    }

    virtual Kind getKind() const override {
        return Kind::ADK_Namespace;
    }
};

class ASTSuperclassDeclaration : public ASTDeclaration {
public:
    //! Superclass modifiers.
    ASTModifiers modifiers;

    //! Name of the class.
    ASTName name;

public:
    ASTSuperclassDeclaration(ASTModifiers mods, const ASTName& n) :
        ASTDeclaration(name.loc),
        modifiers(mods),
        name(n) {}
    ASTSuperclassDeclaration(const ASTName& n) :
        ASTDeclaration(name.loc),
        name(n) {}

    virtual Kind getKind() const override {
        return Kind::ADK_Superclass;
    }
};

class ASTClassDeclaration : public ASTDeclaration {
public:
    //! Class modifiers.
    ASTModifiers modifiers;

    //! Name of the class.
    ASTName name;

    //! List of parent classes.
    std::vector<ASTSuperclassDeclaration*> inherits;

    //! Member and method declarations.
    std::vector<ASTDeclaration*> declarations;

public:
    ASTClassDeclaration(ASTModifiers mods,
                        const ASTName& n,
                        std::vector<ASTSuperclassDeclaration*>* sclist = nullptr,
                        std::vector<ASTDeclaration*>* decllist = nullptr) :
        ASTDeclaration(),
        modifiers(mods),
        name(n)
    {
        if (sclist)
            inherits.swap(*sclist);
        if (decllist)
            declarations.swap(*decllist);
    }

    virtual ~ASTClassDeclaration() {
        for (unsigned int i = 0, count = inherits.size(); i < count; i++)
            delete inherits[i];
        inherits.clear();

        for (unsigned int i = 0, count = declarations.size(); i < count; i++)
            delete declarations[i];
        declarations.clear();
    }

    virtual Kind getKind() const override {
        return Kind::ADK_Class;
    }
};

class ASTFunctionDeclaration : public ASTDeclaration {
public:
    enum AFDKind {
        AFDK_Inline,
        AFDK_Outofline,
        AFDK_ScopedSub,
        AFDK_Sub,
    };

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

class ASTClosureDeclaration : public ASTDeclaration {
public:
    //! Pointer type.
    using Ptr = std::unique_ptr<ASTClosureDeclaration>;

public:
    //! Closure modifiers.
    ASTModifiers modifiers;

    //! Return type.
    ASTExpression::Ptr returnType;

    //! Closure parameters.
    ASTExpression::Ptr params;

    //! Body.
    ASTStatementBlock::Ptr body;

public:
    ASTClosureDeclaration(ASTModifiers mods,
                          ASTExpression* rt = nullptr,
                          ASTExpression* par = nullptr,
                          ASTStatementBlock* stmts = nullptr) :
        ASTDeclaration(),
        modifiers(mods),
        returnType(rt),
        params(par),
        body(stmts) {}

    ASTClosureDeclaration(ASTExpression* rt = nullptr,
                          ASTExpression* par = nullptr,
                          ASTStatementBlock* stmts = nullptr) :
        ASTDeclaration(),
        returnType(rt),
        params(par),
        body(stmts) {}

    virtual Kind getKind() const override {
        return Kind::ADK_Function;
    }
};

class ASTConstantDeclaration : public ASTDeclaration {
public:
    //! Variable modifiers like \c public or \c our.
    ASTModifiers modifiers;

    //! Name of the constant.
    ASTName name;

    //! Value assigned to the constant.
    ASTExpression::Ptr value;

public:
    ASTConstantDeclaration(ASTModifiers mods, const ASTName& n, ASTExpression* v) :
        ASTDeclaration(),
        modifiers(mods),
        name(n),
        value(v) {}
    ASTConstantDeclaration(const ASTName& n, ASTExpression* v) :
        ASTDeclaration(),
        name(n),
        value(v) {}

    virtual Kind getKind() const override {
        return Kind::ADK_Constant;
    }
};

class ASTVariableDeclaration : public ASTDeclaration {
public:
    //! Pointer type.
    using Ptr = std::unique_ptr<ASTVariableDeclaration>;

public:
    //! Variable modifiers like \c our or \c my.
    ASTModifiers modifiers;

    //! Type of the variable.
    ASTName typeName;

    //! Name of the variable.
    ASTName name;

public:
    ASTVariableDeclaration(ASTModifiers mods, const ASTName& tn, const ASTName& n) :
        ASTDeclaration(),
        modifiers(mods),
        typeName(tn),
        name(n) {}
    ASTVariableDeclaration(const ASTName& tn, const ASTName& n) :
        ASTDeclaration(),
        typeName(tn),
        name(n)
    {
        loc.firstLine = tn.loc.firstLine;
        loc.firstCol = tn.loc.firstCol;
        loc.lastLine = n.loc.lastLine;
        loc.lastCol = n.loc.lastCol;
    }
    ASTVariableDeclaration(ASTModifiers mods, const ASTName& n) :
        ASTDeclaration(),
        modifiers(mods),
        name(n) {}
    ASTVariableDeclaration(const ASTName& n) :
        ASTDeclaration(),
        name(n) {}

    virtual Kind getKind() const override {
        return Kind::ADK_Variable;
    }
};

class ASTVarListDeclaration : public ASTDeclaration {
public:
    //! Pointer type.
    using Ptr = std::unique_ptr<ASTVarListDeclaration>;

public:
    //! Variable list modifiers like \c our or \c my.
    ASTModifiers modifiers;

    //! Variable list.
    ASTListExpression::Ptr variables;

public:
    ASTVarListDeclaration(ASTModifiers mods, ASTListExpression* vars) :
        ASTDeclaration(),
        modifiers(mods),
        variables(vars) {}

    virtual Kind getKind() const override {
        return Kind::ADK_VarList;
    }
};

class ASTMemberGroupDeclaration : public ASTDeclaration {
public:
    //! Member group modifiers.
    ASTModifiers modifiers;

    //! Member declarations.
    std::vector<ASTExpression*> members;

public:
    ASTMemberGroupDeclaration(ASTModifiers mods) : ASTDeclaration(), modifiers(mods) {}
    ASTMemberGroupDeclaration(ASTModifiers mods, std::vector<ASTExpression*>* memberlist) :
        ASTDeclaration(),
        modifiers(mods)
    {
        if (memberlist)
            members.swap(*memberlist);
    }

    virtual ~ASTMemberGroupDeclaration() {
        for (unsigned int i = 0, count = members.size(); i < count; i++)
            delete members[i];
        members.clear();
    }

    virtual Kind getKind() const override {
        return Kind::ADK_MemberGroup;
    }
};

#endif // _QLS_AST_DECLARATIONS_ASTDECLARATION_H
