# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file ConnectionSchemeCache.qc module for data access and introspection

/** ConnectionSchemeCache.qc Copyright 2019 - 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the ConnectionProvider module
public namespace ConnectionProvider {
#! Allowed value hash
public hashdecl AllowedValueInfo {
    #! The value
    auto value;

    #! Description for the value
    string desc;
}

#! Connection option information
public hashdecl ConnectionOptionInfo {
    #! Option value type
    string type;

    #! Description
    string desc;

    #! Is the value sensitive? (password or other authentication info)
    bool sensitive = False;

    #! List of allowed values (enum)
    *softlist<hash<AllowedValueInfo>> allowed_values;

    #! Any default value
    auto default_value;

    #! Do environment variable substitution on the value?
    bool subst_env_vars = False;

    #! Modifier for the "file-as-string" type; can a freeform location option be returned?
    bool freeform = False;
}

#! Connection provider information
public hashdecl ConnectionSchemeInfo {
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
public class ConnectionSchemeCache {
    public {
        #! Map of known connection schemes to modules
        const SchemeMap = {
            "awsrests": "AwsRestClient",

            "billwerkrest": "BillwerkRestClient",
            "billwerkrests": "BillwerkRestClient",

            "cdsrests": "CdsRestClient",

            "grests": "GoogleRestClient",

            "jsonrpc": "JsonRpcClient",
            "jsonrpcs": "JsonRpcClient",

            "pop3": "Pop3Client",
            "pop3s": "Pop3Client",

            "rest": "RestClient",
            "rests": "RestClient",

            "sewiorest": "SewioRestClient",
            "sewiorests": "SewioRestClient",

            "sfrests": "SalesforceRestClient",

            "sfsoap": "SalesforceSoapClient",
            "sfsoaps": "SalesforceSoapClient",

            "sap4hanarests": "Sap4HanaRestClient",

            "snrests": "ServiceNowRestClient",

            "sewiows": "SewioWebSocketClient",
            "sewiowss": "SewioWebSocketClient",

            "soap": "SoapClient",
            "soaps": "SoapClient",

            "sftp": "Ssh2Connections",

            "ssh": "Ssh2Connections",

            "smtp": "SmtpClient",
            "smtps": "SmtpClient",
            "smtptls": "SmtpClient",
            "esmtp": "SmtpClient",
            "esmtps": "SmtpClient",
            "esmtptls": "SmtpClient",

            "telnet": "TelnetClient",

            "ws": "WebSocketClient",
            "wss": "WebSocketClient",

            "xmlrpc": "XmlRpcClient",
            "xmlrpcs": "XmlRpcClient",

            "yamlrpc": "YamlRpcClient",
            "yamlrpcs": "YamlRpcClient",

            "zeyosrest": "ZeyosRestClient",
            "zeyosrests": "ZeyosRestClient",
        };
    }

    private {
        #! Connection provider cache
        static hash<string, hash<ConnectionSchemeInfo>> cache = {
            "ftp": FtpConnection::ConnectionScheme,
            "ftps": FtpConnection::ConnectionScheme,

            "dir": FilesystemConnection::ConnectionScheme,
            "file": FilesystemConnection::ConnectionScheme,

            "http": HttpConnection::ConnectionScheme,
            "https": HttpConnection::ConnectionScheme,
        };

        #! Connection provider cache lock
        static Mutex lck();
    }

    #! Register a new connection provider in the cache
    /** @param scheme the scheme for the connection provider
        @param entry the new connection provider entry

        @return True if the connection was reigstered, False if there is already a registered connection with this
        scheme
    */
    static bool registerScheme(string scheme, hash<ConnectionSchemeInfo> entry) {
        bool needs_lock = !lck.lockOwner();
        if (needs_lock) {
            lck.lock();
        }
        on_exit if (needs_lock) {
            lck.unlock();
        }

        if (cache{scheme}) {
            return False;
        }

        cache{scheme} = entry;
        return True;
    }

    #! Returns the entire cache
    static hash<string, hash<ConnectionSchemeInfo>> get() {
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
    static *list<string> listSchemes() {
        return keys cache;
    }

    #! Returns info for the given connection scheme or @ref nothing if not present
    /** @param scheme the scheme for the connection

        @return info for the given connection scheme; @ref NOTHING is returned if the scheme is not
        registered
    */
    static *hash<ConnectionSchemeInfo> getScheme(string scheme) {
        return ConnectionSchemeCache::getSchemeImpl(scheme, False);
    }

    #! Returns info for the given connection scheme or throws an exception if not present
    /** @param scheme the scheme for the connection provider

        @return info for the given connection scheme; an exception is thrown if the scheme is unknown

        @throw SCHEME-ERROR the scheme is unknown
    */
    static hash<ConnectionSchemeInfo> getSchemeEx(string scheme) {
        return ConnectionSchemeCache::getSchemeImpl(scheme, True);
    }

    #! Returns info for the given connection scheme; either throws an exception if not present or returns @ref NOTHING
    /** @param scheme the scheme for the connection provider
        @param throw_exception if @ref True then an exception is thrown if the scheme cannot be resolved, otherwise
        @ref NOTHING is returned
        @param scheme_loader a closure taking a string scheme argument and returning
        <tt>*hash<ConnectionSchemeInfo></tt>

        @return info for the given connection scheme or @ref NOTHING is returned if the scheme cannot be
        resolved and the \a throw_exception argument is @ref False

        @throw SCHEME-ERROR the scheme is unknown; only thrown if the \a throw_exception argument is @ref True
    */
    static *hash<ConnectionSchemeInfo> getSchemeImpl(string scheme, bool throw_exception, *code scheme_loader) {
        # we have to grab the write lock if there is a scheme_loader
        bool needs_lock = !lck.lockOwner();
        if (needs_lock) {
            lck.lock();
        }
        on_exit if (needs_lock) {
            lck.unlock();
        }

        *hash<ConnectionSchemeInfo> rv = cache{scheme};
        if (!rv) {
            if (*string mod = SchemeMap{scheme}) {
                if (ConnectionSchemeCache::tryLoadModule(mod, throw_exception)) {
                    rv = cache{scheme};
                }
            }
            if (!rv && scheme_loader) {
                rv = cache{scheme} = scheme_loader(scheme);
            }
        }

        if (!rv && throw_exception) {
            throw "SCHEME-ERROR", sprintf("connection provider scheme %y is not registered", scheme);
        }
        return rv;
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
