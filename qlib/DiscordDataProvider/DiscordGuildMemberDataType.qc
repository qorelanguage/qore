# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DiscordDataProvider module definition

/** DiscordGuildMemberDataType.qc Copyright 2023 Qore Technologies, s.r.o.

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
#! Discord user data type
public class DiscordGuildMemberDataType inherits HashDataType {
    public {
        #! Field descriptions
        const Fields = {
            "user": {
                "display_name": "Discord User",
                "short_desc": "The user this guild member represents",
                "desc": "The user this guild member represents",
                "type": DiscordUserDataType,
            },
            "nick": {
                "display_name": "Nickname",
                "short_desc": "This user's guild nickname",
                "desc": "This user's guild nickname",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "avatar": {
                "display_name": "Avatar Hash",
                "short_desc": "The member's guild avatar hash",
                "desc": "The member's guild avatar hash",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "roles": {
                "display_name": "Roles",
                "short_desc": "Array of role object ids",
                "desc": "Array of role object ids",
                "type": new ListDataType("DiscordRoleList", StringType, True),
            },
            "joined_at": {
                "display_name": "Join Date",
                "short_desc": "When the user joined the guild",
                "desc": "When the user joined the guild",
                "type": AbstractDataProviderTypeMap."*softdate",
            },
            "premium_since": {
                "display_name": "Premium Date",
                "short_desc": "When the user started boosting the guild",
                "desc": "When the user started boosting the guild",
                "type": AbstractDataProviderTypeMap."*softdate",
            },
            "deaf": {
                "display_name": "Deafened?",
                "short_desc": "Whether the user is deafened in voice channels",
                "desc": "Whether the user is deafened in voice channels",
                "type": AbstractDataProviderTypeMap."bool",
            },
            "mute": {
                "display_name": "Mute?",
                "short_desc": "Whether the user is muted in voice channels",
                "desc": "Whether the user is muted in voice channels",
                "type": AbstractDataProviderTypeMap."bool",
            },
            "flags": {
                "display_name": "Guild Member Flags",
                "short_desc": "Guild member flags represented as a bit set",
                "desc": "Guild member flags represented as a bit set, defaults to 0",
                "type": AbstractDataProviderTypeMap."int",
            },
            "pending": {
                "display_name": "Membership Pending?",
                "short_desc": "Whether the user has not yet passed the guild's Membership Screening requirements",
                "desc": "Whether the user has not yet passed the guild's Membership Screening requirements",
                "type": AbstractDataProviderTypeMap."*bool",
            },
            "permissions": {
                "display_name": "Permissions",
                "short_desc": "Total permissions of the member in the channel",
                "desc": "Total permissions of the member in the channel, including overwrites, returned when in the "
                    "interaction object",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "communication_disabled_until": {
                "display_name": "Communication Disabled End",
                "short_desc": "When the user's communication timeout will expire",
                "desc": "When the user's timeout will expire and the user will be able to communicate in the guild "
                    "again, null or a time in the past if the user is not timed out",
                "type": AbstractDataProviderTypeMap."*softdate",
            },
        };
    }

    #! Creates the object
    constructor() {
        addQoreFields(Fields);
    }
}

#! Discord guild member data type constant
public const DiscordGuildMemberDataType = new DiscordGuildMemberDataType();
}
