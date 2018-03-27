/*
  QoreClassList.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2018 Qore Technologies, s.r.o.

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
#include "qore/intern/QoreClassList.h"
#include "qore/intern/HashDeclList.h"
#include "qore/intern/QoreNamespaceList.h"
#include "qore/intern/QoreClassIntern.h"
#include "qore/intern/QoreNamespaceIntern.h"
#include <qore/minitest.hpp>

#include <assert.h>

#ifdef DEBUG_TESTS
#  include "tests/QoreClassList_tests.cpp"
#endif

void QoreClassList::remove(hm_qc_t::iterator i) {
    QoreClass* qc = i->second.cls;
    //printd(5, "QCL::remove() this: %p '%s' (%p)\n", this, qc->getName(), qc);
    hm.erase(i);
    qore_class_private::get(*qc)->deref();
}

void QoreClassList::deleteAll() {
    for (auto& i : hm) {
        qore_class_private::get(*i.second.cls)->deref();
    }
    hm.clear();
}

QoreClassList::~QoreClassList() {
    deleteAll();
}

void QoreClassList::addInternal(QoreClass *oc, bool priv) {
    printd(5, "QCL::addInternal() this: %p '%s' (%p)\n", this, oc->getName(), oc);

    assert(!find(oc->getName()));
    hm[oc->getName()] = cl_rec_t(oc, priv);
}

int QoreClassList::add(QoreClass *oc) {
    printd(5, "QCL::add() this: %p '%s' (%p)\n", this, oc->getName(), oc);

    if (find(oc->getName()))
        return 1;

    hm[oc->getName()] = cl_rec_t(oc, false);
    return 0;
}

QoreClass* QoreClassList::find(const char* name) {
    hm_qc_t::iterator i = hm.find(name);
    return i != hm.end() ? i->second.cls : nullptr;
}

const QoreClass* QoreClassList::find(const char* name) const {
    hm_qc_t::const_iterator i = hm.find(name);
    return i != hm.end() ? i->second.cls : nullptr;
}

QoreClassList::QoreClassList(const QoreClassList& old, int64 po, qore_ns_private* ns) {
    for (hm_qc_t::const_iterator i = old.hm.begin(), e = old.hm.end(); i != e; ++i) {
        if (!i->second.cls->isSystem()) {
            //printd(5, "QoreClassList::QoreClassList() this: %p c: %p '%s' po & PO_NO_INHERIT_USER_CLASSES: %s pub: %s\n", this, i->second, i->second->getName(), po & PO_NO_INHERIT_USER_CLASSES ? "true": "false", qore_class_private::isPublic(*i->second) ? "true": "false");
            if (po & PO_NO_INHERIT_USER_CLASSES || !qore_class_private::isPublic(*i->second.cls))
                continue;
        }
        else
            if (po & PO_NO_INHERIT_SYSTEM_CLASSES)
                continue;
        QoreClass* qc = new QoreClass(*i->second.cls);
        qore_class_private::setNamespace(qc, ns);
        addInternal(qc, true);
    }
}

void QoreClassList::mergeUserPublic(const QoreClassList& old, qore_ns_private* ns) {
    for (hm_qc_t::const_iterator i = old.hm.begin(), e = old.hm.end(); i != e; ++i) {
        if (i->second.priv || !qore_class_private::isUserPublic(*i->second.cls))
            continue;

        QoreClass* qc = find(i->first);
        if (qc) {
            // the class must be injected or already imported
            assert(qore_class_private::injected(*qc) || qore_class_private::get(*qc) == qore_class_private::get(*i->second.cls));
            continue;
        }

        qc = new QoreClass(*i->second.cls);
        qore_class_private::setNamespace(qc, ns);
        addInternal(qc, true);
    }
}

int QoreClassList::importSystemClasses(const QoreClassList& source, qore_ns_private* ns, ExceptionSink* xsink) {
    int cnt = 0;
    for (hm_qc_t::const_iterator i = source.hm.begin(), e = source.hm.end(); i != e; ++i) {
        if (i->second.cls->isSystem()) {
            hm_qc_t::const_iterator ci = hm.find(i->second.cls->getName());
            if (ci != hm.end()) {
                if (!qore_class_private::injected(*ci->second.cls)) {
                    xsink->raiseException("IMPORT-SYSTEM-API-ERROR", "cannot import system class %s::%s due to an existing class without the injection flag set", ns->name.c_str(), ci->second.cls->getName());
                    break;
                }
                continue;
            }
            //printd(5, "QoreClassList::importSystemClasses() this: %p importing %p %s::'%s'\n", this, i->second, ns->name.c_str(), i->second->getName());
            QoreClass* qc = new QoreClass(*i->second.cls);
            qore_class_private::setNamespace(qc, ns);
            addInternal(qc, true);
            ++cnt;
        }
    }
    return cnt;
}

void QoreClassList::resolveCopy() {
    for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
        qore_class_private::resolveCopy(*(i->second.cls));
}

void QoreClassList::parseInit() {
    for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
        //printd(5, "QoreClassList::parseInit() this: %p initializing %p '%s'\n", this, i->second, i->first);
        qore_class_private::parseInit(*(i->second.cls));
    }
}

void QoreClassList::parseRollback() {
    for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
        qore_class_private::parseRollback(*(i->second.cls));
}

void QoreClassList::parseCommit() {
    for (auto& i : hm) {
        //printd(5, "QoreClassList::parseCommit() this: %p qc: %p '%s' pub: %d\n", this, i.second, i.second->getName(), qore_class_private::isPublic(*i.second));
        qore_class_private::parseCommit(*(i.second.cls));
    }
}

void QoreClassList::parseCommitRuntimeInit(ExceptionSink* xsink) {
    for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
        qore_class_private::parseCommitRuntimeInit(*(i->second.cls), xsink);
}

void QoreClassList::reset() {
    deleteAll();
}

void QoreClassList::assimilate(QoreClassList& n, qore_ns_private& ns) {
    hm_qc_t::iterator i = n.hm.begin();
    while (i != n.hm.end()) {
        if (ns.hashDeclList.find(i->first)) {
            parse_error(*qore_class_private::get(*i->second.cls)->loc, "hashdecl '%s' has already been defined in namespace '%s'", i->first, ns.name.c_str());
            n.remove(i);
        }
        else if (ns.classList.find(i->first)) {
            parse_error(*qore_class_private::get(*i->second.cls)->loc, "class '%s' has already been defined in namespace '%s'", i->first, ns.name.c_str());
            n.remove(i);
        }
        else if (find(i->first)) {
            parse_error(*qore_class_private::get(*i->second.cls)->loc, "class '%s' is already pending in namespace '%s'", i->first, ns.name.c_str());
            n.remove(i);
        }
        else if (ns.nsl.find(i->first)) {
            parse_error(*qore_class_private::get(*i->second.cls)->loc, "cannot add class '%s' to existing namespace '%s' because a subnamespace has already been defined with this name", i->first, ns.name.c_str());
            n.remove(i);
        }
        else {
            //printd(5, "QoreClassList::assimilate() this: %p adding: %p '%s::%s'\n", this, i->second, ns.name.c_str(), i->second->getName());

            // "move" data to new list
            hm[i->first] = i->second;
            qore_class_private::setNamespace(i->second.cls, &ns);
            n.hm.erase(i);
        }
        i = n.hm.begin();
    }
}

QoreHashNode *QoreClassList::getInfo() {
    QoreHashNode *h = new QoreHashNode;
    for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
        h->setKeyValue(i->first, i->second.cls->getMethodList(), nullptr);
    return h;
}

/*
QoreValue QoreClassList::findConstant(const char *cname, const QoreTypeInfo *&typeInfo, bool& found) {
    for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
        QoreValue rv = qore_class_private::parseFindLocalConstantValue(i->second.cls, cname, typeInfo, found);
        if (found) {
            return rv;
        }
    }

    return QoreValue();
}
*/

void QoreClassList::clearConstants(QoreListNode& l) {
    for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
        qore_class_private::clearConstants(i->second.cls, l);
    }
}

void QoreClassList::clear(ExceptionSink *xsink) {
    for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
        qore_class_private::clear(i->second.cls, xsink);
    }
}

void QoreClassList::deleteClassData(ExceptionSink *xsink) {
    for (hm_qc_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
        qore_class_private::deleteClassData(i->second.cls, xsink);
    }
}

void QoreClassList::deleteClearData(ExceptionSink* xsink) {
    for (auto& i : hm) {
        qore_class_private::get(*i.second.cls)->deref(xsink);
    }
    hm.clear();
}

bool ClassListIterator::isPublic() const {
    return !i->second.priv && qore_class_private::isPublic(*i->second.cls);
}

bool ConstClassListIterator::isPublic() const {
    return !i->second.priv && qore_class_private::isPublic(*i->second.cls);
}

bool ClassListIterator::isUserPublic() const {
    return !i->second.priv && qore_class_private::isUserPublic(*i->second.cls);
}

bool ConstClassListIterator::isUserPublic() const {
    return !i->second.priv && qore_class_private::isUserPublic(*i->second.cls);
}
