/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ASTExpression.h

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

#ifndef _QLS_ASTEXPRESSION_H
#define _QLS_ASTEXPRESSION_H

#include <string>

#include "ASTNode.h"
#include "ASTOperator.h"

class ASTExpression : public ASTNode {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTExpression>;

public:
   enum class Kind { // AEK == (A)st (E)xpression (K)ind
      AEK_Literal,            //!< Identifies an instance of \ref ASTLiteralExpression.
      AEK_Name,               //!< Identifies an instance of \ref ASTNameExpression.
      AEK_Decl,               //!< Identifies an instance of \ref ASTDeclExpression.
      AEK_Access,             //!< Identifies an instance of \ref ASTAccessExpression.
      AEK_Index,              //!< Identifies an instance of \ref ASTIndexExpression.
      AEK_Call,               //!< Identifies an instance of \ref ASTCallExpression.
      AEK_Assignment,         //!< Identifies an instance of \ref ASTAssignmentExpression.
      AEK_Unary,              //!< Identifies an instance of \ref ASTUnaryExpression.
      AEK_Binary,             //!< Identifies an instance of \ref ASTBinaryExpression.
      AEK_Ternary,            //!< Identifies an instance of \ref ASTTernaryExpression.
      AEK_Case,               //!< Identifies an instance of \ref ASTCaseExpression.
      AEK_SwitchBody,         //!< Identifies an instance of \ref ASTSwitchBodyExpression.
      AEK_List,               //!< Identifies an instance of \ref ASTListExpression.
      AEK_HashElement,        //!< Identifies an instance of \ref ASTHashElementExpression.
      AEK_Hash,               //!< Identifies an instance of \ref ASTHashExpression.
      AEK_Find,               //!< Identifies an instance of \ref ASTFindExpression.
      AEK_Cast,               //!< Identifies an instance of \ref ASTCastExpression.
      AEK_ContextRow,         //!< Identifies an instance of \ref ASTContextRowExpression.
      AEK_ImplicitElem,       //!< Identifies an instance of \ref ASTImplicitElemExpression.
      AEK_ImplicitArg,        //!< Identifies an instance of \ref ASTImplicitArgExpression.
      AEK_ContextMod,         //!< Identifies an instance of \ref ASTContextModExpression.
      AEK_Returns,            //!< Identifies an instance of \ref ASTReturnsExpression.
      AEK_Closure,            //!< Identifies an instance of \ref ASTClosureExpression.
      AEK_Backquote           //!< Identifies an instance of \ref ASTBackquoteExpression.
      AEK_Regex,              //!< Identifies an instance of \ref ASTRegexExpression.
      AEK_ConstrInit,         //!< Identifies an instance of \ref ASTConstrInitExpression.
   };

public:
   ASTExpression() : ASTNode() {}
   ASTExpression(const ASTParseLocation& l) : ASTNode(l) {}

   virtual Kind getKind() const = 0;
};

class ASTLiteralExpression : public ASTExpression {
public:
   enum ASTLiteralExpressionKind {
      ALEK_Binary,
      ALEK_Date,
      ALEK_Float,
      ALEK_Int,
      ALEK_Number,
      ALEK_String,
   };

   union ASTLiteralExpressionData {
      int64 i;
      double f;
      char* str;
      std::string* stdstr;
   };

public:
   //! Kind of value.
   ASTLiteralExpressionKind kind;

   //! Literal value.
   ASTLiteralExpressionData value;

public:
   ASTLiteralExpression(ASTLiteralExpressionKind k, int64 val) :
      ASTExpression(),
      kind(k),
      value(val) {}
   ASTLiteralExpression(ASTLiteralExpressionKind k, double val) :
      ASTExpression(),
      kind(k),
      value(val) {}
   ASTLiteralExpression(ASTLiteralExpressionKind k, char* val) :
      ASTExpression(),
      kind(k),
      value(val) {}
   ASTLiteralExpression(ASTLiteralExpressionKind k, std::string* val) :
      ASTExpression(),
      kind(k),
      value(val) {}
   
   virtual ~ASTLiteralExpression() {
      if (kind == ALEK_Binary || kind == ALEK_Date || kind == ALEK_Number)
         free(value.str);
      if (kind == ALEK_String)
         delete value.stdstr;
   }

   virtual Kind getKind() const override {
      return Kind::AEK_Literal;
   }
};

class ASTNameExpression : public ASTExpression {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTNameExpression>;

public:
   //! The name.
   ASTName name;

public:
   ASTNameExpression(const char* nameStr) : ASTExpression(), name(nameStr) {}
   ASTNameExpression(const std::string& nameStr) : ASTExpression(), name(nameStr) {}
   ASTNameExpression(const std::string* nameStr) : ASTExpression(), name(nameStr) {}
   ASTNameExpression(const ASTName& n) : ASTExpression(n.loc), name(n) {}

