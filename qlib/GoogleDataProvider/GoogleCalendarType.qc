# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleDataProvider module definition

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

#! Contains all public definitions in the GoogleDataProvider module
public namespace GoogleDataProvider {
#! Google calender type
public class GoogleCalendarType inherits DataProvider::HashDataType {
    public {
        #! Field descriptions
        const Fields = {
            "kind": {
                "type": AbstractDataProviderTypeMap."string",
                "desc": "Type of the resource (`calendar#calendarListEntry`)",
            },
            "etag": {
                "type": AbstractDataProviderTypeMap."string",
                "desc": "A hash that indicates a specific version of the object definition",
            },
            "id": {
                "type": AbstractDataProviderTypeMap."string",
                "desc": "Identifier of the calendar",
            },
            "summary": {
                "type": AbstractDataProviderTypeMap."string",
                "desc": "Title of the calendar",
            },
            "description": {
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Description of the calendar",
            },
            "timeZone": {
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The time zone of the calendar",
            },
            "conferenceProperties": {
                "type": AbstractDataProviderTypeMap."*hash",
                "desc": "The types of conference solutions that are supported for this calendar.\n"
                    "The possible values are:\n"
                    "- `eventHangout`\n"
                    "- `eventNamedHangout`\n"
                    "- `hangoutsMeet`",
            },
            "location": {
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Geographic location of the calendar as free-form text",
            },
        };
    }

    #! Creates the object
    constructor() {
        map addField(new QoreDataField($1.key, $1.value.desc, $1.value.type, $1.value.default_value)),
            Fields.pairIterator();
    }
}
}
