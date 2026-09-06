# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore MongoDbConnection class definition

/** MongoDbConnection.qc Copyright 2026 Qore Technologies, s.r.o.

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

public namespace MongoDbDataProvider {
#! MongoDB connection class
/** Provides connection support for MongoDB servers using the mongodb:// URL scheme.
*/
public class MongoDbConnection inherits ConnectionProvider::AbstractConnectionWithInfo {
    public {
        #! Connection scheme information
        const ConnectionScheme = <ConnectionSchemeInfo>{
            "display_name": "MongoDB Connection",
            "short_desc": "A connection to a MongoDB server or an Atlas cluster",
            "desc": "A connection to a [MongoDB](https://www.mongodb.com) server or a "
                "[MongoDB Atlas](https://www.mongodb.com/atlas) cluster, for document storage, "
                "retrieval, aggregation and Atlas Vector Search.\n\n"
                "Use a `mongodb://` URL for a server or replica set whose hosts are known, and a "
                "`mongodb+srv://` URL for Atlas, where the hosts are discovered from DNS and TLS is "
                "implied.  An Atlas connection string looks like "
                "`mongodb+srv://cluster0.abcde.mongodb.net`.\n\n"
                "Supply the credentials as the `username` and `password` options rather than in the "
                "URL: they are then stored as connection options - the password as a password - and "
                "are URL-encoded when the client is created, so a password containing `@`, `:` or "
                "`/` works without the caller having to encode it.",
            "cls": Class::forName("MongoDbDataProvider::MongoDbConnection"),
            "schemes": {
                "mongodb": True,
                MongoDbConnection::SrvScheme: True,
            },
            "base_scheme_map": {
                "tcp": "mongodb",
            },
            "options": ConnectionOptions,
        };

        #! The URL scheme for an Atlas (DNS seed list) cluster
        /** MongoDB spells this scheme with a \c "+", which is legal in a URL scheme and which
            @ref Qore::parse_url() "parse_url()" parses correctly.
        */
        const SrvScheme = "mongodb+srv";

        #! Connection options
        const ConnectionOptions = {
            "username": <ConnectionOptionInfo>{
                "display_name": "Username",
                "short_desc": "The database user to authenticate as",
                "type": "string",
                "desc": "The database user to authenticate as.\n\n"
                    "For Atlas this is a database user, created under **Database Access** in the "
                    "Atlas UI - not the Atlas account itself.\n\n"
                    "Given here rather than in the URL, the value is URL-encoded when the client is "
                    "created, so it needs no encoding of its own.",
            },
            "password": <ConnectionOptionInfo>{
                "display_name": "Password",
                "short_desc": "The password for the database user",
                "type": "string",
                "desc": "The password for the database user.\n\n"
                    "Given here rather than in the URL, it is stored as a password and is "
                    "URL-encoded when the client is created, so a password containing `@`, `:`, `/` "
                    "or `?` works without the caller having to encode it.",
                "sensitive": True,
            },
            "database": <ConnectionOptionInfo>{
                "display_name": "Default Database",
                "short_desc": "The database used when an action does not name one",
                "type": "string",
                "desc": "The database used when an action does not name one, and the default "
                    "authentication database when `authsource` is not set.",
            },
            "authsource": <ConnectionOptionInfo>{
                "display_name": "Authentication Database",
                "short_desc": "The database holding the user's credentials",
                "type": "string",
                "desc": "The database holding the user's credentials, sent as the `authSource` URI "
                    "option.\n\n"
                    "Atlas keeps database users in `admin`, which is also what the server assumes "
                    "when this is not set; set it only where the credentials live elsewhere.",
            },
            "appname": <ConnectionOptionInfo>{
                "display_name": "Application Name",
                "short_desc": "The name this client reports to the server",
                "type": "string",
                "desc": "The name this client reports to the server, sent as the `appName` URI "
                    "option.\n\n"
                    "Atlas shows it against every operation in the cluster's monitoring, which is "
                    "what makes one application's load distinguishable from another's.  Defaults to "
                    "`qore-mongodb` when neither this option nor the URL sets one.",
            },
            "tls": <ConnectionOptionInfo>{
                "display_name": "Use TLS",
                "short_desc": "Encrypt the connection with TLS",
                "type": "bool",
                "desc": "Encrypt the connection with TLS.\n\n"
                    "Atlas requires TLS and a `mongodb+srv://` URL implies it, so this is only "
                    "needed for a `mongodb://` server that requires TLS without advertising it.",
            },
            "connect_timeout": <ConnectionOptionInfo>{
                "display_name": "Connect Timeout",
                "short_desc": "How long to wait for a connection to be established",
                "type": "timeout",
                "desc": "How long to wait for a connection to be established, sent as the "
                    "`connectTimeoutMS` URI option.",
                "default_value": 10000,
            },
            "server_selection_timeout": <ConnectionOptionInfo>{
                "display_name": "Server Selection Timeout",
                "short_desc": "How long to wait for a suitable server to be found",
                "type": "timeout",
                "desc": "How long to wait for a suitable server to be found, sent as the "
                    "`serverSelectionTimeoutMS` URI option.\n\n"
                    "This is the timeout a connection to an unreachable cluster actually waits on, "
                    "because the driver selects a server before it connects to one.",
                "default_value": 30000,
            },
        };

        #! Maps a connection option to the MongoDB URI option carrying it
        /** Only options that map straight through appear here; the credentials and the timeouts
            need conversion and are handled separately.
        */
        const UriOptionMap = {
            "authsource": "authSource",
            "appname": "appName",
        };
    }

