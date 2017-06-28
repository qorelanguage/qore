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
#include "queries/FindNodeAndParentsQuery.h"

void FindSymbolsQuery::inDeclaration(std::vector<ASTSymbolInfo>* vec, ASTDeclaration* decl) {
    if (!decl)
        return;

    switch (decl->getKind()) {
        case ASTDeclaration::Kind::ADK_Class: {
            ASTClassDeclaration* d = static_cast<ASTClassDeclaration*>(decl);
            vec->push_back(std::move(ASTSymbolInfo(ASYK_Class, &d->name)));
            for (unsigned int i = 0, count = d->inherits.size(); i < count; i++)
                inDeclaration(vec, d->inherits[i]);
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++)
                inDeclaration(vec, d->declarations[i]);
            break;
        }
        case ASTDeclaration::Kind::ADK_Closure: {
            ASTClosureDeclaration* d = static_cast<ASTClosureDeclaration*>(decl);
            inExpression(vec, d->returnType.get());
            inExpression(vec, d->params.get());
            inStatement(vec, d->body.get());
            break;
        }
        case ASTDeclaration::Kind::ADK_Constant: {
            ASTConstantDeclaration* d = static_cast<ASTConstantDeclaration*>(decl);
            vec->push_back(std::move(ASTSymbolInfo(ASYK_Constant, &d->name)));
            inExpression(vec, d->value.get());
            break;
        }
        case ASTDeclaration::Kind::ADK_Function: {
            ASTFunctionDeclaration* d = static_cast<ASTFunctionDeclaration*>(decl);
            vec->push_back(std::move(ASTSymbolInfo(ASYK_Function, &d->name)));
            inExpression(vec, d->returnType.get());
            inExpression(vec, d->params.get());
            inExpression(vec, d->inits.get());
            inStatement(vec, d->body.get());
            break;
        }
        case ASTDeclaration::Kind::ADK_MemberGroup: {
            ASTMemberGroupDeclaration* d = static_cast<ASTMemberGroupDeclaration*>(decl);
            for (unsigned int i = 0, count = d->members.size(); i < count; i++)
                inExpression(vec, d->members[i]);
            break;
        }
        case ASTDeclaration::Kind::ADK_Namespace: {
            ASTNamespaceDeclaration* d = static_cast<ASTNamespaceDeclaration*>(decl);
            vec->push_back(std::move(ASTSymbolInfo(ASYK_Namespace, &d->name)));
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++)
                inDeclaration(vec, d->declarations[i]);
            break;
        }
        case ASTDeclaration::Kind::ADK_Superclass:
            break;
        case ASTDeclaration::Kind::ADK_Variable: {
            ASTVariableDeclaration* d = static_cast<ASTVariableDeclaration*>(decl);
            vec->push_back(std::move(ASTSymbolInfo(ASYK_Variable, &d->name)));
            break;
        }
        case ASTDeclaration::Kind::ADK_VarList: {
            ASTVarListDeclaration* d = static_cast<ASTVarListDeclaration*>(decl);
            inExpression(vec, d->variables.get());
            break;
        }
        default:
            break;
    }
}

