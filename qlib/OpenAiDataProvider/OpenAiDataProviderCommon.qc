# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore OpenAiDataProvider module definition

/** OpenAiDataProviderCommon.qc Copyright 2024 Qore Technologies, s.r.o.

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

#! contains all public definitions in the OpenAiDataProvider module
public namespace OpenAiDataProvider {
#! The OpenAi data provider common base class
public class OpenAiDataProviderCommon inherits DataProvider::AbstractDataProvider {
    public {
        #! The default OpenAi URL
        const DefaultOpenAiUrl = "https://api.openai.com/";

        #! The default OpenAi API version
        const DefaultOpenAiApiVersion = "v1";

        #! Constructor options
        const ConstructorOptions = {
            "api_key": <DataProviderOptionInfo>{
                "display_name": "OpenAi API Key",
                "short_desc": "The API key required to communicate with OpenAi servers",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The API key required to communicate with OpenAi servers",
            },
            "restclient": <DataProviderOptionInfo>{
                "display_name": "REST Client",
                "short_desc": "The REST client object to use",
                "type": AbstractDataProviderType::get(new Type("RestClient"), NOTHING, {
                    DTT_ClientOnly: True,
                }),
                "desc": "The REST client object to use",
            },
            "restclient_options": <DataProviderOptionInfo>{
                "display_name": "REST Client Options",
                "short_desc": "The options to use when creating a REST client",
                "type": AbstractDataProviderType::get(AutoHashType, NOTHING, {
                    DTT_ClientOnly: True,
                }),
                "desc": "Options to the RestClient constructor; only used if a RestClient object is created for a "
                    "call",
            },
            "api": <DataProviderOptionInfo>{
                "display_name": "API Version",
                "short_desc": "The API version to use",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The API version to use",
                "default_value": DefaultOpenAiApiVersion,
            },
        };

        #! IO errors for REST retries
        const RetrySet = {
            "SOCKET-SSL-ERROR": True,
            "SOCKET-CLOSED": True,
        };

        #! Max retries
        const MaxIoRetries = 5;

        #! Headers required for the OpenAI assistant API
        const AssistantApiHdr = {
            "OpenAI-Beta": "assistants=v2",
        };
    }

    private {
        #! The REST client object for API calls
        *RestClient::RestClient rest;

        #! first time initialization
        static bool init;

        #! lock for first time initialization
        static Mutex m();
    }

    #! Creates the object
    constructor(RestClient::RestClient rest) {
        self.rest = rest;
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        if (copts.restclient) {
            rest = copts.restclient;
        }

        if (!rest) {
            if (!copts.api_key) {
                throw "CONSTRUCTOR-ERROR", "missing 'api_key' argument for OpenAI API communication";
            }
            hash<auto> opts;
            opts += copts.restclient_options + {
                "api": copts.api_version,
                "token": copts.api_key,
            };
            rest = new OpenAiRestClient(opts);
        }
    }

    #! Accepts a LoggerInterface object for logging (or clears it)
    setLogger(*LoggerInterface logger) {
        if (rest) {
            rest.setLogger(logger);
        }
        LoggerWrapper::setLogger(logger);
    }

    #! Returns reference data of the given kind if available
    /** @param type the unique type name of the reference data

        @return a list of allowed values for this data

        @since DataProvider 3.0
    */
    private *list<hash<AllowedValueInfo>> getReferenceDataImpl(string type) {
        switch (type) {
            case "models": return getReferenceModels();
        }
        throw "UNKNOWN-REF-DATA", sprintf("Reference data type %y is unknown", type);
    }

    #! Returns available models
    private *list<hash<AllowedValueInfo>> getReferenceModels() {
        list<hash<AllowedValueInfo>> rv;
        foreach hash<auto> model in (doRestCommand("GET", "models").body."data") {
            string desc = sprintf("Model created on %y, owned by %y", localtime(model.created), model.owned_by);
            rv += <AllowedValueInfo>{
                "display_name": model.id,
                "short_desc": desc,
                "desc": desc,
                "value": model.id,
            };
        }
        return rv;
    }

    #! Makes a REST call and returns the response; handles rate limit responses
    private hash<auto> doRestCommand(string method, string path, auto body, *hash<auto> hdr,
            *reference<hash<auto>> info) {
        int retries = 0;
        while (True) {
            try {
                debug("REST %y %y (%y) body: %y", method, rest.getURL(), path, body);
                return rest.doRequest(method, path, body, \info, NOTHING, hdr);
            } catch (hash<ExceptionInfo> ex) {
                if (retry(ex, \retries)) {
                    continue;
                }
                rethrow ex.err, sprintf("%s (HTTP response code: %y)", ex.desc, info."response-headers".status_code);
            }
        }
    }

    #! Returns True if the error indicates that the operation should be retried
    private bool retry(hash<ExceptionInfo> ex, reference<int> retries) {
        if (RetrySet{ex.err}) {
            if (++retries == MaxIoRetries) {
                error("Maximum retries exceeded (%d)", retries);
                return False;
            }
            warn("Technical error making REST call: %s: %s; retrying", ex.err, ex.desc);
            return True;
        }
        return False;
    }
}
}