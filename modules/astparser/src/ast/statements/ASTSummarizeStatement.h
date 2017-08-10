/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTSummarizeStatement.h

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

#ifndef _QLS_AST_STATEMENTS_ASTSUMMARIZESTATEMENT_H
#define _QLS_AST_STATEMENTS_ASTSUMMARIZESTATEMENT_H

#include <memory>
#include <vector>

#include "ast/ASTStatement.h"
#include "ast/ASTExpression.h"
#include "ast/expressions/ASTContextModExpression.h"
#include "ast/expressions/ASTNameExpression.h"
#include "ast/statements/ASTStatementBlock.h"

class ASTSummarizeStatement : public ASTStatement {
public:
    //! Pointer type.
    using Ptr = std::unique_ptr<ASTSummarizeStatement>;

public:
    //! Name expression.
    ASTNameExpression::Ptr name;

    //! Data expression.
    ASTExpression::Ptr data;

    //! By expression.
    ASTExpression::Ptr by;

    //! Context mods.
    std::vector<ASTContextModExpression*> contextMods;

    //! Statements to execute.
    ASTStatementBlock::Ptr statements;

public:
    ASTSummarizeStatement(ASTNameExpression* ne,
                          ASTExpression* de,
                          ASTExpression* be,
                          std::vector<ASTContextModExpression*>* cm,
                          ASTStatementBlock* sb) :
        ASTStatement(),
        name(ne),
        data(de),
        by(be),
        statements(sb)
    {
        if (cm)
            contextMods.swap(*cm);
    }

    virtual ~ASTSummarizeStatement() {
        for (size_t i = 0, count = contextMods.size(); i < count; i++)
            delete contextMods[i];
        contextMods.clear();
    }

    virtual Kind getKind() const override {
        return Kind::ASK_Summarize;
    }
};

#endif // _QLS_AST_STATEMENTS_ASTSUMMARIZESTATEMENT_H
