# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file DataProviderActionCatalog.qc provides the DataProvider catalog API

/** DataProviderActionCatalog.qc Copyright 2019 - 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Application information hash
/** Note that action paths are relative from the data provider created from the factory and the connection if the
    \c scheme attribute is also defined
*/
public hashdecl DataProviderAppInfo {
    #! Unique application name
    string name;

    #! Description of the application with markdown formatting
    string desc;

    #! Connection scheme for the application, if any
    /** This is the connection scheme for the \c ConnectionProvider module and indicates that the given connection
        type produces a data provider which will be the base data provider for application actions

        @note The data provider created by the connection must be the same as the one created by the factory
    */
    *string scheme;

    #! Indicates if the application supports the OAuth2 authorization code flow
    /** This means that the \c scheme attribute must be defined and the connection configuration supports the flow

        @note The OAuth2 client ID and client secret must be provided externally to use the authorization code flow
    */
    bool oauth2_auth_code = False;

    #! Name of the factory that creates the data provider
    /** This produces a data provider which will be the base data provider for application actions

        @note The data provider created by the factory must be the same as the one created by the connection specified
        by the \c scheme attribute
    */
    string factory;
}

/** @defgroup dp_action_type DataProvider Action Type Codes

    Each action implements only one type, so these codes do not make up a bitfield
*/
#/@{
#! Data Provider Action Type: wait for an event
public const DPAT_EVENT = 1;

#! Data Provider Action Type: make an API call
public const DPAT_API = 2;

#! Data Provider Action Type: send a message
public const DPAT_SEND_MESSAGE = 3;

#! Data Provider Action Type: find all matching records
public const DPAT_FIND = 4;

#! Data Provider Action Type: find a single record
public const DPAT_FIND_SINGLE = 5;

#! Data Provider Action Type: create a record
public const DPAT_CREATE = 6;

#! Data Provider Action Type: upsert a record
public const DPAT_UPSERT = 7;

#! Data Provider Action Type: update a record
public const DPAT_UPDATE = 8;

#! Data Provider Action Type: delete a record
public const DPAT_DELETE = 9;
#/@}

#! Maps a @ref db_action_type "action type code" to a data provider attribute string
const ActionMap = {
    DPAT_EVENT: "supports_observable",
    DPAT_API: "supports_request",
    DPAT_SEND_MESSAGE: "supports_messages",
    DPAT_FIND: "supports_read",
    DPAT_FIND_SINGLE: "supports_read",
    DPAT_CREATE: "supports_create",
    DPAT_UPSERT: "supports_upsert",
    DPAT_UPDATE: "supports_update",
    DPAT_DELETE: "supports_delete",
};

#! DataProvider action definition
public hashdecl DataProviderActionInfo {
    #! Name of the application that provides the action
    string app;

    #! The unique name for the action; unique in the application
    string action;

    #! Data provider path to action
    /** The path is relative from the data provider provided by the application
    */
    string path;

    #! Description of any path variables
    /** Keys are path variables, values are string descriptions with markdown formatting
    */
    *hash<string, string> path_vars;

    #! Action short description, plain text formatting
    string short_desc;

    #! Action long description, markdown formatting
    string desc;

    #! Action type code
    /** @see @ref dp_action_type for valid values
    */
    int action_code;
}

#! Data provider action flags
const DataProviderActionFlags = (
    "supports_request",
    "supports_observable",
    "supports_messages",
    "supports_read",
    "supports_create",
    "supports_update",
    "supports_upsert",
    "supports_delete",
);

#! DataProvider action and application catalog class
public class DataProviderActionCatalog {
    public {
        #! Required application key set
        const RequiredAppKeys = (
            "name": True,
            "desc": True,
            "factory": True,
        );

        #! Required action key set (without action_code and path_vars, which are checked separately)
        const RequiredActionKeys = (
            "app": True,
            "action": True,
            "path": True,
            "short_desc": True,
            "desc": True,
        );
    }

