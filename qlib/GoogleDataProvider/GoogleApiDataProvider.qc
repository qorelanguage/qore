# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleDataProvider module definition

/** GoogleApiDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the GoogleDataProvider module
public namespace GoogleDataProvider {
#! The Google data provider base class
public class GoogleApiDataProvider inherits GoogleDataProviderBase {
    private {
        string api;
        string request_label;
    }

    #! Creates the object
    constructor() {
        api = getApi();
        request_label = getRequestLabel();
    }

    #! Creates the object
    constructor(GoogleRestClient::GoogleRestClient rest) : GoogleDataProviderBase(rest) {
        api = getApi();
        request_label = getRequestLabel();
    }

    #! Returns the data provider name
    string getName() {
        return getStaticInfoImpl().name;
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("Google %s %s provider for `%s`", api, request_label, rest.getSafeURL());
    }

    #! Prepares information for an API request
    private *hash<auto> prepareRequestForSchema(auto req) {
        hash<auto> requests = getRequestInfo(api);
        *hash<auto> info = requests{request_label};
        if (!info) {
            throw "SCHEMA-ERROR", sprintf("Google API %y unknown request path %y requested; known request paths: %y", api,
                request_label, keys requests);
        }

        *hash<string, string> path_args = getPathArgs();
        AbstractDataProviderType request_type = path_args ? getRequestTypeImpl() : info.request_type;

        *hash<auto> req_info = prepareRequestIntern(request_type, req, path_args, backfillRequestBody());

        string path = info.path;
        foreach string var in (path =~ x/\{([^\}]*)}/g) {
            if (exists (*string value = path_args{var} ?? req_info.path{var})) {
                path = replace(path, "{" + var + "}", value);
            } else {
                throw "GOOGLE-REQUEST-ERROR", sprintf("%s API %s requires missing path arg %y; path args: %y",
                    api, request_label, var, path_args);
            }
        }

        if (req_info.query) {
            path += req_info.query;
        }

        return req_info + {
            "pathstr": info_map{api}.basePath + path,
        };
    }

    #! Makes a request and returns the response
    /** @param req the request to serialize and make according to the request type
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        hash<auto> req_info = prepareRequestForSchema(req);
        hash<auto> info;
        try {
            *hash<auto> rv = rest.doRequest(getMethodForSchema(api, request_label), req_info.pathstr, req_info.body,
                \info).body;
            return getResponseTypeImpl().acceptsValue(rv);
        } catch (hash<ExceptionInfo> ex) {
            # ensure that any error response body is included in the exception
            *hash<auto> ex_arg = info{"request-uri", "response-code", "response-body"};
            rethrow ex.err, ex.desc, ex.arg + ex_arg;
        }
    }

    #! Returns the description of a successful request message, if any
    /** @return the request type for this provider
    */
    private *DataProvider::AbstractDataProviderType getRequestTypeImpl() {
        *hash<string, string> path_args = getPathArgs();
        return GoogleDataProviderBase::getRequestTypeForSchemaArgs(api, request_label, keys path_args);
    }

    #! Returns the description of a response message, if this object represents a response message
    /** @return the response type for this response message
    */
    private *DataProvider::AbstractDataProviderType getResponseTypeImpl() {
        return GoogleDataProviderBase::getResponseTypeForSchema(api, request_label);
    }

    #! Returns any data provider path arg values for this data provider
    private *hash<string, string> getPathArgs() {
    }

    #! If the request body should be populated with missing values
    private bool backfillRequestBody() {
        return False;
    }

    #! Returns the Google API label (ex: \c "calendar")
    private abstract string getApi();

    #! Returns the Google API request label (ex: \c "events/insert")
    private abstract string getRequestLabel();
}
}