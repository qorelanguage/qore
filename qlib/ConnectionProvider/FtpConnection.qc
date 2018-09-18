# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore HttpBasedConnection class definition

/*  HttpBasedConnection.qc Copyright 2016 - 2018 Qore Technologies, s.r.o.

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

%require-types
%enable-all-warnings
%new-style
%strict-args

#! the ConnectionProvider namespace. All classes used in the ConnectionProvider module should be inside this namespace
public namespace ConnectionProvider {
    #! base class for HTTP-based connections that need their URLs rewritten to create the real connection object
    public class HttpBasedConnection inherits AbstractConnection {
        public {
            string real_url;
        }

        constructor(string name, string desc, string url, bool monitor, *hash opts, hash urlh) : AbstractConnection(name, desc, url, monitor, opts, urlh) {
            # sets an HTTP-based URL for the URL argument
            string scheme = (url =~ x/^([^:]+)/)[0];
            bool ssl = (scheme =~ /s$/);
            string targ = "http" + (ssl ? "s" : "");
            real_url = regex_subst(url, "^" + scheme, targ);
        }
    } # HttpBasedConnection class

    #! class for FTP connections; returns @ref Qore::FtpClient objects
    /** supports the following runtime options in getImpl():
        - \c "path": overrides the path component in the URL at runtime
        - \c "path_add": appends the given string to the path component of the URL at runtime
    */
    public class FtpConnection inherits AbstractConnection {
        #! creates the FtpConnection object
        /** @param name the name of the connection
            @param desc connection description
            @param url connection URL (potentially with password info)
            @param monitor monitoring flag
            @param opts connection options
            @param urlh broken down URL hash (as returned by @ref Qore::parse_url())
        */
        constructor(string name, string desc, string url, bool monitor, *hash opts, hash urlh) : AbstractConnection(name, desc, url, monitor, opts, urlh) {
        }

        #! returns \c "ftp"
        string getType() {
            return "ftp";
        }

        #! returns runtime options
        /** return a hash with the following keys reflecting support for the corresponding runtime options in getImpl():
            - \c "path": overrides the path component in the URL at runtime
            - \c "path_add": appends the given string to the path component of the URL at runtime
        */
        *hash getRuntimeOptions() {
            return {
                "path": True,
                "path_add": True,
            };
        }

        #! returns an @ref Qore::FtpClient object
        /** @param connect if @ref Qore::True "True" then a connection is made any if there is any path in the URL a \c CWD operation is performed to change to the target directory
            @param rtopts this connection type does not accept any runtime option, so this parameter is ignored
        */
        private FtpClient getImpl(bool connect = True, *hash rtopts) {
            # set runtime path
            *string rtpath = rtopts.path ?* urlh.path;
            if (rtopts.path_add.val()) {
                if (rtpath !~ /\/$/ && rtopts.path_add !~ /^\//)
                    rtpath += "/";
                rtpath += rtopts.path_add;
            }

            # create the object and perform post processing if necessary
            FtpClient fc(url);
            if (connect) {
                fc.connect();
                if (rtpath.val()) {
                    fc.cwd(rtpath);
                }
            }
            return fc;
        }

        #! returns a hash that can be used to contruct the object dynamically
        /** @since %ConnectionProvider 1.1
        */
        private hash<ConnectionConstructorInfo> getConstructorInfoImpl() {
            string my_code = sprintf("if (connect) {obj.connect(); *string rtpath = rtopts.path ?* %s; if (rtpath.path_add.val()) {if (rtpath !~ /\\/$/ && rtopts.path_add !~ /^\\//) {rtpath += \"/\";} rtpath += rtopts.path_add;} if (rtpath.val()) {obj.cwd(rtpath);}}", urlh.path.val() ? sprintf("%y", urlh.path) : "NOTHING");

            return new hash<ConnectionConstructorInfo>({
                "class_name": "FtpClient",
                "args": url,
                "post_processing": my_code,
            });
        }

        #! static constructor
        static FtpConnection make(string name, string desc, string url, bool monitor, *hash opts, hash urlh) {
            return new FtpConnection(name, desc, url, monitor, opts, urlh);
        }
    } # FtpConnection class

    #! class for HTTP connections; returns @ref Qore::HTTPClient objects
    /** supports the following options:
        - \c "connect_timeout": connection timeout to use in milliseconds (default: 45 seconds)
        - \c "http_version": HTTP version to use (\c "1.0" or \c "1.1", defaults to \c "1.1")
        - \c "max_redirects": maximum redirects to support
        - \c "proxy": proxy URL to use
        - \c "ssl_cert_path": a path to an X.509 client certificate file in PEM format
        - \c "ssl_key_path": a path to a private key file in PEM format for the X.509 client certificate
        - \c "ssl_key_password": the password to the private key given with \c "ssl_key_path"
        - \c "ssl_verify_cert": if @ref Qore::True "True" then the server's certificate will only be accepted if it's verified
        - \c "timeout": transfer timeout to use in milliseconds (default: 45 seconds)

        @see @ref Qore::HTTPClient::constructor(hash) "HTTPClient::constructor(hash opts)" for more information on the above options
    */
    public class HttpConnection inherits AbstractConnection {
        public {
            #! HttpConnection object connection options
            const Options = (
                "http_version": True,
                "max_redirects": True,
                "proxy": True,
                "timeout": True,
                "connect_timeout": True,
                "ssl_cert_path": True,
                "ssl_key_password": True,
                "ssl_key_path": True,
                "ssl_verify_cert": True,
                );

            #! HttpConnection default options
            const DefaultOptions = (
                "timeout": 45s,
                "connect_timeout": 45s,
                );

            const OptionList = Options.keys();
        }

        #! creates the HttpConnection object
        /** @param name the name of the connection
            @param desc connection description
            @param url connection URL (potentially with password info)
            @param monitor monitoring flag
            @param opts connection options (@see getOptions() for valid options)
            @param urlh broken down URL hash (as returned by @ref Qore::parse_url())
        */
        constructor(string name, string desc, string url, bool monitor, *hash opts, hash urlh) : AbstractConnection(name, desc, url, monitor, opts, urlh) {
        }

        #! returns an @ref Qore::HTTPClient object
        /** @param connect if @ref Qore::True "True" then a connection is made by calling @ref Qore::HTTPClient::connect() before returning the object
            @param rtopts this connection type does not accept any runtime option, so this parameter is ignored
        */
        private Qore::HTTPClient getImpl(bool connect = True, *hash rtopts) {
            HTTPClient sc(("url": url) + opts);
            if (connect)
                sc.connect();

            return sc;
        }

        #! returns a hash that can be used to contruct the object dynamically
        /** @since %ConnectionProvider 1.1
        */
        private hash<ConnectionConstructorInfo> getConstructorInfoImpl() {
            string my_code = "if (connect) {obj.connect();}";

            return new hash<ConnectionConstructorInfo>((
                "class_name": "HTTPClient",
                "args": {"url": url} + opts,
                "post_processing": my_code,
            ));
        }

        #! gets options
        /** @return returns a hash with the following supported options:
            - \c "connect_timeout": connection timeout to use in milliseconds
            - \c "http_version": HTTP version to use (\c "1.0" or \c "1.1", defaults to \c "1.1")
            - \c "max_redirects": maximum redirects to support
            - \c "proxy": proxy URL to use
            - \c "ssl_cert_path": a path to an X.509 client certificate file in PEM format
            - \c "ssl_key_path": a path to a private key file in PEM format for the X.509 client certificate
            - \c "ssl_key_password": the password to the private key given with \c "ssl_key_path"
            - \c "ssl_verify_cert": if @ref Qore::True "True" then the server's certificate will only be accepted if it's verified
            - \c "timeout": transfer timeout to use in milliseconds

            @see @ref Qore::HTTPClient::constructor(hash) "HTTPClient::constructor(hash opts)" for more information on the above options
        */
        hash getOptions() {
            return Options;
        }

        #! returns default options
        /** @return default options as follows:
            - \c "connect_timeout": (@ref relative_dates "relative date/time value") 45 seconds
            - \c "timeout": (@ref relative_dates "relative date/time value") 45 seconds
        */
        *hash getDefaultOptions() {
            return DefaultOptions;
        }

        #! returns \c "http"
        string getType() {
            return "http";
        }

        #! static constructor
        static HttpConnection make(string name, string desc, string url, bool monitor, *hash opts, hash urlh) {
            return new HttpConnection(name, desc, url, monitor, opts, urlh);
        }
    } # HttpConnection class

    #! class for filesystem connections; returns an object of type @ref Qore::Dir; this can be used to monitor filesystem free space or to ensure a network filesystem is mounted, for example
    /** supports the following option:
        - \c "readonly": does not mark the connection as down if the directory is read-only; use for example if the connection should point to a read-only base directory; interface directories should normally be writable
    */
    public class FilesystemConnection inherits AbstractConnection {
        public {
            #! FilesystemConnection object connection options
            const Options = (
                "readonly": True,
                );

            #! FilesystemConnection object connection option list
            const OptionList = Options.keys();
        }

        #! creates the FilesystemConnection object
        /** @param name the name of the connection
            @param desc connection description
            @param url connection URL
            @param monitor monitoring flag
            @param opts connection options
            @param n_urlh broken down URL hash (as returned by @ref Qore::parse_url())
         */
        constructor(string name, string desc, string url, bool monitor, *hash opts, hash n_urlh) : AbstractConnection(name, desc, url, monitor, opts, n_urlh) {
            # the URL hash is parsed with parse_url() which will put a string in the
            # "host" key if it does not look like an absolute path, in which case "host"
            # needs to be prepended back to the path
            if (urlh.host)
                urlh.path = (remove urlh.host) + urlh.path;

            if (!absolute_path(urlh.path)) {
%ifdef Windows
                # here we have to guess where the "root" is
                urlh.path = "c:\\" + urlh.path;
%else
                urlh.path = "/" + urlh.path;
%endif
            }

            # sanitize / normalize the path
            urlh.path = normalize_dir(urlh.path);
        }

        #! returns \c "dir"
        string getType() {
            return "dir";
        }

        #! returns a @ref Qore::Dir object
        /** @param connect if @ref Qore::True "True", then the target directory is checked for existence and if it's writable, if either check fails, an exception is thrown
            @param rtopts this connection type does not accept any runtime option, so this parameter is ignored

            @return a @ref Qore::Dir object
         */
        private Dir getImpl(bool connect = True, *hash rtopts) {
            Dir dir();
            if (urlh.path.val()) {
                dir.chdir(urlh.path);
            }
            if (connect) {
                if (!opts.readonly && (!is_writeable(urlh.path) || !dir.exists()))
                    throw "CONNECTION-ERROR", sprintf("connection %y (type %y url %y) points to non-existent, inaccessible, or read-only directory %y", name, urlh.protocol, url, urlh.path);
            }
            return dir;
        }

        #! returns a hash that can be used to contruct the object dynamically
        /** @since %ConnectionProvider 1.1
        */
        private hash<ConnectionConstructorInfo> getConstructorInfoImpl() {
            string my_code;
            if (urlh.path.val()) {
                my_code = sprintf("obj.chdir(%y); ", urlh.path);
            }
            if (!opts.readonly) {
                string err_msg = sprintf("connection %y (type %y url %y) points to non-existent, inaccessible, or read-only directory %y", name, urlh.protocol, url, urlh.path);
                my_code += sprintf("if (connect && (!is_writeable(%y) || !obj.exists())) {throw \"CONNECTION-ERROR\", %y;};", urlh.path, err_msg);
            }

            return new hash<ConnectionConstructorInfo>((
                "class_name": "Dir",
                "post_processing": my_code,
            ));
        }

        #! gets options
        /** @return returns a hash with the following supported option:
        - \c "readonly": if @ref Qore::True "True", does not mark the connection as down if the directory is read-only; use for example if the connection should point to a read-only base directory; interface directories should normally be writable (default: @ref Qore::False "False")
         */
        hash getOptions() {
            return Options;
        }

        #! static constructor
        static FilesystemConnection make(string name, string desc, string url, bool monitor, *hash opts, hash urlh) {
            return new FilesystemConnection(name, desc, url, monitor, opts, urlh);
        }
    } # FilesystemConnection class
} # ConnectionProvider namespace
