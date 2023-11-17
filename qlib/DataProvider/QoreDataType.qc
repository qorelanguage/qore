# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore QoreDataType class definition

/** QoreDataType.qc Copyright 2019 - 2023 Qore Technologies, s.r.o.

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

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! describes a data type based on a %Qore data type
/** @note %Qore "soft" types that do not accept @ref nothing also accept @ref null; to ensure that soft types
    inheriting this class do not accept @ref null, use the \c qore.no_null option in the constructor
*/
public class QoreDataType inherits AbstractDataProviderType {
    private {
        #! the Qore type
        Type type;

        #! flag for soft types
        bool soft_type;

        #! flag for "or nothing" types
        bool or_nothing_type;

        #! Default value for type
        auto default_value;

        #! hash of base types where no translations are performed; keys are type codes, not names
        /** @note
            - this hash wil not contain the primary type code for the type if such values might undergo translation in
              acceptsValue()
            - keys are %Qore type codes converted to integers, not type names
        */
        hash<string, bool> base_type_hash;

        #! hash of base types accepted by this type; keys are type names
        *hash<string, bool> accept_type_hash;

        #! hash of base types returned by this type; keys are type names
        *hash<string, bool> return_type_hash;

        #! hash of type codes accepted by the base type but not accepted by this type: code -> True
        hash<string, bool> blacklist_type_hash;

        #! supported options
        const SupportedOptions = {
            "qore.no_null": <DataProviderTypeOptionInfo>{
                "type": Type::Boolean,
                "desc": "if True then NULL is not supported on input if NOTHING is also not accepted",
            },
        };

        #! map from normal types to soft types
        const SoftTypeMap = {
            "int": SoftIntType,
            "bool": SoftBoolType,
            "float": SoftFloatType,
            "number": SoftNumberType,
            "string": SoftStringType,
            "date": SoftDateType,
            "list": SoftAutoListType,
            "*int": SoftIntOrNothingType,
            "*bool": SoftBoolOrNothingType,
            "*float": SoftFloatOrNothingType,
            "*number": SoftNumberOrNothingType,
            "*string": SoftStringOrNothingType,
            "*date": SoftDateOrNothingType,
            "*list": SoftAutoListOrNothingType,
        };
    }

    #! creates the type
    constructor(Type type, *hash<auto> options, *hash<auto> tags, auto default_value) {
        self.type = type;
        base_type_hash{type.getBaseTypeCode()} = True;
        if (type.isOrNothingType()) {
            base_type_hash{NT_NOTHING} = True;
        }
        accept_type_hash = type.getAcceptTypeHash();
        return_type_hash = type.getReturnTypeHash();

        soft_type = (type.getName() =~ /^.?soft/);
        or_nothing_type = type.isOrNothingType();

        # set options after setting base type info
        if (options) {
            setOptions(options);
        }
        if (tags) {
            self.tags = tags;
        }

        if (self.options."qore.no_null") {
            accept_type_hash -= "null";
            blacklist_type_hash{NT_NULL} = True;
        }

        self.default_value = default_value;
    }

    #! returns the type name
    string getName() {
        return type.getName();
    }

    #! Returns the description
    string getDesc() {
        return sprintf("Qore data type %y", type.getName());
    }

    #! returns supported options
    *hash<string, hash<DataProviderTypeOptionInfo>> getSupportedOptions() {
        return or_nothing_type ? NOTHING : SupportedOptions;
    }

    #! returns the base type for the type, if any
    *Type getValueType() {
        return type;
    }

    #! returns a hash of native base type code where no translations are performed; keys are type codes, not names
    *hash<string, bool> getDirectTypeHash() {
        return base_type_hash;
    }

    #/! Returns @ref True if this type has a default value
    /** @par Example:
        @code{.py}
    bool b = t.canConvertToScalar();
        @endcode

        @return @ref True if this type has a default value

        @since DataProvider 3.0
    */
    bool hasDefaultValue() {
        return exists default_value;
    }

    #//! Returns the default value for the type or @ref nothing if the type has no default value
    /** @par Example:
        @code{.py}
    auto v = t.getDefaultValue();
        @endcode

        @return the default value for the type or @ref nothing if the type has no default value

        @since DataProvider 3.0
    */
    auto getDefaultValue() {
        return default_value;
    }

    #! returns the subtype (for lists or hashes) if there is only one
    *AbstractDataProviderType getElementType() {
        if (*Type elementType = type.getElementType()) {
            return AbstractDataProviderType::get(elementType);
        }
    }

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type

        @throw RUNTIME-TYPE-ERROR value cannot be assigned to type
    */
    auto acceptsValue(auto value) {
        if (blacklist_type_hash && blacklist_type_hash{value.typeCode()}) {
            throw "RUNTIME-TYPE-ERROR", sprintf("type %y does not accept values of type %y", getName(),
                value.type());
        }
        if ((!exists value || value == NULL) && exists default_value) {
            return default_value;
        }
        return type.acceptsValue(value);
    }

    #! returns the fields of the data structure; if any
    *hash<string, AbstractDataField> getFields() {
        # this method intentionally left blank
    }

    #! returns a hash of base types accepted by this type; keys are type names
    hash<string, bool> getAcceptTypeHash() {
        return accept_type_hash;
    }

    #! returns a hash of base types returned by this type; keys are type names
    hash<string, bool> getReturnTypeHash() {
        return return_type_hash;
    }

    #! returns a description of the type as a hash
    hash<DataTypeInfo> getInfo() {
        hash<DataTypeInfo> rv = AbstractDataProviderType::getInfo();
        if ((base_type_hash{NT_HASH} || base_type_hash{NT_ALL}) && !rv.fields) {
            rv.can_manage_fields = True;
        }
        return rv;
    }

    #! returns a "soft" type equivalent to the current type
    /** @return a "soft" type equivalent to the current type
    */
    AbstractDataProviderType getSoftType() {
        if (!soft_type) {
            # check if a soft type exists
            *Type soft_type = SoftTypeMap{type.getName()};
            if (soft_type) {
                return new QoreDataType(soft_type, options, tags, default_value);
            }
        }
        return self;
    }

    #! returns an "or nothing" type equivalent to the current type
    /** @return an "or nothing" type equivalent to the current type
    */
    AbstractDataProviderType getOrNothingType() {
        if (!or_nothing_type) {
            QoreDataType rv = self.copy();
            rv.or_nothing_type = True;
            rv.accept_type_hash{Type::NothingType} = True;
            rv.return_type_hash{Type::NothingType} = True;
            if (!options."qore.no_null") {
                rv.accept_type_hash{Type::NullType} = True;
            }
            rv.type = type.getOrNothingType();
            return rv;
        }
        return self;
    }
}
}
