# -*- mode: qore; indent-tabs-mode: nil -*-
# Qore HttpConnection class definition

/*  HttpConnection.qc Copyright 2016 - 2024 Qore Technologies, s.r.o.

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

#! The ConnectionProvider namespace
/** All public declarations in the %ConnectionProvider module are defined in this namespace
*/
public namespace ConnectionProvider {
#! class for HTTP connections; returns @ref Qore::HTTPClient objects
/** Supports the following options:
    - \c "assume_encoding": assumes the given encoding if the server does not send a charset value
    - \c "connect_timeout": connection timeout to use in milliseconds (default: 45 seconds)
    - \c "error_passthru": if @ref True "True" then HTTP status codes indicating errors will not cause an
        \c HTTP-CLIENT-RECEIVE-ERROR exception to be raised, rather such responses will be passed through to the
        caller like any other response
    - \c "headers": headers to add in each outgoing request
    - \c "http_version": HTTP version to use (\c "1.0" or \c "1.1", defaults to \c "1.1")
    - \c "max_redirects": maximum redirects to support
    - \c "pre_encoded_urls": If @ref True "True" then all URI paths in URLs are assumed to be already
      <a href="https://en.wikipedia.org/wiki/Percent-encoding">percent encoded</a>; if this flag is set and any
      unencoded characters are sent in a URL with a new request, an exception is raised
    - \c "proxy": proxy URL to use
    - \c "redirect_passthru": if @ref True "True" then redirect responses will be passed to the caller instead of
        processed
    - \c "ssl_cert_path": a path to an X.509 client certificate file in PEM format
    - \c "ssl_key_path": a path to a private key file in PEM format for the X.509 client certificate
    - \c "ssl_key_password": the password to the private key given with \c "ssl_key_path"
    - \c "ssl_verify_cert": if @ref True "True" then the server's certificate will only be accepted if it's verified
    - \c "timeout": transfer timeout to use in milliseconds (default: 45 seconds)

    @see @ref Qore::HTTPClient::constructor(hash<auto>) "HTTPClient::constructor(hash<auto> opts)" for more
    information on the above options
*/
public class HttpConnection inherits AbstractConnectionWithInfo {
    public {
        #! Connection entry info
        const ConnectionScheme = <ConnectionSchemeInfo>{
            "display_name": "HTTP Connection",
            "short_desc": "A connection to a HTTP server",
            "desc": "A connectioas to a HTTP server",
            "cls": Class::forName("HttpConnection"),
            "schemes": {
                "https": True,
                "http": True,
            },
            "options": {
                "assume_encoding": <ConnectionOptionInfo>{
                    "display_name": "Assume Character Encoding",
                    "short_desc": "Sets the character encoding to assume if 'charset' is missing",
                    "type": "string",
                    "desc": "Assumes the given encoding if the server does not send a `charset` value",
                },
                "connect_timeout": <ConnectionOptionInfo>{
                    "display_name": "Connect Timeout",
                    "short_desc": "The connect timeout in milliseconds",
                    "type": "int",
                    "desc": "The connection timeout to use in milliseconds (default: 45 seconds)",
                    "default_value": 45000,
                },
                "disable_automatic_pings": <ConnectionOptionInfo>{
                    "display_name": "Disable Automatic Pings?",
                    "short_desc": "Disables automatic pings",
                    "type": "bool",
                    "desc": "Disables automatic pings; useful for rate-limited or metered connections",
                },
                "encode_chars": <ConnectionOptionInfo>{
                    "display_name": "Encode URL Characters",
                    "short_desc": "A set of additional characters to subject to percent encoding in URLs",
                    "type": "string",
                    "desc": "A set of additional characters to subject to percent encoding in URLs",
                },
                "error_passthru": <ConnectionOptionInfo>{
                    "display_name": "Error Passthrough",
                    "short_desc": "If errors should be returned like normal successful responses",
                    "type": "bool",
                    "desc": "If `True` then HTTP status codes indicating errors will not cause an "
                        "`HTTP-CLIENT-RECEIVE-ERROR` exception to be raised, rather such responses will be "
                        "passed through to the caller like any other response",
                    "default_value": False,
                },
                "headers": <ConnectionOptionInfo>{
                    "display_name": "HTTP Headers",
                    "short_desc": "Sets HTTP headers to include in each request",
                    "type": "hash",
                    "desc": "Sets HTTP headers to include in each outgoing request",
                },
                "http_version": <ConnectionOptionInfo>{
                    "display_name": "HTTP Version",
                    "short_desc": "The HTTP version to use",
                    "type": "string",
                    "desc": "The HTTP version to use (`1.0` or `1.1`, defaults to `1.1`)",
                    "default_value": "1.1",
                    "allowed_values": (
                        <AllowedValueInfo>{
                            "value": "1.0",
                            "desc": "For HTTP 1.0",
                        },
                        <AllowedValueInfo>{
                            "value": "1.1",
                            "desc": "For HTTP 1.1",
                        },
                    ),
                },
                "max_redirects": <ConnectionOptionInfo>{
                    "display_name": "Max Redirects",
                    "short_desc": "Sets the maximum HTTP redirects to support",
                    "type": "int",
                    "desc": "Sets the maximum HTTP redirects to support in a single request",
                },
                "pre_encoded_urls": <ConnectionOptionInfo>{
                    "display_name": "Pre-Encoded URLs",
                    "short_desc": "If true then URLs are assumed to be preencoded",
                    "type": "bool",
                    "desc": "If `true` then all URI paths in URLs are assumed to be already "
                        "[percent encoded](https://en.wikipedia.org/wiki/Percent-encoding); if this flag is set and "
                        "any unencoded characters are sent in a URL with a new request, an exception is raised",
                },
                "proxy": <ConnectionOptionInfo>{
                    "display_name": "HTTP Proxy",
                    "short_desc": "The HTTP proxy to use",
                    "type": "string",
                    "desc": "The HTTP proxy URL to use",
                },
                "redirect_passthru": <ConnectionOptionInfo>{
                    "display_name": "Redirect Passthrough",
                    "short_desc": "If true then redirect responses are returned to the caller",
                    "type": "bool",
                    "desc": "If `True` then redirect responses will be passed to the caller instead of processed",
                    "default_value": False,
                },
                "ssl_cert_data": <ConnectionOptionInfo>{
                    "display_name": "SSL Certificate Data",
                    "short_desc": "X.509 certificate data",
                    "type": "data",
                    "desc": "The X.509 certificate data in PEM format (string) or in DER format (binary); if this "
                        "option is set, then `ssl_cert_path` is ignored",
                },
                "ssl_cert_path": <ConnectionOptionInfo>{
                    "display_name": "SSL Certificate Location",
                    "short_desc": "The location of the X.509 client certificate to use",
                    "type": "string",
                    "desc": "The location of a X.509 client certificate file in PEM format",
                    "subst_env_vars": True,
                },
                "ssl_key_password": <ConnectionOptionInfo>{
                    "display_name": "SSL Key Password",
                    "short_desc": "The password to the private key",
                    "type": "string",
                    "desc": "The password to the private key given with `ssl_key_path`",
                    "sensitive": True,
                },
                "ssl_key_data": <ConnectionOptionInfo>{
                    "display_name": "SSL Key Data",
                    "short_desc": "X.509 private key data",
                    "type": "data",
                    "desc": "The X.509 private key data in PEM format (string) or in DER format (binary); if this "
                        "option is set, then `ssl_key_path` is ignored",
                },
                "ssl_key_path": <ConnectionOptionInfo>{
                    "display_name": "SSL Key Location",
                    "short_desc": "The location of the private key file",
                    "type": "string",
                    "desc": "The location to a private key file in PEM format for the X.509 client certificate",
                    "subst_env_vars": True,
                },
                "ssl_verify_cert": <ConnectionOptionInfo>{
                    "display_name": "SSL Verify Certificate",
                    "short_desc": "If true then server certificates will be verified",
                    "type": "bool",
                    "desc": "if `True` then the server's certificate will only be accepted if it's verified",
                    "default_value": False,
                },
                "timeout": <ConnectionOptionInfo>{
                    "display_name": "I/O Timeout",
                    "short_desc": "The maximum time an individual I/O operation can take in milliseconds",
                    "type": "int",
                    "desc": "The transfer timeout to use in milliseconds (default: 45 seconds)",
                    "default_value": 45000,
                },
            },
        };

        #! HttpConnection object connection options
        const Options = map {$1: True}, keys ConnectionScheme.options;
    }

