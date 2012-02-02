/*
 NamedScope.cpp
 
 Qore Programming Language
 
 Copyright 2003 - 2012 David Nichols
 
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

#include <qore/Qore.h>

void NamedScope::fixBCCall() {
   // fix last string pointer
   std::string &str = strlist[strlist.size() - 1];
   str.erase(0, 2);
}

NamedScope *NamedScope::copy() const {
   return new NamedScope(strdup(ostr));
}

void NamedScope::init() {
   const char *str = ostr;

   while (char *p = (char *)strstr(str, "::")) {
      strlist.push_back(std::string(str, (p - str)));
      str = p + 2;
   }
   strlist.push_back(std::string(str));
}