void FindSymbolsQuery::inExpression(std::vector<ASTSymbolInfo>* vec, ASTExpression* expr) {
    if (!expr)
        return;

    switch (expr->getKind()) {
        case ASTExpression::Kind::AEK_Access: {
            ASTAccessExpression* e = static_cast<ASTAccessExpression*>(expr);
            inExpression(vec, e->variable.get());
            inExpression(vec, e->member.get());
            break;
        }
        case ASTExpression::Kind::AEK_Assignment: {
            ASTAssignmentExpression* e = static_cast<ASTAssignmentExpression*>(expr);
            inExpression(vec, e->left.get());
            inExpression(vec, e->right.get());
            break;
        }
        case ASTExpression::Kind::AEK_Backquote:
            break;
        case ASTExpression::Kind::AEK_Binary: {
            ASTBinaryExpression* e = static_cast<ASTBinaryExpression*>(expr);
            inExpression(vec, e->left.get());
            inExpression(vec, e->right.get());
            break;
        }
        case ASTExpression::Kind::AEK_Call: {
            ASTCallExpression* e = static_cast<ASTCallExpression*>(expr);
            inExpression(vec, e->target.get());
            inExpression(vec, e->args.get());
            break;
        }
        case ASTExpression::Kind::AEK_Case: {
            ASTCaseExpression* e = static_cast<ASTCaseExpression*>(expr);
            inExpression(vec, e->caseExpr.get());
            inStatement(vec, e->statements.get());
            break;
        }
        case ASTExpression::Kind::AEK_Cast: {
            ASTCastExpression* e = static_cast<ASTCastExpression*>(expr);
            inExpression(vec, e->obj.get());
            break;
        }
        case ASTExpression::Kind::AEK_Closure: {
            ASTClosureExpression* e = static_cast<ASTClosureExpression*>(expr);
            inDeclaration(vec, e->closure.get());
            break;
        }
        case ASTExpression::Kind::AEK_ConstrInit: {
            ASTConstrInitExpression* e = static_cast<ASTConstrInitExpression*>(expr);
            for (unsigned int i = 0, count = e->inits.size(); i < count; i++)
                inExpression(vec, e->inits[i]);
            break;
        }
        case ASTExpression::Kind::AEK_ContextMod: {
            ASTContextModExpression* e = static_cast<ASTContextModExpression*>(expr);
            inExpression(vec, e->expression.get());
            break;
        }
        case ASTExpression::Kind::AEK_ContextRow:
            break;
        case ASTExpression::Kind::AEK_Decl: {
            ASTDeclExpression* e = static_cast<ASTDeclExpression*>(expr);
            inDeclaration(vec, e->declaration.get());
            break;
        }
        case ASTExpression::Kind::AEK_Find: {
            ASTFindExpression* e = static_cast<ASTFindExpression*>(expr);
            inExpression(vec, e->result.get());
            inExpression(vec, e->data.get());
            inExpression(vec, e->where.get());
            break;
        }
        case ASTExpression::Kind::AEK_Hash: {
            ASTHashExpression* e = static_cast<ASTHashExpression*>(expr);
            for (unsigned int i = 0, count = e->elements.size(); i < count; i++)
                inExpression(vec, e->elements[i]);
            break;
        }
        case ASTExpression::Kind::AEK_HashElement: {
            ASTHashElementExpression* e = static_cast<ASTHashElementExpression*>(expr);
            inExpression(vec, e->key.get());
            inExpression(vec, e->value.get());
            break;
        }
        case ASTExpression::Kind::AEK_ImplicitArg:
        case ASTExpression::Kind::AEK_ImplicitElem:
            break;
        case ASTExpression::Kind::AEK_Index: {
            ASTIndexExpression* e = static_cast<ASTIndexExpression*>(expr);
            inExpression(vec, e->variable.get());
            inExpression(vec, e->index.get());
            break;
        }
        case ASTExpression::Kind::AEK_List: {
            ASTListExpression* e = static_cast<ASTListExpression*>(expr);
            for (unsigned int i = 0, count = e->elements.size(); i < count; i++)
                inExpression(vec, e->elements[i]);
            break;
        }
        case ASTExpression::Kind::AEK_Literal:
        case ASTExpression::Kind::AEK_Name:
        case ASTExpression::Kind::AEK_Regex:
        case ASTExpression::Kind::AEK_RegexSubst:
        case ASTExpression::Kind::AEK_RegexTrans:
            break;
        case ASTExpression::Kind::AEK_Returns: {
            ASTReturnsExpression* e = static_cast<ASTReturnsExpression*>(expr);
            inExpression(vec, e->typeName.get());
            break;
        }
        case ASTExpression::Kind::AEK_SwitchBody: {
            ASTSwitchBodyExpression* e = static_cast<ASTSwitchBodyExpression*>(expr);
            for (unsigned int i = 0, count = e->cases.size(); i < count; i++)
                inExpression(vec, e->cases[i]);
            break;
        }
        case ASTExpression::Kind::AEK_Ternary: {
            ASTTernaryExpression* e = static_cast<ASTTernaryExpression*>(expr);
            inExpression(vec, e->condition.get());
            inExpression(vec, e->exprTrue.get());
            inExpression(vec, e->exprFalse.get());
            break;
        }
        case ASTExpression::Kind::AEK_Unary: {
            ASTUnaryExpression* e = static_cast<ASTUnaryExpression*>(expr);
            inExpression(vec, e->expression.get());
            break;
        }
        default:
            break;
    }
}

