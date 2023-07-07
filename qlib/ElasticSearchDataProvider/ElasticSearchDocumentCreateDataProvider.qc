# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore ElasticSearchDataProvider module definition

/** ElasticSearchDocumentCreateDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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
public class ElasticSearchDocumentCreateDataProvider inherits AbstractDataProvider {
    public {
        #! REST client connection
        RestClient rest;

        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "create",
            "desc": "ElasticSearch document create API data provider",
            "type": "ElasticSearchDocumentCreateDataProvider",
            "constructor_options": ElasticSearchDataProvider::ConstructorOptions,
            "supports_request": True,
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });

        #! Request type
        const RequestType = new ElasticSearchDocumentCreateRequestDataType();

        #! Response type
        const ResponseType = new ElasticSearchDocumentCreateResponseDataType();

        #! Query args
        const QueryArgs = ("if_seq_no", "if_primary_term", "op_type", "pipeline", "refresh", "routing", "timeout",
            "version", "version_type", "wait_for_active_shards", "require_alias");
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
        string uri_path = sprintf("%s/_doc", remove req.index);
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

#! Create document request data type
public class ElasticSearchDocumentCreateRequestDataType inherits HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            # query parameters
            "index": {
                "type": StringType,
                "desc": "The name of the index to use",
            },
            "if_seq_no": {
                "type": IntOrNothingType,
                "desc": "Only perform the operation if the document has this sequence number",
            },
            "if_primary_term": {
                "type": IntOrNothingType,
                "desc": "Only perform the operation if the document has this primary term",
            },
            "op_type": {
                "type": StringOrNothingType,
                "desc": "Set to create to only index the document if it does not already exist (put if absent). If a "
                    "document with the specified `_id` already exists, the indexing operation will fail. Same as "
                    "using the `<index>/_create` endpoint. Valid values: `index`, `create`. If document id is "
                    "specified, it defaults to `index`. Otherwise, it defaults to `create`",
            },
            "pipeline": {
                "type": StringOrNothingType,
                "desc": "ID of the pipeline to use to preprocess incoming documents. If the index has a default "
                    "ingest pipeline specified, then setting the value to _none disables the default ingest pipeline "
                    "for this request. If a final pipeline is configured it will always run, regardless of the value "
                    "of this parameter",
            },
            "refresh": {
                "type": StringType,
                "desc": "If `true`, Elasticsearch refreshes the affected shards to make this operation visible to "
                    "search, if `wait_for` then wait for a refresh to make this operation visible to search, if "
                    "`false` do nothing with refreshes. Valid values: `true`, `false`, `wait_for`",
                "default_value": "false",
            },
            "routing": {
                "type": StringOrNothingType,
                "desc": "Custom value used to route operations to a specific shard",
            },
            "timeout": {
                "type": StringType,
                "desc": "Period to wait for a response. If no response is received before the "
                    "timeout expires, the request fails and returns an error",
                "default_value": "1m",
            },
            "version": {
                "type": IntOrNothingType,
                "desc": "Explicit version number for concurrency control. The specified version must match the "
                    "current version of the document for the request to succeed",
            },
            "version_type": {
                "type": StringOrNothingType,
                "desc": "Specific version type: `external`, `external_gte`",
            },
            "wait_for_active_shards": {
                "type": StringType,
                "desc": "The number of shard copies that must be active before proceeding with the operation. Set to "
                    "`all` or any positive integer up to the total number of shards in the index (number_of_replicas+1)",
                "default_value": "1",
            },
            "require_alias": {
                "type": SoftBoolStringType,
                "desc": "If `true`, the destination must be an index alias",
                "default_value": False,
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

#! Shard data type
public class ElasticSearchDocumentCreateShardsDataType inherits HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            "total": {
                "type": IntType,
                "desc": "Indicates how many shard copies (primary and replica shards) the index operation should be "
                    "executed on",
            },
            "successful": {
                "type": IntType,
                "desc": "Indicates the number of shard copies the index operation succeeded on. When the index "
                    "operation is successful, `successful` is at least 1.\n\n"
                    "Replica shards might not all be started when an indexing operation returns successfully; ​by "
                    "default, only the primary is required. Set `wait_for_active_shards` to change this default "
                    "behavior",
            },
            "failed": {
                "type": IntType,
                "desc": "An array that contains replication-related errors in the case an index operation failed on "
                    "a replica shard. `0` indicates there were no failures.",
            },
        };
    }

    #! Creates the object
    constructor() {
        map addField(new QoreDataField($1.key, $1.value.desc, $1.value.type, $1.value.default_value)),
            Fields.pairIterator();
    }
}

#! Document create API response
public class ElasticSearchDocumentCreateResponseDataType inherits HashDataType {
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
                "desc": "The result of the indexing operation, `created` or `updated`",
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
            "_type": {
                "type": StringOrNothingType,
                "desc": "The document type. Elasticsearch indices now support a single document type, `_doc`"
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
