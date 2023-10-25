# -*- mode: qore; indent-tabs-mode: nil -*-

/** HttpCallWithMethodRequestDataType.qc Copyright 2023 Qore Technologies, s.r.o.

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
#! Data type for HTTP client request call with an explicit method
public class HttpClientCallWithMethodRequestDataType inherits HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            "method": {
                "display_name": "HTTP Method",
                "type": StringType,
                "desc": "The HTTP method to use",
                "required": True,
            },
            "body": {
                "display_name": "Message Body",
                "type": DataOrNothingType,
                "desc": "The message body",
            },
            "path": {
                "display_name": "Request Path",
                "type": StringType,
                "desc": "The URI path",
            },
            "hdr": {
                "display_name": "HTTP Headers",
                "type": AutoHashOrNothingType,
                "desc": "Any headers to include in the request",
            },
        };
    }

    #! Creates the object
    constructor() {
        addQoreFields(Fields);
    }
}
}
