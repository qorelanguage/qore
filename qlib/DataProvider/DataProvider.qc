# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file DataProvider.qc module for data access and introspection

/** DataProvider.qc Copyright 2019 - 2024 Qore Technologies, s.r.o.

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

%requires reflection
%requires Util

#! Contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Data provider class
public class DataProvider {
    public {
        #! Map of known data provider factory names to modules
        const FactoryMap = {
            # provided by the jni module
            "busylight": "BusyLightDataProvider",

            "cdsrest": "CdsRestDataProvider",
            "csvread": "CsvUtil",
            "csvwrite": "CsvUtil",
            "db": "DbDataProvider",
            "discord": "DiscordDataProvider",
            "discordws": "DiscordDataProvider",
            "elasticsearch": "ElasticSearchDataProvider",
            "empathicbuilding": "EmpathicBuildingDataProvider",

            # provided by the jni module
            "excelread": "ExcelDataProvider",

            "file": "FileDataProvider",
            "filepoller": "FilePoller",
            "fixedlengthread": "FixedLengthUtil",
            "fixedlengthwrite": "FixedLengthUtil",
            "ftpclient": "FtpClientDataProvider",
            "ftppoller": "FtpPoller",
            "gcal": "GoogleCalendarDataProvider",
            "gmail": "GmailDataProvider",
            "httpclient": "HttpClientDataProvider",
            "mewsrest": "MewsRestDataProvider",

            # provided by the jni module
            "mqtt": "MqttDataProvider",

            "pop3": "Pop3ClientDataProvider",
            "restclient": "RestClientDataProvider",
            "salesforcerest": "SalesforceRestDataProvider",
            "sax": "SaxDataProvider",
            "servicenowrest": "ServiceNowRestDataProvider",
            "smtpclient": "SmtpClient",

            # provided by the xml module
            "soap": "SoapDataProvider",

            "swagger": "SwaggerDataProvider",
            "wsclient": "WebSocketClient",
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
        static Gate factory_cache_lock();

        #! data provider data type cache
        static DataProviderTypeCache type_cache();

        #! data provider type module map
        static hash<string, string> type_module_map;

        #! flag to allow automatic configuration from environment variables
        /** must be set before any requests for types or factories are made
        */
        static bool allow_env_config = False;

        #! flag that locks the \a allow_env_config var
        static bool env_config_locked = False;

        #! set of module already loaded
        static hash<string, bool> mod_map;
    }

    #! Register a new data provider factory
    /** @param factory the new data provider factory

        @throw PROVIDER-ERROR the given provider has already been registered
    */
    static registerFactory(AbstractDataProviderFactory factory) {
        factory_cache_lock.enter();
        on_exit factory_cache_lock.exit();

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

        # deregister the app automatically if module initialization fails
        ImplicitModuleTransaction::add(sub () { DataProvider::deregisterFactory(name); });
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
            throw "PROVIDER-ERROR", sprintf("data provider factory %y is unknown; known data provider factories: %y",
                name, keys factory_cache);
        }
        return factory;
    }

    #! Returns a data provider object from the given factory, created with the given constructor options
    /** @param path the \c "/"-separated path to the target data provider; the first element is the factory name
        @param template_options options with templated values; only the keys in this hash are relevant, as they
        indicate options that will be present at a later time when their values can be derived; the values provided in
        this call are assumed to be in an implementation-dependent templated format and should be ignored
        @param options factory creation options

        @throw PROVIDER-ERROR the given provider factory or child provider is unknown

        @since DataProvider 2.7.1
    */
    static AbstractDataProvider getFactoryObjectWithTemplate(string path, *hash<auto> template_options,
            *hash<auto> options) {
        AbstractDataProvider provider;
        {
            list<string> path_list = path.split("/");
            provider = DataProvider::getFactoryEx(shift path_list).createWithTemplate(template_options, options);
            map provider = provider.getChildProviderEx($1), path_list;
        }
        return provider;
    }

    #! Returns a data provider object from the given factory, created with the given options and example data
    /** @param path the \c "/"-separated path to the target data provider; the first element is the factory name
        @param example example data to process
        @param options factory creation options

        @throw PROVIDER-ERROR the given provider factory or child provider is unknown

        @since DataProvider 2.7.1
    */
    static AbstractDataProvider getFactoryObjectFromExample(string path, data example, *hash<auto> options) {
        AbstractDataProvider provider;
        {
            list<string> path_list = path.split("/");
            provider = DataProvider::getFactoryEx(shift path_list).getProviderFromExample(example, options);
            map provider = provider.getChildProviderEx($1), path_list;
        }
        return provider;
    }

    #! Returns a data provider object from the given factory, created with the given options and example data
    /** @param path the \c "/"-separated path to the target data provider; the first element is the factory name
        @param example example data to process
        @param options factory creation options

        @throw PROVIDER-ERROR the given provider factory or child provider is unknown

        @since DataProvider 2.7.1
    */
    static AbstractDataProvider getFactoryObjectFromExample(string path, InputStream example, *hash<auto> options) {
        AbstractDataProvider provider;
        {
            list<string> path_list = path.split("/");
            provider = DataProvider::getFactoryEx(shift path_list).getProviderFromExample(example, options);
            map provider = provider.getChildProviderEx($1), path_list;
        }
        return provider;
    }

    #! Returns a data provider object from the given factory, created with the given constructor options
    /** @param path the \c "/"-separated path to the target data provider; the first element is the factory name
        @param options factory creation options

        @throw PROVIDER-ERROR the given provider factory or child provider is unknown
    */
    static AbstractDataProvider getFactoryObject(string path, *hash<auto> options) {
        AbstractDataProvider provider;
        {
            list<string> path_list = path.split("/");
            provider = DataProvider::getFactoryEx(shift path_list).create(options);
            map provider = provider.getChildProviderEx($1), path_list;
        }
        return provider;
    }

    #! Returns a data provider object from the given factory string
    /** Factory options are given as string-formatted hash in curly brackets and child data providers are separated by
        forward slashes

        @par Example:
        @code{.py}
DbDataProvider db = DataProvider::getFactoryObjectFromString("db{datasource=oracle:user/password@db}/my_table");
        @endcode

        @throw PROVIDER-ERROR the given provider factory or child provider is unknown
    */
    static AbstractDataProvider getFactoryObjectFromString(string name) {
        hash<FactoryInfo> info = DataProvider::getFactoryInfoFromString(name);
        AbstractDataProvider provider = DataProvider::getFactoryEx(info.name).create(info.options);
        map provider = provider.getChildProviderEx($1), info.path_list;
        return provider;
    }

    #! Returns a data provider object from the given factory string using environment variables to find the factory
    /** Factory options are given as string-formatted hash in curly brackets and child data providers are separated by
        forward slashes

        @par Example:
        @code{.py}
DbDataProvider db = DataProvider::getFactoryObjectFromStringUseEnv("db{datasource=oracle:user/password@db}/my_table");
        @endcode

        @throw PROVIDER-ERROR the given provider factory or child provider is unknown
    */
    static AbstractDataProvider getFactoryObjectFromStringUseEnv(string name) {
        hash<FactoryInfo> info = DataProvider::getFactoryInfoFromString(name);
        AbstractDataProviderFactory factory;
        if ((*string factory_name = FactoryMap{info.name}) && DataProvider::tryLoad(factory_name)) {
            factory = DataProvider::getFactoryEx(info.name);
        } else {
            *object v = DataProvider::loadFromEnvironment("get_data_provider_map", "data provider", <SymbolInfo>{
                "type": "factory",
                "name": info.name,
            });
            if (v) {
                factory = cast<AbstractDataProviderFactory>(v);
            }
        }
        if (factory) {
            AbstractDataProvider provider = factory.create(info.options);
            map provider = provider.getChildProvider($1), info.path_list;
            return provider;
        }

        throw "FACTORY-ERROR", sprintf("data provider factory %y in %y is unknown", info.name, name);
    }

    #! Returns a hash of factory information from a string
    private static hash<FactoryInfo> getFactoryInfoFromString(string name) {
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
        list<string> path_list = name.split("/");
        return <FactoryInfo>{
            "name": shift path_list,
            "path_list": path_list,
            "options": options,
        };
    }

    #! Returns a list of registered data provider factories
    static *list<string> listFactories() {
        return keys factory_cache;
    }

    #! Returns the data provider cache
    static DataProviderTypeCache getTypeCache() {
        return type_cache;
    }

    #! Register a new data provider type
    /** @param path a \c "/" separated path for the type; this is the lookup index
        @param type the new data provider type

        @throw PROVIDER-ERROR the given provider has already been registered

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    static registerType(string path, AbstractDataProviderType type) {
        try {
            type_cache.registerType(path, type, NOTHING, True);
        } catch (hash<ExceptionInfo> ex) {
            if (ex.err == "TYPE-ERROR") {
                # check if type is being re-registered after module injection
                if (!DataProvider::checkInjection(type, type_module_map)) {
                    throw "PROVIDER-ERROR", sprintf("provider type %y has already been registered", path);
                }
            } else {
                rethrow;
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
    /** @param path a \c "/" separated path for the type; trailing path elements can be a path into fields

        @return the given data provider type; @ref NOTHING is returned if the type cannot be resolved

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    static *AbstractDataProviderType getType(string path) {
        DataProvider::checkRequest();
        return type_cache.getTypeImpl(path, False, \DataProvider::tryLoadTypeFromPath());
    }

    #! Returns the given data provider type or throws an exception if not present
    /** @param path a \c "/" separated path for the type; trailing path elements can be a path into fields

        @return the given data provider type; an exception is thrown if the data provide is unknown

        @throw PROVIDER-ERROR the given data provider type path is unknown

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    static AbstractDataProviderType getTypeEx(string path) {
        DataProvider::checkRequest();
        return type_cache.getTypeImpl(path, True, \DataProvider::tryLoadTypeFromPath());
    }

    #! Returns the root type entry
    static DataProviderTypeEntry getTypeRoot() {
        return type_cache.getRoot();
    }

    #! Locks all types
    static lockAllTypes() {
        type_cache.lockAll();
    }

    #! Returns a list of registered data provider type paths
    /**
        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    static *list<string> listTypes() {
        return type_cache.listTypes();
    }

    #! Registers all known data provider factories
    static registerKnownFactories(*Logger::LoggerInterface logger) {
        # load known modules
        map DataProvider::tryLoad($1, logger), FactoryModuleList;
    }

    #! Loads data providers from the environment
    static loadProvidersFromEnvironment() {
        DataProvider::loadFromEnvironment("get_data_provider_map", "data provider");
    }

    #! Tries to load a data provider from the environment from the connection name
    static *AbstractDataProvider tryLoadProviderForConnectionFromEnv(string name, *string subtype) {
        load_module("ConnectionProvider");
        load_module("DatasourceProvider");
        *object v = DataProvider::loadFromEnvironment("get_data_provider_map", "data provider", <SymbolInfo>{
            "type": "connection",
            "name": name,
            "subtype": subtype,
        });
        if (v) {
            return cast<AbstractDataProvider>(v);
        }
    }

    #! Registers all known data provider types
    static registerKnownTypes() {
        # register basic qore types
        DataProvider::registerType("qore/hash", new HashDataType());
        DataProvider::registerType("qore/string", new QoreStringDataType());
        DataProvider::registerType("qore/int", new QoreIntDataType());
        DataProvider::registerType("qore/number", new QoreNumberDataType());
        DataProvider::registerType("qore/float", new QoreFloatDataType());
        DataProvider::registerType("qore/date", new QoreDateDataType());
        DataProvider::registerType("qore/bool", new QoreBoolDataType());
        DataProvider::registerType("qore/binary", new QoreBinaryDataType());

        map DataProvider::tryLoad($1), TypeMap.iterator();
    }

    #! Loads data types from the environment
    static loadTypesFromEnvironment() {
        DataProvider::loadFromEnvironment("get_type_provider_map", "type");
    }

    #! Converts an info hash with objects to a hash with string descriptions instead of the objects
    /** @param info0 the raw info with objects describing the types
        @param with_type_info if @ref True then more type information will be returned in the response
    */
    static hash<auto> getInfoAsData(hash<auto> info0, *bool with_type_info) {
        # strip types
        hash info = info0;
        foreach hash<auto> i0 in (info.pairIterator()) {
            if (i0.value.typeCode() == NT_HASH) {
                if (i0.key =~ /_options/ || i0.key == "events" || i0.key == "messages") {
                    # if the info key value is a hash with a "type" key, then return the type name instead of the type
                    # object
                    map info{i0.key}{$1.key}.type = (map $1.getName(), $1.value.type),
                        i0.value.pairIterator(),
                        $1.value.type;
                    if (with_type_info) {
                        map info{i0.key}{$1.key}.type_info = (map $1.getInfo(), $1.value.type),
                            i0.value.pairIterator(),
                            $1.value.type;
                    }
                } else if (i0.key == "expressions") {
                    foreach hash<auto> i1 in (i0.value.pairIterator()) {
                        info{i0.key}{i1.key}.args = map (
                            "type_code": SignatureTypeCodeMap{$1.type_code},
                            "type": $1.type.getName(),
                        ), i1.value.args;
                        info{i0.key}{i1.key}.return_type = i1.value.return_type.getName();
                        if (with_type_info) {
                            info{i0.key}{i1.key}.arg_type_info = map (
                                "type_code": SignatureTypeCodeMap{$1.type_code},
                                "type": $1.type.getInfo(),
                            ), i1.value.args;
                            info{i0.key}{i1.key}.return_type_info = i1.value.return_type.getInfo();
                        }
                    }
                }
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
    private static checkRequest() {
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
        *DataProviderTypeEntry type_entry = type_cache.getRoot();
        map type_entry = type_entry.getChild($1), type_path, type_entry;
        return type_entry;
    }

    #! Try to load the given module
    private static bool tryLoad(string module_str, *Logger::LoggerInterface logger) {
        try {
            if (logger) {
                logger.info("DataProvider loading module: %y", module_str);
            }
            load_module(module_str);
            return True;
        } catch (hash<ExceptionInfo> ex) {
            string err = sprintf("DataProvider load module %y: %s\n", module_str, get_exception_string(ex));
            if (logger) {
                logger.error("%s", err);
            } else {
                stderr.print(err + "\n");
            }
        }
        return False;
    }

    #! Loads modules from the QORE_DATA_PROVIDERS environment variable
    private static *object loadFromEnvironment(string func, string type, *hash<SymbolInfo> info) {
        # load modules from the QORE_DATA_PROVIDERS environment variable
        *string prov = ENV.QORE_DATA_PROVIDERS;
        if (prov) {
            list<string> pl = ENV.QORE_DATA_PROVIDERS.split(PathSep);
            foreach string mod in (pl) {
                if (mod_map{mod}) {
                    continue;
                }
                mod_map{mod} = True;
                Program p(PO_NO_USER_API);
                try {
                    p.loadModule(mod);
                    if (info && (*object rv = DataProvider::checkSymbol(info))) {
                        return rv;
                    }
                } catch (hash<ExceptionInfo> ex) {
                    stderr.printf("ERROR: invalid QORE_DATA_PROVIDERS module %y %s: %s\n", mod, ex.err, ex.desc);
                }
                hash<string, string> dmap;
                try {
                    dmap = p.callFunction(func);
                } catch (hash<ExceptionInfo> ex) {
                    # ignore NO-FUNCTION error
                    if (ex.err != "NO-FUNCTION") {
                        stderr.printf("ERROR: error in QORE_DATA_PROVIDERS module %y %s registration function: %s: "
                            "%s\n", mod, type, ex.err, ex.desc);
                    }
                }
                foreach string mod_str in (dmap.iterator()) {
                    if (mod_map{mod_str}) {
                        continue;
                    }
                    mod_map{mod_str} = True;
                    if (DataProvider::tryLoad(mod_str)) {
                        if (info && (*object rv = DataProvider::checkSymbol(info))) {
                            return rv;
                        }
                    }
                }
            }

        }
    }

    #! Returns True if an object of the given type has been loaded
    private static *object checkSymbol(hash<SymbolInfo> info) {
        switch (info.type) {
            case "connection": {
                try {
                    object conn = call_function("get_connection", info.name);
                    return info.subtype.val() ? conn.getDataProvider(info.subtype) : conn.getDataProvider();
                } catch (hash<ExceptionInfo> ex) {
                    if (ex.err != "CONNECTION-ERROR") {
                        rethrow;
                    }
                }

                # try to get a provider from a DB connection
                try {
                    Datasource ds(call_function("get_ds_string", info.name));
                    load_module("DbDataProvider");
                    return create_object("DbDataProvider", ds);
                } catch (hash<ExceptionInfo> ex) {
                    if (ex.err != "NO-FUNCTION" && ex.err != "DATASOURCE-ERROR"
                        && ex.err != "DATASOURCE-PARSE-ERROR") {
                        rethrow;
                    }
                }
                break;
            }

            case "factory": {
                return DataProvider::getFactory(info.name);
            }

            default:
                throw "INVALID-SYMBOL-TYPE", sprintf("symbol type %y is unknown", info.type);
        }
    }

    #! Check if the object is from a module that has been subject to dependency injections
    private static bool checkInjection(object obj, hash<string, string> module_map) {
        *string module_name = Class::getClass(obj).getModuleName();
        return module_name && module_map{obj.className()} == module_name;
    }

    #! Deregisters a factory if an exception occurs during the other module's initialization
    private static deregisterFactory(string name) {
        factory_cache_lock.enter();
        on_exit factory_cache_lock.exit();

        *AbstractDataProviderFactory factory = remove factory_cache{name};
        if (!factory) {
            return;
        }
        *string module_name = Class::getClass(factory).getModuleName();
        if (exists module_name) {
            remove factory_module_map{factory.className()};
        }
    }
}

# private hashdecls
hashdecl SymbolInfo {
    # symbol type
    string type;
    # symbol name
    string name;
    # subtype, if any
    *string subtype;
}
hashdecl FactoryInfo {
    # factory name
    string name;
    # path to final data provider in factory
    list<string> path_list;
    # factory options
    auto options;
}
}
