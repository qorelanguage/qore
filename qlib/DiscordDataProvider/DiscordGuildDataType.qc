# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DiscordDataProvider module definition

/** DiscordGuildDataType.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the DiscordDataProvider module
public namespace DiscordDataProvider {
#! Discord guild data type
public class DiscordGuildDataType inherits HashDataType {
    public {
        #! Field descriptions
        const Fields = {
            "id": {
                "display_name": "Server ID",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The ID of the server (guild)",
            },
            "name": {
                "display_name": "Server Name",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The name of the server (guild)",
            },
            "icon": {
                "display_name": "Icon Hash",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Icon hash",
            },
            "owner": {
                "display_name": "User Owned?",
                "type": AbstractDataProviderTypeMap."*bool",
                "desc": "Does the user own this server (guild)?",
            },
            "permissions": {
                "display_name": "Server Permissions",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Permission bitfield as a string",
            },
            "features": {
                "display_name": "Server Features",
                "type": new ListDataType("DiscordGuildFeatures", StringType, True),
                "desc": "List of enabled server (guild) features",
            },
        };
    }

    #! Creates the object
    constructor() {
        addQoreFields(Fields);
    }
}
}
