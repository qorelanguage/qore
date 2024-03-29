# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore CdsRestDataProvider module definition

/** CdsRestDataProvider.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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

#! contains all public definitions in the CdsRestDataProvider module
public namespace CdsRestDataProvider {
#! Application name for Dynamics
public const DynamicsAppName = "Dynamics";

#! The CdsRest data provider class
public class CdsRestDataProvider inherits CdsRestDataProviderBase {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "CdsRestDataProvider",
            "supports_children": True,
            "constructor_options": ConstructorOptions,
            "children_can_support_apis": False,
            "children_can_support_records": True,
            "children_can_support_observers": False,
        };

        #! Constructor options
        const ConstructorOptions = {
            "api": <DataProviderOptionInfo>{
                "display_name": "API Version",
                "short_desc": "Sets the API version to use",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "Sets the CDS API version to use",
                "default_value": CdsRestClient::DefaultApiVersion,
            },
            "cdsrestclient_options": <DataProviderOptionInfo>{
                "display_name": "REST Client Options",
                "short_desc": "Sets options to the REST client constructor",
                "type": AbstractDataProviderType::get(AutoHashType),
                "desc": "Sets options to the REST client constructor",
            },
            "client_id": <DataProviderOptionInfo>{
                "display_name": "OAuth2 Client ID",
                "short_desc": "Sets the OAuth2 client ID",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "Sets the OAuth2 client ID (required if 'client_secret' provided)",
            },
            "client_secret": <DataProviderOptionInfo>{
                "display_name": "OAuth2 Client Secret",
                "short_desc": "Sets the OAuth2 client secret",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "Sets the OAuth2 client secret (required if 'client_id' provided)",
            },
            "scope": <DataProviderOptionInfo>{
                "display_name": "OAuth2 Scope",
                "short_desc": "The OAuth2 scope string",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "The OAuth2 scope string",
            },
            "tenant": <DataProviderOptionInfo>{
                "display_name": "Tenant",
                "short_desc": "The tenant ID of the CDS server instance",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "The tenant ID of the CDS server instance",
            },
            "url": <DataProviderOptionInfo>{
                "display_name": "URL",
                "short_desc": "The URL to the CDS server instance",
                "type": AbstractDataProviderType::get(StringType),
                "desc": "The URL to the CDS server instance",
                "required": True,
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
    constructor(CdsRestClient::CdsRestClient rest) : CdsRestDataProviderBase(rest) {
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        bool auto_config = DataProvider::getAutoConfig();
        if (auto_config) {
            map options{$1.key} = ENV{$1.value}, EnvOptions.pairIterator(),
                exists ENV{$1.value} && !exists options{$1.key};
        }
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        rest = new CdsRestClient(getClientOptions(copts));
    }

    #! Returns the data provider name
    string getName() {
        return "/";
    }

    #! Accepts a LoggerInterface object for logging (or clears it)
    setLogger(*LoggerInterface logger) {
        rest.setLogger(logger);
        LoggerWrapper::setLogger(logger);
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

        hash<auto> resp;
        hash<auto> info;
        try {
            resp = rest.get("$metadata?annotations=true", NOTHING, \info).body."edmx:Edmx"."edmx:DataServices".Schema;
        } catch (hash<ExceptionInfo> ex) {
            # ensure that any error response body is included in the exception
            hash<auto> ex_arg = info{"request-uri", "response-code", "response-body"};
            rethrow ex.err, ex.desc, ex.arg + ex_arg;
        }
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
