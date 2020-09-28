# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore AbstractConnection class definition

/*  AbstractConnection.qc Copyright 2016 - 2020 Qore Technologies, s.r.o.

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

#! the ConnectionProvider namespace. All classes used in the ConnectionProvider module should be inside this namespace
public namespace ConnectionProvider {
#! abstract base class for connections
/** this class can be specialized to provide for user-defined connection types

    @deprecated use AbstractConnectionWithInfo instead
*/
public class AbstractConnection inherits Serializable {
    public {
        #! connection name
        string name;

        #! connection description
        string desc;

        #! connection URL (potentially with password info)
        string url;

        #! "safe" URL (password information removed)
        string safe_url;

        #! original connection options
        *hash<auto> orig_opts;

        #! connection options
        *hash<auto> opts;

        #! broken down URL hash (as returned by @ref Qore::parse_url())
        hash<auto> urlh;

        #! date/time of last check/ping
        date last_check;

        #! status string; in case of a connection error this string will be the error string
        string status = "not checked";

        #! connection status flag; set by monitoring or explicit pings/connections
        bool up = False;

        #! monitoring flag
        bool monitor;

        #! date/time of last update
        date updated;

        #! set to @ref True "True" for loopback connections
        bool loopback = False;

        #! optional code for default post-processing of objects created by getImpl(); must take the connection object and the object created as arguments
        static *code post_processing;

        #! enabled/disabled indicator flag
        bool enabled = True;

        #! locked/unlocked flag
        bool locked = False;

        #! debug data flag; to be used an an indication for a higher-level implementation
        bool debug_data = False;

        #! connection tags; user-defined key-value pairs
        hash<auto> tags = {};

        #! internal tags
        hash<auto> internal_info;
    }

    #! creates the AbstractConnection object
    /** @param n_name the name of the connection
        @param n_desc connection description
        @param n_url connection URL (potentially with password info)
        @param n_monitor monitoring flag
        @param n_opts connection options
        @param n_urlh broken down URL hash (as returned by @ref Qore::parse_url())
        @param n_safe_url "safe" URL (password information removed); if not set this will be set automatically by calling @ref getSafeUrl()
        @param n_enabled enabled/disabled flag
    */
    deprecated
    constructor(string n_name,
                string n_desc,
                string n_url,
                bool n_monitor,
                *hash n_opts,
                hash n_urlh,
                *string n_safe_url,
                *bool n_enabled) {
        # remove "enabled" option from option hash, if any
        *bool opt_enabled = remove n_opts.enabled;
        hash<auto> attributes = {
            "monitor": n_monitor,
            "enabled": n_enabled ?? opt_enabled ?? True,
        };
        constructorInit(n_name, n_desc, n_url, attributes, n_opts ?? {});
    }

    #! creates the AbstractConnection object
    /** @param name the name of the connection
        @param description connection description
        @param url connection URL (potentially with password info)
        @param attributes various attributes; see below
        @param options connection options

        @par Attributes
        Attributes are special flags for given connection - stored as public
        attributes of the object instance.
        For now there are:
            - \c enabled (bool), default \c True
            - \c monitor (bool), default \c True
            - \c locked (bool), default \c False
            - \c tags (hash), no default value

        @throw CONNECTION-OPTION-ERROR missing or invalid connection option
    */
    constructor(string name, string description, string url, hash<auto> attributes = {}, hash<auto> options = {}) {
        constructorInit(name, description, url, attributes, options);
    }

    #! returns a hash with the results of the ping operation
    /** @param throw_exception throw an exception if a connection error occurs

        @return a @ref PingInfo hash
    */
    hash<PingInfo> ping(bool throw_exception = False) {
        hash<PingInfo> rv(hash(self).("name", "desc", "opts"));
        rv.url = safe_url;

        last_check = now_us();

        if (throw_exception)
            return pingIntern(rv);

        try {
            return pingIntern(rv);
        } catch (hash<ExceptionInfo> ex) {
            return rv += {
                "ok": False,
                "info": sprintf("%s: %s", ex.err, ex.desc),
                "ex": ex,
            };
        }
    }

    #! performs the ping by executing get(); returns a @ref PingInfo hash
    private hash<PingInfo> pingIntern(hash<PingInfo> rv) {
        date start = now_us();
        get();
        return rv += {
            "ok": True,
            "time": now_us() - start,
            "info": "OK",
        };
    }

