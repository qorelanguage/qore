/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AST.h

  Qore AST Parser

  Copyright (C) 2016 Qore Technologies, sro

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

#ifndef _QLS_AST_AST_H
#define _QLS_AST_AST_H

#include "ast/ASTDeclaration.h"
#include "ast/ASTExpression.h"
#include "ast/ASTModifiers.h"
#include "ast/ASTName.h"
#include "ast/ASTNode.h"
#include "ast/ASTOperator.h"
#include "ast/ASTStatement.h"

#include "ast/statements/ASTBreakStatement.h"
#include "ast/statements/ASTCallStatement.h"
#include "ast/statements/ASTContextStatement.h"
#include "ast/statements/ASTContinueStatement.h"
#include "ast/statements/ASTDoWhileStatement.h"
#include "ast/statements/ASTExpressionStatement.h"
#include "ast/statements/ASTForeachStatement.h"
#include "ast/statements/ASTForStatement.h"
#include "ast/statements/ASTIfStatement.h"
#include "ast/statements/ASTOnBlockExitStatement.h"
#include "ast/statements/ASTRethrowStatement.h"
#include "ast/statements/ASTReturnStatement.h"
#include "ast/statements/ASTStatementBlock.h"
#include "ast/statements/ASTSummarizeStatement.h"
#include "ast/statements/ASTSwitchStatement.h"
#include "ast/statements/ASTThreadExitStatement.h"
#include "ast/statements/ASTThrowStatement.h"
#include "ast/statements/ASTTryStatement.h"
#include "ast/statements/ASTWhileStatement.h"

#endif // _QLS_AST_AST_H