    #! Creates the connection object from a URL
    /** @param name the name of the connection
        @param desc a description of the connection
        @param url the URL to connect to
        @param attributes connection attributes
        @param options connection options
    */
    constructor(string name, string desc, string url, hash<auto> attributes = {}, hash<auto> options = {})
            : AbstractConnectionWithInfo(name, desc, url, attributes, options) {
    }

    #! Creates the connection object from a hash
    /** @param config a hash with connection configuration
        @param attr optional connection attributes
    */
    constructor(hash<auto> config, *hash<auto> attr) : AbstractConnectionWithInfo(config, attr) {
    }

    #! Returns the connection type
    string getType() {
        return "mongodb";
    }

    #! Returns the data provider application name this connection serves
    /** Without this the base class returns nothing and the connection is never associated with its
        application.

        @note must match @ref MongoDbDataProvider::AppName
    */
    *string getAppName() {
        return MongoDbDataProvider::AppName;
    }

    #! Returns True if the connection supports the data provider API
    bool hasDataProvider() {
        return True;
    }

    #! Returns a data provider for this connection
    /** @param constructor_options optional constructor options for the data provider
        @return a MongoDbDataProvider object
    */
    DataProvider::AbstractDataProvider getDataProvider(*hash<auto> constructor_options) {
        # the default database travels in the URI, which is where MongoDB itself reads it from;
        # the provider takes only the client
        return new MongoDbDataProvider({"client": get()} + constructor_options);
    }

