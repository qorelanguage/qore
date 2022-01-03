# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore HashDataType class definition

/** HashDataType.qc Copyright 2019 - 2022 Qore Technologies, s.r.o.

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

# assume local scope for variables, do not use "$" signs
%new-style
# require type definitions everywhere
%require-types
#! strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

%requires reflection

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! describes a data type based on a hash
/** @note objects of this type are only compatible if their fields are compatible; non-matching fields of other types
    are automatically considered compatible; change the default "other field type" by calling
    setDefaultOtherFieldType() to change this; calling setDefaultOtherFieldType() with no argument removes
    compatibility with non-matching fields in other types
*/
public class HashDataType inherits QoreDataType {
    private {
        #! the name of the type
        string name;

        #! fields
        hash<string, AbstractDataField> fields;

        #! allow other fields
        *AbstractDataProviderType default_other_field_type = AbstractDataProviderType::get(AbstractDataProviderType::anyType);

        #! if the type requires validation
        bool has_default_other_field_type = False;
    }

    #! creates the object and assigns the name as the type
    constructor(string name = AutoHashType.getName(), *hash<auto> options) : QoreDataType(AutoHashType, options) {
        self.name = name;
    }

    #! creates the object and assigns the name as the given name or the base type's name
    constructor(Type base_type, *string name, *hash<auto> options) : QoreDataType(base_type, options) {
        self.name = name ?? base_type.getName();
    }

    #! creates the object from the given record description and assigns the name as the type
    constructor(string name = AutoHashType.getName(), hash<string, AbstractDataField> fields, *hash<auto> options)
        : QoreDataType(AutoHashType, options) {
        self.name = name;
        self.fields = fields;
    }

    #! Sets the default field type for unlisted fields
    setDefaultOtherFieldType(*AbstractDataProviderType default_other_field_type) {
        self.default_other_field_type = default_other_field_type;
        if (default_other_field_type) {
            has_default_other_field_type = default_other_field_type.hasType();
        } else {
            has_default_other_field_type = False;
        }
    }

    #! Returns the type name
    string getName() {
        return name;
    }

    #! Returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type

        @throw RUNTIME-TYPE-ERROR value cannot be assigned to type
    */
    auto acceptsValue(auto value) {
        # {} + ensure that the hash value is "hash<auto>"
        value = {} + QoreDataType::acceptsValue(value);
        hash<auto> value_copy = value;
        # check fields if any are defined
        foreach AbstractDataField field in (fields.iterator()) {
            string key = field.getName();
            auto val = remove value_copy{key};
            if (!exists val && !field.isMandatory()) {
                continue;
            }
            try {
                value{key} = field.acceptsValue(val);
            } catch (hash<ExceptionInfo> ex) {
                throw "RUNTIME-TYPE-ERROR", sprintf("error in type %y field %y: %s: %s", getName(), key, ex.err,
                    ex.desc);
            }
        }
        if (value_copy) {
            if (default_other_field_type) {
                if (has_default_other_field_type) {
                    foreach hash<auto> i in (value_copy.pairIterator()) {
                        try {
                            value{i.key} = default_other_field_type.acceptsValue(i.value);
                        } catch (hash<ExceptionInfo> ex) {
                            throw "RUNTIME-TYPE-ERROR", sprintf("error in type %y default field %y: %s: %s", getName(),
                                i.key, ex.err, ex.desc);
                        }
                    }
                } else {
                    value += value_copy;
                }
            } else if (fields) {
                throw "RUNTIME-TYPE-ERROR", sprintf("error in type %y unsupported fields in value: %y; known fields: %y",
                    getName(), keys value_copy, keys fields);
            }
        }
        return value;
    }

    #! Returns True if this type can be assigned from values of the argument type
    /** @note objects of this type are only compatible if their fields are compatible; if either side is a hash
        without declared fields, then they are compatible
    */
    bool isAssignableFrom(AbstractDataProviderType t) {
        *hash<string, AbstractDataField> other_fields = t.getFields();
        if (fields && other_fields) {
            foreach AbstractDataField field in (fields.iterator()) {
                string key = field.getName();
                *AbstractDataField other_field = remove other_fields{key};
                if (!other_field) {
                    if (!field.isMandatory()) {
                        continue;
                    } else {
                        return False;
                    }
                }
                if (!field.getType().isAssignableFrom(other_field.getType())) {
                    return False;
                }
            }
            if (other_fields) {
                if (!default_other_field_type) {
                    return False;
                }
                if (!has_default_other_field_type) {
                    return True;
                }
                foreach AbstractDataField other_field in (other_fields) {
                    if (!default_other_field_type.isAssignableFrom(other_field.getType())) {
                        return False;
                    }
                }
            }
        }
        return QoreDataType::isAssignableFrom(t);
    }

    #! adds a field to the type
    addField(AbstractDataField field) {
        fields{field.getName()} = field;
    }

    #! Returns the given field, if present, or @ref nothing if not
    *AbstractDataField getField(string name) {
        return fields{name};
    }

    #! Returns the fields of the data structure; if any
    *hash<string, AbstractDataField> getFields() {
        return fields;
    }

    #! Returns a "soft" type equivalent to the current type
    /** @return a "soft" type equivalent to the current type
    */
    AbstractDataProviderType getSoftType() {
        if (soft_type) {
            return self;
        }
        HashDataType rv(type, name, options);
        map rv.addField($1.getSoftType()), getFields().iterator();
        rv.soft_type = True;
        return rv;
    }

    #! Returns an "or nothing" type equivalent to the current type
    /** @return an "or nothing" type equivalent to the current type
    */
    AbstractDataProviderType getOrNothingType() {
        # recursively return "or nothing" typed fields in any case
        HashDataType rv = QoreDataType::getOrNothingType();
        map rv.fields{$1.key} = $1.value.getOrNothingType(), fields.pairIterator();
        return rv;
    }
}
}
