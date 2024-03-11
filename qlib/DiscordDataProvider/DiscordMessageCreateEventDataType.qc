# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DiscordDataProvider module definition

/** DiscordMessageCreateEventDataType.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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
#! Discord message create payload data type
public class DiscordMessageCreatePayloadDataType inherits HashDataType {
    public {
        #! Field descriptions
        const Fields = DiscordMessageDataType::Fields + {
            "mentions": {
                "display_name": "User Mentions",
                "type": new ListDataType("DiscordGuildUserList", DiscordGuildUserDataType, True),
                "desc": "Users specifically mentioned in the message",
            },
            "guild_id": {
                "display_name": "Server ID",
                "type": AbstractDataProviderTypeMap."*string",
                "short_desc": "ID of the guild the message was sent in",
                "desc": "ID of the guild the message was sent in - unless it is an ephemeral message",
            },
            "member": {
                "display_name": "Message Author Properties",
                "type": DiscordGuildMemberDataType.getOrNothingType(),
                "short_desc": "Member properties for this message's author",
                "desc": "Member properties for this message's author. Missing for ephemeral messages and messages "
                    "from webhooks",
            },
        };
    }

    #! Creates the object
    constructor() {
        addQoreFields(Fields);
    }
}

#! Discord user info data type
public class DiscordMessageCreateEventDataType inherits DiscordGatewayEventBaseDataType {
    public {
        #! Field descriptions
        const Fields = {
            "d": {
                "display_name": "Message Data",
                "type": new DiscordMessageCreatePayloadDataType(),
                "desc": "The message create information",
            },
        };
    }

    #! Creates the object
    constructor() {
        addQoreFields(Fields);
    }
}
}