    private {
        #! Mutex for atomic operations
        static Mutex lck();

        #! Top-level master application cache keyed by application name
        static hash<string, hash<DataProviderAppInfo>> appcache;

        #! Master action cache, the key <app>/<action>
        static hash<string, hash<DataProviderActionInfo>> top_actionmap;

        #! Action cache, the key is the application key, the second key is the action string
        static hash<string, hash<string, hash<DataProviderActionInfo>>> app_actionmap;

        #! Scheme map; keys are connection schemes
        static hash<string, list<hash<DataProviderActionInfo>>> smap;

        #! Factory map; keys are factory names
        static hash<string, list<hash<DataProviderActionInfo>>> fmap;
    }

    #! Registers a data provider application
    /** @param app the application to register

        @throw APP-ERROR missing or invalid attributes, app already registered
    */
    static registerApp(hash<DataProviderAppInfo> app) {
        # check for required keys
        if (*hash<auto> missing = RequiredAppKeys - keys app) {
            throw "APP-ERROR", sprintf("application is missing the following required keys: %y (app: %y)",
                keys missing, app);
        }

        AutoLock al(lck);
        if (*hash<DataProviderAppInfo> existing_app = appcache{app.name}) {
            throw "APP-ERROR", sprintf("application %y already registered: %y", app.name, existing_app);
        }
        # add to cache
        appcache{app.name} = app;

        # deregister the app automatically if module initialization fails
        DataProviderImplicitModuleTransaction::add(sub () { DataProviderActionCatalog::deregisterApp(app.name); });
    }

