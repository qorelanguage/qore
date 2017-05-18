/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTClassDeclaration.h

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

#ifndef _QLS_AST_DECLARATIONS_ASTCLASSDECLARATION_H
#define _QLS_AST_DECLARATIONS_ASTCLASSDECLARATION_H

#include <vector>

#include "ast/ASTDeclaration.h"
#include "ast/ASTModifiers.h"
#include "ast/ASTName.h"
#include "ast/declarations/ASTSuperclassDeclaration.h"

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

#endif // _QLS_AST_DECLARATIONS_ASTCLASSDECLARATION_H
