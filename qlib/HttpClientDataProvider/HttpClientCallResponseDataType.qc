# -*- mode: qore; indent-tabs-mode: nil -*-

/** HttpCallResponseDataType.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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

#! contains all public definitions in the HttpClientDataProvider module
public namespace HttpClientDataProvider {
#! Data type for HTTP call responses
public class HttpClientCallResponseDataType inherits HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            "request-headers": {
                "display_name": "Request Headers",
                "type": HashType,
                "desc": "A hash of request headers with keys in the original case as sent (ex: `Content-Type`)",
            },
            "request-uri": {
                "display_name": "Request URI",
                "type": StringType,
                "desc": "The request URI string sent (ex: `GET /services/async/38.0/job HTTP/1.1`)",
            },
            "response-body": {
                "display_name": "Response Body",
                "type": DataOrNothingType,
                "desc": "Any message body sent in the response",
            },
            "response-headers": {
                "display_name": "Response Headers",
                "type": HashType,
                "desc": "A hash of processed response headers with keys all converted to lower case letters "
                    "(ex: `content-type`)",
            },
            "response-headers-raw": {
                "display_name": "Raw Response Headers",
                "type": HashType,
                "desc": "The raw, unprocessed response headers with keys in the original case as received "
                    "(ex: `Content-Type`)",
            },
            "response-uri": {
                "display_name": "Response URI",
                "type": StringType,
                "desc": "The HTTP response URI (ex: `HTTP/1.l 200 OK`)",
            },
            "status-code": {
                "display_name": "HTTP Status Code",
                "type": IntType,
                "desc": "The HTTP status code provided in the response",
            },
            "status-message": {
                "display_name": "HTTP Status Message",
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
