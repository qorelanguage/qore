# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DbSelectRecordIterator class definition

/** DbSelectRecordIterator.qc Copyright 2019 - 2023 Qore Technologies, s.r.o.

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

#! contains all public definitions in the DbDataProvider module
public namespace DbDataProvider {
#! Defines the record iterator class for Table-based iterators
public class DbSelectRecordIterator inherits AbstractDbRecordIterator {
    private {
        #! search conditions
        *hash<auto> where_cond;
    }

    #! Creates the iterator
    /** @param ds the datasource to use
        @param where_cond the search conditions to apply to the result set from the SQL
        @param search_options search options; assumed to have already been processed for validity before this call; contains:
        - \c sql (required): the SQL query
        - \c args (optional): a list of bind arguments to \a sql
    */
    constructor(AbstractDatasource ds, *hash<auto> where_cond, hash<auto> select_options)
        : AbstractDbRecordIterator(!ds.currentThreadInTransaction(),
            DbSelectRecordIterator::prepareStatement(ds.getSQLStatement(), select_options)) {
        self.where_cond = where_cond;
    }

    #! Moves the current position to the next element; returns @ref False if there are no more elements
    /** This method will return @ref True again after it returns @ref False once if the object being iterated is not
        empty, otherwise it will always return @ref False.  The iterator object should not be used after this method
        returns @ref False

        @return @ref False if there are no more elements (in which case the iterator object is invalid and should not
        be used); @ref True if successful (meaning that the iterator object is valid)
    */
    bool next() {
        while (stmt.next()) {
            if (!where_cond || (where_cond && matchGeneric(stmt.getValue(), where_cond))) {
                return True;
            }
        }
        return False;
    }

    #! Returns the value of the given field in the current record, if the iterator is valid
    /** @param key the name of the field

        @return the value of the given field in the current record, if the iterator is valid

        @throw FIELD-ERROR invalid or unknown field name
    */
    auto memberGate(string key) {
        return doMemberGate(key);
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
