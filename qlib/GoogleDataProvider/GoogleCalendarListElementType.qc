# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleDataProvider module definition

/** GoogleCalendarListElementType.qc Copyright 2023 Qore Technologies, s.r.o.

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
#! Google calenderList element type
public class GoogleCalendarListElementType inherits GoogleCalendarType {
    private {
        #! Field descriptions
        const Fields = {
            "summaryOverride": {
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The summary that the authenticated user has set for this calendar",
            },
            "colorId": {
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The color of the calendar. This is an ID referring to an entry in the calendar section of "
                    "the colors definition (see the colors endpoint). This property is superseded by the "
                    "`backgroundColor` and `foregroundColor` properties and can be ignored when using these "
                    "properties",
            },
            "foregroundColor": {
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The foreground color of the calendar in the hexadecimal format `#ffffff`. This property "
                    "supersedes the index-based colorId property. To set or change this property, you need to "
                    "specify `colorRgbFormat=true` in the parameters of the insert, update and patch methods",
            },
            "backgroundColor": {
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The background color of the calendar in the hexadecimal format `#ffffff`. This property "
                    "supersedes the index-based colorId property. To set or change this property, you need to "
                    "specify `colorRgbFormat=true` in the parameters of the insert, update and patch methods",
            },
            "selected": {
                "type": AbstractDataProviderTypeMap."*bool",
                "desc": "Whether the calendar content shows up in the calendar UI",
            },
            "deleted": {
                "type": AbstractDataProviderTypeMap."*bool",
                "desc": "Whether this calendar list entry has been deleted from the calendar list",
            },
            "primary": {
                "type": AbstractDataProviderTypeMap."*bool",
                "desc": "Whether the calendar is the primary calendar of the authenticated user",
            },
            "accessRole": {
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The effective access role that the authenticated user has on the calendar. Read-only. "
                    "Possible values are:\n"
                    "- `freeBusyReader`: Provides read access to free/busy information\n"
                    "- `reader`: Provides read access to the calendar. Private events will appear to users with "
                        "reader access, but event details will be hidden\n"
                    "- `writer`: Provides read and write access to the calendar. Private events will appear to users "
                        "with writer access, and event details will be visible\n"
                    "- `owner`: Provides ownership of the calendar. This role has all of the permissions of the "
                        "writer role with the additional ability to see and manipulate ACLs",
            },
            "defaultReminders": {
                "type": AbstractDataProviderTypeMap."*list",
                "desc": "The default reminders that the authenticated user has for this calendar",
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
