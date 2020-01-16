# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DbDataProvider class definition

/** DbDataProvider.qc Copyright 2019 - 2020 Qore Technologies, s.r.o.

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
#! The database data provider class, provides tables as children
public class DbDataProvider inherits AbstractDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "DbDataProvider",
            "supports_read": True,
            "supports_bulk_read": True,
            "supports_children": True,
            "constructor_options": ConstructorOptions,
            "search_options": SearchOptions,
            "create_options": NOTHING,
            "upsert_options": NOTHING,
            "transaction_management": True,
            "has_record": True,
            "record_requires_search_options": True,
        };

        #! Constructor options
        const ConstructorOptions = {
            "datasource": <DataProviderOptionInfo>{
                "type": (
                    AbstractDataProviderType::get(StringType),
                    AbstractDataProviderType::get(new Type("AbstractDatasource")),
                ),
                "desc": "the datasource connection string or an abstract datasource object",
            },
            "database": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(new Type("AbstractDatabase")),
                "desc": "the database for the object",
            },
        };

        #! Search options
        const SearchOptions = {
            "sql": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the raw SQL for the select statement",
                "required": True,
            },
            "args": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(AutoListType),
                "desc": "any bind arguments for the select statement",
            },
        };
    }

    private {
        #! The database object
        AbstractDatabase db;

        #! Lookup to get an abstract datasource from a name
        static *code datasource_lookup;

        #! Lookup to get an abstract table from a datasource and a name
        static *code table_lookup;
    }

    #! creates the object
    constructor(AbstractDatasource ds, *hash<auto> opts) {
        db = AbstractDatabase::getDatabase(ds, opts);
    }

    #! creates the object
    constructor(AbstractDatabase db) {
        self.db = db;
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        if (copts.database instanceof AbstractDatabase) {
            if (copts.datasource instanceof AbstractDatasource && copts.datasource != copts.database.getDatasource()) {
                error("CONSTRUCTOR-ERROR", "option \"database\" is based on a different datasource than the one "
                    "supplied in the \"datasource\" option");
            }
            self.db = copts.database;
            return;
        }
        if (!copts.datasource) {
            error("CONSTRUCTOR-ERROR", "option \"datasource\" is required if no \"database\" option is provided");
        }
        AbstractDatasource ds;
        if (copts.datasource instanceof AbstractDatasource) {
            ds = copts.datasource;
        } else {
            ds = getDatasource(copts.datasource);
        }
        db = AbstractDatabase::getDatabase(ds);
    }

    #! Returns the data provider name
    string getName() {
        return db.getDatasourceDesc();
    }

    #! Commits any transaction in progress in the datasource
    /**
    */
    commit() {
        db.commit();
    }

    #! Rolls back any transaction in progress in the datasource
    /**
    */
    rollback() {
        db.rollback();
    }

    #! Returns the description of the record type, if any
    /** @param search_options to be included and processed by validateSearchOptions() if
        \a recordRequiresSearchOptions() is @ref True for this provider, otherwise any value provided in this argument
        is ignored
    */
    private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options) {
        AbstractDatasource ds = db.getDatasource();
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

        AbstractSQLStatement stmt = ds.getSQLStatement();
        stmt.prepare(search_options.sql);
        if (search_options.args) {
            stmt.bindArgs(search_options.args);
        }
        HashDataType input_data();
        foreach hash<auto> i in (stmt.describe().pairIterator()) {
            # get input column name
            string key = i.key.lwr();
            string desc = i.value.native_type.lwr();
            # add DB maximum size to desc
            if (i.value.maxsize) {
                desc += sprintf("(%d)", i.value.maxsize);
            }
            #printf("i: %y\n", i);
            *string type = DataProvider::OptimalQoreDataTypeMap{TypeCodeMap{i.value.type}};
            AbstractDataProviderType data_type;
            if (!type.val() || type == "any") {
                data_type = AbstractDataProviderType::get(AutoType);
            } else {
                # maps standard types to auto types to avoid type stripping
                if (type =~ /(hash|list)$/) {
                    type += "<auto>";
                }
                data_type = AbstractDataProviderType::get(type);
            }
            input_data.addField(new QoreDataField(key, desc, data_type));
        }
        return input_data.getFields();
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param block_size the number of records in a read block; must be a positive number
        @param where_cond the search criteria after processing by processFieldValues()
        @param search_options the search options after processing by validateSearchOptions()

        @throw INVALID-BLOCK-SIZE the block size must be a positive number
        @throw INVALID-OPERATION the data provider does not support reading
    */
    private AbstractDataProviderBulkRecordInterface searchRecordsBulkImpl(int block_size = 1000, *hash<auto> where_cond, *hash<auto> search_options) {
        return new DbSelectBulkRecordInterface(block_size, db.getDatasource(), where_cond, search_options);
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()

        @see requestSearchRecordsImpl()
    */
    private AbstractDataProviderRecordIterator searchRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        return new DbSelectRecordIterator(db.getDatasource(), where_cond, search_options);
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        return db.listTables();
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @throw CHILD-PROVIDER-ERROR error acquiring child provider

        @see getChildProviderEx()
    */
    private *AbstractDataProvider getChildProviderImpl(string name) {
        *AbstractTable table;
        try {
            table = db.getTable(name);
            if (!table) {
                return;
            }
        } catch (hash<ExceptionInfo> ex) {
            throw "CHILD-PROVIDER-ERROR", sprintf("table %y cannot be acquired from %y: %s: %s", name, getName(), ex.err, ex.desc), ex.arg;
        }
        return new DbTableDataProvider(table);
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }

    #! Sets the datasource lookup
    static setDatasourceLookup(code datasource_lookup) {
        DbDataProvider::datasource_lookup = datasource_lookup;
    }

    #! Sets the table lookup
    static setTableLookup(code table_lookup) {
        DbDataProvider::table_lookup = table_lookup;
    }

    #! Returns the given AbstractDatasource object
    static AbstractDatasource getDatasource(AbstractDatasource ds) {
        return ds;
    }

    #! Returns an AbstractDatasource object from the given string
    static AbstractDatasource getDatasource(string ds_string) {
        return datasource_lookup && (ds_string !~ /:/)
            ? DbDataProvider::datasource_lookup(ds_string)
            : new Datasource(ds_string);
    }

    #! Returns an AbstractTable object from the given datasource and table strings
    static AbstractTable getTable(string ds_string, string table_string) {
        if (table_lookup && (ds_string !~ /:/)) {
            return DbDataProvider::table_lookup(ds_string, table_string);
        }
        AbstractDatasource ds = DbDataProvider::getDatasource(ds_string);
        return AbstractTable::getTable(ds, table_string);
    }

    #! Returns an AbstractTable object from the given datasource and table arguments
    static AbstractTable getTable(AbstractDatasource ds, string table_string) {
        if (table_lookup) {
            return DbDataProvider::table_lookup(ds, table_string);
        }
        return AbstractTable::getTable(ds, table_string);
    }
}
}
