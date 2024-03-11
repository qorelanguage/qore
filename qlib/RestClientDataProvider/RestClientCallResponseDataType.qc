# -*- mode: qore; indent-tabs-mode: nil -*-

/** RestCallResponseDataType.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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
#! Constant type for REST call responses
public const RestClientCallResponseDataType = new RestClientCallResponseDataType();

#! Data type for REST call responses
public class RestClientCallResponseDataType inherits HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            "request-headers": {
                "type": HashType,
                "desc": "A hash of request headers with keys in the original case as sent (ex: `Content-Type`)",
            },
            "request-serialization": {
                "type": StringType,
                "desc": "The type of message serialization used for the request",
            },
            "request-uri": {
                "type": StringType,
                "desc": "The request URI string sent (ex: `GET /services/async/38.0/job HTTP/1.1`)",
            },
            "response-body": {
                "type": AutoType,
                "desc": "Any message body sent in the response",
            },
            "response-headers": {
                "type": HashType,
                "desc": "A hash of processed response headers with keys all converted to lower case letters "
                    "(ex: `content-type`)",
            },
            "response-headers-raw": {
                "type": HashType,
                "desc": "The raw, unprocessed response headers with keys in the original case as received "
                    "(ex: `Content-Type`)",
            },
            "response-serialization": {
                "type": StringType,
                "desc": "The type of message serialization used in the response",
            },
            "response-uri": {
                "type": StringType,
                "desc": "The HTTP response URI (ex: `HTTP/1.l 200 OK`)",
            },
            "status-code": {
                "type": IntType,
                "desc": "The HTTP status code provided in the response",
            },
            "status-message": {
                "type": StringType,
                "desc": "The HTTP status message provided in the response",
            },
        };
    }

    #! Creates the object
    constructor() {
        addQoreFields(Fields);
    }
}
}
