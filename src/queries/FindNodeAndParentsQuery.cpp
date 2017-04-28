/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AstTreePrinter.cpp

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

#include "queries/FindNodeAndParentsQuery.h"

#include "ast/AST.h"

std::vector<ASTNode*>* prepareResultVector() {
    std::vector<ASTNode*>* result = new std::vector<ASTNode*>;
    result->reserve(8);
    return result;
}

std::vector<ASTNode*>* FindNodeAndParentsQuery::findNodeAndParentsInDecl(ASTDeclaration* decl, ast_loc_t line, ast_loc_t col) {
    if (!decl)
        return nullptr;
    if (!decl->loc.inside(line, col))
        return nullptr;

    std::vector<ASTNode*>* result = nullptr;
    switch (decl->getKind()) {
        case ASTDeclaration::Kind::ADK_Class: {
            ASTClassDeclaration* d = static_cast<ASTClassDeclaration*>(decl);
            result = findNodeAndParentsInName(d->name, line, col);
            if (result) { result->push_back(decl); return result; }
            for (unsigned int i = 0, count = d->inherits.size(); i < count; i++) {
                result = findNodeAndParentsInDecl(d->inherits[i], line, col);
                if (result) { result->push_back(decl); return result; }
            }
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++) {
                result = findNodeAndParentsInDecl(d->declarations[i], line, col);
                if (result) { result->push_back(decl); return result; }
            }
            break;
        }
        case ASTDeclaration::Kind::ADK_Closure: {
            ASTClosureDeclaration* d = static_cast<ASTClosureDeclaration*>(decl);
            result = findNodeAndParentsInExpr(d->returnType.get(), line, col);
            if (result) { result->push_back(decl); return result; }
            result = findNodeAndParentsInExpr(d->params.get(), line, col);
            if (result) { result->push_back(decl); return result; }
            result = findNodeAndParentsInStmt(d->body.get(), line, col);
            if (result) { result->push_back(decl); return result; }
            break;
        }
        case ASTDeclaration::Kind::ADK_Constant: {
            ASTConstantDeclaration* d = static_cast<ASTConstantDeclaration*>(decl);
            result = findNodeAndParentsInName(d->name, line, col);
            if (result) { result->push_back(decl); return result; }
            result = findNodeAndParentsInExpr(d->value.get(), line, col);
            if (result) { result->push_back(decl); return result; }
            break;
        }
        case ASTDeclaration::Kind::ADK_Function: {
            ASTFunctionDeclaration* d = static_cast<ASTFunctionDeclaration*>(decl);
            result = findNodeAndParentsInName(d->name, line, col);
            if (result) { result->push_back(decl); return result; }
            result = findNodeAndParentsInExpr(d->returnType.get(), line, col);
            if (result) { result->push_back(decl); return result; }
            result = findNodeAndParentsInExpr(d->params.get(), line, col);
            if (result) { result->push_back(decl); return result; }
            result = findNodeAndParentsInExpr(d->inits.get(), line, col);
            if (result) { result->push_back(decl); return result; }
            result = findNodeAndParentsInStmt(d->body.get(), line, col);
            if (result) { result->push_back(decl); return result; }
            break;
        }
        case ASTDeclaration::Kind::ADK_MemberGroup: {
            ASTMemberGroupDeclaration* d = static_cast<ASTMemberGroupDeclaration*>(decl);
            for (unsigned int i = 0, count = d->members.size(); i < count; i++) {
                result = findNodeAndParentsInExpr(d->members[i], line, col);
                if (result) { result->push_back(decl); return result; }
            }
            break;
        }
        case ASTDeclaration::Kind::ADK_Namespace: {
            ASTNamespaceDeclaration* d = static_cast<ASTNamespaceDeclaration*>(decl);
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++) {
                result = findNodeAndParentsInDecl(d->declarations[i], line, col);
                if (result) { result->push_back(decl); return result; }
            }
            break;
        }
        case ASTDeclaration::Kind::ADK_Superclass: {
            ASTSuperclassDeclaration* d = static_cast<ASTSuperclassDeclaration*>(decl);
            result = findNodeAndParentsInName(d->name, line, col);
            if (result) { result->push_back(decl); return result; }
            break;
        }
        case ASTDeclaration::Kind::ADK_Variable: {
            ASTVariableDeclaration* d = static_cast<ASTVariableDeclaration*>(decl);
            result = findNodeAndParentsInName(d->typeName, line, col);
            if (result) { result->push_back(decl); return result; }
            result = findNodeAndParentsInName(d->name, line, col);
            if (result) { result->push_back(decl); return result; }
            break;
        }
        case ASTDeclaration::Kind::ADK_VarList: {
            ASTVarListDeclaration* d = static_cast<ASTVarListDeclaration*>(decl);
            result = findNodeAndParentsInExpr(d->variables.get(), line, col);
            if (result) { result->push_back(decl); return result; }
            break;
        }
        default:
            break;
    }
    result = prepareResultVector();
    result->push_back(decl);
    return result;
}

