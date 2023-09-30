# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file ImplicitModuleTransaction.qc rolls back module changes if external module initialization fails

/** ImplicitModuleTransaction.qc Copyright 2019 - 2023 Qore Technologies, s.r.o.

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

#! Private definitions
namespace Priv {
#! Private thread-local data to maintain transaction rollback information
thread_local ImplicitModuleTransaction rb;

#! Rolls back module changes if external module initialization fails
class ImplicitModuleTransaction {
    public {}

    private {
        #! list of rollback actions
        list<code> rollback();
    }

    #! Executes all rollback actions if module initialization fails
    destructor() {
        if (active_exception()) {
            # execute all rollback actions
            map $1(), rollback;
        }
    }

    #! Adds a rollback action
    addRollback(code rb_code) {
        rollback += rb_code;
    }

    #! Adds a rollback action and creates the transaction context if necessary
    static add(code rb_code) {
        if (!rb) {
            # create transaction context
            rb = new ImplicitModuleTransaction();
        }
        # add rollback action to transaction context
        rb.addRollback(rb_code);
    }
}
}
