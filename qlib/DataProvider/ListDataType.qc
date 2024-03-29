# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore ListDataType class definition

/** ListDataType.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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
#! describes a data type based on a hash
public class ListDataType inherits QoreDataType {
    private {
        #! the name of the type
        string name;

        #! element type
        AbstractDataProviderType element_type;
    }

    #! creates the object
    private constructor(string name, Type qelement_type, AbstractDataProviderType element_type, bool or_nothing,
            *hash<auto> options, *hash<auto> tags)
            : QoreDataType(qelement_type, options, tags) {
        self.name = name;
        self.element_type = element_type;
    }

    #! creates the object
    constructor(AbstractDataProviderType element_type, bool or_nothing = False, *hash<auto> options,
            *hash<auto> tags)
            : QoreDataType(ListDataType::getQoreType(element_type.getValueType(), or_nothing), options, tags) {
        name = sprintf("%slist<%s>", or_nothing ? "*" : "", element_type.getName());
        self.element_type = element_type;
    }

    #! creates the object
    constructor(Type element_type, bool or_nothing = False, *hash<auto> options, *hash<auto> tags)
            : QoreDataType(ListDataType::getQoreType(element_type, or_nothing), options, tags) {
        name = sprintf("%slist<%s>", or_nothing ? "*" : "", element_type.getName());
        self.element_type = AbstractDataProviderType::get(element_type);
    }

    #! creates the object
    constructor(string name, AbstractDataProviderType element_type, bool or_nothing = False, *hash<auto> options,
            *hash<auto> tags)
            : QoreDataType(ListDataType::getQoreType(element_type.getValueType(), or_nothing), options, tags) {
        self.name = name;
        self.element_type = element_type;
    }

    #! creates the object
    constructor(string name, Type element_type, bool or_nothing = False, *hash<auto> options, *hash<auto> tags)
            : QoreDataType(ListDataType::getQoreType(element_type, or_nothing), options, tags) {
        self.name = name;
        self.element_type = AbstractDataProviderType::get(element_type);
    }

    #! returns the type name
    string getName() {
        return name;
    }

    #! Returns the description
    string getDesc() {
        return sprintf("Qore type %slist<%s>", accept_type_hash{Type::NothingType} ? "*" : "",
            element_type ? element_type.getName() : "auto");
    }

    #! returns the base type for the type, if any
    *Type getValueType() {
        return type;
    }

    #! returns the subtype (for lists or hashes) if there is only one
    *AbstractDataProviderType getElementType() {
        return element_type;
    }

    #! returns the fields of the data structure; if any
    *hash<string, AbstractDataField> getFields() {
        # this method intentionally left blank
    }

    #! Returns an "or nothing" type equivalent to the current type
    /** @return an "or nothing" type equivalent to the current type
    */
    AbstractDataProviderType getOrNothingType() {
        if (or_nothing_type) {
            return self;
        }
        # recursively return "or nothing" typed fields in any case
        ListDataType rv = QoreDataType::getOrNothingType();
        if (name !~ /^\*/) {
            rv.name = "*" + name;
        }
        return rv;
    }

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type

        @throw RUNTIME-TYPE-ERROR value cannot be assigned to type
    */
    auto acceptsValue(auto value) {
        value = QoreDataType::acceptsValue(value);
        if (element_type.hasType()) {
            value = map element_type.acceptsValue($1), value;
        }
        return value;
    }

    #! Updates the element type
    updateElementType(AbstractDataProviderType element_type) {
        self.element_type = element_type;
    }

    #! Returns the underlying %Qore type for this type
    private Type getQoreType(Type element_type, bool or_nothing) {
        return new Type(sprintf("%slist<%s>", or_nothing ? "*" : "", element_type.getName()));
    }
}
}
