# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleDataProvider module definition

/** GoogleApiParentDataProvider.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the GoogleDataProvider module
public namespace GoogleDataProvider {
#! The Google data provider base class
public class GoogleApiParentDataProvider inherits GoogleDataProviderBase {
    private {
        #! The Google API name (ex: "calendar")
        string api;

        #! The resource name
        string resource;

        #! Data provider description
        string desc;

        #! Data provider type name
        string type;

        #! Any path args associated with this data provider
        *hash<auto> path_args;

        #! Method hash
        hash<string, bool> methods;

        #! Path groups under this parent
        *hash<string, bool> subpaths;
    }

    #! Creates the object
    constructor(GoogleRestClient::GoogleRestClient rest, string api, string resource, *hash<auto> path_args, ...)
            : GoogleDataProviderBase(rest) {
        self.api = api;
        self.resource = resource;
        self.path_args = path_args;
        subpaths = map {$1: True}, argv;
        desc = sprintf("Google %s %s provider for `%s`", api, resource, rest.getSafeURL());
        type = sprintf("Google%s%sDataProvider", cap(api), cap(resource));
        methods = getMethodsForResource(api, resource, path_args);
    }

    #! Creates the object
    constructor(string api, string resource, *hash<auto> path_args, ...) {
        self.api = api;
        self.resource = resource;
        self.path_args = path_args;
        subpaths = map {$1: True}, argv;
        desc = sprintf("Google %s %s provider", api, resource);
        type = sprintf("Google%s%sDataProvider", cap(api), cap(resource));
        methods = getMethodsForResource(api, resource, path_args);
    }

    #! Returns the data provider name
    string getName() {
        return resource;
    }

    #! Returns the data provider description
    *string getDesc() {
        return desc;
    }

    #! Return data provider summary info
    *list<hash<DataProvider::DataProviderSummaryInfo>> getChildProviderSummaryInfo() {
        string api = cap(self.api);
        string resource = cap(self.resource);

        *list<hash<DataProvider::DataProviderSummaryInfo>> rv = map <DataProviderSummaryInfo>{
            "name": $1,
            "desc": sprintf("Google %s events/%s provider for `%s`", self.api, $1, rest.getSafeURL()),
            "type": sprintf("Google%s%s%sDataProvider", api, resource, cap($1)),
            "supports_request": True,
        }, keys methods;
        if (subpaths) {
            rv += map GoogleApiParentDataProvider::getSummaryInfo(self.api, $1), keys subpaths;
        }
        return rv;
    }

    static hash<DataProvider::DataProviderSummaryInfo> getSummaryInfo(string api, string resource) {
        string desc = sprintf("Google %s %s provider", api, resource);
        string type = sprintf("Google%s%sDataProvider", GoogleDataProviderBase::cap(api),
            GoogleDataProviderBase::cap(resource));
        return <DataProviderSummaryInfo>{
            "name": resource,
            "desc": desc,
            "type": type,
            "supports_children": True,
            "children_can_support_apis": True,
        };
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        *list<string> rv = keys methods;
        if (subpaths) {
            rv += keys subpaths;
        }
        return rv;
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @see getChildProviderEx()
    */
    private *GoogleDataProvider::GoogleDataProviderBase getChildProviderImpl(string name) {
        if (methods{name}) {
            return new GoogleApiDataProvider(rest, api, resource + "/" + name, path_args);
        }
        if (subpaths{name}) {
            return new GoogleApiParentDataProvider(rest, api, name, path_args);
        }
    }

    #! Returns data provider static info
    private hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return <DataProviderInfo>{
            "name": resource,
            "desc": desc,
            "type": type,
            "supports_children": True,
            "children_can_support_apis": True,
        };
    }
}
}