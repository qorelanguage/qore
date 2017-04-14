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

#ifndef _QLS_ASTSTATEMENT_H
#define _QLS_ASTSTATEMENT_H

#include <memory>
#include <vector>

#include "ASTNode.h"

class ASTStatement : public ASTNode {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTStatement>;

   enum class Kind { // ASK == (A)st (S)tatement (K)ind
      ASK_Expression,     //!< Identifies an instance of \ref ASTExpressionStatement.
      ASK_Block,          //!< Identifies an instance of \ref ASTStatementBlock.
      ASK_If,             //!< Identifies an instance of \ref ASTIfStatement.
      ASK_Return,         //!< Identifies an instance of \ref ASTReturnStatement.
      ASK_Continue,       //!< Identifies an instance of \ref ASTContinueStatement.
      ASK_Break,          //!< Identifies an instance of \ref ASTBreakStatement.
      ASK_Call,           //!< Identifies an instance of \ref ASTCallStatement.
      ASK_For,            //!< Identifies an instance of \ref ASTForStatement.
      ASK_Foreach,        //!< Identifies an instance of \ref ASTForeachStatement.
      ASK_While,          //!< Identifies an instance of \ref ASTWhileStatement.
      ASK_DoWhile,        //!< Identifies an instance of \ref ASTDoWhileStatement.
      ASK_Switch,         //!< Identifies an instance of \ref ASTSwitchStatement.
      ASK_Try,            //!< Identifies an instance of \ref ASTTryStatement.
      ASK_Throw,          //!< Identifies an instance of \ref ASTThrowStatement.
      ASK_Rethrow,        //!< Identifies an instance of \ref ASTRethrowStatement.
      ASK_ThreadExit,     //!< Identifies an instance of \ref ASTThreadExitStatement.
      ASK_OnBlockExit,    //!< Identifies an instance of \ref ASTOnBlockExitStatement.
      ASK_Context,        //!< Identifies an instance of \ref ASTContextStatement.
      ASK_Summarize,      //!< Identifies an instance of \ref ASTSummarizeStatement.
   };

public:
   ASTStatement() : ASTNode() {}
   ASTStatement(const ASTParseLocation& l) : ASTNode(l) {}

   virtual ~ASTStatement() {}

   virtual Kind getKind() const = 0;
};

class ASTExpressionStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTExpressionStatement>;

public:
   //! Expression wrapped by this statement.
   ASTExpression::Ptr expression;

public:
   ASTExpressionStatement(ASTExpression* e) : ASTStatement(e->loc), expression(e) {}

   virtual Kind getKind() const override {
      return Kind::ASK_Expression;
   }
};

class ASTStatementBlock : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTStatementBlock>;

public:
   //! Statements in the block.
   std::vector<ASTStatement*> statements;

public:
   ASTStatementBlock() : ASTStatement() {}

   virtual ~ASTStatementBlock() {
      for (unsigned int i = 0, count = statements.size(); i < count; i++)
         delete statements[i];
      statements.clear();
   }

   void add(ASTStatement* stmt) {
      statements.push_back(stmt);
   }

   virtual Kind getKind() const override {
      return Kind::ASK_Block;
   }
};

class ASTIfStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTIfStatement>;

public:
   //! Condition.
   ASTExpression::Ptr expression;

   //! Code to execute when condition is true.
   ASTStatement::Ptr stmtThen;

   //! Code to execute when condition is false.
   ASTStatement::Ptr stmtElse;

public:
   ASTIfStatement(ASTExpression* expr, ASTStatement* sThen, ASTStatement* sElse) :
      ASTStatement(),
      expression(expr),
      stmtThen(sThen),
      stmtElse(sElse) {}

   virtual ~ASTIfStatement() {}

   virtual Kind getKind() const override {
      return Kind::ASK_If;
   }
};

class ASTReturnStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTReturnStatement>;

public:
   //! Return value.
   ASTExpression::Ptr retval;

public:
   ASTReturnStatement(ASTExpression* rv = nullptr) :
      ASTStatement(),
      retval(rv) {}

   virtual ~ASTReturnStatement() {}

   virtual Kind getKind() const override {
      return Kind::ASK_Return;
   }
};

class ASTContinueStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTContinueStatement>;

public:
   virtual Kind getKind() const override {
      return Kind::ASK_Continue;
   }
};

class ASTBreakStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTBreakStatement>;

public:
   virtual Kind getKind() const override {
      return Kind::ASK_Break;
   }
};

class ASTCallStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTCallStatement>;

public:
   //! Call wrapped by this statement.
   ASTCallExpression::Ptr call;

public:
   ASTCallStatement(ASTCallExpression* c) : ASTStatement(), call(c) {}

   virtual Kind getKind() const override {
      return Kind::ASK_Call;
   }
};

class ASTForStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTForStatement>;

public:
   //! Initialization expression.
   ASTExpression::Ptr init;

   //! Condition expression.
   ASTExpression::Ptr condition;

   //! Iteration expression.
   ASTExpression::Ptr iteration;

   //! Code to execute during the cycle.
   ASTStatement::Ptr statement;

public:
   ASTForStatement(ASTExpression* ie,
                   ASTExpression* cond,
                   ASTExpression* iter,
                   ASTStatement* stmt) :
      ASTStatement(),
      init(ie),
      condition(cond),
      iteration(iter),
      statement(stmt) {}

   virtual ~ASTForStatement() {}

   virtual Kind getKind() const override {
      return Kind::ASK_For;
   }
};

class ASTForeachStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTForeachStatement>;

public:
   //! Declaration of the variable containing value of each iteration.
   ASTExpression::Ptr value;

   //! Source expression (list or iterator).
   ASTExpression::Ptr source;

   //! Code to execute during the cycle.
   ASTStatement::Ptr statement;

public:
   ASTForeachStatement(ASTExpression* val,
                       ASTExpression* src,
                       ASTStatement* stmt) :
      ASTStatement(),
      value(val),
      source(src),
      statement(stmt) {}

   virtual ~ASTForeachStatement() {}

   virtual Kind getKind() const override {
      return Kind::ASK_Foreach;
   }
};

class ASTWhileStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTWhileStatement>;

public:
   //! Condition.
   ASTExpression::Ptr condition;

   //! Statement to perform in cycle.
   ASTStatement::Ptr statement;

public:
   ASTWhileStatement(ASTExpression* cond, ASTStatement* stmt) :
      ASTStatement(),
      condition(cond),
      statement(stmt) {}

   virtual ~ASTWhileStatement() {}

   virtual Kind getKind() const override {
      return Kind::ASK_While;
   }
};

class ASTDoWhileStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTDoWhileStatement>;

public:
   //! Condition.
   ASTExpression::Ptr condition;

   //! Statement to perform in cycle.
   ASTStatement::Ptr statement;

public:
   ASTDoWhileStatement(ASTExpression* cond, ASTStatement* stmt) :
      ASTStatement(),
      condition(cond),
      statement(stmt) {}

   virtual ~ASTDoWhileStatement() {}

   virtual Kind getKind() const override {
      return Kind::ASK_DoWhile;
   }
};

class ASTSwitchStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTSwitchStatement>;

public:
   //! Variable expression.
   ASTExpression::Ptr variable;

   //! Body of the switch statement.
   ASTSwitchBodyExpression::Ptr body;

public:
   ASTSwitchStatement(ASTExpression* var, ASTSwitchBodyExpression* be) :
      ASTStatement(),
      variable(var),
      body(be) {}

   virtual Kind getKind() const override {
      return Kind::ASK_Switch;
   }
};

class ASTTryStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTTryStatement>;

public:
   //! Statement or block to try executing.
   ASTStatement::Ptr tryStmt;

   //! Exception variable expression.
   ASTExpression::Ptr catchVar;

   //! Statement or block to execute in case of an exception.
   ASTStatement::Ptr catchStmt;

public:
   ASTTryStatement(ASTStatement* ts, ASTExpression* cv, ASTStatement* cs) :
      ASTStatement(),
      tryStmt(ts),
      catchVar(cv),
      catchStmt(cs) {}

   virtual Kind getKind() const override {
      return Kind::ASK_Try;
   }
};

class ASTThrowStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTThrowStatement>;

public:
   //! Thrown expression.
   ASTExpression::Ptr expression;

public:
   ASTThrowStatement(ASTExpression* expr) :
      ASTStatement(),
      expression(expr) {}

   virtual ~ASTThrowStatement() {}

   virtual Kind getKind() const override {
      return Kind::ASK_Throw;
   }
};

class ASTRethrowStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTRethrowStatement>;

public:
   virtual Kind getKind() const override {
      return Kind::ASK_Rethrow;
   }
};

class ASTThreadExitStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTThreadExitStatement>;

public:
   virtual Kind getKind() const override {
      return Kind::ASK_ThreadExit;
   }
};

class ASTOnBlockExitStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTOnBlockExitStatement>;

public:
   enum Condition {
      AOBEC_Exit,
      AOBEC_Success,
      AOBEC_Error,
   };

public:
   //! Condition on when to execute.
   Condition condition;

   //! Code to execute if condition met.
   ASTStatement::Ptr statement;

public:
   ASTOnBlockExitStatement(ASTStatement* stmt, Condition cond) :
      ASTStatement(),
      condition(cond),
      statement(stmt) {}

   virtual ~ASTOnBlockExitStatement() {}

   virtual Kind getKind() const override {
      return Kind::ASK_OnBlockExit;
   }
};

class ASTContextStatement : public ASTStatement {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTContextStatement>;

public:
   //! Name expression.
   ASTNameExpression::Ptr name;

   //! Data expression.
   ASTExpression::Ptr data;

   //! Context mods.
   std::vector<ASTContextModExpression*> contextMods;

   //! Statements to execute.
   ASTStatementBlock::Ptr stmts;

   //! Whether this is a (sub)context statement.
   bool subcontext;

public:
   ASTContextStatement(ASTNameExpression* ne,
                       ASTExpression* de,
                       std::vector<ASTContextModExpression*>* cm,
                       ASTStatementBlock* sb,
                       bool subcont = false) :
      ASTStatement(),
      name(ne),
      data(de),
      stmts(sb),
      subcontext(subcont)
   {
      if (cm)
         contextMods.swap(*cm);
   }

   virtual ~ASTContextStatement() {
      for (unsigned int i = 0, count = contextMods.size(); i < count; i++)
         delete contextMods[i];
      contextMods.clear();
   }

   virtual Kind getKind() const override {
      return Kind::ASK_Context;
   }
};

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
      for (unsigned int i = 0, count = contextMods.size(); i < count; i++)
         delete contextMods[i];
      contextMods.clear();
   }

   virtual Kind getKind() const override {
      return Kind::ASK_Summarize;
   }
};

#endif // _QLS_ASTSTATEMENT_H
