# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore AbstractDataField class definition

/** AbstractDataField.qc Copyright 2019 - 2024 Qore Technologies, s.r.o.

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

    #! The display name of the field
    string display_name;

    #! The short description of the field in plain text
    *string short_desc;

    #! the description of the field, if available, with markdown formatting
    *string desc;

    #! the default value for the field
    auto default_value;

    #! for enum fields; allowed values
    *list<auto> allowed_values;

    #! type info
    hash<DataTypeInfo> type;

    #! User-defined attributes set on the field
    *hash<auto> attr;
}

#! describes a data type based on a hashdecl
public class AbstractDataField inherits Qore::Serializable {
    private {
        #! the default value for the field, if any
        auto default_value;

        #! for enum fields; raw allowed values
        list<auto> allowed_values;

        #! for enum fields; allowed values
        /** only supported for types that convert to a string
        */
        hash<string, bool> allowed_values_map;

        #! User-defined custom attributes set on the field
        hash<auto> attr;
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
    AbstractDataField setOption(string opt, auto value) {
        getType().setOption(opt, value);
        return self;
    }

    #! sets the given options on the field's type
    /** @param options a hash of options, if any options match supported options for this type, they are set

        @throw TYPE-OPTION-ERROR option value has an invalid type
    */
    AbstractDataField setOptions(hash<auto> options) {
        getType().setOptions(options);
        return self;
    }

    #! returns the type name
    string getTypeName() {
        return getType().getName();
    }

    #! Sets allowed values for enum types
    AbstractDataField setAllowedValues(list<auto> allowed_values) {
        AbstractDataProviderType type = getType();
        if (allowed_values[0] instanceof hash<AllowedValueInfo>) {
            allowed_values = map $1.value, allowed_values;
        }
        # check that the value can be assigned to the type
        self.allowed_values = map type.acceptsValue($1), allowed_values;
        # NOTE: requires that values can be converted to a string hash key to simulate a set
        allowed_values_map = map {$1: True}, self.allowed_values;
        return self;
    }

    #! sets the default value for the field
    AbstractDataField setDefaultValue(auto default_value) {
        # check that the value can be assigned to the type
        self.default_value = getType().acceptsValue(default_value);
        return self;
    }

    #! get default value, if any
    auto getDefaultValue() {
        return default_value ?? getType().getDefaultValue();
    }

    #! Get allowed values
    *list<auto> getAllowedValues() {
        return allowed_values;
    }

    #! Get allowed values
    *hash<string, bool> getAllowedValueMap() {
        return allowed_values_map;
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
            if (allowed_values) {
                # first do any value / type filtering / conversions
                value = getType().acceptsValue(value);
                if (!allowed_values_map{value}) {
                    throw "FIELD-VALUE-ERROR", sprintf("field %y does not accept value %y; allowed values: %y", getName(),
                        value, allowed_values);
                }
                return value;
            }
            return getType().acceptsValue(value);
        } catch (hash<ExceptionInfo> ex) {
            rethrow ex.err, sprintf("%s (while setting field %y)", ex.desc, getName()), ex.arg;
        }
    }

    #! returns information about the field as an input field
    /** the \c default_value key is not returned, and the \c type key returns only input information
    */
    hash<DataFieldInfo> getInputInfo() {
        return <DataFieldInfo>{
            "name": getName(),
            "display_name": getDisplayName(),
            "short_desc": getShortDescription(),
            "desc": getDescription(),
            "type": getType().getInputInfo(),
            "attr": getAttributes(),
        };
    }

    #! returns information about the field
    hash<DataFieldInfo> getInfo() {
        return <DataFieldInfo>{
            "name": getName(),
            "display_name": getDisplayName(),
            "short_desc": getShortDescription(),
            "desc": getDescription(),
            "default_value": default_value,
            "allowed_values": allowed_values,
            "type": getType().getInfo(),
            "attr": getAttributes(),
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
    /** The base class method returns the same field; this method must be overridden in child classes to return a
        field with a usable "or nothing" type

        @return a field with an "or nothing" type equivalent to the current type
    */
    AbstractDataField getOrNothingType() {
        return self;
    }

    #! Returns the display name of the field
    string getDisplayName() {
        return AbstractDataField::getDisplayName(getName());
    }

    #! Returns the short description, if any
    *string getShortDescription() {
        return AbstractDataField::getShortDescription(getDescription());
    }

    #! Returns custom attributes set on the type
    *hash<auto> getAttributes() {
        return attr;
    }

    #! Sets custom attributes
    AbstractDataField setAttributes(hash<auto> attr) {
        self.attr += attr;
        return self;
    }

    #! Replaces all custom attributes
    replaceAttributes(*hash<auto> attr) {
        remove self.attr;
        if (attr) {
            self.attr += attr;
        }
    }

    #! Returns any example value for the field, otherwise return example data from the type
    /** @return if a default value exists, it is returned first, then the example value for the type, if any

        @since DataProvider 3.0
    */
    auto getExampleValue() {
        return default_value ?? getType().getExampleValue();
    }

    #! Converts a long description into a short description
    static *string getShortDescription(*string desc, *int maxlen) {
        if (!exists desc) {
            return;
        }
        # convert backquotes to single quotes
        desc =~ s/`/'/g;
        # end after the first sentence or newline, if any
        desc =~ s/[\.;\n].*//s;
        trim desc;
        if (maxlen > 0) {
            splice desc, maxlen;
        }
        return desc;
    }

    #! Converts a technical name into a display name
    static string getDisplayName(string name) {
        # convert dashes and underscores to spaces
        name =~ s/[-_]/ /g;
        bool wb;
        string display_name;
        for (int i = 0, int e = name.length(); i < e; ++i) {
            string c = name[i];
            if (!i || wb) {
                display_name += c.upr();
                wb = False;
                continue;
            }
            if (c == "." || c == " ") {
                display_name += " ";
                wb = True;
                continue;
            }
            if (c =~ /[a-z]/ && display_name =~ /[^ ][A-Z]$/ && i > 1) {
                splice display_name, -1, 0, " ";
            }
            display_name += c;
        }
        return display_name;
    }

    #! Converts a display name into a technical name
    static string getNameFromDisplayName(string display_name) {
        string name = display_name;
        # convert the name to all lower case
        name =~ tr/A-Z/a-z/;
        # convert spaces to dashes
        name =~ s/\s+/-/g;
        # remove all punctuation and special characters
        name =~ s/[!@#\$%\^&*\(\)_=\+\{\}\[\]|\\;':",\.\/<>\?`~]//g;
        return name;
    }

    #! returns the name of the field
    abstract string getName();

    #! returns the description, if any
    abstract *string getDescription();

    #! returns the type of the field
    abstract AbstractDataProviderType getType();
}
}
