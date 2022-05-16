# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore ServiceNowRestDataProvider module definition

/** ServiceNowTableDataProvider.qc Copyright 2022 Qore Technologies, s.r.o.

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

#! contains all public definitions in the ServiceNowRestDataProvider module
public namespace ServiceNowRestDataProvider {
#! contains ServiceNow object record information
hashdecl ServiceNowRestRecordInfo {
    #! The record type for the object
    hash<string, AbstractDataField> record_type;

    #! The original field type names
    hash<string, string> field_types;

    #! Hash of date/time fields for this record
    hash<string, bool> date_time_fields;

    #! Hash of boolean fields for this record
    hash<string, bool> bool_fields;

    #! Hash of integer fields for this record
    hash<string, bool> int_fields;
}

#! The ServiceNowTableDataProvider data provider class
public class ServiceNowTableDataProvider inherits DataProvider::AbstractDataProvider {
    public {
        #! The REST client object for API calls
        ServiceNowRestClient::ServiceNowRestClient rest;

        #! current object name
        string name;

        #! access description
        hash<auto> access;

        #! metadata description
        hash<auto> meta;

        #! system columns
        const SysColumns = {
            "sys_created_by": {
                "internal_type": {
                    "value": "string",
                }
            },
            "sys_created_on": {
                "internal_type": {
                    "value": "glide_date_time",
                }
            },
            "sys_updated_by": {
                "internal_type": {
                    "value": "string",
                }
            },
            "sys_updated_on": {
                "internal_type": {
                    "value": "glide_date_time",
                }
            },
        };
    }

    private {
        #! Record info for the table
        hash<ServiceNowRestRecordInfo> record_info();

        #! URI path prefix
        string uri_path = "table/";
    }

    #! Creates the object from the arguments
    constructor(ServiceNowRestClient rest, string name, hash<auto> access, hash<auto> meta) {
        self.rest = rest;
        self.name = name;
        self.access = access;
        self.meta = SysColumns + meta;
        uri_path += name;
        getRecordInfoIntern();
    }

    #! Returns the data provider name
    string getName() {
        return name;
    }

    private *hash<string, DataProvider::AbstractDataField> getRecordTypeImpl(*hash<auto> search_options) {
        return record_info.record_type;
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()

        @see requestSearchRecordsImpl()
    */
    private DataProvider::AbstractDataProviderRecordIterator searchRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
       return new ServiceNowRestRecordIterator(rest, name, record_info, where_cond, search_options);
    }

    #! Returns data provider static info
    private hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        hash<DataProviderInfo> rv = ServiceNowRestDataProvider::ProviderInfo - "constructor_options" + <DataProviderInfo>{
            "supports_children": False,
            "search_options": {
                "columns": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderType::get(AbstractDataProviderType::anyType),
                    "desc": "'select' argument for the query expression",
                },
                "limit": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderType::get(IntType),
                    "desc": "the maximum number of records to return",
                },
                "offset": <DataProviderOptionInfo>{
                    "type": AbstractDataProviderType::get(IntType),
                    "desc": "the offset in the record list for records to return",
                },
                "groupby": <DataProviderOptionInfo>{
                    "type": new QoreListDataType(new Type("softlist<auto>")),
                    "desc": "'GROUPBY' argument for the query expression",
                },
                "orderby": <DataProviderOptionInfo>{
                    "type": new QoreListDataType(new Type("softlist<string>")),
                    "desc": "'ORDERBY' argument for the query expression",
                },
                "orderbydesc": <DataProviderOptionInfo>{
                    "type": new QoreListDataType(new Type("softlist<string>")),
                    "desc": "'ORDERBYDESC' argument for the query expression",
                },
            },
        };
        if (record_info.record_type) {
            rv += {
                "has_record": True,
                "supports_read": access.read_access,
                "supports_native_search": True,
                "supports_update": access.update_access,
                # NOTE delete_access seems to always be False, while records can still be deleted
                "supports_delete": access.create_access,
                "supports_create": access.create_access,
                "search_operators": {
                    QUERY_OP_EQ: AbstractDataProvider::GenericRecordSearchOperators{DP_SEARCH_OP_EQ},
                    QUERY_OP_NE: AbstractDataProvider::GenericRecordSearchOperators{DP_SEARCH_OP_NE},
                    QUERY_OP_LT: AbstractDataProvider::GenericRecordSearchOperators{DP_SEARCH_OP_LT},
                    QUERY_OP_LE: AbstractDataProvider::GenericRecordSearchOperators{DP_SEARCH_OP_LE},
                    QUERY_OP_GT: AbstractDataProvider::GenericRecordSearchOperators{DP_SEARCH_OP_GT},
                    QUERY_OP_GE: AbstractDataProvider::GenericRecordSearchOperators{DP_SEARCH_OP_GE},
                    QUERY_OP_IN: AbstractDataProvider::GenericRecordSearchOperators{DP_SEARCH_OP_IN},
                    QUERY_OP_NOT: AbstractDataProvider::GenericRecordSearchOperators{DP_SEARCH_OP_NOT},
                    QUERY_OP_LIKE: <DataProviderSearchOperatorInfo>{
                        "name": "like",
                        "type": AbstractDataProviderType::get(Reflection::StringType),
                        "desc": "The value to bind as the 'like' value (ex: '%some string%')",
                    },
                },
            };
        }
        return rv;
    }

    #! Creates the given record to the data provider
    /** @param rec a hash representing a single input record
        @param create_options the create options after processing by validateCreateOptions()

        @return the data written to the data provider with the \c "id" field of the new record

        @throw DUPLICATE-RECORD this exception should be thrown if the provider fails due to an attempt to create a
        duplicate record
    */
    private *hash<auto> createRecordImpl(hash<auto> rec, *hash<auto> create_options) {
        hash<auto> info;
        # the id is returned in the "id" field in the response
        hash<auto> resp;
        try {
            resp = rest.post(uri_path, rec, \info);
        } catch (hash<ExceptionInfo> ex) {
            # ensure that any error response body is included in the exception
            hash<auto> ex_arg = info{"request-uri", "response-code", "response-body"};
            throw ex.err, ex.desc, ex.arg + ex_arg;
        }

        *string new_key = resp.body.response.result.sys_id;
        if (!new_key) {
            throw "SERVICENOW-RECORD-CREATE-ERROR", sprintf("failed to find new row ID in response: %y",
                resp.body.response.result);
        }
        return resp.body.response.result;
    }

    #! Updates zero or more records matching the search options
    /** @param set the hash of field data to set
        @param where_cond a hash for identifying the record(s) to be updated
        @param search_options the update options after processing by validateSearchOptions()

        @return the number of records updated
    */
    private int updateRecordsImpl(hash<auto> set, hash<auto> where_cond, *hash<auto> search_options) {
        if (where_cond.sys_id && where_cond.size() == 1) {
            updateSingleRecord(where_cond.sys_id, set);
            return 1;
        }

        AbstractDataProviderRecordIterator i = searchRecords(where_cond, search_options + {"columns": "sys_id"});
        int count = 0;
        map (updateSingleRecord(i.getValue().sys_id, set), ++count), i;
        return count;
    }

    #! Deletes zero or more records
    /**
        @param where_cond a hash for identifying the record(s) to be deleted
        @param search_options the delete options after processing by validateSearchOptions()

        @return the number of records deleted
    */
    private int deleteRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        if (where_cond.sys_id && where_cond.size() == 1) {
            deleteSingleRecord(where_cond.sys_id);
            return 1;
        }

        AbstractDataProviderRecordIterator i = searchRecords(where_cond, search_options + {"columns": "sys_id"});
        int count = 0;
        map (deleteSingleRecord(i.getValue().sys_id), ++count), i;
        return count;
    }

    #! updates a single record
    private updateSingleRecord(string id, hash<auto> set) {
        string url = sprintf("%s/%s", uri_path, id);
        hash<auto> info;
        hash<auto> resp;
        try {
            resp = rest.patch(url, set, \info);
            # the actual values set on the server (which might be different from those requested):
            #     resp.body.response.result{keys set};
            # current cannot be used, so we ignore the response
        } catch (hash<ExceptionInfo> ex) {
            # ensure that any error response body is included in the exception
            hash<auto> ex_arg = info{"request-uri", "response-code", "response-body"};
            throw ex.err, ex.desc, ex.arg + ex_arg;
        }
    }

    #! deletes a single record
    private deleteSingleRecord(string id) {
        string url = sprintf("%s/%s", uri_path, id);
        hash<auto> info;
        hash<auto> resp;
        try {
            resp = rest.del(url, NOTHING, \info);
        } catch (hash<ExceptionInfo> ex) {
            # ensure that any error response body is included in the exception
            hash<auto> ex_arg = info{"request-uri", "response-code", "response-body"};
            throw ex.err, ex.desc, ex.arg + ex_arg;
        }
    }

    #! Retrieves the record type
    private getRecordInfoIntern() {
        foreach hash<auto> i in (meta.pairIterator()) {
            AbstractDataProviderType type;
            hash<auto> opts += {
                "qore.no_null": True,
            };
            switch (i.value.internal_type.value) {
                case "integer":
                    record_info.int_fields{i.key} = True;
                    type = i.value.mandatory == "true"
                        ? AbstractDataProviderType::get(SoftIntType, opts)
                        : AbstractDataProviderType::get(SoftIntOrNothingType, opts);
                    break;

                case "string":
                case "GUID":
                    if (i.value.max_length) {
                        opts."string.max_size_chars" = i.value.max_length.toInt();
                    }
                    type = i.value.mandatory == "true"
                        ? AbstractDataProviderType::get(SoftStringType, opts)
                        : AbstractDataProviderType::get(SoftStringOrNothingType, opts);
                    break;

                case "glide_date":
                case "glide_date_time":
                    record_info.date_time_fields{i.key} = True;
                    type = i.value.mandatory == "true"
                        ? AbstractDataProviderType::get(SoftDateType, opts)
                        : AbstractDataProviderType::get(SoftDateOrNothingType, opts);
                    break;

                case "boolean":
                    record_info.bool_fields{i.key} = True;
                    type = i.value.mandatory == "true"
                        ? AbstractDataProviderType::get(SoftBoolType, opts)
                        : AbstractDataProviderType::get(SoftBoolOrNothingType, opts);
                    break;

                default:
                    type = AbstractDataProviderType::get(AbstractDataProviderType::anyType, opts);
                    break;
            }
            # get description
            *string desc = i.value.column_label;
            record_info.field_types{i.key} = type.getBaseTypeName();
            record_info.record_type{i.key} = new QoreDataField(i.key, desc, type);
        }
    }
}
}
