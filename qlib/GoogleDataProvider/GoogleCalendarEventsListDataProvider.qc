# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleDataProvider module definition

/** GoogleCalendarEventsListDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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
#! The Google calendar events list API data provider
/** This API allows the caller to list calendar events for the user
*/
public class GoogleCalendarEventsListDataProvider inherits GoogleDataProviderBase {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "list",
            "desc": "Google calendar events list API data provider",
            "type": "GoogleCalendarEventsListDataProvider",
            "constructor_options": GoogleDataProvider::ConstructorOptions + {
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
        const RequestType = new GoogleCalendarEventsListRequestType();

        #! Response type
        const ResponseType = new GoogleCalendarEventsListResponseType();
    }

    private {
        #! The calendar's ID
        string id;

        #! The calendar's metadata
        *hash<auto> cal;
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        checkOptions("CONSTRUCTOR-ERROR", GoogleDataProvider::ConstructorOptions, options);
        rest = GoogleDataProvider::getRestConnection(options);
        id = options.id;
    }

    #! Creates the object from a REST connection
    constructor(GoogleRestClient::GoogleRestClient rest, string id, *hash<auto> cal) : GoogleDataProviderBase(rest) {
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
            hash<auto> rv = rest.get("/calendar/v3/calendars/" + id + "/events" + query_args, NOTHING, \info).body;
            return ResponseType.acceptsValue(rv);
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
        return ResponseType;
    }

    #! Returns data provider static info
    hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}

#! Google calender events list request data type
public class GoogleCalendarEventsListRequestType inherits DataProvider::HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            # query parameters
            "eventTypes": {
                "display_name": "Event Types",
                "short_desc": "Event types to return",
                "type": AbstractDataProviderType::get(new Type("*softlist<string>")),
                "desc": "Event types to return. Optional. Possible values are:\n"
                    "- `default`\n"
                    "- `focusTime`\n"
                    "- `outOfOffice`\n"
                    "- `workingLocation`\n"
                    "Currently, these are the only allowed values for this field:\n"
                    "- `default`, `focusTime`, `outOfOffice`\n"
                    "- `default`, `focusTime`, `outOfOffice`, `workingLocation`\n"
                    "- `workingLocation`",
                "allowed_values": (
                    "default",
                    "focusTime",
                    "outOfOffice",
                    "workingLocation",
                ),
                "default_value": ("default", "focusTime", "outOfOffice"),
            },
            "iCalUID": {
                "display_name": "iCal Event ID",
                "short_desc": "Sets an event ID in the iCalendar format to be provided in the response",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Specifies an event ID in the iCalendar format to be provided in the response. Optional. Use "
                    "this if you want to search for an event by its iCalendar ID",
            },
            "maxAttendees": {
                "display_name": "Max Attendees",
                "short_desc": "Max number of attendees to include in the response",
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "The maximum number of attendees to include in the response. If there are more than the "
                    "specified number of attendees, only the participant is returned",
            },
            "maxResults": {
                "display_name": "Max Results",
                "short_desc": "Max number of events returned on one result page",
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "Maximum number of events returned on one result page. The number of events in the resulting "
                    "page may be less than this value, or none at all, even if there are more events matching the "
                    "query. Incomplete pages can be detected by a non-empty `nextPageToken` field in the response. By "
                    "default the value is `250` events. The page size can never be larger than `2500` events",
            },
            "orderBy": {
                "display_name": "Order By",
                "short_desc": "The order of the events returned in the result",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The order of the events returned in the result. The default is an unspecified, stable "
                    "order.\n"
                    "Acceptable values are:\n"
                    "- `startTime`: Order by the start date/time (ascending). This is only available when querying "
                        "single events (i.e. the parameter `singleEvents` is `True`)\n"
                    "- `updated`: Order by last modification time (ascending)",
                "allowed_values": (
                    "startTime",
                    "updated",
                ),
            },
            "pageToken": {
                "display_name": "Page Token",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Token specifying which result page to return",
            },
            "privateExtendedProperty": {
                "display_name": "Private Extended Property",
                "short_desc": "Extended properties constraint to match",
                "type": AbstractDataProviderType::get(new Type("*softlist<string>")),
                "desc": "Extended properties constraint specified as propertyName=value. Matches only private "
                    "properties. This parameter might be repeated multiple times to return events that match all "
                    "given constraints",
            },
            "q": {
                "display_name": "Free Text Query",
                "short_desc": "Free text search terms to find events",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Free text search terms to find events that match these terms in the following fields: "
                    "`summary`, `description`, `location`, attendee's `displayName`, attendee's `email`",
            },
            "sharedExtendedProperty": {
                "display_name": "Shared Extended Property",
                "short_desc": "Extended properties constraint to match",
                "type": AbstractDataProviderType::get(new Type("*softlist<string>")),
                "desc": "Extended properties constraint specified as propertyName=value. Matches only shared "
                    "properties. This parameter might be repeated multiple times to return events that match all "
                    "given constraints",
            },
            "showDeleted": {
                "display_name": "Show Deleted?",
                "short_desc": "Whether to include deleted events in the result",
                "type": SoftBoolDataProviderStringType,
                "desc": "Whether to include deleted events (with status equals `cancelled`) in the result. Cancelled "
                    "instances of recurring events (but not the underlying recurring event) will still be included "
                    "if `showDeleted` and `singleEvents` are both `False`. If `showDeleted` and `singleEvents` are "
                    "both `True`, only single instances of deleted events (but not the underlying recurring events) "
                    "are returned",
                "default_value": False,
            },
            "showHiddenInvitations": {
                "display_name": "Show Hidden Invitations?",
                "type": SoftBoolDataProviderStringType,
                "desc": "Whether to include hidden invitations in the result",
                "default_value": False,
            },
            "singleEvents": {
                "display_name": "Single Events?",
                "short_desc": "Whether to expand recurring events",
                "type": SoftBoolDataProviderStringType,
                "desc": "Whether to expand recurring events into instances and only return single one-off events and "
                    "instances of recurring events, but not the underlying recurring events themselves",
                "default_value": False,
            },
            "syncToken": {
                "display_name": "Sync Token",
                "short_desc": "Token obtained from the last results",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Token obtained from the `nextSyncToken` field returned on the last page of results from the "
                    "previous list request. It makes the result of this list request contain only entries that have "
                    "changed since then. All events deleted since the previous list request will always be in the "
                    "result set and it is not allowed to set `showDeleted` to False.\n\n"
                    "There are several query parameters that cannot be specified together with `nextSyncToken` to "
                    "ensure consistency of the client state.\n\n"
                    "These are:\n"
                    "- `iCalUID`\n"
                    "- `orderBy`\n"
                    "- `privateExtendedProperty`\n"
                    "- `q`\n"
                    "- `sharedExtendedProperty`\n"
                    "- `timeMin`\n"
                    "- `timeMax`\n"
                    "- `updatedMin`\n\n"
                    "All other query parameters should be the same as for the initial synchronization to avoid "
                    "undefined behavior. If the `syncToken` expires, the server will respond with a `410 GONE` "
                    "response code and the client should clear its storage and perform a full synchronization "
                    "without any syncToken.\n"
                    "The default is to return all entries",
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
            "timeZone": {
                "display_name": "Time Zone",
                "short_desc": "Time zone used in the response",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Time zone used in the response. The default is the time zone of the calendar",
            },
            "updatedMin": {
                "display_name": "Minimum Update Time",
                "short_desc": "Lower bound for an event's last modification time to filter by",
                "type": SoftTimestampDataProviderStringType,
                "desc": "Lower bound for an event's last modification time to filter by. When specified, entries "
                    "deleted since this time will always be included regardless of `showDeleted`. The default is not "
                    "to filter by last modification time",
            },
        };
    }

    #! Creates the object
    constructor() : HashDataType("GoogleCalendarEventsListRequestType") {
        addQoreFields(Fields);
    }
}

