# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleCalendarDataProvider module definition

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

#! Contains all public definitions in the GoogleCalendarDataProvider module
public namespace GoogleCalendarDataProvider {
#! The Google calendar free/busy API data provider
/** Returns Free/Busy information for the given calendar
*/
public class GoogleCalendarFreeBusyDataProvider inherits GoogleDataProvider::GoogleDataProviderBase {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "freeBusy",
            "desc": "Google calendar free/busy query API data provider",
            "type": "GoogleCalendarFreeBusyDataProvider",
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
        return sprintf("Google data %s provider for calendar `%s` for `%s`", ProviderInfo.name, id, rest.getSafeURL());
    }

    #! Makes a request and returns the response
    /** @param req the request to serialize and make according to the request type
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        hash<auto> req_info = prepareRequest(req, {"calendarId": id});
        hash<auto> info;
        try {
            hash<auto> rv = rest.post("/calendar/v3/freeBusy", req + {"items": ({"id": id},)}, \info).body;
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
        return GoogleDataProviderBase::getRequestTypeForSchema(GoogleDiscoveryCalendarApiName, "freebusy/query");
    }

    #! Returns the description of a response message, if this object represents a response message
    /** @return the response type for this response message
    */
    private *DataProvider::AbstractDataProviderType getResponseTypeImpl() {
        return GoogleDataProviderBase::getTypeForSchema(GoogleDiscoveryCalendarApiName, "FreeBusyResponse");
    }

    #! Returns data provider static info
    hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}
