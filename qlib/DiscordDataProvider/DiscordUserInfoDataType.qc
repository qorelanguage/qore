# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DiscordDataProvider module definition

/** DiscordUserInfoDataType.qc Copyright 2023 Qore Technologies, s.r.o.

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
#! Discord user info data type
public class DiscordUserInfoDataType inherits HashDataType {
    public {
        #! Field descriptions
        const Fields = {
            "id": {
                "display_name": "User ID",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The user ID",
            },
            "avatar": {
                "display_name": "Avatar Hash",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The user's avatar hash",
            },
            "discriminator": {
                "display_name": "User Tag",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The user's Discord-tag",
            },
            "public_flags": {
                "display_name": "Public Flags",
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "The public flags on a user's account",
            },
            "flags": {
                "display_name": "Flags",
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "The flags on a user's account",
            },
            "banner": {
                "display_name": "Banner Hash",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The user's banner hash",
            },
            "accent_color": {
                "display_name": "Accent Color Code",
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "The user's banner color encoded as an integer representation of hexadecimal color code",
            },
            "mfa_enabled": {
                "display_name": "MFA Enabled?",
                "type": AbstractDataProviderTypeMap."*bool",
                "desc": "Whether the user has multi-factor authentication enabled on their account",
            },
            "locale": {
                "display_name": "Locale",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The user's chosen language option",
            },
            "premium_type": {
                "display_name": "Nitro Subscription Code",
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "The type of Nitro subscription on a user's account",
            },
        };
    }

    #! Creates the object
    constructor() {
        addQoreFields(Fields);
    }
}
}
