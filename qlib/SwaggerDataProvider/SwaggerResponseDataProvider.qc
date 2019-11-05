# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SwaggerResponseDataProvider module definition

/** SwaggerResponseDataProvider.qc Copyright 2019 Qore Technologies, s.r.o.

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
#! strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

#! contains all public definitions in the SwaggerResponseDataProvider module
public namespace SwaggerResponseDataProvider {
#! The Swagger data provider class
public class SwaggerResponseDataProvider inherits SwaggerDataProviderBase {
    public {
        #! The Swagger schema
        SwaggerSchema schema;

        #! current URI path
        string uri_path;

        #! The operation object
        OperationObject op;
    }

    #! Creates the object from the arguments
    constructor(SwaggerSchema schema, string uri_path, OperationObject op) {
        self.schema = schema;
        self.uri_path = uri_path;
        self.op = op;
    }

    #! Returns the data provider name
    string getName() {
        return schema.info.title + uri_path + "/" + op.method.upr() + "/" + "response";
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        return keys op.responses.responses;
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @see getChildProviderEx()
    */
    private *AbstractDataProvider getChildProviderImpl(string name) {
        if (*ResponseObject response = op.responses.responses{name}) {
            return new SwaggerRestResponseDataProvider(schema, uri_path, op, name.toInt(), response);
        }
    }
}
}