   virtual Kind getKind() const override {
      return Kind::AEK_Name;
   }
};

class ASTDeclExpression : public ASTExpression {
public:
   //! Declaration wrapped by this expression.
   ASTDeclaration::Ptr declaration;

public:
   ASTDeclExpression(ASTDeclaration* d) : ASTExpression(d->loc), declaration(d) {}

   virtual Kind getKind() const override {
      return Kind::AEK_Decl;
   }
};

class ASTAccessExpression : public ASTExpression {
public:
   //! Variable being accessed.
   ASTExpression::Ptr variable;

   //! Variable's member being accessed.
   ASTExpression::Ptr member;

public:
   ASTAccessExpression(ASTExpression* var, ASTExpression* me) :
      ASTExpression(),
      variable(var),
      member(me)
   {
      loc.firstLine = var->loc.firstLine;
      loc.firstCol = var->loc.firstCol;
      loc.lastLine = me->loc.lastLine;
      loc.lastCol = me->loc.lastCol;
   }

   virtual Kind getKind() const override {
      return Kind::AEK_Access;
   }
};

class ASTIndexExpression : public ASTExpression {
public:
   enum class IndexKind {
      AIE_SquareBrackets,
      AIE_CurlyBrackets,
   };

public:
   //! Variable being accessed.
   ASTExpression::Ptr variable;

   //! Index.
   ASTExpression::Ptr index;

   //! Type of indexing used.
   IndexKind indexKind;

public:
   ASTIndexExpression(ASTExpression* var, ASTExpression* ie, IndexKind ik) :
      ASTExpression(),
      variable(var),
      index(ie),
      indexKind(ik)
   {
      loc.firstLine = var->loc.firstLine;
      loc.firstCol = var->loc.firstCol;
      loc.lastLine = ie->loc.lastLine;
      loc.lastCol = ie->loc.lastCol;
   }

   virtual Kind getKind() const override {
      return Kind::AEK_Index;
   }
};

class ASTCallExpression : public ASTExpression {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTCallExpression>;

public:
   //! Target function or variable.
   ASTExpression::Ptr target;

   //! Call arguments.
   ASTExpression::Ptr args;

public:
   ASTCallExpression(ASTExpression* targ, ASTExpression* argList) :
      ASTExpression(),
      target(targ),
      args(argList)
   {
      loc.firstLine = varExpr->loc.firstLine;
      loc.firstCol = varExpr->loc.firstCol;
      loc.lastLine = argsExpr->loc.lastLine;
      loc.lastCol = argsExpr->loc.lastCol;
   }

   virtual Kind getKind() const override {
      return Kind::AEK_Call;
   }
};

class ASTAssignmentExpression : public ASTExpression {
public:
   //! Left-side expression.
   ASTExpression::Ptr left;

   //! Right-side expression.
   ASTExpression::Ptr right;

public:
   ASTAssignmentExpression(ASTExpression* l, ASTExpression* r) :
      ASTExpression(),
      left(l),
      right(r)
   {
      loc.firstLine = left->loc.firstLine;
      loc.firstCol = left->loc.firstCol;
      loc.lastLine = right->loc.lastLine;
      loc.lastCol = right->loc.lastCol;
   }

   virtual Kind getKind() const override {
      return Kind::AEK_Assignment;
   }
};

class ASTUnaryExpression : public ASTExpression {
public:
   ASTExpression::Ptr expression;

   //! Operator to apply to the expression.
   ASTOperator op;

public:
   ASTUnaryExpression(ASTExpression* expr, ASTOperator o) :
      ASTExpression(expr->loc)
      expression(expr),
      op(o) {}

   virtual Kind getKind() const override {
      return Kind::AEK_Unary;
   }
};

class ASTBinaryExpression : public ASTExpression {
public:
   //! Left-side expression.
   ASTExpression::Ptr left;

   //! Operator to apply.
   ASTOperator op;

   //! Right-side expression.
   ASTExpression::Ptr right;

public:
   ASTBinaryExpression(ASTExpression* leftExpr, ASTOperator o, ASTExpression* rightExpr) :
      ASTExpression(),
      left(leftExpr),
      op(o),
      right(rightExpr)
   {
      loc.firstLine = left->loc.firstLine;
      loc.firstCol = left->loc.firstCol;
      loc.lastLine = right->loc.lastLine;
      loc.lastCol = right->loc.lastCol;
   }

   virtual Kind getKind() const override {
      return Kind::AEK_Binary;
   }
};

class ASTTernaryExpression : public ASTExpression {
public:
   //! Condition expression.
   ASTExpression::Ptr condition;

   //! Expression to return in case condition is true.
   ASTExpression::Ptr exprTrue;