#! Google date/time object
public class GoogleCalendarDateTimeHashType inherits DataProvider::HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            "date": {
                "display_name": "Date",
                "short_desc": "The date for all-day events",
                "type": AbstractDataProviderTypeMap."*softdate",
                "desc": "The date, in the format `yyyy-mm-dd`, if this is an all-day event",
            },
            "dateTime": {
                "display_name": "Date and Time",
                "type": AbstractDataProviderTypeMap."*softdate",
                "desc": "The time, as a combined date-time value",
            },
            "timeZone": {
                "display_name": "Time Zone",
                "short_desc": "The time zone in which the time is specified",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The time zone in which the time is specified, formatted as an IANA Time Zone Database name, "
                    "e.g. `Europe/Zurich`",
            },
        };
    }

    #! Creates the object
    constructor() : HashDataType(AutoHashOrNothingType, "GoogleCalendarDateTimeHashType") {
        addQoreFields(Fields);
    }
}

#! Timestamp string type for query parameters
public const GoogleCalendarDateTimeHashType = new GoogleCalendarDateTimeHashType();

public class GoogleCalendarSourceHashBaseType inherits HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            "title": {
                "display_name": "Source Title",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Title of the source; for example a title of a web page or an email subject",

            },
            "url": {
                "display_name": "URL",
                "short_desc": "URL of the source pointing to a resource",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "URL of the source pointing to a resource. The URL scheme must be HTTP or HTTPS",
            },
        };
    }

    #! Creates the object
    constructor(bool or_nothing) : HashDataType(or_nothing ? AutoHashOrNothingType : AutoHashType,
            "GoogleCalendarSourceHashType") {
        addQoreFields(Fields);
    }
}

public class GoogleCalendarSourceHashType inherits GoogleCalendarSourceHashBaseType {
    constructor() : GoogleCalendarSourceHashBaseType(False) {
    }
}

