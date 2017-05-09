/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  FindMatchingSymbolsQuery.cpp

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

#include "queries/FindMatchingSymbolsQuery.h"

#include <cstring>
#include <memory>

#include "ast/AST.h"

static void copyLwr(const char* src, char* dest, size_t n) {
    for (size_t i = 0; i < n && src[i]; i++)
        dest[i] = (src[i] > 64 && src[i] < 91) ? src[i]+32 : src[i];
}

static bool matches(const std::string& name, const std::string& query) {
    const char* q = query.c_str();
    char first[3];
    first[0] = q[0];
    first[1] = (q[0] > 64 && q[0] < 91) ? q[0]+32 : ((q[0] > 93 && q[0] < 123) ? q[0]-32 : '\0');
    first[2] = '\0';

    // Find first character of the query (case insensitive).
    size_t pos = name.find_first_of(first);
    if (pos != std::string::npos) {
        char lwrName[512];
        char lwrQuery[512];
        size_t querySize = query.size() % 512;
        copyLwr(name.c_str(), lwrName, querySize);
        copyLwr(query.c_str(), lwrQuery, querySize);
        return !strncmp(lwrName, lwrQuery, querySize);
    }
    return false;
}

void FindMatchingSymbolsQuery::inDeclaration(std::vector<ASTSymbolInfo>* vec, ASTDeclaration* decl, const std::string& query) {
    if (!decl)
        return;

    switch (decl->getKind()) {
        case ASTDeclaration::Kind::ADK_Class: {
            ASTClassDeclaration* d = static_cast<ASTClassDeclaration*>(decl);
            if (matches(d->name.name, query))
                vec->push_back(ASTSymbolInfo(ASYK_Class, &d->name));
            for (unsigned int i = 0, count = d->inherits.size(); i < count; i++)
                inDeclaration(vec, d->inherits[i], query);
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++)
                inDeclaration(vec, d->declarations[i], query);
            break;
        }
        case ASTDeclaration::Kind::ADK_Closure: {
            ASTClosureDeclaration* d = static_cast<ASTClosureDeclaration*>(decl);
            inExpression(vec, d->returnType.get(), query);
            inExpression(vec, d->params.get(), query);
            inStatement(vec, d->body.get(), query);
            break;
        }
        case ASTDeclaration::Kind::ADK_Constant: {
            ASTConstantDeclaration* d = static_cast<ASTConstantDeclaration*>(decl);
            if (matches(d->name.name, query))
                vec->push_back(ASTSymbolInfo(ASYK_Constant, &d->name));
            inExpression(vec, d->value.get(), query);
            break;
        }
        case ASTDeclaration::Kind::ADK_Function: {
            ASTFunctionDeclaration* d = static_cast<ASTFunctionDeclaration*>(decl);
            if (matches(d->name.name, query))
                vec->push_back(ASTSymbolInfo(ASYK_Function, &d->name));
            inExpression(vec, d->returnType.get(), query);
            inExpression(vec, d->params.get(), query);
            inExpression(vec, d->inits.get(), query);
            inStatement(vec, d->body.get(), query);
            break;
        }
        case ASTDeclaration::Kind::ADK_MemberGroup: {
            ASTMemberGroupDeclaration* d = static_cast<ASTMemberGroupDeclaration*>(decl);
            for (unsigned int i = 0, count = d->members.size(); i < count; i++)
                inExpression(vec, d->members[i], query);
            break;
        }
        case ASTDeclaration::Kind::ADK_Namespace: {
            ASTNamespaceDeclaration* d = static_cast<ASTNamespaceDeclaration*>(decl);
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++)
                inDeclaration(vec, d->declarations[i], query);
            break;
        }
        case ASTDeclaration::Kind::ADK_Superclass: {
            ASTSuperclassDeclaration* d = static_cast<ASTSuperclassDeclaration*>(decl);
            if (matches(d->name.name, query))
                vec->push_back(ASTSymbolInfo(ASYK_Class, &d->name));
            break;
        }
        case ASTDeclaration::Kind::ADK_Variable: {
            ASTVariableDeclaration* d = static_cast<ASTVariableDeclaration*>(decl);
            if (matches(d->name.name, query))
                vec->push_back(ASTSymbolInfo(ASYK_Variable, &d->name));
            break;
        }
        case ASTDeclaration::Kind::ADK_VarList: {
            ASTVarListDeclaration* d = static_cast<ASTVarListDeclaration*>(decl);
            inExpression(vec, d->variables.get(), query);
            break;
        }
        default:
            break;
    }
}

