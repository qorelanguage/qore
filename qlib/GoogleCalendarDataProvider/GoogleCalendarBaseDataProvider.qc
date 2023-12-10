# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleCalendarDataProvider module definition

/** GoogleCalendarBaseDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the GoogleCalendarDataProvider module
public namespace GoogleCalendarDataProvider {
#! The parent class for Google calendar REST APIs
public class GoogleCalendarBaseDataProvider inherits GoogleDataProvider::GoogleApiParentDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "calendar",
            "desc": "Google calendar base data provider; parent provider for APIs related to calendars",
            "type": "GoogleCalendarBaseDataProvider",
            "constructor_options": ConstructorOptions,
            "supports_children": True,
            "children_can_support_apis": True,
        };

        #! Constructor options
        const ConstructorOptions = {
            "restclient": <DataProviderOptionInfo>{
                "display_name": "REST Client",
                "short_desc": "The Google REST client to use",
                "type": AbstractDataProviderType::get(new Type("GoogleRestClient")),
                "desc": "The `GoogleRestClient` object to use",
            },
            "restclient_options": <DataProviderOptionInfo>{
                "display_name": "REST Client Options",
                "short_desc": "Google REST client options to use when creating a new client",
                "type": AbstractDataProviderTypeMap."*hash",
                "desc": "Options to the `GoogleRestClient` constructor; only used if a `GoogleRestClient` object is "
                    "created for a call",
            },
            "oauth2_client_id": <DataProviderOptionInfo>{
                "display_name": "OAuth2 Client ID",
                "short_desc": "Sets the OAuth2 client ID",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Sets the OAuth2 client ID",
            },
            "oauth2_client_secret": <DataProviderOptionInfo>{
                "display_name": "OAuth2 Client Secret",
                "short_desc": "Sets the OAuth2 client secret",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The OAuth2 client secret",
            },
            "oauth2_refresh_token": <DataProviderOptionInfo>{
                "display_name": "OAuth2 Refresh Token",
                "short_desc": "Sets the OAuth2 refresh token",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The OAuth2 refresh token, if known; complements option `token`",
            },
            "token": <DataProviderOptionInfo>{
                "display_name": "Authorization Token",
                "short_desc": "Set an authorization token to be included in requests",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Any authorization token to use for the connection; will be passed as "
                    "`Authorization: Bearer ...` (assuming that `token_type` is `Bearer`) in request headers; "
                    "this option cannot be used with `username` and `password` options or authentication information "
                    "in the URL; if this option is set then most OAuth2 options are ignored for logins",
            },
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });
    }

    private {
        const ChildMap = {
            #"calendarList": Class::forName("GoogleCalendarDataProvider::GoogleCalendarListDataProvider"),
            "calendars": Class::forName("GoogleCalendarDataProvider::GoogleCalendarsDataProvider"),
            #"insert": Class::forName("GoogleCalendarDataProvider::GoogleCalendarInsertDataProvider"),
        };
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options)
            : GoogleApiParentDataProvider(
                GoogleDiscoveryCalendarApiName, "calendars", NOTHING, "calendarList", "colors", "freebusy", "settings"
            ) {
        options = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        rest = GoogleDataProviderBase::getRestConnection(options);
    }

    #! Creates the object from a REST connection
    constructor(GoogleRestClient::GoogleRestClient rest) : GoogleApiParentDataProvider(rest,
            GoogleDiscoveryCalendarApiName, "calendars", NOTHING, "calendarList", "colors", "freebusy", "settings") {
    }

    #! Returns the data provider name
    string getName() {
        return ProviderInfo.name;
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("Google data %s provider for `%s`", ProviderInfo.name, rest.getSafeURL());
    }

    #! Return data provider summary info
    *list<hash<DataProvider::DataProviderSummaryInfo>> getChildProviderSummaryInfo() {
        *list<hash<DataProvider::DataProviderSummaryInfo>> rv =
            GoogleApiParentDataProvider::getChildProviderSummaryInfo();
        map rv += $1.getConstant("ProviderSummaryInfo").getValue(), ChildMap.iterator();
        return rv;
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        *list<string> rv = GoogleApiParentDataProvider::getChildProviderNamesImpl();
        map rv += $1, keys ChildMap;
        return rv;
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @see getChildProviderEx()
    */
    private *DataProvider::AbstractDataProvider getChildProviderImpl(string name) {
        if (*GoogleDataProviderBase rv = GoogleApiParentDataProvider::getChildProviderImpl(name)) {
            return rv;
        }
        if (*Class cls = ChildMap{name}) {
            return cls.newObject(rest);
        }
    }

    #! Returns data provider static info
    hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}
