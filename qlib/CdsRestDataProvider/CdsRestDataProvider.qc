# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore CdsRestDataProvider module definition

/** CdsRestDataProvider.qc Copyright 2020 Qore Technologies, s.r.o.

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
%requires qore >= 0.9.5
# assume local scope for variables, do not use "$" signs
%new-style
# require type definitions everywhere
%require-types
# strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

#! contains all public definitions in the CdsRestDataProvider module
public namespace CdsRestDataProvider {
#! The CdsRest data provider class
public class CdsRestDataProvider inherits DataProvider::AbstractDataProvider {
    public {
        #! The REST client object for API calls
        CdsRestClient::CdsRestClient rest;

        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "CdsRestDataProvider",
            "supports_children": True,
            "constructor_options": ConstructorOptions,
        };

        #! Constructor options
        const ConstructorOptions = {
            "api": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the CDS API version to use",
            },
            "cdsrestclient": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(new Type("CdsRestClient")),
                "desc": "the CdsRestClient object",
            },
            "cdsrestclient_options": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(AutoHashType),
                "desc": "options to the CdsRestClient constructor; only used if a CdsRestClient object "
                    "is created for a call",
            },
            "client_id": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the client ID (required if 'client_secret' provided)",
            },
            "client_secret": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the client secret (required if 'client_id' provided)",
            },
            "scope": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the scope string",
            },
            "tenant": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the tenant string",
            },
            "url": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "the URL to the CDS server; overrides any URL in the schema or in any RestClient object "
                    "passed as an option",
            },
        };

        #! Environment options
        const EnvOptions = {
            "url": "CDS_URL",
            "client_id": "CDS_CLIENT_ID",
            "client_secret": "CDS_CLIENT_SECRET",
            "tenant": "CDS_TENANT",
            "scope": "CDS_SCOPE",
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

    private {
        #! Metadata keyed by entity type
        *hash<auto> meta;

        #! Metadata mutex
        Qore::Thread::Mutex lck();
    }

    #! Creates the object from the arguments
    constructor(CdsRestClient::CdsRestClient rest) {
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
        if (copts.cdsrestclient) {
            copts.cdsrestclient.checkClientOptions(copts);
            rest = copts.cdsrestclient;
        }

        if (!rest) {
            rest = new CdsRestClient(getClientOptions(copts));
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
        return keys getMetadata();
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @see getChildProviderEx()
    */
    private *DataProvider::AbstractDataProvider getChildProviderImpl(string name) {
        *hash<auto> obj_meta = getMetadata(){name};
        if (obj_meta) {
            return new CdsEntityDataProvider(rest, name, obj_meta);
        }
    }

    #! Returns metadata
    private *hash<auto> getMetadata() {
        if (meta) {
            return meta;
        }

        lck.lock();
        on_exit lck.unlock();

        # check again in the lock
        if (meta) {
            return meta;
        }

        hash<auto> resp = rest.get("$metadata?annotations=true").body."edmx:Edmx"."edmx:DataServices".Schema;
        #printf("%N\n", resp.EntityType);

        meta = map {$1."^attributes^".Name: $1}, resp.EntityType, ($1.Property || ($1.Abstract == "true"));
        return meta;
    }

    #! Returns data provider static info
    private hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }

    #! Returns options for the CdsRestClient
    static private *hash<auto> getClientOptions(*hash<auto> copts) {
        return copts - ("cdsrestclient_options", "cdsrestclient")
            + copts.cdsrestclient_options;
    }
}
}
