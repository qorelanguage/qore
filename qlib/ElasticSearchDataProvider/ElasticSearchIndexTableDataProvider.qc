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
            "desc": "ElasticSearch index table data provider; provdes table-like access to an ElasticSearch index; "
                "fields returned in searches are document properties, plus the special `_id` (document ID) and "
                "`_score` (query match score) values, which are read-only.\n\n"
                "Updating and deleting can be performed, but the 'where clause hash` must contain only a single "
                "`_id` key to identify the document by its ID.",
            "type": "ElasticSearchIndexTableDataProvider",
            "constructor_options": ElasticSearchDataProvider::ConstructorOptions,
            "has_record": True,
            "supports_read": True,
            "supports_native_search": True,
            "supports_update": True,
            "supports_delete": True,
            "supports_create": True,
            "create_options": {
                "if_seq_no": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."int",
                    "desc": "Only perform the operation if the document has this sequence number",
                },
                "if_primary_term": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."int",
                    "desc": "Only perform the operation if the document has this primary term",
                },
                "op_type": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "Set to create to only index the document if it does not already exist (put if absent). If a "
                        "document with the specified `_id` already exists, the indexing operation will fail. Same as "
                        "using the `<index>/_create` endpoint. Valid values: `index`, `create`. If document id is "
                        "specified, it defaults to `index`. Otherwise, it defaults to `create`",
                },
                "pipeline": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "ID of the pipeline to use to preprocess incoming documents. If the index has a default "
                        "ingest pipeline specified, then setting the value to _none disables the default ingest pipeline "
                        "for this request. If a final pipeline is configured it will always run, regardless of the value "
                        "of this parameter",
                },
                "refresh": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "If `true`, Elasticsearch refreshes the affected shards to make this operation visible to "
                        "search, if `wait_for` then wait for a refresh to make this operation visible to search, if "
                        "`false` do nothing with refreshes. Valid values: `true`, `false`, `wait_for`",
                },
                "routing": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "Custom value used to route operations to a specific shard",
                },
                "timeout": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "Period to wait for a response (ex: `1m`). If no response is received before the "
                        "timeout expires, the request fails and returns an error",
                },
                "version": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."int",
                    "desc": "Explicit version number for concurrency control. The specified version must match the "
                        "current version of the document for the request to succeed",
                },
                "version_type": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "Specific version type: `external`, `external_gte`",
                },
                "wait_for_active_shards": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "The number of shard copies that must be active before proceeding with the operation. Set to "
                        "`all` or any positive integer up to the total number of shards in the index (number_of_replicas+1)",
                },
                "require_alias": <DataProviderOptionInfo>{
                    "type": SoftBoolDataProviderStringType,
                    "desc": "If `true`, the destination must be an index alias",
                },
            },
            "search_options": {
                "if_seq_no": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."int",
                    "desc": "Only perform the operation if the document has this sequence number\n"
                        "(update only)",
                },
                "if_primary_term": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."int",
                    "desc": "Only perform the operation if the document has this primary term\n"
                        "(update only)",
                },
                "lang": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "The script language\n"
                        "(update only)",
                },
                "operator": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "Boolean logic used to interpret text in the query value. Valid values are:\n"
                        "- `or` (Default): For example, a query value of `capital of Hungary` is interpreted as "
                            "`capital OR of OR Hungary`\n"
                        "- `and`: For example, a query value of `capital of Hungary` is interpreted as "
                            "`capital AND of AND Hungary`\n"
                        "(search only)",
                    "allowed_values": (
                        <AllowedValueInfo>{
                            "value": "or",
                            "desc": "Or logic - any terms match (default)",
                        },
                        <AllowedValueInfo>{
                            "value": "and",
                            "desc": "And logic - all terms must match",
                        }
                    ),
                },
                "require_alias": <DataProviderOptionInfo>{
                    "type": SoftBoolDataProviderStringType,
                    "desc": "If `true`, the destination must be an index alias\n"
                        "(update only)",
                },
                "refresh": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "If `true`, Elasticsearch refreshes the affected shards to make this operation visible to "
                        "search, if `wait_for` then wait for a refresh to make this operation visible to search, if "
                        "`false` do nothing with refreshes. Valid values: `true`, `false`, `wait_for`\n"
                        "(update only)",
                },
                "retry_on_conflict": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."int",
                    "desc": "Specify how many times should the operation be retried when a conflict occurs\n"
                        "(update only)",
                },
                "routing": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "Custom value used to route operations to a specific shard\n"
                        "(update only)",
                },
                "_source": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "Set to `false` to disable source retrieval (default: `true`). You can also specify a "
                        "comma-separated list of the fields you want to retrieve\n"
                        "(update only)",
                },
                "_source_excludes": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "Specify the source fields you want to exclude\n"
                        "(update only)",
                },
                "_source_includes": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "Specify the source fields you want to exclude\n"
                        "(update only)",
                },
                "timeout": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "Period to wait for a response (ex: `1m` = 1 minute). If no response is received before "
                        "the timeout expires, the request fails and returns an error",
                },
                "version": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "",
                },
                "wait_for_active_shards": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "The number of shard copies that must be active before proceeding with the operation. "
                        "Set to `all` or any positive integer up to the total number of shards in the index "
                        "(number_of_replicas+1)\n"
                        "(update only)",
                },
            },
        };

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

        #! Search query options
        const SearchQueryOptions = ("timeout",);
    }

    private {
        #! The record type for the object
        hash<string, AbstractDataField> record_type = {
            "_id": new QoreDataField("_id", "the document ID", AbstractDataProviderTypeMap."softstring"),
            "_score": new QoreDataField("_score", "the score for the match (read-only)",
                AbstractDataProviderTypeMap."float"),
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

    #! Updates a single document in the index
    /** @param set the hash of field data to set
        @param where_cond a hash for identifying the record(s) to be updated
        @param search_options the search options (see @ref SearchOptions) after processing by validateSearchOptions()

        @return the number of records updated

        @note this API will commit the row to the database unless the datasource was already in a transaction before
        this call
    */
    private int updateRecordsImpl(hash<auto> set, *hash<auto> where_cond, *hash<auto> search_options) {
        # NOTE: this call will throw an exception if the update operation fails
        rest.post(getDocUriPath("_update", where_cond, "update", search_options,
            ElasticSearchDocumentUpdateDataProvider::QueryArgs), {"doc": set});
        return 1;
    }

    /**
        @param where_cond a hash for identifying the record(s) to be deleted; can only contain the \c "_id" field
        @param search_options the search options (see @ref SearchOptions) after processing by validateSearchOptions();
        ignored for this API call

        @return the number of records deleted

        @throw INVALID-OPERATION the data provider does not support record updating
    */
    private int deleteRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        # NOTE: this call will throw an exception if the delete operation fails
        rest.del(getDocUriPath("_doc", where_cond, "delete", search_options,
            ElasticSearchDocumentDeleteDataProvider::QueryArgs));
        return 1;
    }

    #! Writes the given record to the data provider
    /** @param rec a hash representing a single input record
        @param create_options the create options (see @ref CreateOptions) after processing by validateCreateOptions()

        @return the data written to the data provider plus any output (returning) variables

        @note this API will commit the row to the database unless the datasource was already in a transaction before
        this call
    */
    private *hash<auto> createRecordImpl(hash<auto> rec, *hash<auto> create_options) {
        string uri = sprintf("%s/_doc", name);
        if (create_options) {
            uri = getQueryOptions(uri, create_options, ElasticSearchDocumentCreateDataProvider::QueryArgs);
        }

        # NOTE: this call will throw an exception if the create operation fails
        string id = rest.post(uri, rec).body._id;
        return {"_id": id} + rec;
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

    #! Returns a URI path with query options
    static string getQueryOptions(string uri, hash<auto> search_options, list<string> query_args) {
        bool q;
        foreach string arg in (query_args) {
            if (!exists search_options{arg}) {
                continue;
            }
            if (!q) {
                uri += "?";
                q = True;
            } else {
                uri += "&";
            }
            uri += sprintf("%s=%s", arg, search_options{arg});
        }
        return uri;
    }

    #! Makes sure that the where_cond is only the _id field
    private string getDocUriPath(string uri_str, *hash<auto> where_cond, string action, *hash<auto> search_options,
            *list<string> query_args) {
        if (!where_cond._id.val()) {
            throw "ELASTICSEARCH-" + action.upr() + "-ERROR", sprintf("missing the '_id' field to identify the "
                "document to %s in index %y", action, name);
        }
        if (where_cond._id.typeCode() != NT_STRING) {
            throw "ELASTICSEARCH-" + action.upr() + "-ERROR", sprintf("the '_id' field to identify the document to "
                "%s in index %y must be a string; got type %y instead", action, name, where_cond._id.type());
        }
        if (where_cond.size() > 1) {
            throw "ELASTICSEARCH-" + action.upr() + "-ERROR", sprintf("only one '_id' field can be used to identify "
                "the document to %s in index %y; got %d fields (%y)", action, name, where_cond.size(),
                keys where_cond);
        }
        string rv = sprintf("%s/%s/%s", name, uri_str, where_cond._id);
        return search_options && query_args ? getQueryOptions(rv, search_options, query_args) : rv;
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