    #! returns a @ref ConnectionInfo hash of information about the connection
    /** @param with_password determines if the password is returned or not

        @return a @ref ConnectionInfo hash of information about the connection
    */
    hash<ConnectionInfo> getInfo(bool with_password = False) {
        hash<ConnectionInfo> h = cast<hash<ConnectionInfo>>(self.("name", "desc", "url", "monitor", "last_check",
            "updated", "status", "up", "loopback", "enabled", "locked", "debug_data", "tags") + {
            "type": getType(),
            "url_hash": urlh,
            "has_provider": hasDataProvider(),
        });
        if (!with_password) {
            h.url = safe_url;
            delete h.url_hash.password;
        }
        h.opts = orig_opts;
        return h;
    }

    #! returns a @ref ConfigInfo hash of static configuration information about the connection
    /** @returns a @ref ConfigInfo hash of static configuration information about the connection
    */
    hash<ConfigInfo> getConfigHash() {
        return cast<hash<ConfigInfo>>(self.("name", "desc", "url", "opts"));
    }

    #! Returns a default ConnectionSchemeInfo hash
    hash<ConnectionSchemeInfo> getConnectionSchemeInfo() {
        hash<string, hash<ConnectionOptionInfo>> options;
        *hash<auto> opt = getOptions();
        if (opt) {
            *hash<auto> def = getDefaultOptions();
            options = map {
                $1.key: <ConnectionOptionInfo>{
                    "type": "any",
                    "desc": "n/a; update connection to use the `AbstractConnectionWithInfo` class instead",
                    "default_value": def{$1.key},
                },
            }, opt.pairIterator();
        }
        return <ConnectionSchemeInfo>{
            "cls": Class::getClass(self),
            "options": options,
        };
    }

    #! returns static / initialization options
    *hash<auto> getOptions() {
    }

    #! returns runtime options
    *hash<auto> getRuntimeOptions() {
    }

    #! returns default options
    *hash<auto> getDefaultOptions() {
    }

    #! returns options for saving the connection's configuration information
    *hash<auto> getRealOptions() {
        return orig_opts;
    }

    #! returns the underlying connection object
    /** calls @ref getImpl() to actually acquire the connection object
    */
    object get(bool connect = True, *hash<auto> rtopts) {
        if (!connect) {
            object o = getImpl(False, rtopts);
            if (post_processing)
                post_processing(self, o);
            return o;
        }

        on_exit
            last_check = now_us();

        try {
            object o = getImpl(True, rtopts);
            if (post_processing)
                post_processing(self, o);
            up = True;
            status = "OK";
            return o;
        } catch (hash<ExceptionInfo> ex) {
            status = sprintf("%s: %s", ex.err, ex.desc);
            up = False;
            rethrow;
        }
    }

    #! this method is called when parsing connection file definitions from text files when loading into the database
    /** reimplement this method in subclasses to parse the option hash as set from a text file if needed (for special option formats, etc)
    */
    parseTextOptions() {
    }

    #! returns all tags associated with the connection
    hash<auto> getTags() {
        return tags;
    }

    #! returns the value of the given tag or @ref nothing if not present
    auto getTag(string tag) {
        return tags{tag};
    }

    #! returns the value of the given tag or throws an exception if the tag is not set with a value
    /** @throw TAG-ERROR the given tag is not set on the connection
    */
    auto getTagEx(string tag) {
        auto rv = tags{tag};
        if (!exists rv) {
            throw "TAG-ERROR", sprintf("tag %y is not set on connection %y", tag, name);
        }
        return rv;
    }

    #! returns a data provider object for this connection, if supported
    /** @return a data provider object for this connection, if supported

        @throw DATA-PROVIDER-ERROR this object does not support the data provider API

        This base class method throws a \c DATA-PROVIDER-ERROR exception by default; to support the data provider
        API, override this method in child classes

        @see hasDataProvider()

        @since ConnectionProvider 1.3
    */
    AbstractDataProvider getDataProvider() {
        throw "DATA-PROVIDER-ERROR", sprintf("%y connections do not support the data provider API", getType());
    }

    #! returns @ref True if the connection returns a data provider with the @ref getDataProvider() method
    /** @return @ref True if the connection returns a data provider with the @ref getDataProvider() method

        This base class method returns @ref False; to support the data provider API, override this method in child
        classes

        @see @ref getDataProvider()

        @since ConnectionProvider 1.3
    */
    bool hasDataProvider() {
        return False;
    }

