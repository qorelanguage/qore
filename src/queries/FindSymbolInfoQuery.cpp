/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  FindSymbolInfoQuery.cpp

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

#include "queries/FindSymbolInfoQuery.h"

#include <memory>
#include <string>
#include <sstream>

#include "AstPrinter.h"
#include "AstTreePrinter.h"
#include "ast/AST.h"
#include "queries/FindNodeAndParentsQuery.h"

/*void FindSymbolInfoQuery::classHoverInfo(ASTSymbolInfo& hi, ASTClassDeclaration* d) {
    hi.loc = d->loc;
    std::ostringstream os;
    AstPrinter::printClassSignature(os, d);
    hi.strings.push_back(std::move(os.str()));

    // Class methods.
    os.str("");
    os << "__Methods:__\n\n";
    for (size_t i = 0, count = d->declarations.size(); i < count; i++) {
        ASTDeclaration* inner = d->declarations[i];
        if (inner && inner->getKind() == ASTDeclaration::Kind::ADK_Function) {
            AstPrinter::printFunctionSignature(os, static_cast<ASTFunctionDeclaration*>(inner));
            os << "\n\n";
        }
    }
    hi.strings.push_back(std::move(os.str()));

    // Class members.
    for (size_t i = 0, count = d->declarations.size(); i < count; i++) {
        ASTDeclaration* inner = d->declarations[i];
        if (inner && inner->getKind() == ASTDeclaration::Kind::ADK_MemberGroup) {
            ASTMemberGroupDeclaration* mgd = static_cast<ASTMemberGroupDeclaration*>(inner);
            os.str("");
            if (mgd->modifiers.contains(AM_Public))
                os << "__Public members:__\n\n";
            else if (mgd->modifiers.contains(AM_Private))
                os << "__Private members:__\n\n";
            else if (mgd->modifiers.contains(AM_PrivateHierarchy))
                os << "__Private:hierarchy members:__\n\n";
            else if (mgd->modifiers.contains(AM_PrivateInternal))
                os << "__Private:internal members:__\n\n";
            else
                os << "__Members:__\n\n";
            for (size_t j = 0, mcount = mgd->members.size(); j < mcount; j++) {
                ASTExpression* expr = mgd->members[j];
                if (expr->getKind() == ASTExpression::Kind::AEK_Decl) {
                    AstPrinter::printDeclExpression(os, static_cast<ASTDeclExpression*>(expr));
                    os << "\n\n";
                }
                else if (expr->getKind() == ASTExpression::Kind::AEK_Assignment) {
                    ASTExpression* left = static_cast<ASTAssignmentExpression*>(expr)->left.get();
                    if (left->getKind() == ASTExpression::Kind::AEK_Decl) {
                        AstPrinter::printDeclExpression(os, static_cast<ASTDeclExpression*>(left));
                        os << "\n\n";
                    }
                }
            }
            hi.strings.push_back(std::move(os.str()));
        }
    }
}*/

bool exprMatches(ASTExpression* expr, ast_loc_t line, ast_loc_t col) {
    return expr && expr->loc.inside(line, col) && expr->getKind() == ASTExpression::Kind::AEK_Name;
}

ASTSymbolInfo exprHoverInfo(ASTExpression* expr, ASTSymbolKind sk, ASTSymbolUsageKind suk) {
    ASTNameExpression* name = static_cast<ASTNameExpression*>(expr);
    return ASTSymbolInfo(sk, suk, name->loc, name->name.name);
}

ASTSymbolInfo FindSymbolInfoQuery::nextRound(std::vector<ASTNode*>* nodes, ast_loc_t line, ast_loc_t col) {
    if (nodes->size() > 0) {
        nodes->erase(nodes->begin());
        if (nodes->size() > 0) {
            ASTNodeType nt = nodes->front()->getNodeType();
            if (nt == ANT_Declaration)
                return std::move(inDeclaration(nodes, line, col));
            else if (nt == ANT_Expression)
                return std::move(inExpression(nodes, line, col));
            else if (nt == ANT_Statement)
                return std::move(inStatement(nodes, line, col));
        }
    }
    return ASTSymbolInfo();
}

