/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  FindSymbolsQuery.cpp

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

#include "queries/FindSymbolsQuery.h"

#include <memory>

#include "ast/AST.h"

void FindSymbolsQuery::findSymbolsInDecl(std::vector<ASTSymbolInfo>* vec, ASTDeclaration* decl) {
    if (!decl)
        return;

    switch (decl->getKind()) {
        case ASTDeclaration::Kind::ADK_Class: {
            ASTClassDeclaration* d = static_cast<ASTClassDeclaration*>(decl);
            vec->push_back(ASTSymbolInfo(ASYK_Class, &d->name));
            for (unsigned int i = 0, count = d->inherits.size(); i < count; i++)
                findSymbolsInDecl(vec, d->inherits[i]);
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++)
                findSymbolsInDecl(vec, d->declarations[i]);
            break;
        }
        case ASTDeclaration::Kind::ADK_Closure: {
            ASTClosureDeclaration* d = static_cast<ASTClosureDeclaration*>(decl);
            findSymbolsInExpr(vec, d->returnType.get());
            findSymbolsInExpr(vec, d->params.get());
            findSymbolsInStmt(vec, d->body.get());
            break;
        }
        case ASTDeclaration::Kind::ADK_Constant: {
            ASTConstantDeclaration* d = static_cast<ASTConstantDeclaration*>(decl);
            vec->push_back(ASTSymbolInfo(ASYK_Constant, &d->name));
            findSymbolsInExpr(vec, d->value.get());
            break;
        }
        case ASTDeclaration::Kind::ADK_Function: {
            ASTFunctionDeclaration* d = static_cast<ASTFunctionDeclaration*>(decl);
            vec->push_back(ASTSymbolInfo(ASYK_Function, &d->name));
            findSymbolsInExpr(vec, d->returnType.get());
            findSymbolsInExpr(vec, d->params.get());
            findSymbolsInExpr(vec, d->inits.get());
            findSymbolsInStmt(vec, d->body.get());
            break;
        }
        case ASTDeclaration::Kind::ADK_MemberGroup: {
            ASTMemberGroupDeclaration* d = static_cast<ASTMemberGroupDeclaration*>(decl);
            for (unsigned int i = 0, count = d->members.size(); i < count; i++)
                findSymbolsInExpr(vec, d->members[i]);
            break;
        }
        case ASTDeclaration::Kind::ADK_Namespace: {
            ASTNamespaceDeclaration* d = static_cast<ASTNamespaceDeclaration*>(decl);
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++)
                findSymbolsInDecl(vec, d->declarations[i]);
            break;
        }
        case ASTDeclaration::Kind::ADK_Superclass: {
            ASTSuperclassDeclaration* d = static_cast<ASTSuperclassDeclaration*>(decl);
            vec->push_back(ASTSymbolInfo(ASYK_Class, &d->name));
            break;
        }
        case ASTDeclaration::Kind::ADK_Variable: {
            ASTVariableDeclaration* d = static_cast<ASTVariableDeclaration*>(decl);
            vec->push_back(ASTSymbolInfo(ASYK_Variable, &d->name));
            break;
        }
        case ASTDeclaration::Kind::ADK_VarList: {
            ASTVarListDeclaration* d = static_cast<ASTVarListDeclaration*>(decl);
            findSymbolsInExpr(vec, d->variables.get());
            break;
        }
        default:
            break;
    }
}

