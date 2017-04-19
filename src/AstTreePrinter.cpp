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

#include "AstTreePrinter.h"

#include "ast/AST.h"

static void printIndent(std::ostream& os, int indent) {
    for (int i = 0; i < indent; i++)
        os << "  ";
}

static void printString(std::ostream& os, const char* str, int indent) {
    printIndent(os, indent);
    os << str;
}

void AstTreePrinter::printModifiers(std::ostream& os, ASTModifiers mods, int indent) {
    if (mods.empty())
        return;
    printIndent(os, indent);
    os << "modifiers: ";
    if (mods.contains(AM_Abstract))
        os << " abstract";
    if (mods.contains(AM_Deprecated))
        os << " deprecated";
    if (mods.contains(AM_Final))
        os << " final";
    if (mods.contains(AM_Static))
        os << " static";
    if (mods.contains(AM_Synchronized))
        os << " synchronized";
    if (mods.contains(AM_Our))
        os << " our";
    if (mods.contains(AM_My))
        os << " my";
    if (mods.contains(AM_Public))
        os << " public";
    if (mods.contains(AM_Private))
        os << " private";
    if (mods.contains(AM_PrivateHierarchy))
        os << " private:hierarchy";
    if (mods.contains(AM_PrivateInternal))
        os << " private:internal";
    os << std::endl;
}

void AstTreePrinter::printDeclaration(std::ostream& os, ASTDeclaration* decl, int indent) {
    printIndent(os, indent);
    if (!decl) {
        os << "<null declaration>" << std::endl;
        return;
    }

    switch (decl->getKind()) {
        case ASTDeclaration::Kind::ADK_Class: {
            ASTClassDeclaration* d = static_cast<ASTClassDeclaration*>(decl);
            os << "ClassDecl " << d << std::endl;
            printModifiers(os, d->modifiers, indent+1);
            printName(os, d->name, indent+1);
            printString(os, "inherits:\n", indent+1);
            for (unsigned int i = 0, count = d->inherits.size(); i < count; i++)
                printDeclaration(os, d->inherits[i], indent+2);
            printString(os, "declarations:\n", indent+1);
            for (unsigned int i = 0, count = d->declarations.size(); i < count; i++)
                printDeclaration(os, d->declarations[i], indent+2);
            break;
        }
        case ASTDeclaration::Kind::ADK_Closure: {
            ASTClosureDeclaration* d = static_cast<ASTClosureDeclaration*>(decl);
            os << "ClosureDecl " << d << std::endl;
            printModifiers(os, d->modifiers, indent+1);
            if (d->returnType.get()) {
                printString(os, "returnType:\n", indent+1);
                printExpression(os, d->returnType.get(), indent+2);
            }
            if (d->params.get()) {
                printString(os, "params:\n", indent+1);
                printExpression(os, d->params.get(), indent+2);
            }
            printString(os, "body:\n", indent+1);
            printStatement(os, d->body.get(), indent+2);
            break;
        }
        case ASTDeclaration::Kind::ADK_Constant: {
            ASTConstantDeclaration* d = static_cast<ASTConstantDeclaration*>(decl);
            os << "ConstantDecl " << d << std::endl;
            printModifiers(os, d->modifiers, indent+1);
            printName(os, d->name, indent+1);
            printExpression(os, d->value.get(), indent+1);
            break;
        }
        case ASTDeclaration::Kind::ADK_Function: {
            ASTFunctionDeclaration* d = static_cast<ASTFunctionDeclaration*>(decl);
            os << "FunctionDecl " << d << std::endl;
            printModifiers(os, d->modifiers, indent+1);
            printName(os, d->name, indent+1);
            if (d->returnType.get()) {
                printString(os, "returnType:\n", indent+1);
                printExpression(os, d->returnType.get(), indent+2);
            }
            if (d->params.get()) {
                printString(os, "params:\n", indent+1);
                printExpression(os, d->params.get(), indent+2);
            }
            if (d->inits.get()) {
                printString(os, "inits:\n", indent+1);
                printExpression(os, d->inits.get(), indent+2);
            }
            if (d->body.get()) {
                printString(os, "body:\n", indent+1);
                printStatement(os, d->body.get(), indent+2);
            }
            break;
        }
        case ASTDeclaration::Kind::ADK_MemberGroup: {
            ASTMemberGroupDeclaration* d = static_cast<ASTMemberGroupDeclaration*>(decl);
            os << "MemberGroupDecl " << d << std::endl;
            printModifiers(os, d->modifiers, indent+1);
            if (d->members.size() > 0) {
                for (unsigned int i = 0, count = d->members.size(); i < count; i++)
                    printExpression(os, d->members[i], indent+1);
            }
            else {
                printString(os, "<empty>\n", indent+1);
            }
            break;
        }
        case ASTDeclaration::Kind::ADK_Namespace: {
            ASTNamespaceDeclaration* d = static_cast<ASTNamespaceDeclaration*>(decl);
            os << "NamespaceDecl " << d << std::endl;
            printModifiers(os, d->modifiers, indent+1);
            if (d->declarations.size() > 0) {
                for (unsigned int i = 0, count = d->declarations.size(); i < count; i++)
                    printDeclaration(os, d->declarations[i], indent+1);
            }
            else {
                printString(os, "<empty>\n", indent+1);
            }
            break;
        }
        case ASTDeclaration::Kind::ADK_Superclass: {
            ASTSuperclassDeclaration* d = static_cast<ASTSuperclassDeclaration*>(decl);
            os << "SuperclassDecl " << d << std::endl;
            printModifiers(os, d->modifiers, indent+1);
            printName(os, d->name, indent+1);
            break;
        }
        case ASTDeclaration::Kind::ADK_Variable: {
            ASTVariableDeclaration* d = static_cast<ASTVariableDeclaration*>(decl);
            os << "VariableDecl " << d << std::endl;
            printModifiers(os, d->modifiers, indent+1);
            printName(os, d->name, indent+1);
            printName(os, d->typeName, indent+1, true, "typeName: ");
            break;
        }
        case ASTDeclaration::Kind::ADK_VarList: {
            ASTVarListDeclaration* d = static_cast<ASTVarListDeclaration*>(decl);
            os << "VarListDecl " << d << std::endl;
            printModifiers(os, d->modifiers, indent+1);
            if (d->variables.get()) {
                printExpression(os, d->variables.get(), indent+1);
            }
            else {
                printString(os, "<empty>\n", indent+1);
            }
            break;
        }
        default:
            break;
    }
}

