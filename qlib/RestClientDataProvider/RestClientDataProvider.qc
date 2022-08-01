# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore RestClientDataProvider class definition

/** RestClientDataProvider.qc Copyright 2022 Qore Technologies, s.r.o.

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

#! contains all public definitions in the RestClientDataProvider module
public namespace RestClientDataProvider {
#! The REST client data provider class, provides API classes as children
public class RestClientDataProvider inherits AbstractDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "RestClientDataProvider",
            "constructor_options": ConstructorOptions,
            "supports_children": True,
        };

        #! Constructor arguments
        const ConstructorOptions = {
            "assume_encoding": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "Assumes the given encoding if the server does not send a `charset` value",
            },
            "connect_timeout": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(IntType),
                "desc": "The connection timeout to use in milliseconds (default: 45 seconds)",
                "default_value": 45000,
            },
            "content_encoding": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "Sets the send encoding (if the `send_encoding` option is not set) and the "
                        "response encoding to request.  The following options are supported:\n"
                    "- `bzip`: use bzip2 compression\n"
                    "- `deflate`: use deflate compression\n"
                    "- `gzip`: use gzip compression\n"
                    "- `identity`: no compression\n"
                    "Sets the send encoding (if the `send_encoding` option is not set) and the requested response "
                    "encoding (note that only outgoing message bodies over `1024` bytes in size are compressed)",
                "default_value": "gzip",
            },
            "data": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "Determines how message bodies are serialized:\n"
                    "- `auto`: uses the following serialization options in order of availability: `json`, `yaml`, "
                        "`rawxml`, `xml`, `url`, and `text`\n"
                    "- `bin`: message bodies are not serialized, binary data is used instead\n"
                    "- `json`: use JSON serialization\n"
                    "- `rawxml`: message bodies are encoded with XML without any data type encodings\n"
                    "- `xml`: use XML-RPC value serialization\n"
                    "- `text`: message bodies are not serialized, plain text value is used\n"
                    "- `url`: message bodies are encoded in URL form encoded format (see "
                        "[RFC 2738 2.2](https://tools.ietf.org/html/rfc1738))\n"
                    "- `yaml`: use YAML serialization",
                "default_value": "auto",
            },
            "error_passthru": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(BoolType),
                "desc": "If `True` then REST status codes indicating errors will not cause an "
                    "`REST-CLIENT-RECEIVE-ERROR` exception to be raised, rather such responses will be "
                    "passed through to the caller like any other response",
                "default_value": False,
            },
            "headers": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(HashType),
                "desc": "An optional hash of headers to send with every request"
            },
            "http_version": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "HTTP version to use (`1.0` or `1.1`, defaults to `1.1`)",
                "default_value": "1.1",
            },
            "max_redirects": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(IntType),
                "desc": "Maximum redirects to support",
            },
            "no_charset": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(BoolType),
                "desc": "If `True` no charset will be added to the `Content-Type` header",
            },
            "proxy": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "The proxy URL to use",
            },
            "redirect_passthru": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(BoolType),
                "desc": "if `True` then redirect responses will be passed to the caller instead of processed",
                "default_value": False,
            },
            "send_encoding": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "Sets the send encoding (if the `send_encoding` option is not set) and the "
                        "response encoding to request.  The following options are supported:\n"
                    "- `auto`: use `gzip` compression\n"
                    "- `bzip`: use bzip2 compression\n"
                    "- `deflate`: use deflate compression\n"
                    "- `gzip`: use gzip compression\n"
                    "- `identity`: no compression\n"
                    "Sets the send encoding (if the `send_encoding` option is not set) and the requested response "
                    "encoding (note that only outgoing message bodies over `1024` bytes in size are compressed)",
                "default_value": "auto",
            },
            "ssl_cert_der": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(BoolType),
                "desc": "If `True` then the data represented by `ssl_cert_location` will be assumed to be in binary "
                    "DER format",
                "default_value": False,
            },
            "ssl_cert_location": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType, NOTHING, {
                    DTT_FromFile: True,
                    DTT_FromLocation: True,
                }),
                "desc": "A path or location to an X.509 client certificate file",
            },
            "ssl_key_der": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(BoolType),
                "desc": "If `True` then the data represented by `ssl_key_location` will be assumed to be in binary "
                    "DER format",
                "default_value": False,
            },
            "ssl_key_location": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType, NOTHING, {
                    DTT_FromFile: True,
                    DTT_FromLocation: True,
                }),
                "desc": "A path or location to a private key file for the X.509 client certificate",
            },
            "ssl_key_password": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "The password to the private key given with `ssl_key_path` (text PEM format only)",
                "sensitive": True,
            },
            "ssl_verify_cert": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(BoolType),
                "desc": "if `True` then the server's certificate will only be accepted if it's verified",
                "default_value": False,
            },
            "swagger": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType, NOTHING, {
                    DTT_FromFile: True,
                    DTT_FromLocation: True,
                }),
                "desc": "A Swagger 2.0 schema location or file data for the REST connection",
            },
            "timeout": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(IntType),
                "desc": "Transfer timeout to use in milliseconds (default: 45 seconds)",
                "default_value": 45000,
            },
            "url": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "A URL for a REST or RESTS connection",
                "required": True,
            },
            "validator_base_path": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType),
                "desc": "In case a REST validator is used, the base path in the schema can be overridden with this "
                    "option",
            },
        };

        RestClient rest;
    }

    private {
        const ChildMap = {
            "call": Class::forName("RestClientDataProvider::RestClientCallDataProvider"),
            "delete": Class::forName("RestClientDataProvider::RestClientDeleteDataProvider"),
            "get": Class::forName("RestClientDataProvider::RestClientGetDataProvider"),
            "head": Class::forName("RestClientDataProvider::RestClientHeadDataProvider"),
            "options": Class::forName("RestClientDataProvider::RestClientOptionsDataProvider"),
            "path": Class::forName("RestClientDataProvider::RestClientPatchDataProvider"),
            "post": Class::forName("RestClientDataProvider::RestClientPostDataProvider"),
            "put": Class::forName("RestClientDataProvider::RestClientPutDataProvider"),
        };
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        rest = getRestConnection(options);
    }

    #! Creates the object from a REST connection
    constructor(RestClient rest) {
        self.rest = rest;
    }

    #! Returns the data provider name
    string getName() {
        return "rest";
    }

    #! Returns the data provider description
    *string getDesc() {
        return "Data provider for REST servers";
    }

    #! Return data provider summary info
    *list<hash<DataProviderSummaryInfo>> getChildProviderSummaryInfo() {
        return map $1.getStaticMember("StaticInfo").getValue(), ChildMap.iterator();
    }

    #! Returns a REST connection
    static RestClient getRestConnection(hash<auto> options) {
        if (*string loc = remove options.ssl_cert_location) {
            if (options.ssl_cert_location =~ /^[a-z]+:\/\//) {
                options.ssl_cert_data = options.ssl_cert_der
                    ? FileLocationHandler::getBinaryFileFromLocation(loc)
                    : FileLocationHandler::getTextFileFromLocation(loc);
            } else {
                options.ssl_cert_path = loc;
            }
        }
        if (*string loc = remove options.ssl_key_location) {
            if (options.ssl_key_location =~ /^[a-z]+:\/\//) {
                options.ssl_key_data = options.ssl_key_der
                    ? FileLocationHandler::getBinaryFileFromLocation(loc)
                    : FileLocationHandler::getTextFileFromLocation(loc);
            } else {
                options.ssl_key_path = loc;
            }
        }
        if (*string base_path = remove options.validator_base_path) {
            options.swagger_base_path = base_path;
        }
        return new RestClient(options);
    }

    #! Makes a REST request and returns the response
    static hash<auto> makeRequest(RestClient rest, string meth, *data body, string path, *hash<auto> hdr) {
        hash<auto> info;
        hash<auto> resp = rest.doRequest(meth, path, body, \info, NOTHING, hdr);
        return {
            "request-headers": info.headers,
            "request-serialization": info."request-serialization",
            "request-uri": info."request-uri",
            "response-body": resp.body,
            "response-headers": (map {$1.key.lwr(): $1.value}, info."response-headers-raw".pairIterator()),
            "response-headers-raw": info."response-headers-raw",
            "response-serialization": info."response-serialization",
            "response-uri": info."response-uri",
            "status-code": info."response-code",
            "status-message": info."response-headers".status_message,
        };
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        return keys ChildMap;
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @see getChildProviderEx()
    */
    private *AbstractDataProvider getChildProviderImpl(string name) {
        *Class cls = ChildMap{name};
        if (!cls) {
            return;
        }
        return cls.newObject(rest);
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}