# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SwaggerRestResponseDataProvider module definition

/** SwaggerRestResponseDataProvider.qc Copyright 2019 Qore Technologies, s.r.o.

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

#! contains all public definitions in the SwaggerResponseDataProvider module
public namespace SwaggerDataProvider {
#! The Swagger data provider class
public class SwaggerRestResponseDataProvider inherits SwaggerDataProviderBase {
    public {
        #! The Swagger schema
        SwaggerSchema schema;

        #! The current URI path
        string uri_path;

        #! The operation object
        OperationObject op;

        #! The HTTP status code for the response
        int http_code;

        #! The response object itself
        ResponseObject response;
    }

    #! Creates the object from the arguments
    constructor(SwaggerSchema schema, string uri_path, OperationObject op, int http_code, ResponseObject response) {
        self.schema = schema;
        self.uri_path = uri_path;
        self.op = op;
        self.http_code = http_code;
        self.response = response;
    }

    #! Returns the data provider name
    string getName() {
        return schema.info.title + uri_path + "/" + op.method.upr() + "/" + "response" + "/" + http_code;
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
    *hash<string, AbstractDataField> getRecordType() {
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
}
