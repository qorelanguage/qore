# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GmailDataProvider module definition

/** GmailMessageWatchDataProviderBase.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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
#! The parent class for Gmail REST APIs
public class GmailMessageWatchDataProviderBase inherits GoogleDataProvider::GoogleDataProviderBase,
        DataProvider::DelayedObservable {
    public {
        #! Constructor options
        const ConstructorOptions = {
            "delete_messages": <DataProviderOptionInfo>{
                "display_name": "Delete Messages?",
                "short_desc": "Delete messages matched?",
                "type": (AbstractDataProviderTypeMap."bool",),
                "desc": "Delete messages matched?",
            },
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
                "desc": "How often to check for matching emails in seconds; lower bound 30; lower numbers will set "
                    "the minimum polling period",
                "default_value": 120,
            },
            "retrieve_attachments": <DataProviderOptionInfo>{
                "display_name": "Retrieve Attachments?",
                "short_desc": "Retrieve attachment data automatically?",
                "type": (AbstractDataProviderTypeMap."bool",),
                "desc": "Retrieve attachment data automatically?",
            },
            "start_date": <DataProviderOptionInfo>{
                "display_name": "Start Date",
                "short_desc": "Do not retrieve emails earlier than the given date",
                "type": (AbstractDataProviderTypeMap."date",),
                "desc": "Do not retrieve emails earlier than the given date",
            },
        };

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
    constructor(GoogleRestClient::GoogleRestClient rest,
            hash<string, hash<DataProviderOptionInfo>> constructor_options, *hash<auto> options)
            : GoogleDataProviderBase(rest) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", constructor_options,
            ImpliedArgumentHelper::get(){keys constructor_options} + options);
        processOptions(copts);
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

    #! Process constructor options
    private processOptions(*hash<auto> copts) {
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

    #! I/O polling thread
    private pollThread() {
        on_exit {
            lck.lock();
            on_exit lck.unlock();

            remove io_tid;

            cond.broadcast();
        }

        hash<auto> info = setup();

        while (!stop) {
            pollOnce(\info);

            # wait until next poll
            lck.lock();
            on_exit lck.unlock();
            cond.wait(lck, poll_secs * 1000);
        }
    }

    #! Setup for a poll operation
    private hash<auto> setup() {
        string uri = "gmail/v1/users/me/messages?q=" + q;

        int epoch;
        if (start_date) {
            epoch = mktime(start_date);
        }

        AbstractDataProviderType msgtype = GoogleDataProviderBase::getTypeForSchema(GoogleDiscoveryGmailApiName,
            "Message");

        return {
            "uri": uri,
            "epoch": epoch,
            "msgtype": msgtype,
        };
    }

    #! Perform one poll
    private *hash<auto> pollOnce(reference<hash<auto>> setup) {
        # check for matching mail
        hash<auto> info;
        try {
            hash<auto> res;
            string this_uri = setup.uri;
            if (setup.epoch) {
                this_uri += sprintf(" after:%d", setup.epoch);
            }
            bool epoch_updated;
            LoggerWrapper::debug("Gmail polling using URI: %y epoch: %y poll_interval: %ds", this_uri, setup.epoch,
                poll_secs);
            res = rest.get(this_uri, NOTHING, \info).body;
            int match;
            foreach hash<auto> minfo in (res.messages) {
                # skip messages already processed
                if (setup.last_msg{minfo.id}) {
                    continue;
                }
                # retrieve each email
                string msguri = sprintf("gmail/v1/users/me/messages/%s", minfo.id);
                hash<auto> msg = setup.msgtype.acceptsValue(rest.get(msguri, NOTHING, \info).body);
                # internalDate is given as a ms offset from the epoch
                int msg_epoch = msg.internalDate / 1000;
                if (msg_epoch > setup.epoch) {
                    # remove last msg hash
                    setup.last_msg = {
                        minfo.id: True,
                    };
                    setup.epoch = msg_epoch;
                    epoch_updated = True;
                } else if (msg_epoch == setup.epoch) {
                    setup.last_msg{minfo.id} = True;
                }
                if (retrieve_attachments) {
                    msg.attachments = ();
                    foreach hash<auto> part in (msg.payload.parts) {
                        if (*string aid = part.body.attachmentId) {
                            # parse headers
                            *hash<auto> hdr = map {$1.name: $1.value}, part.headers;
                            # retrieve attachment
                            string atturi = sprintf("gmail/v1/users/me/messages/%s/attachments/%s", minfo.id,
                                aid);
                            hash<auto> att = rest.get(atturi, NOTHING, \info).body;
                            msg.attachments += getAttachment(att, hdr);
                        }
                    }
                    LoggerWrapper::info("Gmail: matched message %y attachments: %d", minfo.id, msg.attachments.size());
                } else {
                    LoggerWrapper::info("Gmail: matched message %y", minfo.id);
                }

                if (setup.test_mode) {
                    return msg;
                }
                ++match;
                messageReceived(msg);
                if (delete_messages) {
                    string deluri = sprintf("/gmail/v1/users/me/messages/%s", minfo.id);
                    rest.del(deluri, NOTHING, \info);
                    LoggerWrapper::info("Gmail: deleted matched message %y", minfo.id);
                }
                if (epoch_updated) {
                    LoggerWrapper::debug("Gmail using new epoch: %y", setup.epoch);
                }
            }
            if (!match) {
                LoggerWrapper::debug("Gmail: matched no messages");
            }
        } catch (hash<ExceptionInfo> ex) {
            if (setup.test_mode) {
                LoggerWrapper::error("Gmail error retrieving emails in test mode: %s", get_exception_string(ex));
                return;
            }

            LoggerWrapper::error("Gmail error retrieving emails: %s; polling again in %ds", get_exception_string(ex),
                poll_secs);
        }
    }

    #! Returns attachment data
    private hash<auto> getAttachment(hash<auto> att, *hash<auto> hdr) {
        string filename;
        # try to get filename
        if (hdr."Content-Disposition" && (*string fn =
            (hdr."Content-Disposition" =~ x/filename="([^"]+)"/)[0])) {
            filename = fn;
        }
        # try to determine the content type
        *string content_type = hdr."Content-Type";
        *string encoding;
        data body;
        if (content_type.val()) {
            encoding = (content_type =~ x/charset="([^"]+)"/)[0];
            content_type =~ s/;.*//;
        } else if (filename.val()) {
            content_type = get_mime_type_from_ext(filename);
        }
        if (content_type.val() && is_mime_text(content_type)) {
            body = parse_base64_url_string_to_string(att."data", encoding);
        } else {
            body = parse_base64_url_string(att."data");
        }
        hash<auto> attachment = {
            "hdr": hdr,
            "body": body,
        };
        if (filename.val()) {
            attachment.filename = filename;
        }
        if (content_type) {
            attachment.content_type = content_type;
        }
        if (encoding) {
            attachment.encoding = encoding;
        }
        return attachment;
    }

    #! Raise events
    abstract private messageReceived(hash<auto> msg);
}
}
