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

#include "queries/FindSymbolsQuery.h"

static void copyLwr(const char* src, char* dest, size_t n) {
    size_t i = 0;
    for (; i < n && src[i]; i++)
        dest[i] = (src[i] > 64 && src[i] < 91) ? src[i]+32 : src[i];
    if (i < n)
        dest[i] = '\0';
}

bool FindMatchingSymbolsQuery::matches(const std::string& name, const std::string& query) {
    const char* q = query.c_str();
    char first[3];
    first[0] = q[0];
    first[1] = (q[0] > 64 && q[0] < 91) ? q[0]+32 : ((q[0] > 93 && q[0] < 123) ? q[0]-32 : '\0');
    first[2] = '\0';

    // Find first character of the query (case insensitive).
    size_t pos = name.find_first_of(first);
    while (pos != std::string::npos) {
        char lwrName[512];
        char lwrQuery[512];
        size_t querySize = query.size() % 512;

        // Compare lower-case name and query.
        copyLwr(name.c_str()+pos, lwrName, querySize);
        copyLwr(query.c_str(), lwrQuery, querySize);
        if (!strncmp(lwrName, lwrQuery, querySize))
            return true;

        // Search next characters behind this occurence.
        pos = name.find_first_of(first, pos+1);
    }
    return false;
}

void FindMatchingSymbolsQuery::filterByQuery(std::vector<ASTSymbolInfo>* vec, const std::string& query) {
    if (query.empty() || vec->empty())
        return;
    for (int i = vec->size()-1; i >= 0; i--) {
        if (!matches(vec->at(i).name, query))
            vec->erase(vec->begin()+i);
    }
}

std::vector<ASTSymbolInfo>* FindMatchingSymbolsQuery::find(ASTTree* tree, const std::string& query) {
    if (!tree)
        return nullptr;

    std::unique_ptr<std::vector<ASTSymbolInfo> > vec(FindSymbolsQuery::find(tree));
    filterByQuery(vec.get(), query);

    return vec.release();
}

std::vector<ASTSymbolInfo>* FindMatchingSymbolsQuery::find(const std::vector<ASTSymbolInfo>* symbols, const std::string& query) {
    std::unique_ptr<std::vector<ASTSymbolInfo> > vec(new std::vector<ASTSymbolInfo>(*symbols));
    filterByQuery(vec.get(), query);

    return vec.release();
}
