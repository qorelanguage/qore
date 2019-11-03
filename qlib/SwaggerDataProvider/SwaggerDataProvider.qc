# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SwaggerDataProvider module definition

/** SwaggerDataProvider.qc Copyright 2019 Qore Technologies, s.r.o.

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

# minimum required Qore version
%requires qore >= 0.9.4
# assume local scope for variables, do not use "$" signs
%new-style
# require type definitions everywhere
%require-types
#! strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

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
        };

        #! Constructor options
        const ConstructorOptions = {
            "schema": <DataProviderOptionInfo>{
                "type": (StringType, new Type("SwaggerSchema")),
                "desc": "the Swagger schema object or source URL",
                "required": True,
            },
            "restclient": <DataProviderOptionInfo>{
                "type": new Type("RestClient"),
                "desc": "the RestClient object",
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
    }

    #! Creates the object from the arguments
    constructor(SwaggerSchema schema, *RestClient rest) {
        self.schema = schema;
        if (rest) {
            self.rest = rest;
        }

        setupTree();
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        switch (copts.schema.typeCode()) {
            case NT_STRING:
                schema = SwaggerLoader::fromUrl(copts.schema);
                break;
            case NT_OBJECT:
                schema = copts.schema;
                break;
        }

        if (copts.restclient) {
            rest = copts.restclient;
        }

        setupTree();
    }

    #! Returns the data provider name
    string getName() {
        return schema.info.title + uri_path;
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
                return new SwaggerOperationDataProvider(schema, rest, uri_path, pio.getOperation(name.lwr(), uri_path));
            } catch (hash<ExceptionInfo> ex) {
                if (ex.err != "INVALID-METHOD") {
                    rethrow;
                }
            }
        }

        if (exists path_tree."*" && !HttpMethods{name.upr()}) {
            return getChildIntern("*");
        }
    }

    #! Returns the child provider for the given path component
    AbstractDataProvider getChildIntern(string name) {
        string new_uri_path = uri_path;
        if (new_uri_path !~ /\/$/) {
            new_uri_path += "/";
        }
        new_uri_path += name;
        return new SwaggerDataProvider(path_tree{name}, new_uri_path, schema, rest);
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
} # end public namespace swagger
