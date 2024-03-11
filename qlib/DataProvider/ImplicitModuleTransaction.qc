# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file DataProviderImplicitModuleTransaction.qc provides the DataProvider catalog API

/** DataProviderImplicitModuleTransaction.qc Copyright 2019 - 2024 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Implements a mechanism to roll back changes made in external module initialization if an error occurs
public class ImplicitModuleTransaction {
    public {}

    private {
        list<code> rollback();
    }

    #! Executes all rollback code if there is an active exception in module initialization
    destructor() {
        if (active_exception()) {
            # execute all rollback actions
            map $1(), rollback;
        }
    }

    #! Adds a rollback closure to be executed if there is an active exception in module initialization
    static add(code rb_code) {
        if (!rb) {
            rb = new ImplicitModuleTransaction();
        }
        rb.addRollback(rb_code);
    }

    #! Adds a rollback closure to be executed if there is an active exception in module initialization
    private addRollback(code rb_code) {
        rollback += rb_code;
    }
}
}

#! Private definitions
namespace Priv {
# holds and optionally executes rollback code
thread_local ImplicitModuleTransaction rb;
}
