/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AstPrinter.cpp

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

#include "AstPrinter.h"

#include "AstTreePrinter.h"
#include "ast/AST.h"

void AstPrinter::printAssignmentExpression(std::ostream& os, ASTAssignmentExpression* ae) {
    if (!ae)
        return;
    if (ae->left->getKind() == ASTExpression::Kind::AEK_Decl) {
        printDeclExpression(os, static_cast<ASTDeclExpression*>(ae->left.get()));
    }
    os << " = ";
    // TODO
}

void AstPrinter::printDeclExpression(std::ostream& os, ASTDeclExpression* de) {
    if (!de)
        return;
    ASTDeclaration* decl = de->declaration.get();
    if (!decl)
        return;
    if (decl->getKind() == ASTDeclaration::Kind::ADK_Variable)
        printVariableSignature(os, static_cast<ASTVariableDeclaration*>(decl));
    else if (decl->getKind() == ASTDeclaration::Kind::ADK_Constant)
        printConstantSignature(os, static_cast<ASTConstantDeclaration*>(decl));
}

void AstPrinter::printListExpression(std::ostream& os, ASTListExpression* le) {
    if (!le)
        return;
    for (size_t i = 0, count = le->elements.size(); i < count; i++) {
        if (i > 0)
            os << ", ";
        ASTExpression* expr = le->elements[i];
        if (expr->getKind() == ASTExpression::Kind::AEK_Decl)
            printDeclExpression(os, static_cast<ASTDeclExpression*>(expr));
        else if (expr->getKind() == ASTExpression::Kind::AEK_List)
            printListExpression(os, static_cast<ASTListExpression*>(expr));
        else if (expr->getKind() == ASTExpression::Kind::AEK_Assignment)
            printAssignmentExpression(os, static_cast<ASTAssignmentExpression*>(expr));
    }
}

void AstPrinter::printClassSignature(std::ostream& os, ASTClassDeclaration* d) {
    if (!d)
        return;
    AstTreePrinter::printModifiers(os, d->modifiers, 0, true);
    os << "class " << d->name.name;
}

void AstPrinter::printConstantSignature(std::ostream& os, ASTConstantDeclaration* d) {
    if (!d)
        return;
    AstTreePrinter::printModifiers(os, d->modifiers, 0, true);
    os << "const " << d->name.name << " = ";

}

void AstPrinter::printFunctionSignature(std::ostream& os, ASTFunctionDeclaration* d) {
    if (!d)
        return;
    AstTreePrinter::printModifiers(os, d->modifiers, 0, true);
    if (d->returnType) {
        ASTNameExpression* ret;
        if (d->returnType->getKind() == ASTExpression::Kind::AEK_Returns)
            ret = static_cast<ASTReturnsExpression*>(d->returnType.get())->typeName.get();
        else
            ret = static_cast<ASTNameExpression*>(d->returnType.get());
        if (ret)
            os << " " << ret->name.name << " ";
    }
    os << d->name.name << "(";
    if (d->params) {
        if (d->params->getKind() == ASTExpression::Kind::AEK_Decl)
            printDeclExpression(os, static_cast<ASTDeclExpression*>(d->params.get()));
        else if (d->params->getKind() == ASTExpression::Kind::AEK_List)
            printListExpression(os, static_cast<ASTListExpression*>(d->params.get()));
    }
    os << ")";
}

void AstPrinter::printVariableSignature(std::ostream& os, ASTVariableDeclaration* d) {
    if (!d)
        return;
    AstTreePrinter::printModifiers(os, d->modifiers, 0, true);
    if (!d->typeName.name.empty())
        os << d->typeName.name;
    os << " " << d->name.name;
}