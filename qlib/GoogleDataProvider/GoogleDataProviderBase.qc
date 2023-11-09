# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GoogleDataProvider module definition

/** GoogleDataProviderBase.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! Contains all public definitions in the GoogleDataProvider module
public namespace GoogleDataProvider {
#! The Google data provider base class
public class GoogleDataProviderBase inherits DataProvider::AbstractDataProvider {
    public {
        #! Discovery URL base
        const DiscoveryUrlBase = "https://discovery.googleapis.com/discovery/v1/apis";
    }

    private {
        #! The REST client object for API calls
        *GoogleRestClient::GoogleRestClient rest;

        #! API -> URL map
        static hash<string, string> api_map;

        #! Schema info: API -> info
        static hash<string, hash<auto>> info_map;

        #! input schema cache; API -> schema name -> type
        static hash<string, hash<string, AbstractDataProviderType>> input_schema_map;

        #! output schema cache; API -> schema name -> type
        static hash<string, hash<string, AbstractDataProviderType>> output_schema_map;

        #! schema cache; API -> request path -> type
        static hash<string, hash<auto>> request_map;

        #! schema cache lock
        static Mutex m();
    }

    #! Creates the object
    constructor() {
    }

    #! Creates the object
    constructor(GoogleRestClient::GoogleRestClient rest) {
        self.rest = rest;
    }

    #! Accepts a LoggerInterface object for logging (or clears it)
    setLogger(*LoggerInterface logger) {
        rest.setLogger(logger);
        LoggerWrapper::setLogger(logger);
    }

    #! Prepares a request with a request type created from the Discovery API
    /** @param req The incoming request data
        @param implicit_path_args any implicit path args

        @return a hash with the following keys:
        - \c body: a hash of body params
        - \c path: a hash of path params
        - \c query: a string of query params prefixed by "?"
    */
    *hash<auto> prepareRequest(auto req, *hash<string, string> implicit_path_args, *bool backfill_request_body) {
        AbstractDataProviderType request_type = getRequestTypeImpl();

        hash<auto> rv;
        hash<string, bool> body_keys;
        foreach AbstractDataField field in (request_type.getFields().values()) {
            string name = field.getName();
            *string loc = field.getAttributes().location;
            if (loc == "query") {
                if (req.hasKey(name)) {
                    auto val = remove req{name};
                    if (exists val) {
                        # this handles list key values by repeating the expression in the output
                        map (rv.query += ((rv.query ? "&" : "?") + sprintf("%s=%s", name, $1))), val;
                    }
                }
            } else if (loc == "path") {
                auto val = (remove req{name});
                if (val.typeCode() != NT_STRING) {
                    throw "GOOGLE-REQUEST-ERROR", sprintf("path argument %y is missing from the request", name);
                }
                rv.path{name} = val;
            } else {
                if (backfill_request_body) {
                    body_keys{name} = True;
                }
                if (req.hasKey(name)) {
                    rv.body{name} = remove req{name};
                }
            }
        }
        if (implicit_path_args) {
            rv.path += implicit_path_args;
        }
        if (req) {
            throw "GOOGLE-REQUEST-ERROR", sprintf("request has invalid keys: %y", req);
        }
        if (backfill_request_body && rv.body.size() < body_keys.size()) {
            map rv.body{$1.key} = $1.value, backfillRequestBody(rv).pairIterator(), !rv.body.hasKey($1.key);
        }
        return rv;
    }

    #! Used to set attributes in the request body that are not set in the request
    private hash<auto> backfillRequestBody(hash<auto> req_info) {
        throw "UNIMPLEMENTED", sprintf("must be overridden in class %y", self.className());
    }

    #! Returns a GoogleRestClient connection from the options
    static GoogleRestClient::GoogleRestClient getRestConnection(*hash<auto> options) {
        GoogleRestClient rest;
        if (options.restclient) {
            rest = options.restclient;
        } else {
            if (!options.oauth2_client_id) {
                throw "CONSTRUCTOR-ERROR", "no 'restclient' or 'oauth2_client_id' option passed; "
                    "cannot create REST client to Google instance without authentication information";
            }
            if (!options.oauth2_client_secret) {
                throw "CONSTRUCTOR-ERROR", "no 'restclient', 'oauth2_client_secret' option passed; "
                    "cannot create REST client to Google instance without authentication information";
            }
            if (!options.token) {
                throw "CONSTRUCTOR-ERROR", "no 'restclient', 'token' option passed; "
                    "cannot create REST client to Google instance without authentication information";
            }

            hash<auto> opts;
            opts += options.restclient_options + options{
                "oauth2_client_id",
                "oauth2_client_secret",
                "token",
                "oauth2_refresh_token",
            };
            rest = new GoogleRestClient(opts);
        }
        rest.setConnectionPath("/");
        return rest;
    }

    #! Returns a data type for the given request
    static DataProvider::AbstractDataProviderType getRequestTypeForSchema(string api, string path, ...) {
        hash<auto> requests = GoogleDataProviderBase::getRequestInfo(api);

        string rpath = path;
        if (argv) {
            rpath += "-" + (foldl $1 + "-" + $2, argv);
        }
        if (*AbstractDataProviderType rv = requests{rpath}.request_type) {
            return rv;
        }
        # create new type without implicit args
        if (argv && (*AbstractDataProviderType rv = requests{path}.request_type)) {
            hash<string, bool> fmap = map {$1: True}, argv;
            HashDataType nrv();
            map nrv.addField($1), rv.getFields().iterator(), !fmap{$1.getName()};
            requests{rpath} = nrv;
            return nrv;
        }
        throw "SCHEMA-ERROR", sprintf("Google API %y has unknown request path %y; known request paths: %y", api,
            path, keys requests);
    }

    #! Returns a data type for the given Google schema
    static DataProvider::AbstractDataProviderType getTypeForSchema(string api, string schema) {
        hash<string, AbstractDataProviderType> api_schemas = GoogleDataProviderBase::getApi(api);
        if (*AbstractDataProviderType rv = api_schemas{schema}) {
            return rv;
        }
        throw "SCHEMA-ERROR", sprintf("Unknown schema %y requested from Google API %y; known schemas: %y", schema,
            api, keys api_schemas);
    }

    #! Returns all request types for the given API
    static hash<auto> getRequestInfo(string api) {
        *hash<auto> rv;
        if (rv = request_map{api}) {
            return rv;
        }

        AutoLock al(m);
        # check again in the lock
        if (rv = request_map{api}) {
            return rv;
        }

        hash<auto> api_info = GoogleDataProviderBase::getSchemaInfo(api);
        hash<string, AbstractDataProviderType> input_schema = input_schema_map{api}
            ?? GoogleDataProviderBase::getInputSchemaMap(api, api_info);
        hash<string, AbstractDataProviderType> output_schema = output_schema_map{api}
            ?? GoogleDataProviderBase::getOutputSchemaMap(api, api_info);

        # create types from the schema
        TypeResolverHelper unresolved();
        hash<auto> requests = foldl $1 + $2, (
            map GoogleDataProviderBase::processRequests($1.key, $1.value, unresolved),
                api_info.resources.pairIterator()
        );
        # resolve all unresolved types
        requests = unresolved.resolveRequests(api, requests, input_schema_map, output_schema_map);

        # save and return the schema info
        return request_map{api} = requests;
    }

    #! Process requests
    static private hash<auto> processRequests(string api, hash<auto> h, TypeResolverHelper unresolved) {
        hash<auto> rv;
        foreach hash<auto> i in (h.methods.pairIterator()) {
            string api_path = api + "/" + i.key;
            rv{api_path} = GoogleDataProviderBase::processRequest(api_path, i.value, unresolved);
        }
        return rv;
    }

    #! Returns a hash from a Google schema description
    static hash<auto> processRequest(string key, hash<auto> info, TypeResolverHelper unresolved) {
        HashDataType request_type(AutoHashType, key);
        map request_type.addField(GoogleDataProviderBase::makeFieldFromProperty(request_type, $1.key, $1.value,
            unresolved, True)), info.parameters.pairIterator(), !$1.value."deprecated";
        if (*string ref = info.request."$ref") {
            unresolved.addRequestTypeReference(key, request_type, ref, info.required);
        }
        if (*string ref = info.response."$ref") {
            unresolved.addResponseTypeReference(key, ref, info.required);
        }
        return {
            "request_type": request_type,
        };
    }

    #! Returns all schemas for the given API
    static hash<string, AbstractDataProviderType> getApi(string api) {
        *hash<string, AbstractDataProviderType> rv;
        if (rv = output_schema_map{api}) {
            return rv;
        }

        AutoLock al(m);
        # check again in the lock
        if (rv = output_schema_map{api}) {
            return rv;
        }

        hash<auto> api_info = GoogleDataProviderBase::getSchemaInfo(api);
        return GoogleDataProviderBase::getOutputSchemaMap(api, api_info);
    }

    #! Creates the input schema map for the given API
    static hash<string, AbstractDataProviderType> getInputSchemaMap(string api, hash<auto> api_info) {
        # create types from the schema
        TypeResolverHelper unresolved();
        hash<string, AbstractDataProviderType> schema = map {
            $1.key: GoogleDataProviderBase::makeTypeFromSchemaInfo($1.key, $1.value, unresolved, True)
        }, api_info.schemas.pairIterator();

        # resolve all unresolved types
        unresolved.resolve(schema);

        # save and return the schema info
        return input_schema_map{api} = schema;
    }

    #! Creates the output schema map for the given API
    static hash<string, AbstractDataProviderType> getOutputSchemaMap(string api, hash<auto> api_info) {
        # create types from the schema
        TypeResolverHelper unresolved();
        hash<string, AbstractDataProviderType> schema = map {
            $1.key: GoogleDataProviderBase::makeTypeFromSchemaInfo($1.key, $1.value, unresolved)
        }, api_info.schemas.pairIterator();

        # resolve all unresolved types
        unresolved.resolve(schema);

        # save and return the schema info
        return output_schema_map{api} = schema;
    }

    static hash<auto> getSchemaInfo(string api) {
        if (*hash<auto> info = info_map{api}) {
            return info;
        }

        RestClient rest({"url": DiscoveryUrlBase});
        string url = GoogleDataProviderBase::getUrlForApi(api, rest);
        rest.setURL(url);

        # otherwise load it
        hash<auto> api_info;
        {
            hash<auto> info;
            on_error rethrow $1.err, sprintf("%s: info: %y", $1.desc, info{"request-uri", "response-uri"});
            api_info = rest.get("").body;
        }

        return info_map{api} = api_info;
    }

    #! Returns a type from a Google schema description
    private static AbstractDataProviderType makeTypeFromSchemaInfo(HashDataType type, hash<auto> info,
            TypeResolverHelper unresolved, *bool for_input) {
        if (*string ref = info."$ref") {
            return unresolved.addReference(type, ref, info.required);
        }
        return GoogleDataProviderBase::makeTypeFromSchemaInfo("Additional Properties", info, unresolved);
    }

    #! Returns a type from a Google schema description
    private static AbstractDataProviderType makeTypeFromSchemaInfo(ListDataType type, string key, hash<auto> info,
            TypeResolverHelper unresolved, *bool for_input) {
        if (*string ref = info."$ref") {
            return unresolved.addReference(type, ref);
        }
        return GoogleDataProviderBase::makeTypeFromSchemaInfo(key, info, unresolved, for_input);
    }

    #! Returns a type from a Google schema description
    private static AbstractDataProviderType makeTypeFromSchemaInfo(QoreDataField field, hash<auto> info,
            TypeResolverHelper unresolved, *bool for_input) {
        if (*string ref = info."$ref") {
            return unresolved.addReference(field, ref);
        }
        return GoogleDataProviderBase::makeTypeFromSchemaInfo(field.getName(), info, unresolved, for_input);
    }

    #! Returns a type from a Google schema description
    private static AbstractDataProviderType makeTypeFromSchemaInfo(string key, hash<auto> info,
            TypeResolverHelper unresolved, *bool for_input) {
        switch (info.type) {
            case "object": return GoogleDataProviderBase::makeHashFromSchemaInfo(key, info, unresolved, for_input);
            case "array": return GoogleDataProviderBase::makeListFromSchemaInfo(key, info, unresolved, for_input);
            case "string": {
                switch (info.format) {
                    case "int32":
                    case "int64":
                        return !info.required
                            ? AbstractDataProviderTypeMap."*softint"
                            : AbstractDataProviderTypeMap."softint";

                    case "date-time":
                        return !info.required
                            ? AbstractDataProviderTypeMap."*softdate"
                            : AbstractDataProviderTypeMap."softdate";

                    case NOTHING:
                        return !info.required
                            ? AbstractDataProviderTypeMap."*string"
                            : AbstractDataProviderTypeMap."string";
                }
            }
            case "boolean": return for_input ? (
                    !info.required
                    ? SoftBoolOrNothingDataProviderStringType
                    : SoftBoolDataProviderStringType
                )
                : (
                    !info.required
                    ? SoftBoolOrNothingDataProviderInputType
                    : SoftBoolDataProviderInputType
                );
            case "integer": return !info.required
                ? AbstractDataProviderTypeMap."*softint"
                : AbstractDataProviderTypeMap."softint";
            case "any": return AbstractDataProviderTypeMap."any";

            default:
                throw "UNSUPPORTED-TYPE", sprintf("don't know how to process Google REST type %y", info.type);
        }
    }

    #! Returns a list from a Google schema description
    static ListDataType makeListFromSchemaInfo(string key, hash<auto> info, TypeResolverHelper unresolved,
            *bool for_input) {
        ListDataType rv(key, AbstractDataProviderTypeMap{"any"}, True);
        rv.updateElementType(GoogleDataProviderBase::makeTypeFromSchemaInfo(rv, key, info.items, unresolved,
            for_input));
        return rv;
    }

    #! Returns a hash from a Google schema description
    static HashDataType makeHashFromSchemaInfo(string key, hash<auto> info, TypeResolverHelper unresolved,
            *bool for_input) {
        HashDataType rv(!info.required ? AutoHashOrNothingType : AutoHashType, key);
        map rv.addField(GoogleDataProviderBase::makeFieldFromProperty(rv, $1.key, $1.value, unresolved)),
            info.properties.pairIterator(), !$1.value."deprecated";
        if (info.additionalProperties) {
            rv.setDefaultOtherFieldType(GoogleDataProviderBase::makeTypeFromSchemaInfo(rv, info.additionalProperties,
                unresolved, for_input));
        }
        return rv;
    }

    #! Returns a hash from a Google schema description
    static QoreDataField makeFieldFromProperty(HashDataType h, string key, hash<auto> info,
            TypeResolverHelper unresolved, *bool for_input) {
        on_error rethrow $1.err, sprintf("%s (while creating field from info: %y)", $1.desc, info);
        QoreDataField rv({
            "name": key,
            "desc": info.description ?? "no description provided",
            "type": AbstractDataProviderTypeMap{"any"},
            "default_value": info."default",
        });
        if (for_input && info.location) {
            rv.setAttributes(info{"location",});
        }
        if (info.enum) {
            rv.setAllowedValues(info.enum);
        }
        rv.updateFieldType(GoogleDataProviderBase::makeTypeFromSchemaInfo(rv, info, unresolved, for_input));
        return rv;
    }

    #! Returns the URL for the given API
    static string getUrlForApi(string api, RestClient::RestClient rest) {
        *string url;
        if (url = api_map{api}) {
            return url;
        }

        rest.setURL(DiscoveryUrlBase);
        url = sprintf("?name=%s", api);

        hash<auto> info;
        on_error rethrow $1.err, sprintf("%s: info: %y", $1.desc, info{"request-uri", "response-uri"});
        hash<auto> res = rest.get(url, NOTHING, \info);
        return api_map{api} = rest.get(url).body.items[0].discoveryRestUrl;
    }
}
}

