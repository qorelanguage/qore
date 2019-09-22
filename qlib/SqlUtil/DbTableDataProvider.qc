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
        #! Search options
        const SearchOptions = {
            "select_hash": <DataProviderOptionInfo>{
                "type": HashType,
                "required": False,
                "desc": "a select hash for SqlUtil-based selects; if neither this option nor \"select_sql\" are "
                    "present, then the entire table is selected",
            },
            "select_sql": <DataProviderOptionInfo>{
                "type": StringType,
                "required": False,
                "desc": "a select string; if neither this option nor \"select_hash\" are present, then the entire "
                    "table is selected",
            },
            "select_sql_args": <DataProviderOptionInfo>{
                "type": SoftAutoListType,
                "required": False,
                "desc": "optional bind arguments for \"select_sql\"",
            },
        };
    }

    private {
        #! the table
        AbstractTable table;
    }

    #! Creates the object
    DbTableDataProvider(AbstractTable table) {
        self.table = table;
    }

    #! Returns the type name
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

    #! Returns True if the data provider supports writing output
    bool supportsWrite() {
        return True;
    }

    #! Writes the given record to the data provider
    /** @param rec a hash representing a single input record

        @return the data written to the data provider
    */
    *hash<auto> writeRecord(hash<auto> rec) {
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

    #! validates search options
    /** @throw SEARCH-OPTION-ERROR invalid or unsupported search option
    */
    private hash<auto> validateSearchOptions(hash<auto> search_options) {
        if (search_options.select_hash && search_options.select_sql) {
            error("SEARCH-OPTION-ERROR", "\"search_hash\" and \"search_sql\" cannot be given together");
        }
        return AbstractDataProvider::validateSearchOptions(search_options);
    }

    #! Returns a single record matching the search options
    /** @param search_options the search options after processing by validateSearchOptions()
    */
    private *hash<auto> searchSingleRecordImpl(hash<auto> search_options) {
        if (search_options.select_sql) {
            return table.getDatasource().selectRow(search_options.select_sql, search_options.select_sql_args);
        }
        return table.selectRow(search_options.select_hash);
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param search_options the search options after processing by validateSearchOptions()
    */
    private AbstractIterator searchRecordsImpl(hash<auto> search_options) {
        if (search_options.select_sql) {
            AbstractSQLStatement stmt = table.getDatasource().getSQLStatement();
            stmt.prepare(search_options.select_sql);
            stmt.execArgs(search_options.select_sql_args);
            return stmt;
        }
        return table.getStatement(search_options.select_hash);
    }
}
}
