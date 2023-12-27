# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GmailDataProvider module definition

/** GmailMessageWatchDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the GmailDataProvider module
public namespace GmailDataProvider {
#! Gmail email matched event
public const EVENT_EMAIL_MATCHED = "EMAIL-MATCHED";

#! The parent class for Gmail REST APIs
public class GmailMessageWatchDataProvider inherits GoogleDataProvider::GoogleDataProviderBase,
        DataProvider::DelayedObservable {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "watch",
            "desc": "Gmail message watch data provider",
            "type": "GmailMessageWatchDataProvider",
            "constructor_options": ConstructorOptions,
            "supports_observable": True,
        };

        const ConstructorOptions = {
            "q": <DataProviderOptionInfo>{
                "display_name": "Search Criteria",
                "short_desc": "The gmail search criteria for matching messages",
                "desc": "The gmail search criteria for matching messages; ex: `in:inbox to:me@gmail.com "
                    "subject:hello is:unread`",
                "type": (AbstractDataProviderTypeMap."string",),
                "example_value": "in:inbox to:me@gmail.com subject:hello is:unread",
                "required": True,
            },
            "poll_interval": <DataProviderOptionInfo>{
                "display_name": "Poll Interval",
                "short_desc": "How often to check for matching emails in seconds",
                "type": (AbstractDataProviderTypeMap."int",),
                "desc": "How often to check for matching emails in seconds; minimum 30; lower numbers will set the "
                    "minimum polling period",
                "default_value": 120,
            },
            "retrieve_attachments": <DataProviderOptionInfo>{
                "display_name": "Retrieve Attachments?",
                "short_desc": "Retrieve attachment data automatically?",
                "type": (AbstractDataProviderTypeMap."bool",),
                "desc": "Retrieve attachment data automatically?",
            },
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });

        #! Minimum poll interval in seconds
        const MinPollInterval = 30;
    }

    private {
        # I/O lock
        Mutex lck();

        #! I/O Condition
        Condition cond();

        #! Number of threads waiting on the I/O condition
        int waiting;

        #! I/O TID
        int io_tid;

        #! I/O stop flag
        bool stop;

        #! Poll interval in seconds
        int poll_secs;

        #! Retrieve attachments
        bool retrieve_attachments;

        #! Delete emails after retreiving
        bool delete_messages;

        #! Start date for scan
        date start_date;

        #! Match query
        string q;
    }

    #! Creates the object from a REST connection
    constructor(GoogleRestClient::GoogleRestClient rest, *hash<auto> options) : GoogleDataProviderBase(rest) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, ImpliedArgumentHelper::get()
            + options);
        if (copts.poll_interval) {
            poll_secs = copts.poll_interval;
            if (poll_secs < MinPollInterval) {
                poll_secs = MinPollInterval;
            }
        }
        if (copts.retrieve_attachments) {
            retrieve_attachments = True;
        }
        if (copts.delete_messages) {
            delete_messages = True;
        }
        if (copts.start_date) {
            start_date = copts.start_date;
        }

        q = copts.q;
    }

    #! Stops observing and deletes the object
    destructor() {
        {
            lck.lock();
            on_exit lck.unlock();

            if (io_tid) {
                # set the stop flag
                stop = True;
                # signal the I/O thread to wake up
                cond.signal();
                # wait for the thread to terminate
                while (io_tid) {
                    cond.wait(lck);
                }
            }
        }
    }

    #! Returns the data provider name
    string getName() {
        return ProviderInfo.name;
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("Google data %s provider for `%s`", ProviderInfo.name, rest.getSafeURL());
    }

    #! Returns a hash of all supported event types
    /** @return a hash of all supported event types

        @note only called if the provider supports the observer pattern / event API
    */
    private hash<string, hash<DataProviderMessageInfo>> getEventTypesImpl() {
        return {
            EVENT_EMAIL_MATCHED: <DataProviderMessageInfo>{
                "desc": "A email has been matched",
                "type": GmailMessageEventDataType,
            },
        };
    }

    #! Called when all observers have been added to the object
    /** This method is meant to trigger event generation
    */
    observersReady() {
        lck.lock();
        on_exit lck.unlock();

        # I/O thread already running
        if (io_tid) {
            return;
        }

        # start the I/O thread
        io_tid = background pollThread();
    }

    #! Returns data provider static info
    private hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }

    #! I/O polling thread
    private pollThread() {
        on_exit {
            lck.lock();
            on_exit lck.unlock();

            remove io_tid;

            cond.broadcast();
        }

        string uri = "gmail/v1/users/me/messages?q=" + q;

        int epoch;
        if (start_date) {
            epoch = mktime(start_date);
        }

        AbstractDataProviderType msgtype = GoogleDataProviderBase::getTypeForSchema(GoogleDiscoveryGmailApiName,
            "Message");

        while (!stop) {
            # check for matching mail
            hash<auto> info;
            try {
                hash<auto> res;
                string this_uri = uri;
                if (epoch) {
                    this_uri += sprintf(" after:%d", epoch);
                }
                LoggerWrapper::info("using URI: %y epoch: %y", this_uri, epoch);
                res = rest.get(this_uri, NOTHING, \info).body;
                foreach hash<auto> minfo in (res.messages) {
                    # retrieve each email
                    string msguri = sprintf("gmail/v1/users/me/messages/get?messageId=%s&id=%s", minfo.messageId,
                        minfo.id);
                    hash<auto> msg = msgtype.acceptsValue(rest.get(msguri, NOTHING, \info).body);
                    if (msg.internalDate > epoch) {
                        epoch = msg.internalDate;
                    }
                    if (retrieve_attachments) {
                        msg.attachments = ();
                        foreach hash<auto> part in (msg.payload.parts) {
                            if (*string aid = part.body.attachmentId) {
                                # parse headers
                                *hash<auto> hdr = map {$1.name: $1.value}, part.headers;
                                # retrieve attachment
                                string atturi = sprintf("gmail/v1/users/me/messages/%s/attachments/%s",
                                    minfo.messageId, aid);
                                hash<auto> att = rest.get(atturi, NOTHING, \info).body;
                                data body;
                                if (hdr."Content-Type" =~ /text\//) {
                                    body = parse_base64_url_string_to_string(att."data");
                                } else {
                                    body = parse_base64_url_string(att."data");
                                }
                                msg.attachments += {
                                    "hdr": hdr,
                                    "body": body,
                                };
                            }
                        }
                    }
                    notifyObservers(EVENT_EMAIL_MATCHED, msg);
                    if (delete_messages) {
                        string deluri = sprintf("/gmail/v1/users/me/messages/%s", minfo.messageId);
                        rest.del(deluri, NOTHING, \info);
                        LoggerWrapper::info("deleted message %y", minfo.messageId);
                    }
                }
                LoggerWrapper::info("new epoch: %y", epoch);
            } catch (hash<ExceptionInfo> ex) {
                LoggerWrapper::error("error retrieving emails: %s; polling again in %ds", get_exception_string(ex),
                    poll_secs);
            }

            # wait until next poll
            lck.lock();
            on_exit lck.unlock();
            cond.wait(lck, poll_secs * 1000);
        }
    }
}

public const GmailMessageEventDataType = new GmailMessageEventDataType();

public class GmailMessageEventDataType inherits HashDataType {
    public {
        const AttachmentFields = {
            "hdr": {
                "display_name": "Headers",
                "short_desc": "Attachment headers",
                "desc": "Attachment headers",
                "type": AbstractDataProviderTypeMap."hash",
            },
            "body": {
                "display_name": "Attachment Body",
                "short_desc": "Attachment body",
                "desc": "Attachment body",
                "type": AbstractDataProviderTypeMap."data",
            },
        };
    }
    constructor() : HashDataType("GmailWatchMessage") {
        AbstractDataProviderType t = GoogleDataProviderBase::getTypeForSchema(GoogleDiscoveryGmailApiName, "Message");
        map addField($1), t.getFields().iterator();
        HashDataType attachment("GmailAttachment");
        attachment.addQoreFields(AttachmentFields);
        addField(new QoreDataField({
            "name": "attachments",
            "display_name": "Attachments",
            "short_desc": "Message attachments",
            "desc": "Message attachments",
            "type": new ListDataType("AttachmentList", attachment, True),
        }));
    }
}
}
