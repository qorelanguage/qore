# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DbDataProvider class definition

/** DbDataProvider.qc Copyright 2019 - 2023 Qore Technologies, s.r.o.

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
            "child_create_options": ChildCreateOptions,
            "transaction_management": True,
            "has_record": True,
            "record_requires_search_options": True,
            "supports_child_create": True,
            "supports_child_delete": True,
            "children_can_support_apis": False,
            "children_can_support_records": True,
            "children_can_support_transactions": True,
        };

        #! Constructor options
        const ConstructorOptions = {
            "datasource": <DataProviderOptionInfo>{
                "type": (
                    AbstractDataProviderType::get(StringType),
                    AbstractDataProviderType::get(new Type("AbstractDatasource")),
                ),
                "desc": "the datasource connection string or an abstract datasource object",
                "required": True,
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

        #! Child provider creation options
        const ChildCreateOptions = {
            "primary_key": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get("softlist<string>"),
                "desc": "a list of column names for the primary key for the table; use `primary_key_name` to set the "
                    "name of the primary key constraint manually, otherwise a suitable name will be automatically "
                    "generated",
            },
            "primary_key_name": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the name of the primary key constraint; only used if `primary_key` is set",
            },
            "indexes": <DataProviderOptionInfo>{
                "type": new DbIndexesDataType(),
                "desc": "a hash of indexes keyed by index name",
            },
            "foreign_constraints": <DataProviderOptionInfo>{
                "type": new DbForeignConstraintsDataType(),
                "desc": "a hash of foreign constraints keyed by foreign constraint name",
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

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("Data provider for database `%s`; use the search API with the `sql` and `args` arguments to "
            "execute record-based queries", db.getDatasourceDesc());
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
    private AbstractDataProviderBulkRecordInterface searchRecordsBulkImpl(int block_size = 1000,
            *hash<auto> where_cond, *hash<auto> search_options) {
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

    #! Return data provider summary info
    *list<hash<DataProviderSummaryInfo>> getChildProviderSummaryInfo() {
        return map cast<hash<DataProviderSummaryInfo>>(DbTableDataProvider::ProviderInfo{DataProviderSummaryInfoKeys}
            + {
            "name": $1,
            "desc": sprintf("Record-based data provider for db table `%s`; supports create, read/search, update, delete, "
                "upsert, and bulk operations", $1),
            "type": "DbTableDataProvider",
            "supports_children": False,
            "children_can_support_records": False,
        }), getChildProviderNames();
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
            rethrow "CHILD-PROVIDER-ERROR", sprintf("table %y cannot be acquired from %y: %s: %s", name, getName(),
                ex.err, ex.desc), ex.arg;
        }
        return new DbTableDataProvider(table, logger);
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
        hash<auto> table_desc = makeTableDesc(name, fields, child_create_options);
        AbstractTable table = db.makeTable(name, table_desc);
        DbTableTransactionHelper th(table);
        th.create();
        if (logger) {
            logger.info("DbDataProvider %y DDL: %s", name, table.getCreateSqlString());
        }
        return new DbTableDataProvider(table, logger);
    }

    #! Deletes a child data provider
    /** @param name the name of the new child data provider
        @param child_delete_options the options for deleting a child data provider

        @throw DELETE-CHILD-PROVIDER-ERROR error dropping the child data provider

        @since DataProvider 2.0
    */
    private deleteChildProviderImpl(string name, *hash<auto> child_delete_options) {
        *AbstractTable t = db.getTable(name);
        if (!t) {
            throw "NO-CHILD", sprintf("%s has no child %y", getName(), name);
        }
        DbTableTransactionHelper th(t);
        if (logger) {
            logger.info("DbDataProvider %y DDL: %s", name, (foldl $1 + "\n" + $2, t.getDropSql()));
        }
        th.drop();
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
    static hash<GenericColumnInfo> getColumnDesc(AbstractDatabase db, AbstractDataField field, *hash<auto> opts) {
        AbstractDataProviderType type = field.getType();
        hash<GenericColumnInfo> col({
            "default_value": field.getDefaultValue(),
            "notnull": !field.isAssignableFrom(Reflection::NothingType),
        });

        string type_name = type.getName();
        switch (type.getBaseTypeCode()) {
            case NT_INT:
                type_name = "integer";
                break;

            case NT_STRING:
                if (*int size = field.getType().getOptionValue("string.max_size_chars")) {
                    col.size = size;
                }
                break;

            case NT_NUMBER:
                if (db.requiresScale()) {
                    col.size = db.getMaximumPrecision();
                    col.scale = col.size / 3;
                }
                break;
        }
        col.qore_type = type_name;

        # add field description as a comment if possible
        if (*string desc = field.getDescription()) {
            col.comment = desc;
        }
        return col;
    }

    #! Returns a table description hash from a field description
    /** @throw TABLE-DESC-ERROR error in table description
    */
    hash<auto> makeTableDesc(string name, hash<string, AbstractDataField> fields, *hash<auto> opts) {
        hash<string, hash<GenericColumnInfo>> cols = map {$1.key: getColumnDesc(db, $1.value, opts)},
            fields.pairIterator();

        hash<auto> tdesc += {
            "columns": cols,
        };
        # process PK
        if (opts.primary_key) {
            foreach auto cname in (opts.primary_key) {
                if (cname.typeCode() != NT_STRING) {
                    throw "TABLE-DESC-ERROR", sprintf("table %y primary key column value %d/%d (starting from 1) is "
                        "type %y; expecting \"string\"", name, $# + 1, opts.primary_key.size(), name.type());
                }
                if (!fields{cname}) {
                    throw "TABLE-DESC-ERROR", sprintf("table %y primary key column %y is not present in the input "
                        "field description; known fields: %y", name, cname, keys fields);
                }
            }
            string pk_name = opts.primary_key_name ?? sprintf("pk_%s", name);
            tdesc.primary_key = {
                "name": pk_name,
                "columns": opts.primary_key,
            };
        }
        # process indexes
        if (opts.indexes) {
            tdesc.indexes = opts.indexes;
        }
        # process foreign_constraints
        if (opts.foreign_constraints) {
            tdesc.foreign_constraints = opts.foreign_constraints;
        }

        return tdesc;
    }
}
}
