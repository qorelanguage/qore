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
        #! Constructor options
        const ConstructorOptions = {
            "table": <DataProviderOptionInfo>{
                "type": new Type("AbstractTable"),
                "required": True,
                "desc": "the table the object is based on",
            },
        };

        #! Search options
        const SearchOptions = {
            "where_hash": <DataProviderOptionInfo>{
                "type": HashType,
                "required": False,
                "desc": "a select \"where hash\" for SqlUtil-based selects; if neither this option nor "
                    "\"select_sql\" are present, then the entire table is selected",
            },
            "where_sql": <DataProviderOptionInfo>{
                "type": StringType,
                "required": False,
                "desc": "the where clause of a select string; if neither this option nor \"where_hash\" are present, "
                    "then the entire table is selected",
            },
            "where_sql_args": <DataProviderOptionInfo>{
                "type": SoftAutoListType,
                "required": False,
                "desc": "optional bind arguments for \"where_sql\"",
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
        search option; if @ref nothing is returned, then the provider does not support searching
    */
    *hash<string, hash<DataProviderOptionInfo>> getSearchOptions() {
        return SearchOptions;
    }

    #! Returns child providers; return @ref nothing if there are no child providers
    *AbstractDataProvider getChildProviders() {
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

    #! Writes the given record to the data provider
    /** @param rec a hash representing a single input record

        @return the data written to the data provider
    */
    *hash<auto> createRecord(hash<auto> rec) {
        table.insert(rec);
        return rec;
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

    #! validates search options
    /** @throw SEARCH-OPTION-ERROR invalid or unsupported search option
    */
    private *hash<auto> validateSearchOptions(*hash<auto> search_options) {
        if (search_options.select_hash && search_options.select_sql) {
            error("SEARCH-OPTION-ERROR", "\"search_hash\" and \"search_sql\" cannot be given together");
        }
        return AbstractDataProvider::validateSearchOptions(search_options);
    }

    #! Returns a single record matching the search options
    /** @param search_options the search options after processing by validateSearchOptions()
    */
    private *hash<auto> searchSingleRecordImpl(*hash<auto> search_options) {
        if (search_options.where_sql) {
            string sql = "select * from " + table.getSqlName() + " where " + search_options.where_sql;
            return table.getDatasource().selectRow(sql, search_options.where_sql_args);
        }
        return table.selectRow({"where": search_options.where_hash});
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param search_options the search options after processing by validateSearchOptions()

        @note this method acquires a transaction lock and requires a commit or rollback after the iteration is done
    */
    private AbstractIterator searchRecordsImpl(*hash<auto> search_options) {
        if (search_options.where_sql) {
            AbstractSQLStatement stmt = table.getDatasource().getSQLStatement();
            stmt.prepare("select * from " + table.getSqlName() + " where " + search_options.where_sql);
            stmt.execArgs(search_options.where_sql_args);
            return stmt;
        }
        return table.getStatement({"where": search_options.where_hash});
    }

    #! Updates a single record matching the search options
    /** @param set the hash of field data to set
        @param search_options the search options after processing by validateSearchOptions()

        @note this API will commit the row to the database unless the datasource was already in a transaction before
        this call
    */
    private bool updateSingleRecordImpl(hash<auto> set, hash<auto> search_options) {
        if (!search_options.where_hash && !search_options.where_sql) {
            throw "UPDATE-SINGLE-RECORD-ERROR", sprintf("missing \"where_hash\" or \"where_sql\" in update call; "
                "search options: %y", search_options);
        }
        hash<SqlCommandInfo> update_hash = table.getUpdateSql(set, search_options.where_hash);
        if (search_options.where_sql) {
            update_hash = table.getUpdateSql(set);
            update_hash.sql += " where " + search_options.where_sql;
        } else {
            update_hash = table.getUpdateSql(set, search_options.where_hash);
        }

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
            throw "UPDATE-SINGLE-RECORD-ERROR", sprintf("%d record(s) were updated; the update has been rolled "
                "back; search args: %y (set: %y)", rows, search_options, set);
        }
        return rows.toBool();
    }
}
}
