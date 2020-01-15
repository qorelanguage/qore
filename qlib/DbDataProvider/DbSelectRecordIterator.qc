# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DbSelectRecordIterator class definition

/** DbSelectRecordIterator.qc Copyright 2019 - 2020 Qore Technologies, s.r.o.

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

# minimum required Qore version
%requires qore >= 0.9.4
# assume local scope for variables, do not use "$" signs
%new-style
# require type definitions everywhere
%require-types
#! strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

#! contains all public definitions in the DbDataProvider module
public namespace DbDataProvider {
#! Defines the record iterator class for Table-based iterators
public class DbSelectRecordIterator inherits AbstractDbRecordIterator {
    #! Creates the iterator
    /** @param table the table to iteratate
        @param search_options search options; assumed to have already been processed for validity before this call
    */
    constructor(AbstractDatasource ds, hash<auto> select_options)
        : AbstractDbRecordIterator(!ds.currentThreadInTransaction(),
            DbSelectRecordIterator::prepareStatement(ds.getStatement(), select_options)) {
    }

    #! Prepares the AbstractSQLStatement object for the iterator
    private static AbstractSQLStatement prepareStatement(AbstractSQLStatement stmt, hash<auto> select_options) {
        stmt.prepare(select_options.sql);
        if (select_options.args) {
            stmt.bindArgs(select_options.args);
        }
        return stmt;
    }
}
}
