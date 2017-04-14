/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTStatement.h

  Qore Programming Language

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

#ifndef _QLS_AST_ASTSTATEMENT_H
#define _QLS_AST_ASTSTATEMENT_H

#include <memory>

#include "ASTNode.h"

class ASTStatement : public ASTNode {
public:
    //! Pointer type.
    using Ptr = std::unique_ptr<ASTStatement>;

    enum class Kind { // ASK == (A)st (S)tatement (K)ind
        ASK_Block,          //!< Identifies an instance of \ref ASTStatementBlock.
        ASK_Break,          //!< Identifies an instance of \ref ASTBreakStatement.
        ASK_Call,           //!< Identifies an instance of \ref ASTCallStatement.
        ASK_Context,        //!< Identifies an instance of \ref ASTContextStatement.
        ASK_Continue,       //!< Identifies an instance of \ref ASTContinueStatement.
        ASK_DoWhile,        //!< Identifies an instance of \ref ASTDoWhileStatement.
        ASK_Expression,     //!< Identifies an instance of \ref ASTExpressionStatement.
        ASK_For,            //!< Identifies an instance of \ref ASTForStatement.
        ASK_Foreach,        //!< Identifies an instance of \ref ASTForeachStatement.
        ASK_If,             //!< Identifies an instance of \ref ASTIfStatement.
        ASK_OnBlockExit,    //!< Identifies an instance of \ref ASTOnBlockExitStatement.
        ASK_Rethrow,        //!< Identifies an instance of \ref ASTRethrowStatement.
        ASK_Return,         //!< Identifies an instance of \ref ASTReturnStatement.
        ASK_Summarize,      //!< Identifies an instance of \ref ASTSummarizeStatement.
        ASK_Switch,         //!< Identifies an instance of \ref ASTSwitchStatement.
        ASK_ThreadExit,     //!< Identifies an instance of \ref ASTThreadExitStatement.
        ASK_Throw,          //!< Identifies an instance of \ref ASTThrowStatement.
        ASK_Try,            //!< Identifies an instance of \ref ASTTryStatement.
        ASK_While,          //!< Identifies an instance of \ref ASTWhileStatement.
    };

public:
    ASTStatement() : ASTNode() {}
    ASTStatement(const ASTParseLocation& l) : ASTNode(l) {}

    virtual ~ASTStatement() {}

    virtual Kind getKind() const = 0;
};

#endif // _QLS_AST_ASTSTATEMENT_H