namespace Priv {
class TypeResolverHelper {
    public {}

    private {
        #! list of request list references
        list<hash<auto>> lists;

        #! list of request hash references
        list<hash<auto>> hashes;

        #! list of request field references
        list<hash<auto>> fields;

        #! list of request type references
        list<hash<auto>> requests;

        #! list of response type references
        list<hash<auto>> responses;
    }

    addRequestTypeReference(string key, HashDataType type, string ref, *bool required) {
        requests += {
            "key": key,
            "type": type,
            "ref": ref,
            "required": required,
        };
    }

    addResponseTypeReference(string key, string ref, *bool required) {
        responses += {
            "key": key,
            "ref": ref,
            "required": required,
        };
    }

    AbstractDataProviderType addReference(HashDataType type, string ref, *bool required) {
        hashes += {
            "type": type,
            "ref": ref,
            "required": required,
        };
        return AbstractDataProviderTypeMap{"any"};
    }

    AbstractDataProviderType addReference(ListDataType type, string ref) {
        lists += {
            "type": type,
            "ref": ref,
        };
        return AbstractDataProviderTypeMap{"any"};
    }

    AbstractDataProviderType addReference(QoreDataField field, string ref) {
        fields += {
            "field": field,
            "ref": ref,
        };
        return AbstractDataProviderTypeMap{"any"};
    }

