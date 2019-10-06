# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DbDataProvider class definition

/** DbDataProvider.qc Copyright 2019 Qore Technologies, s.r.o.

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
            "supports_read": False,
            "supports_create": False,
            "supports_update": False,
            "supports_upsert": False,
            "supports_delete": False,
            "supports_native_search": False,
            "supports_bulk_create": False,
            "supports_bulk_upsert": False,
            "constructor_options": ConstructorOptions,
            "search_options": NOTHING,
            "create_options": NOTHING,
            "upsert_options": NOTHING,
            "transaction_management": False,
        };

        #! Constructor options
        const ConstructorOptions = {
            "datasource": <DataProviderOptionInfo>{
                "type": (StringType, new Type("AbstractDatasource")),
                "desc": "the datasource connection string or an abstract datasource object",
            },
            "database": <DataProviderOptionInfo>{
                "type": new Type("AbstractDatabase"),
                "desc": "the database for the object",
            },
        };
    }

    private {
        #! The database object
        AbstractDatabase db;
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
            self.db = copts.db;
            return;
        }
        if (!copts.datasource) {
            error("CONSTRUCTOR-ERROR", "option \"datasource\" is required if no \"database\" option is provided");
        }
        AbstractDatasource ds;
        if (copts.datasource instanceof AbstractDatasource) {
            ds = copts.datasource;
        } else {
            ds = new Datasource(copts.datasource);
        }
        db = AbstractDatabase::getDatabase(ds);
    }

    #! Returns the data provider name
    string getName() {
        return db.getDatasourceDesc();
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        return db.listTables();
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @throw INVALID-CHILD-PROVIDER unknown child provider

        @see getChildProviderEx()
    */
    private *AbstractDataProvider getChildProviderImpl(string name) {
        AbstractTable table;
        try {
            table = db.getTable(name);
        } catch (hash<ExceptionInfo> ex) {
            throw "INVALID-CHILD-PROVIDER", sprintf("table %y does not exist in %y: %s", name, getName(), ex.desc), ex.arg;
        }
        return new DbTableDataProvider(table);
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}