    #! Registers a data provider action
    /** @param action the action to register

        @throw ACTION-ERROR missing or invalid attributes, action already registered

        The \c app and \c action attributes must be unique for every action registered
    */
    static registerAction(hash<DataProviderActionInfo> action) {
        # check for required keys
        if (*hash<auto> missing = RequiredActionKeys - keys action) {
            throw "ACTION-ERROR", sprintf("action is missing the following required keys: %y (action: %y)",
                keys missing, action);
        }
        if (action.action_code < 1 || action.action_code > DPAT_DELETE) {
            throw "ACTION-ERROR", sprintf("invalid action code %y in action: %y", action.action_code, action);
        }
        action.path = DataProviderActionCatalog::normalizePath(action.path);

        # check that all path variables are defined
        if (action.path_vars) {
            softlist<string> vars = (action.path =~ x/\{([^\}]*)}/g);
            if (*hash<string, string> pvc = (action.path_vars - vars)) {
                throw "ACTION-ERROR", sprintf("action has definitions for nonexistent path variables: %y", keys pvc);
            }
            if (vars) {
                hash<string, bool> vmap = map {$1: True}, vars;
                if (*hash<string, bool> ev = vmap - keys action.path_vars) {
                    throw "ACTION-ERROR", sprintf("action is missing definitions for path variables: %y", keys ev);
                }
            }
        } else {
            if (*string var = (action.path =~ x/\{([^\}]*)}/[0])) {
                throw "ACTION-ERROR", sprintf("action has path variables without definitions; path: %y", action.path);
            }
        }

        string key = sprintf("%s/%s", action.app, action.action);

        AutoLock al(lck);
        # check that the application exists
        *hash<DataProviderAppInfo> app = appcache{action.app};
        if (!app) {
            throw "ACTION-ERROR", sprintf("application %y not registered (action %y)", action.app, action);
        }
        if (*hash<DataProviderActionInfo> act = app_actionmap{action.app}{action.action}) {
            throw "ACTION-ERROR", sprintf("action with application %y, action %y already registered: %y", act.app,
                act.action, act);
        }
        # add to caches
        top_actionmap{key} = action;
        app_actionmap{action.app}{action.action} = action;
        # add to scheme map
        if (app.scheme) {
            if (!smap{app.scheme}) {
                smap{app.scheme} = ();
            }
            smap{app.scheme} += action;
        }
        # add to factory map
        if (!fmap{app.factory}) {
            fmap{app.factory} = ();
        }
        fmap{app.factory} += action;
    }

    #! Returns the application definition for the given application name
    /** @param app the app name

        @return the application definition for the given application name or @ref nothing if not present

        @see getAppEx()
    */
    static *hash<DataProviderAppInfo> getApp(string app) {
        return appcache{app};
    }

    #! Returns the application definition for the given application name
    /** @param app the app name

        @return the application definition for the given application name; throws an exception if the app is not
        registered

        @throw APP-ERROR the given application is not registered

        @see getApp()
    */
    static hash<DataProviderAppInfo> getAppEx(string app) {
        if (*hash<DataProviderAppInfo> rv = appcache{app}) {
            return rv;
        }
        throw "APP-ERROR", sprintf("application %y is not registered", app);
    }

    #! Returns all actions for a particular connection scheme
    /** @param scheme the connection scheme

        @return a list of actions associated with the given scheme
    */
    static *list<hash<DataProviderActionInfo>> getActionsForScheme(string scheme) {
        return smap{scheme};
    }

    #! Returns all actions for a particular data provider factory
    /** @param factory the data provider factory

        @return a list of actions associated with the given data provider factory
    */
    static *list<hash<DataProviderActionInfo>> getActionsForFactory(string factory) {
        return fmap{factory};
    }

    #! Returns all actions for the given application
    /** @param app the data provider application

        @return a list of actions for the given application

        @see getActionsEx()
    */
    static *list<hash<DataProviderActionInfo>> getActions(string app) {
        return app_actionmap{app}.values();
    }

    #! Returns all actions for the given application; throws an exception if the app is not registered
    /** @param app the data provider application

        @return a list of actions for the given application

        @throw APP-ERROR the given application is not registered

        @see getActions()
    */
    static list<hash<DataProviderActionInfo>> getActionsEx(string app) {
        if (*list<hash<DataProviderActionInfo>> rv = app_actionmap{app}.values()) {
            return rv;
        }
        throw "APP-ERROR", sprintf("application %y is not registered", app);
    }

    #! Returns all actions in the cache
    /** @return all actions in the cache
    */
    static *list<hash<DataProviderActionInfo>> getAll() {
        return top_actionmap.values();
    }

    #! Searches for actions whose descriptions match a regular expression pattern
    /** @param regex_pattern A regular expression pattern to match in the \c short_desc and \c desc fields
        @param regex_options Regular expression options; a case-insensitive match is performed by default

        @return All actions that match the regular expression pattern in the \c short_desc or \c desc fields
    */
    static *list<hash<DataProviderActionInfo>> searchRegex(string regex_pattern, int regex_options = RE_Caseless) {
        return map $1, top_actionmap.values(),
            regex($1.short_desc, regex_pattern, regex_options) || regex($1.desc, regex_pattern, regex_options);
    }

    #! Searches for actions whose descriptions match a substring
    /** @param substr A substring to match in the \c short_desc and \c desc fields
        @param ignore_case Ignore case when searching

        @return All actions that match the substring argument in the \c short_desc or \c desc fields
    */
    static *list<hash<DataProviderActionInfo>> searchSubstr(string substr, bool ignore_case = True) {
        if (ignore_case) {
            substr = substr.upr();
            return map $1, top_actionmap.values(),
                ($1.short_desc.upr().find(substr) >= 0) || ($1.desc.upr().find(substr) >= 0);
        }
        return map $1, top_actionmap.values(), ($1.short_desc.find(substr) >= 0) || ($1.desc.find(substr) >= 0);
    }

    #! Returns a normalized path to ensure that comparisons always work
    private static string normalizePath(string path) {
        # ensure that "/" characters are never duplicated
        if (path =~ /\/\//) {
            throw "ACTION-ERROR", sprintf("%y: illegal consecutive \"/\" characters", path);
        }
        # remove any leading or trailing "/" chars
        path =~ s/^\/?(.*[^\/])\/?/$1/;
        if (!path.val()) {
            throw "ACTION-ERROR", sprintf("empty action path provided", path);
        }
        return path;
    }

    #! Deregisters an app and all its actions if an exception occurs during the other module's initialization
    private static deregisterApp(string name) {
        AutoLock al(lck);

        *hash<DataProviderAppInfo> app = appcache{name};
        if (!app) {
            return;
        }

        if (app.scheme) {
            remove smap{app.scheme};
        }
        remove fmap{app.factory};
        map remove top_actionmap{name + "/" + $1.action}, app_actionmap{name}.values();
        remove app_actionmap{name};
    }
}
}
