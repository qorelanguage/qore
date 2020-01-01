# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore SalesforceRestDataProvider module definition

/** SalesforceRestObjectDataProvider.qc Copyright 2019 Qore Technologies, s.r.o.

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
# strict argument handling
%strict-args
# enable all warnings
%enable-all-warnings

#! contains all public definitions in the SalesforceRestDataProvider module
public namespace SalesforceRestDataProvider {
#! contains Salesforce object record information
hashdecl SalesforceRestRecordInfo {
    #! The record type for the object
    hash<string, AbstractDataField> record_type;

    #! The original field type names
    hash<string, string> field_types;

    #! Hash of date/time fields for this record
    hash<string, bool> date_time_fields;

    #! Hash of date fields for this record
    hash<string, bool> date_fields;

    #! Hash of time fields for this record
    hash<string, bool> time_fields;
}

#! The SalesforceRestObjectDataProvider data provider class
public class SalesforceRestObjectDataProvider inherits AbstractDataProvider {
    public {
        #! The REST client object for API calls
        SalesforceRestClient rest;

        #! current object name
        string name;

        #! sobject description
        hash<auto> sobject;
    }

    private {
        #! The description of this object type
        hash<auto> desc;

        #! Record info for this object
        hash<SalesforceRestRecordInfo> record_info();
    }

    #! Creates the object from the arguments
    constructor(SalesforceRestClient rest, string name, hash<auto> sobject) {
        self.rest = rest;
        self.name = name;
        self.sobject = sobject;
        getRecordInfoIntern();
    }

    #! Returns the data provider name
    string getName() {
        return name;
    }

    *hash<string, AbstractDataField> getRecordType() {
        return record_info.record_type;
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()

        @see requestSearchRecordsImpl()
    */
    private AbstractDataProviderRecordIterator searchRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        return new SalesforceRestRecordIterator(rest, name, record_info, where_cond, search_options);
    }

    #! Returns data provider static info
    private hash<DataProviderInfo> getStaticInfoImpl() {
        hash<DataProviderInfo> rv = SalesforceRestDataProvider::ProviderInfo - "constructor_options" + <DataProviderInfo>{
            "search_options": {
                "columns": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderType::get(AbstractDataProviderType::anyType),
                    "desc": "column argument for the SOQL select expression",
                },
                "limit": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderType::get(IntType),
                    "desc": "the maximum number of records to return",
                },
                "offset": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderType::get(IntType),
                    "desc": "the offset number in records to return",
                },
                "groupby": <DataProviderOptionInfo>{
                    "type": new QoreListDataType(new Type("softlist<string>")),
                    "desc": "group by argument for the SOQL select expression",
                },
            },
        };
        if (record_info.record_type) {
            if (sobject.queryable) {
                rv.supports_read = True;
                rv.supports_native_search = True;
            }
            /*
            FIXME: implement
            if (sobject.createable) {
                rv.supports_create = True;
            }
            if (sobject.updateable) {
                rv.supports_update = True;
            }
            if (sobject.deletable) {
                rv.supports_delete = True;
            }
            */
        }
        return rv;
    }

    #! Retrieves the record type
    private getRecordInfoIntern() {
        desc = rest.get("sobjects/" + name + "/describe").body;
        foreach hash<auto> field in (desc.fields) {
            AbstractDataProviderType type;
            hash<auto> opts += {
                "qore.no_null": True,
            };
            record_info.field_types{field.name} = field.type;
            switch (field.type) {
                case "string":
                case "id":
                case "textarea":
                case "email":
                case "url": {
                    if (field.length) {
                        opts."string.max_size_chars" = field.length;
                    }
                    type = AbstractDataProviderType::get(field.nillable ? SoftStringOrNothingType : SoftStringType, opts);
                    break;
                }

                case "picklist":
                    string typename = field.picklistValues[0].value.type();
                    if (field.nillable) {
                        typename = "*" + typename;
                    }
                    type = AbstractDataProviderType::get(typename, opts);
                    # FIXME: add enum values when supported
                    break;

                case "datetime":
                    type = AbstractDataProviderType::get(field.nillable ? SoftDateOrNothingType : SoftDateType, opts);
                    record_info.date_time_fields{field.name} = True;
                    break;

                case "date":
                    type = AbstractDataProviderType::get(field.nillable ? SoftDateOrNothingType : SoftDateType, opts);
                    record_info.date_fields{field.name} = True;
                    break;

                case "time":
                    type = AbstractDataProviderType::get(field.nillable ? SoftStringOrNothingType : SoftStringType, opts);
                    record_info.time_fields{field.name} = True;
                    break;

                case "int":
                    type = AbstractDataProviderType::get(field.nillable ? SoftIntOrNothingType : SoftIntType, opts);
                    break;

                case "double":
                    type = AbstractDataProviderType::get(field.nillable ? SoftFloatOrNothingType : SoftFloatType, opts);
                    break;

                case "boolean":
                    type = AbstractDataProviderType::get(field.nillable ? SoftBoolOrNothingType : SoftBoolType, opts);
                    break;

                default:
                    type = AbstractDataProviderType::get(AbstractDataProviderType::anyType, opts);
                    break;
            }
            record_info.record_type{field.name} = new QoreDataField(field.name, field.label, type);
        }
    }
}
}
