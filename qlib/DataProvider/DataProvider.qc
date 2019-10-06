# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file DataProvider.qc module for performing bulk DML operations with SqlUtil

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
        static hash<string, AbstractDataProviderFactory> cache;

        #! data provider factory cache lock
        static Mutex cache_lock();
    }

    #! Register a new data provider factory
    /** @param factory the new data provider factory

        @throw PROVIDER-ERROR the given provider has already been registered
    */
    static register(AbstractDataProviderFactory factory) {
        cache_lock.lock();
        on_exit cache_lock.unlock();

        string name = factory.getName();
        if (cache{name}) {
            throw "PROVIDER-ERROR", sprintf("provider %y has already been registered", name);
        }

        cache{name} = factory;
    }

    #! Returns the given data provider factory or @ref nothing if not present
    static *AbstractDataProviderFactory get(string name) {
        return cache{name};
    }

    #! Returns the given data provider factory or throws an exception if not present
    /** @param the name of the data provider to retrieve

        @return the given data provider; an exception is thrown if the data provide is unknown

        @throw PROVIDER-ERROR the given provider is unknown
    */
    static AbstractDataProviderFactory getEx(string name) {
        *AbstractDataProviderFactory factory = cache{name};
        if (!factory) {
            throw "PROVIDER-ERROR", sprintf("data provider factory %y is unknown; known data provider factories: %y", name,
                keys cache);
        }
        return factory;
    }

    #! Returns a data provider object from the given factory, created with the given constructor options
    /**
        @throw PROVIDER-ERROR the given provider factory or child provider is unknown
    */
    static AbstractDataProvider getObject(string name, *hash<auto> options) {
        AbstractDataProvider provider;
        {
            list<string> path = name.split("/");
            provider = DataProvider::getEx(shift path).create(options);
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
    static AbstractDataProvider getObjectFromString(string name) {
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

        return DataProvider::getObject(name, options);
    }

    #! Returns a list of registered data provider factories
    static *list<string> listTypes() {
        return keys cache;
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
}
}
