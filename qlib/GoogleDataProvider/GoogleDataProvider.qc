# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleDataProvider module definition

/** GoogleDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the GoogleDataProvider module
public namespace GoogleDataProvider {
#! The Google data provider class
public class GoogleDataProvider inherits DataProvider::AbstractDataProvider {
    public {
        #! The REST client object for API calls
        *GoogleRestClient::GoogleRestClient rest;

        #! current URI path
        string uri_path = "/";

        #! The value to returns as the name of the object
        string display_name;

        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "GoogleDataProvider",
            "supports_children": True,
            "constructor_options": ConstructorOptions,
            "children_can_support_apis": True,
        };

        #! Constructor options
        const ConstructorOptions = {
            "restclient": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(new Type("GoogleRestClient")),
                "desc": "The GoogleRestClient object",
            },
            "restclient_options": <DataProviderOptionInfo>{
                "type": AbstractDataProviderTypeMap."*hash",
                "desc": "Options to the GoogleRestClient constructor; only used if a GoogleRestClient object is "
                    "created for a call",
            },
            "oauth2_client_id": <DataProviderOptionInfo>{
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The OAuth2 client ID",
            },
            "oauth2_client_secret": <DataProviderOptionInfo>{
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The OAuth2 client secret",
            },
            "oauth2_refresh_token": <DataProviderOptionInfo>{
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The OAuth2 refresh token, if known",
            },
            "token": <DataProviderOptionInfo>{
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The OAuth2 token, if known",
            },
        };
    }

    private {
        const ChildMap = {
            "calendar": Class::forName("GoogleDataProvider::GoogleCalendarBaseDataProvider"),
        };
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        rest = GoogleDataProvider::getRestConnection(copts);
    }

    #! Creates the object from a GoogleRestClient
    constructor(GoogleRestClient rest) {
        rest.setConnectionPath("/");
        self.rest = rest;
    }

    static RestClient::RestClient getRestConnection(*hash<auto> options) {
        RestClient rest;
        if (options.restclient) {
            rest = options.restclient;
        } else {
            if (!options.oauth2_client_id) {
                throw "CONSTRUCTOR-ERROR", "no 'restclient' or 'ouath2_client_id' option passed; "
                    "cannot create REST client to Google instance without authentication information";
            }
            if (!options.oauth2_client_secret) {
                throw "CONSTRUCTOR-ERROR", "no 'restclient', 'oauth2_client_secret' option passed; "
                    "cannot create REST client to Google instance without authentication information";
            }
            if (!options.token) {
                throw "CONSTRUCTOR-ERROR", "no 'restclient', 'token' option passed; "
                    "cannot create REST client to Google instance without authentication information";
            }

            hash<auto> opts;
            opts += options.restclient_options + options{
                "oauth2_client_id",
                "oauth2_client_secret",
                "token",
                "oauth2_refresh_token",
            };
            rest = new GoogleRestClient(opts);
        }
        rest.setConnectionPath("/");
        return rest;
    }

    #! Returns the data provider name
    string getName() {
        return "google";
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("Google  REST API data provider for `%s`", rest.getSafeURL());
    }

    #! Return data provider summary info
    *list<hash<DataProvider::DataProviderSummaryInfo>> getChildProviderSummaryInfo() {
        return map $1.getConstant("ProviderSummaryInfo").getValue(), ChildMap.iterator();
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        return keys ChildMap;
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @see getChildProviderEx()
    */
    private *DataProvider::AbstractDataProvider getChildProviderImpl(string name) {
        *Class cls = ChildMap{name};
        if (!cls) {
            return;
        }
        return cls.newObject(rest);
    }

    #! Returns data provider static info
    private hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}

#! Boolean string type for query parameters
public const SoftBoolStringType = new SoftBoolStringType();

#! Boolean data provider string type for query parameters
public const SoftBoolDataProviderStringType = AbstractDataProviderType::get(SoftBoolStringType);

#! Boolean string type
public class SoftBoolStringType inherits Qore::Reflection::Type {
    #! Creates the object
    constructor() : Type("*softstring") {
    }

    #! Returns the value after any conversions by the type
    auto acceptsValue(auto value) {
        return parse_boolean(value) ? "true" : "false";
    }

    #! Returns the default value for the type or NOTHING if the type has no default value
    auto getDefaultValue() {
        return "false";
    }
}

#! Data provider type for timestamps in API arguments
const SoftTimestampDataProviderStringType = new SoftTimestampDataProviderStringType();

#! Timestamp string type
public class SoftTimestampDataProviderStringType inherits DataProvider::QoreSoftStringOrNothingDataType {
    #! Creates the object
    constructor() {
    }

    #! Returns the value after any conversions by the type
    auto acceptsValue(auto value) {
        return exists value ? date(value).format("YYYY-MM-DDTHH:mm:SSZ") : NOTHING;
    }
}

#! Data provider type for timestamps in API arguments
const SoftDateDataProviderStringType = new SoftDateDataProviderStringType();

#! Date string type
public class SoftDateDataProviderStringType inherits DataProvider::QoreSoftStringOrNothingDataType {
    #! Creates the object
    constructor() {
    }

    #! Returns the value after any conversions by the type
    auto acceptsValue(auto value) {
        return exists value ? date(value).format("YYYY-MM-DD") : NOTHING;
    }
}
}
