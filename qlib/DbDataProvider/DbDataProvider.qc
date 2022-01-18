# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DbDataProvider class definition

/** DbDataProvider.qc Copyright 2019 - 2022 Qore Technologies, s.r.o.

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
            "supports_child_create": True,
            "supports_child_delete": True,
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

    #! Begins a transaction in the datasource
    /**
        @see:
        - commit()
        - rollback()

        @since DbDataProvider 1.0.1
    */
    beginTransaction() {
        db.beginTransaction();
    }

    #! Commits any transaction in progress in the datasource
    /**
        @see:
        - beginTransaction()
        - rollback()
    */
    commit() {
        db.commit();
    }

    #! Rolls back any transaction in progress in the datasource
    /**
        @see:
        - beginTransaction()
        - commit()
    */
    rollback() {
        db.rollback();
    }

    #! Returns the record type description from a describe hash
    /** @param describe_hash a hash as returned by
        @ref Qore::SQL::AbstractSQLStatement::describe() "AbstractSQLStatement::describe()"

        @return the record type of the query results
    */
    static *hash<string, AbstractDataField> getRecordTypeFromDescribeHash(hash<auto> describe_hash) {
        HashDataType input_data();
        foreach hash<auto> i in (describe_hash.pairIterator()) {
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
        return getRecordTypeFromDescribeHash(stmt.describe());
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
            throw "CHILD-PROVIDER-ERROR", sprintf("table %y cannot be acquired from %y: %s: %s", name, getName(),
                ex.err, ex.desc), ex.arg;
        }
        return new DbTableDataProvider(table);
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }

    #! Creates a new child data provider and returns it after adding as a child
    /** @param name the name of the new child data provider
        @param fields the fields for the new child data provider
        @param child_create_options the options for creating the new child data provider

        @return the new child data provider

        @since DataProvider 2.0
    */
    private AbstractDataProvider createChildProviderImpl(string name, hash<string, AbstractDataField> fields,
            *hash<auto> child_create_options) {
        hash<auto> table_desc = makeTableDesc(fields, child_create_options);
        AbstractTable table = db.makeTable(name, table_desc);
        table.create();
        return new DbTableDataProvider({
            "table": table,
        });
    }

    #! Deletes a child data provider
    /** @param name the name of the new child data provider
        @param child_delete_options the options for deleting a child data provider

        @throw DELETE-CHILD-PROVIDER-ERROR error dropping the child data provider

        @since DataProvider 2.0
    */
    private deleteChildProviderImpl(string name, *hash<auto> child_delete_options) {
        bool dropped;
        {
            on_error rethrow "DELETE-CHILD-PROVIDER-ERROR", sprintf("%s: %s", $1.err, $1.desc), $1.arg;
            dropped = db.dropTableIfExists(name);
        }
        if (!dropped) {
            throw "DELETE-CHILD-PROVIDER-ERROR", sprintf("%s has no child %y", getName(), name);
        }
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

    #! Returns a column description hash for a field
    static hash<GenericColumnInfo> getColumnDesc(AbstractDataField field, *hash<auto> opts) {
        string type_name = field.getTypeName();
        if (type_name == "int") {
            type_name = "integer";
        }
        hash<GenericColumnInfo> col({
            "qore_type": type_name,
            "default_value": field.getDefaultValue(),
            "notnull": !field.isAssignableFrom(Reflection::NothingType),
        });
        if (*string desc = field.getDescription()) {
            col.comment = desc;
        }
        return col;
    }

    #! Returns a table description hash from a field description
    static hash<auto> makeTableDesc(hash<string, AbstractDataField> fields, *hash<auto> opts) {
        hash<string, hash<GenericColumnInfo>> cols = map {$1.key: DbDataProvider::getColumnDesc($1.value, opts)},
            fields.pairIterator();

        return {
            "columns": cols,
        };
    }
}
}
