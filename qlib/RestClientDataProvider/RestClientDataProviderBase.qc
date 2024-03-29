# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore RestClientDataProvider class definition

/** RestClientDataProviderBase.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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

#! contains all public definitions in the RestClientDataProvider module
public namespace RestClientDataProvider {
#! The REST client data provider class, provides API classes as children
public class RestClientDataProviderBase inherits DataProvider::AbstractDataProvider {
    private {
        #! The REST client object
        RestClient::RestClient rest;
    }

    #! Creates the object
    constructor() {
    }

    #! Creates the object
    constructor(RestClient::RestClient rest) {
        self.rest = rest;
    }

    #! Accepts a LoggerInterface object for logging (or clears it)
    setLogger(*LoggerInterface logger) {
        rest.setLogger(logger);
        LoggerWrapper::setLogger(logger);
    }

    #! Returns the request path for the given request
    static string getRequestPath(hash<auto> req) {
        if (req.query_args) {
            if (req.path =~ /\?/) {
                req.path += "&";
            } else {
                req.path += "?";
            }
            req.path += foldl $1 + "&" + $2,
                (map sprintf("%s=%s", $1.key, RestClientDataProviderBase::getUriValue($1.value)),
                    req.query_args.pairIterator());
        }
        return req.path;
    }

    #! Returns the value for the given query argument
    static string getUriValue(auto v) {
        switch (v.typeCode()) {
            case NT_BOOLEAN:
                return v ? "true" : "false";
            case NT_LIST:
                return foldl $1 + "," + $2, (map RestClientDataProviderBase::getUriValue($1), v);
            case NT_STRING:
                return v;
            default:
                return v.toString();
        }
    }
}
}