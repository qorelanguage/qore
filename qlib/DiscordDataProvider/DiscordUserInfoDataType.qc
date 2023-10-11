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
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The user ID",
            },
            "avatar": {
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The user's avatar hash",
            },
            "discriminator": {
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The user's Discord-tag",
            },
            "public_flags": {
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "The public flags on a user's account",
            },
            "flags": {
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "The flags on a user's account",
            },
            "banner": {
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The user's banner hash",
            },
            "accent_color": {
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "The user's banner color encoded as an integer representation of hexadecimal color code",
            },
            "mfa_enabled": {
                "type": AbstractDataProviderTypeMap."*bool",
                "desc": "Whether the user has multi-factor authentication enabled on their account",
            },
            "locale": {
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "The user's chosen language option",
            },
            "premium_type": {
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "The type of Nitro subscription on a user's account",
            },
        };
    }

    #! Creates the object
    constructor() {
        map addField(new QoreDataField($1.key, $1.value.desc, $1.value.type, $1.value.default_value,
            $1.value.allowed_values)), Fields.pairIterator();
    }
}
}
