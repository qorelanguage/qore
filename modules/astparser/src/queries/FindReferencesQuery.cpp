/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  FindReferencesQuery.cpp

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

#include "queries/FindReferencesQuery.h"

#include <memory>

#include "ast/AST.h"

void FindReferencesQuery::inDeclaration(std::vector<ASTNode*>* vec, ASTDeclaration* decl, const std::string& name) {
    if (!decl)
        return;

    switch (decl->getKind()) {
        case ASTDeclaration::Kind::ADK_Class: {
            ASTClassDeclaration* d = static_cast<ASTClassDeclaration*>(decl);
            inName(vec, d->name, name);
            for (unsigned int i = 0, count = d->inherits.size(); i < count; i++)
                inDeclaration(vec, d->inherits[i], name);
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++)
                inDeclaration(vec, d->declarations[i], name);
            break;
        }
        case ASTDeclaration::Kind::ADK_Closure: {
            ASTClosureDeclaration* d = static_cast<ASTClosureDeclaration*>(decl);
            inExpression(vec, d->returnType.get(), name);
            inExpression(vec, d->params.get(), name);
            inStatement(vec, d->body.get(), name);
            break;
        }
        case ASTDeclaration::Kind::ADK_Constant: {
            ASTConstantDeclaration* d = static_cast<ASTConstantDeclaration*>(decl);
            inName(vec, d->name, name);
            inExpression(vec, d->value.get(), name);
            break;
        }
        case ASTDeclaration::Kind::ADK_Function: {
            ASTFunctionDeclaration* d = static_cast<ASTFunctionDeclaration*>(decl);
            inName(vec, d->name, name);
            inExpression(vec, d->returnType.get(), name);
            inExpression(vec, d->params.get(), name);
            inExpression(vec, d->inits.get(), name);
            inStatement(vec, d->body.get(), name);
            break;
        }
        case ASTDeclaration::Kind::ADK_MemberGroup: {
            ASTMemberGroupDeclaration* d = static_cast<ASTMemberGroupDeclaration*>(decl);
            for (unsigned int i = 0, count = d->members.size(); i < count; i++)
                inExpression(vec, d->members[i], name);
            break;
        }
        case ASTDeclaration::Kind::ADK_Namespace: {
            ASTNamespaceDeclaration* d = static_cast<ASTNamespaceDeclaration*>(decl);
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++)
                inDeclaration(vec, d->declarations[i], name);
            break;
        }
        case ASTDeclaration::Kind::ADK_Superclass: {
            ASTSuperclassDeclaration* d = static_cast<ASTSuperclassDeclaration*>(decl);
            inName(vec, d->name, name);
            break;
        }
        case ASTDeclaration::Kind::ADK_Variable: {
            ASTVariableDeclaration* d = static_cast<ASTVariableDeclaration*>(decl);
            inName(vec, d->typeName, name);
            inName(vec, d->name, name);
            break;
        }
        case ASTDeclaration::Kind::ADK_VarList: {
            ASTVarListDeclaration* d = static_cast<ASTVarListDeclaration*>(decl);
            inExpression(vec, d->variables.get(), name);
            break;
        }
        default:
            break;
    }
}

