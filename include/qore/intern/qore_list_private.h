/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    qore_list_private.h

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

#ifndef _QORE_QORELISTPRIVATE_H
#define _QORE_QORELISTPRIVATE_H

#include <string.h>

typedef ReferenceHolder<QoreListNode> safe_qorelist_t;

#define LIST_PAD   15

struct qore_list_private {
    QoreValue* entry = nullptr;
    size_t length = 0;
    size_t allocated = 0;
    unsigned obj_count = 0;
    const QoreTypeInfo* complexTypeInfo = nullptr;
    bool finalized : 1;
    bool vlist : 1;

    DLLLOCAL qore_list_private() : finalized(false), vlist(false) {
    }

    DLLLOCAL ~qore_list_private() {
        assert(!length);

        if (entry) {
            free(entry);
        }
    }

    DLLLOCAL const QoreTypeInfo* getValueTypeInfo() const {
        return complexTypeInfo ? QoreTypeInfo::getUniqueReturnComplexList(complexTypeInfo) : nullptr;
    }

    DLLLOCAL const QoreTypeInfo* getTypeInfo() const {
        return complexTypeInfo ? complexTypeInfo : listTypeInfo;
    }

    DLLLOCAL void getTypeName(QoreString& str) const {
        if (complexTypeInfo)
            str.concat(QoreTypeInfo::getName(complexTypeInfo));
        else
            str.concat("list");
    }

    DLLLOCAL QoreListNode* getCopy() const {
        QoreListNode* l = new QoreListNode;
        if (complexTypeInfo) {
            l->priv->complexTypeInfo = complexTypeInfo;
        }
        return l;
    }

    DLLLOCAL QoreListNode* getEmptyCopy(bool is_value) const {
        QoreListNode* l = new QoreListNode(!is_value);
        if (complexTypeInfo) {
            l->priv->complexTypeInfo = complexTypeInfo;
        }
        return l;
    }

    DLLLOCAL QoreListNode* copy(const QoreTypeInfo* newComplexTypeInfo) const {
        QoreListNode* l = new QoreListNode;
        l->priv->complexTypeInfo = newComplexTypeInfo;
        copyIntern(*l->priv);
        return l;
    }

    // strip = copy without type information
    DLLLOCAL QoreListNode* copy(bool strip = false) const {
        // issue #2791 perform type stripping at the source
        if (!strip || !complexTypeInfo) {
            QoreListNode* l = getCopy();
            copyIntern(*l->priv);
            return l;
        }
        QoreListNode* l = new QoreListNode;
        l->priv->reserve(length);
        for (size_t i = 0; i < length; ++i) {
            l->priv->pushIntern(copy_strip_complex_types(entry[i]));
        }
        return l;
    }

