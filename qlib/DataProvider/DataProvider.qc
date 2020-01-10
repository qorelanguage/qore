# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file DataProvider.qc module for data access and introspection

/** DataProvider.qc Copyright 2019 - 2020 Qore Technologies, s.r.o.

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

# minimum required Qore version
%requires qore >= 0.9.4
# assume local scope for variables, do not use "$" signs
%new-style
# require type definitions everywhere
%require-types
#! strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

%requires reflection
%requires Util

#! Contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Data provider class
public class DataProvider {
    public {
        #! Map of known data provider factory names to modules
        const FactoryMap = {
            "db": "DbDataProvider",
            "swagger": "SwaggerDataProvider",
            "csvread": "CsvUtil",
            "csvwrite": "CsvUtil",
            "fixedlengthread": "FixedLengthUtil",
            "fixedlengthwrite": "FixedLengthUtil",
            "salesforcerest": "SalesforceRestDataProvider",
            # provided by the xml module
            "soap": "SoapDataProvider",
        };

        #! Map of known type path prefixes to modules
        const TypeMap = {
            "qore/ftp": "FtpPollerUtil",
            "qore/sftp": "SftpPollerUtil",
            "qore/fsevents": "FsEventPollerUtil",
        };

        #! List of data provider factory modules
        const FactoryModuleList = keys (map {$1: True}, FactoryMap.iterator());
        # "map" is used above to ensure that the values only appear once in the final list
    }

    private {
        #! data provider factory cache
        static hash<string, AbstractDataProviderFactory> factory_cache;

        #! data provider factory module map
        static hash<string, string> factory_module_map;

        #! data provider factory cache lock
        static Mutex factory_cache_lock();

        #! data provider data type cache
        static DataProviderTypeEntry type_cache();

        #! data provider type module map
        static hash<string, string> type_module_map;

        #! data provider type cache lock
        static Mutex type_cache_lock();

        #! flag to allow automatic configuration from environment variables
        /** must be set before any requests for types or factories are made
        */
        static bool allow_env_config = False;

        #! flag that locks the \a allow_env_config var
        static bool env_config_locked = False;
    }

    #! Register a new data provider factory
    /** @param factory the new data provider factory

        @throw PROVIDER-ERROR the given provider has already been registered
    */
    static registerFactory(AbstractDataProviderFactory factory) {
        factory_cache_lock.lock();
        on_exit factory_cache_lock.unlock();

        string name = factory.getName();
        if (factory_cache{name}) {
            # check if factory is being re-registered after module injection
            if (!DataProvider::checkInjection(factory, factory_module_map)) {
                throw "PROVIDER-ERROR", sprintf("provider %y has already been registered", name);
            }
        }
        {
            *string module_name = Class::getClass(factory).getModuleName();
            if (exists module_name) {
                factory_module_map{factory.className()} = module_name;
            }
        }
        factory_cache{name} = factory;
    }

    #! Returns the given data provider factory or @ref nothing if not present
    static *AbstractDataProviderFactory getFactory(string name) {
        DataProvider::checkRequest();
        if (!factory_cache{name}) {
            *string module_name = FactoryMap{name} ?? DataProvider::tryGetFactoryModuleName(name);
            if (!module_name) {
                return;
            }
            if (!DataProvider::tryLoad(module_name)) {
                return;
            }
        }
        return factory_cache{name};
    }

    #! Returns the given data provider factory or throws an exception if not present
    /** @param name the name of the data provider to retrieve

        @return the given data provider; an exception is thrown if the data provide is unknown

        @throw PROVIDER-ERROR the given provider is unknown
    */
    static AbstractDataProviderFactory getFactoryEx(string name) {
        *AbstractDataProviderFactory factory = DataProvider::getFactory(name);
        if (!factory) {
            throw "PROVIDER-ERROR", sprintf("data provider factory %y is unknown; known data provider factories: %y", name,
                keys factory_cache);
        }
        return factory;
    }

    #! Returns a data provider object from the given factory, created with the given constructor options
    /**
        @throw PROVIDER-ERROR the given provider factory or child provider is unknown
    */
    static AbstractDataProvider getFactoryObject(string name, *hash<auto> options) {
        AbstractDataProvider provider;
        {
            list<string> path = name.split("/");
            provider = DataProvider::getFactoryEx(shift path).create(options);
            map provider = provider.getChildProvider($1), path;
        }

        return provider;
    }

    #! Returns a data provider object from the given factory with options given as string-formatted hash in curly brackets and child data providers separated by forward slashes
    /** @par Example:
        @code{.py}
DbDataProvider db = DataProvider::getObjectFromString("db{oracle:user/password@db}/my_table");
        @endcode

        @throw PROVIDER-ERROR the given provider factory or child provider is unknown
    */
    static AbstractDataProvider getFactoryObjectFromString(string name) {
        # get options, respecting balanced curly brackets
        *string opts = (name =~ x/{((?:(?>[^{}]*)|(?0))*)}/)[0];
        # remove empty options; the above regex does not catch them :(
        if (!opts) {
            name =~ s/{}//;
        }
        auto options;
        if (opts) {
            name = replace(name, "{" + opts + "}", "");
            options = parse_to_qore_value(opts);
            if (exists options && options.typeCode() != NT_HASH) {
                throw "PROVIDER-ERROR", sprintf("option string in %y does not parse to type \"hash\"; got "
                    "type %y instead", name, options.type());
            }
        }

        return DataProvider::getFactoryObject(name, options);
    }

    #! Returns a list of registered data provider factories
    static *list<string> listFactories() {
        return keys factory_cache;
    }

    #! Register a new data provider type
    /** @param path a \c "/" separated path for the type; this is the lookup index
        @param type the new data provider type

        @throw PROVIDER-ERROR the given provider has already been registered

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    static registerType(string path, AbstractDataProviderType type) {
        list<string> type_path = DataProvider::getPathList(path);

        DataProviderTypeEntry type_entry = type_cache;

        type_cache_lock.lock();
        on_exit type_cache_lock.unlock();

        map type_entry = type_entry.getCreateChild($1), type_path;

        try {
            type_entry.setType(type);
        } catch (hash<ExceptionInfo> ex) {
            if (ex.err == "TYPE-ERROR") {
                # check if type is being re-registered after module injection
                if (!DataProvider::checkInjection(type, type_module_map)) {
                    throw "PROVIDER-ERROR", sprintf("provider type %y has already been registered", path);
                }
            }
        }

        {
            *string module_name = Class::getClass(type).getModuleName();
            if (exists module_name) {
                type_module_map{type.className()} = module_name;
            }
        }
    }

    #! Returns the given data provider type or @ref nothing if not present
    /** @param path a \c "/" separated path for the type

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    static *AbstractDataProviderType getType(string path) {
        DataProvider::checkRequest();
        list<string> type_path = DataProvider::getPathList(path);
        *DataProviderTypeEntry type_entry = type_cache;
        map type_entry = type_entry.getChild($1), type_path, type_entry;
        if (!type_entry) {
            type_entry = DataProvider::tryLoadTypeFromPath(type_path);
        }
        if (type_entry) {
            return type_entry.getType();
        }
    }

    #! Returns the given data provider type or throws an exception if not present
    /** @param path a \c "/" separated path for the type

        @return the given data provider; an exception is thrown if the data provide is unknown

        @throw PROVIDER-ERROR the given data provider type path is unknown

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    static AbstractDataProviderType getTypeEx(string path) {
        DataProvider::checkRequest();
        list<string> type_path = DataProvider::getPathList(path);
        DataProviderTypeEntry type_entry = type_cache;
        foreach string node in (type_path) {
            *DataProviderTypeEntry next_entry = type_entry.getChild(node);
            if (!next_entry) {
                if (next_entry = DataProvider::tryLoadTypeFromPath(type_path)) {
                    type_entry = next_entry;
                    break;
                }
                throw "PROVIDER-ERROR", sprintf("%y: data provider type element %y is unknown in %y; known children: %y",
                    path, node, type_entry.getPath(), type_entry.getChildNames());

            }
            type_entry = next_entry;
        }

        *AbstractDataProviderType type = type_entry.getType();
        if (!type) {
            throw "PROVIDER-ERROR", sprintf("data provider type path %y does not contain a type but rather is an "
                "intermediate node with children: %y", path, type_entry.getChildNames());
        }
        return type;
    }

    #! Returns the root type entry
    static DataProviderTypeEntry getTypeRoot() {
        return type_cache;
    }

    #! Returns a list of registered data provider type paths
    /**
        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    static *list<string> listTypes() {
        return type_cache.listAllTypes();
    }

    #! Registers all known data provider factories
    static registerKnownFactories() {
        map DataProvider::tryLoad($1), FactoryModuleList;
    }

    #! Converts an info hash with objects to a hash with string descriptions instead of the objects
    static hash<auto> getInfoAsData(hash<auto> info0) {
        # strip types
        hash info = info0;
        foreach hash<auto> i0 in (info.pairIterator()) {
            if (i0.value.typeCode() == NT_HASH && i0.key =~ /_options/) {
                # if the info key value is a hash with a "type" key, then return the type name instead of the type object
                map info{i0.key}{$1.key}.type = (map $1.getName(), $1.value.type),
                    i0.value.pairIterator(),
                    $1.value.type;
            }
        }
        return info;
    }

    #! Sets the flag that allows for automatic configuration from environment variables
    /** Must be called before any requests for types or factories are made, or an exception is raised

        @throw DATA-PROVIDER-ERROR cannot call this method after requests for types or factories are made
    */
    static setAutoConfig() {
        if (env_config_locked) {
            throw "DATA-PROVIDER-ERROR", "cannot call DataProvider::setAutoConfigFromEnvironment() after type or "
                "factories have been loaded";
        }
        allow_env_config = True;
    }

    #! Returns the auto config flag
    static bool getAutoConfig() {
        return allow_env_config;
    }

    #! Returns a list of strings in a path separated by "/" characters
    static list<string> getPathList(string path) {
        # remove leading "/" chars
        path =~ s/^\/+//;
        # combine multiple "/" chars into a single char
        path =~ s/\/+/\//g;
        return path.split("/");
    }

    #! Sets the \a env_config_locked variable if not already set
    static private checkRequest() {
        if (!env_config_locked) {
            env_config_locked = True;
        }
    }

    #! Tries to load a module corresponding to the given factory
    private static *string tryGetFactoryModuleName(string name) {
        return DataProvider::tryGetModuleName(name, "get_data_provider_map", "data provider");
    }

    #! Tries to load a module corresponding to the given factory
    private static *string tryGetTypeModuleName(string name) {
        return DataProvider::tryGetModuleName(name, "get_type_provider_map", "type");
    }

    #! Tries to load a module corresponding to the given factory
    private static *string tryGetModuleName(string name, string func, string type) {
        *string prov = ENV.QORE_DATA_PROVIDERS;
        if (prov) {
            list<string> pl = ENV.QORE_DATA_PROVIDERS.split(PathSep);
            foreach string mod in (pl) {
                Program p(PO_NO_USER_API);
                try {
                    p.loadModule(mod);
                } catch (hash<ExceptionInfo> ex) {
                    stderr.printf("ERROR: invalid QORE_DATA_PROVIDERS module %y %s: %s\n", mod, ex.err, ex.desc);
                }
                try {
                    hash<string, string> dmap = p.callFunction(func);
                    *string mod = dmap{name};
                    if (mod) {
                        return mod;
                    }
                } catch (hash<ExceptionInfo> ex) {
                    # ignore NO-FUNCTION error
                    if (ex.err != "NO-FUNCTION") {
                        stderr.printf("ERROR: error in QORE_DATA_PROVIDERS module %y %s registration function: %s: "
                            "%s\n", mod, type, ex.err, ex.desc);
                    }
                }
            }
        }
    }

    #! Tries to load the given type
    private static *DataProviderTypeEntry tryLoadTypeFromPath(list<string> type_path) {
        string key = type_path[0 .. (type_path.size() - 2)].join("/");
        *string module_name = TypeMap{key} ?? DataProvider::tryGetTypeModuleName(key);
        if (!module_name) {
            return;
        }
        if (!DataProvider::tryLoad(module_name)) {
            return;
        }
        # now that the module has been loaded, try to return the type
        *DataProviderTypeEntry type_entry = type_cache;
        map type_entry = type_entry.getChild($1), type_path, type_entry;
        return type_entry;
    }

    #! Try to load the given module
    private static bool tryLoad(string module_str, *bool verbose) {
        try {
            load_module(module_str);
            return True;
        } catch (hash<ExceptionInfo> ex) {
            if (verbose || True) {
                stderr.printf("%s: %s: %s\n", get_ex_pos(ex), ex.err, ex.desc);
            }
        }
        return False;
    }

    #! Check if the object is from a module that has been subject to dependency injections
    private static bool checkInjection(object obj, hash<string, string> module_map) {
        *string module_name = Class::getClass(obj).getModuleName();
        return module_name && module_map{obj.className()} == module_name;
    }
}
}
