/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  SymbolInfoFixes.h

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

#ifndef _QLS_QUERIES_SYMBOLINFOFIXES_H
#define _QLS_QUERIES_SYMBOLINFOFIXES_H

#include <vector>

#include "ast/ASTSymbolInfo.h"

class ASTTree;

class SymbolInfoFixes {
public:
    SymbolInfoFixes() = delete;
    SymbolInfoFixes(const SymbolInfoFixes& other) = delete;

    //! Fix symbol infos for the passed symbol info vector.
    /**
        @param tree AST tree of the symbols
        @param vec vector of symbol infos to fix
        @param bareNames whether to return bare symbol names (without namespace and class prefixes)
     */
    static void fixSymbolInfos(ASTTree* tree, std::vector<ASTSymbolInfo>& vec, bool bareNames);

private:
    static void fixClassInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames);
    static void fixConstantInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames);
    static void fixFunctionInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames);
    static void fixHashDeclInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames);
    static void fixHashMemberInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames);
    static void fixVariableInfo(ASTSymbolInfo& si, std::vector<ASTNode*>* nodes, bool bareNames);
};

#endif // _QLS_QUERIES_SYMBOLINFOFIXES_H
