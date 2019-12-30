# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SalesforceRestDataProvider module definition

/** SalesforceRestDataProvider.qc Copyright 2019 Qore Technologies, s.r.o.

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
# strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

#! contains all public definitions in the SalesforceRestDataProvider module
public namespace SalesforceRestDataProvider {
#! The SalesforceRest data provider class
public class SalesforceRestDataProvider inherits AbstractDataProvider {
    public {
        #! The REST client object for API calls
        *SalesforceRestClient rest;

        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "SalesforceRestDataProvider",
            "supports_read": False,
            "supports_create": False,
            "supports_update": False,
            "supports_upsert": False,
            "supports_delete": False,
            "supports_native_search": False,
            "supports_bulk_create": False,
            "supports_bulk_upsert": False,
            "supports_children": True,
            "constructor_options": ConstructorOptions,
            "search_options": NOTHING,
            "create_options": NOTHING,
            "upsert_options": NOTHING,
            "transaction_management": False,
        };

        #! Constructor options
        const ConstructorOptions = {
            "client_id": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the Salesforce.com consumer key",
                "required": False,
            },
            "client_secret": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the Salesforce.com consumer secret",
                "required": False,
            },
            "username": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the Salesforce.com account username",
                "required": False,
            },
            "password": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the Salesforce.com account password",
                "required": False,
            },
            "salesforcerestclient": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(new Type("SalesforceRestClient")),
                "desc": "the SalesforceRestClient object",
            },
            "api": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the Salesforce.com API version to use (\"auto\" = latest version available)",
            },
            "oauth_url_auth": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "URL for logging in, overrides the default value",
            },
            "oauth_url_token": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "URL for token acquisition, overrides the default value, used when logging in",
            },
            "oauth_url_revoke": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "URL for token revocation, overrides the default value, used when logging out",
            },
            "salesforcerestclient_options": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(AutoHashType),
                "desc": "options to the SalesforceRestClient constructor; only used if a SalesforceRestClient object "
                    "is created for a call",
            },
        };

        #! Environment options
        const EnvOptions = {
            "client_id": "SALESFORCE_CONSUMER_KEY",
            "client_secret": "SALESFORCE_CONSUMER_SECRET",
            "username": "SALESFORCE_USER",
            "password": "SALESFORCE_PASS",
        };

        #! Hash of HTTP Methods
        const HttpMethods = {
            "GET": True,
            "PUT": True,
            "PATCH": True,
            "POST": True,
            "DELETE": True,
        };
    }

    #! Creates the object from the arguments
    constructor(*SalesforceRestClient rest) {
        self.rest = rest;
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        bool auto_config = DataProvider::getAutoConfig();
        if (auto_config) {
            map options{$1.key} = ENV{$1.value}, EnvOptions.pairIterator(),
                exists ENV{$1.value} && !exists options{$1.key};
        }
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        if (copts.salesforcerestclient) {
            copts.salesforcerestclient.checkClientOptions(copts);
            rest = copts.salesforcerestclient;
        }

        if (!rest) {
            rest = new SalesforceRestClient(getClientOptions(copts));
        }
    }

    #! Returns the data provider name
    string getName() {
        return "/";
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        hash<auto> v = rest.get("sobjects");
        return map $1.name, v.body.sobjects, $1.queryable;
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @see getChildProviderEx()
    */
    private *AbstractDataProvider getChildProviderImpl(string name) {
        hash<auto> info;
        try {
            hash<auto> v = rest.get("sobjects/" + name, NOTHING, \info);
            if (!v.body.objectDescribe) {
                return;
            }
            return new SalesforceRestObjectDataProvider(rest, name, v.body.objectDescribe);
        } catch (hash<ExceptionInfo> ex) {
            if (ex.err == "HTTP-CLIENT-RECEIVE-ERROR" && info."response-headers".status_code == 404) {
                return;
            }
            rethrow;
        }
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }

    #! Returns options for the SalesforceRestClient
    static private hash<auto> getClientOptions(hash<auto> copts) {
        return copts - ("salesforcerestclient_options", "salesforcerestclient")
            + copts.salesforcerestclient_options;
    }
}
}
