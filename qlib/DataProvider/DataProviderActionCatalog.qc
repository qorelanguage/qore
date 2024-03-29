# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file DataProviderActionCatalog.qc provides the DataProvider catalog API

/** DataProviderActionCatalog.qc Copyright 2019 - 2024 Qore Technologies, s.r.o.

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
/** Note that action paths are relative from the data provider created from the connection and optionally the subtype

    If both \c scheme and \c action.cls are missing, it implies that the data provider will be implicitly provided by
    the context where the action is used
*/
public hashdecl DataProviderAppInfo {
    #! Unique application name
    /** This technical name should have no spaces and be in camel case or all caps by convention
    */
    string name;

    #! The display name for the application
    string display_name;

    #! A short description for the app, plain text formatting
    string short_desc;

    #! Description of the application with markdown formatting
    string desc;

    #! Connection scheme for the application
    /** This is the connection scheme for the \c ConnectionProvider module and indicates that the given connection
        type produces a data provider which will be the base data provider for application actions
    */
    *string scheme;

    #! The application logo
    /** Must be an SVG or PNG; all logos must be square, and PNG must be 200x200 px
    */
    data logo;

    #! The logo file name
    string logo_file_name;

    #! The mime type for \c logo
    /** Must be one of:
        - SVG: \c image/svg+xml
        - PNG: \c image/png
    */
    string logo_mime_type;
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
public const ActionAttrMap = {
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

#! Maps a @ref db_action_type "action type code" to a user-friendly name
public const ActionNameMap = {
    DPAT_EVENT: "EVENT",
    DPAT_API: "API",
    DPAT_SEND_MESSAGE: "MESSAGE",
    DPAT_FIND: "FIND",
    DPAT_FIND_SINGLE: "FIND_SINGLE",
    DPAT_CREATE: "CREATE",
    DPAT_UPSERT: "UPSERT",
    DPAT_UPDATE: "UPDATE",
    DPAT_DELETE: "DELETE",
};

#! Data provider action option definition
public hashdecl ActionOptionInfo {
    #! Data type of the option
    string type;

    #! Display name of the option
    string display_name;

    #! Short description of the option in plain text
    string short_desc;

    #! Description of the option with markdown rendering
    string desc;

    #! Is the option required or not?
    bool required;

    #! Any default value for the option
    auto default_value;

    #! Is the option value sensitive?
    bool sensitive;

    #! List of allowed values (enum)
    *softlist<hash<AllowedValueInfo>> allowed_values;

    #! An optional example value for the option
    *auto example_value;

    #! If the option should be preselected
    bool preselected;

    #! Option location for record-based data providers and data providers provided by a class
    /** values:
        - \c where: in the where clause
        - \c options: in the options argument
        - \c set: in the set argument for update statements
        - \c constructor: option to be used in the class constructor
    */
    *string loc;
}

#! DataProvider path var info
public hashdecl DataProviderPathVarInfo {
    #! The display name for the action
    string display_name;

    #! Action short description, plain text formatting
    string short_desc;

    #! Action long description, markdown formatting
    string desc;

    #! An optional example value for the option
    *string example_value;
}

#! DataProvider action definition
public hashdecl DataProviderActionInfo {
    #! Name of the application that provides the action
    string app;

    #! The unique name for the action; unique in the application
    string action;

    #! Any data provider subtype
    /** This is the value that will be passed to \c AbstractConnection::getDataProvider(string subtype)
    */
    *string subtype;

    #! Data provider path to action
    /** The path is relative from the data provider provided by the application
    */
    string path;

    #! Description of any path variables
    /** Keys are path variables, values provide info about the path variable
    */
    *hash<string, hash<DataProviderPathVarInfo>> path_vars;

    #! The display name for the action
    string display_name;

    #! Action short description, plain text formatting
    string short_desc;

    #! Action long description, markdown formatting
    string desc;

    #! Action type code
    /** @see @ref dp_action_type for valid values
    */
    int action_code;

    #! Message or event type
    /** if \c action_code is:
        - @ref DPAT_EVENT: this is the event type
        - @ref DPAT_SEND_MESSAGE: this is the message type
    */
    *string action_val;

    #! The DataProvider class
    /** This is for the case when there is no application \c scheme and the application is not implied by the context.

        It is an error to supply both application \c scheme and \c cls.
    */
    *string cls;

    #! Values to add to any "where" argument for record-based actions
    *hash<auto> where_add;

    #! Values to add to any "options" argument for record-based actions
    *hash<auto> options_add;

    #! Values to add to any "set" argument for record-based actions
    *hash<auto> set_add;

    #! Convenience options that can be provided without a data provider
    /** For API calls and messages only
    */
    *hash<string, hash<ActionOptionInfo>> convenience_options;

    #! Advanced options that can be provided without a data provider
    /** For API calls and messages only
    */
    *hash<string, hash<ActionOptionInfo>> advanced_options;
}

#! DataProvider action and application catalog class
public class DataProviderActionCatalog {
    public {
        #! Required application key set
        const RequiredAppKeys = (
            "name": True,
            "display_name": True,
            "short_desc": True,
            "desc": True,
            "logo": True,
            "logo_file_name": True,
            "logo_mime_type": True,
        );

        #! Required action key set (without action_code and path_vars, which are checked separately)
        const RequiredActionKeys = (
            "app": True,
            "action": True,
            "path": True,
            "display_name": True,
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

        #! Default description
        const DefaultDescription = "no description provided";
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
            # ignore if the application has the same definition
            if (existing_app == app) {
                return;
            }
            throw "APP-ERROR", sprintf("application %y already registered: %y", app.name, existing_app);
        }
        # add to cache
        appcache{app.name} = app;

        # deregister the app automatically if module initialization fails
        ImplicitModuleTransaction::add(sub () { DataProviderActionCatalog::deregisterApp(app.name); });
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
            if (*hash<string, hash<DataProviderPathVarInfo>> pvc = (action.path_vars - vars)) {
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
        if (app.scheme && exists action.cls) {
            throw "ACTION-ERROR", sprintf("application %y has scheme %y and action has cls %y; it is not allowed to "
                "provide an action class when the application has a scheme", action.app, app.scheme, action.cls);
        }
        if (*hash<DataProviderActionInfo> act = app_actionmap{action.app}{action.action}) {
            # ignore duplicates to cover injection scenarios
            if (action == act) {
                return;
            }
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
    }

    #! Returns all applications in the cache
    /**
        @return all applications in the cache
    */
    static *list<hash<DataProviderAppInfo>> getAllApps() {
        return appcache.values();
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

    #! Searches for applications whose names or descriptions match a regular expression pattern
    /** @param regex_pattern A regular expression pattern to match in the \c name and \c desc fields
        @param regex_options Regular expression options; a case-insensitive match is performed by default

        @return All applications that match the regular expression pattern in the \c name or \c desc fields
    */
    static *list<hash<DataProviderAppInfo>> searchAppRegex(string regex_pattern, int regex_options = RE_Caseless) {
        return map $1, appcache.values(),
            regex($1.name, regex_pattern, regex_options) || regex($1.desc, regex_pattern, regex_options);
    }

    #! Searches for applications whose names or descriptions match a substring
    /** @param substr A substring to match in the \c name and \c desc fields
        @param ignore_case Ignore case when searching

        @return All applications that match the substring argument in the \c name or \c desc fields
    */
    static *list<hash<DataProviderAppInfo>> searchAppSubstr(string substr, bool ignore_case = True) {
        if (ignore_case) {
            substr = substr.upr();
            return map $1, appcache.values(),
                ($1.name.upr().find(substr) >= 0) || ($1.desc.upr().find(substr) >= 0);
        }
        return map $1, appcache.values(), ($1.name.find(substr) >= 0) || ($1.desc.find(substr) >= 0);
    }

    #! Returns all actions for a particular connection scheme
    /** @param scheme the connection scheme

        @return a list of actions associated with the given scheme
    */
    static *list<hash<DataProviderActionInfo>> getActionsForScheme(string scheme) {
        return smap{scheme};
    }

    #! Returns a hash of all actions for the given application
    /** @param app the data provider application

        @return a hash of actions keyed by action name for the given application

        @see getActionHashEx()
    */
    static *hash<string, hash<DataProviderActionInfo>> getActionHash(string app) {
        return app_actionmap{app};
    }

    #! Returns a hash of all actions for the given application
    /** @param app the data provider application

        @return a hash of actions keyed by action name for the given application

        @throw APP-ERROR the given application is not registered

        @see getActionHash()
    */
    static *hash<string, hash<DataProviderActionInfo>> getActionHashEx(string app) {
        if (*hash<string, hash<DataProviderActionInfo>> rv = app_actionmap{app}) {
            return rv;
        }
        throw "APP-ERROR", sprintf("application %y is not registered", app);
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
    static *list<hash<DataProviderActionInfo>> getAllActions() {
        return top_actionmap.values();
    }

    #! Returns the action definition for the given application and action
    /** @param app the app name
        @param action the action name

        @return the action definition for the given application name and action or @ref nothing if not present

        @see getAppActionEx()
    */
    static *hash<DataProviderActionInfo> getAppAction(string app, string action) {
        return app_actionmap{app}{action};
    }

    #! Returns the application definition for the given application name
    /** @param app the app name
        @param action the action name

        @return the action definition for the given application name and action; throws an exception if the app is not
        registered

        @throw APP-ERROR the given application is not registered
        @throw ACTION-ERROR the given action is not registered

        @see getAppAction()
    */
    static hash<DataProviderActionInfo> getAppActionEx(string app, string action) {
        *hash<string, hash<DataProviderActionInfo>> actionmap = app_actionmap{app};
        if (!actionmap) {
            throw "APP-ACTION-ERROR", sprintf("application %y is not registered", app);
        }
        if (*hash<DataProviderActionInfo> rv = actionmap{action}) {
            return rv;
        }
        throw "ACTION-ERROR", sprintf("application %y has no action %y (known actions: %y)", app, action,
            keys actionmap);
    }

    #! Returns a data provider option for the given action and options
    /** @param action the action to use to return the data provider
        @param options a reference to options where constructor options will be taken from

        @return a data provider option for the given action and options
    */
    static AbstractDataProvider getDataProviderForAction(hash<DataProviderActionInfo> action,
            *reference<hash<auto>> options) {
        hash<auto> constructor_opts;
        map constructor_opts{$1.key} = remove options{$1.key},
            (action.convenience_options + action.advanced_options).pairIterator(),
            $1.value.loc == "constructor" && options.hasKey($1.key);
        return Class::forName(action.cls).newObject(constructor_opts);
    }

    #! Searches for application actions whose descriptions match a regular expression pattern
    /** @param app The application to search for actions in
        @param regex_pattern A regular expression pattern to match in the \c short_desc and \c desc fields
        @param regex_options Regular expression options; a case-insensitive match is performed by default

        @return All actions that match the regular expression pattern in the \c short_desc or \c desc fields

        @note If the application does not exist, no results are returned
    */
    static *list<hash<DataProviderActionInfo>> searchAppActionRegex(string app, string regex_pattern,
            int regex_options = RE_Caseless) {
        return map $1, app_actionmap{app}.values(),
            regex($1.short_desc, regex_pattern, regex_options) || regex($1.desc, regex_pattern, regex_options);
    }

    #! Searches for application actions whose descriptions match a substring
    /** @param app The application to search for actions in
        @param substr A substring to match in the \c short_desc and \c desc fields
        @param ignore_case Ignore case when searching

        @return All actions that match the substring argument in the \c short_desc or \c desc fields

        @note If the application does not exist, no results are returned
    */
    static *list<hash<DataProviderActionInfo>> searchAppActionSubstr(string app, string substr,
            bool ignore_case = True) {
        if (ignore_case) {
            substr = substr.upr();
            return map $1, app_actionmap{app}.values(),
                ($1.short_desc.upr().find(substr) >= 0) || ($1.desc.upr().find(substr) >= 0);
        }
        return map $1, app_actionmap{app}.values(), ($1.short_desc.find(substr) >= 0) || ($1.desc.find(substr) >= 0);
    }

    #! Searches for application actions whose descriptions match a regular expression pattern
    /** @param app The application to search for actions in
        @param regex_pattern A regular expression pattern to match in the \c short_desc and \c desc fields
        @param regex_options Regular expression options; a case-insensitive match is performed by default

        @return All actions that match the regular expression pattern in the \c short_desc or \c desc fields

        @throw APP-ERROR the given application is not registered

        @note If the application does not exist, an exception is thrown
    */
    static *list<hash<DataProviderActionInfo>> searchAppActionRegexEx(string app, string regex_pattern,
            int regex_options = RE_Caseless) {
        *hash<string, hash<DataProviderActionInfo>> appmap = app_actionmap{app};
        if (!appmap) {
            throw "APP-ERROR", sprintf("application %y is not registered", app);
        }
        return map $1, appmap.values(),
            regex($1.short_desc, regex_pattern, regex_options) || regex($1.desc, regex_pattern, regex_options);
    }

    #! Searches for application actions whose descriptions match a substring
    /** @param app The application to search for actions in
        @param substr A substring to match in the \c short_desc and \c desc fields
        @param ignore_case Ignore case when searching

        @return All actions that match the substring argument in the \c short_desc or \c desc fields

        @throw APP-ERROR the given application is not registered

        @note If the application does not exist, an exception is thrown
    */
    static *list<hash<DataProviderActionInfo>> searchAppActionSubstrEx(string app, string substr,
            bool ignore_case = True) {
        *hash<string, hash<DataProviderActionInfo>> appmap = app_actionmap{app};
        if (!appmap) {
            throw "APP-ERROR", sprintf("application %y is not registered", app);
        }
        if (ignore_case) {
            substr = substr.upr();
            return map $1, appmap.values(),
                ($1.short_desc.upr().find(substr) >= 0) || ($1.desc.upr().find(substr) >= 0);
        }
        return map $1, appmap.values(), ($1.short_desc.find(substr) >= 0) || ($1.desc.find(substr) >= 0);
    }

    #! Searches for actions whose descriptions match a regular expression pattern
    /** @param regex_pattern A regular expression pattern to match in the \c short_desc and \c desc fields
        @param regex_options Regular expression options; a case-insensitive match is performed by default

        @return All actions that match the regular expression pattern in the \c short_desc or \c desc fields
    */
    static *list<hash<DataProviderActionInfo>> searchActionRegex(string regex_pattern, int regex_options = RE_Caseless) {
        return map $1, top_actionmap.values(),
            regex($1.short_desc, regex_pattern, regex_options) || regex($1.desc, regex_pattern, regex_options);
    }

    #! Searches for actions whose descriptions match a substring
    /** @param substr A substring to match in the \c short_desc and \c desc fields
        @param ignore_case Ignore case when searching

        @return All actions that match the substring argument in the \c short_desc or \c desc fields
    */
    static *list<hash<DataProviderActionInfo>> searchActionSubstr(string substr, bool ignore_case = True) {
        if (ignore_case) {
            substr = substr.upr();
            return map $1, top_actionmap.values(),
                ($1.short_desc.upr().find(substr) >= 0) || ($1.desc.upr().find(substr) >= 0);
        }
        return map $1, top_actionmap.values(), ($1.short_desc.find(substr) >= 0) || ($1.desc.find(substr) >= 0);
    }

    #! Converts field definitions into option descriptions
    static hash<string, hash<ActionOptionInfo>> getActionOptionFromFields(hash<auto> fields,
            *hash<auto> additional_fields) {
        hash<string, hash<ActionOptionInfo>> rv;
        foreach hash<auto> i in (fields.pairIterator()) {
            hash<ActionOptionInfo> info = <ActionOptionInfo>{
                "display_name": i.value.display_name ?? AbstractDataField::getDisplayName(i.key),
                "short_desc": i.value.short_desc ?? AbstractDataField::getShortDescription(i.value.desc),
                "desc": i.value.desc ?? DefaultDescription,
            };
            auto type = i.value.type;
            if (type.typeCode() == NT_LIST) {
                type = type[0];
            }
            switch (type.typeCode()) {
                case NT_OBJECT:
                    if (type instanceof AbstractDataProviderType) {
                        info.type = type.getOptions()."qore.external_name" ?? type.getBaseTypeName();
                    } else {
                        info.type = type.getName();
                    }
                    break;

                case NT_STRING:
                    info.type = type;
                    break;

                default:
                    throw "OPTION-ERROR", sprintf("Cannot set option type from \"type\" option with type %y", type);
            }
            info += i.value{"default_value", "required", "sensitive", "example_value"} + additional_fields;
            if (i.value.allowed_values) {
                if (i.value.allowed_values instanceof list<hash<AllowedValueInfo>>
                    || i.value.allowed_values instanceof hash<AllowedValueInfo>) {
                    info.allowed_values = i.value.allowed_values;
                } else {
                    info.allowed_values = map <AllowedValueInfo>{
                        "value": $1,
                        "desc": DefaultDescription,
                    }, i.value.allowed_values;
                }
            }
            rv{i.key} = info;
        }
        return rv;
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
        # ensure path starts with a leading "/"
        return path == "/" ? path : "/" + path;
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
        map remove top_actionmap{name + "/" + $1.action}, app_actionmap{name}.values();
        remove app_actionmap{name};
    }
}
}