void FindMatchingSymbolsQuery::inExpression(std::vector<ASTSymbolInfo>* vec, ASTExpression* expr, const std::string& query) {
    if (!expr)
        return;

    switch (expr->getKind()) {
        case ASTExpression::Kind::AEK_Access: {
            ASTAccessExpression* e = static_cast<ASTAccessExpression*>(expr);
            inExpression(vec, e->variable.get(), query);
            inExpression(vec, e->member.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_Assignment: {
            ASTAssignmentExpression* e = static_cast<ASTAssignmentExpression*>(expr);
            inExpression(vec, e->left.get(), query);
            inExpression(vec, e->right.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_Backquote:
            break;
        case ASTExpression::Kind::AEK_Binary: {
            ASTBinaryExpression* e = static_cast<ASTBinaryExpression*>(expr);
            inExpression(vec, e->left.get(), query);
            inExpression(vec, e->right.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_Call: {
            ASTCallExpression* e = static_cast<ASTCallExpression*>(expr);
            inExpression(vec, e->target.get(), query);
            inExpression(vec, e->args.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_Case: {
            ASTCaseExpression* e = static_cast<ASTCaseExpression*>(expr);
            inExpression(vec, e->caseExpr.get(), query);
            inStatement(vec, e->statements.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_Cast: {
            ASTCastExpression* e = static_cast<ASTCastExpression*>(expr);
            //inName(vec, e->castType, query); TODO
            inExpression(vec, e->obj.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_Closure: {
            ASTClosureExpression* e = static_cast<ASTClosureExpression*>(expr);
            inDeclaration(vec, e->closure.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_ConstrInit: {
            ASTConstrInitExpression* e = static_cast<ASTConstrInitExpression*>(expr);
            for (unsigned int i = 0, count = e->inits.size(); i < count; i++)
                inExpression(vec, e->inits[i], query);
            break;
        }
        case ASTExpression::Kind::AEK_ContextMod: {
            ASTContextModExpression* e = static_cast<ASTContextModExpression*>(expr);
            inExpression(vec, e->expression.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_ContextRow:
            break;
        case ASTExpression::Kind::AEK_Decl: {
            ASTDeclExpression* e = static_cast<ASTDeclExpression*>(expr);
            inDeclaration(vec, e->declaration.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_Find: {
            ASTFindExpression* e = static_cast<ASTFindExpression*>(expr);
            inExpression(vec, e->result.get(), query);
            inExpression(vec, e->data.get(), query);
            inExpression(vec, e->where.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_Hash: {
            ASTHashExpression* e = static_cast<ASTHashExpression*>(expr);
            for (unsigned int i = 0, count = e->elements.size(); i < count; i++)
                inExpression(vec, e->elements[i], query);
            break;
        }
        case ASTExpression::Kind::AEK_HashElement: {
            ASTHashElementExpression* e = static_cast<ASTHashElementExpression*>(expr);
            inExpression(vec, e->key.get(), query);
            inExpression(vec, e->value.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_ImplicitArg:
        case ASTExpression::Kind::AEK_ImplicitElem:
            break;
        case ASTExpression::Kind::AEK_Index: {
            ASTIndexExpression* e = static_cast<ASTIndexExpression*>(expr);
            inExpression(vec, e->variable.get(), query);
            inExpression(vec, e->index.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_List: {
            ASTListExpression* e = static_cast<ASTListExpression*>(expr);
            for (unsigned int i = 0, count = e->elements.size(); i < count; i++)
                inExpression(vec, e->elements[i], query);
            break;
        }
        case ASTExpression::Kind::AEK_Literal:
            break;
        case ASTExpression::Kind::AEK_Name: {
            ASTNameExpression* e = static_cast<ASTNameExpression*>(expr);
            //inName(vec, e->name, query); TODO
            break;
        }
        case ASTExpression::Kind::AEK_Regex:
        case ASTExpression::Kind::AEK_RegexSubst:
        case ASTExpression::Kind::AEK_RegexTrans:
            break;
        case ASTExpression::Kind::AEK_Returns: {
            ASTReturnsExpression* e = static_cast<ASTReturnsExpression*>(expr);
            inExpression(vec, e->typeName.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_SwitchBody: {
            ASTSwitchBodyExpression* e = static_cast<ASTSwitchBodyExpression*>(expr);
            for (unsigned int i = 0, count = e->cases.size(); i < count; i++)
                inExpression(vec, e->cases[i], query);
            break;
        }
        case ASTExpression::Kind::AEK_Ternary: {
            ASTTernaryExpression* e = static_cast<ASTTernaryExpression*>(expr);
            inExpression(vec, e->condition.get(), query);
            inExpression(vec, e->exprTrue.get(), query);
            inExpression(vec, e->exprFalse.get(), query);
            break;
        }
        case ASTExpression::Kind::AEK_Unary: {
            ASTUnaryExpression* e = static_cast<ASTUnaryExpression*>(expr);
            inExpression(vec, e->expression.get(), query);
            break;
        }
        default:
            break;
    }
}

void FindMatchingSymbolsQuery::inName(std::vector<ASTSymbolInfo>* vec, ASTName& name, const std::string& query) {
     // TODO
    return;
}

void FindMatchingSymbolsQuery::inName(std::vector<ASTSymbolInfo>* vec, ASTName* name, const std::string& query) {
    if (!name)
        return;
     // TODO
    return;
}

void FindMatchingSymbolsQuery::inStatement(std::vector<ASTSymbolInfo>* vec, ASTStatement* stmt, const std::string& query) {
    if (!stmt)
        return;

    switch (stmt->getKind()) {
        case ASTStatement::Kind::ASK_Block: {
            ASTStatementBlock* s = static_cast<ASTStatementBlock*>(stmt);
            for (unsigned int i = 0, count = s->statements.size(); i < count; i++)
                inStatement(vec, s->statements[i], query);
            break;
        }
        case ASTStatement::Kind::ASK_Break:
            break;
        case ASTStatement::Kind::ASK_Call: {
            ASTCallStatement* s = static_cast<ASTCallStatement*>(stmt);
            inExpression(vec, s->call.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_Context: {
            ASTContextStatement* s = static_cast<ASTContextStatement*>(stmt);
            inExpression(vec, s->name.get(), query);
            inExpression(vec, s->data.get(), query);
            for (unsigned int i = 0, count = s->contextMods.size(); i < count; i++)
                inExpression(vec, s->contextMods[i], query);
            inStatement(vec, s->statements.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_Continue:
            break;
        case ASTStatement::Kind::ASK_DoWhile: {
            ASTDoWhileStatement* s = static_cast<ASTDoWhileStatement*>(stmt);
            inExpression(vec, s->condition.get(), query);
            inStatement(vec, s->statement.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_Expression: {
            ASTExpressionStatement* s = static_cast<ASTExpressionStatement*>(stmt);
            inExpression(vec, s->expression.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_For: {
            ASTForStatement* s = static_cast<ASTForStatement*>(stmt);
            inExpression(vec, s->init.get(), query);
            inExpression(vec, s->condition.get(), query);
            inExpression(vec, s->iteration.get(), query);
            inStatement(vec, s->statement.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_Foreach: {
            ASTForeachStatement* s = static_cast<ASTForeachStatement*>(stmt);
            inExpression(vec, s->value.get(), query);
            inExpression(vec, s->source.get(), query);
            inStatement(vec, s->statement.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_If: {
            ASTIfStatement* s = static_cast<ASTIfStatement*>(stmt);
            inExpression(vec, s->condition.get(), query);
            inStatement(vec, s->stmtThen.get(), query);
            inStatement(vec, s->stmtElse.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_OnBlockExit: {
            ASTOnBlockExitStatement* s = static_cast<ASTOnBlockExitStatement*>(stmt);
            inStatement(vec, s->statement.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_Rethrow:
            break;
        case ASTStatement::Kind::ASK_Return: {
            ASTReturnStatement* s = static_cast<ASTReturnStatement*>(stmt);
            inExpression(vec, s->retval.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_Summarize: {
            ASTSummarizeStatement* s = static_cast<ASTSummarizeStatement*>(stmt);
            inExpression(vec, s->name.get(), query);
            inExpression(vec, s->data.get(), query);
            inExpression(vec, s->by.get(), query);
            for (unsigned int i = 0, count = s->contextMods.size(); i < count; i++)
                inExpression(vec, s->contextMods[i], query);
            inStatement(vec, s->statements.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_Switch: {
            ASTSwitchStatement* s = static_cast<ASTSwitchStatement*>(stmt);
            inExpression(vec, s->variable.get(), query);
            inExpression(vec, s->body.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_ThreadExit:
            break;
        case ASTStatement::Kind::ASK_Throw: {
            ASTThrowStatement* s = static_cast<ASTThrowStatement*>(stmt);
            inExpression(vec, s->expression.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_Try: {
            ASTTryStatement* s = static_cast<ASTTryStatement*>(stmt);
            inStatement(vec, s->tryStmt.get(), query);
            inExpression(vec, s->catchVar.get(), query);
            inStatement(vec, s->catchStmt.get(), query);
            break;
        }
        case ASTStatement::Kind::ASK_While: {
            ASTWhileStatement* s = static_cast<ASTWhileStatement*>(stmt);
            inExpression(vec, s->condition.get(), query);
            inStatement(vec, s->statement.get(), query);
            break;
        }
        default:
            break;
    }
}

std::vector<ASTSymbolInfo>* FindMatchingSymbolsQuery::find(ASTTree* tree, const std::string& query) {
    if (!tree)
        return nullptr;

    std::unique_ptr<std::vector<ASTSymbolInfo> > vec(new std::vector<ASTSymbolInfo>);
    vec->reserve(64);
    for (unsigned int i = 0, count = tree->nodes.size(); i < count; i++) {
        ASTNode* node = tree->nodes[i];
        switch (node->getNodeType()) {
            case ANT_Declaration: {
                ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
                inDeclaration(vec.get(), decl, query);
                break;
            }
            case ANT_Expression: {
                ASTExpression* expr = static_cast<ASTExpression*>(node);
                inExpression(vec.get(), expr, query);
                break;
            }
            case ANT_Name: {
                ASTName* name = static_cast<ASTName*>(node);
                inName(vec.get(), name, query);
                break;
            }
            case ANT_ParseOption:
                break;
            case ANT_Statement: {
                ASTStatement* stmt = static_cast<ASTStatement*>(node);
                inStatement(vec.get(), stmt, query);
                break;
            }
            case ANT_None:
            default:
                break;
        }
    }
    return vec.release();
}
