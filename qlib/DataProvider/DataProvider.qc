# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file DataProvider.qc module for data access and introspection

/** DataProvider.qc Copyright 2019 Qore Technologies, s.r.o.

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
            # provided by the xml module
            "soap": "SoapDataProvider",
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
        static hash<string, AbstractDataProviderType> type_cache;

        #! data provider type module map
        static hash<string, string> type_module_map;

        #! data provider type cache lock
        static Mutex type_cache_lock();
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
        if (!factory_cache{name}) {
            *string module_name = FactoryMap{name} ?? DataProvider::tryGetModuleName(name);
            if (!module_name) {
                return;
            }
            if (!DataProvider::tryLoad(module_name)) {
                return;
            }
        }
        return factory_cache{name};
    }

    #! Tries to load a module corresponding to the given factory
    static *string tryGetModuleName(string name) {
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
                    hash<string, string> dmap = p.callFunction("get_data_provider_map");
                    *string mod = dmap{name};
                    if (mod) {
                        return mod;
                    }
                } catch (hash<ExceptionInfo> ex) {
                    stderr.printf("ERROR: error in QORE_DATA_PROVIDERS module %y registration function: %s: %s\n",
                        mod, ex.err, ex.desc);
                }
            }
        }
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
    /** @param label the label for the type; this is the lookup index
        @param type the new data provider type

        @throw PROVIDER-ERROR the given provider has already been registered

        @note types are stored based on labels which may differ from their type names (which may be generic and
        therefore not suitable for indexing)
    */
    static registerType(string label, AbstractDataProviderType type) {
        type_cache_lock.lock();
        on_exit type_cache_lock.unlock();

        if (type_cache{label}) {
            # check if type is being re-registered after module injection
            if (!DataProvider::checkInjection(type, type_module_map)) {
                throw "PROVIDER-ERROR", sprintf("provider type label %y has already been registered", label);
            }
        }
        {
            *string module_name = Class::getClass(type).getModuleName();
            if (exists module_name) {
                type_module_map{type.className()} = module_name;
            }
        }

        type_cache{label} = type;
    }

    #! Returns the given data provider type or @ref nothing if not present
    /** @param label the label of the data provider type to retrieve

        @note types are stored based on labels which may differ from their type names (which may be generic and
        therefore not suitable for indexing)
    */
    static *AbstractDataProviderType getType(string label) {
        return type_cache{label};
    }

    #! Returns the given data provider type or throws an exception if not present
    /** @param label the label of the data provider type to retrieve

        @return the given data provider; an exception is thrown if the data provide is unknown

        @throw PROVIDER-ERROR the given data provider type label is unknown

        @note types are stored based on labels which may differ from their type names (which may be generic and
        therefore not suitable for indexing)
    */
    static AbstractDataProviderType getTypeEx(string label) {
        *AbstractDataProviderType type = DataProvider::getType(label);
        if (!type) {
            throw "PROVIDER-ERROR", sprintf("data provider type label %y is unknown; known data provider type "
                "labels: %y", label, keys type_cache);
        }
        return type;
    }

    #! Returns a list of registered data provider type labels
    /**
        @note types are stored based on labels which may differ from their type names (which may be generic and
        therefore not suitable for indexing)
    */
    static *list<string> listTypes() {
        return keys type_cache;
    }

    #! Registers all known data provider factories
    static registerKnownFactories() {
        map DataProvider::tryLoad($1), FactoryModuleList;
    }

    static bool tryLoad(string module_str, *bool verbose) {
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

    #! Converts an info hash with objects to a hash with string descriptions instead of the objects
    static hash<auto> getInfoAsData(hash<auto> info0) {
        # strip types
        hash info = info0;
        foreach hash<auto> i0 in (info.pairIterator()) {
            if (i0.value.typeCode() == NT_HASH && i0.key =~ /_options/) {
                foreach hash<auto> i1 in (i0.value.pairIterator()) {
                    if (i1.value.type) {
                        info{i0.key}{i1.key}.type = map $1.getName(), i1.value.type;
                    }
                }
            }
        }
        return info;
    }

    #! Check if the object is from a module that has been subject to dependency injections
    static private bool checkInjection(object obj, hash<string, string> module_map) {
        *string module_name = Class::getClass(obj).getModuleName();
        return module_name && module_map{obj.className()} == module_name;
    }
}
}