void FindSymbolsQuery::inName(std::vector<ASTSymbolInfo>* vec, ASTName& name) {
    return;
}

void FindSymbolsQuery::inName(std::vector<ASTSymbolInfo>* vec, ASTName* name) {
    if (!name)
        return;
    return;
}

void FindSymbolsQuery::inStatement(std::vector<ASTSymbolInfo>* vec, ASTStatement* stmt) {
    if (!stmt)
        return;

    switch (stmt->getKind()) {
        case ASTStatement::Kind::ASK_Block: {
            ASTStatementBlock* s = static_cast<ASTStatementBlock*>(stmt);
            for (unsigned int i = 0, count = s->statements.size(); i < count; i++)
                inStatement(vec, s->statements[i]);
            break;
        }
        case ASTStatement::Kind::ASK_Break:
            break;
        case ASTStatement::Kind::ASK_Call: {
            ASTCallStatement* s = static_cast<ASTCallStatement*>(stmt);
            inExpression(vec, s->call.get());
            break;
        }
        case ASTStatement::Kind::ASK_Context: {
            ASTContextStatement* s = static_cast<ASTContextStatement*>(stmt);
            inExpression(vec, s->name.get());
            inExpression(vec, s->data.get());
            for (unsigned int i = 0, count = s->contextMods.size(); i < count; i++)
                inExpression(vec, s->contextMods[i]);
            inStatement(vec, s->statements.get());
            break;
        }
        case ASTStatement::Kind::ASK_Continue:
            break;
        case ASTStatement::Kind::ASK_DoWhile: {
            ASTDoWhileStatement* s = static_cast<ASTDoWhileStatement*>(stmt);
            inExpression(vec, s->condition.get());
            inStatement(vec, s->statement.get());
            break;
        }
        case ASTStatement::Kind::ASK_Expression: {
            ASTExpressionStatement* s = static_cast<ASTExpressionStatement*>(stmt);
            inExpression(vec, s->expression.get());
            break;
        }
        case ASTStatement::Kind::ASK_For: {
            ASTForStatement* s = static_cast<ASTForStatement*>(stmt);
            inExpression(vec, s->init.get());
            inExpression(vec, s->condition.get());
            inExpression(vec, s->iteration.get());
            inStatement(vec, s->statement.get());
            break;
        }
        case ASTStatement::Kind::ASK_Foreach: {
            ASTForeachStatement* s = static_cast<ASTForeachStatement*>(stmt);
            inExpression(vec, s->value.get());
            inExpression(vec, s->source.get());
            inStatement(vec, s->statement.get());
            break;
        }
        case ASTStatement::Kind::ASK_If: {
            ASTIfStatement* s = static_cast<ASTIfStatement*>(stmt);
            inExpression(vec, s->condition.get());
            inStatement(vec, s->stmtThen.get());
            inStatement(vec, s->stmtElse.get());
            break;
        }
        case ASTStatement::Kind::ASK_OnBlockExit: {
            ASTOnBlockExitStatement* s = static_cast<ASTOnBlockExitStatement*>(stmt);
            inStatement(vec, s->statement.get());
            break;
        }
        case ASTStatement::Kind::ASK_Rethrow:
            break;
        case ASTStatement::Kind::ASK_Return: {
            ASTReturnStatement* s = static_cast<ASTReturnStatement*>(stmt);
            inExpression(vec, s->retval.get());
            break;
        }
        case ASTStatement::Kind::ASK_Summarize: {
            ASTSummarizeStatement* s = static_cast<ASTSummarizeStatement*>(stmt);
            inExpression(vec, s->name.get());
            inExpression(vec, s->data.get());
            inExpression(vec, s->by.get());
            for (unsigned int i = 0, count = s->contextMods.size(); i < count; i++)
                inExpression(vec, s->contextMods[i]);
            inStatement(vec, s->statements.get());
            break;
        }
        case ASTStatement::Kind::ASK_Switch: {
            ASTSwitchStatement* s = static_cast<ASTSwitchStatement*>(stmt);
            inExpression(vec, s->variable.get());
            inExpression(vec, s->body.get());
            break;
        }
        case ASTStatement::Kind::ASK_ThreadExit:
            break;
        case ASTStatement::Kind::ASK_Throw: {
            ASTThrowStatement* s = static_cast<ASTThrowStatement*>(stmt);
            inExpression(vec, s->expression.get());
            break;
        }
        case ASTStatement::Kind::ASK_Try: {
            ASTTryStatement* s = static_cast<ASTTryStatement*>(stmt);
            inStatement(vec, s->tryStmt.get());
            inExpression(vec, s->catchVar.get());
            inStatement(vec, s->catchStmt.get());
            break;
        }
        case ASTStatement::Kind::ASK_While: {
            ASTWhileStatement* s = static_cast<ASTWhileStatement*>(stmt);
            inExpression(vec, s->condition.get());
            inStatement(vec, s->statement.get());
            break;
        }
        default:
            break;
    }
}

