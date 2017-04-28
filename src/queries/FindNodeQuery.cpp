/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  FindNodeQuery.cpp

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
*/

#include "queries/FindNodeQuery.h"

#include "ast/AST.h"

ASTNode* FindNodeQuery::findNodeInDeclaration(ASTDeclaration* decl, ast_loc_t line, ast_loc_t col) {
    if (!decl)
        return nullptr;
    if (!decl->loc.inside(line, col))
        return nullptr;

    ASTNode* result = nullptr;
    switch (decl->getKind()) {
        case ASTDeclaration::Kind::ADK_Class: {
            ASTClassDeclaration* d = static_cast<ASTClassDeclaration*>(decl);
            result = findNodeInName(d->name, line, col);
            if (result) return result;
            for (unsigned int i = 0, count = d->inherits.size(); i < count; i++) {
                result = findNodeInDeclaration(d->inherits[i], line, col);
                if (result) return result;
            }
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++) {
                result = findNodeInDeclaration(d->declarations[i], line, col);
                if (result) return result;
            }
            break;
        }
        case ASTDeclaration::Kind::ADK_Closure: {
            ASTClosureDeclaration* d = static_cast<ASTClosureDeclaration*>(decl);
            result = findNodeInExpression(d->returnType.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(d->params.get(), line, col);
            if (result) return result;
            result = findNodeInStatement(d->body.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTDeclaration::Kind::ADK_Constant: {
            ASTConstantDeclaration* d = static_cast<ASTConstantDeclaration*>(decl);
            result = findNodeInName(d->name, line, col);
            if (result) return result;
            result = findNodeInExpression(d->value.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTDeclaration::Kind::ADK_Function: {
            ASTFunctionDeclaration* d = static_cast<ASTFunctionDeclaration*>(decl);
            result = findNodeInName(d->name, line, col);
            if (result) return result;
            result = findNodeInExpression(d->returnType.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(d->params.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(d->inits.get(), line, col);
            if (result) return result;
            result = findNodeInStatement(d->body.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTDeclaration::Kind::ADK_MemberGroup: {
            ASTMemberGroupDeclaration* d = static_cast<ASTMemberGroupDeclaration*>(decl);
            for (unsigned int i = 0, count = d->members.size(); i < count; i++) {
                result = findNodeInExpression(d->members[i], line, col);
                if (result) return result;
            }
            break;
        }
        case ASTDeclaration::Kind::ADK_Namespace: {
            ASTNamespaceDeclaration* d = static_cast<ASTNamespaceDeclaration*>(decl);
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++) {
                result = findNodeInDeclaration(d->declarations[i], line, col);
                if (result) return result;
            }
            break;
        }
        case ASTDeclaration::Kind::ADK_Superclass: {
            ASTSuperclassDeclaration* d = static_cast<ASTSuperclassDeclaration*>(decl);
            result = findNodeInName(d->name, line, col);
            if (result) return result;
            break;
        }
        case ASTDeclaration::Kind::ADK_Variable: {
            ASTVariableDeclaration* d = static_cast<ASTVariableDeclaration*>(decl);
            result = findNodeInName(d->typeName, line, col);
            if (result) return result;
            result = findNodeInName(d->name, line, col);
            if (result) return result;
            break;
        }
        case ASTDeclaration::Kind::ADK_VarList: {
            ASTVarListDeclaration* d = static_cast<ASTVarListDeclaration*>(decl);
            result = findNodeInExpression(d->variables.get(), line, col);
            if (result) return result;
            break;
        }
        default:
            break;
    }
    return decl;
}

ASTNode* FindNodeQuery::findNodeInExpression(ASTExpression* expr, ast_loc_t line, ast_loc_t col) {
    if (!expr)
        return nullptr;
    if (!expr->loc.inside(line, col))
        return nullptr;

    ASTNode* result = nullptr;
    switch (expr->getKind()) {
        case ASTExpression::Kind::AEK_Access: {
            ASTAccessExpression* e = static_cast<ASTAccessExpression*>(expr);
            result = findNodeInExpression(e->variable.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(e->member.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_Assignment: {
            ASTAssignmentExpression* e = static_cast<ASTAssignmentExpression*>(expr);
            result = findNodeInExpression(e->left.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(e->right.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_Backquote:
            return expr;
        case ASTExpression::Kind::AEK_Binary: {
            ASTBinaryExpression* e = static_cast<ASTBinaryExpression*>(expr);
            result = findNodeInExpression(e->left.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(e->right.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_Call: {
            ASTCallExpression* e = static_cast<ASTCallExpression*>(expr);
            result = findNodeInExpression(e->target.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(e->args.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_Case: {
            ASTCaseExpression* e = static_cast<ASTCaseExpression*>(expr);
            result = findNodeInExpression(e->caseExpr.get(), line, col);
            if (result) return result;
            result = findNodeInStatement(e->statements.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_Cast: {
            ASTCastExpression* e = static_cast<ASTCastExpression*>(expr);
            result = findNodeInName(e->castType, line, col);
            if (result) return result;
            result = findNodeInExpression(e->obj.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_Closure: {
            ASTClosureExpression* e = static_cast<ASTClosureExpression*>(expr);
            result = findNodeInDeclaration(e->closure.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_ConstrInit: {
            ASTConstrInitExpression* e = static_cast<ASTConstrInitExpression*>(expr);
            for (unsigned int i = 0, count = e->inits.size(); i < count; i++) {
                result = findNodeInExpression(e->inits[i], line, col);
                if (result) return result;
            }
            break;
        }
        case ASTExpression::Kind::AEK_ContextMod: {
            ASTContextModExpression* e = static_cast<ASTContextModExpression*>(expr);
            result = findNodeInExpression(e->expression.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_ContextRow:
            return expr;
        case ASTExpression::Kind::AEK_Decl: {
            ASTDeclExpression* e = static_cast<ASTDeclExpression*>(expr);
            result = findNodeInDeclaration(e->declaration.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_Find: {
            ASTFindExpression* e = static_cast<ASTFindExpression*>(expr);
            result = findNodeInExpression(e->result.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(e->data.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(e->where.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_Hash: {
            ASTHashExpression* e = static_cast<ASTHashExpression*>(expr);
            for (unsigned int i = 0, count = e->elements.size(); i < count; i++) {
                result = findNodeInExpression(e->elements[i], line, col);
                if (result) return result;
            }
            break;
        }
        case ASTExpression::Kind::AEK_HashElement: {
            ASTHashElementExpression* e = static_cast<ASTHashElementExpression*>(expr);
            result = findNodeInExpression(e->key.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(e->value.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_ImplicitArg:
        case ASTExpression::Kind::AEK_ImplicitElem:
            return expr;
        case ASTExpression::Kind::AEK_Index: {
            ASTIndexExpression* e = static_cast<ASTIndexExpression*>(expr);
            result = findNodeInExpression(e->variable.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(e->index.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_List: {
            ASTListExpression* e = static_cast<ASTListExpression*>(expr);
            for (unsigned int i = 0, count = e->elements.size(); i < count; i++) {
                result = findNodeInExpression(e->elements[i], line, col);
                if (result) return result;
            }
            break;
        }
        case ASTExpression::Kind::AEK_Literal:
            return expr;
        case ASTExpression::Kind::AEK_Name: {
            ASTNameExpression* e = static_cast<ASTNameExpression*>(expr);
            return &e->name;
        }
        case ASTExpression::Kind::AEK_Regex:
        case ASTExpression::Kind::AEK_RegexSubst:
        case ASTExpression::Kind::AEK_RegexTrans:
            return expr;
        case ASTExpression::Kind::AEK_Returns: {
            ASTReturnsExpression* e = static_cast<ASTReturnsExpression*>(expr);
            result = findNodeInExpression(e->typeName.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_SwitchBody: {
            ASTSwitchBodyExpression* e = static_cast<ASTSwitchBodyExpression*>(expr);
            for (unsigned int i = 0, count = e->cases.size(); i < count; i++) {
                result = findNodeInExpression(e->cases[i], line, col);
                if (result) return result;
            }
            break;
        }
        case ASTExpression::Kind::AEK_Ternary: {
            ASTTernaryExpression* e = static_cast<ASTTernaryExpression*>(expr);
            result = findNodeInExpression(e->condition.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(e->exprTrue.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(e->exprFalse.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTExpression::Kind::AEK_Unary: {
            ASTUnaryExpression* e = static_cast<ASTUnaryExpression*>(expr);
            result = findNodeInExpression(e->expression.get(), line, col);
            if (result) return result;
            break;
        }
        default:
            break;
    }
    return expr;
}

ASTNode* FindNodeQuery::findNodeInName(ASTName& name, ast_loc_t line, ast_loc_t col) {
    if (!name.loc.inside(line, col))
        return nullptr;
    return &name;
}

ASTNode* FindNodeQuery::findNodeInName(ASTName* name, ast_loc_t line, ast_loc_t col) {
    if (!name)
        return nullptr;
    if (!name->loc.inside(line, col))
        return nullptr;
    return name;
}

ASTNode* FindNodeQuery::findNodeInParseOption(ASTParseOption* po, ast_loc_t line, ast_loc_t col) {
    if (!po)
        return nullptr;
    if (!po->loc.inside(line, col))
        return nullptr;
    return po;
}

ASTNode* FindNodeQuery::findNodeInStatement(ASTStatement* stmt, ast_loc_t line, ast_loc_t col) {
    if (!stmt)
        return nullptr;
    if (!stmt->loc.inside(line, col))
        return nullptr;

    ASTNode* result = nullptr;
    switch (stmt->getKind()) {
        case ASTStatement::Kind::ASK_Block: {
            ASTStatementBlock* s = static_cast<ASTStatementBlock*>(stmt);
            for (unsigned int i = 0, count = s->statements.size(); i < count; i++) {
                result = findNodeInStatement(s->statements[i], line, col);
                if (result) return result;
            }
            break;
        }
        case ASTStatement::Kind::ASK_Break:
            return stmt;
        case ASTStatement::Kind::ASK_Call: {
            ASTCallStatement* s = static_cast<ASTCallStatement*>(stmt);
            result = findNodeInExpression(s->call.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_Context: {
            ASTContextStatement* s = static_cast<ASTContextStatement*>(stmt);
            result = findNodeInExpression(s->name.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(s->data.get(), line, col);
            if (result) return result;
            for (unsigned int i = 0, count = s->contextMods.size(); i < count; i++) {
                result = findNodeInExpression(s->contextMods[i], line, col);
                if (result) return result;
            }
            result = findNodeInStatement(s->statements.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_Continue:
            return stmt;
        case ASTStatement::Kind::ASK_DoWhile: {
            ASTDoWhileStatement* s = static_cast<ASTDoWhileStatement*>(stmt);
            result = findNodeInExpression(s->condition.get(), line, col);
            if (result) return result;
            result = findNodeInStatement(s->statement.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_Expression: {
            ASTExpressionStatement* s = static_cast<ASTExpressionStatement*>(stmt);
            result = findNodeInExpression(s->expression.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_For: {
            ASTForStatement* s = static_cast<ASTForStatement*>(stmt);
            result = findNodeInExpression(s->init.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(s->condition.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(s->iteration.get(), line, col);
            if (result) return result;
            result = findNodeInStatement(s->statement.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_Foreach: {
            ASTForeachStatement* s = static_cast<ASTForeachStatement*>(stmt);
            result = findNodeInExpression(s->value.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(s->source.get(), line, col);
            if (result) return result;
            result = findNodeInStatement(s->statement.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_If: {
            ASTIfStatement* s = static_cast<ASTIfStatement*>(stmt);
            result = findNodeInExpression(s->condition.get(), line, col);
            if (result) return result;
            result = findNodeInStatement(s->stmtThen.get(), line, col);
            if (result) return result;
            result = findNodeInStatement(s->stmtElse.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_OnBlockExit: {
            ASTOnBlockExitStatement* s = static_cast<ASTOnBlockExitStatement*>(stmt);
            result = findNodeInStatement(s->statement.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_Rethrow:
            return stmt;
        case ASTStatement::Kind::ASK_Return: {
            ASTReturnStatement* s = static_cast<ASTReturnStatement*>(stmt);
            result = findNodeInExpression(s->retval.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_Summarize: {
            ASTSummarizeStatement* s = static_cast<ASTSummarizeStatement*>(stmt);
            result = findNodeInExpression(s->name.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(s->data.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(s->by.get(), line, col);
            if (result) return result;
            for (unsigned int i = 0, count = s->contextMods.size(); i < count; i++) {
                result = findNodeInExpression(s->contextMods[i], line, col);
                if (result) return result;
            }
            result = findNodeInStatement(s->statements.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_Switch: {
            ASTSwitchStatement* s = static_cast<ASTSwitchStatement*>(stmt);
            result = findNodeInExpression(s->variable.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(s->body.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_ThreadExit:
            return stmt;
        case ASTStatement::Kind::ASK_Throw: {
            ASTThrowStatement* s = static_cast<ASTThrowStatement*>(stmt);
            result = findNodeInExpression(s->expression.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_Try: {
            ASTTryStatement* s = static_cast<ASTTryStatement*>(stmt);
            result = findNodeInStatement(s->tryStmt.get(), line, col);
            if (result) return result;
            result = findNodeInExpression(s->catchVar.get(), line, col);
            if (result) return result;
            result = findNodeInStatement(s->catchStmt.get(), line, col);
            if (result) return result;
            break;
        }
        case ASTStatement::Kind::ASK_While: {
            ASTWhileStatement* s = static_cast<ASTWhileStatement*>(stmt);
            result = findNodeInExpression(s->condition.get(), line, col);
            if (result) return result;
            result = findNodeInStatement(s->statement.get(), line, col);
            if (result) return result;
            break;
        }
        default:
            break;
    }
    return stmt;
}

ASTNode* FindNodeQuery::findNode(ASTTree* tree, ast_loc_t line, ast_loc_t col) {
    if (!tree)
        return nullptr;

    for (unsigned int i = 0, count = tree->nodes.size(); i < count; i++) {
        ASTNode* result = nullptr;
        ASTNode* node = tree->nodes[i];
        switch (node->getNodeType()) {
            case ANT_Declaration: {
                ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
                result = findNodeInDeclaration(decl, line, col);
                break;
            }
            case ANT_Expression: {
                ASTExpression* expr = static_cast<ASTExpression*>(node);
                result = findNodeInExpression(expr, line, col);
                break;
            }
            case ANT_Name: {
                ASTName* name = static_cast<ASTName*>(node);
                result = findNodeInName(name, line, col);
                break;
            }
            case ANT_ParseOption: {
                ASTParseOption* po = static_cast<ASTParseOption*>(node);
                result = findNodeInParseOption(po, line, col);
                break;
            }
            case ANT_Statement: {
                ASTStatement* stmt = static_cast<ASTStatement*>(node);
                result = findNodeInStatement(stmt, line, col);
                break;
            }
            case ANT_None:
            default:
                break;
        }
        if (result)
            return result;
    }
    return nullptr;
}
