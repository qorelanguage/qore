# -*- mode: qore; indent-tabs-mode: nil -*-

/** RestCallWithoutBodyRequestDataType.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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
#! Constant type for REST call requests without a body
public const RestClientCallWithoutBodyRequestDataType = new RestClientCallWithoutBodyRequestDataType();

#! Data type for REST client call requests with no message bodies
public class RestClientCallWithoutBodyRequestDataType inherits RestClientCallRequestDataType {
    private {
        #! Field descriptions
        const Fields = RestClientCallRequestDataType::Fields - "body";
    }

    #! Creates the object
    constructor() {
    }

    #! Returns fields to add
    private hash<auto> getConstructorFields() {
        return Fields;
    }
}
}