    #! Parse the URL to a hash
    /** @param url a string with url
        @return hash untyped, depends on the parsing method

        The base implementation calls @ref Qore::parse_url().

        Reimplement \c parseUrl() if your connection uses different URL scheme.
        Like eg. Qore::Datasource connection string.
    */
    private hash<UrlInfo> parseUrl(string url) {
        return Qore::parse_url(url);
    }

    #! creates a "safe" URL string with password information removed
    /** @param urlh a parsed hash (as returned from parseUrl()
        @return string with safe URL

        This base/default implementation expects the \c urlh
        as returned by @ref Qore::parse_url())

        Public attribute \c safe_url is created from the \c url calling
        AbstractConnection::getSafeUrl(). Reimplement this method if your
        connection does not follow standard URL in form:
        @code
scheme://user:pass@hostname:port/path
        @endcode
    */
    private string getSafeUrl(hash<auto> urlh) {
        string url = urlh.protocol + "://";
        if (urlh.username)
            url += urlh.username + "@";
        url += urlh.host;
        if (urlh.port)
            url += ":" + urlh.port;
        if (urlh.path && urlh.path != "/")
            url += urlh.path;
        return url;
    }

    #! performs options validation in the constructor
    /** @param options a hash with options from the constructor
        @return validated option hash

        This method can be overloaded in connections where are options created dynamically for example

        @throw CONNECTION-OPTION-ERROR missing or invalid connection option
    */
    private hash<auto> validateOptions(hash<auto> options) {
        hash<ConnectionSchemeInfo> scheme_info = getConnectionSchemeInfo();
        options = getDefaultOptions() + options;
        if (options) {
            # check for unknown options
            *hash<auto> unknown_opts = options - (keys scheme_info.options);
            if (unknown_opts) {
                throw "CONNECTION-ERROR", sprintf("connection %y (type %y url %y) has unknown option%s %y (known "
                    "options: %y)", name, urlh.protocol, url, unknown_opts.size() == 1 ? "" : "s", unknown_opts,
                    keys scheme_info.options);
            }
            foreach hash<auto> i in (options.pairIterator()) {
                if (!exists i.value) {
                    continue;
                }
                string type = scheme_info.options{i.key}.type;
                if (type == "int") {
                    type = Type::Int;
                }
                string vtype = i.value.type();
                if (type != "any" && vtype != type) {
                    if (type == Type::Int && i.value.intp()) {
                        options{i.key} = i.value.toInt();
                        continue;
                    }
                    if (type == "bool" && vtype == "string") {
                        options{i.key} = parse_boolean(i.value);
                        continue;
                    }
                    throw "CONNECTION-OPTION-ERROR",
                        sprintf("connection %y (type %y url %y) option %y was passed with type %s (%y); expecting %y",
                            name, urlh.protocol, url, i.key, i.value.type(), i.value, type);
                }
            }
        }
        # check for required options
        if (scheme_info.required_options) {
            list<string> req_list = scheme_info.required_options.split("|");
            bool ok;
            foreach string req in (req_list) {
                list<string> req_opt = req.split(",");
                # check which options are missing
                *hash<auto> have = options{req_opt};
                if (have.size() == req_opt.size()) {
                    ok = True;
                    break;
                }
            }
            if (!ok) {
                throw "CONNECTION-OPTION-ERROR", sprintf("missing required options %y; options provided: %y",
                    scheme_info.required_options, options);
            }
        }
        return options;
    }

    #! Common construtor implementation
    /**
        @throw CONNECTION-OPTION-ERROR missing or invalid connection option
    */
    private:internal constructorInit(string name, string description, string url, hash<auto> attributes, hash<auto> options) {
        # metadata
        self.name = name;
        desc = description;

        # url handling
        self.url = url;
        urlh = parseUrl(self.url);
        safe_url = getSafeUrl(self.urlh);

        # attributes
        monitor = attributes.monitor ?? True;
        enabled = attributes.enabled ?? True;
        locked = attributes.locked ?? False;
        debug_data = attributes.debug_data ?? False;
        tags = attributes.tags ?? {};

        opts = validateOptions(options);
        orig_opts = opts;
    }

    #! returns the string type name for the connection object
    abstract string getType();

    #! this method must return the connection object corresponding to the object's configuration
    /** @param connect if @ref True "True" then the connection should be returned connected (and if possible a round-trip "ping" of the server should be made)
        @param rtopts if the object acquisition requires any runtime options, then these are passed here

        @return the connection object
    */
    private abstract object getImpl(bool connect = True, *hash<auto> rtopts);
}
} # ConnectionProvider namespace
