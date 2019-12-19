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

        #! The current URI path
        string uri_path;

        #! The operation object
        OperationObject op;

        #! The success response code, if any
        string success_response;

        #! If the request supports a list of hashes in the response and therefore the record API
        *hash<string, AbstractDataField> record_type;
    }

    #! Creates the object from the arguments
    constructor(SwaggerSchema schema, *RestClient rest, string uri_path, OperationObject op) {
        self.schema = schema;
        self.rest = rest;
        self.uri_path = uri_path;
        self.op = op;

        # check for any successful response data structure
        foreach hash<auto> i in (op.responses.responses.pairIterator()) {
            if (i.key =~ /^2/ && i.value) {
                success_response = i.key;
                record_type = getRecordTypeIntern();
            }
        }
    }

    #! Returns the data provider name
    string getName() {
        return schema.info.title + uri_path + "/" + op.method.upr() + "/" + "request";
    }

    #! Returns data provider info
    hash<DataProviderInfo> getInfo() {
        hash<DataProviderInfo> rv({
            "name": getName(),
        });
        rv += getStaticInfoImpl();
        return rv;
    }

    #! Returns the description of the record type, if any
    /** Returns the record type description for any successful response
    */
    *hash<string, AbstractDataField> getRecordType() {
        return record_type;
    }

    private *hash<string, AbstractDataField> getRecordTypeIntern() {
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

    #! Returns the description of a successful request message, if any
    private *AbstractDataProviderType getRequestTypeImpl() {
        HashDataType rv(getName() + ": request");
        if (op.parameters) {
            addFieldsFromParameters(rv, op.parameters);
        }
        if (op.body) {
            rv.addField(getFieldFromSchema("body", "REST request body argument", cast<BodyParameter>(op.body).schema, op.body.required));
        }
        if (op.formData) {
            HashDataType formData(getName() + ": request formData");
            foreach hash<auto> i in (op.formData.pairIterator()) {
                OtherParameter formData = cast<OtherParameter>(i.value);
                rv.addField(new QoreDataField(i.key, "formData." + i.key, getType(i.value.type, i.value.items, i.value.required)));
            }
            rv.addField(new QoreDataField("formData", "REST request form data", formData));
        }
        return rv;
    }

    #! Returns the description of a successful response message, if any
    private *AbstractDataProviderType getResponseTypeImpl() {
        *ResponseObject response = getSuccessResponse();
        if (response) {
            return getResponseTypeIntern(response, "success");
        }
    }

    #! Returns a hash of error responses, if any
    private *hash<string, AbstractDataProviderType> getErrorResponseTypesImpl() {
        *hash<string, AbstractDataProviderType> rv = map {$1.key: getResponseTypeIntern($1.value,
            ($1.key =~ /^2/ ? "success" : "error") + sprintf(" (%s)", $1.key))},
            op.responses.responses.pairIterator(), $1.key != success_response;
        if (op.responses.defaultResp) {
            rv."default" = getResponseTypeIntern(op.responses.defaultResp, "default");
        }
        return rv;
    }

    #! Returns a data provider type object for a Swagger response
    private AbstractDataProviderType getResponseTypeIntern(ResponseObject response, string label) {
        label += " response";
        HashDataType rv();
        if (response.schema) {
            rv.addField(getFieldFromSchema("body", "REST " + label + " body argument", response.schema));
        }
        if (response.headers) {
            rv.addField(getFieldForHeaders("headers", response.headers));
        }
        return rv;
    }

    #! Returns the schema for the first successful response message found
    private *ResponseObject getSuccessResponse() {
        return success_response
            ? op.responses.responses{success_response}
            : op.responses.defaultResp;
    }

    #! Makes a request and returned the response
    /** @param req the request to serialize and make according to the request type
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        return doRequestIntern(req, request_options);
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param req the request to serialize and make according to the request type
        @param where_cond the search criteria; will be processed by processFieldValues()
        @param search_options the search options after processing by validateSearchOptions()

        This will execute the request and perform a default search on any record(s) returned

        @throw SWAGGER-SEARCH-ERROR the response type is not list or hash and does not support searching
    */
    private AbstractDataProviderRecordIterator requestSearchRecordsImpl(auto req, *hash<auto> where_cond, *hash<auto> search_options) {
        hash<auto> resp = doRequestIntern(req, search_options);
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
    private hash<auto> doRequestIntern(auto req, *hash<auto> options) {
        RestClient rest = getRestClient();
        # make URI path with any query args
        string uri_path = self.uri_path;
        if (req.query) {
            uri_path += "?" +
                (foldl $1 + "&" + $1,
                    (map sprintf("%s=%s", $1.key, $1.value), req.query.pairIterator()));
            remove req.query;
        }
        # make the call
        hash<auto> info;
        hash<auto> resp = rest.doRequest(op.method, uri_path, req.body, \info, NOTHING, req.header + options.hdr);
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
            "supports_read": exists record_type,
            "supports_request": True,
            "has_record": exists record_type,
            "request_options": {
                "hdr": hash<DataProviderOptionInfo>{
                    "type": AbstractDataProviderType::get(AutoHashType),
                    "required": False,
                    "desc": "headers to send in the request",
                },
            },
        };
        return rv;
    }
}
}
