# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleCalendarDataProvider module definition

/** GoogleCalendarListListDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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
#! The Google calendarList list API data provider
/** This API allows the caller to list all calendars for the user
*/
public class GoogleCalendarListListDataProvider inherits GoogleDataProvider::GoogleDataProviderBase {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "list",
            "desc": "Google calendarList list API data provider",
            "type": "GoogleCalendarListListDataProvider",
            "constructor_options": GoogleCalendarBaseDataProvider::ConstructorOptions,
            "supports_request": True,
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        checkOptions("CONSTRUCTOR-ERROR", GoogleCalendarBaseDataProvider::ConstructorOptions, options);
        rest = GoogleDataProviderBase::getRestConnection(options);
    }

    #! Creates the object from a REST connection
    constructor(GoogleRestClient::GoogleRestClient rest) : GoogleDataProvider::GoogleDataProviderBase(rest) {
    }

    #! Returns the data provider name
    string getName() {
        return ProviderInfo.name;
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("Google calendarList %s provider for `%s`", ProviderInfo.name, rest.getSafeURL());
    }

    #! Makes a request and returns the response
    /** @param req the request to serialize and make according to the request type
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        *hash<auto> req_info = prepareRequest(req);
        hash<auto> info;
        try {
            hash<auto> rv = rest.get("/calendar/v3/users/me/calendarList" + req_info.query, NOTHING, \info).body;
            return getResponseTypeImpl().acceptsValue(rv);
        } catch (hash<ExceptionInfo> ex) {
            # ensure that any error response body is included in the exception
            hash<auto> ex_arg = info{"request-uri", "response-code", "response-body"};
            rethrow ex.err, ex.desc, ex.arg + ex_arg;
        }
    }

    #! Returns the description of a successful request message, if any
    /** @return the request type for this provider
    */
    private *DataProvider::AbstractDataProviderType getRequestTypeImpl() {
        return GoogleDataProviderBase::getRequestTypeForSchema(GoogleDiscoveryCalendarApiName, "calendarList/list");
    }

    #! Returns the description of a response message, if this object represents a response message
    /** @return the response type for this response message
    */
    private *DataProvider::AbstractDataProviderType getResponseTypeImpl() {
        return GoogleDataProviderBase::getTypeForSchema(GoogleDiscoveryCalendarApiName, "CalendarList");
    }

    #! Returns data provider static info
    hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}

#! Google calenderList list request data type
public class GoogleCalendarListRequestType inherits DataProvider::HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            # query parameters
            "maxResults": {
                "display_name": "Max Results",
                "short_desc": "Maximum number of entries returned on one result page",
                "type": AbstractDataProviderTypeMap."int",
                "desc": "Maximum number of entries returned on one result page. By default the value is 100 entries. "
                    "The page size can never be larger than 250 entries",
                "default_value": 100,
            },
            "minAccessRole": {
                "display_name": "Min Access Role",
                "short_desc": "The minimum access role for the user in the returned entries",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The minimum access role for the user in the returned entries. Optional. The default is no "
                    "restriction\n"
                    "Acceptable values are:\n"
                    "- `freeBusyReader`: The user can read free/busy information\n"
                    "- `owner`: The user can read and modify events and access control lists\n"
                    "- `reader`: The user can read events that are not private\n"
                    "- `writer`: The user can read and modify events",
                "allowed_values": (
                    "freeBusyReader",
                    "owner",
                    "reader",
                    "writer",
                ),
            },
            "pageToken": {
                "display_name": "Page Token",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Token specifying which result page to return",
            },
            "showDeleted": {
                "display_name": "Show Deleted?",
                "type": SoftBoolOrNothingDataProviderStringType,
                "desc": "Whether to include deleted calendar list entries in the result",
                "default_value": False,
            },
            "showHidden": {
                "display_name": "Show Hidden?",
                "type": SoftBoolOrNothingDataProviderStringType,
                "desc": "Whether to show hidden entries",
                "default_value": False,
            },
            "syncToken": {
                "display_name": "Sync Token",
                "short_desc": "Token obtained from the nextSyncToken field returned in the last results",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Token obtained from the nextSyncToken field returned on the last page of results from the "
                    "previous list request. It makes the result of this list request contain only entries that have "
                    "changed since then. If only read-only fields such as calendar properties or ACLs have changed, "
                    "the entry won't be returned. All entries deleted and hidden since the previous list request "
                    "will always be in the result set and it is not allowed to set showDeleted neither showHidden to "
                    "`False`. To ensure client state consistency, the `minAccessRole` query parameter cannot be "
                    "specified together with `nextSyncToken`.\n"
                    "If the `syncToken` expires, the server will respond with a `410 GONE` response code and the "
                    "client should clear its storage and perform a full synchronization without any `syncToken`.\n"
                    "The default is to return all entries",
            },
        };
    }

    #! Creates the object
    constructor() {
        addQoreFields(Fields);
    }
}
}
