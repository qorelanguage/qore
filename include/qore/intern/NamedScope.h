/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  NamedScope.h

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

  NamedScopes are children of a program object.  there is a parse
  lock per program object to ensure that objects are added (or backed out)
  atomically per program object.  All the objects referenced here should 
  be safe to read & copied at all times.  They will only be deleted when the
  program object is deleted (except the pending structures, which will be
  deleted any time there is a parse error, together with all other
  pending structures)

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

#ifndef QORE_NAMEDSCOPE_H

#define QORE_NAMEDSCOPE_H

#include <vector>
#include <string>

// for parsing namespace/class scope resolution
class NamedScope {
private:
   typedef std::vector<std::string> nslist_t;

   bool del;

   DLLLOCAL void init();
      
public:
   char *ostr;
   nslist_t strlist;

   DLLLOCAL NamedScope(char *str) : del(true), ostr(str) {
      init();
   }

   DLLLOCAL NamedScope(const char *str) : del(false), ostr((char *)str) {
      init();
   }

   // takes all values from and deletes the argument
   DLLLOCAL NamedScope(NamedScope *ns) : del(ns->del), ostr(ns->ostr), strlist(ns->strlist) {
      ns->ostr = 0;
      delete ns;
   }

   DLLLOCAL NamedScope(const NamedScope &old) : del(true), ostr(strdup(old.ostr)), strlist(old.strlist) {
   }

   DLLLOCAL ~NamedScope() {
      clear();
   }

   DLLLOCAL void clear() {
      if (ostr && del)
         free(ostr);
      strlist.clear();
      ostr = 0;
      del = false;
   }

   DLLLOCAL const char *getIdentifier() const {
      return strlist[strlist.size() - 1].c_str();
   }

   DLLLOCAL const std::string &getIdentifierStr() const {
      return strlist[strlist.size() - 1];
   }

   DLLLOCAL unsigned size() const {
      return strlist.size(); 
   }

   DLLLOCAL NamedScope *copy() const;
   DLLLOCAL void fixBCCall();

   DLLLOCAL char *takeName() {
      char *rv = ostr;
      ostr = 0;
      clear();
      return rv;
   }
};

#endif // QORE_NAMEDSCOPE_H