    resolve(hash<string, AbstractDataProviderType> schemas) {
        if (requests || responses) {
            throw "TYPE-RESOLUTION-ERROR", "resolver has 'requests' and / or 'responses' when resolving schemas";
        }

        map $1.type.updateElementType(getType($1.ref, schemas)), lists;
        map $1.type.setDefaultOtherFieldType(getType($1.ref, schemas)), hashes;
        map $1.field.updateFieldType(getType($1.ref, schemas, $1.required)), fields;
    }

    hash<auto> resolveRequests(string api, hash<auto> req,
            hash<string, hash<string, AbstractDataProviderType>> input_schema_map,
            hash<string, hash<string, AbstractDataProviderType>> output_schema_map) {
        if (lists || hashes || fields) {
            throw "TYPE-RESOLUTION-ERROR", "resolver has 'lists' and / or 'hashes' and / or 'fields' when resolving "
                "requests";
        }

        foreach hash<auto> h in (requests) {
            req{h.key}.request = h.type;
            AbstractDataProviderType type = getType(h.ref, input_schema_map{api}, h.required);
            if (*hash<string, AbstractDataField> fields = type.getFields()) {
                map req{h.key}.request.addField($1), fields.iterator();
            }
        }

        foreach hash<auto> h in (responses) {
            req{h.key}.response = getType(h.ref, output_schema_map{api}, h.required);
        }
        return req;
    }

    static AbstractDataProviderType getType(string ref, hash<string, AbstractDataProviderType> schemas, *bool required) {
        *AbstractDataProviderType type = schemas{ref};
        if (!type) {
            throw "REFERENCE-ERROR", sprintf("type reference %y cannot be resolved; known types: %y", ref,
                keys schemas);
        }
        return required ? type : type.getOrNothingType();
    }
}
}