std::vector<ASTNode*>* FindNodeAndParentsQuery::findNodeAndParentsInExpr(ASTExpression* expr, ast_loc_t line, ast_loc_t col) {
    if (!expr)
        return nullptr;
    if (!expr->loc.inside(line, col))
        return nullptr;

    std::vector<ASTNode*>* result = nullptr;
    switch (expr->getKind()) {
        case ASTExpression::Kind::AEK_Access: {
            ASTAccessExpression* e = static_cast<ASTAccessExpression*>(expr);
            result = findNodeAndParentsInExpr(e->variable.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            result = findNodeAndParentsInExpr(e->member.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_Assignment: {
            ASTAssignmentExpression* e = static_cast<ASTAssignmentExpression*>(expr);
            result = findNodeAndParentsInExpr(e->left.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            result = findNodeAndParentsInExpr(e->right.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_Backquote:
            break;
        case ASTExpression::Kind::AEK_Binary: {
            ASTBinaryExpression* e = static_cast<ASTBinaryExpression*>(expr);
            result = findNodeAndParentsInExpr(e->left.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            result = findNodeAndParentsInExpr(e->right.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_Call: {
            ASTCallExpression* e = static_cast<ASTCallExpression*>(expr);
            result = findNodeAndParentsInExpr(e->target.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            result = findNodeAndParentsInExpr(e->args.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_Case: {
            ASTCaseExpression* e = static_cast<ASTCaseExpression*>(expr);
            result = findNodeAndParentsInExpr(e->caseExpr.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            result = findNodeAndParentsInStmt(e->statements.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_Cast: {
            ASTCastExpression* e = static_cast<ASTCastExpression*>(expr);
            result = findNodeAndParentsInName(e->castType, line, col);
            if (result) { result->push_back(expr); return result; }
            result = findNodeAndParentsInExpr(e->obj.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_Closure: {
            ASTClosureExpression* e = static_cast<ASTClosureExpression*>(expr);
            result = findNodeAndParentsInDecl(e->closure.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_ConstrInit: {
            ASTConstrInitExpression* e = static_cast<ASTConstrInitExpression*>(expr);
            for (unsigned int i = 0, count = e->inits.size(); i < count; i++) {
                result = findNodeAndParentsInExpr(e->inits[i], line, col);
                if (result) { result->push_back(expr); return result; }
            }
            break;
        }
        case ASTExpression::Kind::AEK_ContextMod: {
            ASTContextModExpression* e = static_cast<ASTContextModExpression*>(expr);
            result = findNodeAndParentsInExpr(e->expression.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_ContextRow:
            break;
        case ASTExpression::Kind::AEK_Decl: {
            ASTDeclExpression* e = static_cast<ASTDeclExpression*>(expr);
            result = findNodeAndParentsInDecl(e->declaration.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_Find: {
            ASTFindExpression* e = static_cast<ASTFindExpression*>(expr);
            result = findNodeAndParentsInExpr(e->result.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            result = findNodeAndParentsInExpr(e->data.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            result = findNodeAndParentsInExpr(e->where.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_Hash: {
            ASTHashExpression* e = static_cast<ASTHashExpression*>(expr);
            for (unsigned int i = 0, count = e->elements.size(); i < count; i++) {
                result = findNodeAndParentsInExpr(e->elements[i], line, col);
                if (result) { result->push_back(expr); return result; }
            }
            break;
        }
        case ASTExpression::Kind::AEK_HashElement: {
            ASTHashElementExpression* e = static_cast<ASTHashElementExpression*>(expr);
            result = findNodeAndParentsInExpr(e->key.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            result = findNodeAndParentsInExpr(e->value.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_ImplicitArg:
        case ASTExpression::Kind::AEK_ImplicitElem:
            break;
        case ASTExpression::Kind::AEK_Index: {
            ASTIndexExpression* e = static_cast<ASTIndexExpression*>(expr);
            result = findNodeAndParentsInExpr(e->variable.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            result = findNodeAndParentsInExpr(e->index.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_List: {
            ASTListExpression* e = static_cast<ASTListExpression*>(expr);
            for (unsigned int i = 0, count = e->elements.size(); i < count; i++) {
                result = findNodeAndParentsInExpr(e->elements[i], line, col);
                if (result) { result->push_back(expr); return result; }
            }
            break;
        }
        case ASTExpression::Kind::AEK_Literal:
            break;
        case ASTExpression::Kind::AEK_Name: {
            ASTNameExpression* e = static_cast<ASTNameExpression*>(expr);
            result = findNodeAndParentsInName(e->name, line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_Regex:
        case ASTExpression::Kind::AEK_RegexSubst:
        case ASTExpression::Kind::AEK_RegexTrans:
            break;
        case ASTExpression::Kind::AEK_Returns: {
            ASTReturnsExpression* e = static_cast<ASTReturnsExpression*>(expr);
            result = findNodeAndParentsInExpr(e->typeName.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_SwitchBody: {
            ASTSwitchBodyExpression* e = static_cast<ASTSwitchBodyExpression*>(expr);
            for (unsigned int i = 0, count = e->cases.size(); i < count; i++) {
                result = findNodeAndParentsInExpr(e->cases[i], line, col);
                if (result) { result->push_back(expr); return result; }
            }
            break;
        }
        case ASTExpression::Kind::AEK_Ternary: {
            ASTTernaryExpression* e = static_cast<ASTTernaryExpression*>(expr);
            result = findNodeAndParentsInExpr(e->condition.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            result = findNodeAndParentsInExpr(e->exprTrue.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            result = findNodeAndParentsInExpr(e->exprFalse.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        case ASTExpression::Kind::AEK_Unary: {
            ASTUnaryExpression* e = static_cast<ASTUnaryExpression*>(expr);
            result = findNodeAndParentsInExpr(e->expression.get(), line, col);
            if (result) { result->push_back(expr); return result; }
            break;
        }
        default:
            break;
    }
    result = prepareResultVector();
    result->push_back(expr);
    return result;
}

std::vector<ASTNode*>* FindNodeAndParentsQuery::findNodeAndParentsInName(ASTName& name, ast_loc_t line, ast_loc_t col) {
    if (!name.loc.inside(line, col))
        return nullptr;
    std::vector<ASTNode*>* result = prepareResultVector();
    result->push_back(&name);
    return result;
}

std::vector<ASTNode*>* FindNodeAndParentsQuery::findNodeAndParentsInName(ASTName* name, ast_loc_t line, ast_loc_t col) {
    if (!name)
        return nullptr;
    if (!name->loc.inside(line, col))
        return nullptr;
    std::vector<ASTNode*>* result = prepareResultVector();
    result->push_back(name);
    return result;
}

std::vector<ASTNode*>* FindNodeAndParentsQuery::findNodeAndParentsInPO(ASTParseOption* po, ast_loc_t line, ast_loc_t col) {
    if (!po)
        return nullptr;
    if (!po->loc.inside(line, col))
        return nullptr;
    std::vector<ASTNode*>* result = prepareResultVector();
    result->push_back(po);
    return result;
}

std::vector<ASTNode*>* FindNodeAndParentsQuery::findNodeAndParentsInStmt(ASTStatement* stmt, ast_loc_t line, ast_loc_t col) {
    if (!stmt)
        return nullptr;
    if (!stmt->loc.inside(line, col))
        return nullptr;

    std::vector<ASTNode*>* result = nullptr;
    switch (stmt->getKind()) {
        case ASTStatement::Kind::ASK_Block: {
            ASTStatementBlock* s = static_cast<ASTStatementBlock*>(stmt);
            for (unsigned int i = 0, count = s->statements.size(); i < count; i++) {
                result = findNodeAndParentsInStmt(s->statements[i], line, col);
                if (result) { result->push_back(stmt); return result; }
            }
            break;
        }
        case ASTStatement::Kind::ASK_Break:
            break;
        case ASTStatement::Kind::ASK_Call: {
            ASTCallStatement* s = static_cast<ASTCallStatement*>(stmt);
            result = findNodeAndParentsInExpr(s->call.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_Context: {
            ASTContextStatement* s = static_cast<ASTContextStatement*>(stmt);
            result = findNodeAndParentsInExpr(s->name.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInExpr(s->data.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            for (unsigned int i = 0, count = s->contextMods.size(); i < count; i++) {
                result = findNodeAndParentsInExpr(s->contextMods[i], line, col);
                if (result) { result->push_back(stmt); return result; }
            }
            result = findNodeAndParentsInStmt(s->statements.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_Continue:
            break;
        case ASTStatement::Kind::ASK_DoWhile: {
            ASTDoWhileStatement* s = static_cast<ASTDoWhileStatement*>(stmt);
            result = findNodeAndParentsInExpr(s->condition.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInStmt(s->statement.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_Expression: {
            ASTExpressionStatement* s = static_cast<ASTExpressionStatement*>(stmt);
            result = findNodeAndParentsInExpr(s->expression.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_For: {
            ASTForStatement* s = static_cast<ASTForStatement*>(stmt);
            result = findNodeAndParentsInExpr(s->init.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInExpr(s->condition.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInExpr(s->iteration.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInStmt(s->statement.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_Foreach: {
            ASTForeachStatement* s = static_cast<ASTForeachStatement*>(stmt);
            result = findNodeAndParentsInExpr(s->value.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInExpr(s->source.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInStmt(s->statement.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_If: {
            ASTIfStatement* s = static_cast<ASTIfStatement*>(stmt);
            result = findNodeAndParentsInExpr(s->condition.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInStmt(s->stmtThen.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInStmt(s->stmtElse.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_OnBlockExit: {
            ASTOnBlockExitStatement* s = static_cast<ASTOnBlockExitStatement*>(stmt);
            result = findNodeAndParentsInStmt(s->statement.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_Rethrow:
            break;
        case ASTStatement::Kind::ASK_Return: {
            ASTReturnStatement* s = static_cast<ASTReturnStatement*>(stmt);
            result = findNodeAndParentsInExpr(s->retval.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_Summarize: {
            ASTSummarizeStatement* s = static_cast<ASTSummarizeStatement*>(stmt);
            result = findNodeAndParentsInExpr(s->name.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInExpr(s->data.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInExpr(s->by.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            for (unsigned int i = 0, count = s->contextMods.size(); i < count; i++) {
                result = findNodeAndParentsInExpr(s->contextMods[i], line, col);
                if (result) { result->push_back(stmt); return result; }
            }
            result = findNodeAndParentsInStmt(s->statements.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_Switch: {
            ASTSwitchStatement* s = static_cast<ASTSwitchStatement*>(stmt);
            result = findNodeAndParentsInExpr(s->variable.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInExpr(s->body.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_ThreadExit:
            break;
        case ASTStatement::Kind::ASK_Throw: {
            ASTThrowStatement* s = static_cast<ASTThrowStatement*>(stmt);
            result = findNodeAndParentsInExpr(s->expression.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_Try: {
            ASTTryStatement* s = static_cast<ASTTryStatement*>(stmt);
            result = findNodeAndParentsInStmt(s->tryStmt.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInExpr(s->catchVar.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInStmt(s->catchStmt.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        case ASTStatement::Kind::ASK_While: {
            ASTWhileStatement* s = static_cast<ASTWhileStatement*>(stmt);
            result = findNodeAndParentsInExpr(s->condition.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            result = findNodeAndParentsInStmt(s->statement.get(), line, col);
            if (result) { result->push_back(stmt); return result; }
            break;
        }
        default:
            break;
    }
    result = prepareResultVector();
    result->push_back(stmt);
    return result;
}

std::vector<ASTNode*>* FindNodeAndParentsQuery::findNodeAndParents(ASTTree* tree, ast_loc_t line, ast_loc_t col) {
    if (!tree)
        return nullptr;

    std::vector<ASTNode*>* result = nullptr;
    for (unsigned int i = 0, count = tree->nodes.size(); i < count; i++) {
        ASTNode* node = tree->nodes[i];
        switch (node->getNodeType()) {
            case ANT_Declaration: {
                ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
                result = findNodeAndParentsInDecl(decl, line, col);
                break;
            }
            case ANT_Expression: {
                ASTExpression* expr = static_cast<ASTExpression*>(node);
                result = findNodeAndParentsInExpr(expr, line, col);
                break;
            }
            case ANT_Name: {
                ASTName* name = static_cast<ASTName*>(node);
                result = findNodeAndParentsInName(name, line, col);
                break;
            }
            case ANT_ParseOption: {
                ASTParseOption* po = static_cast<ASTParseOption*>(node);
                result = findNodeAndParentsInPO(po, line, col);
                break;
            }
            case ANT_Statement: {
                ASTStatement* stmt = static_cast<ASTStatement*>(node);
                result = findNodeAndParentsInStmt(stmt, line, col);
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
