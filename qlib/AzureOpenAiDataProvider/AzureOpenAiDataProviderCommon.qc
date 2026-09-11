# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore AzureOpenAiDataProvider common base class

/** AzureOpenAiDataProviderCommon.qc Copyright 2026 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the AzureOpenAiDataProvider module
public namespace AzureOpenAiDataProvider {
#! Common base for Azure OpenAI data providers
/** Resolves the Azure auth header (api-key, entra-client-credentials, or
    entra-managed-identity) on every call so cached Entra tokens are
    refreshed transparently by @ref azureauthintro "AzureAuth".
*/
public class AzureOpenAiDataProviderCommon inherits DataProvider::AbstractDataProvider {
    public {
        const DefaultApiVersion = "2024-10-21";

        const ConstructorOptions = {
            "url": <DataProviderOptionInfo>{
                "display_name": "Resource Endpoint",
                "short_desc": "Azure OpenAI resource endpoint",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "Azure OpenAI resource endpoint, e.g. "
                    "`https://my-resource.openai.azure.com`",
            },
            "azure_api_version": <DataProviderOptionInfo>{
                "display_name": "Legacy API Version",
                "short_desc": "Azure OpenAI REST API version",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Dated version for legacy deployment routes. Generated actions use their own v1 API version.",
                "default_value": DefaultApiVersion,
            },
            "azure_auth_mode": <DataProviderOptionInfo>{
                "display_name": "Authentication",
                "short_desc": "Authentication mode",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Authentication mode: "
                    "`api_key`, `entra_client_credentials`, or "
                    "`entra_managed_identity`",
                "default_value": "api_key",
                "allowed_values": (
                    <AllowedValueInfo>{"value": "api_key",
                        "display_name": "API Key"},
                    <AllowedValueInfo>{"value": "entra_client_credentials",
                        "display_name": "Entra Client Credentials"},
                    <AllowedValueInfo>{"value": "entra_managed_identity",
                        "display_name": "Entra Managed Identity"},
                ),
            },
            "api_key": <DataProviderOptionInfo>{
                "display_name": "API Key",
                "short_desc": "Azure OpenAI resource api-key",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Azure OpenAI resource api-key (when "
                    "`azure_auth_mode` = `api_key`)",
                "sensitive": True,
            },
            "azure_tenant_id": <DataProviderOptionInfo>{
                "display_name": "Entra Tenant ID",
                "short_desc": "Entra tenant ID",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Entra tenant ID (GUID or domain). Required for "
                    "`entra_client_credentials`",
            },
            "azure_client_id": <DataProviderOptionInfo>{
                "display_name": "Entra Client ID",
                "short_desc": "Entra app client ID / managed-identity client ID",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Entra app client ID for client_credentials, or "
                    "optional user-assigned managed-identity client ID",
            },
            "azure_managed_identity_client_id": <DataProviderOptionInfo>{
                "display_name": "Managed Identity Client ID",
                "short_desc": "Optional user-assigned managed identity",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Client ID of a user-assigned managed identity. Omit it to use the system-assigned identity.",
            },
            "azure_client_secret": <DataProviderOptionInfo>{
                "display_name": "Entra Client Secret",
                "short_desc": "Entra app client secret",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Entra app client secret. Required for "
                    "`entra_client_credentials`",
                "sensitive": True,
            },
            "azure_entra_scope": <DataProviderOptionInfo>{
                "display_name": "Entra Scope",
                "short_desc": "OAuth2 scope",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "OAuth2 scope; default "
                    "`https://cognitiveservices.azure.com/.default`",
                "default_value": "https://cognitiveservices.azure.com/.default",
            },
            "restclient": <DataProviderOptionInfo>{
                "display_name": "REST Client",
                "short_desc": "Pre-built RestClientIo to use",
                "type": AbstractDataProviderType::get(
                    new Type("RestClientIo::RestClientIo"), NOTHING, {
                        DTT_ClientOnly: True,
                    }),
                "desc": "Pre-built RestClientIo; when provided, the auth "
                    "options are ignored",
            },
        };
    }

    private {
        *RestClientIo::RestClientIo rest;
        string base_url;
        string api_version = DefaultApiVersion;
    }

    #! Creates the object from a pre-built RestClientIo
    constructor(RestClientIo::RestClientIo rest) {
        self.rest = cast<RestClientIo::RestClientIo>(rest.copyWithUrl(rest.getUrl()));
        self.rest.clearConnectionPath();
        base_url = rest.getUrl();
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        api_version = copts.azure_api_version ?? DefaultApiVersion;
        if (copts.restclient) {
            rest = cast<RestClientIo::RestClientIo>(copts.restclient.copyWithUrl(copts.restclient.getUrl()));
            rest.clearConnectionPath();
            base_url = rest.getUrl();
            return;
        }
        if (!copts.url.val()) {
            throw "CONSTRUCTOR-ERROR",
                "AzureOpenAiDataProvider requires 'url' when no 'restclient' is supplied";
        }
        base_url = copts.url;
        rest = new AzureOpenAiRestClient::AzureOpenAiRestClientIo(copts);
    }

    #! Returns the REST client object
    *RestClientIo::RestClientIo getRestClient() {
        return rest;
    }

    setLogger(*LoggerInterface logger) {
        LoggerWrapper::setLogger(logger);
    }

    #! Returns the deployment URL path for the given deployment + suffix
    private string getDeploymentPath(string deployment, string suffix) {
        return sprintf("/openai/deployments/%s/%s?api-version=%s",
            encode_url(deployment, True),
            suffix,
            encode_url(api_version, True));
    }

    #! Returns the configured API version
    string getApiVersion() {
        return api_version;
    }
}
}