    #! Returns the MongoDB URI this connection connects with
    /** The URL a user configures carries the cluster; the credentials and the driver settings are
        connection options, because a URL is not a place to put a password - Qorus stores and
        renders a password option as one, and a value pasted into a URL cannot be treated that way.

        Both sides are honoured: an option is only applied where the URL does not already say
        otherwise, so a URL that does carry credentials or query options keeps working unchanged.

        @return the URI the \c MongoClient object is created with

        @throw MONGODB-CONNECTION-ERROR a password was given without a username
    */
    string getUri() {
        hash<UrlInfo> h = parse_url(url);
        string scheme = h.protocol ?? "mongodb";

        # credentials from the options, unless the URL already carries them
        string userinfo;
        *string username = h.username ?? opts.username;
        *string password = h.password ?? opts.password;
        if (password.val() && !username.val()) {
            throw "MONGODB-CONNECTION-ERROR", sprintf("connection %y: a password was given with no "
                "username; MongoDB has no way to send one without the other", name);
        }
        if (username.val()) {
            # a password may legitimately contain "@", ":" and "/", which must be percent-encoded
            # here or the URI parses into something else entirely
            userinfo = encode_url(username, True)
                + (password.val() ? ":" + encode_url(password, True) : "")
                + "@";
        }

        # the host part: parse_url() reports the port separately, and an SRV URL has none
        string host = h.host ?? "";
        if (h.port) {
            host += sprintf(":%d", h.port);
        }

        # options already in the URL win, so that a URL written by hand is never overridden
        hash<string, string> query = getUrlQuery(h.path);
        hash<string, string> add = {};
        foreach hash<auto> i in (UriOptionMap.pairIterator()) {
            if (opts{i.key}.val() && !exists query{i.value}) {
                add{i.value} = opts{i.key};
            }
        }
        if (exists opts.tls && !exists query.tls) {
            add.tls = opts.tls ? "true" : "false";
        }
        # timeouts are given to the connection as timeouts and to MongoDB as milliseconds
        if (exists opts.connect_timeout && !exists query.connectTimeoutMS) {
            add.connectTimeoutMS = get_ms(opts.connect_timeout);
        }
        if (exists opts.server_selection_timeout && !exists query.serverSelectionTimeoutMS) {
            add.serverSelectionTimeoutMS = get_ms(opts.server_selection_timeout);
        }

        # the path component of a MongoDB URI is the default database; note that splitting an
        # empty string yields an empty list, so the element cannot be indexed unconditionally
        string path = ((h.path ?? "").split("?")[0] ?? "");
        if (!path.val() || path == "/") {
            path = opts.database.val() ? "/" + opts.database : "/";
        }

        query += add;
        string rv = sprintf("%s://%s%s%s", scheme, userinfo, host, path);
        if (query) {
            rv += "?" + (map sprintf("%s=%s", $1.key, encode_url($1.value, True)),
                query.pairIterator()).join("&");
        }
        return rv;
    }

    #! Returns the options already present in a URL's query string
    private static hash<string, string> getUrlQuery(*string path) {
        hash<string, string> rv = {};
        *string qs = (path ?? "") =~ x/\?(.*)$/[0];
        if (!qs.val()) {
            return rv;
        }
        foreach string pair in (qs.split("&")) {
            if (!pair.val()) {
                continue;
            }
            list<string> kv = pair.split("=");
            rv{decode_url(kv[0])} = kv.size() > 1 ? decode_url(kv[1]) : "";
        }
        return rv;
    }

    #! Returns the number of milliseconds in a timeout option
    private static string get_ms(auto value) {
        return value.typeCode() == NT_DATE
            ? sprintf("%d", cast<date>(value).durationMilliseconds())
            : sprintf("%d", value.toInt());
    }

    #! Returns the connection object
    /** @param connect if True, connects to the server, which authenticates the credentials
        @param rtopts runtime options
        @return a MongoClient object

        @throw MONGODB-CONNECTION-ERROR the server did not answer the ping
    */
    private MongoClient getImpl(bool connect = True, *hash<auto> rtopts) {
        MongoClient client(getUri());
        if (connect) {
            # the ping is a server command, so it fails for credentials the server rejects; this is
            # also what the connection ping runs, so a bad password cannot report a healthy
            # connection
            if (!client.ping()) {
                throw "MONGODB-CONNECTION-ERROR", "failed to ping MongoDB server";
            }
        }
        return client;
    }

    #! Returns the ConnectionSchemeInfo hash
    private hash<ConnectionSchemeInfo> getConnectionSchemeInfoImpl() {
        return ConnectionScheme;
    }
}
}
