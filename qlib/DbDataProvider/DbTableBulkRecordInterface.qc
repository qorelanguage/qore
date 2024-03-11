# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DbTableBulkRecordInterface class definition

/** DbTableBulkRecordInterface.qc Copyright 2019 - 2024 Qore Technologies, s.r.o.

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
public class DbTableBulkRecordInterface inherits AbstractDataProviderBulkRecordInterface {
    private:internal {
        #! The table being iterated
        AbstractTable table;

        #! the statement being iterated
        AbstractSQLStatement stmt;

        #! release the transaction with a rollback in the destructor?
        bool release_transaction;
    }

    #! creates the iterator
    /** @param block_size the number of records in a read block; must be a positive number
        @param table the table to iteratate
        @param where_cond the search criteria; will be processed by processFieldValues()
        @param search_options search options; assumed to have already been processed for validity before this call
    */
    constructor(int block_size, AbstractTable table, *hash<auto> where_cond, *hash<auto> search_options)
            : AbstractDataProviderBulkRecordInterface(block_size) {
        self.table = table;
        # release the transaction with a rollback in the destructor by default if we do not already have a transaction
        # lock
        release_transaction = !table.getDatasource().currentThreadInTransaction();
        stmt = table.getStatement({"where": where_cond});
    }

    #! rolls back the transaction if a transaction lock was acquired in the constructor and keepTransactionLock() was not called
    destructor() {
        if (release_transaction) {
            stmt.rollback();
        }
    }

    #! Ensures that no rollback is performed when the object is destroyed
    keepTransactionLock() {
        release_transaction = False;
    }

    #! Returns a hash of lists according to the block size
    /** @throw INVALID-ITERATOR the iterator is not pointing at a valid element

        @note This call moves the internal record pointer forward, therefore multiple calls of this methods will
        return different results as long as data is available
    */
    private hash<string, list<auto>> getValueImpl() {
        return stmt.fetchColumns(block_size);
    }

    #! Returns the record description, if available
    /** @return the record type of the table
    */
    *hash<string, AbstractDataField> getRecordType() {
        return table.getRecordType();
    }
}
}