ASTSymbolInfo FindSymbolInfoQuery::inDeclaration(std::vector<ASTNode*>* nodes, ast_loc_t line, ast_loc_t col) {
    ASTDeclaration* decl = static_cast<ASTDeclaration*>(nodes->front());
    if (!decl)
        return ASTSymbolInfo();
    switch (decl->getKind()) {
        case ASTDeclaration::Kind::ADK_Class: {
            ASTClassDeclaration* d = static_cast<ASTClassDeclaration*>(decl);
            //classHoverInfo(hi, d);
            if (d->name.loc.inside(line, col))
                return ASTSymbolInfo(ASYK_Class, ASUK_ClassDeclName, d->name.loc, d->name.name);
            break;
        }
        case ASTDeclaration::Kind::ADK_Closure:
            break;
        case ASTDeclaration::Kind::ADK_Constant: {
            ASTConstantDeclaration* d = static_cast<ASTConstantDeclaration*>(decl);
            if (d->name.loc.inside(line, col))
                return ASTSymbolInfo(ASYK_Constant, ASUK_ConstantDeclName, d->name.loc, d->name.name);
            break;
        }
        case ASTDeclaration::Kind::ADK_Function: {
            ASTFunctionDeclaration* d = static_cast<ASTFunctionDeclaration*>(decl);
            if (d->name.loc.inside(line, col))
                return ASTSymbolInfo(ASYK_Function, ASUK_FuncDeclName, d->name.loc, d->name.name);
            if (d->returnType && d->returnType->loc.inside(line, col)) {
                if (d->returnType->getKind() == ASTExpression::Kind::AEK_Name) {
                    ASTNameExpression* name = static_cast<ASTNameExpression*>(d->returnType.get());
                    return ASTSymbolInfo(ASYK_Class, ASUK_FuncReturnType, d->returnType->loc, name->name.name);
                }
            }
            break;
        }
        case ASTDeclaration::Kind::ADK_MemberGroup:
            break;
        case ASTDeclaration::Kind::ADK_Namespace: {
            ASTNamespaceDeclaration* d = static_cast<ASTNamespaceDeclaration*>(decl);
            if (d->name.loc.inside(line, col))
                return ASTSymbolInfo(ASYK_Namespace, ASUK_NamespaceDeclName, d->name.loc, d->name.name);
            break;
        }
        case ASTDeclaration::Kind::ADK_Superclass: {
            ASTSuperclassDeclaration* d = static_cast<ASTSuperclassDeclaration*>(decl);
            if (d->name.loc.inside(line, col))
                return ASTSymbolInfo(ASYK_Class, ASUK_SuperclassDeclName, d->name.loc, d->name.name);
            break;
        }
        case ASTDeclaration::Kind::ADK_Variable: {
            ASTVariableDeclaration* d = static_cast<ASTVariableDeclaration*>(decl);
            if (d->typeName.loc.inside(line, col))
                return ASTSymbolInfo(ASYK_Class, ASUK_VarDeclTypeName, d->typeName.loc, d->typeName.name);
            if (d->name.loc.inside(line, col))
                return ASTSymbolInfo(ASYK_Variable, ASUK_VarDeclName, d->name.loc, d->name.name);
            break;
        }
        case ASTDeclaration::Kind::ADK_VarList:
        default:
            break;
    }
    return ASTSymbolInfo();
}

