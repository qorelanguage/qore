# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore CohereDataProviderCommon class definition

/** CohereDataProviderCommon.qc Copyright 2026 Qore Technologies, s.r.o.

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

public namespace CohereDataProvider {
public class CohereDataProviderCommon inherits DataProvider::AbstractDataProvider {
    public {
        const DefaultCohereUrl = "https://api.cohere.com";
        const DefaultCohereApiVersion = "v2";

        const ConstructorOptions = {
            "apikey": <DataProviderOptionInfo>{
                "display_name": "Cohere API Key",
                "short_desc": "Cohere API key for Bearer authentication",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The Cohere API key.",
                "sensitive": True,
            },
            "url": <DataProviderOptionInfo>{
                "display_name": "Server URL",
                "short_desc": "The URL of the Cohere REST API",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The URL of the Cohere REST API.",
                "default_value": DefaultCohereUrl,
            },
            "restclient": <DataProviderOptionInfo>{
                "display_name": "REST Client",
                "short_desc": "The REST client object to use",
                "type": AbstractDataProviderType::get(
                    new Type("RestClientIo::RestClientIo"), NOTHING, {
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
                "desc": "Options to the RestClientIo constructor.",
            },
            "api": <DataProviderOptionInfo>{
                "display_name": "API Version",
                "short_desc": "Cohere API version segment",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "Retained for compatibility; each action selects its documented API version.",
                "allowed_values": (
                    <AllowedValueInfo>{"value": "v2", "display_name": "V2"},
                    <AllowedValueInfo>{"value": "v1", "display_name": "V1 (Legacy)"},
                ),
                "default_value": DefaultCohereApiVersion,
            },
        };

        #! Legacy retry errors retained for source compatibility
        const RetrySet = {
            "SOCKET-SSL-ERROR": True,
            "SOCKET-CLOSED": True,
        };

        #! Legacy retry limit retained for source compatibility
        const MaxIoRetries = 5;
    }

    private {
        *RestClientIo::RestClientIo rest;
    }

    #! Uses a copy of a REST client with each action's explicit API path
    /** @param rest the authenticated transport to copy */
    constructor(RestClientIo::RestClientIo rest) {
        self.rest = cast<RestClientIo::RestClientIo>(rest.copyWithUrl(rest.getUrl().toString()));
        self.rest.clearConnectionPath();
    }

    #! Creates a provider from an API key or supplied REST client
    /** @param options the ConstructorOptions values
        @throw CONSTRUCTOR-ERROR required authentication is missing or an option is invalid
    */
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        if (copts.restclient) {
            rest = cast<RestClientIo::RestClientIo>(copts.restclient.copyWithUrl(copts.restclient.getUrl().toString()));
        }
        if (!rest) {
            if (!copts.apikey.val()) {
                throw "CONSTRUCTOR-ERROR",
                    "missing required 'apikey' argument for Cohere API communication";
            }
            hash<auto> opts = (copts.restclient_options ?? {}) + {
                "url": copts.url,
                "api": copts.api,
                "apikey": copts.apikey,
            };
            rest = new CohereRestClient::CohereRestClientIo(opts);
        }
        rest.clearConnectionPath();
    }

    #! Returns a provider bound to a schema-derived action
    /** @param action the stable action ID
        @return the corresponding provider using this connection's transport
    */
    private RestSchemaActionDataProvider getSchemaProvider(string action) {
        RestSchemaActionSet actions = CohereSchema::getActionSet();
        return new RestSchemaActionDataProvider(actions, actions.getOperationEx(action), rest);
    }

    #! Returns the reference data supported by all Cohere action providers
    /** @return names of available dropdowns */
    *hash<string, bool> getSupportedReferenceData() {
        if (CohereSchema::getActionSet().getOperation(getName())) {
            return getSchemaProvider(getName()).getSupportedReferenceData();
        }
        return;
    }

    private *list<hash<AllowedValueInfo>> getReferenceDataImpl(string type, *hash<auto> opts) {
        return getSchemaProvider(getName()).getReferenceData(type, opts);
    }
}
}
