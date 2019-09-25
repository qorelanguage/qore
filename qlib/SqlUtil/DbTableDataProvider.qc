# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DbTableDataProvider class definition

/** DbTableDataProvider.qc Copyright 2019 Qore Technologies, s.r.o.

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

#! contains all public definitions in the SqlUtil module
public namespace SqlUtil {
#! Defines a data provider based on a single SQL table
public class DbTableDataProvider inherits AbstractDataProvider {
    public {
        #! Create options
        const CreateOptions = {
            "returning": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::anyType,
                "desc": "a string (giving the output name) or a hash describing the return parameters for an insert; "
                    "a hash will have the following keys: 'key': (required) the column name to return, and 'type' "
                    "(optional) the data type for the output placeholder buffer (ex: 'number')",
            },
        };

        #! Constructor options
        const ConstructorOptions = {
            "table": <DataProviderOptionInfo>{
                "type": new Type("AbstractTable"),
                "required": True,
                "desc": "the table the object is based on",
            },
        };
    }

    private {
        #! the table
        AbstractTable table;
    }

    #! Creates the object
    constructor(hash<auto> options) {
        processConstructorOptions(ConstructorOptions, options);
    }

    #! Returns the data provider name
    string getName() {
        return table.getName();
    }

    #! Returns the description of the record type, if any
    *hash<string, AbstractDataField> getRecordType() {
        return table.getRecordType();
    }

    #! Returns options that can be used for searching
    /** @return a hash of options that can be used for searching; keys are search option names, values describe the
        search option; if @ref nothing is returned, then the provider does not support any search options
    */
    *hash<string, hash<DataProviderOptionInfo>> getSearchOptions() {
    }

    #! Returns options that can be used for creating records
    /** @return a hash of options that can be used for creating records; keys are option names, values describe the
        option; if @ref nothing is returned, then the provider does not support any creation options
    */
    *hash<string, hash<DataProviderOptionInfo>> getCreateOptions() {
        return CreateOptions;
    }

    #! Returns child providers; return @ref nothing if there are no child providers
    *AbstractDataProvider getChildProviders() {
    }

    #! Returns @ref True if the data provider supports the record search API
    bool supportsSearch() {
        return True;
    }

    #! Returns @ref True if the data provider supports the record creation API
    bool supportsCreate() {
        return True;
    }

    #! Returns @ref True if the data provider supports the record update API
    bool supportsUpdate() {
        return True;
    }

    #! Returns @ref True if the data provider supports the record upsert API
    bool supportsUpsert() {
        return True;
    }

    #! Returns @ref True if the data provider supports the record deletion API
    bool supportsDelete() {
        return True;
    }

    #! Returns @ref True if the data provider supports transaction management
    /** @return @ref True if the data provider supports transaction management, in which case commit() or rollback()
        must be called to flush or discard data written to the data provider
    */
    bool requiresTransactionManagement() {
        return True;
    }

    #! Commits data written to the data provider
    /** Has no effect if the data provider does not support transaction management
    */
    commit() {
        table.commit();
    }

    #! Rolls back data written to the data provider
    /** Has no effect if the data provider does not support transaction management
    */
    rollback() {
        table.rollback();
    }

    #! returns constructor options
    static hash<string, hash<DataProviderOptionInfo>> getConstructorOptions() {
        return ConstructorOptions;
    }

    #! Writes the given record to the data provider
    /** @param rec a hash representing a single input record
        @param create_options the create options after processing by validateCreateOptions()

        @return the data written to the data provider plus any output (returning) variables
    */
    private *hash<auto> createRecordImpl(hash<auto> rec, *hash<auto> create_options) {
        *hash<auto> returning = table.insert(rec, create_options);
        return rec + returning;
    }

    #! Returns a single record matching the search options
    /** @param search_options the search options after processing by validateSearchOptions()
    */
    private *hash<auto> searchSingleRecordImpl(hash<auto> where_cond, *hash<auto> search_options) {
        return table.selectRow({"where": where_cond});
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param search_options the search options after processing by validateSearchOptions()

        @note this method acquires a transaction lock if one is not already acquired; the iterator returned performs a
        rollback in the destructor if a new transaction lock was acquired by this call and
        @ref SqlUtil::DbTableRecordIterator::keepTransactionLock() "DbTableRecordIterator::keepTransactionLock()" is
        not called
    */
    private DbTableRecordIterator searchRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        return new DbTableRecordIterator(table, where_cond, search_options);
    }

    #! Updates a single record matching the search options
    /** @param set the hash of field data to set
        @param search_options the search options after processing by validateSearchOptions()

        @note this API will commit the row to the database unless the datasource was already in a transaction before
        this call
    */
    private bool updateSingleRecordImpl(hash<auto> set, hash<auto> where_cond, *hash<auto> search_options) {
        hash<SqlCommandInfo> update_hash = table.getUpdateSql(set, where_cond);
        AbstractDatasource ds = table.getDatasource();

        # commit transaction on success if we are not already in a transaction
        bool in_trans = ds.currentThreadInTransaction();
        on_error {
            if (!in_trans) {
                ds.rollback();
            }
        }
        on_success {
            if (!in_trans) {
                ds.commit();
            }
        }

        int rows = ds.vexec(update_hash.sql, update_hash.args);
        if (rows > 1) {
            throw "UPDATE-SINGLE-RECORD-ERROR", sprintf("%d record(s) were updated; %ssearch args: %y (set: %y)", rows,
                (!in_trans ? "the update has been rolled back; " : ""), search_options, set);
        }
        return rows.toBool();
    }
}
}