void FindSymbolsQuery::findSymbolsInExpr(std::vector<ASTSymbolInfo>* vec, ASTExpression* expr) {
    if (!expr)
        return;

    switch (expr->getKind()) {
        case ASTExpression::Kind::AEK_Access: {
            ASTAccessExpression* e = static_cast<ASTAccessExpression*>(expr);
            findSymbolsInExpr(vec, e->variable.get());
            findSymbolsInExpr(vec, e->member.get());
            break;
        }
        case ASTExpression::Kind::AEK_Assignment: {
            ASTAssignmentExpression* e = static_cast<ASTAssignmentExpression*>(expr);
            findSymbolsInExpr(vec, e->left.get());
            findSymbolsInExpr(vec, e->right.get());
            break;
        }
        case ASTExpression::Kind::AEK_Backquote:
            break;
        case ASTExpression::Kind::AEK_Binary: {
            ASTBinaryExpression* e = static_cast<ASTBinaryExpression*>(expr);
            findSymbolsInExpr(vec, e->left.get());
            findSymbolsInExpr(vec, e->right.get());
            break;
        }
        case ASTExpression::Kind::AEK_Call: {
            ASTCallExpression* e = static_cast<ASTCallExpression*>(expr);
            findSymbolsInExpr(vec, e->target.get());
            findSymbolsInExpr(vec, e->args.get());
            break;
        }
        case ASTExpression::Kind::AEK_Case: {
            ASTCaseExpression* e = static_cast<ASTCaseExpression*>(expr);
            findSymbolsInExpr(vec, e->caseExpr.get());
            findSymbolsInStmt(vec, e->statements.get());
            break;
        }
        case ASTExpression::Kind::AEK_Cast: {
            ASTCastExpression* e = static_cast<ASTCastExpression*>(expr);
            //findSymbolsInName(vec, e->castType); TODO
            findSymbolsInExpr(vec, e->obj.get());
            break;
        }
        case ASTExpression::Kind::AEK_Closure: {
            ASTClosureExpression* e = static_cast<ASTClosureExpression*>(expr);
            findSymbolsInDecl(vec, e->closure.get());
            break;
        }
        case ASTExpression::Kind::AEK_ConstrInit: {
            ASTConstrInitExpression* e = static_cast<ASTConstrInitExpression*>(expr);
            for (unsigned int i = 0, count = e->inits.size(); i < count; i++)
                findSymbolsInExpr(vec, e->inits[i]);
            break;
        }
        case ASTExpression::Kind::AEK_ContextMod: {
            ASTContextModExpression* e = static_cast<ASTContextModExpression*>(expr);
            findSymbolsInExpr(vec, e->expression.get());
            break;
        }
        case ASTExpression::Kind::AEK_ContextRow:
            break;
        case ASTExpression::Kind::AEK_Decl: {
            ASTDeclExpression* e = static_cast<ASTDeclExpression*>(expr);
            findSymbolsInDecl(vec, e->declaration.get());
            break;
        }
        case ASTExpression::Kind::AEK_Find: {
            ASTFindExpression* e = static_cast<ASTFindExpression*>(expr);
            findSymbolsInExpr(vec, e->result.get());
            findSymbolsInExpr(vec, e->data.get());
            findSymbolsInExpr(vec, e->where.get());
            break;
        }
        case ASTExpression::Kind::AEK_Hash: {
            ASTHashExpression* e = static_cast<ASTHashExpression*>(expr);
            for (unsigned int i = 0, count = e->elements.size(); i < count; i++)
                findSymbolsInExpr(vec, e->elements[i]);
            break;
        }
        case ASTExpression::Kind::AEK_HashElement: {
            ASTHashElementExpression* e = static_cast<ASTHashElementExpression*>(expr);
            findSymbolsInExpr(vec, e->key.get());
            findSymbolsInExpr(vec, e->value.get());
            break;
        }
        case ASTExpression::Kind::AEK_ImplicitArg:
        case ASTExpression::Kind::AEK_ImplicitElem:
            break;
        case ASTExpression::Kind::AEK_Index: {
            ASTIndexExpression* e = static_cast<ASTIndexExpression*>(expr);
            findSymbolsInExpr(vec, e->variable.get());
            findSymbolsInExpr(vec, e->index.get());
            break;
        }
        case ASTExpression::Kind::AEK_List: {
            ASTListExpression* e = static_cast<ASTListExpression*>(expr);
            for (unsigned int i = 0, count = e->elements.size(); i < count; i++)
                findSymbolsInExpr(vec, e->elements[i]);
            break;
        }
        case ASTExpression::Kind::AEK_Literal:
            break;
        case ASTExpression::Kind::AEK_Name: {
            ASTNameExpression* e = static_cast<ASTNameExpression*>(expr);
            //findSymbolsInName(vec, e->name); TODO
            break;
        }
        case ASTExpression::Kind::AEK_Regex:
        case ASTExpression::Kind::AEK_RegexSubst:
        case ASTExpression::Kind::AEK_RegexTrans:
            break;
        case ASTExpression::Kind::AEK_Returns: {
            ASTReturnsExpression* e = static_cast<ASTReturnsExpression*>(expr);
            findSymbolsInExpr(vec, e->typeName.get());
            break;
        }
        case ASTExpression::Kind::AEK_SwitchBody: {
            ASTSwitchBodyExpression* e = static_cast<ASTSwitchBodyExpression*>(expr);
            for (unsigned int i = 0, count = e->cases.size(); i < count; i++)
                findSymbolsInExpr(vec, e->cases[i]);
            break;
        }
        case ASTExpression::Kind::AEK_Ternary: {
            ASTTernaryExpression* e = static_cast<ASTTernaryExpression*>(expr);
            findSymbolsInExpr(vec, e->condition.get());
            findSymbolsInExpr(vec, e->exprTrue.get());
            findSymbolsInExpr(vec, e->exprFalse.get());
            break;
        }
        case ASTExpression::Kind::AEK_Unary: {
            ASTUnaryExpression* e = static_cast<ASTUnaryExpression*>(expr);
            findSymbolsInExpr(vec, e->expression.get());
            break;
        }
        default:
            break;
    }
}

