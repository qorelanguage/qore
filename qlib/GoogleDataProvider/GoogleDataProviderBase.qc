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

        #! Discovery API name
        const GoogleDiscoveryApiName = "calendar";
    }

    private {
        #! The REST client object for API calls
        *GoogleRestClient::GoogleRestClient rest;

        #! API -> URL map
        static hash<string, string> api_map;

        #! schema cache; API -> schema name -> type
        static hash<string, hash<string, AbstractDataProviderType>> schema_map;

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

    #! Returns a data type for the given Google schema
    static DataProvider::AbstractDataProviderType getTypeForSchema(string api, string schema) {
        hash<string, AbstractDataProviderType> api_schemas = GoogleDataProviderBase::getApi(api);
        if (*AbstractDataProviderType rv = api_schemas{schema}) {
            return rv;
        }
        throw "SCHEMA-ERROR", sprintf("Google API %y has unknown schema %y; known schemas: %y", api, schema,
            keys api_schemas);
    }

    #! Returns all schemas for the given API
    static hash<string, AbstractDataProviderType> getApi(string api) {
        *hash<string, AbstractDataProviderType> rv;
        if (rv = schema_map{api}) {
            return rv;
        }

        AutoLock al(m);
        # check again in the lock
        if (rv = schema_map{api}) {
            return rv;
        }

        RestClient rest({"url": DiscoveryUrlBase});
        string url = GoogleDataProviderBase::getUrlForApi(api, rest);
        rest.setURL(url);

        # otherwise load it
        hash<auto> api;
        {
            hash<auto> info;
            on_error rethrow $1.err, sprintf("%s: info: %y", $1.desc, info{"request-uri", "response-uri"});
            api = rest.get("").body;
        }
        TypeResolverHelper unresolved();
        hash<string, AbstractDataProviderType> schema = map {
            $1.key: GoogleDataProviderBase::makeTypeFromSchemaInfo($1.key, $1.value, unresolved)
        }, api.schemas.pairIterator();
        unresolved.resolve(schema);
        return schema_map{api} = schema;
    }

    #! Returns a type from a Google schema description
    private static AbstractDataProviderType makeTypeFromSchemaInfo(HashDataType type, hash<auto> info,
            TypeResolverHelper unresolved) {
        if (*string ref = info."$ref") {
            return unresolved.addReference(type, ref, info.required);
        }
        return GoogleDataProviderBase::makeTypeFromSchemaInfo("Additional Properties", info, unresolved);
    }

    #! Returns a type from a Google schema description
    private static AbstractDataProviderType makeTypeFromSchemaInfo(ListDataType type, string key, hash<auto> info,
            TypeResolverHelper unresolved) {
        if (*string ref = info."$ref") {
            return unresolved.addReference(type, ref);
        }
        return GoogleDataProviderBase::makeTypeFromSchemaInfo(key, info, unresolved);
    }

    #! Returns a type from a Google schema description
    private static AbstractDataProviderType makeTypeFromSchemaInfo(QoreDataField field, hash<auto> info,
            TypeResolverHelper unresolved) {
        if (*string ref = info."$ref") {
            return unresolved.addReference(field, ref);
        }
        return GoogleDataProviderBase::makeTypeFromSchemaInfo(field.getName(), info, unresolved);
    }

    #! Returns a type from a Google schema description
    private static AbstractDataProviderType makeTypeFromSchemaInfo(string key, hash<auto> info,
            TypeResolverHelper unresolved) {
        switch (info.type) {
            case "object": return GoogleDataProviderBase::makeHashFromSchemaInfo(key, info, unresolved);
            case "array": return GoogleDataProviderBase::makeListFromSchemaInfo(key, info, unresolved);
            case "string": {
                switch (info.format) {
                    case "int32":
                    case "int64":
                        return !info.required
                            ? AbstractDataProviderTypeMap."*int"
                            : AbstractDataProviderTypeMap."int";

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
            case "boolean": return !info.required
                ? AbstractDataProviderTypeMap."*bool"
                : AbstractDataProviderTypeMap."bool";
            case "integer": return !info.required
                ? AbstractDataProviderTypeMap."*int"
                : AbstractDataProviderTypeMap."int";
            case "any": return AbstractDataProviderTypeMap."any";

            default:
                throw "UNSUPPORTED-TYPE", sprintf("don't know how to process Google REST type %y", info.type);
        }
    }

    #! Returns a list from a Google schema description
    static ListDataType makeListFromSchemaInfo(string key, hash<auto> info, TypeResolverHelper unresolved) {
        ListDataType rv(key, AbstractDataProviderTypeMap{"any"}, True);
        rv.updateElementType(GoogleDataProviderBase::makeTypeFromSchemaInfo(rv, key, info.items, unresolved));
        return rv;
    }

    #! Returns a hash from a Google schema description
    static HashDataType makeHashFromSchemaInfo(string key, hash<auto> info, TypeResolverHelper unresolved) {
        HashDataType rv(!info.required ? AutoHashOrNothingType : AutoHashType, key);
        map rv.addField(GoogleDataProviderBase::makeFieldFromProperty(rv, $1.key, $1.value, unresolved)),
            info.properties.pairIterator();
        if (info.additionalProperties) {
            rv.setDefaultOtherFieldType(GoogleDataProviderBase::makeTypeFromSchemaInfo(rv, info.additionalProperties,
                unresolved));
        }
        return rv;
    }

    #! Returns a hash from a Google schema description
    static QoreDataField makeFieldFromProperty(HashDataType h, string key, hash<auto> info,
            TypeResolverHelper unresolved) {
        on_error rethrow $1.err, sprintf("%s (while creating field from info: %y)", $1.desc, info);
        QoreDataField rv({
            "name": key,
            "desc": info.description ?? "no description provided",
            "type": AbstractDataProviderTypeMap{"any"},
            "default_value": info."default",
        });
        rv.updateFieldType(GoogleDataProviderBase::makeTypeFromSchemaInfo(rv, info, unresolved));
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
        #! list of list references
        list<hash<auto>> lists;

        #! list of hash references
        list<hash<auto>> hashes;

        #! list of field references
        list<hash<auto>> fields;
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
        map $1.type.updateElementType(getType($1.ref, schemas)), lists;
        map $1.type.setDefaultOtherFieldType(getType($1.ref, schemas)), hashes;
        map $1.field.updateFieldType(getType($1.ref, schemas, $1.required)), fields;
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