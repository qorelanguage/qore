# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore ElasticSearchDataProvider module definition

/** ElasticSearchIndexesDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the ElasticSearchDataProvider module
public namespace ElasticSearchDataProvider {
#! The REST client GET data provider class
public class ElasticSearchIndexTableDataProvider inherits AbstractDataProvider {
    public {
        #! REST client connection
        RestClient rest;

        #! Current index name
        string name;

        #! Index hash
        hash<auto> index;

        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "desc": "ElasticSearch index table data provider; provdes table-like access to an ElasticSearch index",
            "type": "ElasticSearchIndexTableDataProvider",
            "constructor_options": ElasticSearchDataProvider::ConstructorOptions,
            "has_record": True,
            "supports_read": True,
            #"supports_native_search": True,
            #"supports_update": True,
            #"supports_delete": True,
            #"supports_create": True,
            #"supports_search_expresions": True,
            #"expressions": (map {$1.key: $1.value.exp}, Expressions.pairIterator()),
        };

        /*
        #! Supported expressions
        const Expressions = {
            DP_OP_AND: {
                "exp": AbstractDataProvider::GenericExpressions{DP_OP_AND},
                "impl": string sub (string cn, auto arg) {
                    throw "WHERE-ERROR", "not implemented";
                },
            },
            QUERY_OP_EQ: {
                "exp": sym(AbstractDataProvider::GenericExpressions{DP_SEARCH_OP_EQ}, QUERY_OP_EQ),
                "impl": string sub (string cn, auto arg) {
                    return sprintf("%s=%s", cn, arg);
                },
            },
        };
        */

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });

        #! ElasticSearch property types to data types
        const TypeMap = {
            "binary": AbstractDataProviderTypeMap."base64binary",

            "boolean": AbstractDataProviderTypeMap."boolean",

            "keyword": AbstractDataProviderTypeMap."string",
            "constant_keyword": AbstractDataProviderTypeMap."string",
            "wildcard": AbstractDataProviderTypeMap."string",

            "ip": AbstractDataProviderTypeMap."string",
            "version": AbstractDataProviderTypeMap."string",
            "mumur3": AbstractDataProviderTypeMap."string",

            "text": AbstractDataProviderTypeMap."string",
            "match_only_text": AbstractDataProviderTypeMap."string",

            "byte": AbstractDataProviderTypeMap."int",
            "short": AbstractDataProviderTypeMap."int",
            "integer": AbstractDataProviderTypeMap."int",
            "long": AbstractDataProviderTypeMap."int",

            "unsigned_long": AbstractDataProviderTypeMap."number",

            "date": AbstractDataProviderTypeMap."date",
            # NOTE: lossy conversion here; Qore only supports microsecond resolution
            "date_nanos": AbstractDataProviderTypeMap."date",

            "double": AbstractDataProviderTypeMap."float",
            "float": AbstractDataProviderTypeMap."float",
            "half_float": AbstractDataProviderTypeMap."float",
            "scaled_float": AbstractDataProviderTypeMap."float",

            "object": AbstractDataProviderTypeMap."hash",
            "flattened": AbstractDataProviderTypeMap."hash",

            "nested": AbstractDataProviderTypeMap."list",
        };
    }

    private {
        #! The record type for the object
        hash<string, AbstractDataField> record_type = {
            "_id": new QoreDataField("_id", "the document ID", AbstractDataProviderTypeMap."string"),
            "_score": new QoreDataField("_score", "the score for the match", AbstractDataProviderTypeMap."float"),
        };
    }

    #! Creates the object from a REST connection
    constructor(RestClient rest, string name, hash<auto> index) {
        self.rest = rest;
        self.name = name;
        self.index = index;
        getRecordInfoIntern();
    }

    #! Returns the data provider name
    string getName() {
        return name;
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("ElasticSearch data provider for index `%s` for `%s`", name, rest.getSafeURL());
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()

        @see requestSearchRecordsImpl()
    */
    private DataProvider::AbstractDataProviderRecordIterator searchRecordsImpl(*hash<auto> where_cond,
            *hash<auto> search_options) {
        return new ElasticSearchRecordIterator(rest, name, index, record_type, where_cond, search_options);
    }

    #! Returns data provider static info
    hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }

    #! Returns a data type for a property
    DataProvider::AbstractDataProviderType getTypeForProperty(string name, hash<auto> prop) {
        if (prop.properties) {
            HashDataType type();
            map type.addField(new QoreDataField($1.key, NOTHING,
                ElasticSearchIndexTableDataProvider::getTypeForProperty($1.key, $1.value))),
                prop.properties.pairIterator();
            return type;
        }

        return TypeMap{prop.type} ?? AbstractDataProviderTypeMap."any";
    }

    #! Retrieves the record type
    private getRecordInfoIntern() {
        foreach hash<auto> i in (index.mappings.properties.pairIterator()) {
            # skip aliases
            if (i.value.type == "alias") {
                continue;
            }
            record_type{i.key} = new QoreDataField(i.key, NOTHING, getTypeForProperty(i.key, i.value));
        }
    }

    private *hash<string, DataProvider::AbstractDataField> getRecordTypeImpl(*hash<auto> search_options) {
        return record_type;
    }
}
}