void FindReferencesQuery::inExpression(std::vector<ASTNode*>* vec, ASTExpression* expr, const std::string& name) {
    if (!expr)
        return;

    switch (expr->getKind()) {
        case ASTExpression::Kind::AEK_Access: {
            ASTAccessExpression* e = static_cast<ASTAccessExpression*>(expr);
            inExpression(vec, e->variable.get(), name);
            inExpression(vec, e->member.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_Assignment: {
            ASTAssignmentExpression* e = static_cast<ASTAssignmentExpression*>(expr);
            inExpression(vec, e->left.get(), name);
            inExpression(vec, e->right.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_Backquote:
            break;
        case ASTExpression::Kind::AEK_Binary: {
            ASTBinaryExpression* e = static_cast<ASTBinaryExpression*>(expr);
            inExpression(vec, e->left.get(), name);
            inExpression(vec, e->right.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_Call: {
            ASTCallExpression* e = static_cast<ASTCallExpression*>(expr);
            inExpression(vec, e->target.get(), name);
            inExpression(vec, e->args.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_Case: {
            ASTCaseExpression* e = static_cast<ASTCaseExpression*>(expr);
            inExpression(vec, e->caseExpr.get(), name);
            inStatement(vec, e->statements.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_Cast: {
            ASTCastExpression* e = static_cast<ASTCastExpression*>(expr);
            inName(vec, e->castType, name);
            inExpression(vec, e->obj.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_Closure: {
            ASTClosureExpression* e = static_cast<ASTClosureExpression*>(expr);
            inDeclaration(vec, e->closure.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_ConstrInit: {
            ASTConstrInitExpression* e = static_cast<ASTConstrInitExpression*>(expr);
            for (unsigned int i = 0, count = e->inits.size(); i < count; i++)
                inExpression(vec, e->inits[i], name);
            break;
        }
        case ASTExpression::Kind::AEK_ContextMod: {
            ASTContextModExpression* e = static_cast<ASTContextModExpression*>(expr);
            inExpression(vec, e->expression.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_ContextRow:
            break;
        case ASTExpression::Kind::AEK_Decl: {
            ASTDeclExpression* e = static_cast<ASTDeclExpression*>(expr);
            inDeclaration(vec, e->declaration.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_Find: {
            ASTFindExpression* e = static_cast<ASTFindExpression*>(expr);
            inExpression(vec, e->result.get(), name);
            inExpression(vec, e->data.get(), name);
            inExpression(vec, e->where.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_Hash: {
            ASTHashExpression* e = static_cast<ASTHashExpression*>(expr);
            for (unsigned int i = 0, count = e->elements.size(); i < count; i++)
                inExpression(vec, e->elements[i], name);
            break;
        }
        case ASTExpression::Kind::AEK_HashElement: {
            ASTHashElementExpression* e = static_cast<ASTHashElementExpression*>(expr);
            inExpression(vec, e->key.get(), name);
            inExpression(vec, e->value.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_ImplicitArg:
        case ASTExpression::Kind::AEK_ImplicitElem:
            break;
        case ASTExpression::Kind::AEK_Index: {
            ASTIndexExpression* e = static_cast<ASTIndexExpression*>(expr);
            inExpression(vec, e->variable.get(), name);
            inExpression(vec, e->index.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_List: {
            ASTListExpression* e = static_cast<ASTListExpression*>(expr);
            for (unsigned int i = 0, count = e->elements.size(); i < count; i++)
                inExpression(vec, e->elements[i], name);
            break;
        }
        case ASTExpression::Kind::AEK_Literal:
            break;
        case ASTExpression::Kind::AEK_Name: {
            ASTNameExpression* e = static_cast<ASTNameExpression*>(expr);
            inName(vec, e->name, name);
            break;
        }
        case ASTExpression::Kind::AEK_Regex:
        case ASTExpression::Kind::AEK_RegexSubst:
        case ASTExpression::Kind::AEK_RegexTrans:
            break;
        case ASTExpression::Kind::AEK_Returns: {
            ASTReturnsExpression* e = static_cast<ASTReturnsExpression*>(expr);
            inExpression(vec, e->typeName.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_SwitchBody: {
            ASTSwitchBodyExpression* e = static_cast<ASTSwitchBodyExpression*>(expr);
            for (unsigned int i = 0, count = e->cases.size(); i < count; i++)
                inExpression(vec, e->cases[i], name);
            break;
        }
        case ASTExpression::Kind::AEK_Ternary: {
            ASTTernaryExpression* e = static_cast<ASTTernaryExpression*>(expr);
            inExpression(vec, e->condition.get(), name);
            inExpression(vec, e->exprTrue.get(), name);
            inExpression(vec, e->exprFalse.get(), name);
            break;
        }
        case ASTExpression::Kind::AEK_Unary: {
            ASTUnaryExpression* e = static_cast<ASTUnaryExpression*>(expr);
            inExpression(vec, e->expression.get(), name);
            break;
        }
        default:
            break;
    }
}

void FindReferencesQuery::inName(std::vector<ASTNode*>* vec, ASTName& n, const std::string& name) {
    if (n.name == name)
        vec->push_back(&n);
}

void FindReferencesQuery::inName(std::vector<ASTNode*>* vec, ASTName* n, const std::string& name) {
    if (!n)
        return;
    if (n->name == name)
        vec->push_back(n);
}

void FindReferencesQuery::inStatement(std::vector<ASTNode*>* vec, ASTStatement* stmt, const std::string& name) {
    if (!stmt)
        return;

    switch (stmt->getKind()) {
        case ASTStatement::Kind::ASK_Block: {
            ASTStatementBlock* s = static_cast<ASTStatementBlock*>(stmt);
            for (unsigned int i = 0, count = s->statements.size(); i < count; i++)
                inStatement(vec, s->statements[i], name);
            break;
        }
        case ASTStatement::Kind::ASK_Break:
            break;
        case ASTStatement::Kind::ASK_Call: {
            ASTCallStatement* s = static_cast<ASTCallStatement*>(stmt);
            inExpression(vec, s->call.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_Context: {
            ASTContextStatement* s = static_cast<ASTContextStatement*>(stmt);
            inExpression(vec, s->name.get(), name);
            inExpression(vec, s->data.get(), name);
            for (unsigned int i = 0, count = s->contextMods.size(); i < count; i++)
                inExpression(vec, s->contextMods[i], name);
            inStatement(vec, s->statements.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_Continue:
            break;
        case ASTStatement::Kind::ASK_DoWhile: {
            ASTDoWhileStatement* s = static_cast<ASTDoWhileStatement*>(stmt);
            inExpression(vec, s->condition.get(), name);
            inStatement(vec, s->statement.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_Expression: {
            ASTExpressionStatement* s = static_cast<ASTExpressionStatement*>(stmt);
            inExpression(vec, s->expression.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_For: {
            ASTForStatement* s = static_cast<ASTForStatement*>(stmt);
            inExpression(vec, s->init.get(), name);
            inExpression(vec, s->condition.get(), name);
            inExpression(vec, s->iteration.get(), name);
            inStatement(vec, s->statement.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_Foreach: {
            ASTForeachStatement* s = static_cast<ASTForeachStatement*>(stmt);
            inExpression(vec, s->value.get(), name);
            inExpression(vec, s->source.get(), name);
            inStatement(vec, s->statement.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_If: {
            ASTIfStatement* s = static_cast<ASTIfStatement*>(stmt);
            inExpression(vec, s->condition.get(), name);
            inStatement(vec, s->stmtThen.get(), name);
            inStatement(vec, s->stmtElse.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_OnBlockExit: {
            ASTOnBlockExitStatement* s = static_cast<ASTOnBlockExitStatement*>(stmt);
            inStatement(vec, s->statement.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_Rethrow:
            break;
        case ASTStatement::Kind::ASK_Return: {
            ASTReturnStatement* s = static_cast<ASTReturnStatement*>(stmt);
            inExpression(vec, s->retval.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_Summarize: {
            ASTSummarizeStatement* s = static_cast<ASTSummarizeStatement*>(stmt);
            inExpression(vec, s->name.get(), name);
            inExpression(vec, s->data.get(), name);
            inExpression(vec, s->by.get(), name);
            for (unsigned int i = 0, count = s->contextMods.size(); i < count; i++)
                inExpression(vec, s->contextMods[i], name);
            inStatement(vec, s->statements.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_Switch: {
            ASTSwitchStatement* s = static_cast<ASTSwitchStatement*>(stmt);
            inExpression(vec, s->variable.get(), name);
            inExpression(vec, s->body.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_ThreadExit:
            break;
        case ASTStatement::Kind::ASK_Throw: {
            ASTThrowStatement* s = static_cast<ASTThrowStatement*>(stmt);
            inExpression(vec, s->expression.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_Try: {
            ASTTryStatement* s = static_cast<ASTTryStatement*>(stmt);
            inStatement(vec, s->tryStmt.get(), name);
            inExpression(vec, s->catchVar.get(), name);
            inStatement(vec, s->catchStmt.get(), name);
            break;
        }
        case ASTStatement::Kind::ASK_While: {
            ASTWhileStatement* s = static_cast<ASTWhileStatement*>(stmt);
            inExpression(vec, s->condition.get(), name);
            inStatement(vec, s->statement.get(), name);
            break;
        }
        default:
            break;
    }
}

std::vector<ASTNode*>* FindReferencesQuery::find(ASTTree* tree, const std::string& name) {
    if (!tree)
        return nullptr;

    std::unique_ptr<std::vector<ASTNode*> > vec(new std::vector<ASTNode*>);
    for (unsigned int i = 0, count = tree->nodes.size(); i < count; i++) {
        ASTNode* node = tree->nodes[i];
        switch (node->getNodeType()) {
            case ANT_Declaration: {
                ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
                inDeclaration(vec.get(), decl, name);
                break;
            }
            case ANT_Expression: {
                ASTExpression* expr = static_cast<ASTExpression*>(node);
                inExpression(vec.get(), expr, name);
                break;
            }
            case ANT_Name: {
                ASTName* n = static_cast<ASTName*>(node);
                inName(vec.get(), n, name);
                break;
            }
            case ANT_ParseOption:
                break;
            case ANT_Statement: {
                ASTStatement* stmt = static_cast<ASTStatement*>(node);
                inStatement(vec.get(), stmt, name);
                break;
            }
            case ANT_None:
            default:
                break;
        }
    }
    return vec.release();
}
