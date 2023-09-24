# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleDataProvider module definition

/** GoogleCalendarEventsPatchDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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
#! The Google calendar event patch API data provider
/** This API allows the caller to patch a calendar event
*/
public class GoogleCalendarEventsPatchDataProvider inherits GoogleDataProviderBase {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "patch",
            "desc": "Google calendar events patch calender event API data provider",
            "type": "GoogleCalendarEventsPatchDataProvider",
            "constructor_options": GoogleDataProvider::ConstructorOptions + {
                "id": <DataProviderOptionInfo>{
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
        const RequestType = new GoogleCalendarEventsPatchRequestType();

        #! Response type
        const ResponseType = new GoogleCalendarEventsElementType();

        #! Query args
        const QueryArgs = ("conferenceDataVersion", "maxAttendees", "sendUpdates", "supportsAttachments");
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
        foreach hash<auto> i in (req{QueryArgs}.pairIterator()) {
            # this handles list key values by repeating the expression in the output
            map (query_args += ((query_args ? "&" : "?") + sprintf("%s=%s", i.key, $1))), i.value;
            remove req{i.key};
        }
        hash<auto> info;
        try {
            hash<auto> rv = rest.patch("/calendar/v3/calendars/" + id + "/events/" + req.id + query_args,
                req, \info).body;
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

#! Google event patch request data type
public class GoogleCalendarEventsPatchRequestType inherits GoogleCalendarEventsGetRequestType {
    public {
        #! Field descriptions
        const Fields = {
            # query args
            "conferenceDataVersion": {
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "Version number of conference data supported by the API client. Version `0` assumes no "
                    "conference data support and ignores conference data in the event's body. Version `1` enables "
                    "support for copying of `ConferenceData` as well as for creating new conferences using the "
                    "`createRequest` field of `conferenceData`. The default is `0`. Acceptable values are `0` to "
                    "`1`, inclusive",
                "allowed_values": (
                    0,
                    1,
                ),
            },
            "maxAttendees": {
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "The maximum number of attendees to include in the response. If there are more than the "
                    "specified number of attendees, only the participant is returned. Optional",
            },
            "sendUpdates": {
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Guests who should receive notifications about the event update (for example, title changes, "
                    "etc.).\n"
                    "Acceptable values are:\n"
                    "- `all`: Notifications are sent to all guests\n"
                    "- `externalOnly`: Notifications are sent to non-Google Calendar guests only\n"
                    "- `none`: No notifications are sent. For calendar migration tasks, consider using the "
                        "`Events.import` method instead",
                "allowed_values": (
                    "all",
                    "externalOnly",
                    "none",
                ),
            },
            "supportsAttachments": {
                "type": SoftBoolDataProviderStringType,
                "desc": "Whether API client performing operation supports event attachments. Optional. The default "
                    "is `False`",
            },
        } + map {$1.key: $1.value + {"type": $1.value.type.getOrNothingType()}},
            GoogleCalendarEventsElementType::Fields.pairIterator(), $1.value.updatable;
    }

    #! Creates the object
    constructor() : GoogleCalendarEventsGetRequestType("GoogleCalendarEventsPatchRequestType") {
        map addField(new QoreDataField($1.key, $1.value.desc, $1.value.type, $1.value.default_value,
            $1.value.allowed_values)), Fields.pairIterator();
    }
}
}