    DLLLOCAL void copyIntern(qore_list_private& l) const {
        l.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            l.pushIntern(entry[i].refSelf());
        }
    }

    DLLLOCAL QoreListNode* concatenate(const QoreListNode* l, ExceptionSink* xsink) const {
        bool strip = !QoreTypeInfo::equal(complexTypeInfo, l->priv->complexTypeInfo);
        ReferenceHolder<QoreListNode> rv(copy(strip), xsink);
        rv->merge(l, xsink);
        return *xsink ? nullptr : rv.release();
    }

    DLLLOCAL QoreListNode* concatenateElement(QoreValue e, ExceptionSink* xsink) const {
        const QoreTypeInfo* et = QoreTypeInfo::getUniqueReturnComplexList(complexTypeInfo);
        bool strip = !QoreTypeInfo::equal(et, e.getTypeInfo());
        ReferenceHolder<QoreListNode> rv(copy(strip), xsink);
        return rv->priv->push(e, xsink) ? nullptr : rv.release();
    }

    DLLLOCAL QoreListNode* prependElement(QoreValue e, ExceptionSink* xsink) const {
        const QoreTypeInfo* et = QoreTypeInfo::getUniqueReturnComplexList(complexTypeInfo);
        bool strip = !QoreTypeInfo::equal(et, e.getTypeInfo());
        ReferenceHolder<QoreListNode> rv(new QoreListNode, xsink);

        if (complexTypeInfo && !strip) {
            rv->priv->complexTypeInfo = complexTypeInfo;
        }

        rv->priv->pushIntern(e);

        for (size_t i = 0; i < length; ++i) {
            QoreValue v = entry[i];
            if (strip) {
                v = copy_strip_complex_types(v);
            }
            else {
                v.refSelf();
            }
            rv->priv->pushIntern(v);
        }

        return rv.release();
    }

    DLLLOCAL int checkVal(ValueHolder& holder, ExceptionSink* xsink) {
        if (complexTypeInfo) {
            const QoreTypeInfo* vti = QoreTypeInfo::getUniqueReturnComplexList(complexTypeInfo);
            if (!QoreTypeInfo::superSetOf(vti, holder->getTypeInfo()) && QoreTypeInfo::hasType(complexTypeInfo)) {
                QoreValue v(holder.release());
                QoreTypeInfo::acceptInputParam(vti, -1, nullptr, v, xsink);
                holder = v;
                return xsink && *xsink ? -1 : 0;
            }
            return 0;
        }
        else {
            switch (holder->getType()) {
                case NT_LIST:
                case NT_HASH: {
                    {
                        ValueHolder v(holder.release(), xsink);
                        holder = copy_strip_complex_types(*v);
                    }
                    return xsink && *xsink ? -1 : 0;
                }
                default:
                    break;
            }
        }
        return 0;
    }

    DLLLOCAL int push(QoreValue val, ExceptionSink* xsink) {
        ValueHolder holder(val, xsink);
        if (checkVal(holder, xsink)) {
            return -1;
        }
        pushIntern(holder.release());
        return 0;
    }

    DLLLOCAL int merge(const QoreListNode* list, ExceptionSink* xsink) {
        reserve(length + list->size());
        ConstListIterator i(list);
        while (i.next()) {
            if (push(i.getReferencedValue(), xsink))
                return -1;
        }
        return 0;
    }

    DLLLOCAL void pushIntern(QoreValue val) {
        getEntryReference(length) = val;
        if (needs_scan(val)) {
            incScanCount(1);
        }
    }

    DLLLOCAL size_t checkOffset(ptrdiff_t offset) {
        if (offset < 0) {
            offset = length + offset;
            return offset < 0 ? 0 : offset;
        }
        else if ((size_t)offset > length)
            return length;

        return offset;
    }

    DLLLOCAL void checkOffset(ptrdiff_t offset, ptrdiff_t len, size_t &n_offset, size_t &n_len) {
        n_offset = checkOffset(offset);
        if (len < 0) {
            len = length + len - n_offset;
            n_len = len < 0 ? 0 : len;
            return;
        }
        n_len = len;
    }

    QoreValue spliceSingle(size_t offset) {
        assert(offset < length);

        QoreValue rv = entry[offset];
        if (needs_scan(rv)) {
            incScanCount(-1);
        }

        size_t end = offset + 1;

        if (end != length) {
            memmove(entry + offset, entry + end, sizeof(QoreValue) * (length - end));
            // zero out trailing entries
            zeroEntries(length - 1, length);
        }
        else // set last entry to 0
            entry[end - 1] = QoreValue();

        resize(length - 1);

        return rv;
    }

    DLLLOCAL QoreListNode* spliceIntern(size_t offset, size_t len, bool extract) {
        //printd(5, "spliceIntern(offset: %d, len: %d, length: %d)\n", offset, len, length);
        size_t end;
        if (len > (length - offset)) {
            end = length;
            len = length - offset;
        }
        else
            end = offset + len;

        QoreListNode* rv = extract ? getCopy() : nullptr;

        // dereference all entries that will be removed or add to return value list
        for (size_t i = offset; i < end; i++) {
            removeEntry(entry[i], rv);
        }

        // move down entries if necessary
        if (end != length) {
            memmove(entry + offset, entry + end, sizeof(QoreValue) * (length - end));
            // zero out trailing entries
            zeroEntries(length - len, length);
        }
        else // set last entry to 0
            entry[end - 1] = QoreValue();

        resize(length - len);

        return rv;
    }

    DLLLOCAL QoreListNode* spliceIntern(size_t offset, size_t len, const QoreValue l, bool extract, ExceptionSink* xsink) {
        // check type compatibility before modifying the list
        ValueHolder holder(xsink);
        if (l.getType() == NT_LIST) {
            // create a new temporary list and check types first
            const qore_list_private* sl = l.get<QoreListNode>()->priv;
            QoreListNode* tmp;
            holder = tmp = sl->getCopy();
            tmp->priv->reserve(sl->length);
            for (size_t i = 0; i < sl->length; ++i) {
                ValueHolder eh(sl->entry[i].refSelf(), xsink);
                if (checkVal(eh, xsink)) {
                    return nullptr;
                }
                tmp->priv->entry[i] = eh.release();
                ++tmp->priv->length;
            }
        }
        else {
            holder = l.refSelf();
            if (checkVal(holder, xsink)) {
                return nullptr;
            }
        }

        //printd(5, "spliceIntern(offset: %d, len: %d, length: %d)\n", offset, len, length);
        size_t end;
        if (len > (length - offset)) {
            end = length;
            len = length - offset;
        }
        else
            end = offset + len;

        QoreListNode* rv = extract ? getCopy() : nullptr;

        // dereference all entries that will be removed or add to return value list
        for (size_t i = offset; i < end; i++) {
            removeEntry(entry[i], rv);
        }

        // get number of entries to insert
        size_t n = l.getType() == NT_LIST ? l.get<const QoreListNode>()->size() : 1;
        // difference
        if (n > len) { // make bigger
            size_t ol = length;
            resize(length - len + n);
            // move trailing entries forward if necessary
            if (end != ol)
                memmove(entry + (end - len + n), entry + end, sizeof(QoreValue) * (ol - end));
        }
        else if (len > n) { // make list smaller
            memmove(entry + offset + n, entry + offset + len, sizeof(QoreValue) * (length - offset - n));
            // zero out trailing entries
            zeroEntries(length - (len - n), length);
            // resize list
            resize(length - (len - n));
        }

        // add in new entries
        if (l.getType() != NT_LIST) {
            entry[offset] = holder.release();
            if (needs_scan(l))
                incScanCount(1);
        }
        else {
            qore_list_private* lst = holder->get<const QoreListNode>()->priv;
            for (size_t i = 0; i < n; ++i) {
                QoreValue v = lst->entry[i];
                lst->entry[i] = QoreValue();
                if (needs_scan(v))
                    incScanCount(1);
                entry[offset + i] = v;
            }
            lst->length = 0;
        }

        return rv;
    }

    DLLLOCAL QoreValue& getEntryReference(size_t num) {
        if (num >= length) {
            resize(num + 1);
        }
        return entry[num];
    }

    DLLLOCAL QoreValue getAndClear(size_t i) {
        if (i >= length) {
            return QoreValue();
        }
        QoreValue rv = entry[i];
        entry[i] = QoreValue();

        if (needs_scan(rv)) {
            incScanCount(-1);
        }

        return rv;
    }

    DLLLOCAL static QoreListNode* getPlainList(QoreListNode* l) {
        if (!l->priv->complexTypeInfo)
            return l;
        // no exception is possible
        ReferenceHolder<QoreListNode> holder(l, nullptr);
        return l->priv->copy(true);
    }

    DLLLOCAL QoreValue swapIntern(qore_offset_t offset, QoreValue val) {
        assert(offset >= 0);
        QoreValue& p = getEntryReference((size_t)offset);

        QoreValue rv = p;
        p = val;

        return rv;
    }

    DLLLOCAL QoreValue swap(qore_offset_t offset, QoreValue val) {
        QoreValue& p = getEntryReference((size_t)offset);

        bool before = needs_scan(p);
        bool after = needs_scan(val);
        if (before) {
            if (!after)
                --obj_count;
        }
        else if (after) {
            ++obj_count;
        }

        QoreValue rv = p;
        p = val;

        return rv;
    }

    DLLLOCAL QoreValue takeExists(size_t offset) {
        if (offset >= length) {
            return QoreValue();
        }

        QoreValue rv = entry[offset];
        entry[offset].assignNothing();

        if (needs_scan(rv)) {
            --obj_count;
        }

        return rv;
    }

    DLLLOCAL void reserve(size_t num) {
        if (num < length)
            return;
        // make larger
        if (num >= allocated) {
            size_t d = num >> 2;
            allocated = num + (d < LIST_PAD ? LIST_PAD : d);
            entry = (QoreValue*)realloc(entry, sizeof(QoreValue) * allocated);
        }
    }

    DLLLOCAL void resize(size_t num) {
        if (num < length) { // make smaller
            //entry = (QoreValue*)realloc(entry, sizeof(QoreValue*) * num);
            length = num;
            return;
        }
        // make larger
        if (num >= length) {
            if (num >= allocated) {
                size_t d = num >> 2;
                allocated = num + (d < LIST_PAD ? LIST_PAD : d);
                entry = (QoreValue*)realloc(entry, sizeof(QoreValue) * allocated);
            }
            zeroEntries(length, num);
        }
        length = num;
    }

    DLLLOCAL void zeroEntries(size_t start, size_t end) {
        for (size_t i = start; i < end; ++i) {
            entry[i] = QoreValue();
        }
    }

    DLLLOCAL void removeEntry(QoreValue& v, QoreListNode*& rv) {
        if (needs_scan(v)) {
            incScanCount(-1);
        }
        if (!rv)
            rv = new QoreListNode(autoTypeInfo);
        rv->priv->pushIntern(v);
    }

    DLLLOCAL int getLValue(size_t ind, LValueHelper& lvh, bool for_remove, ExceptionSink* xsink);

    // mergesort for controlled and interruptible sorts (stable)
    DLLLOCAL int mergesort(const ResolvedCallReferenceNode* fr, bool ascending, ExceptionSink* xsink);

    // quicksort for controlled and interruptible sorts (unstable)
    DLLLOCAL int qsort(const ResolvedCallReferenceNode* fr, size_t left, size_t right, bool ascending, ExceptionSink* xsink);

    DLLLOCAL void incScanCount(int dt) {
        assert(dt);
        assert(obj_count || (dt > 0));
        //printd(5, "qore_list_private::incScanCount() this: %p dt: %d: %d -> %d\n", this, dt, obj_count, obj_count + dt);
        obj_count += dt;
    }

    DLLLOCAL QoreListNode* eval(ExceptionSink* xsink);

    DLLLOCAL static void setNeedsEval(QoreListNode& l) {
        l.needs_eval_flag = true;
        l.value = false;
    }

    DLLLOCAL static int parseInitComplexListInitialization(const QoreProgramLocation* loc, LocalVar *oflag, int pflag, QoreParseListNode* args, QoreValue& new_args, const QoreTypeInfo* vti);

    DLLLOCAL static int parseInitListInitialization(const QoreProgramLocation* loc, LocalVar *oflag, int pflag, int& lvids, QoreParseListNode* args, QoreValue& new_args, const QoreTypeInfo*& argTypeInfo);

    DLLLOCAL static void parseCheckComplexListInitialization(const QoreProgramLocation* loc, const QoreTypeInfo* typeInfo, const QoreTypeInfo* expTypeInfo, const QoreValue exp, const char* context_action, bool strict_check = true);

    DLLLOCAL static void parseCheckTypedAssignment(const QoreProgramLocation* loc, const QoreValue arg, const QoreTypeInfo* vti, const char* context_action, bool strict_check = true);

    DLLLOCAL static QoreListNode* newComplexList(const QoreTypeInfo* typeInfo, const QoreValue args, ExceptionSink* xsink);

    // caller owns any reference in "init"
    DLLLOCAL static QoreListNode* newComplexListFromValue(const QoreTypeInfo* typeInfo, QoreValue init, ExceptionSink* xsink);

    DLLLOCAL static const qore_list_private* get(const QoreListNode& l) {
        return l.priv;
    }

    DLLLOCAL static qore_list_private* get(QoreListNode& l) {
        return l.priv;
    }

    DLLLOCAL static unsigned getScanCount(const QoreListNode& l) {
        return l.priv->obj_count;
    }

    DLLLOCAL static void incScanCount(const QoreListNode& l, int dt) {
        l.priv->incScanCount(dt);
    }
};

#endif
