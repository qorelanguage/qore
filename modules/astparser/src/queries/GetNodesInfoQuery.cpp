/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  GetNodesInfoQuery.cpp

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

#include "queries/GetNodesInfoQuery.h"

#include <sstream>

#include "qore/Qore.h"

#include "AstTreePrinter.h"
#include "ast/AST.h"

QoreHashNode* GetNodesInfoQuery::getDeclaration(ASTDeclaration* decl, ExceptionSink* xsink) {
    if (!decl)
        return nullptr;

    ReferenceHolder<QoreHashNode> nodeInfo(new QoreHashNode, xsink);
    if (*xsink)
        return nullptr;

    nodeInfo->setKeyValue("nodetype", new QoreStringNode("decl"), xsink);
    nodeInfo->setKeyValue("loc", getLocation(decl->loc), xsink);
    switch (decl->getKind()) {
        case ASTDeclaration::Kind::ADK_Class: {
            ASTClassDeclaration* d = static_cast<ASTClassDeclaration*>(decl);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Class"), xsink);
            nodeInfo->setKeyValue("modifiers", getModifiers(d->modifiers), xsink);
            nodeInfo->setKeyValue("name", getName(d->name, xsink), xsink);
            ReferenceHolder<QoreListNode> inherits(new QoreListNode, xsink);
            ReferenceHolder<QoreListNode> declarations(new QoreListNode, xsink);
            if (*xsink)
                return nullptr;
            for (size_t i = 0, count = d->inherits.size(); i < count; i++)
                inherits->push(getDeclaration(d->inherits[i], xsink));
            for (size_t i = 0, count = d->declarations.size(); i < count; i++)
                declarations->push(getDeclaration(d->declarations[i], xsink));
            nodeInfo->setKeyValue("inherits", inherits.release(), xsink);
            nodeInfo->setKeyValue("declarations", declarations.release(), xsink);
            break;
        }
        case ASTDeclaration::Kind::ADK_Closure: {
            ASTClosureDeclaration* d = static_cast<ASTClosureDeclaration*>(decl);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Closure"), xsink);
            nodeInfo->setKeyValue("modifiers", getModifiers(d->modifiers), xsink);
            nodeInfo->setKeyValue("returnType", getExpression(d->returnType.get(), xsink), xsink);
            nodeInfo->setKeyValue("params", getExpression(d->params.get(), xsink), xsink);
            nodeInfo->setKeyValue("body", getStatement(d->body.get(), xsink), xsink);
            break;
        }
        case ASTDeclaration::Kind::ADK_Constant: {
            ASTConstantDeclaration* d = static_cast<ASTConstantDeclaration*>(decl);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Constant"), xsink);
            nodeInfo->setKeyValue("modifiers", getModifiers(d->modifiers), xsink);
            nodeInfo->setKeyValue("name", getName(d->name, xsink), xsink);
            nodeInfo->setKeyValue("value", getExpression(d->value.get(), xsink), xsink);
            break;
        }
        case ASTDeclaration::Kind::ADK_Function: {
            ASTFunctionDeclaration* d = static_cast<ASTFunctionDeclaration*>(decl);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Function"), xsink);
            nodeInfo->setKeyValue("modifiers", getModifiers(d->modifiers), xsink);
            nodeInfo->setKeyValue("name", getName(d->name, xsink), xsink);
            nodeInfo->setKeyValue("afdKind", new QoreBigIntNode(static_cast<int64>(d->afdKind)), xsink);
            nodeInfo->setKeyValue("returnType", getExpression(d->returnType.get(), xsink), xsink);
            nodeInfo->setKeyValue("params", getExpression(d->params.get(), xsink), xsink);
            nodeInfo->setKeyValue("inits", getExpression(d->inits.get(), xsink), xsink);
            nodeInfo->setKeyValue("body", getStatement(d->body.get(), xsink), xsink);
            break;
        }
        case ASTDeclaration::Kind::ADK_MemberGroup: {
            ASTMemberGroupDeclaration* d = static_cast<ASTMemberGroupDeclaration*>(decl);
            nodeInfo->setKeyValue("kind", new QoreStringNode("MemberGroup"), xsink);
            nodeInfo->setKeyValue("modifiers", getModifiers(d->modifiers), xsink);
            ReferenceHolder<QoreListNode> members(new QoreListNode, xsink);
            if (*xsink)
                return nullptr;
            for (size_t i = 0, count = d->members.size(); i < count; i++)
                members->push(getExpression(d->members[i], xsink));
            nodeInfo->setKeyValue("members", members.release(), xsink);
            break;
        }
        case ASTDeclaration::Kind::ADK_Namespace: {
            ASTNamespaceDeclaration* d = static_cast<ASTNamespaceDeclaration*>(decl);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Namespace"), xsink);
            nodeInfo->setKeyValue("modifiers", getModifiers(d->modifiers), xsink);
            nodeInfo->setKeyValue("name", getName(d->name, xsink), xsink);
            ReferenceHolder<QoreListNode> declarations(new QoreListNode, xsink);
            if (*xsink)
                return nullptr;
            for (size_t i = 0, count = d->declarations.size(); i < count; i++)
                declarations->push(getDeclaration(d->declarations[i], xsink));
            nodeInfo->setKeyValue("declarations", declarations.release(), xsink);
            break;
        }
        case ASTDeclaration::Kind::ADK_Superclass: {
            ASTSuperclassDeclaration* d = static_cast<ASTSuperclassDeclaration*>(decl);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Superclass"), xsink);
            nodeInfo->setKeyValue("modifiers", getModifiers(d->modifiers), xsink);
            nodeInfo->setKeyValue("name", getName(d->name, xsink), xsink);
            break;
        }
        case ASTDeclaration::Kind::ADK_Variable: {
            ASTVariableDeclaration* d = static_cast<ASTVariableDeclaration*>(decl);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Variable"), xsink);
            nodeInfo->setKeyValue("modifiers", getModifiers(d->modifiers), xsink);
            nodeInfo->setKeyValue("name", getName(d->name, xsink), xsink);
            nodeInfo->setKeyValue("typeName", getName(d->typeName, xsink), xsink);
            break;
        }
        case ASTDeclaration::Kind::ADK_VarList: {
            ASTVarListDeclaration* d = static_cast<ASTVarListDeclaration*>(decl);
            nodeInfo->setKeyValue("kind", new QoreStringNode("VarList"), xsink);
            nodeInfo->setKeyValue("modifiers", getModifiers(d->modifiers), xsink);
            nodeInfo->setKeyValue("variables", getExpression(d->variables.get(), xsink), xsink);
            break;
        }
        default:
            break;
    }
    if (*xsink)
        return nullptr;
    return nodeInfo.release();
}

