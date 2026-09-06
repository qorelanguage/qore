# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore RestSchemaActions module definition

/*  RestSchemaActionSetDataProvider.qc Copyright (C) 2026 Qore Technologies, s.r.o.

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

#! contains all public definitions in the RestSchemaActions module
public namespace RestSchemaActions {
#! Presents an action set's actions as child data providers
/** Every action definition names a data provider path relative to the application's root provider, so the
    root has to resolve each action ID to a provider that can execute it.  This class does that, and is
    intended to be inherited by an application's root data provider.

    @par Example:
    @code{.py}
public class MyAppDataProvider inherits RestSchemaActionSetDataProvider {
    constructor(*AbstractRestClient rest) : RestSchemaActionSetDataProvider(MyApp::ActionSet, rest) {
    }

    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
    @endcode
*/
public class RestSchemaActionSetDataProvider inherits AbstractDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "RestSchemaActionSetDataProvider",
            "supports_children": True,
            "children_can_support_apis": True,
        };
    }

    private {
        #! The action set exposed as children
        RestSchemaActionSet action_set;

        #! The REST client used for API calls
        *AbstractRestClient rest;

        #! Shared by every action under this root, so a resolved host is looked up once for all of them
        /** Only created when the application actually has a host source and a client to copy from; every
            other application - which is nearly all of them - carries nothing extra for this.
        */
        *RestSchemaHostRouter router;
    }

    #! Creates the object from the arguments
    /** @param action_set the action set to expose
        @param rest the REST client to use for API calls
        @param router an existing host router to share; give one whose lifetime is the client's to keep a
        resolved host cached across data provider instances, which is what an application whose data plane
        is sharded per object wants.  A router of this root's own is created when none is given.
    */
    constructor(RestSchemaActionSet action_set, *AbstractRestClient rest, *RestSchemaHostRouter router) {
        self.action_set = action_set;
        self.rest = rest;
        if (router) {
            self.router = router;
        } else if (rest && action_set.getAllHostSourceDecls()) {
            self.router = new RestSchemaHostRouter(rest);
        }
    }

    #! Returns the host router shared by this root's actions, if any
    *RestSchemaHostRouter getHostRouter() {
        return router;
    }

    #! Returns the data provider name
    string getName() {
        return "rest-schema-actions";
    }

    #! Returns the data provider description
    *string getDesc() {
        return sprintf("Data provider exposing %d schema-derived action(s)", action_set.getActionNames().size());
    }

    #! Returns the action set exposed by this provider
    RestSchemaActionSet getActionSet() {
        return action_set;
    }

    #! Returns the action IDs
    private *list<string> getChildProviderNamesImpl() {
        return action_set.getActionNames();
    }

    #! Returns the provider for the given action, or @ref nothing if the action is unknown
    private *AbstractDataProvider getChildProviderImpl(string name) {
        *RestSchemaActionOperation op = action_set.getOperation(name);
        if (!op) {
            return;
        }
        return new RestSchemaActionDataProvider(action_set, op, rest, router);
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}
