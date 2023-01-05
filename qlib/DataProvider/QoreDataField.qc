# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore QoreDataField class definition

/** QoreDataField.qc Copyright 2019 - 2023 Qore Technologies, s.r.o.

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
#! describes a data type based on a hashdecl
public class QoreDataField inherits AbstractDataField {
    private {
        #! name of the field
        string name;

        #! description of the field
        *string desc;

        #! field type
        AbstractDataProviderType type;
    }

    #! creates the field from the arguments
    constructor(string name, *string desc, Type type, auto default_value, *softlist<auto> allowed_values) {
        self.name = name;
        self.desc = desc;
        self.type = AbstractDataProviderType::get(type);
        if (exists default_value) {
            setDefaultValue(default_value);
        }
        if (exists allowed_values) {
            setAllowedValues(allowed_values);
        }
    }

    #! creates the field from the arguments
    constructor(string name, *string desc, AbstractDataProviderType type, auto default_value,
            *softlist<auto> allowed_values) {
        self.name = name;
        self.desc = desc;
        self.type = type;
        if (exists default_value) {
            setDefaultValue(default_value);
        }
        if (exists allowed_values) {
            setAllowedValues(allowed_values);
        }
    }

    #! returns the name of the field
    string getName() {
        return name;
    }

    #! returns the description, if any
    *string getDescription() {
        return desc;
    }

    #! returns the type of the field
    AbstractDataProviderType getType() {
        return type;
    }

    #! adds a string to the description
    /** @param separator string to add first if there is already a description
        @param new_desc the new string to add to the description
    */
    appendDescription(string separator, string new_desc) {
        if (desc) {
            desc += separator;
        }
        desc += new_desc;
    }

    #! returns information about the field
    hash<DataFieldInfo> getInfo() {
        return <DataFieldInfo>{
            "name": name,
            "desc": desc,
            "default_value": default_value,
            "type": type.getInfo(),
        };
    }

    #! returns a field with a "soft" type equivalent to the current type
    /** @return a field with a "soft" type equivalent to the current type
    */
    AbstractDataField getSoftType() {
        return getNewFieldType(type.getSoftType());
    }

    #! returns a field with an "or nothing" type equivalent to the current type
    /** @return a field with an "or nothing" type equivalent to the current type
    */
    AbstractDataField getOrNothingType() {
        return getNewFieldType(type.getOrNothingType());
    }

    #! returns a field with the new type
    /** @return a field with the new type
    */
    private AbstractDataField getNewFieldType(AbstractDataProviderType new_type) {
        AbstractDataProviderType type = getType();
        if (new_type == type) {
            return self;
        }
        QoreDataField rv = copy();
        rv.type = new_type;
        return rv;
    }
}
}
