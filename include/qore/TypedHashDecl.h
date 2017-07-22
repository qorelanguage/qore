/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  TypedHashDecl.h

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_TYPEDHASHDECL_H

#define _QORE_TYPEDHASHDECL_H

// forward references
class typed_hash_decl_private;

//! typed hash declaration
/** @since %Qore 0.8.13
 */
class TypedHashDecl {
    friend class typed_hash_decl_private;

public:
    DLLEXPORT TypedHashDecl(const char* name);

    DLLEXPORT TypedHashDecl(const TypedHashDecl& old);

    //! returns the type info object for the hashdecl
    DLLEXPORT const QoreTypeInfo* getTypeInfo(bool or_nothing = false) const;

    //! adds an element to a built-in hashdecl
    DLLEXPORT void addMember(const char* name, const QoreTypeInfo* memberTypeInfo, QoreValue init_val);

    DLLEXPORT const char* getName() const;

    DLLEXPORT bool isSystem() const;

protected:
    //! deletes the object and frees all memory
    DLLEXPORT ~TypedHashDecl();

private:
    DLLEXPORT TypedHashDecl(typed_hash_decl_private* p);

    typed_hash_decl_private* priv;
};

//! allows for temporary storage of a TypedHashDecl pointer
/** @since %Qore 0.8.13
 */
class TypedHashDeclHolder {
public:
   //! creates the object
   DLLLOCAL TypedHashDeclHolder(TypedHashDecl* thd) : thd(thd) {
   }

   //! deletes the TypedHashDecl object if still managed
   DLLEXPORT ~TypedHashDeclHolder();

   //! implicit conversion to QoreClass*
   DLLLOCAL operator TypedHashDecl*() const {
      return thd;
   }

   //! releases the QoreClass*
   DLLLOCAL TypedHashDecl* release() {
      auto rv = thd;
      thd = nullptr;
      return rv;
   }

private:
   //! the object being managed
   TypedHashDecl* thd;
};

//! StatInfo hashdecl
DLLEXPORT extern const TypedHashDecl* hashdeclStatInfo;

//! DirStatInfo hashdecl
DLLEXPORT extern const TypedHashDecl* hashdeclDirStatInfo;

//! FilesystemStatInfo hashdecl
DLLEXPORT extern const TypedHashDecl* hashdeclFilesystemStatInfo;

//! DateTimeInfo hashdecl
DLLEXPORT extern const TypedHashDecl* hashdeclDateTimeInfo;

//! IsoWeekInfo hashdecl
DLLEXPORT extern const TypedHashDecl* hashdeclIsoWeekInfo;

#endif
