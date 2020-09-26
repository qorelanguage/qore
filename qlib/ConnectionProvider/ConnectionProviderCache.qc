# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file ConnectionProviderCache.qc module for data access and introspection

/** ConnectionProviderCache.qc Copyright 2019 - 2020 Qore Technologies, s.r.o.

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
#%requires Util

#! Contains all public definitions in the ConnectionProvider module
public namespace ConnectionProvider {
#! Connection option information
public hashdecl ConnectionOptionInfo {
    #! Option value type
    string type;

    #! Description
    string desc;

    #! Is the value sensitive? (password or other authentication info)
    bool sensitive = False;

    #! List of allowed values (enum)
    *list<auto> allowed_values;

    #! Any default value
    auto default_value;
}

#! Connection provider information
public hashdecl ConnectionProviderEntry {
    #! The connection class
    Class cls;

    #! Connection options: name -> option info
    *hash<string, hash<ConnectionOptionInfo>> options;

    #! Required option string
    /** format is required option names separated by commas (\c ","); if there are multiple possibilities, they should
        be separated by pipe chars (\c "|""); ex: client_id,client_secret,tenant|token

        All strings must be valid option names in \a options
    */
    *string required_options;
}

#! Connection provider type cache class
public class ConnectionProviderCache {
    public {
        #! Map of known connection schemes to modules
        const SchemeMap = {
            "rest": "RestClient",
            "rests": "RestClient",

            "billwerkrest": "BillwerkRestClient",
            "billwerkrests": "BillwerkRestClient",

            "sewiorest": "SewioRestClient",
            "sewiorests": "SewioRestClient",

            "sfrests": "SalesforceRestClient",

            "sap4hanarests": "Sap4HanaRestClient",

            "awsrests": "AwsRestClient",

            "cdsrests": "RestClient",

            "zeyosrest": "ZeyosRestClient",
            "zeyosrests": "ZeyosRestClient",

            "smtp": "SmtpClient",
            "smtps": "SmtpClient",
            "smtptls": "SmtpClient",
            "esmtp": "SmtpClient",
            "esmtps": "SmtpClient",
            "esmtptls": "SmtpClient",

            "pop3": "Pop3Client",
            "pop3s": "Pop3Client",

            "telnet": "TelnetClient",

            "ws": "WebSocketClient",
            "wss": "WebSocketClient",

            "sewiows": "SewioWebSocketClient",
            "sewiowss": "SewioWebSocketClient",

            "soap": "SoapClient",
            "soaps": "SoapClient",

            "sfsoap": "SalesforceSoapClient",
            "sfsoaps": "SalesforceSoapClient",

            "sftp": "Ssh2Connections",

            "ssh": "Ssh2Connections",

            "jsonrpc": "JsonRpcClient",
            "jsonrpcs": "JsonRpcClient",

            "xmlrpc": "XmlRpcClient",
            "xmlrpcs": "XmlRpcClient",

            "yamlrpc": "YamlRpcClient",
            "yamlrpcs": "YamlRpcClient",
        };
    }

    private {
        #! Connection provider cache
        static hash<string, hash<ConnectionProviderEntry>> cache = {
            "ftp": FtpConnection::ConnectionEntry,
            "ftps": FtpConnection::ConnectionEntry,

            "dir": FilesystemConnection::ConnectionEntry,
            "file": FilesystemConnection::ConnectionEntry,

            "http": HttpConnection::ConnectionEntry,
            "https": HttpConnection::ConnectionEntry,
        };

        #! Connection provider cache lock
        static Mutex lck();
    }

    #! Register a new connection provider in the cache
    /** @param scheme the scheme for the connection provider
        @param entry the new connection provider entry

        @throw PROVIDER-ERROR the given provider has already been registered
    */
    static registerConnection(string scheme, hash<ConnectionProviderEntry> entry) {
        bool needs_lock = !lck.lockOwner();
        if (needs_lock) {
            lck.lock();
        }
        on_exit if (needs_lock) {
            lck.unlock();
        }

        if (cache{scheme}) {
            if (cache{scheme} == entry) {
                return;
            }
            throw "PROVIDER-ERROR", sprintf("scheme %y is already registered with class %y (conflicting class %y)",
                scheme, cache{scheme}.cls.getPathName(), entry.cls.getPathName());
        }

        cache{scheme} = entry;
    }

