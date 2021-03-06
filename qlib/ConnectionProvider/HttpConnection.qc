# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore HttpConnection class definition

/*  HttpConnection.qc Copyright 2016 - 2020 Qore Technologies, s.r.o.

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
    - \c "assume_encoding": assumes the given encoding if the server does not send a charset value
    - \c "connect_timeout": connection timeout to use in milliseconds (default: 45 seconds)
    - \c "error_passthru": if @ref True "True" then HTTP status codes indicating errors will not cause an
        \c HTTP-CLIENT-RECEIVE-ERROR exception to be raised, rather such responses will be passed through to the caller
        like any other response
    - \c "http_version": HTTP version to use (\c "1.0" or \c "1.1", defaults to \c "1.1")
    - \c "max_redirects": maximum redirects to support
    - \c "proxy": proxy URL to use
    - \c "redirect_passthru": if @ref True "True" then redirect responses will be passed to the caller instead of
        processed
    - \c "ssl_cert_path": a path to an X.509 client certificate file in PEM format
    - \c "ssl_key_path": a path to a private key file in PEM format for the X.509 client certificate
    - \c "ssl_key_password": the password to the private key given with \c "ssl_key_path"
    - \c "ssl_verify_cert": if @ref True "True" then the server's certificate will only be accepted if it's verified
    - \c "timeout": transfer timeout to use in milliseconds (default: 45 seconds)

    @see @ref Qore::HTTPClient::constructor(hash<auto>) "HTTPClient::constructor(hash<auto> opts)" for more information on the above options
*/
public class HttpConnection inherits AbstractConnectionWithInfo {
    public {
        #! Connection entry info
        const ConnectionScheme = <ConnectionSchemeInfo>{
            "cls": Class::forName("FilesystemConnection"),
            "options": {
                "assume_encoding": <ConnectionOptionInfo>{
                    "type": "string",
                    "desc": "assumes the given encoding if the server does not send a `charset` value",
                },
                "connect_timeout": <ConnectionOptionInfo>{
                    "type": "int",
                    "desc": "connection timeout to use in milliseconds (default: 45 seconds)",
                    "default_value": 45000,
                },
                "error_passthru": <ConnectionOptionInfo>{
                    "type": "bool",
                    "desc": "if `True` then HTTP status codes indicating errors will not cause an "
                        "`HTTP-CLIENT-RECEIVE-ERROR` exception to be raised, rather such responses will be "
                        "passed through to the caller like any other response",
                    "default_value": False,
                },
                "http_version": <ConnectionOptionInfo>{
                    "type": "string",
                    "desc": "HTTP version to use (`1.0` or `1.1`, defaults to `1.1`)",
                    "default_value": "1.1",
                },
                "max_redirects": <ConnectionOptionInfo>{
                    "type": "int",
                    "desc": "maximum redirects to support",
                },
                "proxy": <ConnectionOptionInfo>{
                    "type": "string",
                    "desc": "proxy URL to use",
                },
                "redirect_passthru": <ConnectionOptionInfo>{
                    "type": "bool",
                    "desc": "if `True` then redirect responses will be passed to the caller instead of processed",
                    "default_value": False,
                },
                "ssl_cert_path": <ConnectionOptionInfo>{
                    "type": "string",
                    "desc": "a path to an X.509 client certificate file in PEM format",
                    "subst_env_vars": True,
                },
                "ssl_key_password": <ConnectionOptionInfo>{
                    "type": "string",
                    "desc": "the password to the private key given with `ssl_key_path`",
                    "sensitive": True,
                },
                "ssl_key_path": <ConnectionOptionInfo>{
                    "type": "string",
                    "desc": "a path to a private key file in PEM format for the X.509 client certificate",
                    "subst_env_vars": True,
                },
                "ssl_verify_cert": <ConnectionOptionInfo>{
                    "type": "bool",
                    "desc": "if `True` then the server's certificate will only be accepted if it's verified",
                    "default_value": False,
                },
                "timeout": <ConnectionOptionInfo>{
                    "type": "int",
                    "desc": "transfer timeout to use in milliseconds (default: 45 seconds)",
                    "default_value": 45000,
                },
            },
        };

        #! HttpConnection object connection options
        const Options = map {$1: True}, keys ConnectionScheme.options;

        #! HttpConnection default options
        const DefaultOptions = map {$1.key: $1.value.default_value}, ConnectionScheme.options.pairIterator(),
            exists $1.value.default_value;
    }

    #! creates the HttpConnection object
    /** @param name the name of the connection
        @param desc connection description
        @param url connection URL (potentially with password info)
        @param attributes various attributes. See below
        @param options connection options

        See @ref AbstractConnection::constructor() for \c attributes and \c options reference.

        @throw CONNECTION-OPTION-ERROR missing or invalid connection option
    */
    constructor(string name, string desc, string url, hash<auto> attributes = {}, hash<auto> options = {})
        : AbstractConnectionWithInfo(name, desc, url, attributes, options) {
    }

    #! returns \c "http"
    string getType() {
        return "http";
    }

    #! returns an @ref Qore::HTTPClient object
    /** @param connect if @ref True "True" then a connection is made by calling @ref Qore::HTTPClient::connect() before returning the object
        @param rtopts this connection type does not accept any runtime option, so this parameter is ignored
    */
    private Qore::HTTPClient getImpl(bool connect = True, *hash<auto> rtopts) {
        HTTPClient sc({"url": url} + opts);
        if (connect)
            sc.connect();

        return sc;
    }

    #! Returns the ConnectionSchemeInfo hash for this object
    private hash<ConnectionSchemeInfo> getConnectionSchemeInfoImpl() {
        return ConnectionScheme;
    }
}
} # ConnectionProvider namespace