public class GoogleCalendarSourceHashOrNothingType inherits GoogleCalendarSourceHashBaseType {
    constructor() : GoogleCalendarSourceHashBaseType(True) {
    }
}

#! Google calendar source type
public const GoogleCalendarSourceHashType = new GoogleCalendarSourceHashType();

#! Google calendar source "or nothing" type
public const GoogleCalendarSourceHashOrNothingType = new GoogleCalendarSourceHashOrNothingType();

#! Base type for Google calendar time hash types
public class GoogleCalendarTimeHashBaseType inherits HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            "date": {
                "display_name": "Date",
                "short_desc": "The date for all-day events",
                "type": SoftDateDataProviderStringType,
                "desc": "The date, in the format `yyyy-mm-dd`, if this is an all-day event",

            },
            "dateTime": {
                "display_name": "Date and Time",
                "short_desc": "The time, as a combined date-time value",
                "type": SoftTimestampDataProviderStringType,
                "desc": "The time, as a combined date-time value (formatted according to RFC3339). A time zone "
                    "offset is required unless a time zone is explicitly specified in `timeZone`",
            },
            "timeZone": {
                "display_name": "Time Zone",
                "short_desc": "The time zone region in which the time is specified",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The time zone in which the time is specified. (Formatted as an IANA Time Zone Database "
                    "name, e.g. `Europe/Zurich`.) For recurring events this field is required and specifies the time "
                    "zone in which the recurrence is expanded. For single events this field is optional and "
                    "indicates a custom time zone for the event start/end",
            },
        };
    }

    #! Creates the object
    constructor(bool or_nothing) : HashDataType(or_nothing ? AutoHashOrNothingType : AutoHashType,
            "GoogleCalendarTimeHashType") {
        addQoreFields(Fields);
    }
}

public class GoogleCalendarTimeHashType inherits GoogleCalendarTimeHashBaseType {
    constructor() : GoogleCalendarTimeHashBaseType(False) {
    }
}

public class GoogleCalendarTimeHashOrNothingType inherits GoogleCalendarTimeHashBaseType {
    constructor() : GoogleCalendarTimeHashBaseType(True) {
    }
}

#! Google calendar time hash type
public const GoogleCalendarTimeHashType = new GoogleCalendarTimeHashType();

#! Google calendar time hash "or nothing" type
public const GoogleCalendarTimeHashOrNothingType = new GoogleCalendarTimeHashOrNothingType();

#! Create calendar events request data type
public class GoogleCalendarEventsListResponseType inherits DataProvider::HashDataType {
    public {
        #! Field descriptions
        const Fields = {
            "kind": {
                "display_name": "Collecton Type",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The type of the collection",
            },
            "etag": {
                "display_name": "Object Version",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "A hash that indicates a specific version of the object definition",
            },
            "summary": {
                "display_name": "Calendar Title",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "Title of the calendar",
            },
            "description": {
                "display_name": "Calendar Description",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "Description of the calendar",
            },
            "updated": {
                "display_name": "Last Updated",
                "type": AbstractDataProviderTypeMap."softdate",
                "desc": "Last modification time of the calendar",
            },
            "timeZone": {
                "display_name": "Time Zone",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The time zone of the calendar",
            },
            "accessRole": {
                "display_name": "Access Role",
                "short_desc": "The user's access role for this calendar",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The user's access role for this calendar. Read-only. Possible values are:\n"
                    "- `none`: The user has no access\n"
                    "- `freeBusyReader`: The user has read access to free/busy information\n"
                    "- `reader`: The user has read access to the calendar. Private events will appear to users with "
                        "reader access, but event details will be hidden\n"
                    "- `writer`: The user has read and write access to the calendar. Private events will appear to "
                        "users with writer access, and event details will be visible\n"
                    "- `owner`: The user has ownership of the calendar. This role has all of the permissions of the "
                        "writer role with the additional ability to see and manipulate ACLs",
            },
            "defaultReminders":{
                "display_name": "Default Reminders",
                "short_desc": "The default reminders on the calendar for the authenticated user",
                "type": AbstractDataProviderTypeMap."list",
                "desc": "The default reminders on the calendar for the authenticated user. These reminders apply to "
                    "all events on this calendar that do not explicitly override them (i.e. do not have "
                    "`reminders.useDefault` set to `True`)",
            },
            "nextPageToken": {
                "display_name": "Next Page Token",
                "short_desc": "Token used to access the next page of this result",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Token used to access the next page of this result. Omitted if no further results are "
                    "available, in which case `nextSyncToken` is provided",
            },
            "nextSyncToken": {
                "display_name": "Next Sync Token",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "A synchronization token for the data",
            },
            "items": {
                "display_name": "Event Items",
                "type": new ListDataType("*GoogleCalendarEvents", GoogleCalendarEventsElementType, True),
                "desc": "Response items",
            }
        };
    }

    #! Creates the object
    constructor() : HashDataType("GoogleCalendarEventsListResponseType") {
        addQoreFields(Fields);
    }
}
}