    #! Register or replaces a connection provider in the cache
    /** @param scheme the scheme for the connection provider
        @param entry the new connection provider entry

        @return True if the entry was replaced or added
    */
    static bool registerOrReplaceConnection(string scheme, hash<ConnectionProviderEntry> entry) {
        bool needs_lock = !lck.lockOwner();
        if (needs_lock) {
            lck.lock();
        }
        on_exit if (needs_lock) {
            lck.unlock();
        }

        *hash<ConnectionProviderEntry> old_entry = cache{scheme};
        cache{scheme} = entry;
        return old_entry != entry;
    }

    #! Returns the entire cache
    static hash<string, hash<ConnectionProviderEntry>> get() {
        return cache;
    }

    #! Returns the number of types in the cache
    static int size() {
        return cache.size();
    }

    #! Returns True if the cache is empty, False if not
    static bool empty() {
        return cache.empty();
    }

    #! Returns a list of all cached schemes
    /**
    */
    static *list<string> listConnections() {
        return keys cache;
    }

    #! Returns the given connection provider entry or @ref nothing if not present
    /** @param scheme the scheme for the connection provider

        @return the connection provider entry for the given scheme; @ref NOTHING is returned if the scheme is not
        registered
    */
    static *hash<ConnectionProviderEntry> getConnection(string scheme) {
        return ConnectionProviderCache::getConnectionImpl(scheme, False);
    }

    #! Returns the given connection provider or throws an exception if not present
    /** @param scheme the scheme for the connection provider

        @return connection provider entry for the given scheme; an exception is thrown if the scheme is unknown

        @throw PROVIDER-ERROR the scheme is unknown

        @note types are stored based on the path provided which may differ from their type names (which may be generic
        and therefore not suitable for indexing)
    */
    static hash<ConnectionProviderEntry> getConnectionEx(string scheme) {
        return ConnectionProviderCache::getConnectionImpl(scheme, True);
    }

    #! Returns the given connection provider and either throws an exception if not present or returns @ref NOTHING
    /** @param scheme the scheme for the connection provider
        @param throw_exception if @ref True then an exception is thrown if the scheme cannot be resolved, otherwise
        @ref NOTHING is returned
        @param connection_loader a closure taking a string scheme argument and returning
        <tt>*hash<ConnectionProviderEntry></tt>

        @return connection provider entry for the given scheme or @ref NOTHING is returned if the scheme cannot be
        resolved and the \a throw_exception argument is @ref False

        @throw PROVIDER-ERROR the scheme is unknown; only thrown if the \a throw_exception argument is @ref True
    */
    static *hash<ConnectionProviderEntry> getConnectionImpl(string scheme, bool throw_exception, *code connection_loader) {
        # we have to grab the write lock if there is a connection_loader
        bool needs_lock = !lck.lockOwner();
        if (needs_lock) {
            lck.lock();
        }
        on_exit if (needs_lock) {
            lck.unlock();
        }

        *hash<ConnectionProviderEntry> rv = cache{scheme};
        if (!rv) {
            if (*string mod = SchemeMap{scheme}) {
                if (ConnectionProviderCache::tryLoadModule(mod, throw_exception)) {
                    rv = cache{scheme};
                }
            }
            if (!rv && connection_loader) {
                rv = cache{scheme} = connection_loader(scheme);
            }
        }

        if (!rv && throw_exception) {
            throw "PROVIDER-ERROR", sprintf("connection provider scheme %y is not registered", scheme);
        }
        return rv;
    }

    #! Returns the lock to allow for external serialization
    static Mutex getLock() {
        return lck;
    }

    #! Try to load the given module
    private static bool tryLoadModule(string mod, bool throw_exception) {
        try {
            load_module(mod);
            return True;
        } catch (hash<ExceptionInfo> ex) {
            if (throw_exception) {
                rethrow;
            }
        }
        return False;
    }
}
}