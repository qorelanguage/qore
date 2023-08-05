# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore ElasticSearchDataProvider module definition

/** ElasticSearchIndexReadDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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
#! The ElasticSearch index read API data provider
/** This class implements an API that reads index data given the index name as an argument; the response is the
    description of the index as returned by the ElasticSearch server.

    Note that wildcards can be used, in which case multiple indices can be returned in the return value.
*/
public class ElasticSearchIndexReadDataProvider inherits AbstractDataProvider {
    public {
        #! REST client connection
        RestClient::RestClient rest;

        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "read",
            "desc": "ElasticSearch index read API data provider",
            "type": "ElasticSearchIndexReadDataProvider",
            "constructor_options": ElasticSearchDataProvider::ConstructorOptions,
            "supports_request": True,
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });

        #! Request type
        const RequestType = new ElasticSearchIndexReadRequestDataType();

        #! Response type
        const ResponseType = new ElasticSearchIndexReadResponseDataType();

        #! Query args
        const QueryArgs = ("allow_no_indices", "expand_wildcards", "features", "flat_settings", "include_defaults",
            "ignore_unavailable", "local", "master_timeout");
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        checkOptions("CONSTRUCTOR-ERROR", ElasticSearchDataProvider::ConstructorOptions, options);
        rest = ElasticSearchDataProvider::getRestConnection(options);
    }

    #! Creates the object from a REST connection
    constructor(RestClient::RestClient rest) {
        self.rest = rest;
    }

    #! Returns the data provider name
    string getName() {
        return ProviderInfo.name;
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("ElasticSearch data index %s provider for `%s`", ProviderInfo.name, rest.getSafeURL());
    }

    #! Makes a request and returns the response
    /** @param req the request to serialize and make according to the request type
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        string uri_path = remove req.index;
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
        return rest.get(uri_path).body;
    }

    #! Returns the description of a successful request message, if any
    /** @return the request type for this provider
    */
    private *DataProvider::AbstractDataProviderType getRequestTypeImpl() {
        return RequestType;
    }

    #! Returns the description of a response message, if this object represents a response message
    /** @return the response type for this response message
    */
    private *DataProvider::AbstractDataProviderType getResponseTypeImpl() {
        return ResponseType;
    }

    #! Returns data provider static info
    hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}

#! ElasticSearch index read API request
class ElasticSearchIndexReadRequestDataType inherits DataProvider::HashDataType {
    public {
        #! Field descriptions
        const Fields = {
            # query parameters
            "index": {
                "type": StringType,
                "desc": "The name of the index to read",
            },
            "allow_no_indices": {
                "type": SoftBoolStringType,
                "desc": "f `false`, the request returns an error if any wildcard expression, index alias, or `_all` "
                    "value targets only missing or closed indices. This behavior applies even if the request targets "
                    "other open indices. For example, a request targeting `foo*,bar*` returns an error if an index "
                    "starts with `foo` but no index starts with `bar`",
            },
            "expand_wildcards": {
                "type": StringOrNothingType,
                "desc": "Type of index that wildcard patterns can match. If the request can target data streams, "
                    "this argument determines whether wildcard expressions match hidden data streams. Supports "
                    "comma-separated values, such as open,hidden. Valid values are:\n"
                    "- `all`: Match any data stream or index, including hidden ones\n"
                    "- `open`: (default) Match open, non-hidden indices. Also matches any non-hidden data stream.\n"
                    "- `closed`: Match closed, non-hidden indices. Also matches any non-hidden data stream. Data "
                        "streams cannot be closed\n"
                    "- `hidden`: Match hidden data streams and hidden indices. Must be combined with `open`, "
                        "`closed`, or both\n"
                    "- `none`: Wildcard patterns are not accepted",
            },
            "features": {
                "type": StringOrNothingType,
                "desc": "Return information about specific index features. Supports comma-separated values. Valid "
                    "values are: `aliases`, `mappings`, and `settings`.",
            },
            "flat_settings": {
                "type": SoftBoolStringType,
                "desc": "If `true`, returns settings in flat format",
            },
            "include_defaults": {
                "type": SoftBoolStringType,
                "desc": "If `true`, return all default settings in the response",
            },
            "ignore_unavailable": {
                "type": SoftBoolStringType,
                "desc": "If `false`, the request returns an error if it targets a missing or closed index",
            },
            "master_timeout": {
                "type": StringOrNothingType,
                "desc": "Period to wait for a connection to the master node. If no response is received before the "
                    "timeout expires, the request fails and returns an error",
            },
        };
    }

    #! Creates the object
    constructor() {
        map addField(new QoreDataField($1.key, $1.value.desc, $1.value.type, $1.value.default_value)),
            Fields.pairIterator();
    }
}

#! ElasticSearch index read API response
class ElasticSearchIndexReadResponseDataType inherits HashDataType {
    #! Creates the object
    constructor() {
        setDefaultOtherFieldType(new ElasticSearchIndexDataTypeBase());
    }
}
}