ASTSymbolInfo FindSymbolInfoQuery::inExpression(std::vector<ASTNode*>* nodes, ast_loc_t line, ast_loc_t col) {
    ASTExpression* expr = static_cast<ASTExpression*>(nodes->front());
    if (!expr)
        return ASTSymbolInfo();
    switch (expr->getKind()) {
        case ASTExpression::Kind::AEK_Access: {
            ASTAccessExpression* e = static_cast<ASTAccessExpression*>(expr);
            if (exprMatches(e->variable.get(), line, col))
                return std::move(exprHoverInfo(e->variable.get(), ASYK_Variable, ASUK_AccessVariable));
            if (exprMatches(e->member.get(), line, col))
                return std::move(exprHoverInfo(e->member.get(), ASYK_Variable, ASUK_AccessMember));
            break;
        }
        case ASTExpression::Kind::AEK_Assignment: {
            ASTAssignmentExpression* e = static_cast<ASTAssignmentExpression*>(expr);
            if (exprMatches(e->left.get(), line, col))
                return std::move(exprHoverInfo(e->left.get(), ASYK_Variable, ASUK_AssignmentLeft));
            if (exprMatches(e->right.get(), line, col))
                return std::move(exprHoverInfo(e->right.get(), ASYK_Variable, ASUK_AssignmentRight));
            break;
        }
        case ASTExpression::Kind::AEK_Backquote:
            break;
        case ASTExpression::Kind::AEK_Binary: {
            ASTBinaryExpression* e = static_cast<ASTBinaryExpression*>(expr);
            if (exprMatches(e->left.get(), line, col))
                return std::move(exprHoverInfo(e->left.get(), ASYK_Variable, ASUK_BinaryLeft));
            if (exprMatches(e->right.get(), line, col))
                return std::move(exprHoverInfo(e->right.get(), ASYK_Variable, ASUK_BinaryRight));
            break;
        }
        case ASTExpression::Kind::AEK_Call: {
            ASTCallExpression* e = static_cast<ASTCallExpression*>(expr);
            if (exprMatches(e->target.get(), line, col))
                return std::move(exprHoverInfo(e->target.get(), ASYK_Function, ASUK_CallTarget));
            if (exprMatches(e->args.get(), line, col))
                return std::move(exprHoverInfo(e->args.get(), ASYK_Variable, ASUK_CallArgs));
            break;
        }
        case ASTExpression::Kind::AEK_Case: {
            ASTCaseExpression* e = static_cast<ASTCaseExpression*>(expr);
            if (exprMatches(e->caseExpr.get(), line, col))
                return std::move(exprHoverInfo(e->caseExpr.get(), ASYK_Variable, ASUK_CaseExpr));
            break;
        }
        case ASTExpression::Kind::AEK_Cast: {
            ASTCastExpression* e = static_cast<ASTCastExpression*>(expr);
            if (e->castType.loc.inside(line, col))
                return ASTSymbolInfo(ASYK_Class, ASUK_CastType, e->castType.loc, e->castType.name);
            if (exprMatches(e->obj.get(), line, col))
                return std::move(exprHoverInfo(e->obj.get(), ASYK_Variable, ASUK_CastObject));
            break;
        }
        case ASTExpression::Kind::AEK_Closure:
        case ASTExpression::Kind::AEK_ConstrInit:
        case ASTExpression::Kind::AEK_ContextMod:
        case ASTExpression::Kind::AEK_ContextRow:
            break;
        case ASTExpression::Kind::AEK_Decl:
            return std::move(nextRound(nodes, line, col));
        case ASTExpression::Kind::AEK_Find: {
            ASTFindExpression* e = static_cast<ASTFindExpression*>(expr);
            if (exprMatches(e->data.get(), line, col))
                return std::move(exprHoverInfo(e->data.get(), ASYK_Variable, ASUK_FindData));
            break;
        }
        case ASTExpression::Kind::AEK_Hash:
            break;
        case ASTExpression::Kind::AEK_HashElement: {
            ASTHashElementExpression* e = static_cast<ASTHashElementExpression*>(expr);
            if (exprMatches(e->value.get(), line, col))
                return std::move(exprHoverInfo(e->value.get(), ASYK_Variable, ASUK_HashValue));
            break;
        }
        case ASTExpression::Kind::AEK_ImplicitArg:
        case ASTExpression::Kind::AEK_ImplicitElem:
            break;
        case ASTExpression::Kind::AEK_Index: {
            ASTIndexExpression* e = static_cast<ASTIndexExpression*>(expr);
            if (exprMatches(e->variable.get(), line, col))
                return std::move(exprHoverInfo(e->variable.get(), ASYK_Variable, ASUK_IndexVariable));
            if (exprMatches(e->index.get(), line, col))
                return std::move(exprHoverInfo(e->index.get(), ASYK_Variable, ASUK_IndexIndex));
            break;
        }
        case ASTExpression::Kind::AEK_List: {
            ASTListExpression* e = static_cast<ASTListExpression*>(expr);
            for (size_t i = 0, count = e->elements.size(); i < count; i++) {
                ASTExpression* le = e->elements[i];
                if (exprMatches(le, line, col))
                    return std::move(exprHoverInfo(le, ASYK_Variable, ASUK_ListElement));
            }
            break;
        }
        case ASTExpression::Kind::AEK_Literal:
            break;
        case ASTExpression::Kind::AEK_Name:
            return std::move(nextRound(nodes, line, col));
        case ASTExpression::Kind::AEK_Regex:
        case ASTExpression::Kind::AEK_RegexSubst:
        case ASTExpression::Kind::AEK_RegexTrans:
            break;
        case ASTExpression::Kind::AEK_Returns: {
            ASTReturnsExpression* e = static_cast<ASTReturnsExpression*>(expr);
            if (exprMatches(e->typeName.get(), line, col))
                return ASTSymbolInfo(ASYK_Class, ASUK_ReturnsType, e->typeName->loc, e->typeName->name.name);
            break;
        }
        case ASTExpression::Kind::AEK_SwitchBody:
            break;
        case ASTExpression::Kind::AEK_Ternary: {
            ASTTernaryExpression* e = static_cast<ASTTernaryExpression*>(expr);
            if (exprMatches(e->condition.get(), line, col))
                return std::move(exprHoverInfo(e->condition.get(), ASYK_Variable, ASUK_TernaryCond));
            if (exprMatches(e->exprTrue.get(), line, col))
                return std::move(exprHoverInfo(e->exprTrue.get(), ASYK_Variable, ASUK_TernaryTrue));
            if (exprMatches(e->exprFalse.get(), line, col))
                return std::move(exprHoverInfo(e->exprFalse.get(), ASYK_Variable, ASUK_TernaryFalse));
            break;
        }
        case ASTExpression::Kind::AEK_Unary: {
            ASTUnaryExpression* e = static_cast<ASTUnaryExpression*>(expr);
            if (exprMatches(e->expression.get(), line, col))
                return std::move(exprHoverInfo(e->expression.get(), ASYK_Variable, ASUK_Unary));
            break;
        }
        default:
            break;
    }
    return ASTSymbolInfo();
}

