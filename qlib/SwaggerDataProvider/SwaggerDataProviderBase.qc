# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SwaggerDataProvider module definition

/** SwaggerDataProviderBase.qc Copyright 2019 - 2023 Qore Technologies, s.r.o.

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
#! The Swagger data provider base class
public class SwaggerDataProviderBase inherits SwaggerDataProviderCommon {
    #! Creates the object
    constructor() {
    }

    #! Creates the object
    constructor(RestClient::RestClient rest) : SwaggerDataProviderCommon(rest) {
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return SwaggerDataProvider::ProviderInfo - "constructor_options";
    }

    #! Returns a field definition from Swagger parameters
    private addFieldsFromParameters(HashDataType rv, hash<string, AbstractParameterObject> parameters,
            reference<bool> required) {
        # temporary hash for fields: loc -> name -> field
        hash<string, hash<string, AbstractDataField>> params;
        # required flag map for type: loc -> bool
        hash<string, bool> req_map;
        foreach hash<auto> i in (parameters.pairIterator()) {
            switch (i.value.inLoc) {
                # body and formData parameters are handled directly under the operation
                case "query": {
                    params{i.value.inLoc}{i.value.name} = getFieldFromParameter("!query/" + i.value.name,
                        i.value.name, i.value.type, i.value.format, i.value.items, i.value.desc, i.value.required);
                    # if at least one query argument is required, then we set "required" to True for the hash
                    if (i.value.required && !req_map{i.value.inLoc}) {
                        req_map{i.value.inLoc} = True;
                    }
                    break;
                }

                case "header": {
                    params{i.value.inLoc}{i.value.name} = getFieldFromParameter("!headers/" + i.value.name,
                        i.value.name, "string", NOTHING, NOTHING, i.value.desc, i.value.required);
                    # if at least one query argument is required, then we set "required" to True for the hash
                    if (i.value.required) {
                        if (!req_map{i.value.inLoc}) {
                            req_map{i.value.inLoc} = True;
                        }
                        if (!required) {
                            required = True;
                        }
                    }
                    break;
                }
            }
        }

        foreach hash<auto> i in (params.pairIterator()) {
            HashDataType type(req_map{i.key} ? AutoHashType : AutoHashOrNothingType);
            map type.addField($1), i.value.iterator();
            rv.addField(new QoreDataField(i.key, "Swagger " + i.key + " parameter", type));
        }
    }

    #! Returns a field definition from the Swagger parameter
    private AbstractDataField getFieldFromSchema(string name, *string desc, SchemaObject schema,
            bool required = True, *RestClient rest) {
        return getFieldFromSchemaIntern(name, name, desc, schema, required, rest);
    }

    #! Returns a field definition from the Swagger parameter
    private AbstractDataField getFieldFromSchemaIntern(string path, string name, *string desc, SchemaObject schema,
            bool required = True, *RestClient rest, auto default_value) {
        if (schema.desc) {
            if (desc) {
                desc += ": " + schema.desc;
            } else {
                desc = schema.desc;
            }
        }
        if (schema.type == "object") {
            # check for recursive field definitions
            RecursiveFieldHelper rfh(self, "schema", path);
            return rfh.get() ?? rfh.set(new QoreDataField({
                "name": name,
                "desc": desc,
                "type": getSchemaDataType(path, schema, required, rest),
                "allowed_values": keys schema.enum,
                "default_value": default_value,
            }));
        }
        return getFieldFromParameter(path, name, schema.type, schema.format, schema.items, desc, required,
            keys schema.enum, default_value);
    }

    #! Returns the data type for the given schema
    private AbstractDataProviderType getSchemaDataType(string path, SchemaObject schema, bool required = True,
            *RestClient rest) {
        # check for recursive type definitions
        RecursiveTypeHelper rth(self, schema.uniqueHash());
        if (*AbstractDataProviderType t = rth.get()) {
            return t;
        }

        if (schema.type == "string") {
            return AbstractDataProviderType::get((schema.required ? "" : "*") + "string");
        }

        auto def_body;
        if (rest) {
            def_body = rest.preprocessRequestBody();
            if (exists def_body && def_body.typeCode() != NT_HASH) {
                remove def_body;
            }
        }

        HashDataType type(required ? AutoHashType : AutoHashOrNothingType);
        rth.set(type);
        map type.addField(getFieldFromSchemaIntern(path + "/" + $1.key, $1.key, NOTHING, $1.value,
            exists schema.required{$1.key}, NOTHING, def_body{$1.key})), schema.properties.pairIterator();
        if (schema.additionalProperties) {
            on_error printf("schema: %N\n", schema);
            type.setDefaultOtherFieldType(getType(path + "/" + schema.additionalProperties.type,
                schema.additionalProperties.type, schema.additionalProperties.format,
                schema.additionalProperties.items,
                !schema.additionalProperties.nullable));
        } else {
            # make sure that extra fields are not supported
            type.setDefaultOtherFieldType();
        }
        return type;
    }

    #! Returns a field from a Swagger parameter
    private AbstractDataField getFieldFromParameter(string path, string name, string type, *string format,
            *SchemaObject arrayItems, *string desc, bool required, *list<auto> allowed_values, auto default_value) {
        # check for recursive field definitions
        RecursiveFieldHelper rfh(self, "field", path);
        return rfh.get() ?? rfh.set(new QoreDataField({
            "name": name,
            "desc": desc,
            "type": getType(path, type, format, arrayItems, required),
            "allowed_values": allowed_values,
            "default_value": default_value,
        }));
    }

    #! Returns the data type corresponding to the values
    private AbstractDataProviderType getType(string path, string typestr, *string format, *SchemaObject arrayItems,
            bool required = True) {
        hash<auto> type_options;
        if (required) {
            type_options += {"qore.no_null": True};
        }
        if (typestr == "string") {
            switch (format) {
                case "byte": return required
                    ? AbstractDataProviderType::get(Base64BinaryType)
                    :  AbstractDataProviderType::get(Base64BinaryOrNothingType);
                case "binary": return required
                    ? AbstractDataProviderType::get(SoftBinaryType)
                    :  AbstractDataProviderType::get(SoftBinaryOrNothingType);
                case "date":
                case "date-time": return required
                    ? AbstractDataProviderType::get(SoftDateType)
                    :  AbstractDataProviderType::get(SoftDateOrNothingType);
            }
        }
        switch (typestr) {
            case "boolean":
                return required
                    ? new QoreSoftBoolDataType(type_options)
                    : new QoreBoolOrNothingDataType(type_options);

            case "file":
                return required
                    ? new QoreSoftStringDataType(type_options)
                    : new QoreSoftStringOrNothingDataType(type_options);

            case "array":
                if (!arrayItems) {
                    throw "SWAGGER-SCHEMA-ERROR", sprintf("no item definition for array path %y", path);
                }
                return new SoftListDataType(getSchemaDataType(path, arrayItems), !required);

            case "object":
                return new HashDataType();
        }

        if (*string newname = OptimalQoreSoftDataTypeMap{typestr}) {
            typestr = newname;
        }
        if (!required && (typestr != "any" && typestr != "auto")) {
            typestr = "*" + typestr;
        }
        return AbstractDataProviderType::get(new Type(typestr), type_options);
    }

    #! Returns a field of header fields
    private AbstractDataField getFieldForHeaders(string name, hash<auto> headers) {
        RecursiveFieldHelper rfh(self, "headers", name);
        if (*QoreDataField f = rfh.get()) {
            return f;
        }

        HashDataType recordType();
        map recordType.addField(getFieldFromParameter("!headers/" + $1.key, $1.key, $1.value.type, $1.value.format,
            $1.value.items, $1.value.desc, False)), headers.pairIterator();
        return rfh.set(new QoreDataField(name, "HTTP headers", recordType));
    }
}
}

namespace Priv {
const RecursiveKey = "__swagger_dp_rec_key$$";

class RecursiveFieldHelper {
    private {
        *hash<auto> tld = get_thread_data(RecursiveKey);
        string hash_str;
        string ptype;
        string name;
        *QoreDataField f;
    }

    constructor(object obj, string ptype, string name) {
        # check for recursive field definitions
        hash_str = obj.uniqueHash();
        self.ptype = ptype;
        self.name = name;

        f = tld.field{hash_str}{ptype}{name};
    }

    *QoreDataField get() {
        return f;
    }

    QoreDataField set(QoreDataField new_field) {
        tld.field{hash_str}{ptype}{name} = new_field;
        save_thread_data(RecursiveKey, tld);
        return new_field;
    }
}

class RecursiveTypeHelper {
    private {
        *hash<auto> tld = get_thread_data(RecursiveKey);
        string hash_str;
        *AbstractDataProviderType t;
    }

    constructor(object obj, string hash_str) {
        # check for recursive field definitions
        self.hash_str = hash_str;

        t = tld.type{hash_str};
    }

    *AbstractDataProviderType get() {
        return t;
    }

    AbstractDataProviderType set(AbstractDataProviderType new_type) {
        tld.type{hash_str} = new_type;
        save_thread_data(RecursiveKey, tld);
        return new_type;
    }
}

class RecursiveSearchHelper {
    destructor() {
        delete_thread_data(RecursiveKey);
    }
}
}