QoreHashNode* GetNodesInfoQuery::getExpression(ASTExpression* expr, ExceptionSink* xsink) {
    if (!expr)
        return nullptr;

    ReferenceHolder<QoreHashNode> nodeInfo(new QoreHashNode, xsink);
    if (*xsink)
        return nullptr;

    nodeInfo->setKeyValue("nodetype", new QoreStringNode("expr"), xsink);
    nodeInfo->setKeyValue("loc", getLocation(expr->loc), xsink);
    switch (expr->getKind()) {
        case ASTExpression::Kind::AEK_Access: {
            ASTAccessExpression* e = static_cast<ASTAccessExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Access"), xsink);
            nodeInfo->setKeyValue("variable", getExpression(e->variable.get(), xsink), xsink);
            nodeInfo->setKeyValue("member", getExpression(e->member.get(), xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Assignment: {
            ASTAssignmentExpression* e = static_cast<ASTAssignmentExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Assignment"), xsink);
            nodeInfo->setKeyValue("left", getExpression(e->left.get(), xsink), xsink);
            nodeInfo->setKeyValue("right", getExpression(e->right.get(), xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Backquote: {
            ASTBackquoteExpression* e = static_cast<ASTBackquoteExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Backquote"), xsink);
            nodeInfo->setKeyValue("command", new QoreStringNode(e->command), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Binary: {
            ASTBinaryExpression* e = static_cast<ASTBinaryExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Binary"), xsink);
            nodeInfo->setKeyValue("op", getOperator(e->op), xsink);
            nodeInfo->setKeyValue("left", getExpression(e->left.get(), xsink), xsink);
            nodeInfo->setKeyValue("right", getExpression(e->right.get(), xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Call: {
            ASTCallExpression* e = static_cast<ASTCallExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Call"), xsink);
            nodeInfo->setKeyValue("target", getExpression(e->target.get(), xsink), xsink);
            nodeInfo->setKeyValue("args", getExpression(e->args.get(), xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Case: {
            ASTCaseExpression* e = static_cast<ASTCaseExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Case"), xsink);
            nodeInfo->setKeyValue("op", getOperator(e->op), xsink);
            nodeInfo->setKeyValue("caseExpr", getExpression(e->caseExpr.get(), xsink), xsink);
            nodeInfo->setKeyValue("statements", getStatement(e->statements.get(), xsink), xsink);
            nodeInfo->setKeyValue("defaultCase", get_bool_node(e->defaultCase), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Cast: {
            ASTCastExpression* e = static_cast<ASTCastExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Cast"), xsink);
            nodeInfo->setKeyValue("castType", getName(e->castType, xsink), xsink);
            nodeInfo->setKeyValue("obj", getExpression(e->obj.get(), xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Closure: {
            ASTClosureExpression* e = static_cast<ASTClosureExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Closure"), xsink);
            nodeInfo->setKeyValue("closure", getDeclaration(e->closure.get(), xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_ConstrInit: {
            ASTConstrInitExpression* e = static_cast<ASTConstrInitExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("ConstrInit"), xsink);
            ReferenceHolder<QoreListNode> inits(new QoreListNode, xsink);
            if (*xsink)
                return nullptr;
            for (size_t i = 0, count = e->inits.size(); i < count; i++)
                inits->push(getExpression(e->inits[i], xsink));
            nodeInfo->setKeyValue("inits", inits.release(), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_ContextMod: {
            ASTContextModExpression* e = static_cast<ASTContextModExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("ContextMod"), xsink);
            nodeInfo->setKeyValue("acmeKind", new QoreBigIntNode(static_cast<int64>(e->acmeKind)), xsink);
            nodeInfo->setKeyValue("expression", getExpression(e->expression.get(), xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_ContextRow: {
            nodeInfo->setKeyValue("kind", new QoreStringNode("ContextRow"), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Decl: {
            ASTDeclExpression* e = static_cast<ASTDeclExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Decl"), xsink);
            nodeInfo->setKeyValue("declaration", getDeclaration(e->declaration.get(), xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Find: {
            ASTFindExpression* e = static_cast<ASTFindExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Find"), xsink);
            nodeInfo->setKeyValue("result", getExpression(e->result.get(), xsink), xsink);
            nodeInfo->setKeyValue("data", getExpression(e->data.get(), xsink), xsink);
            nodeInfo->setKeyValue("where", getExpression(e->where.get(), xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Hash: {
            ASTHashExpression* e = static_cast<ASTHashExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Hash"), xsink);
            ReferenceHolder<QoreListNode> elements(new QoreListNode, xsink);
            if (*xsink)
                return nullptr;
            for (size_t i = 0, count = e->elements.size(); i < count; i++)
                elements->push(getExpression(e->elements[i], xsink));
            nodeInfo->setKeyValue("elements", elements.release(), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_HashElement: {
            ASTHashElementExpression* e = static_cast<ASTHashElementExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("HashElement"), xsink);
            nodeInfo->setKeyValue("key", getExpression(e->key.get(), xsink), xsink);
            nodeInfo->setKeyValue("value", getExpression(e->value.get(), xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_ImplicitArg: {
            ASTImplicitArgExpression* e = static_cast<ASTImplicitArgExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("ImplicitArg"), xsink);
            nodeInfo->setKeyValue("offset", new QoreBigIntNode(static_cast<int64>(e->offset)), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_ImplicitElem: {
            nodeInfo->setKeyValue("kind", new QoreStringNode("ImplicitElem"), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Index: {
            ASTIndexExpression* e = static_cast<ASTIndexExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Index"), xsink);
            nodeInfo->setKeyValue("variable", getExpression(e->variable.get(), xsink), xsink);
            nodeInfo->setKeyValue("index", getExpression(e->index.get(), xsink), xsink);
            nodeInfo->setKeyValue("indexKind", new QoreBigIntNode(static_cast<int64>(e->indexKind)), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_List: {
            ASTListExpression* e = static_cast<ASTListExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("List"), xsink);
            ReferenceHolder<QoreListNode> elements(new QoreListNode, xsink);
            if (*xsink)
                return nullptr;
            for (size_t i = 0, count = e->elements.size(); i < count; i++)
                elements->push(getExpression(e->elements[i], xsink));
            nodeInfo->setKeyValue("elements", elements.release(), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Literal: {
            ASTLiteralExpression* e = static_cast<ASTLiteralExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Literal"), xsink);
            switch (e->kind) {
                case ALEK_Binary:
                    nodeInfo->setKeyValue("literalKind", new QoreStringNode("binary"), xsink);
                    nodeInfo->setKeyValue("value", new QoreStringNode(e->value.str), xsink);
                    break;
                case ALEK_Date:
                    nodeInfo->setKeyValue("literalKind", new QoreStringNode("date"), xsink);
                    nodeInfo->setKeyValue("value", new DateTimeNode(e->value.str), xsink);
                    break;
                case ALEK_Float:
                    nodeInfo->setKeyValue("literalKind", new QoreStringNode("float"), xsink);
                    nodeInfo->setKeyValue("value", new QoreFloatNode(e->value.d), xsink);
                    break;
                case ALEK_Int:
                    nodeInfo->setKeyValue("literalKind", new QoreStringNode("int"), xsink);
                    nodeInfo->setKeyValue("value", new QoreBigIntNode(e->value.i), xsink);
                    break;
                case ALEK_Number:
                    nodeInfo->setKeyValue("literalKind", new QoreStringNode("number"), xsink);
                    nodeInfo->setKeyValue("value", new QoreNumberNode(e->value.str), xsink);
                    break;
                case ALEK_String:
                    nodeInfo->setKeyValue("literalKind", new QoreStringNode("string"), xsink);
                    nodeInfo->setKeyValue("value", new QoreStringNode(e->value.stdstr), xsink);
                    break;
                default:
                    break;
            }
            break;
        }
        case ASTExpression::Kind::AEK_Name: {
            ASTNameExpression* e = static_cast<ASTNameExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Name"), xsink);
            nodeInfo->setKeyValue("name", getName(e->name, xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Regex: {
            ASTRegexExpression* e = static_cast<ASTRegexExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Regex"), xsink);
            nodeInfo->setKeyValue("str", new QoreStringNode(e->str), xsink);
            nodeInfo->setKeyValue("extractRegex", get_bool_node(e->extractRegex), xsink);
            nodeInfo->setKeyValue("caseSensitive", get_bool_node(e->caseSensitive), xsink);
            nodeInfo->setKeyValue("extended", get_bool_node(e->extended), xsink);
            nodeInfo->setKeyValue("dotAll", get_bool_node(e->dotAll), xsink);
            nodeInfo->setKeyValue("multiline", get_bool_node(e->multiline), xsink);
            nodeInfo->setKeyValue("global", get_bool_node(e->global), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_RegexSubst: {
            ASTRegexSubstExpression* e = static_cast<ASTRegexSubstExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("RegexSubst"), xsink);
            nodeInfo->setKeyValue("source", new QoreStringNode(e->source), xsink);
            nodeInfo->setKeyValue("target", new QoreStringNode(e->target), xsink);
            nodeInfo->setKeyValue("caseSensitive", get_bool_node(e->caseSensitive), xsink);
            nodeInfo->setKeyValue("extended", get_bool_node(e->extended), xsink);
            nodeInfo->setKeyValue("dotAll", get_bool_node(e->dotAll), xsink);
            nodeInfo->setKeyValue("multiline", get_bool_node(e->multiline), xsink);
            nodeInfo->setKeyValue("global", get_bool_node(e->global), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_RegexTrans: {
            ASTRegexTransExpression* e = static_cast<ASTRegexTransExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("RegexTrans"), xsink);
            nodeInfo->setKeyValue("source", new QoreStringNode(e->source), xsink);
            nodeInfo->setKeyValue("target", new QoreStringNode(e->target), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Returns: {
            ASTReturnsExpression* e = static_cast<ASTReturnsExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Returns"), xsink);
            nodeInfo->setKeyValue("typeName", getExpression(e->typeName.get(), xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_SwitchBody: {
            ASTSwitchBodyExpression* e = static_cast<ASTSwitchBodyExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("SwitchBody"), xsink);
            ReferenceHolder<QoreListNode> cases(new QoreListNode, xsink);
            if (*xsink)
                return nullptr;
            for (size_t i = 0, count = e->cases.size(); i < count; i++)
                cases->push(getExpression(e->cases[i], xsink));
            nodeInfo->setKeyValue("cases", cases.release(), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Ternary: {
            ASTTernaryExpression* e = static_cast<ASTTernaryExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Ternary"), xsink);
            nodeInfo->setKeyValue("condition", getExpression(e->condition.get(), xsink), xsink);
            nodeInfo->setKeyValue("exprTrue", getExpression(e->exprTrue.get(), xsink), xsink);
            nodeInfo->setKeyValue("exprFalse", getExpression(e->exprFalse.get(), xsink), xsink);
            break;
        }
        case ASTExpression::Kind::AEK_Unary: {
            ASTUnaryExpression* e = static_cast<ASTUnaryExpression*>(expr);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Unary"), xsink);
            nodeInfo->setKeyValue("op", getOperator(e->op), xsink);
            nodeInfo->setKeyValue("expression", getExpression(e->expression.get(), xsink), xsink);
            break;
        }
        default:
            break;
    }
    if (*xsink)
        return nullptr;
    return nodeInfo.release();
}

QoreStringNode* GetNodesInfoQuery::getLocation(const ASTParseLocation& loc) {
    std::ostringstream oss;
    AstTreePrinter::printLocation(oss, loc, 0, false);
    return new QoreStringNode(oss.str());
}

QoreStringNode* GetNodesInfoQuery::getModifiers(ASTModifiers& mods) {
    std::ostringstream oss;
    AstTreePrinter::printModifiers(oss, mods, 0, true);
    return new QoreStringNode(oss.str());
}

QoreHashNode* GetNodesInfoQuery::getName(ASTName& name, ExceptionSink* xsink) {
    ReferenceHolder<QoreHashNode> nodeInfo(new QoreHashNode, xsink);
    if (*xsink)
        return nullptr;

    nodeInfo->setKeyValue("nodetype", new QoreStringNode("name"), xsink);
    nodeInfo->setKeyValue("loc", getLocation(name.loc), xsink);
    nodeInfo->setKeyValue("name", new QoreStringNode(name.name), xsink);
    if (*xsink)
        return nullptr;
    return nodeInfo.release();
}

QoreHashNode* GetNodesInfoQuery::getName(ASTName* name, ExceptionSink* xsink) {
    if (!name)
        return nullptr;
    return getName(*name, xsink);
}

QoreStringNode* GetNodesInfoQuery::getOperator(ASTOperator& op) {
    std::ostringstream oss;
    AstTreePrinter::printOperator(oss, op, 0, false);
    return new QoreStringNode(oss.str());
}

QoreHashNode* GetNodesInfoQuery::getParseOption(ASTParseOption* po, ExceptionSink* xsink) {
    if (!po)
        return nullptr;

    ReferenceHolder<QoreHashNode> nodeInfo(new QoreHashNode, xsink);
    if (*xsink)
        return nullptr;

    nodeInfo->setKeyValue("nodetype", new QoreStringNode("po"), xsink);
    nodeInfo->setKeyValue("loc", getLocation(po->loc), xsink);
    std::ostringstream oss;
    AstTreePrinter::printParseOptionString(oss, po);
    nodeInfo->setKeyValue("option", new QoreStringNode(oss.str()), xsink);
    if (*xsink)
        return nullptr;
    return nodeInfo.release();
}

QoreHashNode* GetNodesInfoQuery::getStatement(ASTStatement* stmt, ExceptionSink* xsink) {
    if (!stmt)
        return nullptr;

    ReferenceHolder<QoreHashNode> nodeInfo(new QoreHashNode, xsink);
    if (*xsink)
        return nullptr;

    nodeInfo->setKeyValue("nodetype", new QoreStringNode("stmt"), xsink);
    nodeInfo->setKeyValue("loc", getLocation(stmt->loc), xsink);
    switch (stmt->getKind()) {
        case ASTStatement::Kind::ASK_Block: {
            ASTStatementBlock* s = static_cast<ASTStatementBlock*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("StatementBlock"), xsink);
            ReferenceHolder<QoreListNode> statements(new QoreListNode, xsink);
            for (size_t i = 0, count = s->statements.size(); i < count; i++)
                statements->push(getStatement(s->statements[i], xsink));
            nodeInfo->setKeyValue("statements", statements.release(), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_Break: {
            nodeInfo->setKeyValue("kind", new QoreStringNode("Break"), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_Call: {
            ASTCallStatement* s = static_cast<ASTCallStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Call"), xsink);
            nodeInfo->setKeyValue("call", getExpression(s->call.get(), xsink), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_Context: {
            ASTContextStatement* s = static_cast<ASTContextStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Context"), xsink);
            nodeInfo->setKeyValue("name", getExpression(s->name.get(), xsink), xsink);
            nodeInfo->setKeyValue("data", getExpression(s->data.get(), xsink), xsink);
            ReferenceHolder<QoreListNode> contextMods(new QoreListNode, xsink);
            if (*xsink)
                return nullptr;
            for (size_t i = 0, count = s->contextMods.size(); i < count; i++)
                contextMods->push(getExpression(s->contextMods[i], xsink));
            nodeInfo->setKeyValue("contextMods", contextMods.release(), xsink);
            nodeInfo->setKeyValue("statements", getStatement(s->statements.get(), xsink), xsink);
            nodeInfo->setKeyValue("subcontext", get_bool_node(s->subcontext), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_Continue: {
            nodeInfo->setKeyValue("kind", new QoreStringNode("Continue"), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_DoWhile: {
            ASTDoWhileStatement* s = static_cast<ASTDoWhileStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("DoWhile"), xsink);
            nodeInfo->setKeyValue("condition", getExpression(s->condition.get(), xsink), xsink);
            nodeInfo->setKeyValue("statement", getStatement(s->statement.get(), xsink), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_Expression: {
            ASTExpressionStatement* s = static_cast<ASTExpressionStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Expression"), xsink);
            nodeInfo->setKeyValue("expression", getExpression(s->expression.get(), xsink), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_For: {
            ASTForStatement* s = static_cast<ASTForStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("For"), xsink);
            nodeInfo->setKeyValue("init", getExpression(s->init.get(), xsink), xsink);
            nodeInfo->setKeyValue("condition", getExpression(s->condition.get(), xsink), xsink);
            nodeInfo->setKeyValue("iteration", getExpression(s->iteration.get(), xsink), xsink);
            nodeInfo->setKeyValue("statement", getStatement(s->statement.get(), xsink), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_Foreach: {
            ASTForeachStatement* s = static_cast<ASTForeachStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Foreach"), xsink);
            nodeInfo->setKeyValue("value", getExpression(s->value.get(), xsink), xsink);
            nodeInfo->setKeyValue("source", getExpression(s->source.get(), xsink), xsink);
            nodeInfo->setKeyValue("statement", getStatement(s->statement.get(), xsink), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_If: {
            ASTIfStatement* s = static_cast<ASTIfStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("If"), xsink);
            nodeInfo->setKeyValue("condition", getExpression(s->condition.get(), xsink), xsink);
            nodeInfo->setKeyValue("stmtThen", getStatement(s->stmtThen.get(), xsink), xsink);
            nodeInfo->setKeyValue("stmtElse", getStatement(s->stmtElse.get(), xsink), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_OnBlockExit: {
            ASTOnBlockExitStatement* s = static_cast<ASTOnBlockExitStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("OnBlockExit"), xsink);
            if (s->condition == ASTOnBlockExitStatement::Condition::AOBEC_Error)
                nodeInfo->setKeyValue("condition", new QoreStringNode("error"), xsink);
            else if (s->condition == ASTOnBlockExitStatement::Condition::AOBEC_Exit)
                nodeInfo->setKeyValue("condition", new QoreStringNode("exit"), xsink);
            else if (s->condition == ASTOnBlockExitStatement::Condition::AOBEC_Success)
                nodeInfo->setKeyValue("condition", new QoreStringNode("success"), xsink);
            nodeInfo->setKeyValue("statement", getStatement(s->statement.get(), xsink), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_Rethrow: {
            nodeInfo->setKeyValue("kind", new QoreStringNode("Rethrow"), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_Return: {
            ASTReturnStatement* s = static_cast<ASTReturnStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Return"), xsink);
            nodeInfo->setKeyValue("retval", getExpression(s->retval.get(), xsink), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_Summarize: {
            ASTSummarizeStatement* s = static_cast<ASTSummarizeStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Summarize"), xsink);
            nodeInfo->setKeyValue("name", getExpression(s->name.get(), xsink), xsink);
            nodeInfo->setKeyValue("data", getExpression(s->data.get(), xsink), xsink);
            nodeInfo->setKeyValue("by", getExpression(s->by.get(), xsink), xsink);
            ReferenceHolder<QoreListNode> contextMods(new QoreListNode, xsink);
            if (*xsink)
                return nullptr;
            for (size_t i = 0, count = s->contextMods.size(); i < count; i++)
                contextMods->push(getExpression(s->contextMods[i], xsink));
            nodeInfo->setKeyValue("contextMods", contextMods.release(), xsink);
            nodeInfo->setKeyValue("statements", getStatement(s->statements.get(), xsink), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_Switch: {
            ASTSwitchStatement* s = static_cast<ASTSwitchStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Switch"), xsink);
            nodeInfo->setKeyValue("variable", getExpression(s->variable.get(), xsink), xsink);
            nodeInfo->setKeyValue("body", getExpression(s->body.get(), xsink), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_ThreadExit: {
            nodeInfo->setKeyValue("kind", new QoreStringNode("ThreadExit"), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_Throw: {
            ASTThrowStatement* s = static_cast<ASTThrowStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Throw"), xsink);
            nodeInfo->setKeyValue("expression", getExpression(s->expression.get(), xsink), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_Try: {
            ASTTryStatement* s = static_cast<ASTTryStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("Try"), xsink);
            nodeInfo->setKeyValue("tryStmt", getStatement(s->tryStmt.get(), xsink), xsink);
            nodeInfo->setKeyValue("catchVar", getExpression(s->catchVar.get(), xsink), xsink);
            nodeInfo->setKeyValue("catchStmt", getStatement(s->catchStmt.get(), xsink), xsink);
            break;
        }
        case ASTStatement::Kind::ASK_While: {
            ASTWhileStatement* s = static_cast<ASTWhileStatement*>(stmt);
            nodeInfo->setKeyValue("kind", new QoreStringNode("While"), xsink);
            nodeInfo->setKeyValue("condition", getExpression(s->condition.get(), xsink), xsink);
            nodeInfo->setKeyValue("statement", getStatement(s->statement.get(), xsink), xsink);
            break;
        }
        default:
            break;
    }
    if (*xsink)
        return nullptr;
    return nodeInfo.release();
}

QoreListNode* GetNodesInfoQuery::get(ASTTree* tree) {
    if (!tree)
        return nullptr;

    ExceptionSink xsink;
    ReferenceHolder<QoreListNode> lst(new QoreListNode, &xsink);
    if (xsink) {
        lst = nullptr;
        xsink.clear();
        return nullptr;
    }

    for (size_t i = 0, count = tree->nodes.size(); i < count; i++) {
        ReferenceHolder<QoreHashNode> nodeInfo(&xsink);
        ASTNode* node = tree->nodes[i];
        switch (node->getNodeType()) {
            case ANT_Declaration: {
                ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
                nodeInfo = getDeclaration(decl, &xsink);
                break;
            }
            case ANT_Expression: {
                ASTExpression* expr = static_cast<ASTExpression*>(node);
                nodeInfo = getExpression(expr, &xsink);
                break;
            }
            case ANT_Name: {
                ASTName* name = static_cast<ASTName*>(node);
                nodeInfo = getName(name, &xsink);
                break;
            }
            case ANT_ParseOption: {
                ASTParseOption* po = static_cast<ASTParseOption*>(node);
                nodeInfo = getParseOption(po, &xsink);
                break;
            }
            case ANT_Statement: {
                ASTStatement* stmt = static_cast<ASTStatement*>(node);
                nodeInfo = getStatement(stmt, &xsink);
                break;
            }
            case ANT_None:
            default:
                break;
        }
        if (xsink) {
            lst = nullptr;
            xsink.clear();
            return nullptr;
        }
        if (nodeInfo) {
            lst->push(nodeInfo.release());
            if (xsink) {
                lst = nullptr;
                xsink.clear();
                return nullptr;
            }
        }
    }

    return lst.release();
}