    #! Creates the HttpConnection object
    /** @param name the name of the connection
        @param desc connection description
        @param url connection URL (potentially with password info)
        @param attributes various attributes. See below
        @param options connection options

        See @ref AbstractConnection::constructor() for \c attributes and \c options reference.

        @throw CONNECTION-OPTION-ERROR missing or invalid connection option
    */
    constructor(string name, string desc, string url, hash<auto> attributes = {}, hash<auto> options = {},
            *Logger logger) : AbstractConnectionWithInfo(name, desc, url, attributes, options, logger) {
    }

    #! Creates the HttpConnection object
    /** @param config with the following keys:
        - name (required string): the connection name
        - display_name (optional string): the display name
        - short_desc (optional string): a short description in plain text
        - desc (optional string): a long description with markdown formatting
        - url (required string): the connection URL
        - opts (optional hash): connection options
        - logger (optional LoggerInterface object): logger for the connection
        @param attr optional connection attributes
        - monitor (optional bool): should the connection be monitored? Default: True
        - enabled (optional bool): is the connection enabled? Default: True
        - locked (optional bool): is the connection locked? Default: False
        - debug_data (optional bool): debug data? Default: False
        - tags (optional hash): tags for the connection; no default value

        @throw CONNECTION-OPTION-ERROR missing or invalid connection option or attribute
    */
    constructor(hash<auto> config, *hash<auto> attr) : AbstractConnectionWithInfo(config, attr) {
    }

