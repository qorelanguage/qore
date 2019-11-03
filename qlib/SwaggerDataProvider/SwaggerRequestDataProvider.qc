# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SwaggerRequestDataProvider module definition

/** SwaggerRequestDataProvider.qc Copyright 2019 Qore Technologies, s.r.o.

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

#! contains all public definitions in the SwaggerRequestDataProvider module
public namespace SwaggerRequestDataProvider {
#! The Swagger data provider class
public class SwaggerRequestDataProvider inherits AbstractDataProvider {
    public {
        #! The Swagger schema
        SwaggerSchema schema;

        #! The REST client object for API calls
        *RestClient rest;

        #! current URI path
        string uri_path = "/";

        #! The operation object
        OperationObject op;
    }

    #! Creates the object from the arguments
    constructor(SwaggerSchema schema, *RestClient rest, string uri_path, OperationObject op) {
        self.schema = schema;
        self.rest = rest;
        self.uri_path = uri_path;
        self.op = op;
    }

    #! Returns the data provider name
    string getName() {
        return schema.info.title + uri_path + "/" + op.method.upr() + "/" + "request";
    }

    #! Returns data provider info
    hash<DataProviderInfo> getInfo() {
        hash<DataProviderInfo> rv({
            "name": getName(),
        });
        rv += getStaticInfoImpl();
        rv += getOperationInfo(op.method);
        return rv;
    }

    #! Returns the description of the record type, if any
    *hash<string, AbstractDataField> getRecordType() {
        hash<string, AbstractDataField> rv;
        if (op.parameters) {
            rv.params = getFieldFromParameters("parameters", op.parameters);
        }
        if (op.body) {
            rv.body = getFieldFromSchema("body", cast<BodyParameter>(op.body).schema);
        }
        if (op.formData) {
            OtherParameter param = cast<OtherParameter>(op.formData);
            rv.formData = getFieldFromParameter("formData", param.type, param.desc, param.required);
        }
        return rv;
    }

    #! Returns a field definition from Swagger parameters
    AbstractDataField getFieldFromParameters(string name, hash<string, AbstractParameterObject> parameters) {
        HashDataType recordType();
        map recordType.addField(getFieldFromParameter($1.key, $1.value.type, $1.value.desc, $1.value.required)),
            parameters.pairIterator();
        return new QoreDataField(name, getName() + " record", recordType);
    }

    #! Returns a field definition from the Swagger parameter
    AbstractDataField getFieldFromSchema(string name, SchemaObject schema, bool required = True) {
        if (schema.type == "object") {
            HashDataType recordType();
            map recordType.addField(getFieldFromSchema($1.key, $1.value, schema.required{$1.key})), schema.properties.pairIterator();
            return new QoreDataField(name, schema.desc, recordType);
        }
        return getFieldFromParameter(name, schema.type, schema.desc, required);
    }

    #! Returns a field from a Swagger parameter
    AbstractDataField getFieldFromParameter(string name, string type, *string desc, bool required) {
        Type paramType;
        switch (type) {
            case "boolean":
                paramType = new Type("softbool");
                break;
            case "file":
                paramType = new Type("softstring");
                break;
            case "array":
                paramType = new Type("list<auto>");
                break;
            default:
                paramType = new Type(OptimalQoreSoftDataTypeMap{type} ?? type);
                break;
        }
        if (!required) {
            paramType = paramType.getOrNothingType();
        }
        return new QoreDataField(name, desc, paramType);
    }

    #! Returns flags for the operation supported by this object
    private static hash<auto> getOperationInfo(string method) {
        switch (method) {
            case "GET":
                return {
                    "supports_read": True,
                };
                break;
            case "POST": {
                return {
                    "supports_create": True,
                };
                break;
            }
            case "DELETE": {
                return {
                    "supports_delete": True,
                };
                break;
            }
        }

        # assume all other methods are updates
        return {
            "supports_update": True,
        };
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return SwaggerDataProvider::ProviderInfo;
    }
}
}