void FindSymbolsQuery::fixClassInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames) {
    if (bareNames)
        return;

    size_t nextNode = 0;
    size_t nodeCount = nodes->size();
    for (; nextNode < nodeCount; nextNode++) {
        ASTNode* node = nodes->at(nextNode);
        if (node->getNodeType() != ANT_Declaration)
            continue;
        ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
        if (decl->getKind() == ASTDeclaration::Kind::ADK_Class) {
            nextNode++;
            break;
        }
    }

    for (; nextNode < nodeCount; nextNode++) {
        ASTNode* node = nodes->at(nextNode);
        if (node->getNodeType() == ANT_Declaration) {
            ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
            if (decl->getKind() == ASTDeclaration::Kind::ADK_Class)
                si.name.insert(0, static_cast<ASTClassDeclaration*>(decl)->name.name + "::");
            else if (decl->getKind() == ASTDeclaration::Kind::ADK_Namespace)
                si.name.insert(0, static_cast<ASTNamespaceDeclaration*>(decl)->name.name + "::");
        }
    }
}

void FindSymbolsQuery::fixConstantInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames) {
    if (bareNames)
        return;

    size_t nextNode = 0;
    size_t nodeCount = nodes->size();
    for (; nextNode < nodeCount; nextNode++) {
        ASTNode* node = nodes->at(nextNode);
        if (node->getNodeType() != ANT_Declaration)
            continue;
        ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
        if (decl->getKind() == ASTDeclaration::Kind::ADK_Constant) {
            nextNode++;
            break;
        }
    }

    for (; nextNode < nodeCount; nextNode++) {
        ASTNode* node = nodes->at(nextNode);
        if (node->getNodeType() == ANT_Declaration) {
            ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
            if (decl->getKind() == ASTDeclaration::Kind::ADK_Function)
                break;
            else if (decl->getKind() == ASTDeclaration::Kind::ADK_Class)
                si.name.insert(0, static_cast<ASTClassDeclaration*>(decl)->name.name + "::");
            else if (decl->getKind() == ASTDeclaration::Kind::ADK_Namespace)
                si.name.insert(0, static_cast<ASTNamespaceDeclaration*>(decl)->name.name + "::");
        }
    }
}

void FindSymbolsQuery::fixFunctionInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames) {
    size_t nextNode = 0;
    size_t nodeCount = nodes->size();
    for (; nextNode < nodeCount; nextNode++) {
        ASTNode* node = nodes->at(nextNode);
        if (node->getNodeType() != ANT_Declaration)
            continue;
        ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
        if (decl->getKind() == ASTDeclaration::Kind::ADK_Function) {
            nextNode++;
            break;
        }
    }

    for (; nextNode < nodeCount; nextNode++) {
        ASTNode* node = nodes->at(nextNode);
        if (node->getNodeType() == ANT_Declaration) {
            ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
            if (decl->getKind() == ASTDeclaration::Kind::ADK_Class) {
                si.kind = (si.name == "constructor") ? ASYK_Constructor : ASYK_Method;
                if (!bareNames)
                    si.name.insert(0, static_cast<ASTClassDeclaration*>(decl)->name.name + "::");
            }
            else if (decl->getKind() == ASTDeclaration::Kind::ADK_Namespace) {
                if (!bareNames)
                    si.name.insert(0, static_cast<ASTNamespaceDeclaration*>(decl)->name.name + "::");
            }
        }
    }
}

