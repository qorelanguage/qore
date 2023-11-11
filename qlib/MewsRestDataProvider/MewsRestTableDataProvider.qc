# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore MewsRestDataProvider module definition

/** MewsRestTableDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! contains all public definitions in the MewsRestDataProvider module
public namespace MewsRestDataProvider {
#! contains Mews object record information
hashdecl MewsRestRecordInfo {
    #! The record type for the object
    hash<string, AbstractDataField> record_type;

    #! The original field type names
    hash<string, string> field_types;

    /*
    #! Hash of date/time fields for this record
    hash<string, bool> date_time_fields;

    #! Hash of boolean fields for this record
    hash<string, bool> bool_fields;

    #! Hash of integer fields for this record
    hash<string, bool> int_fields;
    */
}

#! The MewsRestTableDataProvider data provider class
public class MewsRestTableDataProvider inherits MewsRestDataProviderBase {
    public {
        #! Constructor options
        const ConstructorOptions = MewsRestDataProvider::ConstructorOptions + {
            "table": <DataProviderOptionInfo>{
                "display_name": "Table",
                "short_desc": "The table name",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The table name",
                "required": True,
            },
        };

        #! Search options
        const SearchOptions = {
            "columns": <DataProviderOptionInfo>{
                "display_name": "Columns",
                "short_desc": "Columns to include in the output of the query",
                "type": AbstractDataProviderType::get(AbstractDataProviderType::anyType),
                "desc": "Columns to include in the output of the query; `select` argument for the query expression",
            },
            "limit": <DataProviderOptionInfo>{
                "display_name": "Limit",
                "short_desc": "The maximum number of records to return",
                "type": AbstractDataProviderType::get(IntType),
                "desc": "The maximum number of records to return",
            },
            "offset": <DataProviderOptionInfo>{
                "display_name": "Offset",
                "short_desc": "The offset in the record list for records to return",
                "type": AbstractDataProviderType::get(IntType),
                "desc": "The offset in the record list for records to return",
            },
            "groupby": <DataProviderOptionInfo>{
                "display_name": "Group By",
                "short_desc": "Field(s) to group results by",
                "type": new QoreListDataType(new Type("softlist<auto>")),
                "desc": "Field(s) to group results by in case there are aggregate expressions in the query; the "
                    "`GROUPBY` argument for the query expression",
            },
            "orderby": <DataProviderOptionInfo>{
                "display_name": "Order By",
                "short_desc": "Fields to order the query result by",
                "type": new QoreListDataType(new Type("softlist<string>")),
                "desc": "Fields to order the query result by; the `ORDERBY` argument for the query expression",
            },
            "orderbydesc": <DataProviderOptionInfo>{
                "display_name": "Order By Descending",
                "short_desc": "Fields to order the query result in descending order by",
                "type": new QoreListDataType(new Type("softlist<string>")),
                "desc": "Fields to order the query result in descending order by; the `ORDERBYDESC` argument for "
                    "the query expression",
            },
        };
    }

    private {
        #! The table name
        string table;

        #! Record info for the table
        hash<MewsRestRecordInfo> record_info;
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        rest = MewsRestDataProvider::getRestConnection(copts);
        table = copts.table;
        getRecordInfoIntern();
    }

    #! Creates the object from the arguments
    constructor(MewsRestClient rest, string table) : MewsRestDataProviderBase(rest){
        self.table = table;
        getRecordInfoIntern();
    }

    #! Returns the data provider name
    string getName() {
        return table;
    }

    private *hash<string, DataProvider::AbstractDataField> getRecordTypeImpl(*hash<auto> search_options) {
        return record_info.record_type;
    }

    #! Retrieves the record type
    private getRecordInfoIntern() {
        record_info = getRecordInfoInternImpl();
    }

    #! Returns an equality comparison expression
    private hash<DataProviderExpression> getEqualityComparisonExpression(int role, hash<DataProviderInfo> info,
            string key, auto value) {
        return <DataProviderExpression>{
            "exp": info.expressions.eq.label,
            "args": (<DataProviderFieldReference>{"field": key}, value),
        };
    }

    #! Retrieve the record information
    abstract private hash<MewsRestRecordInfo> getRecordInfoInternImpl();
}
}
