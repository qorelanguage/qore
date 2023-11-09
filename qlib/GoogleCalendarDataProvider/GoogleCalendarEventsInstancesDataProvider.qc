# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleCalendarDataProvider module definition

/** GoogleCalendarEventsInstancesDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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
#! The Google calendar event instances API data provider
/** This API allows the caller to get instances of a recurring calendar event
*/
public class GoogleCalendarEventsInstancesDataProvider inherits GoogleDataProvider::GoogleDataProviderBase {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "instances",
            "desc": "Google calendar events get instances of a recurring event API data provider",
            "type": "GoogleCalendarEventsInstancesDataProvider",
            "constructor_options": GoogleCalendarBaseDataProvider::ConstructorOptions + {
                "id": <DataProviderOptionInfo>{
                    "display_name": "Calendar ID",
                    "short_desc": "The calendar ID",
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "The calendar ID",
                },
            },
            "supports_request": True,
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });

        #! Request type
        const RequestType = new GoogleCalendarEventsInstancesRequestType();
    }

    private {
        #! The calendar's ID
        string id;

        #! The calendar's metadata
        *hash<auto> cal;
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        checkOptions("CONSTRUCTOR-ERROR", GoogleCalendarBaseDataProvider::ConstructorOptions, options);
        rest = GoogleDataProviderBase::getRestConnection(options);
        id = options.id;
    }

    #! Creates the object from a REST connection
    constructor(GoogleRestClient::GoogleRestClient rest, string id, *hash<auto> cal) : GoogleDataProvider::GoogleDataProviderBase(rest) {
        self.id = id;
        self.cal = cal;
    }

    #! Returns the data provider name
    string getName() {
        return ProviderInfo.name;
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("Google calendar events %s provider for `%s`", ProviderInfo.name, rest.getSafeURL());
    }

    #! Makes a request and returns the response
    /** @param req the request to serialize and make according to the request type
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        string query_args;
        foreach hash<auto> i in (req.pairIterator()) {
            # this handles list key values by repeating the expression in the output
            map (query_args += ((query_args ? "&" : "?") + sprintf("%s=%s", i.key, $1))), i.value;
        }
        hash<auto> info;
        try {
            hash<auto> rv = rest.get("/calendar/v3/calendars/" + id + "/events/" + req.id + "/instances" + query_args,
                NOTHING, \info).body;
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
        return RequestType;
    }

    #! Returns the description of a response message, if this object represents a response message
    /** @return the response type for this response message
    */
    private *DataProvider::AbstractDataProviderType getResponseTypeImpl() {
        return GoogleDataProviderBase::getTypeForSchema(GoogleDiscoveryCalendarApiName, "Events");
    }

    #! Returns data provider static info
    hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}

#! Google calendar event instances request data type
public class GoogleCalendarEventsInstancesRequestType inherits GoogleCalendarEventsGetRequestType {
    private {
        #! Field descriptions
        const Fields = {
            # query parameters
            "maxResults": {
                "display_name": "Max Results",
                "short_desc": "Maximum number of events returned on one result page",
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "Maximum number of events returned on one result page. The number of events in the resulting "
                    "page may be less than this value, or none at all, even if there are more events matching the "
                    "query. Incomplete pages can be detected by a non-empty `nextPageToken` field in the response. By "
                    "default the value is `250` events. The page size can never be larger than `2500` events",
            },
            "originalStart": {
                "display_name": "Original Start Time",
                "type": SoftTimestampDataProviderStringType,
                "desc": "The original start time of the instance in the result. Optional",
            },
            "pageToken": {
                "display_name": "Page Token",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Token specifying which result page to return",
            },
            "showDeleted": {
                "display_name": "Show Deleted?",
                "short_desc": "Whether to include deleted events in the result",
                "type": SoftBoolOrNothingDataProviderStringType,
                "desc": "Whether to include deleted events (with status equals `cancelled`) in the result. Cancelled "
                    "instances of recurring events (but not the underlying recurring event) will still be included "
                    "if `showDeleted` and `singleEvents` are both `False`. If `showDeleted` and `singleEvents` are "
                    "both `True`, only single instances of deleted events (but not the underlying recurring events) "
                    "are returned",
                "default_value": False,
            },
            "timeMax": {
                "display_name": "Max Time",
                "short_desc": "Upper bound (exclusive) for an event's start time to filter by",
                "type": SoftTimestampDataProviderStringType,
                "desc": "Upper bound (exclusive) for an event's start time to filter by. The default is not to "
                    "filter by start time. Resolution to the second. If `timeMin` is set, `timeMax` must be greater "
                    "than `timeMin`",
            },
            "timeMin": {
                "display_name": "Min Time",
                "short_desc": "Lower bound (exclusive) for an event's end time to filter by",
                "type": SoftTimestampDataProviderStringType,
                "desc": "Lower bound (exclusive) for an event's end time to filter by. The default is not to filter "
                    "by end time. Resolution to the second. If `timeMax` is set, `timeMin` must be smaller than "
                    "`timeMax`",
            },
        };
    }

    #! Creates the object
    constructor() : GoogleCalendarEventsGetRequestType("GoogleCalendarEventsInstancesRequestType") {
        addQoreFields(Fields);
    }
}
}