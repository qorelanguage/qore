/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  SymbolInfoFixes.cpp

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

#include "queries/SymbolInfoFixes.h"

#include <memory>

#include "ast/AST.h"
#include "queries/FindNodeAndParentsQuery.h"

void SymbolInfoFixes::fixClassInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames) {
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

void SymbolInfoFixes::fixConstantInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames) {
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

void SymbolInfoFixes::fixFunctionInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames) {
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

void SymbolInfoFixes::fixHashDeclInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames) {
    if (bareNames)
        return;

    size_t nextNode = 0;
    size_t nodeCount = nodes->size();
    for (; nextNode < nodeCount; nextNode++) {
        ASTNode* node = nodes->at(nextNode);
        if (node->getNodeType() != ANT_Declaration)
            continue;
        ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
        if (decl->getKind() == ASTDeclaration::Kind::ADK_Hash) {
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

void SymbolInfoFixes::fixHashMemberInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames) {
    if (bareNames)
        return;

    size_t nextNode = 0;
    size_t nodeCount = nodes->size();
    for (; nextNode < nodeCount; nextNode++) {
        ASTNode* node = nodes->at(nextNode);
        if (node->getNodeType() != ANT_Declaration)
            continue;
        ASTDeclaration* decl = static_cast<ASTDeclaration*>(node);
        if (decl->getKind() == ASTDeclaration::Kind::ADK_HashMember) {
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
            else if (decl->getKind() == ASTDeclaration::Kind::ADK_Hash)
                si.name.insert(0, static_cast<ASTHashDeclaration*>(decl)->name.name + "::");
            else if (decl->getKind() == ASTDeclaration::Kind::ADK_Namespace)
                si.name.insert(0, static_cast<ASTNamespaceDeclaration*>(decl)->name.name + "::");
        }
    }
}

void SymbolInfoFixes::fixVariableInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames) {
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

void SymbolInfoFixes::fixSymbolInfos(ASTTree* tree, std::vector<ASTSymbolInfo>& vec, bool bareNames) {
    for (size_t i = 0, count = vec.size(); i < count; i++) {
        ASTSymbolInfo& si = vec[i];

        // Do this only for classes, constants, functions, variables, hashdecl and hash members.
        if (!(si.kind == ASYK_Constructor ||
              si.kind == ASYK_Function ||
              si.kind == ASYK_Method ||
              si.kind == ASYK_Variable ||
              si.kind == ASYK_Class ||
              si.kind == ASYK_Constant ||
              si.kind == ASYK_Field ||
              si.kind == ASYK_Interface))
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
            case ASYK_Constructor:
            case ASYK_Function:
            case ASYK_Method:
                fixFunctionInfo(si, nodes.get(), bareNames); break;
            case ASYK_Field:
                fixHashMemberInfo(si, nodes.get(), bareNames); break;
            case ASYK_Interface:
                fixHashDeclInfo(si, nodes.get(), bareNames); break;
            case ASYK_Variable:
                fixVariableInfo(si, nodes.get(), bareNames); break;
            default:
                break;
        }
    }
}
