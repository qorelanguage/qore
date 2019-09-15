# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore AbstractDataProviderType class definition

/** AbstractDataProviderType.qc Copyright 2019 Qore Technologies, s.r.o.

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

%requires reflection

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! describes type options
public hashdecl DataProviderTypeOptionInfo {
    #! the option value type
    string type;

    #! the description of the option
    string desc;
}

#! describes a data type
public class AbstractDataProviderType {
    private {
        static Type nothingType("nothing");
        static Type anyType("any");

        #! type options
        hash<auto> options;
    }

    #! creates the type
    constructor() {
    }

    #! creates the type and sets options
    /** @param options a hash of options, if any options match supported options for this type, they are set

        @throw TYPE-OPTION-ERROR option value has an invalid type
    */
    constructor(hash<auto> options) {
        foreach hash<auto> i in (getSupportedOptions().pairIterator()) {
            if (exists (auto value = options{i.key})) {
                string value_type = value.type();
                if (value_type == "object") {
                    value_type = value.fullType();
                }
                if (i.value.type != value_type) {
                    throw "TYPE-OPTION-ERROR", sprintf("type %y expects type %y for option %y; got type %y instead "
                        "(value: %y)", getName(), i.value.type, i.key, value.type());
                }
                setOptionIntern(i.key, value);
            }
        }
    }

    #! returns True if this type can be assigned from values of the argument type
    bool isAssignableFrom(AbstractDataProviderType t) {
        *Type othertype = t.getValueType();
        if (!othertype) {
            return False;
        }
        return isAssignableFrom(othertype);
    }

    #! returns True if this type can be assigned from values of the argument type
    bool isAssignableFrom(Type t) {
        *Type thistype = getValueType();
        if (!thistype) {
            return False;
        }
        return thistype.isAssignableFrom(t);
    }

    #! returns True if this type is a list
    bool isList() {
        *Type thistype = getValueType();
        return thistype ? AutoListOrNothingType.isOutputCompatible(thistype) : False;
    }

    #! returns True if the type must have a value
    bool isMandatory() {
        *Type thistype = getValueType();
        return thistype ? !thistype.isOrNothingType() : True;
    }

    #! returns the given field, if present, or @ref nothing if not
    *AbstractDataField getField(string field_name) {
        return getFields().field_name;
    }

    #! returns True if the type is not a wildcard type
    bool hasType() {
        *Type thistype = getValueType();
        return !thistype || thistype.hasType();
    }

    #! returns the base type code for the type
    int getBaseTypeCode() {
        *Type thistype = getValueType();
        return thistype
            ? thistype.getBaseTypeCode()
            : NT_ALL;
    }

    #! returns a hash of native base type code where no translations are performed
    *hash<string, bool> getDirectTypeHash() {
        *Type thistype = getValueType();
        if (thistype) {
            return {
                thistype.getBaseTypeCode(): True,
            };
        }
    }

    #! returns True if the type also accepts @ref nothing
    bool isOrNothingType() {
        *Type thistype = getValueType();
        return thistype
            ? thistype.isOrNothingType()
            : False;
    }

    #! get the given field type if it exists, otherwise return @ref nothing
    *AbstractDataProviderType getFieldType(string field_name) {
        *AbstractDataField field = getField(field_name);
        if (field) {
            return field.getType();
        }
    }

    #! returns the value of the given option
    auto getOptionValue(string opt) {
        return options{opt};
    }

    #! returns options set on the type
    *hash<auto> getOptions() {
        return options;
    }

    #! returns supported options
    *hash<string, hash<DataProviderTypeOptionInfo>> getSupportedOptions() {
        # this method intentionally left empty
    }

    #! sets the given option on the type
    /** @param opt the option to set
        @param value the value to set

        @throw TYPE-OPTION-ERROR invalid option or invalid option type
    */
    setOption(string opt, auto value) {
        if (!(*hash<string, hash<DataProviderTypeOptionInfo>> options = getSupportedOptions()){opt}) {
            throw "TYPE-OPTION-ERROR", sprintf("option %y is not supported by type %y; supported options: %y",
                opt, getName(), keys options);
        }
        string value_type = value.type();
        if (value_type == "object") {
            value_type = value.fullType();
        }
        if (options{opt}.type != value_type) {
            throw "TYPE-OPTION-ERROR", sprintf("type %y expectys type %y for option %y; got type %y instead "
                "(value: %y)", getName(), options{opt}.type, opt, value.type(), value);
        }

        setOptionIntern(opt, value);
    }

    #! returns the type name
    abstract string getName();

    #! returns the base type for the type, if any
    abstract *Type getValueType();

    #! returns the subtype (for lists or hashes) if there is only one
    abstract *AbstractDataProviderType getElementType();

    #! returns the fields of the data structure; if any
    abstract *hash<string, AbstractDataField> getFields();

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type
    */
    abstract auto acceptsValue(auto value);

    #! returns an appropriate object for the given type
    static AbstractDataProviderType get(Type type, *hash<auto> options) {
        if (type.isTypedHash()) {
            return new HashDeclDataType(type, options);
        } else if (type.getBaseTypeCode() == NT_HASH) {
            return new QoreHashDataType(type, options);
        } else if (type.getBaseTypeCode() == NT_STRING) {
            return new QoreStringTypeBase(type, options);
        }
        return new QoreDataType(type, options);
    }

    #! sets the given option without any validation of the option
    private setOptionIntern(string opt, auto value) {
        options{opt} = value;
    }
}
}