void AstTreePrinter::printExpression(std::ostream& os, ASTExpression* expr, int indent) {
    printIndent(os, indent);
    if (!expr) {
        os << "<null expression>" << std::endl;
        return;
    }
    switch (expr->getKind()) {
        case ASTExpression::Kind::AEK_Access: {
            ASTAccessExpression* e = static_cast<ASTAccessExpression*>(expr);
            os << "AccessExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Assignment: {
            ASTAssignmentExpression* e = static_cast<ASTAssignmentExpression*>(expr);
            os << "AssignmentExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Backquote: {
            ASTBackquoteExpression* e = static_cast<ASTBackquoteExpression*>(expr);
            os << "BackquoteExpr " << e << "; cmd=`" << e->command << "`" << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Binary: {
            ASTBinaryExpression* e = static_cast<ASTBinaryExpression*>(expr);
            os << "BinaryExpr " << e << std::endl;
            printString(os, "left:\n", indent+1);
            printExpression(os, e->left.get(), indent+2);
            printString(os, "op:", indent+1);
            printOperator(os, e->op, 0, true);
            printString(os, "right:\n", indent+1);
            printExpression(os, e->right.get(), indent+2);
            break;
        }
        case ASTExpression::Kind::AEK_Call: {
            ASTCallExpression* e = static_cast<ASTCallExpression*>(expr);
            os << "CallExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Case: {
            ASTCaseExpression* e = static_cast<ASTCaseExpression*>(expr);
            os << "CaseExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Cast: {
            ASTCastExpression* e = static_cast<ASTCastExpression*>(expr);
            os << "CastExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Closure: {
            ASTClosureExpression* e = static_cast<ASTClosureExpression*>(expr);
            os << "ClosureExpr " << e << std::endl;
            printDeclaration(os, e->closure.get(), indent+1);
            break;
        }
        case ASTExpression::Kind::AEK_ConstrInit: {
            ASTConstrInitExpression* e = static_cast<ASTConstrInitExpression*>(expr);
            os << "ConstrInitExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_ContextMod: {
            ASTContextModExpression* e = static_cast<ASTContextModExpression*>(expr);
            os << "ContextModExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_ContextRow: {
            os << "ContextRowExpr " << expr << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Decl: {
            ASTDeclExpression* e = static_cast<ASTDeclExpression*>(expr);
            os << "DeclExpr " << e << std::endl;
            printDeclaration(os, e->declaration.get(), indent+1);
            break;
        }
        case ASTExpression::Kind::AEK_Find: {
            ASTFindExpression* e = static_cast<ASTFindExpression*>(expr);
            os << "FindExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Hash: {
            ASTHashExpression* e = static_cast<ASTHashExpression*>(expr);
            os << "HashExpr " << e << std::endl;
            if (e->elements.size() > 0) {
                for (unsigned int i = 0, count = e->elements.size(); i < count; i++)
                    printExpression(os, e->elements[i], indent+1);
            }
            else {
                printString(os, "<empty>\n", indent+1);
            }
            break;
        }
        case ASTExpression::Kind::AEK_HashElement: {
            ASTHashElementExpression* e = static_cast<ASTHashElementExpression*>(expr);
            os << "HashElementExpr " << e << std::endl;
            printString(os, "key:\n", indent+1);
            printExpression(os, e->key.get(), indent+2);
            printString(os, "value:\n", indent+1);
            printExpression(os, e->value.get(), indent+2);
            break;
        }
        case ASTExpression::Kind::AEK_ImplicitArg: {
            ASTImplicitArgExpression* e = static_cast<ASTImplicitArgExpression*>(expr);
            os << "ImplicitArgExpr " << e << "; offset=" << e->offset << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_ImplicitElem: {
            os << "ImplicitElemExpr " << expr << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Index: {
            ASTIndexExpression* e = static_cast<ASTIndexExpression*>(expr);
            os << "IndexExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_List: {
            ASTListExpression* e = static_cast<ASTListExpression*>(expr);
            os << "ListExpr " << e << std::endl;
            if (e->elements.size() > 0) {
                for (unsigned int i = 0, count = e->elements.size(); i < count; i++)
                    printExpression(os, e->elements[i], indent+1);
            }
            else {
                printString(os, "<empty>\n", indent+1);
            }
            break;
        }
        case ASTExpression::Kind::AEK_Literal: {
            ASTLiteralExpression* e = static_cast<ASTLiteralExpression*>(expr);
            os << "LiteralExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Name: {
            ASTNameExpression* e = static_cast<ASTNameExpression*>(expr);
            os << "NameExpr " << e;
            printName(os, e->name, 0, false, ": ");
            os << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Regex: {
            ASTRegexExpression* e = static_cast<ASTRegexExpression*>(expr);
            os << "RegexExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_RegexSubst: {
            ASTRegexSubstExpression* e = static_cast<ASTRegexSubstExpression*>(expr);
            os << "RegexSubstExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_RegexTrans: {
            ASTRegexTransExpression* e = static_cast<ASTRegexTransExpression*>(expr);
            os << "RegexTransExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Returns: {
            ASTReturnsExpression* e = static_cast<ASTReturnsExpression*>(expr);
            os << "ReturnsExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_SwitchBody: {
            ASTSwitchBodyExpression* e = static_cast<ASTSwitchBodyExpression*>(expr);
            os << "SwitchBodyExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Ternary: {
            ASTTernaryExpression* e = static_cast<ASTTernaryExpression*>(expr);
            os << "TernaryExpr " << e << std::endl;
            break;
        }
        case ASTExpression::Kind::AEK_Unary: {
            ASTUnaryExpression* e = static_cast<ASTUnaryExpression*>(expr);
            os << "UnaryExpr " << e << std::endl;
            printString(os, "op:", indent+1);
            printOperator(os, e->op, 0, true);
            printExpression(os, e->expression.get(), indent+1);
            break;
        }
        default:
            break;
    }
}

void AstTreePrinter::printName(std::ostream& os, ASTName& name, int indent, bool newline, const char* prefix) {
    printIndent(os, indent);
    os << prefix << name.name;
    if (newline)
        os << std::endl;
}

void AstTreePrinter::printStatement(std::ostream& os, ASTStatement* stmt, int indent) {
    printIndent(os, indent);
    if (!stmt) {
        os << "<null statement>" << std::endl;
        return;
    }
    os << "statement" << std::endl;
}

void AstTreePrinter::printOperator(std::ostream& os, ASTOperator op, int indent, bool newline) {
    printIndent(os, indent);
    switch (op.op) {
        case AOK_None: os << "None"; if (newline) os << std::endl; break;
        case AOK_Background: os << "Background"; if (newline) os << std::endl; break;
        case AOK_Chomp: os << "Chomp"; if (newline) os << std::endl; break;
        case AOK_Delete: os << "Delete"; if (newline) os << std::endl; break;
        case AOK_Elements: os << "Elements"; if (newline) os << std::endl; break;
        case AOK_Exists: os << "Exists"; if (newline) os << std::endl; break;
        case AOK_Extract: os << "Extract"; if (newline) os << std::endl; break;
        case AOK_Foldl: os << "Foldl"; if (newline) os << std::endl; break;
        case AOK_Foldr: os << "Foldr"; if (newline) os << std::endl; break;
        case AOK_Instanceof: os << "Instanceof"; if (newline) os << std::endl; break;
        case AOK_Keys: os << "Keys"; if (newline) os << std::endl; break;
        case AOK_Map: os << "Map"; if (newline) os << std::endl; break;
        case AOK_New: os << "New"; if (newline) os << std::endl; break;
        case AOK_Pop: os << "Pop"; if (newline) os << std::endl; break;
        case AOK_Push: os << "Push"; if (newline) os << std::endl; break;
        case AOK_Remove: os << "Remove"; if (newline) os << std::endl; break;
        case AOK_Select: os << "Select"; if (newline) os << std::endl; break;
        case AOK_Shift: os << "Shift"; if (newline) os << std::endl; break;
        case AOK_Splice: os << "Splice"; if (newline) os << std::endl; break;
        case AOK_Trim: os << "Trim"; if (newline) os << std::endl; break;
        case AOK_Unshift: os << "Unshift"; if (newline) os << std::endl; break;
        case AOK_Reference: os << "Reference"; if (newline) os << std::endl; break;
        case AOK_PreIncrement: os << "PreIncrement"; if (newline) os << std::endl; break;
        case AOK_PostIncrement: os << "PostIncrement"; if (newline) os << std::endl; break;
        case AOK_PreDecrement: os << "PreDecrement"; if (newline) os << std::endl; break;
        case AOK_PostDecrement: os << "PostDecrement"; if (newline) os << std::endl; break;
        case AOK_Plus: os << "Plus"; if (newline) os << std::endl; break;
        case AOK_Minus: os << "Minus"; if (newline) os << std::endl; break;
        case AOK_Multiply: os << "Multiply"; if (newline) os << std::endl; break;
        case AOK_Divide: os << "Divide"; if (newline) os << std::endl; break;
        case AOK_Modulo: os << "Modulo"; if (newline) os << std::endl; break;
        case AOK_UnaryPlus: os << "UnaryPlus"; if (newline) os << std::endl; break;
        case AOK_UnaryMinus: os << "UnaryMinus"; if (newline) os << std::endl; break;
        case AOK_BinaryAnd: os << "BinaryAnd"; if (newline) os << std::endl; break;
        case AOK_BinaryOr: os << "BinaryOr"; if (newline) os << std::endl; break;
        case AOK_BinaryXor: os << "BinaryXor"; if (newline) os << std::endl; break;
        case AOK_BinaryNot: os << "BinaryNot"; if (newline) os << std::endl; break;
        case AOK_ShiftLeft: os << "ShiftLeft"; if (newline) os << std::endl; break;
        case AOK_ShiftRight: os << "ShiftRight"; if (newline) os << std::endl; break;
        case AOK_Assignment: os << "Assignment"; if (newline) os << std::endl; break;
        case AOK_PlusEquals: os << "PlusEquals"; if (newline) os << std::endl; break;
        case AOK_MinusEquals: os << "MinusEquals"; if (newline) os << std::endl; break;
        case AOK_MultiplyEquals: os << "MultiplyEquals"; if (newline) os << std::endl; break;
        case AOK_DivideEquals: os << "DivideEquals"; if (newline) os << std::endl; break;
        case AOK_ModuloEquals: os << "ModuloEquals"; if (newline) os << std::endl; break;
        case AOK_AndEquals: os << "AndEquals"; if (newline) os << std::endl; break;
        case AOK_OrEquals: os << "OrEquals"; if (newline) os << std::endl; break;
        case AOK_XorEquals: os << "XorEquals"; if (newline) os << std::endl; break;
        case AOK_ShiftLeftEquals: os << "ShiftLeftEquals"; if (newline) os << std::endl; break;
        case AOK_ShiftRightEquals: os << "ShiftRightEquals"; if (newline) os << std::endl; break;
        case AOK_Equals: os << "Equals"; if (newline) os << std::endl; break;
        case AOK_NotEquals: os << "NotEquals"; if (newline) os << std::endl; break;
        case AOK_Comparison: os << "Comparison"; if (newline) os << std::endl; break;
        case AOK_GreaterThan: os << "GreaterThan"; if (newline) os << std::endl; break;
        case AOK_GreaterThanOrEquals: os << "GreaterThanOrEquals"; if (newline) os << std::endl; break;
        case AOK_LessThan: os << "LessThan"; if (newline) os << std::endl; break;
        case AOK_LessThanOrEquals: os << "LessThanOrEquals"; if (newline) os << std::endl; break;
        case AOK_AbsoluteEquals: os << "AbsoluteEquals"; if (newline) os << std::endl; break;
        case AOK_AbsoluteNotEquals: os << "AbsoluteNotEquals"; if (newline) os << std::endl; break;
        case AOK_LogicalAnd: os << "LogicalAnd"; if (newline) os << std::endl; break;
        case AOK_LogicalOr: os << "LogicalOr"; if (newline) os << std::endl; break;
        case AOK_LogicalNot: os << "LogicalNot"; if (newline) os << std::endl; break;
        case AOK_NullCoalesce: os << "NullCoalesce"; if (newline) os << std::endl; break;
        case AOK_ValueCoalesce: os << "ValueCoalesce"; if (newline) os << std::endl; break;
        case AOK_RegexMatch: os << "RegexMatch"; if (newline) os << std::endl; break;
        case AOK_RegexNotMatch: os << "RegexNotMatch"; if (newline) os << std::endl; break;
        case AOK_BrokenLogicalOr: os << "BrokenLogicalOr"; if (newline) os << std::endl; break;
        case AOK_BrokenBinaryOr: os << "BrokenBinaryOr"; if (newline) os << std::endl; break;
        case AOK_BrokenBinaryXor: os << "BrokenBinaryXor"; if (newline) os << std::endl; break;
        case AOK_WS_LOGICAL_LE: os << "WS_LOGICAL_LE"; if (newline) os << std::endl; break;
        case AOK_WS_LOGICAL_GE: os << "WS_LOGICAL_GE"; if (newline) os << std::endl; break;
        case AOK_WS_LOGICAL_NE: os << "WS_LOGICAL_NE"; if (newline) os << std::endl; break;
        case AOK_WS_LOGICAL_EQ: os << "WS_LOGICAL_EQ"; if (newline) os << std::endl; break;
        case AOK_WS_ABSOLUTE_EQ: os << "WS_ABSOLUTE_EQ"; if (newline) os << std::endl; break;
        case AOK_WS_ABSOLUTE_NE: os << "WS_ABSOLUTE_NE"; if (newline) os << std::endl; break;
        case AOK_WS_LOGICAL_CMP: os << "WS_LOGICAL_CMP"; if (newline) os << std::endl; break;
        case AOK_WS_LOGICAL_AND: os << "WS_LOGICAL_AND"; if (newline) os << std::endl; break;
        case AOK_WS_LOGICAL_OR: os << "WS_LOGICAL_OR"; if (newline) os << std::endl; break;
        case AOK_WS_BROKEN_LOGICAL_OR: os << "WS_BROKEN_LOGICAL_OR"; if (newline) os << std::endl; break;
        case AOK_WS_SHIFT_RIGHT: os << "WS_SHIFT_RIGHT"; if (newline) os << std::endl; break;
        case AOK_WS_SHIFT_LEFT: os << "WS_SHIFT_LEFT"; if (newline) os << std::endl; break;
        case AOK_WS_PLUS_EQUALS: os << "WS_PLUS_EQUALS"; if (newline) os << std::endl; break;
        case AOK_WS_MINUS_EQUALS: os << "WS_MINUS_EQUALS"; if (newline) os << std::endl; break;
        case AOK_WS_AND_EQUALS: os << "WS_AND_EQUALS"; if (newline) os << std::endl; break;
        case AOK_WS_OR_EQUALS: os << "WS_OR_EQUALS"; if (newline) os << std::endl; break;
        case AOK_WS_MODULO_EQUALS: os << "WS_MODULO_EQUALS"; if (newline) os << std::endl; break;
        case AOK_WS_MULTIPLY_EQUALS: os << "WS_MULTIPLY_EQUALS"; if (newline) os << std::endl; break;
        case AOK_WS_DIVIDE_EQUALS: os << "WS_DIVIDE_EQUALS"; if (newline) os << std::endl; break;
        case AOK_WS_XOR_EQUALS: os << "WS_XOR_EQUALS"; if (newline) os << std::endl; break;
        case AOK_WS_SHIFT_RIGHT_EQUALS: os << "WS_SHIFT_RIGHT_EQUALS"; if (newline) os << std::endl; break;
        case AOK_WS_SHIFT_LEFT_EQUALS: os << "WS_SHIFT_LEFT_EQUALS"; if (newline) os << std::endl; break;
        default: break;
    }
}

void AstTreePrinter::printNode(std::ostream& os, ASTNode* node, int indent) {
    if (!node)
        return;

    switch (node->getNodeType()) {
        case ANT_Declaration: {
            ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
            printDeclaration(os, decl, indent);
            break;
        }
        case ANT_Expression: {
            ASTExpression* expr = static_cast<ASTExpression*>(node);
            printExpression(os, expr, indent);
            break;
        }
        case ANT_Name: {
            ASTName* name = static_cast<ASTName*>(node);
            printName(os, *name, indent);
            break;
        }
        case ANT_Statement: {
            ASTStatement* stmt = static_cast<ASTStatement*>(node);
            printStatement(os, stmt, indent);
            break;
        }
        case ANT_None:
        default:
            break;
    }
}

void AstTreePrinter::printTree(std::ostream& os, ASTTree* tree) {
    if (!tree) {
        os << "no tree to print out\n";
        return;
    }

    for (unsigned int i = 0, count = tree->nodes.size(); i < count; i++)
        printNode(os, tree->nodes[i], 0);
}