    #! Returns \c "http"
    string getType() {
        return "http";
    }

    #! Returns an HttpDataProvider object for this connection
    /** @param constructor_options any additional constructor options for the data provider

        @return an HttpDataProvider object for this connection

        @see hasDataProvider()
    */
    DataProvider::AbstractDataProvider getDataProvider(*hash<auto> constructor_options) {
        # to avoid circular dependencies, this object loads the HttpDataProvider and creates the data provider
        # object dynamically
        load_module("HttpDataProvider");
        return create_object("HttpDataProvider", get());
    }

    #! Returns @ref True, as the connection returns a data provider with the @ref getDataProvider() method
    /** @return @ref True, as the connection returns a data provider with the @ref getDataProvider() method

        @see @ref getDataProvider()
    */
    bool hasDataProvider() {
        return True;
    }

    #! returns @ref True, since this connection type supports the %Qore @ref Qore::Socket "Socket-based" polling API
    /** @return @ref True

        @since ConnectionProvider 1.7
    */
    bool supportsPollingApi() {
        return True;
    }

    #! Called to start the connection if the connection supports the polling API
    /**
        @return a socket poll operation object that will allow the connection goal to be reached with polling

        @see supportsPollingApi()

        @since ConnectionProvider 1.7
    */
    Qore::AbstractPollOperation startPollConnect() {
        return getPollImpl().startPollConnect();
    }

    #! Returns options for creating a new connection
    *hash<auto> getConnectionOptions(*hash<auto> rtopts) {
        return {"url": url} + opts;
    }

    #! Sets child data provider capabilities
    private setChildCapabilities() {
        children_can_support_apis = True;
    }

    #! returns an @ref Qore::HTTPClient object
    /** @param connect if @ref True "True" then a connection is made by calling @ref Qore::HTTPClient::connect() before returning the object
        @param rtopts this connection type does not accept any runtime option, so this parameter is ignored
    */
    private Qore::HTTPClient getImpl(bool connect = True, *hash<auto> rtopts) {
        HTTPClient sc(getConnectionOptions());
        if (connect) {
            sc.connect();
        }

        return sc;
    }

    #! Returns the ConnectionSchemeInfo hash for this object
    private hash<ConnectionSchemeInfo> getConnectionSchemeInfoImpl() {
        return ConnectionScheme;
    }
}
} # ConnectionProvider namespace