void FindSymbolsQuery::fixVariableInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames) {
    if (bareNames)
        return;

    size_t nextNode = 0;
    size_t nodeCount = nodes->size();
    for (; nextNode < nodeCount; nextNode++) {
        ASTNode* node = nodes->at(nextNode);
        if (node->getNodeType() != ANT_Declaration)
            continue;
        ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
        if (decl->getKind() == ASTDeclaration::Kind::ADK_Variable) {
            nextNode++;
            break;
        }
    }

    for (; nextNode < nodeCount; nextNode++) {
        ASTNode* node = nodes->at(nextNode);
        if (node->getNodeType() == ANT_Declaration) {
            ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
            if (decl->getKind() == ASTDeclaration::Kind::ADK_Function)
                break;
            else if (decl->getKind() == ASTDeclaration::Kind::ADK_Class)
                si.name.insert(0, static_cast<ASTClassDeclaration*>(decl)->name.name + "::");
            else if (decl->getKind() == ASTDeclaration::Kind::ADK_Namespace)
                si.name.insert(0, static_cast<ASTNamespaceDeclaration*>(decl)->name.name + "::");
        }
    }
}

void FindSymbolsQuery::fixSymbolInfos(ASTTree* tree, std::vector<ASTSymbolInfo>* vec, bool bareNames) {
    for (unsigned int i = 0, count = vec->size(); i < count; i++) {
        ASTSymbolInfo& si = vec->at(i);

        // Do this only for classes, constants, functions and variables.
        if (si.kind != ASYK_Function && si.kind != ASYK_Variable && si.kind != ASYK_Class && si.kind != ASYK_Constant)
            continue;

        // Get symbol's location.
        ASTParseLocation& loc = si.loc;

        // Target node is first, all the parents follow.
        std::unique_ptr<std::vector<ASTNode*> > nodes(FindNodeAndParentsQuery::find(tree, loc.firstLine, loc.firstCol));
        if (!nodes)
            continue;

        // Fix the symbol info.
        switch (si.kind) {
            case ASYK_Class:
                fixClassInfo(si, nodes.get(), bareNames); break;
            case ASYK_Constant:
                fixConstantInfo(si, nodes.get(), bareNames); break;
            case ASYK_Function:
                fixFunctionInfo(si, nodes.get(), bareNames); break;
            case ASYK_Variable:
                fixVariableInfo(si, nodes.get(), bareNames); break;
            default:
                break;
        }
    }
}

std::vector<ASTSymbolInfo>* FindSymbolsQuery::find(ASTTree* tree, bool bareNames) {
    if (!tree)
        return nullptr;

    std::unique_ptr<std::vector<ASTSymbolInfo> > vec(new std::vector<ASTSymbolInfo>);
    if (!vec)
        return nullptr;
    vec->reserve(64);
    for (unsigned int i = 0, count = tree->nodes.size(); i < count; i++) {
        ASTNode* node = tree->nodes[i];
        switch (node->getNodeType()) {
            case ANT_Declaration: {
                ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
                inDeclaration(vec.get(), decl);
                break;
            }
            case ANT_Expression: {
                ASTExpression* expr = static_cast<ASTExpression*>(node);
                inExpression(vec.get(), expr);
                break;
            }
            case ANT_Name: {
                ASTName* n = static_cast<ASTName*>(node);
                inName(vec.get(), n);
                break;
            }
            case ANT_ParseOption:
                break;
            case ANT_Statement: {
                ASTStatement* stmt = static_cast<ASTStatement*>(node);
                inStatement(vec.get(), stmt);
                break;
            }
            case ANT_None:
            default:
                break;
        }
    }

    fixSymbolInfos(tree, vec.get(), bareNames);
    return vec.release();
}
