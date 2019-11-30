/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
    GlobalThreadLocalVar.h

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

#ifndef _QORE_VARIABLE_H
#define _QORE_VARIABLE_H

class GlobalThreadLocalVar {
public:
    DLLLOCAL GlobalThreadLocalVar(const QoreProgramLocation* loc, const char* name) 
        : loc(loc), val(QV_Node), name(name) {
        int rc = pthread_key_create(&key, autoDel);
        if (!rc) {
            local_key = true;
        } else {
            // use a map in TLD
            assert(false);
            local_key = false;
        }
    }

    DLLLOCAL ~GlobalThreadLocalVar() {
        if (local_key) {
            pthread_key_delete(&key);
        }
    }

private:
    pthread_key_t key;
    const QoreProgramLocation* loc;      // location of the initial definition
    QoreLValue<qore_gvar_ref_u> val;
    std::string name;
    const QoreTypeInfo* refTypeInfo = nullptr;
    bool pub = false,                    // is this global var public (valid and set for modules only)
        finalized = false,               // has this var already been cleared during Program destruction?
        local_key = false;

    DLLLOCAL static void autoDel(void* ptr) {
        ExceptionSink xsink;
        GlobalThreadLocalVar* self = reinterpret_cast<GlobalThreadLocalVar*>(ptr);
        assert(self->local_key);
        self->val.discard(&xsink);
    }
};

#endif