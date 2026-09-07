# -*- mode: qore; indent-tabs-mode: nil -*-
/** PineconeAdminDataProvider.qc Copyright 2026 Qore Technologies, s.r.o.

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

public namespace PineconeDataProvider {
#! Pinecone organization administration through the pinned Admin action set
public class PineconeAdminDataProvider inherits RestSchemaActionSetDataProvider {
    public {
        #! Discovery metadata for the Admin provider
        const ProviderInfo = <DataProviderInfo>{
            "name": "pinecone-admin", "type": "PineconeAdminDataProvider",
            "supports_children": True, "children_can_support_apis": True,
            "constructor_options": ConstructorOptions,
        };
        #! Direct provider construction options; connections normally supply the client
        const ConstructorOptions = {
            "oauth2_client_id": <DataProviderOptionInfo>{"display_name": "Client ID",
                "short_desc": "The service account OAuth2 client ID",
                "desc": "The OAuth2 client ID of a service account in your Pinecone organization.",
                "type": AbstractDataProviderTypeMap."string", "required": True, "sensitive": True},
            "oauth2_client_secret": <DataProviderOptionInfo>{"display_name": "Client Secret",
                "short_desc": "The service account client secret",
                "desc": "The secret issued when the service account is created or rotated.",
                "type": AbstractDataProviderTypeMap."string", "required": True, "sensitive": True},
            "url": <DataProviderOptionInfo>{"display_name": "Admin API URL", "short_desc": "Admin API base URL",
                "desc": "The Pinecone Admin API base URL.", "type": AbstractDataProviderTypeMap."string",
                "default_value": PineconeRestClientBase::DefaultControlPlaneUrl},
        };
    }

    #! Creates a provider from an authenticated REST client
    /** @param rest the connection's REST client, or nothing for metadata-only discovery
        @throw REST-SCHEMA-ACTION-ERROR the Admin manifest is invalid
    */
    constructor(*AbstractRestClient rest) : RestSchemaActionSetDataProvider(PineconeSchema::getAdminActionSet(), rest) {
    }

    #! Creates a provider directly from client credentials
    /** @param options the constructor options
        @throw CONSTRUCTOR-ERROR an option is unknown or a required credential is missing
    */
    constructor(hash<auto> options) : RestSchemaActionSetDataProvider(PineconeSchema::getAdminActionSet(),
            PineconeAdminDataProvider::getRest(options)) {
    }

    #! Returns the registered provider name
    /** @return pinecone-admin
    */
    string getName() {
        return "pinecone-admin";
    }

    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }

    private static AbstractRestClient getRest(hash<auto> options) {
        foreach string name in (keys options) {
            if (!ConstructorOptions.hasKey(name)) {
                throw "CONSTRUCTOR-ERROR", sprintf("unknown Pinecone Admin option %y", name);
            }
        }
        foreach string name in (("oauth2_client_id", "oauth2_client_secret")) {
            if (!options{name}.val()) {
                throw "CONSTRUCTOR-ERROR", sprintf("missing required Pinecone Admin option %y", name);
            }
        }
        return new PineconeAdminRestClientIo(options);
    }
}

#! Factory for the Pinecone Admin application
public class PineconeAdminDataProviderFactory inherits AbstractDataProviderFactory {
    private {
        static Class cls = new Class("PineconeAdminDataProvider");
        const FactoryInfo = <DataProviderFactoryInfo>{"name": "pinecone-admin",
            "desc": "Pinecone organization administration", "children_can_support_apis": True};
    }
    private hash<DataProviderFactoryInfo> getInfoImpl() {
        return FactoryInfo;
    }
    private hash<DataProviderInfo> getProviderInfoImpl() {
        return PineconeAdminDataProvider::ProviderInfo;
    }
    private Class getClassImpl() {
        return cls;
    }
}
}
