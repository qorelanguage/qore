# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore MewsRestDataProvider class definition

/** MewsRestDataProviderBase.qc Copyright 2023 Qore Technologies, s.r.o.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated doContent-Typecumentation files (the "Software"),
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

#! contains all public definitions in the MewsRestDataProvider module
public namespace MewsRestDataProvider {
#! The MewsRest data provider base class
public class MewsRestDataProviderBase inherits DataProvider::AbstractDataProvider {
    public {
        #! Max records that the Mews API will return in one request
        const MaxRecords = 1000;
    }

    private {
        MewsRestClient::MewsRestClient rest;
        SwaggerDataProvider::SwaggerDataProvider sdp;
    }

    #! Creates the object
    constructor() {
    }

    #! Creates the object from a REST connection
    constructor(MewsRestClient::MewsRestClient rest, *SwaggerDataProvider::SwaggerDataProvider sdp) {
        self.rest = rest;
        if (sdp) {
            self.sdp = sdp;
        } else {
            AbstractRestSchemaValidator v = rest.getValidator();
            if (!(v instanceof SwaggerSchema)) {
                throw "MEWS-CLIENT-ERROR", sprintf("The MewsRestClient object has no Swagger schema; validator "
                    "used: %y", v.className());
            }
            self.sdp = new SwaggerDataProvider(cast<SwaggerSchema>(v), rest);
        }
    }

    #! Accepts a LoggerInterface object for logging (or clears it)
    setLogger(*LoggerInterface logger) {
        rest.setLogger(logger);
        LoggerWrapper::setLogger(logger);
    }

    static MewsRestClient::MewsRestClient getRestConnection(*hash<auto> options) {
        MewsRestClient rest;
        if (options.restclient) {
            rest = options.restclient;
            if (options.url) {
                rest.setURL(options.url);
            }
        } else {
            if (!options.url && !options.restclient_options) {
                throw "CONSTRUCTOR-ERROR", "no 'restclient', 'url', or 'restclient_options' option passed; "
                    "cannot create REST client to MewsRest instance without a URL";
            }

            hash<auto> opts;
            opts += options.restclient_options;
            if (options.url) {
                opts.url = options.url;
            }
            rest = new MewsRestClient(opts);
        }
        return rest;
    }
}
}

