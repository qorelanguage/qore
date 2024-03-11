# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DiscordDataProvider module definition

/** DiscordMessageDataType.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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
public class DiscordMessageDataType inherits HashDataType {
    public {
        #! Field descriptions
        const Fields = {
            "id": {
                "display_name": "Message ID",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The ID of the message",
            },
            "channel_id": {
                "display_name": "Channel ID",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "The ID of the channel the message was sent in",
            },
            "author": {
                "display_name": "Author",
                "type": DiscordUserDataType,
                "desc": "The author of this message (not guaranteed to be a valid user)",
            },
            "content": {
                "display_name": "Message Content",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "Content of the message",
            },
            "timestamp": {
                "display_name": "Timestamp",
                "type": AbstractDataProviderTypeMap."date",
                "desc": "When this message was sent",
            },
            "edited_timestamp": {
                "display_name": "Last Edited Timestamp",
                "type": AbstractDataProviderTypeMap."*date",
                "desc": "When this message was edited (or null if never)",
            },
            "tts": {
                "display_name": "Text-To-Speech?",
                "type": AbstractDataProviderTypeMap."bool",
                "desc": "Whether this was a TTS message",
            },
            "mention_everyone": {
                "display_name": "Mention Everyone?",
                "type": AbstractDataProviderTypeMap."bool",
                "desc": "Whether this message mentions everyone",
            },
            "mentions": {
                "display_name": "User Mentions",
                "type": new ListDataType("DiscordUserList", DiscordUserDataType, True),
                "desc": "Users specifically mentioned in the message",
            },
            "mention_roles": {
                "display_name": "Role Mentions",
                # array of roles
                "type": AbstractDataProviderTypeMap."*list",
                "desc": "Roles specifically mentioned in this message",
            },
            "mention_channels": {
                "display_name": "Channel Mentions",
                # array of channel mention objects
                "type": AbstractDataProviderTypeMap."*list",
                "desc": "Channels specifically mentioned in this message",
            },
            "attachments": {
                "display_name": "Attachments",
                # array of attachment objects
                "type": AbstractDataProviderTypeMap."*list",
                "desc": "Any attached files",
            },
            "embeds": {
                "display_name": "Embedded Content",
                # array of embed objects
                "type": AbstractDataProviderTypeMap."*list",
                "desc": "Any embedded content",
            },
            "reactions": {
                "display_name": "Reactions",
                # array of reaction objects
                "type": AbstractDataProviderTypeMap."*list",
                "desc": "Reactions to the message",
            },
            "nonce": {
                "display_name": "Nonce",
                # integer or string
                "type": AbstractDataProviderTypeMap."auto",
                "desc": "used for validating a message was sent",
            },
            "pinned": {
                "display_name": "Pinned?",
                "type": AbstractDataProviderTypeMap."bool",
                "desc": "Whether this message is pinned",
            },
            "webhook_id": {
                "display_name": "Webhook ID",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "If the message is generated by a webhook, this is the webhook's id",
            },
            "type": {
                "display_name": "Message Type",
                "type": AbstractDataProviderTypeMap."int",
                "desc": "Type of message",
            },
            "activity": {
                "display_name": "Activity",
                # message activity object
                "type": AbstractDataProviderTypeMap."*hash",
                "desc": "Sent with Rich Presence-related chat embeds",
            },
            "application": {
                "display_name": "Application",
                # partial application object
                "type": AbstractDataProviderTypeMap."*hash",
                "desc": "Sent with Rich Presence-related chat embeds",
            },
            "application_id": {
                "display_name": "Application ID",
                "type": AbstractDataProviderTypeMap."*string",
                "desc": "If the message is an Interaction or application-owned webhook, this is the id of the "
                    "application",
            },
            "message_reference": {
                "display_name": "Message Reference",
                # message reference object
                "type": AbstractDataProviderTypeMap."*hash",
                "desc": "Data showing the source of a crosspost, channel follow add, pin, or reply message",
            },
            "flags": {
                "display_name": "Message Flags",
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "Message flags combined as a bitfield",
            },
            "referenced_message": {
                "display_name": "Referenced Message",
                # message object
                "type": AbstractDataProviderTypeMap."*hash",
                "desc": "The message associated with the message_reference",
            },
            "interaction": {
                "display_name": "Interaction",
                # message interaction object
                "type": AbstractDataProviderTypeMap."*hash",
                "desc": "Sent if the message is a response to an Interaction",
            },
            "thread": {
                "display_name": "Thread",
                # channel object
                "type": AbstractDataProviderTypeMap."*hash",
                "desc": "The thread that was started from this message, includes thread member object",
            },
            "components": {
                "display_name": "Components",
                # array of message components
                "type": AbstractDataProviderTypeMap."*list",
                "desc": "Sent if the message contains components like buttons, action rows, or other interactive "
                    "components",
            },
            "sticker_items": {
                "display_name": "Stickers",
                # array of message sticker item objects
                "type": AbstractDataProviderTypeMap."*list",
                "desc": "Sent if the message contains stickers",
            },
            "stickers": {
                "display_name": "Stickers (Deprecated)",
                # array of sticker objects
                "type": AbstractDataProviderTypeMap."*list",
                "desc": "Deprecated: the stickers sent with the message",
            },
            "position": {
                "display_name": "Thread Position",
                "short_desc": "An integer representing the approximate position of the message in a thread",
                "type": AbstractDataProviderTypeMap."*int",
                "desc": "A generally increasing integer (there may be gaps or duplicates) that represents the "
                    "approximate position of the message in a thread, it can be used to estimate the relative "
                    "position of the message in a thread in company with `total_message_sent` on parent thread",
            },
            "role_subscription_data": {
                "display_name": "Role Subscription Data",
                "short_desc": "Data of the role subscription purchase or renewal",
                # role subscription data object
                "type": AbstractDataProviderTypeMap."*hash",
                "desc": "Data of the role subscription purchase or renewal that prompted this "
                    "`ROLE_SUBSCRIPTION_PURCHASE` message",
            },
            "resolved": {
                "display_name": "Resolved",
                "type": AbstractDataProviderTypeMap."*hash",
                "desc": "Data for users, members, channels, and roles in the message's auto-populated select menus",
            },
        };
    }

    #! Creates the object
    constructor() {
        addQoreFields(Fields);
    }
}

#! Discord message data type
public const DiscordMessageDataType = new DiscordMessageDataType();
}
