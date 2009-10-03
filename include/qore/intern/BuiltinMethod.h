/*
 BuiltinMethod.h
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
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

#ifndef _QORE_BUILTINMETHOD_H

#define _QORE_BUILTINMETHOD_H

#include <qore/intern/Function.h>
#include <qore/QoreReferenceCounter.h>

class BuiltinMethod : public BuiltinFunction, public QoreReferenceCounter {
   protected:
      DLLLOCAL ~BuiltinMethod() {}

   public:
      QoreClass *myclass;

      DLLLOCAL BuiltinMethod(QoreClass *c, const char *nme, q_method2_t m) : BuiltinFunction(nme, m, QDOM_DEFAULT), myclass(c) {}
      DLLLOCAL BuiltinMethod(QoreClass *c, const char *nme, q_method_t m) : BuiltinFunction(nme, m, QDOM_DEFAULT), myclass(c) {}
      DLLLOCAL BuiltinMethod(QoreClass *c, q_constructor_t m) : BuiltinFunction(m, QDOM_DEFAULT), myclass(c) {}
      DLLLOCAL BuiltinMethod(QoreClass *c, q_constructor2_t m) : BuiltinFunction(m, QDOM_DEFAULT), myclass(c) {}
      DLLLOCAL BuiltinMethod(QoreClass *c, q_system_constructor_t m) : BuiltinFunction(m, QDOM_DEFAULT), myclass(c) {}
      DLLLOCAL BuiltinMethod(QoreClass *c, q_system_constructor2_t m) : BuiltinFunction(m, QDOM_DEFAULT), myclass(c) {}
      DLLLOCAL BuiltinMethod(QoreClass *c, q_destructor_t m) : BuiltinFunction(m, QDOM_DEFAULT), myclass(c) {}
      DLLLOCAL BuiltinMethod(QoreClass *c, q_destructor2_t m) : BuiltinFunction(m, QDOM_DEFAULT), myclass(c) {}
      DLLLOCAL BuiltinMethod(QoreClass *c, q_copy_t m) : BuiltinFunction(m, QDOM_DEFAULT), myclass(c) {}
      DLLLOCAL BuiltinMethod(QoreClass *c, q_copy2_t m) : BuiltinFunction(m, QDOM_DEFAULT), myclass(c) {}
      DLLLOCAL BuiltinMethod(QoreClass *c, q_delete_blocker_t m) : BuiltinFunction(m), myclass(c) {}

      // for static methods
      DLLLOCAL BuiltinMethod(QoreClass *c, const char *n_name, q_func_t m) : BuiltinFunction(n_name, m, QDOM_DEFAULT), myclass(c) {}
      DLLLOCAL BuiltinMethod(QoreClass *c, const char *n_name, q_static_method2_t m) : BuiltinFunction(n_name, m, QDOM_DEFAULT), myclass(c) {}

      DLLLOCAL void deref();
};

#endif
