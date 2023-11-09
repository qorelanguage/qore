# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleCalendarDataProvider module definition

/** GoogleCalendarDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the GoogleCalendarDataProvider module
public namespace GoogleCalendarDataProvider {
#! The parent class for Google calendar REST APIs
public class GoogleCalendarDataProvider inherits GoogleDataProvider::GoogleDataProviderBase {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "name": "calendar",
            "desc": "Google calendar data provider; parent provider for APIs related to calendars",
            "type": "GoogleCalendarDataProvider",
            "constructor_options": GoogleCalendarBaseDataProvider::ConstructorOptions + {
                "id": <DataProviderOptionInfo>{
                    "display_name": "Calendar ID",
                    "short_desc": "The calendar ID",
                    "type": AbstractDataProviderTypeMap."string",
                    "desc": "The calendar ID",
                },
            },
            "supports_children": True,
            "children_can_support_apis": True,
        };

        #! Provider summary info
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });
    }

    private {
        #! The calendar's ID
        string id;

        #! The calendar's metadata
        *hash<auto> cal;

        const DefaultChildMap = {
            "delete": Class::forName("GoogleCalendarDataProvider::GoogleCalendarDeleteDataProvider"),
            "events": Class::forName("GoogleCalendarDataProvider::GoogleCalendarEventsDataProvider"),
            "freeBusy": Class::forName("GoogleCalendarDataProvider::GoogleCalendarFreeBusyDataProvider"),
            "get": Class::forName("GoogleCalendarDataProvider::GoogleCalendarGetDataProvider"),
            "patch": Class::forName("GoogleCalendarDataProvider::GoogleCalendarPatchDataProvider"),
            "update": Class::forName("GoogleCalendarDataProvider::GoogleCalendarUpdateDataProvider"),
        };

        static hash<string, Reflection::Class> childMap = DefaultChildMap;
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        checkOptions("CONSTRUCTOR-ERROR", GoogleCalendarBaseDataProvider::ConstructorOptions, options);
        rest = GoogleDataProviderBase::getRestConnection(options);
        id = options.id;
    }

    #! Creates the object from a REST connection
    constructor(GoogleRestClient::GoogleRestClient rest, string id, *hash<auto> cal)
            : GoogleDataProvider::GoogleDataProviderBase(rest) {
        self.id = id;
        self.cal = cal;
    }

    #! Returns the data provider name
    string getName() {
        return ProviderInfo.name;
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("Google data %s provider for `%s` for `%s`", ProviderInfo.name, id, rest.getSafeURL());
    }

    #! Return data provider summary info
    *list<hash<DataProvider::DataProviderSummaryInfo>> getChildProviderSummaryInfo() {
        return map $1.getConstant("ProviderSummaryInfo").getValue(), childMap.iterator();
    }

    #! Register a new child data provider for Google calendars
    static registerChild(string pathname, Reflection::Class cls) {
        if (childMap{pathname}) {
            throw "GOOGLE-CALENDAR-CHILD-ERROR", sprintf("child %y already registered", pathname);
        }

        childMap{pathname} = cls;

        ImplicitModuleTransaction::add(sub () { GoogleCalendarDataProvider::deregisterChild(pathname); });
    }

    #! Deregister the named child data provider
    private static deregisterChild(string pathname) {
        if (!DefaultChildMap{pathname}) {
            remove childMap{pathname};
        }
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        return keys childMap;
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @see getChildProviderEx()
    */
    private *GoogleDataProvider::GoogleDataProviderBase getChildProviderImpl(string name) {
        *Class cls = childMap{name};
        if (!cls) {
            return;
        }
        return cls.newObject(rest, id, cal);
    }

    #! Returns data provider static info
    hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}
