# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SwaggerRequestDataProvider module definition

/** SwaggerRequestDataProvider.qc Copyright 2019 Qore Technologies, s.r.o.

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
# strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

#! contains all public definitions in the SwaggerRequestDataProvider module
public namespace SwaggerDataProvider {
#! The Swagger data provider class
public class SwaggerRequestDataProvider inherits SwaggerDataProviderBase {
    public {
        #! The Swagger schema
        SwaggerSchema schema;

        #! The REST client object for API calls
        *RestClient rest;

        #! current URI path
        string uri_path;

        #! The operation object
        OperationObject op;
    }

    #! Creates the object from the arguments
    constructor(SwaggerSchema schema, *RestClient rest, string uri_path, OperationObject op) {
        self.schema = schema;
        self.rest = rest;
        self.uri_path = uri_path;
        self.op = op;
    }

    #! Returns the data provider name
    string getName() {
        return schema.info.title + uri_path + "/" + op.method.upr() + "/" + "request";
    }

    #! Returns data provider info
    hash<DataProviderInfo> getInfo() {
        hash<DataProviderInfo> rv({
            "name": getName(),
            "supports_read": True,
            "supports_request": True,
            "has_record": True,
        });
        rv += getStaticInfoImpl();
        return rv;
    }

    #! Returns the description of the record type, if any
    /** Returns the record type description for any successful response
    */
    *hash<string, AbstractDataField> getRecordType() {
        *ResponseObject response = getSuccessResponse();
        if (response) {
            AbstractDataField response_field = getFieldFromSchema("body", "REST response body argument",
                response.schema);
            AbstractDataProviderType type = response_field.getType();
            *hash<string, AbstractDataField> rv = type.getFields();
            # if we have a list of hashes, then return the list element type
            if (type.getBaseTypeCode() == NT_LIST && (rv = type.getElementType().getFields())) {
                return rv;
            }
        }
    }

    #! Returns the description of a sucessful response message, if any
    private *hash<string, AbstractDataField> getSuccessResponseTypeImpl() {
        *ResponseObject response = getSuccessResponse();
        if (response) {
            hash<string, AbstractDataField> rv;
            if (response.schema) {
                rv.body = getFieldFromSchema("body", "REST response body argument", response.schema);
            }
            if (response.headers) {
                rv.headers = getFieldForHeaders("headers", response.headers);
            }
            return rv;
        }
    }

    #! Returns the schema for the first successful response message found
    private *ResponseObject getSuccessResponse() {
        # check for any successful response data structure
        foreach hash<auto> i in (op.responses.responses.pairIterator()) {
            if (i.key =~ /^2/ && i.value) {
                return i.value;
            }
        }
    }

    #! Makes a request and returned the response
    /**
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request

        @throw INVALID-OPERATION the data provider does not support the request API
    */
    private auto doRequestImpl(*hash<auto> request_options) {
        return doRequestIntern(request_options);
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()

        This will execute the request and perform a default search on any record(s) returned

        @throw SWAGGER-SEARCH-ERROR the response type it not list or hash and does not support searching
    */
    private AbstractDataProviderRecordIterator searchRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        hash<auto> resp = doRequestIntern(search_options);
        # return an iterator for the result
        AbstractIterator i;
        switch (resp.body.typeCode()) {
            case NT_HASH:
                i = new SingleValueIterator(resp.body);
                break;
            case NT_LIST:
                i = new ListIterator(resp.body);
                break;
            default:
                throw "SWAGGER-SEARCH-ERROR", sprintf("the response has type %y, which does not support searching",
                    resp.body.type());
                break;
        }
        return new DefaultRecordIterator(i, where_cond);
    }

    #! Makes a REST request and returns the response
    private hash<auto> doRequestIntern(*hash<auto> options) {
        RestClient rest = getRestClient();
        # make URI path with any query args
        string uri_path = self.uri_path;
        if (options.query) {
            uri_path += "?" +
                (foldl $1 + "&" + $1,
                    (map sprintf("%s=%s", $1.key, $1.value), options.query.pairIterator()));
        }
        # make the call
        hash<auto> info;
        hash<auto> resp = rest.doRequest(op.method, uri_path, options.body, \info, NOTHING, options.header);
        return resp + {"info": info};
    }

    #! Returns a REST client for HTTP operations
    private RestClient getRestClient() {
        if (!rest) {
            rest = new RestClient({
                "url": schema.getTargetUrl(),
                "validator": schema,
            });
        }
        return rest;
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        hash<DataProviderInfo> rv = SwaggerDataProvider::ProviderInfo - "constructor_options";
        rv += {
            "supports_read": True,
            "supports_request": True,
            "has_record": True,
        };
        rv.request_options = rv.search_options = getApiOptions();
        return rv;
    }

    #! Returns API options for the current operation
    private *hash<string, hash<DataProviderOptionInfo>> getApiOptions() {
        *hash<string, hash<DataProviderOptionInfo>> rv;
        if (op.parameters) {
            rv = getOptionInfoFromParameters("parameters", op.parameters);
        }
        if (op.body) {
            rv.body = <DataProviderOptionInfo>{
                "type": getSchemaDataType(cast<BodyParameter>(op.body).schema),
                "required": op.body.required,
                "desc": "REST request body argument",
            };
        }
        if (op.formData) {
            OtherParameter formData = cast<OtherParameter>(op.formData);
            rv.formData = <DataProviderOptionInfo>{
                "type": getType(formData.type, formData.items),
                "required": formData.required,
                "desc": "REST request query arguments",
            };
        }
        return rv;
    }
}
}
