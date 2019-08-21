/*
    HashDeclList.cpp

    Qore Programming Language

    Copyright (C) 2003 - 2019 Qore Technologies, s.r.o.

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

#include <qore/Qore.h>
#include "qore/intern/HashDeclList.h"
#include "qore/intern/QoreClassList.h"
#include "qore/intern/QoreNamespaceList.h"
#include "qore/intern/typed_hash_decl_private.h"
#include "qore/intern/QoreNamespaceIntern.h"

#include <cassert>

void HashDeclList::remove(hm_qth_t::iterator i) {
    TypedHashDecl* thd = i->second;
    //printd(5, "hdL::remove() this: %p '%s' (%p)\n", this, hd->getName(), hd);
    hm.erase(i);
    typed_hash_decl_private::get(*thd)->deref();
}

void HashDeclList::deleteAll() {
    for (hm_qth_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
        typed_hash_decl_private::get(*i->second)->deref();

    hm.clear();
}

HashDeclList::~HashDeclList() {
    deleteAll();
}

void HashDeclList::addInternal(TypedHashDecl* hd) {
    //printd(5, "hdL::addInternal() this: %p '%s' (%p)\n", this, hd->getName(), hd);

    assert(!find(hd->getName()));
    hm[hd->getName()] = hd;
}

int HashDeclList::add(TypedHashDecl* hd) {
    //printd(5, "hdL::add() this: %p '%s' (%p)\n", this, hd->getName(), hd);

    if (find(hd->getName()))
        return -1;

    hm[hd->getName()] = hd;
    return 0;
}

TypedHashDecl* HashDeclList::find(const char* name) {
    hm_qth_t::iterator i = hm.find(name);
    return i != hm.end() ? i->second : nullptr;
}

const TypedHashDecl* HashDeclList::find(const char *name) const {
    hm_qth_t::const_iterator i = hm.find(name);
    return i != hm.end() ? i->second : nullptr;
}

HashDeclList::HashDeclList(const HashDeclList& old, int64 po, qore_ns_private* ns) {
    //printd(5, "HashDeclList::HashDeclList() this: %p ns: '%s' size: %d\n", this, ns->name.c_str(), (int)old.hm.size());
    for (auto& i : old.hm) {
        //printd(5, "HashDeclList::HashDeclList() this: %p hd: %p '%s' po & PO_NO_INHERIT_USER_HASHDECLS: %s sys: %s pub: %s\n", this, i.second, i.second->getName(), po & PO_NO_INHERIT_USER_HASHDECLS ? "true": "false", i.second->isSystem() ? "true": "false", typed_hash_decl_private::get(*i.second)->isPublic() ? "true": "false");
        if (!i.second->isSystem()) {
            if (po & PO_NO_INHERIT_USER_HASHDECLS || !typed_hash_decl_private::get(*i.second)->isPublic())
                continue;
        }
        else
            if (po & PO_NO_INHERIT_SYSTEM_HASHDECLS)
                continue;

        //printd(5, "HashDeclList::HashDeclList() this: %p copying '%s' %p to target\n", this, i.second->getName(), i.second);
        TypedHashDecl* hd = new TypedHashDecl(*i.second);
        typed_hash_decl_private::get(*hd)->setNamespace(ns);
        addInternal(hd);
    }
}

void HashDeclList::mergeUserPublic(const HashDeclList& old, qore_ns_private* ns) {
    for (auto& i : old.hm) {
        if (!typed_hash_decl_private::get(*i.second)->isUserPublic())
            continue;

        if (find(i.first))
            continue;
        TypedHashDecl* hd = new TypedHashDecl(*i.second);
        typed_hash_decl_private::get(*hd)->setNamespace(ns);
        addInternal(hd);
    }
}

int HashDeclList::importSystemHashDecls(const HashDeclList& source, qore_ns_private* ns, ExceptionSink* xsink) {
    int cnt = 0;
    for (auto& i : source.hm) {
        if (i.second->isSystem()) {
            hm_qth_t::const_iterator ci = hm.find(i.second->getName());
            if (ci != hm.end()) {
                xsink->raiseException("IMPORT-SYSTEM-API-ERROR", "cannot import system hashdecl %s::%s due to the presence of an existing hashdecl with the same name in the target namespace", ns->name.c_str(), ci->second->getName());
                break;
            }
            //printd(5, "HashDeclList::importSystemClasses() this: %p importing %p %s::'%s'\n", this, i->second, ns->name.c_str(), i->second->getName());
            TypedHashDecl* hd = new TypedHashDecl(*i.second);
            typed_hash_decl_private::get(*hd)->setNamespace(ns);
            addInternal(hd);
            ++cnt;
        }
    }
    return cnt;
}

void HashDeclList::parseInit() {
     for (auto& i : hm) {
          //printd(5, "HashDeclList::parseInit() this: %p initializing %p '%s'\n", this, i->second, i->first);
          typed_hash_decl_private::get(*(i.second))->parseInit();
     }
}

void HashDeclList::reset() {
    deleteAll();
}

void HashDeclList::assimilate(HashDeclList& n, qore_ns_private& ns) {
    hm_qth_t::iterator i = n.hm.begin();
    while (i != n.hm.end()) {
        if (ns.classList.find(i->first)) {
           parse_error(*typed_hash_decl_private::get(*i->second)->getParseLocation(), "class '%s' has already been defined in namespace '%s'", i->first, ns.name.c_str());
           n.remove(i);
        }
        else if (ns.hashDeclList.find(i->first)) {
           parse_error(*typed_hash_decl_private::get(*i->second)->getParseLocation(), "hashdecl '%s' has already been defined in namespace '%s'", i->first, ns.name.c_str());
           n.remove(i);
        }
        else if (find(i->first)) {
            parse_error(*typed_hash_decl_private::get(*i->second)->getParseLocation(), "hashdecl '%s' is already pending in namespace '%s'", i->first, ns.name.c_str());
            n.remove(i);
        }
        else {
            //printd(5, "HashDeclList::assimilate() this: %p adding: %p '%s::%s'\n", this, i->second, ns.name.c_str(), i->second->getName());

            // "move" data to new list
            hm[i->first] = i->second;
            typed_hash_decl_private::get(*(i->second))->setNamespace(&ns);
            n.hm.erase(i);
        }
        i = n.hm.begin();
    }
}

bool HashDeclListIterator::isPublic() const {
    return typed_hash_decl_private::get(*i->second)->isPublic();
}

bool HashDeclListIterator::isUserPublic() const {
    return typed_hash_decl_private::get(*i->second)->isUserPublic();
}

bool ConstHashDeclListIterator::isPublic() const {
    return typed_hash_decl_private::get(*i->second)->isPublic();
}

bool ConstHashDeclListIterator::isUserPublic() const {
    return typed_hash_decl_private::get(*i->second)->isUserPublic();
}
