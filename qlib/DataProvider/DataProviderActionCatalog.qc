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
#! DataProvider action definition
public hashdecl DataProviderActionInfo {
    #! Name of the factory that create the data provider
    /** Note that the path is relative from both the factory and the connection
    */
    string factory;

    #! Connection scheme for connection providing the action, if any
    *string scheme;

    #! Data provider path to action
    /** The path is relative from the factory and the connection (if any)
    */
    string path;

    #! The unique action code for the action; unique in the factory and path
    string action;

    #! Description of any path variables
    /** Keys are path variables, values are string descriptions with markdown formatting
    */
    *hash<string, string> path_vars;

    #! Action short description, plain text formatting
    string short_desc;

    #! Action long description, markdown formatting
    string desc;

    #! Action type: API
    bool supports_request = False;

    #! Action type: event
    bool supports_observable = False;

    #! Action type: message
    bool supports_messages = False;

    #! Action type: search records
    bool supports_read = False;

    #! Action type: create records
    bool supports_create = False;

    #! Action type: update records
    bool supports_update = False;

    #! Action type: upsert records
    bool supports_upsert = False;

    #! Action type: delete records
    bool supports_delete = False;

    #! Action subtype: do a single record search
    /** Only used when \c supports_read is True
    */
    bool single_record = False;
}

#! data provider action flags
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

#! DataProvider action catalog class
public class DataProviderActionCatalog {
    public {}

    private {
        #! Mutex for atomic operations
        static Mutex lck();

        #! Action cache, the key is made up of <factory>-<path>, the second key is the action
        static hash<string, hash<string, hash<DataProviderActionInfo>>> cache;

        #! Scheme map; keys are connection schemes
        static hash<string, list<hash<DataProviderActionInfo>>> smap;

        #! Factory map; keys are factory names
        static hash<string, list<hash<DataProviderActionInfo>>> fmap;
    }

    #! Registers a data provider action
    /** @param action the action to register

        @throw ACTION-ERROR missing \c factory or \c path attributes, action already registered

        The \c factory and \c path attributes must be unique for every action registered
    */
    static registerAction(hash<DataProviderActionInfo> action) {
        if (!action.factory.val()) {
            throw "ACTION-ERROR", sprintf("missing factory name in action: %y", action);
        }
        if (!action.path.val()) {
            throw "ACTION-ERROR", sprintf("missing path in action: %y", action);
        }
        if (!action.action.val()) {
            throw "ACTION-ERROR", sprintf("missing action code in action: %y", action);
        }
        action.path = DataProviderActionCatalog::normalizePath(action.path);
        string key = sprintf("%s/%s", action.factory, action.path);

        # check that all path variables are defined
        if (action.path_vars) {
            *list<string> vars = (action.path =~ x/\{([^\}]*)}/g);
            if (*hash<string, string> pvc = action.path_vars - vars) {
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

        AutoLock al(lck);
        if (*hash<DataProviderActionInfo> act = cache{key}{action.action}) {
            throw "ACTION-ERROR", sprintf("action with factory %y, path %y, and action %y already registered: %y",
                act.factory, act.path, action.action, act);
        }
        # add to cache
        cache{key}{action.action} = action;
        # add to scheme map
        if (action.scheme) {
            if (!smap{action.scheme}) {
                smap{action.scheme} = ();
            }
            smap{action.scheme} += action;
        }
        # add to factory map
        if (!smap{action.factory}) {
            smap{action.factory} = ();
        }
        smap{action.factory} += action;
    }

    #! Returns all actions for a particular connection scheme
    /** @param the connection scheme

        @return a list of actions associated with the given scheme
    */
    *list<hash<DataProviderActionInfo>> getActionsForScheme(string scheme) {
        return smap{scheme};
    }

    #! Returns all actions for a particular data provider factory
    /** @param the data provider factory

        @return a list of actions associated with the given data provider factory
    */
    *list<hash<DataProviderActionInfo>> getActionsForFactory(string factory) {
        return fmap{factory};
    }

    #! Returns all actions for the given factory and path
    /** @param factory the data provider factory
        @param path the path for the action

        @return a list of actions for the given factory and path
    */
    *list<hash<DataProviderActionInfo>> getActions(string factory, string path) {
        return cache{sprintf("%s/%s", factory, DataProviderActionCatalog::normalizePath(path))}.values();
    }

    #! Returns all actions in the cache
    /** @return all actions in the cache
    */
    static *list<hash<DataProviderActionInfo>> getAll() {
        return foldl $1 + $2, (map $1.values(), cache.values());
    }

    #! Searches for actions whose descriptions match a regular expression pattern
    /** @param regex_pattern A regular expression pattern to match in the \c short_desc and \c desc fields
        @param regex_options Regular expression options; a case-insensitive match is performed by default

        @return All actions that match the regular expression pattern in the \c short_desc or \c desc fields
    */
    static *list<hash<DataProviderActionInfo>> searchRegex(string regex_pattern, int regex_options = RE_Caseless) {
        return map $1, (foldl $1 + $2, (map $1.values(), cache.values())),
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
            return map $1, (foldl $1 + $2, (map $1.values(), cache.values())),
                ($1.short_desc.upr().find(substr) >= 0) || ($1.desc.upr().find(substr) >= 0);
        }
        return map $1, (foldl $1 + $2, (map $1.values(), cache.values())),
            ($1.short_desc.find(substr) >= 0) || ($1.desc.find(substr) >= 0);
    }

    #! Returns a normalized path to ensure that comparisons always work
    private static string normalizePath(string path) {
        # ensure that "/" characters are never duplicated
        if (path =~ /\/\//) {
            throw "PATH-ERROR", sprintf("%y: illegal consecutive \"/\" characters", path);
        }
        # remove any leading or trailing "/" chars
        path =~ s/^\/?(.*[^\/])\/?/$1/;
        return path;
    }
}
}
