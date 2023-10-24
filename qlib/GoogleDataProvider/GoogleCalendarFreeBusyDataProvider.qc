# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleDataProvider module definition

/** GoogleCalendarFreeBusyDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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
#! The Google calendar free/busy API data provider
/** Returns Free/Busy information for the given calendar
*/
public class GoogleCalendarFreeBusyDataProvider inherits GoogleDataProviderBase {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "freeBusy",
            "desc": "Google calendar free/busy query API data provider",
            "type": "GoogleCalendarFreeBusyDataProvider",
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
        const RequestType = new GoogleCalendarFreeBusyRequestType();

        #! Response type
        const ResponseType = new GoogleCalendarFreeBusyResponseType();
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
        return sprintf("Google data %s provider for calendar `%s` for `%s`", ProviderInfo.name, id, rest.getSafeURL());
    }

    #! Makes a request and returns the response
    /** @param req the request to serialize and make according to the request type
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        hash<auto> info;
        req.items = ({
            "id": id,
        },);
        try {
            hash<auto> rv = rest.post("/calendar/v3/freeBusy", req, \info).body;
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

#! Google calendar free/busy query request type
public class GoogleCalendarFreeBusyRequestType inherits DataProvider::HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            "timeMin": {
                "type": SoftTimestampDataProviderStringType,
                "desc": "The end of the interval for the query",
            },
            "timeMax": {
                "type": SoftTimestampDataProviderStringType,
                "desc": "The start of the interval for the query",
            },
            "timeZone": {
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Time zone used in the response. The default is UTC",
            },
            "groupExpansionMax": {
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "Maximal number of calendar identifiers to be provided for a single group. Optional. An "
                    "error is returned for a group with more members than this value. Maximum value is `100`",
            },
            "calendarExpansionMax": {
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "Maximal number of calendars for which FreeBusy information is to be provided. Optional. "
                    "Maximum value is `50`",
            },
        };
    }

    #! Creates the object
    constructor() : HashDataType("GoogleCalendarFreeBusyRequestType") {
        map addField(new QoreDataField($1.key, $1.value.desc, $1.value.type, $1.value.default_value)),
            Fields.pairIterator();
    }
}

#! Google calendar busy period data type
public class GoogleCalendarBusyPeriodDataType inherits DataProvider::HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            "start": {
                "type": AbstractDataProviderTypeMap."softdate",
                "desc": "The (inclusive) start of the time period",
            },
            "end": {
                "type": AbstractDataProviderTypeMap."softdate",
                "desc": "The (exclusive) end of the time period",
            },
        };
    }

    #! Creates the object
    constructor() : HashDataType("GoogleCalendarFreeBusyDataType") {
        map addField(new QoreDataField($1.key, $1.value.desc, $1.value.type, $1.value.default_value)),
            Fields.pairIterator();
    }
}

#! Google calendar free/busy data type
public class GoogleCalendarFreeBusyDataType inherits DataProvider::HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            "errors": {
                "type": new ListDataType("GoogleCalendarGroupErrorType", AutoHashType, True),
                "desc": "Optional error(s) (if computation for the group failed)",
            },
            "busy": {
                "type": new ListDataType("GoogleCalendarGroupCalendarBusyType",
                    new GoogleCalendarBusyPeriodDataType(), True),
                "desc": "List of time ranges during which this calendar should be regarded as busy",
            },
        };
    }

    #! Creates the object
    constructor() : HashDataType("GoogleCalendarFreeBusyDataType") {
        map addField(new QoreDataField($1.key, $1.value.desc, $1.value.type, $1.value.default_value)),
            Fields.pairIterator();
    }
}

#! Google calendar free/busy data type
public class GoogleCalendarFreeBusyBaseDataType inherits DataProvider::HashDataType {
    #! Creates the object
    constructor() : HashDataType(AutoHashOrNothingType, "GoogleCalendarFreeBusyBaseDataType") {
        setDefaultOtherFieldType(new GoogleCalendarFreeBusyDataType());
    }
}

#! Google calendar free/busy query request type
public class GoogleCalendarFreeBusyResponseType inherits DataProvider::HashDataType {
    private {
        #! Field descriptions
        const Fields = {
            "kind": {
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The type of the collection",
            },
            "timeMax": {
                "type": AbstractDataProviderTypeMap."softdate",
                "desc": "The start of the interval for the query",
            },
            "timeMin": {
                "type": AbstractDataProviderTypeMap."softdate",
                "desc": "The end of the interval for the query",
            },
            "groups": {
                "type": AbstractDataProviderTypeMap."*hash",
                "desc": "Expansion of groups",
            },
            "calendars": {
                "type": new GoogleCalendarFreeBusyBaseDataType(),
                "desc": "List of free/busy information for calendars",
            },
        };
    }

    #! Creates the object
    constructor() : HashDataType("GoogleCalendarFreeBusyResponseType") {
        map addField(new QoreDataField($1.key, $1.value.desc, $1.value.type, $1.value.default_value)),
            Fields.pairIterator();
    }
}
}
