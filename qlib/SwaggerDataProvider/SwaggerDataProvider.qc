# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SwaggerDataProvider module definition

/** SwaggerDataProvider.qc Copyright 2019 - 2023 Qore Technologies, s.r.o.

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

#! contains all public definitions in the SwaggerDataProvider module
public namespace SwaggerDataProvider {
#! The Swagger data provider class
public class SwaggerDataProvider inherits AbstractDataProvider {
    public {
        #! The Swagger schema
        SwaggerSchema schema;

        #! The REST client object for API calls
        *RestClient rest;

        #! current URI path
        string uri_path = "/";

        #! The value to returns as the name of the object
        string display_name;

        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "SwaggerDataProvider",
            "supports_read": False,
            "supports_create": False,
            "supports_update": False,
            "supports_upsert": False,
            "supports_delete": False,
            "supports_native_search": False,
            "supports_bulk_create": False,
            "supports_bulk_upsert": False,
            "supports_children": True,
            "constructor_options": ConstructorOptions,
            "search_options": NOTHING,
            "create_options": NOTHING,
            "upsert_options": NOTHING,
            "transaction_management": False,
            "supports_schema": True,
            "schema_type": "swagger",
            "children_can_support_apis": True,
            "children_can_support_records": False,
            "children_can_support_observers": False,
        };

        #! Constructor options
        const ConstructorOptions = {
            "schema": <DataProviderOptionInfo>{
                "type": (
                    AbstractDataProviderType::get(StringType, NOTHING, {
                        DTT_FromFile: True,
                        DTT_FromLocation: True,
                    }),
                    AbstractDataProviderType::get(new Type("SwaggerSchema")),
                ),
                "desc": "the Swagger schema object or source URL",
                "required": True,
            },
            "restclient": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(new Type("RestClient"), NOTHING, {
                    DTT_ClientOnly: True,
                }),
                "desc": "the RestClient object",
            },
            "url": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(StringType, NOTHING, {
                    DTT_ClientOnly: True,
                }),
                "desc": "the URL to the REST server; overrides any URL in the schema or in any RestClient object "
                    "passed as an option",
            },
            "restclient_options": <DataProviderOptionInfo>{
                "type": AbstractDataProviderType::get(AutoHashType, NOTHING, {
                    DTT_ClientOnly: True,
                }),
                "desc": "options to the RestClient constructor; only used if a RestClient object is created for a "
                    "call",
            },
        };

        #! Hash of HTTP Methods
        const HttpMethods = {
            "GET": True,
            "PUT": True,
            "PATCH": True,
            "POST": True,
            "DELETE": True,
        };
    }

    private {
        #! hash of valid paths
        hash<auto> path_tree;
    }

    #! Private constructor; used when traversing the tree
    private constructor(hash<auto> path_tree, string uri_path, SwaggerSchema schema, *RestClient rest) {
        self.path_tree = path_tree;
        self.uri_path = uri_path;
        self.schema = schema;
        self.rest = rest;

        display_name = uri_path.split("/").last();
    }

    #! Creates the object from the arguments
    constructor(SwaggerSchema schema, *RestClient rest) {
        if (rest) {
            self.rest = rest;
        }

        setSchema(schema);
        setupTree();
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        switch (copts.schema.typeCode()) {
            case NT_STRING:
                # determine if the string is a file or a location
                if (copts.schema.size() > 1024 || copts.schema =~ /^swagger:/ || copts.schema =~ /^{/
                    || copts.schema =~ /^swagger":/) {
                    setSchema(SwaggerLoader::fromString(copts.schema));
                } else {
                    setSchema(SwaggerLoader::fromUrl(copts.schema));
                }
                break;
            case NT_OBJECT:
                setSchema(copts.schema);
                break;
            default:
                throw "SWAGGER-ERROR", sprintf("unsupported schema type: %y", copts.schema.type());
        }

        if (copts.restclient) {
            checkRestClient(copts.restclient);
            rest = copts.restclient;
        }

        if (!rest) {
            if (copts.url || copts.restclient_options) {
                hash<auto> opts;
                opts += copts.restclient_options + {
                    "validator": schema,
                };
                if (copts.url) {
                    opts.url = copts.url;
                }
                rest = new RestClient(opts);
            }
        } else if (copts.url) {
            rest.setURL(copts.url);
        }

        setupTree();
    }

    #! Returns the data provider name
    string getName() {
        return display_name;
    }

    #! Returns the data provider description
    *string getDesc() {
        string url;
        if (rest) {
            url = rest.getURL();
        } else {
            string url;
            if (schema.schemes) {
                url = schema.schemes.https ? "https" : schema.schemes.firstKey();
            } else {
                url = "https";
            }
            url += "://";
            url += schema.host ?? "localhost";
            if (schema.basePath) {
                url += "/" + schema.basePath;
            }
        }
        return sprintf("Swagger/OpenAPI data provider for URL `%s` and URI path `%s`", url, uri_path);
    }

    #! Sets the schema and display name
    private setSchema(SwaggerSchema schema) {
        self.schema = schema;
        display_name = schema.info.title;
        display_name =~ s/\//-/g;
    }

    #! Checks the REST client
    private checkRestClient(RestClient rest) {
        *AbstractRestSchemaValidator validator = rest.getValidator();
        if (validator) {
            if (validator.getHash() != schema.getHash()) {
                throw "CONSTRUCTOR-ERROR", sprintf("RestClient provided in %s::constructor() is configured with a "
                    "different REST validator schema than %y required by this data provider", self.className(),
                    getName());
            }
        } else {
            rest.setValidator(validator);
        }
    }

    private setupTree() {
        hash<string, list<string>> paths = schema.getPathOperationHash();
        foreach hash<auto> i in (paths.pairIterator()) {
            # ignore paths with no operations
            if (!i.value) {
                continue;
            }
            code do_ref = sub (reference<auto> ref, list<string> path_elements, int offset = 0) {
                string path_element = path_elements[offset];
                # check for wildcards
                if (path_element =~ /^{.*}$/) {
                    path_element = "*";
                }
                if (!exists ref{path_element}) {
                    ref{path_element} = {};
                }
                if (++offset == path_elements.size()) {
                    return;
                }
                do_ref(\ref{path_element}, path_elements, offset);
            };

            do_ref(\path_tree, i.key.split("/"));
        }
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    private *list<string> getChildProviderNamesImpl() {
        *list<string> rv = keys path_tree;

        # now try to add operations
        *PathItemObject pio = schema.paths.tryMatch(uri_path);
        if (pio) {
            rv += map $1.upr(), pio.getMethods();
        }

        return rv;
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @see getChildProviderEx()
    */
    private *AbstractDataProvider getChildProviderImpl(string name) {
        if (exists path_tree{name}) {
            return getChildIntern(name);
        }

        # try to match an operation
        *PathItemObject pio = schema.paths.tryMatch(uri_path);
        if (pio) {
            try {
                return new SwaggerRequestDataProvider(schema, rest, uri_path, pio,
                    pio.getOperation(name.lwr(), uri_path));
            } catch (hash<ExceptionInfo> ex) {
                if (ex.err != "INVALID-METHOD") {
                    rethrow ex.err, sprintf("%s (while resolving %y %y)", ex.desc, name, uri_path);
                }
            }
        }

        if (exists path_tree."*") {
            return getChildIntern("*", name);
        }
    }

    #! Returns the child provider for the given path component
    private AbstractDataProvider getChildIntern(string name, *string real_name) {
        string new_uri_path = uri_path;
        if (new_uri_path !~ /\/$/) {
            new_uri_path += "/";
        }
        new_uri_path += real_name ?? name;
        return new SwaggerDataProvider(path_tree{name}, new_uri_path, schema, rest);
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }

    #! Returns the schema supporting this data provider
    private object getSchemaObjectImpl() {
        return schema;
    }
}
} # end public namespace swagger
