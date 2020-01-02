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
                "scope": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderType::get(StringType),
                    "desc": "the scope to use for the query",
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
                    "type": new QoreListDataType(new Type("softlist<auto>")),
                    "desc": "group by argument for the SOQL select expression",
                },
                "having": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderType::get(AutoHashType),
                    "desc": "having argument for the SOQL select expression",
                },
                "orderby": <DataProviderOptionInfo>{
                    "type": new QoreListDataType(new Type("softlist<string>")),
                    "desc": "order by argument for the SOQL select expression",
                },
                "forview": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderType::get(SoftBoolType),
                    "desc": "uses FOR VIEW with the query to update the view date",
                },
                "forreference": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderType::get(SoftBoolType),
                    "desc": "uses FOR REFERENCE with the query to update the reference date",
                },
            },
            "upsert_options": {
                "extidfield": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderType::get(StringType),
                    "desc": "the name of the external ID field in the new record data",
                    "required": True,
                },
            },
        };
        if (record_info.record_type) {
            if (sobject.queryable) {
                rv.supports_read = True;
                rv.supports_native_search = True;
            }
            if (sobject.createable) {
                rv.supports_create = True;
            }
            if (sobject.mergeable) {
                rv.supports_upsert = True;
            }
            if (sobject.deletable) {
                rv.supports_delete = True;
            }
            if (sobject.updateable) {
                rv.supports_update = True;
            }
        }
        return rv;
    }

    #! Creates the given record to the data provider
    /** @param rec a hash representing a single input record
        @param create_options the create options after processing by validateCreateOptions()

        @return the data written to the data provider with the \c "id" field of the new record
    */
    private *hash<auto> createRecordImpl(hash<auto> rec, *hash<auto> create_options) {
        string url = sprintf("sobjects/%s", name);
        hash<auto> info;
        # the id is returned in the "id" field in the response
        hash<auto> resp;
        try {
            resp = rest.post(url, rec, \info);
        } catch (hash<ExceptionInfo> ex) {
            # ensure that any error response body is included in the exception
            hash<auto> ex_arg = info{"request-uri", "response-code", "response-body"};
            throw ex.err, ex.desc, ex_arg;
        }
        if (!resp.body.success) {
            throw "UPSERT-ERROR", sprintf("error creating record: %y", resp.errors);
        }
        return rec + {"id": resp.body.id};
    }

    #! Upserts the given record to the data provider
    /** @param rec a hash representing a single input record
        @param upsert_options the create options after processing by validateUpsertOptions()

        @return see @ref db_provider_upsert_results for possible values
    */
    private string upsertRecordImpl(hash<auto> rec, *hash<auto> upsert_options) {
        string extid = upsert_options.extidfield;
        # the external ID info must be removed from the request body
        *softstring extid_value = remove rec{extid};
        if (!extid_value.val()) {
            throw "UPSERT-ERROR", sprintf("upsert request requires a value for the %y field, but none was given",
            extid);
        }
        string url = sprintf("sobjects/%s/%s/%s", name, extid, extid_value);
        hash<auto> info;
        # the id is returned in the "id" field in the response
        hash<auto> resp;
        try {
            resp = rest.patch(url, rec, \info);
        } catch (hash<ExceptionInfo> ex) {
            # ensure that any error response body is included in the exception
            hash<auto> ex_arg = info{"request-uri", "response-code", "response-body"};
            throw ex.err, ex.desc, ex_arg;
        }
        if (!resp.body.success) {
            throw "UPSERT-ERROR", sprintf("error upserting record with external ID %y = %y: %y", extid, rec{extid},
                resp.errors);
        }
        # status code 201 = inserted, 204 = updated
        return info."response-headers".status_code == 201
            ? UpsertResultInserted
            : UpsertResultUpdated;
    }

    #! Updates zero or more records matching the search options
    /** @param set the hash of field data to set
        @param where_cond a hash for identifying the record(s) to be updated
        @param search_options the update options after processing by validateSearchOptions()

        @return the number of records updated
    */
    private int updateRecordsImpl(hash<auto> set, hash<auto> where_cond, *hash<auto> search_options) {
        if (where_cond.Id) {
            updateSingleRecord(where_cond.Id, set);
            return 1;
        }
        AbstractDataProviderRecordIterator i = searchRecords(where_cond, search_options);
        int count = 0;
        map (updateSingleRecord(i.getValue().Id, set), ++count), i;
        return count;
    }

    #! Deletes zero or more records
    /**
        @param where_cond a hash for identifying the record(s) to be deleted
        @param search_options the delete options after processing by validateSearchOptions()

        @return the number of records deleted
    */
    private int deleteRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        if (where_cond.Id) {
            deleteSingleRecord(where_cond.Id);
            return 1;
        }
        AbstractDataProviderRecordIterator i = searchRecords(where_cond, search_options);
        int count;
        map (deleteSingleRecord(i.getValue().Id), ++count), i;
        return count;
    }

    #! updates a single record
    private updateSingleRecord(string id, hash<auto> set) {
        string url = sprintf("sobjects/%s/%s", name, id);
        hash<auto> info;
        # the id is returned in the "id" field in the response
        hash<auto> resp;
        try {
            resp = rest.patch(url, set, \info);
        } catch (hash<ExceptionInfo> ex) {
            # ensure that any error response body is included in the exception
            hash<auto> ex_arg = info{"request-uri", "response-code", "response-body"};
            throw ex.err, ex.desc, ex_arg;
        }
    }

    #! deletes a single record
    private deleteSingleRecord(string id) {
        string url = sprintf("sobjects/%s/%s", name, id);
        hash<auto> info;
        # the id is returned in the "id" field in the response
        hash<auto> resp;
        try {
            resp = rest.del(url, NOTHING, \info);
        } catch (hash<ExceptionInfo> ex) {
            # ensure that any error response body is included in the exception
            hash<auto> ex_arg = info{"request-uri", "response-code", "response-body"};
            throw ex.err, ex.desc, ex_arg;
        }
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

                case "currency":
                case "double":
                    type = AbstractDataProviderType::get(field.nillable ? SoftFloatOrNothingType : SoftFloatType, opts);
                    break;

                case "boolean":
                    type = AbstractDataProviderType::get(field.nillable ? SoftBoolOrNothingType : SoftBoolType, opts);
                    break;

                case "address":
                    type = new HashDataType(field.nillable ? AutoHashOrNothingType : AutoHashType, field.type, opts);
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
