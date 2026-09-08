# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore FireworksDataProviderCommon class definition

/** FireworksDataProviderCommon.qc Copyright 2026 Qore Technologies, s.r.o.

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

public namespace FireworksDataProvider {
#! The Fireworks AI data provider common base class
public class FireworksDataProviderCommon inherits DataProvider::AbstractDataProvider {
    public {
        const DefaultFireworksUrl = "https://api.fireworks.ai/inference";
        const DefaultFireworksApiVersion = "v1";

        const ConstructorOptions = {
            "apikey": <DataProviderOptionInfo>{
                "display_name": "Fireworks AI API Key",
                "short_desc": "Fireworks AI API key for Bearer authentication",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The Fireworks AI API key.  Get one from the "
                    "[Fireworks Dashboard](https://fireworks.ai/account/api-keys).",
                "sensitive": True,
            },
            "url": <DataProviderOptionInfo>{
                "display_name": "Server URL",
                "short_desc": "The URL of the Fireworks AI REST API",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The URL of the Fireworks AI REST API.",
                "default_value": DefaultFireworksUrl,
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
                "short_desc": "Fireworks API version segment",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The API version segment used for `/v1/*` routing.",
                "default_value": DefaultFireworksApiVersion,
            },
        };

    }

    private {
        *RestClientIo::RestClientIo rest;
    }

    constructor(RestClientIo::RestClientIo rest) {
        self.rest = cast<RestClientIo::RestClientIo>(rest.copyWithUrl(rest.getUrl().toString()));
        self.rest.clearConnectionPath();
    }

    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        if (copts.restclient) {
            rest = cast<RestClientIo::RestClientIo>(copts.restclient.copyWithUrl(copts.restclient.getUrl().toString()));
        }
        if (!rest) {
            if (!copts.apikey.val()) {
                throw "CONSTRUCTOR-ERROR",
                    "missing required 'apikey' argument for Fireworks AI API communication";
            }
            hash<auto> opts = (copts.restclient_options ?? {}) + {
                "url": copts.url,
                "api": copts.api,
                "apikey": copts.apikey,
            };
            rest = new FireworksRestClient::FireworksRestClientIo(opts);
        }
        rest.clearConnectionPath();
    }

    #! Returns a provider for one of the schema actions
    /** @param action stable action ID
        @return a provider using this connection's transport
    */
    private RestSchemaActionDataProvider getSchemaProvider(string action) {
        RestSchemaActionSet aset = FireworksSchema::getActionSet(action);
        return new RestSchemaActionDataProvider(aset, aset.getOperationEx(action), rest);
    }

    #! Returns the supported dropdown names
    /** @return dropdowns provided by the account/model catalog */
    *hash<string, bool> getSupportedReferenceData() {
        return getSchemaProvider("list-models").getSupportedReferenceData();
    }

    private *list<hash<AllowedValueInfo>> getReferenceDataImpl(string type, *hash<auto> options) {
        return getSchemaProvider("list-models").getReferenceData(type, options);
    }

    #! Makes a non-retried inference request; retrying a paid POST after a lost response can double bill
    private hash<auto> doRestCommand(string method, string path, auto body, *hash<auto> hdr) {
        return rest.restDoRequest(method, "/inference/v1/" + ltrim(path, "/"), body, hdr);
    }
}
}