ASTSymbolInfo FindSymbolInfoQuery::inStatement(std::vector<ASTNode*>* nodes, ast_loc_t line, ast_loc_t col) {
    ASTStatement* stmt = static_cast<ASTStatement*>(nodes->front());
    if (!stmt)
        return ASTSymbolInfo();
    switch (stmt->getKind()) {
        case ASTStatement::Kind::ASK_Block:
        case ASTStatement::Kind::ASK_Break:
        case ASTStatement::Kind::ASK_Call:
            break;
        case ASTStatement::Kind::ASK_Context: {
            ASTContextStatement* s = static_cast<ASTContextStatement*>(stmt);
            if (exprMatches(s->name.get(), line, col))
                return std::move(exprHoverInfo(s->name.get(), ASYK_Variable, ASUK_ContextStmtName));
            if (exprMatches(s->data.get(), line, col))
                return std::move(exprHoverInfo(s->data.get(), ASYK_Variable, ASUK_ContextStmtData));
            break;
        }
        case ASTStatement::Kind::ASK_Continue:
            break;
        case ASTStatement::Kind::ASK_DoWhile: {
            ASTDoWhileStatement* s = static_cast<ASTDoWhileStatement*>(stmt);
            if (exprMatches(s->condition.get(), line, col))
                return std::move(exprHoverInfo(s->condition.get(), ASYK_Variable, ASUK_DoWhileStmtCond));
            break;
        }
        case ASTStatement::Kind::ASK_Expression: {
            ASTExpressionStatement* s = static_cast<ASTExpressionStatement*>(stmt);
            if (exprMatches(s->expression.get(), line, col))
                return std::move(exprHoverInfo(s->expression.get(), ASYK_Variable, ASUK_ExprStmtExpr));
            break;
        }
        case ASTStatement::Kind::ASK_For: {
            ASTForStatement* s = static_cast<ASTForStatement*>(stmt);
            if (exprMatches(s->init.get(), line, col))
                return std::move(exprHoverInfo(s->init.get(), ASYK_Variable, ASUK_ForStmtInit));
            if (exprMatches(s->condition.get(), line, col))
                return std::move(exprHoverInfo(s->condition.get(), ASYK_Variable, ASUK_ForStmtCond));
            if (exprMatches(s->iteration.get(), line, col))
                return std::move(exprHoverInfo(s->iteration.get(), ASYK_Variable, ASUK_ForStmtIter));
            break;
        }
        case ASTStatement::Kind::ASK_Foreach: {
            ASTForeachStatement* s = static_cast<ASTForeachStatement*>(stmt);
            if (exprMatches(s->value.get(), line, col))
                return std::move(exprHoverInfo(s->value.get(), ASYK_Variable, ASUK_ForeachStmtVal));
            if (exprMatches(s->source.get(), line, col))
                return std::move(exprHoverInfo(s->source.get(), ASYK_Variable, ASUK_ForeachStmtSrc));
            break;
        }
        case ASTStatement::Kind::ASK_If: {
            ASTIfStatement* s = static_cast<ASTIfStatement*>(stmt);
            if (exprMatches(s->condition.get(), line, col))
                return std::move(exprHoverInfo(s->condition.get(), ASYK_Variable, ASUK_IfStmtCond));
            break;
        }
        case ASTStatement::Kind::ASK_OnBlockExit:
        case ASTStatement::Kind::ASK_Rethrow:
            break;
        case ASTStatement::Kind::ASK_Return: {
            ASTReturnStatement* s = static_cast<ASTReturnStatement*>(stmt);
            if (exprMatches(s->retval.get(), line, col))
                return std::move(exprHoverInfo(s->retval.get(), ASYK_Variable, ASUK_ReturnStmtVal));
            break;
        }
        case ASTStatement::Kind::ASK_Summarize: {
            ASTSummarizeStatement* s = static_cast<ASTSummarizeStatement*>(stmt);
            if (exprMatches(s->name.get(), line, col))
                return std::move(exprHoverInfo(s->name.get(), ASYK_Variable, ASUK_SummarizeStmtName));
            if (exprMatches(s->data.get(), line, col))
                return std::move(exprHoverInfo(s->data.get(), ASYK_Variable, ASUK_SummarizeStmtData));
            break;
        }
        case ASTStatement::Kind::ASK_Switch: {
            ASTSwitchStatement* s = static_cast<ASTSwitchStatement*>(stmt);
            if (exprMatches(s->variable.get(), line, col))
                return std::move(exprHoverInfo(s->variable.get(), ASYK_Variable, ASUK_SwitchStmtVar));
            break;
        }
        case ASTStatement::Kind::ASK_ThreadExit:
            break;
        case ASTStatement::Kind::ASK_Throw: {
            ASTThrowStatement* s = static_cast<ASTThrowStatement*>(stmt);
            if (exprMatches(s->expression.get(), line, col))
                return std::move(exprHoverInfo(s->expression.get(), ASYK_Variable, ASUK_ThrowStmtExpr));
            break;
        }
        case ASTStatement::Kind::ASK_Try: {
            ASTTryStatement* s = static_cast<ASTTryStatement*>(stmt);
            if (exprMatches(s->catchVar.get(), line, col))
                return std::move(exprHoverInfo(s->catchVar.get(), ASYK_Variable, ASUK_TryStmtCatchVar));
            break;
        }
        case ASTStatement::Kind::ASK_While: {
            ASTWhileStatement* s = static_cast<ASTWhileStatement*>(stmt);
            if (exprMatches(s->condition.get(), line, col))
                return std::move(exprHoverInfo(s->condition.get(), ASYK_Variable, ASUK_WhileStmtCond));
            break;
        }
        default:
            break;
    }
    return ASTSymbolInfo();
}

ASTSymbolInfo FindSymbolInfoQuery::find(ASTTree* tree, ast_loc_t line, ast_loc_t col) {
    if (!tree)
        return ASTSymbolInfo();

    // Target node is first, all the parents follow.
    std::unique_ptr<std::vector<ASTNode*> > nodes(FindNodeAndParentsQuery::find(tree, line, col));
    if (!nodes)
        return ASTSymbolInfo();

    // Get rid of ASTName at the beginning.
    if (nodes->at(0)->getNodeType() == ANT_Name)
        nodes->erase(nodes->begin());

    ASTNode* node = nodes->front();
    switch(node->getNodeType()) {
        case ANT_Declaration:
            return std::move(inDeclaration(nodes.get(), line, col));
        case ANT_Expression:
            return std::move(inExpression(nodes.get(), line, col));
        case ANT_ParseError:
        case ANT_ParseOption:
            break;
        case ANT_Statement:
            return std::move(inStatement(nodes.get(), line, col));
        default:
            break;
    }

    return ASTSymbolInfo();
}