# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore Pop3ClientListDataProvider class definition

/** Pop3ClientListDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the Pop3ClientDataProvider module
public namespace Pop3ClientDataProvider {
#! The POP3 message list parent data provider
public class Pop3ClientListDataProvider inherits Pop3ClientDataProviderBase {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "Pop3ClientListDataProvider",
            "constructor_options": ConstructorOptions,
            "supports_children": True,
            "children_can_support_apis": True,
        };
    }

    private {
        #! Message set
        *hash<string, bool> msgs;

        #! Metadata mutex
        Qore::Thread::Mutex lck();
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) : Pop3ClientDataProviderBase(options) {
    }

    #! Creates the object from an POP3 connection
    constructor(Pop3Client pop3) : Pop3ClientDataProviderBase(pop3) {
    }

    #! Returns the data provider name
    string getName() {
        return "list";
    }

    #! Returns the data provider description
    *string getDesc() {
        return "Parent data provider for POP3 messages";
    }

    #! Return data provider summary info
    *list<hash<DataProvider::DataProviderSummaryInfo>> getChildProviderSummaryInfo() {
        return map <DataProviderSummaryInfo>{
            "name": $1,
            "desc": sprintf("POP3 message %y", $1),
            "type": "Pop3ClientMessageDataProvider",
            "supports_children": True,
            "children_can_support_apis": True,
        }, getChildProviderNames();
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        return keys getAllMessages();
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @see getChildProviderEx()
    */
    private DataProvider::AbstractDataProvider getChildProviderImpl(string name) {
        if (getAllMessages(){name}) {
            return new Pop3ClientMessageDataProvider(pop3, name);
        }
    }

    #! Returns data provider static info
    hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }

    #! Returns the set of all messages
    private *hash<string, bool> getAllMessages() {
        if (exists msgs) {
            return msgs;
        }

        lck.lock();
        on_exit lck.unlock();

        # check again in the lock
        if (exists msgs) {
            return msgs;
        }

        return msgs = map {$1: True}, keys pop3.list();
    }
}
}