void FindSymbolsQuery::findSymbolsInName(std::vector<ASTSymbolInfo>* vec, ASTName& name) {
    return; // TODO
}

void FindSymbolsQuery::findSymbolsInName(std::vector<ASTSymbolInfo>* vec, ASTName* name) {
    if (!name)
        return;
    return; // TODO
}

void FindSymbolsQuery::findSymbolsInStmt(std::vector<ASTSymbolInfo>* vec, ASTStatement* stmt) {
    if (!stmt)
        return;

    switch (stmt->getKind()) {
        case ASTStatement::Kind::ASK_Block: {
            ASTStatementBlock* s = static_cast<ASTStatementBlock*>(stmt);
            for (unsigned int i = 0, count = s->statements.size(); i < count; i++)
                findSymbolsInStmt(vec, s->statements[i]);
            break;
        }
        case ASTStatement::Kind::ASK_Break:
            break;
        case ASTStatement::Kind::ASK_Call: {
            ASTCallStatement* s = static_cast<ASTCallStatement*>(stmt);
            findSymbolsInExpr(vec, s->call.get());
            break;
        }
        case ASTStatement::Kind::ASK_Context: {
            ASTContextStatement* s = static_cast<ASTContextStatement*>(stmt);
            findSymbolsInExpr(vec, s->name.get());
            findSymbolsInExpr(vec, s->data.get());
            for (unsigned int i = 0, count = s->contextMods.size(); i < count; i++)
                findSymbolsInExpr(vec, s->contextMods[i]);
            findSymbolsInStmt(vec, s->statements.get());
            break;
        }
        case ASTStatement::Kind::ASK_Continue:
            break;
        case ASTStatement::Kind::ASK_DoWhile: {
            ASTDoWhileStatement* s = static_cast<ASTDoWhileStatement*>(stmt);
            findSymbolsInExpr(vec, s->condition.get());
            findSymbolsInStmt(vec, s->statement.get());
            break;
        }
        case ASTStatement::Kind::ASK_Expression: {
            ASTExpressionStatement* s = static_cast<ASTExpressionStatement*>(stmt);
            findSymbolsInExpr(vec, s->expression.get());
            break;
        }
        case ASTStatement::Kind::ASK_For: {
            ASTForStatement* s = static_cast<ASTForStatement*>(stmt);
            findSymbolsInExpr(vec, s->init.get());
            findSymbolsInExpr(vec, s->condition.get());
            findSymbolsInExpr(vec, s->iteration.get());
            findSymbolsInStmt(vec, s->statement.get());
            break;
        }
        case ASTStatement::Kind::ASK_Foreach: {
            ASTForeachStatement* s = static_cast<ASTForeachStatement*>(stmt);
            findSymbolsInExpr(vec, s->value.get());
            findSymbolsInExpr(vec, s->source.get());
            findSymbolsInStmt(vec, s->statement.get());
            break;
        }
        case ASTStatement::Kind::ASK_If: {
            ASTIfStatement* s = static_cast<ASTIfStatement*>(stmt);
            findSymbolsInExpr(vec, s->condition.get());
            findSymbolsInStmt(vec, s->stmtThen.get());
            findSymbolsInStmt(vec, s->stmtElse.get());
            break;
        }
        case ASTStatement::Kind::ASK_OnBlockExit: {
            ASTOnBlockExitStatement* s = static_cast<ASTOnBlockExitStatement*>(stmt);
            findSymbolsInStmt(vec, s->statement.get());
            break;
        }
        case ASTStatement::Kind::ASK_Rethrow:
            break;
        case ASTStatement::Kind::ASK_Return: {
            ASTReturnStatement* s = static_cast<ASTReturnStatement*>(stmt);
            findSymbolsInExpr(vec, s->retval.get());
            break;
        }
        case ASTStatement::Kind::ASK_Summarize: {
            ASTSummarizeStatement* s = static_cast<ASTSummarizeStatement*>(stmt);
            findSymbolsInExpr(vec, s->name.get());
            findSymbolsInExpr(vec, s->data.get());
            findSymbolsInExpr(vec, s->by.get());
            for (unsigned int i = 0, count = s->contextMods.size(); i < count; i++)
                findSymbolsInExpr(vec, s->contextMods[i]);
            findSymbolsInStmt(vec, s->statements.get());
            break;
        }
        case ASTStatement::Kind::ASK_Switch: {
            ASTSwitchStatement* s = static_cast<ASTSwitchStatement*>(stmt);
            findSymbolsInExpr(vec, s->variable.get());
            findSymbolsInExpr(vec, s->body.get());
            break;
        }
        case ASTStatement::Kind::ASK_ThreadExit:
            break;
        case ASTStatement::Kind::ASK_Throw: {
            ASTThrowStatement* s = static_cast<ASTThrowStatement*>(stmt);
            findSymbolsInExpr(vec, s->expression.get());
            break;
        }
        case ASTStatement::Kind::ASK_Try: {
            ASTTryStatement* s = static_cast<ASTTryStatement*>(stmt);
            findSymbolsInStmt(vec, s->tryStmt.get());
            findSymbolsInExpr(vec, s->catchVar.get());
            findSymbolsInStmt(vec, s->catchStmt.get());
            break;
        }
        case ASTStatement::Kind::ASK_While: {
            ASTWhileStatement* s = static_cast<ASTWhileStatement*>(stmt);
            findSymbolsInExpr(vec, s->condition.get());
            findSymbolsInStmt(vec, s->statement.get());
            break;
        }
        default:
            break;
    }
}

std::vector<ASTSymbolInfo>* FindSymbolsQuery::findSymbols(ASTTree* tree) {
    if (!tree)
        return nullptr;

    std::unique_ptr<std::vector<ASTSymbolInfo> > vec(new std::vector<ASTSymbolInfo>);
    vec->reserve(64);
    for (unsigned int i = 0, count = tree->nodes.size(); i < count; i++) {
        ASTNode* node = tree->nodes[i];
        switch (node->getNodeType()) {
            case ANT_Declaration: {
                ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
                findSymbolsInDecl(vec.get(), decl);
                break;
            }
            case ANT_Expression: {
                ASTExpression* expr = static_cast<ASTExpression*>(node);
                findSymbolsInExpr(vec.get(), expr);
                break;
            }
            case ANT_Name: {
                ASTName* n = static_cast<ASTName*>(node);
                findSymbolsInName(vec.get(), n);
                break;
            }
            case ANT_ParseOption:
                break;
            case ANT_Statement: {
                ASTStatement* stmt = static_cast<ASTStatement*>(node);
                findSymbolsInStmt(vec.get(), stmt);
                break;
            }
            case ANT_None:
            default:
                break;
        }
    }
    return vec.release();
}
