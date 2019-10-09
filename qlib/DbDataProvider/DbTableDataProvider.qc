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

#! contains all public definitions in the DbDataProvider module
public namespace DbDataProvider {
#! Defines a data provider based on a single SQL table
public class DbTableDataProvider inherits AbstractDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "DbTableDataProvider",
            "supports_read": True,
            "supports_create": True,
            "supports_update": True,
            "supports_upsert": True,
            "supports_delete": True,
            "supports_native_search": True,
            "supports_bulk_create": True,
            "supports_bulk_upsert": True,
            "supports_children": False,
            "constructor_options": ConstructorOptions,
            "search_options": NOTHING,
            "create_options": CreateOptions,
            "upsert_options": UpsertOptions,
            "transaction_management": True,
        };

        #! Constructor options
        const ConstructorOptions = {
            "datasource": <DataProviderOptionInfo>{
                "type": (StringType, new Type("AbstractDatasource")),
                "desc": "the datasource connection string or an abstract datasource object; in case a connection "
                    "string is given, then the \"table\" option also needs to be given as a string",
            },
            "table": <DataProviderOptionInfo>{
                "type": (StringType, new Type("AbstractTable")),
                "required": True,
                "desc": "the table name or table object; if a table string is provided, then the \"datasource\" "
                    "option must also be provided",
            },
        };

        #! Create options
        const CreateOptions = {
            "returning": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::anyType,
                "desc": "a string (giving the output name) or a hash describing the return parameters for an insert; "
                    "a hash will have the following keys: 'key': (required) the column name to return, and 'type' "
                    "(optional) the data type for the output placeholder buffer (ex: 'number')",
            },
        };

        #! Upsert options
        const UpsertOptions = {
            "upsert_strategy": <DataProviderOptionInfo>{
                "type": StringType,
                "desc": "providers the upsert strategy; 'UpsertInsertFirst': try to insert first, if it fails, try "
                    "updating; 'UpsertUpdateFirst': try to update first, if it fails, try inserting; "
                    "'UpsertSelectFirst': try to select first, if it fails, insert, if it succeeds, update if "
                    "necessary; 'UpsertInsertOnly': only insert, never update, 'UpsertUpdateOnly': only update, "
                    "never insert; 'UpsertAuto' (the default) use the most efficient upsert for the underlying DB",
            },
            "omit_update": <DataProviderOptionInfo>{
                "type": new Type("softlist<string>"),
                "desc": "allows for an asymmetrical upsert where a set of column values is inserted, but a smaller "
                    "set is updated in case the unique key values are present in the target table; the value of this "
                    "option should be set to the columns to omit in the update clause",
            },
        };

        #! Maps SqlUtil @ref upsert_results to @ref db_provider_upsert_results
        const DbUpsertMap = {
            AbstractTable::UR_Inserted: UpsertResultInserted,
            AbstractTable::UR_Verified: UpsertResultVerified,
            AbstractTable::UR_Updated: UpsertResultUpdated,
            AbstractTable::UR_Unchanged: UpsertResultUnchanged,
            AbstractTable::UR_Deleted: UpsertResultDeleted,
        };
    }

    private {
        #! the table
        AbstractTable table;
    }

    #! Creates the object
    constructor(AbstractTable table) {
        self.table = table;
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        if (copts.table instanceof AbstractTable) {
            if (copts.datasource instanceof AbstractDatasource && copts.datasource != copts.table.getDatasource()) {
                error("CONSTRUCTOR-ERROR", "option \"table\" is based on a different datasource than the one "
                    "supplied in the \"datasource\" option");
            }
            self.table = copts.table;
            return;
        }
        if (!copts.datasource) {
            error("CONSTRUCTOR-ERROR", "option \"datasource\" is required if no \"table\" option providing "
                "an AbstractTable object is provided");
        }
        table = DbDataProvider::getTable(copts.datasource, copts.table);
    }

    #! Returns the data provider name
    string getName() {
        return table.getName();
    }

    #! Returns the description of the record type, if any
    *hash<string, AbstractDataField> getRecordType() {
        return table.getRecordType();
    }

    #! Returns child providers; return @ref nothing if there are no child providers
    *AbstractDataProvider getChildProviders() {
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

    #! Returns a bulk insert operation object for the data provider
    /** @return a bulk insert operation object for the data provider
    */
    AbstractDataProviderBulkOperation getBulkInserter() {
        return new DbTableBulkInserter(self, table);
    }

    #! Returns a bulk upsert operation object for the data provider
    /** @return a bulk upsert operation object for the data provider
    */
    AbstractDataProviderBulkOperation getBulkUpserter() {
        return new DbTableBulkUpserter(self, table);
    }

    #! Writes the given record to the data provider
    /** @param rec a hash representing a single input record
        @param create_options the create options after processing by validateCreateOptions()

        @return the data written to the data provider plus any output (returning) variables

        @note this API will commit the row to the database unless the datasource was already in a transaction before
        this call
    */
    private *hash<auto> createRecordImpl(hash<auto> rec, *hash<auto> create_options) {
        DbTableTransactionHelper db(table);
        *hash<auto> returning = db.insert(rec, create_options);
        return rec + returning;
    }

    #! Upserts the given record to the data provider
    /** @param rec a hash representing a single input record
        @param upsert_options the create options after processing by validateUpsertOptions()

        @return see @ref db_provider_upsert_results for possible values

        @note this API will commit the row to the database unless the datasource was already in a transaction before
        this call
    */
    string upsertRecordImpl(hash<auto> rec, *hash<auto> upsert_options) {
        DbTableTransactionHelper db(table);
        return DbUpsertMap{db.upsert(rec,
            AbstractTable::UpsertStrategyDescriptionMap{remove upsert_options.upsert_strategy}
            ?? AbstractTable::UpsertAuto, upsert_options)};
    }

    #! Returns a single record matching the search options
    /** @param search_options the search options after processing by validateSearchOptions()

        @throw MULTIPLE-RECORDS-ERROR multiple records found
    */
    private *hash<auto> searchSingleRecordImpl(hash<auto> where_cond, *hash<auto> search_options) {
        try {
            return table.selectRow({"where": where_cond});
        } catch (hash<ExceptionInfo> ex) {
            if (ex.err == "DBI-SELECT-ROW-ERROR") {
                throw "MULTIPLE-RECORDS-ERROR", ex.desc, ex.arg;
            }
            rethrow;
        }
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
        @param where_cond a hash for identifying the record to be update
        @param search_options the update options after processing by validateSearchOptions()

        @throw UPDATE-SINGLE-RECORD-ERROR if the update would update more than one record

        @note this API will commit the row to the database unless the datasource was already in a transaction before
        this call
    */
    private bool updateSingleRecordImpl(hash<auto> set, hash<auto> where_cond, *hash<auto> search_options) {
        int rows = updateRecordsImpl(set, where_cond, search_options);
        if (rows > 1) {
            bool in_trans = table.getDatasource().currentThreadInTransaction();
            throw "UPDATE-SINGLE-RECORD-ERROR", sprintf("%d record(s) were updated; %ssearch args: %y (set: %y)", rows,
                (!in_trans ? "the update has been rolled back; " : ""), search_options, set);
        }
        return rows.toBool();
    }

    #! Updates zero or more records matching the search options
    /** @param set the hash of field data to set
        @param where_cond a hash for identifying the record(s) to be updated
        @param search_options the update options after processing by validateSearchOptions()

        @return the number of records updated

        @note this API will commit the row to the database unless the datasource was already in a transaction before
        this call
    */
    private int updateRecordsImpl(hash<auto> set, *hash<auto> where_cond, *hash<auto> search_options) {
        DbTableTransactionHelper db(table);
        return db.update(set, where_cond);
    }

    #! Deletes zero or more records
    /**
        @param where_cond a hash for identifying the record(s) to be deleted
        @param search_options the update options after processing by validateSearchOptions()

        @return the number of records deleted

        @throw INVALID-OPERATION the data provider does not support record updating

        @note this API will commit the row to the database unless the datasource was already in a transaction before
        this call
    */
    private int deleteRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        DbTableTransactionHelper db(table);
        return db.del(where_cond);
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}
