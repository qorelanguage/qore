# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SwaggerDataProvider module definition

/** SwaggerDataProviderBase.qc Copyright 2019 Qore Technologies, s.r.o.

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
#! The Swagger data provider base class
public class SwaggerDataProviderBase inherits AbstractDataProvider {
    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        return SwaggerDataProvider::ProviderInfo;
    }

    #! Returns a field definition from Swagger parameters
    private *hash<string, AbstractDataField> getRequestRecordFromParameters(string name, hash<string, AbstractParameterObject> parameters) {
        hash<string, AbstractDataField> rv;
        # temporary hash for types
        hash<string, HashDataType> params;
        foreach hash<auto> i in (parameters.pairIterator()) {
            switch (i.value.inLoc) {
                case "body": {
                    rv.body = getFieldFromSchema("body", "REST request body argument", i.value.schema, i.value.required);
                    break;
                }
                case "formData":
                case "query": {
                    if (!rv{i.value.inLoc}) {
                        params{i.value.inLoc} = new HashDataType();
                        rv{i.value.inLoc} = new QoreDataField("query", "REST request query arguments", params{i.value.inLoc});
                    }
                    params{i.value.inLoc}.addField(getFieldFromParameter(i.value.name, i.value.type, i.value.items, i.value.desc, i.value.required));
                    break;
                }
                case "header": {
                    if (!rv.header) {
                        params.header = new HashDataType();
                        rv.header = new QoreDataField("query", "REST request query arguments", params.header);
                    }
                    params.header.addField(getFieldFromParameter(i.value.name, "string", NOTHING, i.value.desc, i.value.required));
                    break;
                }
                # ignore path
            }
        }

        return rv;
    }

    #! Returns a field definition from the Swagger parameter
    private AbstractDataField getFieldFromSchema(string name, *string desc, SchemaObject schema, bool required = True) {
        if (schema.desc) {
            if (desc) {
                desc += ": " + schema.desc;
            } else {
                desc = schema.desc;
            }
        }
        if (schema.type == "object") {
            return new QoreDataField(name, desc, getSchemaDataType(schema, required));
        }
        return getFieldFromParameter(name, schema.type, schema.items, desc, required);
    }

    #! Returns the data type for the given schema
    private AbstractDataProviderType getSchemaDataType(SchemaObject schema, bool required = True) {
        HashDataType type(required ? AutoHashType : AutoHashOrNothingType);
        map type.addField(getFieldFromSchema($1.key, NOTHING, $1.value, schema.required{$1.key})), schema.properties.pairIterator();
        return type;
    }

    #! Returns a field from a Swagger parameter
    private AbstractDataField getFieldFromParameter(string name, string type, *SchemaObject arrayItems, *string desc, bool required) {
        return new QoreDataField(name, desc, getType(type, arrayItems, required));
    }

    #! Returns the data type corresponding to the values
    private AbstractDataProviderType getType(string typestr, *SchemaObject arrayItems, bool required = True) {
        switch (typestr) {
            case "boolean":
                return required ? new QoreBoolDataType() : new QoreBoolOrNothingDataType();

            case "file":
                return required ? new QoreStringDataType() : new QoreStringOrNothingDataType();

            case "array":
                return new ListDataType(getSchemaDataType(arrayItems), !required);
        }

        return AbstractDataProviderType::get(new Type(OptimalQoreDataTypeMap{typestr} ?? typestr));
    }

    #! Returns a field of header fields
    private AbstractDataField getFieldForHeaders(string name, hash<auto> headers) {
        HashDataType recordType();
        map recordType.addField(getFieldFromParameter($1.key, $1.value.type, $1.value.items, $1.value.desc, False)), headers.pairIterator();
        return new QoreDataField(name, "HTTP headers", recordType);
    }
}
}
