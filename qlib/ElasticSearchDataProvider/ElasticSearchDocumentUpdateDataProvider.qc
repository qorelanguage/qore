# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore ElasticSearchDataProvider module definition

/** ElasticSearchDocumentUpdateDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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
#! The ElasticSearch update data provider API
public class ElasticSearchDocumentUpdateDataProvider inherits AbstractDataProvider {
    public {
        #! REST client connection
        RestClient rest;

        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "update",
            "desc": "ElasticSearch document update API data provider",
            "type": "ElasticSearchDocumentUpdateDataProvider",
            "constructor_options": ElasticSearchDataProvider::ConstructorOptions,
            "supports_request": True,
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });

        #! Request type
        const RequestType = new ElasticSearchDocumentUpdateRequestDataType();

        #! Response type
        const ResponseType = new ElasticSearchDocumentUpdateResponseDataType();

        #! Query args
        const QueryArgs = ("if_seq_no", "if_primary_term", "lang", "require_alias", "refresh", "retry_on_conflict",
            "routing", "_source", "_source_excludes", "_source_includes", "timeout", "wait_for_active_shards");
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        checkOptions("CONSTRUCTOR-ERROR", ElasticSearchDataProvider::ConstructorOptions, options);
        rest = ElasticSearchDataProvider::getRestConnection(options);
    }

    #! Creates the object from a REST connection
    constructor(RestClient rest) {
        self.rest = rest;
    }

    #! Returns the data provider name
    string getName() {
        return ProviderInfo.name;
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("ElasticSearch data document %s provider for `%s`", ProviderInfo.name, rest.getSafeURL());
    }

    #! Makes a request and returns the response
    /** @param req the request to serialize and make according to the request type
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        string uri_path = sprintf("%s/_update/%s", remove req.index, remove req.id);
        bool q;
        foreach string arg in (QueryArgs) {
            if (!exists req{arg}) {
                continue;
            }
            if (!q) {
                uri_path += "?";
                q = True;
            } else {
                uri_path += "&";
            }
            uri_path += sprintf("%s=%s", arg, remove req{arg});
        }
        return rest.post(uri_path, req).body;
    }

    #! Returns the description of a successful request message, if any
    /** @return the request type for this provider
    */
    private *AbstractDataProviderType getRequestTypeImpl() {
        return RequestType;
    }

    #! Returns the description of a response message, if this object represents a response message
    /** @return the response type for this response message
    */
    private *AbstractDataProviderType getResponseTypeImpl() {
        return ResponseType;
    }

    #! Returns data provider static info
    hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}

#! Update document request data type
public class ElasticSearchDocumentUpdateRequestDataType inherits HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            # query parameters
            "index": {
                "type": StringType,
                "desc": "The name of the index to use",
            },
            "id": {
                "type": StringType,
                "desc": "The ID of the document to update",
            },
            "if_seq_no": {
                "type": IntOrNothingType,
                "desc": "Only perform the operation if the document has this sequence number",
            },
            "if_primary_term": {
                "type": IntOrNothingType,
                "desc": "Only perform the operation if the document has this primary term",
            },
            "lang": {
                "type": StringOrNothingType,
                "desc": "The script language",
                "default_value": "painless",
            },
            "require_alias": {
                "type": SoftBoolStringType,
                "desc": "If `true`, the destination must be an index alias",
                "default_value": False,
            },
            "refresh": {
                "type": StringType,
                "desc": "If `true`, Elasticsearch refreshes the affected shards to make this operation visible to "
                    "search, if `wait_for` then wait for a refresh to make this operation visible to search, if "
                    "`false` do nothing with refreshes. Valid values: `true`, `false`, `wait_for`",
                "default_value": "false",
            },
            "retry_on_conflict": {
                "type": IntType,
                "desc": "Specify how many times should the operation be retried when a conflict occurs",
                "default_value": 0,
            },
            "routing": {
                "type": StringOrNothingType,
                "desc": "Custom value used to route operations to a specific shard",
            },
            "_source": {
                "type": StringType,
                "desc": "Set to `false` to disable source retrieval (default: `true`). You can also specify a "
                    "comma-separated list of the fields you want to retrieve",
                "default_value": "true",
            },
            "_source_excludes": {
                "type": StringOrNothingType,
                "desc": "Specify the source fields you want to exclude",
            },
            "_source_includes": {
                "type": StringOrNothingType,
                "desc": "Specify the source fields you want to exclude",
            },
            "timeout": {
                "type": StringType,
                "desc": "Period to wait for the following operations:\n"
                    "- Dynamic mapping updates\n"
                    "- Waiting for active shards\n\n"
                    "This guarantees Elasticsearch waits for at least the timeout before failing. The actual wait "
                    "time could be longer, particularly when multiple waits occur",
                "default_value": "1m",
            },
            "wait_for_active_shards": {
                "type": StringType,
                "desc": "The number of shard copies that must be active before proceeding with the operation. Set to "
                    "`all` or any positive integer up to the total number of shards in the index (number_of_replicas+1)",
                "default_value": "1",
            },

            # body parameters
            "script": {
                "type": AutoType,
                "desc": "the script to run",
            },
            "doc": {
                "type": AutoHashOrNothingType,
                "desc": "Updates the document with the given hash; ignored if `script` is also provided",
            },
            "detect_noop": {
                "type": BoolOrNothingType,
                "desc": "Ignore noops",
            },
            "doc_as_upsert": {
                "type": BoolOrNothingType,
                "desc": "Instead of sending a partial doc plus an upsert doc, you can set `doc_as_upsert` to `true` "
                    "to use the contents of `doc` as the upsert value",
            },
            "scripted_upsert": {
                "type": BoolOrNothingType,
                "desc": "To run the script whether or not the document exists",
            },
            "upsert": {
                "type": AutoHashOrNothingType,
                "desc": "Hash to insert as a new document if the document did not exist previously",
            },
        };
    }

    #! Creates the object
    constructor() {
        map addField(new QoreDataField($1.key, $1.value.desc, $1.value.type, $1.value.default_value)),
            Fields.pairIterator();

        # body field values must be JSON strings
        setDefaultOtherFieldType(AbstractDataProviderTypeMap."string");
    }
}

#! Document update API response
public class ElasticSearchDocumentUpdateResponseDataType inherits HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            "_index": {
                "type": StringType,
                "desc": "The name of the index the document was added to",
            },
            "_id": {
                "type": StringType,
                "desc": "The unique identifier for the added document",
            },
            "_version": {
                "type": IntType,
                "desc": "The document version. Incremented each time the document is updated",
            },
            "result": {
                "type": StringType,
                "desc": "The result of the indexing operation, `noop` or `updated`",
            },
            "_shards": {
                "type": new ElasticSearchDocumentCreateShardsDataType(),
                "desc": "Provides information about the replication process of the index operation",
            },
            "_seq_no": {
                "type": IntType,
                "desc": "The sequence number assigned to the document for the indexing operation. Sequence numbers "
                    "are used to ensure an older version of a document doesn’t overwrite a newer version",
            },
            "_primary_term": {
                "type": IntType,
                "desc": "The primary term assigned to the document for the indexing operation",
            },
            "get": {
                "type": AutoHashOrNothingType,
                "desc": "Information about the document after the update"
            },
        };
    }

    #! Creates the object
    constructor() {
        map addField(new QoreDataField($1.key, $1.value.desc, $1.value.type, $1.value.default_value)),
            Fields.pairIterator();
    }
}
}
