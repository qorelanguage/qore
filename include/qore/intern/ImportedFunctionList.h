/*
 ImportedFunctionList.h
 
 Qore Programming Language
 
 Copyright 2003 - 2010 David Nichols
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _QORE_IMPORTEDFUNCTIONLIST_H

#define _QORE_IMPORTEDFUNCTIONLIST_H

#include <qore/common.h>

#include <map>

class ImportedFunctionNode {
public:
   QoreProgram *pgm;
   UserFunction *func;
   
   DLLLOCAL ImportedFunctionNode(QoreProgram *p, UserFunction *u);
};

typedef std::map<const char *, ImportedFunctionNode *, class ltstr> ifn_map_t;

class ImportedFunctionList : public ifn_map_t {
public:
   DLLLOCAL ImportedFunctionList();
   DLLLOCAL ~ImportedFunctionList();
   DLLLOCAL void add(QoreProgram *pgm, UserFunction *func);
   DLLLOCAL UserFunction *find(const char *name) const;
   DLLLOCAL ImportedFunctionNode *findNode(const char *name) const;
};

#endif
