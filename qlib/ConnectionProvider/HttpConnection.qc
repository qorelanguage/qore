# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore HttpConnection class definition

/*  HttpConnection.qc Copyright 2016 - 2019 Qore Technologies, s.r.o.

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
    #! class for HTTP connections; returns @ref Qore::HTTPClient objects
    /** supports the following options:
        - \c "connect_timeout": connection timeout to use in milliseconds (default: 45 seconds)
        - \c "error_passthru": if @ref Qore::True "True" then HTTP status codes indicating errors will not cause an
          \c HTTP-CLIENT-RECEIVE-ERROR exception to be raised, rather such responses will be passed through to the caller
          like any other response
        - \c "http_version": HTTP version to use (\c "1.0" or \c "1.1", defaults to \c "1.1")
        - \c "max_redirects": maximum redirects to support
        - \c "proxy": proxy URL to use
        - \c "redirect_passthru": if @ref Qore::True "True" then redirect responses will be passed to the called instead of
          processed
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
            const Options = {
                "error_passthru": True,
                "http_version": True,
                "max_redirects": True,
                "proxy": True,
                "timeout": True,
                "connect_timeout": True,
                "redirect_passthru": True,
                "ssl_cert_path": True,
                "ssl_key_password": True,
                "ssl_key_path": True,
                "ssl_verify_cert": True,
            };

            #! HttpConnection default options
            const DefaultOptions = {
                "timeout": 45s,
                "connect_timeout": 45s,
            };

            const OptionList = Options.keys();
        }

        #! DEPRECATED: creates the HttpConnection object
        /** @param name the name of the connection
            @param desc connection description
            @param url connection URL (potentially with password info)
            @param monitor monitoring flag
            @param opts connection options (@see getOptions() for valid options)
            @param urlh broken down URL hash (as returned by @ref Qore::parse_url())

            @deprecated since Qorus 0.9
        */
        deprecated
        constructor(string name, string desc, string url, bool monitor, *hash opts, hash urlh)
            : AbstractConnection(name, desc, url, {"monitor": monitor}, opts ?? {}) {
        }

        #! creates the HttpConnection object
        /** @param name the name of the connection
            @param description connection description
            @param url connection URL (potentially with password info)
            @param attributes various attributes. See below
            @param options connection options

            See @ref AbstractConnection::constructor() for \c attributes and \c options reference.
         */
        constructor(string name, string description, string url, hash attributes = {}, hash options = {})
            : AbstractConnection(name, description, url, attributes, options) {
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
            - \c "error_passthru": if @ref Qore::True "True" then HTTP status codes indicating errors will not cause an
              \c HTTP-CLIENT-RECEIVE-ERROR exception to be raised, rather such responses will be passed through to the caller
              like any other response
            - \c "http_version": HTTP version to use (\c "1.0" or \c "1.1", defaults to \c "1.1")
            - \c "max_redirects": maximum redirects to support
            - \c "proxy": proxy URL to use
            - \c "redirect_passthru": if @ref Qore::True "True" then redirect responses will be passed to the called instead of
              processed
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

        #! DEPRECATED: static constructor
        /** @deprecated since Qore 0.9 in favor of new constructor and Qore::Reflection
         */
        deprecated
        static HttpConnection make(string name, string desc, string url, bool monitor, *hash opts, hash urlh) {
            hash attributes = {
                "monitor": monitor,
            };

            return new HttpConnection(name, desc, url, attributes, opts ?? {});
        }
    }
} # ConnectionProvider namespace