   //! Expression to return in case condition is false.
   ASTExpression::Ptr exprFalse;

public:
   ASTTernaryExpression(ASTExpression* cond, ASTExpression* ifTrue, ASTExpression* ifFalse) :
      ASTExpression(),
      condition(cond),
      exprTrue(ifTrue),
      exprFalse(ifFalse)
   {
      loc.firstLine = cond->loc.firstLine;
      loc.firstCol = cond->loc.firstCol;
      loc.lastLine = ifFalse->loc.lastLine;
      loc.lastCol = ifFalse->loc.lastCol;
   }

   virtual Kind getKind() const override {
      return Kind::AEK_Ternary;
   }
};

class ASTCaseExpression : public ASTExpression {
public:
   //! Case operator.
   ASTOperator op;

   //! Case.
   ASTExpression::Ptr caseExpr;

   //! Statements to execute for this case.
   ASTStatementBlock::Ptr statements;

   //! Whether this is the "default" case.
   bool defaultCase;

public:
   ASTCaseExpression(ASTExpression* ce, ASTStatementBlock* sb, bool def = False) :
      ASTExpression(),
      caseExpr(ce),
      statements(sb),
      defaultCase(def) {}
   ASTCaseExpression(ASTOperator o, ASTExpression* ce, ASTStatementBlock* sb, bool def = False) :
      ASTExpression(),
      op(o),
      caseExpr(ce),
      statements(sb),
      defaultCase(def) {}

   virtual Kind getKind() const override {
      return Kind::AEK_Case;
   }
};

class ASTSwitchBodyExpression : public ASTExpression {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTSwitchBodyExpression>;

public:
   //! Cases of the switch statement.
   std::vector<ASTCaseExpression*> cases;

public:
   ASTSwitchBodyExpression() : ASTExpression() {}

   virtual ~ASTSwitchBodyExpression() {
      for (int i = 0, count = cases.size(); i < count; i++)
         delete cases[i];
      cases.clear();
   }

   virtual Kind getKind() const override {
      return Kind::AEK_SwitchBody;
   }
};

class ASTListExpression : public ASTExpression {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTListExpression>;

public:
   //! List elements.
   std::vector<ASTExpression*> elements;

public:
   ASTListExpression(std::vector<ASTExpression*>* elems = nullptr) : ASTExpression() {
      if (elems)
         elements.swap(*elems);
   }

   virtual ~ASTListExpression() {
      for (unsigned int i = 0, count = elements.size(); i < count; i++)
         delete elements[i];
      elements.clear();
   }

   virtual Kind getKind() const override {
      return Kind::AEK_List;
   }
};

class ASTHashElementExpression : public ASTExpression {
public:
   //! Key expression.
   ASTExpression::Ptr key;

   //! Value expression.
   ASTExpression::Ptr value;

public:
   ASTHashElementExpression(ASTExpression* k, ASTExpression* val) :
      ASTExpression(),
      key(k),
      value(val)
   {
      loc.firstLine = k->loc.firstLine;
      loc.firstCol = k->loc.firstCol;
      loc.lastLine = val->loc.lastLine;
      loc.lastCol = val->loc.lastCol;
   }

   virtual Kind getKind() const override {
      return Kind::AEK_HashElement;
   }
};

class ASTHashExpression : public ASTExpression {
public:
   //! Hash elements.
   std::vector<ASTHashElementExpression*> elements;

public:
   ASTHashExpression(std::vector<ASTHashElementExpression*>* elems = nullptr) : ASTExpression() {
      if (elems)
         elements.swap(*elems);
   }

   virtual ~ASTHashExpression() {
      for (unsigned int i = 0, count = elements.size(); i < count; i++)
         delete elements[i];
      elements.clear();
   }

   virtual Kind getKind() const override {
      return Kind::AEK_Hash;
   }
};

class ASTFindExpression : public ASTExpression {
public:
   //! Result expression.
   ASTExpression::Ptr result;

   //! Data expression.
   ASTExpression::Ptr data;

   //! Where expression.
   ASTExpression::Ptr where;

public:
   ASTFindExpression(ASTExpression* re, ASTExpression* de, ASTExpression* we) :
      ASTExpression(),
      result(re),
      data(de),
      where(we)
   {
   }

   virtual Kind getKind() const override {
      return Kind::AEK_Find;
   }
};

class ASTCastExpression : public ASTExpression {
public:
   //! (Class)type to which the expression should be cast.
   ASTName castType;

   //! Object expression which should be casted to a different type.
   ASTExpression::Ptr obj;

public:
   ASTCastExpression(const ASTName& ct, ASTExpression* oe) :
      ASTExpression(),
      castType(ct),
      obj(oe)
   {
      loc.firstLine = castType.loc.firstLine;
      loc.firstCol = castType.loc.firstCol;
      loc.lastLine = oe->loc.lastLine;
      loc.lastCol = oe->loc.lastCol;
   }

