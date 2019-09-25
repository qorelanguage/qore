# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SqlUtilColumnDataType class definition

/** SqlUtilColumnDataType.qc Copyright 2019 Qore Technologies, s.r.o.

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

#! contains all public definitions in the SqlUtil module
public namespace SqlUtil {
#! generic type for SQL datatypes where no translations are provided for NULL for "or nothing" types
/**
    @since SqlUtil 1.6
*/
public class SqlUtilColumnDataType inherits DataProvider::AbstractDataProviderType {
    private {
        #! the type name
        string name;

        #! wrapped type
        DataProvider::AbstractDataProviderType type;

        #! hash of base types where no translations are performed
        hash<string, bool> base_type_hash;
    }

    #! creates the object
    constructor(string native_type, string type, *hash<auto> options) {
        name = native_type;
        self.type = AbstractDataProviderType::get(new Type(OptimalQoreSoftDataTypeMap{type} ?? type),
            options + {"qore.no_null": True});
        init();
    }

    #! creates the object
    constructor(DataProvider::AbstractDataProviderType type) : AbstractDataProviderType() {
        self.type = type;
        init();
    }

    #! returns a hash of native base type code where no translations are performed
    *hash<string, bool> getDirectTypeHash() {
        return base_type_hash;
    }

    #! returns the type name
    string getName() {
        return name;
    }

    #! returns the base type for the type, if any
    *Type getValueType() {
        return type.getValueType();
    }

    #! returns the subtype (for lists or hashes) if there is only one
    *AbstractDataProviderType getElementType() {
        return type.getElementType();
    }

    #! returns the fields of the data structure; if any
    *hash<string, AbstractDataField> getFields() {
        return type.getFields();
    }

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type
    */
    auto acceptsValue(auto value) {
        if (base_type_hash{value.typeCode()}) {
            return value;
        }
        return type.acceptsValue(value);
    }

    #! returns the value of the given option
    auto getOptionValue(string opt) {
        return type.getOptionValue(opt);
    }

    #! returns options set on the type
    *hash<auto> getOptions() {
        return type.getOptions();
    }

    #! returns supported options
    *hash<string, hash<DataProviderTypeOptionInfo>> getSupportedOptions() {
        return type.getSupportedOptions();
    }

    #! sets the given option on the type
    /** @param opt the option to set
        @param value the value to set

        @throw TYPE-OPTION-ERROR invalid option or invalid option type
    */
    setOption(string opt, auto value) {
        type.setOption(opt, value);
    }

    #! returns a hash of types accepted by this type
    hash<string, bool> getAcceptTypeHash() {
        return type.getAcceptTypeHash();
    }

    #! returns a hash of types returned by this type
    hash<string, bool> getReturnTypeHash() {
        return type.getReturnTypeHash();
    }

    #! initializes the object
    private init() {
        base_type_hash = type.getDirectTypeHash();
        if (base_type_hash{NT_NOTHING}) {
            base_type_hash{NT_NULL} = True;
        }
    }
}
}
