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

#! Contains all public definitions in the DataProvider module
public namespace DataProvider {
#! Data provider class
public class DataProvider {
    public {
        #! List of standard data provider factory modules
        const FactoryList = (
            "DbDataProvider",
            "CsvUtil",
        );
    }

    private {
        #! data provider factory cache
        static hash<string, AbstractDataProviderFactory> factory_cache;

        #! data provider factory cache lock
        static Mutex factory_cache_lock();

        #! data provider data type cache
        static hash<string, AbstractDataProviderType> type_cache;

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
            throw "PROVIDER-ERROR", sprintf("provider %y has already been registered", name);
        }

        factory_cache{name} = factory;
    }

    #! Returns the given data provider factory or @ref nothing if not present
    static *AbstractDataProviderFactory getFactory(string name) {
        return factory_cache{name};
    }

    #! Returns the given data provider factory or throws an exception if not present
    /** @param name the name of the data provider to retrieve

        @return the given data provider; an exception is thrown if the data provide is unknown

        @throw PROVIDER-ERROR the given provider is unknown
    */
    static AbstractDataProviderFactory getFactoryEx(string name) {
        *AbstractDataProviderFactory factory = factory_cache{name};
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
            throw "PROVIDER-ERROR", sprintf("provider type label %y has already been registered", label);
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
        *AbstractDataProviderType type = type_cache{label};
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

    #! Registers known data provider factories
    static registerKnownFactories() {
        foreach string factory in (FactoryList) {
            try {
                load_module(factory);
            } catch (hash<ExceptionInfo> ex) {
                # DEBUG
                #stderr.printf("%s: %s\n", ex.err, ex.desc);
            }
        }
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
}
}