   virtual Kind getKind() const override {
      return Kind::AEK_Cast;
   }
};

class ASTContextRowExpression : public ASTExpression {
public:
   virtual Kind getKind() const override {
      return Kind::AEK_ContextRow;
   }
};

class ASTImplicitElemExpression : public ASTExpression {
public:
   virtual Kind getKind() const override {
      return Kind::AEK_ImplicitElem;
   }
};

class ASTImplicitArgExpression : public ASTExpression {
public:
   int offset;

public:
   ASTImplicitArgExpression(int noffset = -1) : ASTExpression(), offset(noffset) {}

   virtual Kind getKind() const override {
      return Kind::AEK_ImplicitArg;
   }
};

class ASTContextModExpression : public ASTExpression {
public:
   enum ACMEKind {
      ACMEK_SortBy,
      ACMEK_SortDescBy,
      ACMEK_Where, 
   };

   //! Context mod kind.
   ACMEKind acmeKind;

   //! Expression.
   ASTExpression::Ptr expression;

public:
   ASTContextModExpression(ACMEKind k, ASTExpression* e) :
      ASTExpression(),
      acmeKind(k),
      expression(e)
   {
      loc.lastLine = e->loc.lastLine;
      loc.lastCol = e->loc.lastCol;
   }

   virtual Kind getKind() const override {
      return Kind::AEK_ContextMod;
   }
};

class ASTReturnsExpression : public ASTExpression {
public:
   //! Return type.
   ASTNameExpression::Ptr typeName;

public:
   ASTReturnsExpression() : ASTExpression() {}
   ASTReturnsExpression(ASTNameExpression* tn) : ASTExpression(tn->loc), typeName(tn) {}

   virtual Kind getKind() const override {
      return Kind::AEK_Returns;
   }
};

class ASTClosureExpression : public ASTExpression {
public:
   //! Closure.
   ASTClosureDeclaration::Ptr closure;

public:
   ASTClosureExpression(ASTClosureDeclaration* cd) : ASTExpression(cd->loc), closure(cd) {}

   virtual Kind getKind() const override {
      return Kind::AEK_Closure;
   }
};

class ASTBackquoteExpression : public ASTExpression {
public:
   //! Command to execute.
   std::string command;

public:
   ASTBackquoteExpression(const char* cmd) : ASTExpression(), command(cmd) {}

   virtual Kind getKind() const override {
      return Kind::AEK_Backquote;
   }
};

class ASTRegexExpression : public ASTExpression {
public:
   //! Regex string.
   std::string str;

   //! Whether this is an extract regex.
   bool extractRegex;

   //! Whether the regex is case sensitive.
   bool caseSensitive = true;

   //! Whether the regex is of extended type.
   bool extended = false;

   bool dotAll = false;

   //! Whether the regex is multi-line.
   bool multiline = false;

   // only valid for extract regex
   bool global = false;

public:
   ASTRegexExpression(bool extract = False) : ASTExpression(), extractRegex(extract) {}

   virtual Kind getKind() const override {
      return Kind::AEK_Regex;
   }
};

class ASTRegexSubstExpression : public ASTExpression {
public:
   //! Regex source string.
   std::string source;

   //! Regex target string.
   std::string target;

   //! Whether the regex is case sensitive.
   bool caseSensitive = true;

   //! Whether the regex is of extended type.
   bool extended = false;

   bool dotAll = false;

   //! Whether the regex is multi-line.
   bool multiline = false;

   bool global = false;

public:
   ASTRegexSubstExpression() : ASTExpression() {}

   virtual Kind getKind() const override {
      return Kind::AEK_RegexSubst;
   }
};

class ASTRegexTransExpression : public ASTExpression {
public:
   //! Regex source string.
   std::string source;

   //! Regex target string.
   std::string target;

public:
   ASTRegexTransExpression() : ASTExpression() {}

   virtual Kind getKind() const override {
      return Kind::AEK_RegexTrans;
   }
};

class ASTConstrInitExpression : public ASTExpression {
public:
   //! Pointer type.
   using Ptr = std::unique_ptr<ASTConstrInitExpression>;

public:
   //! Class constructor (base class) initializations.
   std::vector<ASTCallExpression*> inits;

public:
   ASTConstrInitExpression(std::vector<ASTCallExpression*>* iv) : ASTExpression() {
      if (iv)
         inits.swap(*iv);
   }

   virtual ~ASTConstrInitExpression() {
      for (unsigned int i = 0, count = inits.size(); i < count; i++)
         delete inits[i];
      inits.clear();
   }

   virtual Kind getKind() const override {
      return Kind::AEK_ConstrInit;
   }
};

#endif // _QLS_ASTEXPRESSION_H
