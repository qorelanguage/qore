# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore AbstractDataField class definition

/** AbstractDataField.qc Copyright 2019 - 2021 Qore Technologies, s.r.o.

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

%requires reflection

#! contains all public definitions in the DataProvider module
public namespace DataProvider {
#! describes a data type
public hashdecl DataFieldInfo {
    #! the name of the field
    string name;

    #! the description of the field, if available
    *string desc;

    #! the default value for the field
    auto default_value;

    #! type info
    hash<DataTypeInfo> type;
}

#! describes a data type based on a hashdecl
public class AbstractDataField inherits Serializable {
    private {
        #! the default value for the field, if any
        auto default_value;
    }

    #! returns True if this field's type can be assigned from values of the argument type
    bool isAssignableFrom(AbstractDataProviderType t) {
        return getType().isAssignableFrom(t);
    }

    #! returns True if this field's type can be assigned from values of the argument type
    bool isAssignableFrom(Type t) {
        return getType().isAssignableFrom(t);
    }

    #! returns True if this field's type is a list
    bool isList() {
        return getType().isList();
    }

    #! returns True if the field's type must have a value
    bool isMandatory() {
        return getType().isMandatory();
    }

    #! returns the value of the given option on the field's type
    auto getOptionValue(string opt) {
        return getType().getOptionValue(opt);
    }

    #! returns options set on the field's type
    *hash<auto> getOptions() {
        return getType().getOptions();
    }

    #! returns supported options on the field's type
    *hash<string, hash<DataProviderTypeOptionInfo>> getSupportedOptions() {
        return getType().getSupportedOptions();
    }

    #! sets the given option on the field's type
    /** @param opt the option to set
        @param value the value to set

        @throw TYPE-OPTION-ERROR invalid option or invalid option type
    */
    setOption(string opt, auto value) {
        getType().setOption(opt, value);
    }

    #! sets the given options on the field's type
    /** @param options a hash of options, if any options match supported options for this type, they are set

        @throw TYPE-OPTION-ERROR option value has an invalid type
    */
    setOptions(hash<auto> options) {
        getType().setOptions(options);
    }

    #! returns the type name
    string getTypeName() {
        return getType().getName();
    }

    #! sets the default value for the field
    setDefaultValue(auto default_value) {
        # check that the value can be assigned to the type
        self.default_value = getType().acceptsValue(default_value);
    }

    #! get default value, if any
    auto getDefaultValue() {
        return default_value;
    }

    #! returns True if the field's type is not a wildcard type
    bool hasType() {
        return getType().hasType();
    }

    #! returns the value if the value can be assigned to the type
    /** @param value the value to assign to the type

        @return the value to be assigned; can be converted by the type
    */
    auto acceptsValue(auto value) {
        if (!exists value && exists default_value) {
            return default_value;
        }
        try {
            return getType().acceptsValue(value);
        } catch (hash<ExceptionInfo> ex) {
            throw ex.err, sprintf("%s (while setting field %y)", ex.desc, getName()), ex.arg;
        }
    }

    #! returns information about the field as an input field
    /** the \c default_value key is not returned, and the \c type key returns only input information
    */
    hash<DataFieldInfo> getInputInfo() {
        return <DataFieldInfo>{
            "name": getName(),
            "desc": getDescription(),
            "type": getType().getInputInfo(),
        };
    }

    #! returns information about the field
    hash<DataFieldInfo> getInfo() {
        return <DataFieldInfo>{
            "name": getName(),
            "desc": getDescription(),
            "default_value": default_value,
            "type": getType().getInfo(),
        };
    }

    #! returns a field with a "soft" type equivalent to the current type
    /** The base class method returns the same field; this method must be overridden in child classes to return a
        field with a usable "soft" type

        @return a field with a "soft" type equivalent to the current type
    */
    AbstractDataField getSoftType() {
        return self;
    }

    #! returns a field with an "or nothing" type equivalent to the current type
    /** The base class method returns the same fielf; this method must be overridden in child classes to return a
        field with a usable "or nothing" type

        @return a field with an "or nothing" type equivalent to the current type
    */
    AbstractDataField getOrNothingType() {
        return self;
    }

    #! returns the name of the field
    abstract string getName();

    #! returns the description, if any
    abstract *string getDescription();

    #! returns the type of the field
    abstract AbstractDataProviderType getType();
}
}
