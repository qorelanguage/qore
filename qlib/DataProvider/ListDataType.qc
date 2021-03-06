# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore ListDataType class definition

/** ListDataType.qc Copyright 2019 Qore Technologies, s.r.o.

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
public class ListDataType inherits AbstractDataProviderType {
    private {
        #! the name of the type
        string name;

        #! element type
        AbstractDataProviderType element_type;

        #! hash of base types accepted by this type; keys are type names
        *hash<string, bool> accept_type_hash;

        #! hash of base types returned by this type; keys are type names
        *hash<string, bool> return_type_hash;
    }

    #! creates the object
    constructor(AbstractDataProviderType element_type, bool or_nothing = False) {
        init(element_type, or_nothing);
    }

    #! creates the object
    constructor(Type element_type, bool or_nothing = False) {
        init(AbstractDataProviderType::get(element_type), or_nothing);
    }

    #! returns the type name
    string getName() {
        return name;
    }

    #! returns the base type for the type, if any
    *Type getValueType() {
        return accept_type_hash{Type::NothingType} ? AutoListOrNothingType : AutoListType;
    }

    #! returns the subtype (for lists or hashes) if there is only one
    *AbstractDataProviderType getElementType() {
        return element_type;
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

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type

        @throw RUNTIME-TYPE-ERROR value cannot be assigned to type
    */
    auto acceptsValue(auto value) {
        string typename = value.type();
        if (!accept_type_hash{typename}) {
            throw "RUNTIME-TYPE-ERROR", sprintf("type %y does not accept values of type %y", getName(),
                value.fullType());
        }
        if (typename == Type::List) {
            return map element_type.acceptsValue($1), value;
        }
    }

    #! common constructor initialization
    private init(AbstractDataProviderType element_type, bool or_nothing) {
        name = sprintf("%slist<%s>", or_nothing ? "*" : "", element_type.getName());
        self.element_type = element_type;
        accept_type_hash = return_type_hash = {
            Type::List: True,
        };
        if (or_nothing) {
            accept_type_hash{Type::NothingType} = True;
            return_type_hash{Type::NothingType} = True;
        }
    }
}
}
