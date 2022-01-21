# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore AbstractDataProvider class definition

/** AbstractDataProvider.qc Copyright 2019 - 2022 Qore Technologies, s.r.o.

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
/** @defgroup db_provider_upsert_results DB Provider Upsert Result Codes
    These are the results codes returned with single-record upsert actions
*/
#@{
#! Indicates that the record was inserted
public const UpsertResultInserted = "inserted";

#! Indicates that the record was updated
public const UpsertResultUpdated = "updated";

#! Indicates that the record was verified as already in the target state
public const UpsertResultVerified = "verified";

#! Indicates that the record was left unchanged
public const UpsertResultUnchanged = "unchanged";

#! Indicates that the record was deleted
public const UpsertResultDeleted = "deleted";
#@}

#! Data provider option info
public hashdecl DataProviderOptionInfo {
    #! The option value type or types
    softlist<AbstractDataProviderType> type;

    #! Required flag
    bool required = False;

    #! The option description
    string desc;
}

#! Data provider info
public hashdecl DataProviderInfo {
    #! Data provider name
    /** @note this is a dynamic attribute; it differs for different data providers of the same type
    */
    string name;

    #! The name of the provider type
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    string type;

    #! Does the data provider support reading
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool supports_read = False;

    #! Does the data provider support record creation?
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool supports_create = False;

    #! Does the data provider support record updates?
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool supports_update = False;

    #! Does the data provider support record upserts (create or update)?
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool supports_upsert = False;

    #! Does the data provider support record deletion?
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool supports_delete = False;

    #! Does the data provider support native record searching?
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool supports_native_search = False;

    #! Does the data provider support native / optimized bulk reads?
    /** if this is @ref False and \a supports_read is @ref True, the data provider will still support bulk read
        APIs but without any performance improvements for bulk data (simulated bulk reads)

        @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool supports_bulk_read = False;

    #! Does the data provider support native / optimized bulk creation?
    /** if this is @ref False and \a supports_creation is @ref True, the data provider will still support bulk write
        APIs but without any performance improvements for bulk data (simulated bulk creates)

        @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool supports_bulk_create = False;

    #! Does the data provider support native / optimized bulk upserts?
    /** if this is @ref False and \a supports_upsert is @ref True, the data provider will still support bulk upsert
        APIs but without any performance improvements for bulk data (simulated bulk upserts)

        @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool supports_bulk_upsert = False;

    #! Does the data provider support the request API?
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool supports_request = False;

    #! Does the data provider support children?
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool supports_children = False;

    #! Does the data provider require transaction management?
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool transaction_management = False;

    #! Does the data provider provide a record?
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool has_record = False;

    #! Do we require search options to retrieve the record type?
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    bool record_requires_search_options = False;

    #! Does the data provider support creating new child providers?
    /** @note this is a static attribute; it is the same for all data providers of the same type

        @since DataProvider 2.0
    */
    bool supports_child_create = False;

    #! Does the data provider support deleting new child providers?
    /** @note this is a static attribute; it is the same for all data providers of the same type

        @since DataProvider 2.0
    */
    bool supports_child_delete = False;

    #! Does the data provider support adding new fields?
    /** @note this is a static attribute; it is the same for all data providers of the same type

        @since DataProvider 2.0
    */
    bool supports_add_field = False;

    #! Does the data provider support updating existing fields?
    /** @note this is a static attribute; it is the same for all data providers of the same type

        @since DataProvider 2.0
    */
    bool supports_update_field = False;

    #! Does the data provider support deleting fields?
    /** @note this is a static attribute; it is the same for all data providers of the same type

        @since DataProvider 2.0
    */
    bool supports_delete_field = False;

    #! Constructor options supported by the data provider for the constructor variant taking a hash
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    *hash<string, hash<DataProviderOptionInfo>> constructor_options;

    #! Create options supported by the data provider
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    *hash<string, hash<DataProviderOptionInfo>> create_options;

    #! Upsert options supported by the data provider
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    *hash<string, hash<DataProviderOptionInfo>> upsert_options;

    #! Search options supported by the data provider
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    *hash<string, hash<DataProviderOptionInfo>> search_options;

    #! Request options supported by the data provider
    /** @note this is a static attribute; it is the same for all data providers of the same type
    */
    *hash<string, hash<DataProviderOptionInfo>> request_options;

    #! Child creation options supported by the data provider
    /** @note this is a static attribute; it is the same for all data providers of the same type

        @since DataProvider 2.0
    */
    *hash<string, hash<DataProviderOptionInfo>> child_create_options;

    #! Child deletion options supported by the data provider
    /** @note this is a static attribute; it is the same for all data providers of the same type

        @since DataProvider 2.0
    */
    *hash<string, hash<DataProviderOptionInfo>> child_delete_options;

    #! Add field options supported by the data provider
    /** @note this is a static attribute; it is the same for all data providers of the same type

        @since DataProvider 2.0
    */
    *hash<string, hash<DataProviderOptionInfo>> add_field_options;

    #! Update field options supported by the data provider
    /** @note this is a static attribute; it is the same for all data providers of the same type

        @since DataProvider 2.0
    */
    *hash<string, hash<DataProviderOptionInfo>> update_field_options;

    #! Delete field options supported by the data provider
    /** @note this is a static attribute; it is the same for all data providers of the same type

        @since DataProvider 2.0
    */
    *hash<string, hash<DataProviderOptionInfo>> delete_field_options;

    #! A hash of mapper key information
    /** @note
        - this is a static attribute; it is the same for all data providers of the same type
        - the \a handler keys are not returned as they are only valid at runtime
    */
    hash<string, hash<MapperRuntimeKeyInfo>> mapper_keys = Mapper::MapperKeyInfo;

    #! A list of child data providers in this data provider
    /** @note this is a dynamic attribute; it differs for different data providers of the same type
    */
    *list<string> children;
}

#! The AbstractDataProvider class
public class AbstractDataProvider {
    public {
    }

    private {
        #! flag if callbacks are locked
        static bool callbacks_locked = False;

        #! static callback the returns a bool if the value needs dynamic resolution
        static code cb_value_needs_resolution;

        #! static callback for dynamic value resolution
        static code cb_resolve_value;
    }

    #! Returns static provider information as data; no objects are returned
    /** @param with_type_info if @ref True then more type information will be returned in the response

        @note the \c name and \c children attributes are not returned as they are dynamic attributes
    */
    hash<auto> getInfoAsData(*bool with_type_info) {
        return DataProvider::getInfoAsData(getInfo(), with_type_info);
    }

    #! Returns data provider info
    hash<DataProviderInfo> getInfo() {
        hash<DataProviderInfo> rv({
            "name": getName(),
        });
        rv += getStaticInfoImpl() + {
            "children": getChildProviderNames(),
        };
        if (!exists rv.has_record) {
            rv.has_record = rv.supports_read;
        }
        return rv;
    }

    #! Creates the given record in the data provider
    /** @param rec a hash representing a single input record
        @param create_options the create options; will be processed by validateCreateOptions()

        @return the data written to the data provider

        @throw INVALID-OPERATION the data provider does not support record creation
        @throw DUPLICATE-RECORD this exception should be thrown if the provider fails due to an attempt to create a
        duplicate record
    */
    *hash<auto> createRecord(hash<auto> rec, *hash<auto> create_options) {
        checkCreate();
        return createRecordImpl(rec, validateCreateOptions(create_options));
    }

    #! Upserts the given record in the data provider
    /** @param rec a hash representing a single input record
        @param upsert_options the upsert options; will be processed by validateUpsertOptions()

        @return see @ref db_provider_upsert_results for possible values

        @throw INVALID-OPERATION the data provider does not support upsert operations
    */
    string upsertRecord(hash<auto> rec, *hash<auto> upsert_options) {
        checkUpsert();
        return upsertRecordImpl(rec, validateUpsertOptions(upsert_options));
    }

    #! Returns the first record matching the search options
    /** @param where_cond the search criteria; will be processed by processFieldValues()
        @param search_options the search options; will be processed by validateSearchOptions()

        @throw INVALID-OPERATION the data provider does not support reading
    */
    *hash<auto> searchFirstRecord(hash<auto> where_cond, *hash<auto> search_options) {
        checkRead();
        search_options = validateSearchOptions(search_options);
        return searchFirstRecordImpl(processFieldValues(where_cond, search_options), search_options);
    }

    #! Returns a single record matching the search options
    /** @param where_cond the search criteria; will be processed by processFieldValues()
        @param search_options the search options; will be processed by validateSearchOptions()

        @throw INVALID-OPERATION the data provider does not support reading
        @throw MULTIPLE-RECORDS-ERROR multiple records found
    */
    *hash<auto> searchSingleRecord(hash<auto> where_cond, *hash<auto> search_options) {
        checkRead();
        search_options = validateSearchOptions(search_options);
        return searchSingleRecordImpl(processFieldValues(where_cond, search_options), search_options);
    }

    #! Returns an iterator iterating all records
    /** @param search_options the search options; will be processed by validateSearchOptions()

        @throw INVALID-OPERATION the data provider does not support reading
    */
    AbstractDataProviderRecordIterator getRecordIterator(*hash<auto> search_options) {
        checkRead();
        return searchRecords(NOTHING, validateSearchOptions(search_options));
    }

    #! Returns an iterator iterating all records with the bulk read API
    /** @param block_size the number of records in a read block; must be a positive number
        @param search_options the search options; will be processed by validateSearchOptions()

        @return a bulk record interface object that will return the records in bulk format

        @throw INVALID-BLOCK-SIZE the block size must be a positive number
        @throw INVALID-OPERATION the data provider does not support reading
    */
    AbstractDataProviderBulkRecordInterface getBulkRecordInterface(int block_size = 1000,
            *hash<auto> search_options) {
        checkRead();
        return searchRecordsBulk(block_size, NOTHING, validateSearchOptions(search_options));
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param block_size the number of records in a read block; must be a positive number
        @param where_cond the search criteria; will be processed by processFieldValues()
        @param search_options the search options; will be processed by validateSearchOptions()

        @return a bulk record interface object that will return the records in bulk format

        @throw INVALID-BLOCK-SIZE the block size must be a positive number
        @throw INVALID-OPERATION the data provider does not support reading
    */
    AbstractDataProviderBulkRecordInterface searchRecordsBulk(int block_size = 1000, *hash<auto> where_cond,
            *hash<auto> search_options) {
        checkRead();
        if (block_size <= 0) {
            throw "INVALID-BLOCK-SIZE", sprintf("block_size %d is invalid; the block size mst be a positive number",
                block_size);
        }
        search_options = validateSearchOptions(search_options);
        return searchRecordsBulkImpl(block_size, processFieldValues(where_cond, search_options), search_options);
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param where_cond the search criteria; will be processed by processFieldValues()
        @param search_options the search options; will be processed by validateSearchOptions()

        @throw INVALID-OPERATION the data provider does not support reading
    */
    AbstractDataProviderRecordIterator searchRecords(*hash<auto> where_cond, *hash<auto> search_options) {
        checkRead();
        search_options = validateSearchOptions(search_options);
        return searchRecordsImpl(processFieldValues(where_cond, search_options), search_options);
    }

    #! Returns an iterator for zero or more records matching the search options according to an API request
    /** @param req the request to serialize and make according to the request type
        @param where_cond the search criteria; will be processed by processFieldValues()
        @param search_options the search options after processing by validateSearchOptions()

        This will execute the request and perform a default search on any record(s) returned

        @throw INVALID-OPERATION the data provider does not support reading records or the request / response API
    */
    AbstractDataProviderRecordIterator requestSearchRecords(auto req, *hash<auto> where_cond,
            *hash<auto> search_options) {
        checkRead();
        checkRequest();
        return requestSearchRecordsImpl(req, where_cond, search_options);
    }

    #! Updates a single record matching the search options
    /** @param set the hash of field data to set; will be processed by processFieldValues()
        @param where_cond the search criteria; will be processed by processFieldValues()
        @param search_options the search options; will be processed by validateSearchOptions()

        @returns @ref True if the record was updated, @ref False if not (no record found)

        @throw INVALID-OPERATION the data provider does not support record updating
    */
    bool updateSingleRecord(hash<auto> set, hash<auto> where_cond, *hash<auto> search_options) {
        checkUpdate();
        search_options = validateSearchOptions(search_options);
        return updateSingleRecordImpl(processFieldValues(set, search_options),
            processFieldValues(where_cond, search_options), search_options);
    }

    #! Updates zero or more records matching the search options
    /** @param set the hash of field data to set
        @param where_cond a hash for identifying the record(s) to be updated
        @param search_options the search options; will be processed by validateSearchOptions()

        @return the number of records updated

        @throw INVALID-OPERATION the data provider does not support record updating
    */
    int updateRecords(hash<auto> set, *hash<auto> where_cond, *hash<auto> search_options) {
        checkUpdate();
        search_options = validateSearchOptions(search_options);
        return updateRecordsImpl(processFieldValues(set, search_options),
            processFieldValues(where_cond, search_options), search_options);
    }

    #! Deletes zero or more records
    /**
        @param where_cond a hash for identifying the record(s) to be deleted
        @param search_options the search options; will be processed by validateSearchOptions()

        @return the number of records deleted

        @throw INVALID-OPERATION the data provider does not support record deletion
    */
    int deleteRecords(*hash<auto> where_cond, *hash<auto> search_options) {
        checkDelete();
        search_options = validateSearchOptions(search_options);
        return deleteRecordsImpl(processFieldValues(where_cond, search_options), search_options);
    }

    #! Makes a request and returns the response
    /** @param req the request to serialize and make according to the request type
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request

        @throw INVALID-OPERATION the data provider does not support the request API
        @throw INVALID-REQUEST the request argument provided do not match the expected request type
    */
    auto doRequest(auto req, *hash<auto> request_options) {
        checkRequest();
        *AbstractDataProviderType request_type = getRequestType();
        if (request_type) {
            try {
                req = request_type.acceptsValue(req);
            } catch (hash<ExceptionInfo> ex) {
                throw "INVALID-REQUEST", sprintf("invalid request argument for request %y: %s: %s: %s", getName(),
                    get_ex_pos(ex), ex.err, ex.desc);
            }
        }
        return doRequestImpl(req, validateRequestOptions(request_options));
    }

    #! Creates a new child data provider and returns it after adding as a child
    /** @param name the name of the new child data provider
        @param fields the fields for the new child data provider; must have at least one field
        @param child_create_options the options for creating the new child data provider

        @return the new child data provider

        @throw INVALID-OPERATION thrown if the data provider does not support this API
        @throw CREATE-CHILD-PROVIDER-ERROR error creating the child provider

        @since DataProvider 2.0
    */
    AbstractDataProvider createChildProvider(string name, hash<string, AbstractDataField> fields,
            *hash<auto> child_create_options) {
        checkCreateChild();
        on_error rethrow "CREATE-CHILD-PROVIDER-ERROR", sprintf("%s: %s", $1.err, $1.desc), $1.arg;
        child_create_options = validateChildCreateOptions(child_create_options);
        if (!fields) {
            throw "FIELD-ERROR", "at least one field must be present in the field hash";
        }
        return createChildProviderImpl(name, fields, child_create_options);
    }

    #! Deletes a child data provider
    /** @param name the name of the new child data provider
        @param child_delete_options the options for deleting a child data provider

        @throw INVALID-OPERATION thrown if the data provider does not support this API
        @throw DELETE-CHILD-PROVIDER-ERROR error creating the child provider

        @since DataProvider 2.0
    */
    deleteChildProvider(string name, *hash<auto> child_delete_options) {
        checkDeleteChild();
        on_error rethrow "DELETE-CHILD-PROVIDER-ERROR", sprintf("%s: %s", $1.err, $1.desc), $1.arg;
        child_delete_options = validateChildDeleteOptions(child_delete_options);
        return deleteChildProviderImpl(name, child_delete_options);
    }

    #! Creates a new field and returns it after adding it
    /**
        @param field the field to add
        @param field_add_options the options for adding the new field

        @return the new child data provider

        @throw INVALID-OPERATION thrown if the data provider does not support this API
        @throw ADD-FIELD-ERROR error adding the field

        @since DataProvider 2.0
    */
    AbstractDataProvider addField(AbstractDataField field, *hash<auto> field_add_options) {
        checkAddField();
        on_error rethrow "ADD-FIELD-ERROR", sprintf("%s: %s", $1.err, $1.desc), $1.arg;
        field_add_options = validateFieldAddOptions(field_add_options);
        return addFieldImpl(field, field_add_options);
    }

    #! Updates an existing field
    /** @param name the name of the current field to update
        @param field the new definition of the field
        @param field_update_options the options for adding the new field

        @throw INVALID-OPERATION thrown if the data provider does not support this API
        @throw UPDATE-FIELD-ERROR error adding the field

        @since DataProvider 2.0
    */
    updateField(string name, AbstractDataField field, *hash<auto> field_update_options) {
        checkUpdateField();
        on_error rethrow "UPDATE-FIELD-ERROR", sprintf("%s: %s", $1.err, $1.desc), $1.arg;
        field_update_options = validateFieldUpdateOptions(field_update_options);
        return updateFieldImpl(name, field, field_update_options);
    }

    #! Deletes an existing field
    /** @param name the name of the field to delete
        @param field_delete_options the options for deleting the new field

        @throw INVALID-OPERATION thrown if the data provider does not support this API
        @throw DELETE-FIELD-ERROR error adding the field

        @since DataProvider 2.0
    */
    deleteField(string name, *hash<auto> field_delete_options) {
        checkDeleteField();
        on_error rethrow "DELETE-FIELD-ERROR", sprintf("%s: %s", $1.err, $1.desc), $1.arg;
        field_delete_options = validateFieldDeleteOptions(field_delete_options);
        return deleteFieldImpl(name, field_delete_options);
    }

    #! Returns the description of a successful request message, if any
    /** @return the request type for this provider

        @throw INVALID-OPERATION the data provider does not support the request API
    */
    *AbstractDataProviderType getRequestType() {
        checkRequest();
        return getRequestTypeImpl();
    }

    #! Returns the description of a response message, if this object represents a response message
    /** @return the response type for this response message

        @throw INVALID-OPERATION the data provider does not support the request API
    */
    *AbstractDataProviderType getResponseType() {
        checkRequest();
        return getResponseTypeImpl();
    }

    #! Returns a hash of error responses, if any
    /** @return a hash of error responses, if any, keyed by response code or string

        @throw INVALID-OPERATION the data provider does not support the request API
    */
    *hash<string, AbstractDataProviderType> getErrorResponseTypes() {
        checkRequest();
        return getErrorResponseTypesImpl();
    }

    #! Returns the type for the given error code
    /** @param error_code the error code for the response; must be a known error code, or an \c UNKNOWN-ERROR-RESPONSE
        exception is thrown
        @return the type for the given error code

        @throw INVALID-OPERATION the data provider does not support the request API
        @throw UNKNOWN-ERROR-RESPONSE the error response given is not known
    */
    AbstractDataProviderType getErrorResponseType(string error_code) {
        checkRequest();
        return getErrorResponseTypeImpl(error_code);
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any
    */
    *list<string> getChildProviderNames() {
        return getChildProviderNamesImpl();
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown

        @throw INVALID-CHILD-PROVIDER unknown child provider

        @see getChildProviderEx()
    */
    *AbstractDataProvider getChildProvider(string name) {
        return getChildProviderImpl(name);
    }

    #! Returns the given child provider or throws an exception if the given child is unknown
    /** @return the given child provider or throws an exception if the given child is unknown

        @throw INVALID-CHILD-PROVIDER unknown child provider

        @see getChildProvider()
    */
    AbstractDataProvider getChildProviderEx(string name) {
        *AbstractDataProvider provider = getChildProviderImpl(name);
        if (!provider) {
            throw "INVALID-CHILD-PROVIDER", sprintf("provider %y is unknown; known children of %y: %y", name,
                getName(), getChildProviderNames());
        }
        return provider;
    }

    #! Returns the given child provider from a \c "/" separated path string
    /** throws an exception with unknown path elements

        @param path a string giving a path to the target provider where child elements are separated by \c "/" characters

        @return the target child provider; throws an exception if any element in the path is unknown

        @throw INVALID-CHILD-PROVIDER unknown child provider
    */
    AbstractDataProvider getChildProviderPath(string path) {
        AbstractDataProvider provider = self;
        map provider = provider.getChildProviderEx($1), path.split("/");
        return provider;
    }

    #! Returns @ref True if the data provider supports transaction management
    /** @return @ref True if the data provider supports transaction management, in which case commit() or rollback()
        must be called to flush or discard data written to the data provider
    */
    bool requiresTransactionManagement() {
        return False;
    }

    #! Begins a transaction with a data provider
    /** Has no effect if the data provider does not support transaction management

        @see:
        - commit()
        - rollback()

        @since DataProvider 1.2.2
    */
    beginTransaction() {
    }

    #! Commits data written to the data provider
    /** Has no effect if the data provider does not support transaction management

        @see:
        - beginTransaction()
        - rollback()
    */
    commit() {
    }

    #! Rolls back data written to the data provider
    /** Has no effect if the data provider does not support transaction management

        @see:
        - beginTransaction()
        - commit()
    */
    rollback() {
    }

    #! Returns a bulk insert operation object for the data provider
    /** @return a bulk insert operation object for the data provider

        @throw INVALID-OPERATION the data provider does not support create operations
    */
    AbstractDataProviderBulkOperation getBulkInserter() {
        checkCreate();
        return new DefaultBulkInserter(self);
    }

    #! Returns a bulk upsert operation object for the data provider
    /** @return a bulk upsert operation object for the data provider

        @throw INVALID-OPERATION the data provider does not support upsert operations
    */
    AbstractDataProviderBulkOperation getBulkUpserter() {
        checkUpsert();
        return new DefaultBulkUpserter(self);
    }

    #! Returns custom data mapper runtime keys
    /** @return custom data mapper runtime keys

        This base method returns @ref nothing; reimplment in child classes to return a value
    */
    *hash<string, hash<MapperRuntimeKeyInfo>> getMapperRuntimeKeys() {
        # this method intentionally left empty
    }

    #! Ensures that the data provider supports read operations
    /**
        @throw INVALID-OPERATION the data provider does not support read operations
    */
    checkRead() {
        if (!supportsRead()) {
            throw "INVALID-OPERATION", sprintf("data provider %y (type %y) does not support reading records",
                getName(), self.className());
        }
    }

    #! Ensures that the data provider supports record creation
    /**
        @throw INVALID-OPERATION the data provider does not support record creation
    */
    checkCreate() {
        if (!supportsCreate()) {
            throw "INVALID-OPERATION", sprintf("data provider %y (type %y) does not support record creation",
                getName(), self.className());
        }
    }

    #! Ensures that the data provider supports record upserts
    /**
        @throw INVALID-OPERATION the data provider does not support upsert operations
    */
    checkUpsert() {
        if (!supportsUpsert()) {
            throw "INVALID-OPERATION", sprintf("data provider %y (type %y) does not support record upserts",
                getName(), self.className());
        }
    }

    #! Ensures that the data provider supports record upserts
    /**
        @throw INVALID-OPERATION the data provider does not support record updating
    */
    checkUpdate() {
        if (!supportsUpdate()) {
            throw "INVALID-OPERATION", sprintf("data provider %y (type %y) does not support record updating",
                getName(), self.className());
        }
    }

    #! Ensures that the data provider supports record deletion
    /**
        @throw INVALID-OPERATION the data provider does not support record deletion
    */
    checkDelete() {
        if (!supportsDelete()) {
            throw "INVALID-OPERATION", sprintf("data provider %y (type %y) does not support record deletion",
                getName(), self.className());
        }
    }

    #! Ensures that the data provider supports the request API
    /**
        @throw INVALID-OPERATION the data provider does not support the request API
    */
    checkRequest() {
        if (!supportsRequest()) {
            throw "INVALID-OPERATION", sprintf("data provider %y (type %y) does not support the request API",
                getName(), self.className());
        }
    }

    #! Ensures that the data provider supports creating children
    /**
        @throw INVALID-OPERATION the data provider does not support the child creation API

        @since DataProvider 2.0
    */
    checkCreateChild() {
        if (!supportsCreateChild()) {
            throw "INVALID-OPERATION", sprintf("data provider %y (type %y) does not support creating children",
                getName(), self.className());
        }
    }

    #! Ensures that the data provider supports deleting children
    /**
        @throw INVALID-OPERATION the data provider does not support the child deletion API

        @since DataProvider 2.0
    */
    checkDeleteChild() {
        if (!supportsDeleteChild()) {
            throw "INVALID-OPERATION", sprintf("data provider %y (type %y) does not support deleting children",
                getName(), self.className());
        }
    }

    #! Ensures that the data provider supports adding fields
    /**
        @throw INVALID-OPERATION the data provider does not support the add field API

        @since DataProvider 2.0
    */
    checkAddField() {
        if (!supportsAddField()) {
            throw "INVALID-OPERATION", sprintf("data provider %y (type %y) does not support adding fields",
                getName(), self.className());
        }
    }

    #! Ensures that the data provider supports updating fields
    /**
        @throw INVALID-OPERATION the data provider does not support the update field API

        @since DataProvider 2.0
    */
    checkUpdateField() {
        if (!supportsUpdateField()) {
            throw "INVALID-OPERATION", sprintf("data provider %y (type %y) does not support updating fields",
                getName(), self.className());
        }
    }

    #! Ensures that the data provider supports deleting fields
    /**
        @throw INVALID-OPERATION the data provider does not support the delete field API

        @since DataProvider 2.0
    */
    checkDeleteField() {
        if (!supportsDeleteField()) {
            throw "INVALID-OPERATION", sprintf("data provider %y (type %y) does not support deleting fields",
                getName(), self.className());
        }
    }

    #! Ensures that no callbacks can be set for dynamic URI resolution
    /** @return False if this call was ignored because callbacks are locked, True if the call was successful

        @note after this call, callbacks cannot be changed
    */
    static bool setDynamicValueCallbacks() {
        if (!AbstractDataProvider::checkCallbacks()) {
            return !exists cb_resolve_value;
        }
        return True;
    }

    #! Set callbacks for dynamic URI resolution to allow for variable URI path elements to be resolved at runtime
    /** @param value_needs_resolution signature <tt>bool sub (string value) { ... }</tt>; this callback will be used
        to check if a URI path needs dynamic resolution
        @param resolve_value signature <tt>auto sub (auto value) { ... }</tt>; if \c value_needs_resolution returns
        @ref True on a URI path, then this callback will be called at runtime for each request to resolve the URI path
        to the final path

        @return False if this call was ignored because callbacks are locked, True if the call was successful

        @note after this call, callbacks cannot be changed
    */
    static bool setDynamicValueCallbacks(code value_needs_resolution, code resolve_value) {
        if (!AbstractDataProvider::checkCallbacks()) {
            return value_needs_resolution == cb_value_needs_resolution && resolve_value == cb_resolve_value;
        }
        cb_value_needs_resolution = value_needs_resolution;
        cb_resolve_value = resolve_value;
        return True;
    }

    #! Checks if callbacks have already been set or locked
    private static bool checkCallbacks() {
        if (callbacks_locked) {
            return False;
        }
        return callbacks_locked = True;
    }

    #! validates create options
    /** @throw CREATE-OPTION-ERROR invalid or unsupported create option
    */
    private *hash<auto> validateCreateOptions(*hash<auto> create_options) {
        return checkOptions("CREATE-OPTION-ERROR", getCreateOptions(), create_options);
    }

    #! validates upsert options
    /** @throw UPSERT-OPTION-ERROR invalid or unsupported upsert option
    */
    private *hash<auto> validateUpsertOptions(*hash<auto> upsert_options) {
        return checkOptions("UPSERT-OPTION-ERROR", getUpsertOptions(), upsert_options);
    }

    #! validates search options
    /** @throw SEARCH-OPTION-ERROR invalid or unsupported search option
    */
    private *hash<auto> validateSearchOptions(*hash<auto> search_options) {
        return checkOptions("SEARCH-OPTION-ERROR", getSearchOptions(), search_options);
    }

    #! validates request options
    /** @throw REQUEST-OPTION-ERROR invalid or unsupported request option
    */
    private *hash<auto> validateRequestOptions(*hash<auto> request_options) {
        return checkOptions("REQUEST-OPTION-ERROR", getRequestOptions(), request_options);
    }

    #! validates child create options
    /** @throw CHILD-CREATE-OPTION-ERROR invalid or unsupported child create option

        @since DataProvider 2.0
    */
    private *hash<auto> validateChildCreateOptions(*hash<auto> child_create_options) {
        return checkOptions("CHILD-CREATE-OPTION-ERROR", getChildCreateOptions(), child_create_options);
    }

    #! validates child delete options
    /** @throw CHILD-DELETE-OPTION-ERROR invalid or unsupported child delete option

        @since DataProvider 2.0
    */
    private *hash<auto> validateChildDeleteOptions(*hash<auto> child_delete_options) {
        return checkOptions("CHILD-DELETE-OPTION-ERROR", getChildDeleteOptions(), child_delete_options);
    }

    #! validates field add options
    /** @throw FIELD-ADD-OPTION-ERROR invalid or unsupported field add option

        @since DataProvider 2.0
    */
    private *hash<auto> validateFieldAddOptions(*hash<auto> field_add_options) {
        return checkOptions("FIELD-ADD-OPTION-ERROR", getFieldAddOptions(), field_add_options);
    }

    #! validates field update options
    /** @throw FIELD-UPDATE-OPTION-ERROR invalid or unsupported field update option

        @since DataProvider 2.0
    */
    private *hash<auto> validateFieldUpdateOptions(*hash<auto> field_update_options) {
        return checkOptions("FIELD-UPDATE-OPTION-ERROR", getFieldUpdateOptions(), field_update_options);
    }

    #! validates field delete options
    /** @throw FIELD-DELETE-OPTION-ERROR invalid or unsupported field delete option

        @since DataProvider 2.0
    */
    private *hash<auto> validateFieldDeleteOptions(*hash<auto> field_delete_options) {
        return checkOptions("FIELD-DELETE-OPTION-ERROR", getFieldDeleteOptions(), field_delete_options);
    }

    #! processes options passed to the constructor
    private processConstructorOptions(*hash<string, hash<DataProviderOptionInfo>> option_desc, *hash<auto> options) {
        self += checkOptions("CONSTRUCTOR-OPTION-ERROR", option_desc, options);
    }

    #! verifies options according to the option
    private *hash<auto> checkOptions(string err, *hash<string, hash<DataProviderOptionInfo>> option_desc, *hash<auto> options) {
        if (!option_desc && options) {
            error(err, "invalid options: %y; no options are supported",
                keys options);
        }
        if (*list<string> invalid_keys = keys (options - (keys option_desc))) {
            error(err, "invalid options: %y; supported options: %y", invalid_keys, keys option_desc);
        }
        # check types and required keys
        foreach hash<auto> i in (option_desc.pairIterator()) {
            bool present = options.hasKey(i.key);
            if (i.value.required && !present) {
                error(err, "missing required option %y%s", i.key, i.value.desc ? sprintf(" (%s)", i.value.desc) : "");
            }
            if (!present) {
                continue;
            }
            # process value
            bool ok;
            string err;
            # issue #4062: ensure "options" remains "hash<auto>"
            options = {} + options;
            foreach AbstractDataProviderType type in (i.value.type) {
                try {
                    options{i.key} = type.acceptsValue(options{i.key});
                    ok = True;
                    break;
                } catch (hash<ExceptionInfo> ex) {
                    if (ex.err == "RUNTIME-TYPE-ERROR") {
                        err = sprintf("option %y of type %y cannot be assigned a value of type %y: %s: %s", i.key,
                            type.getName(), options{i.key}.type(), ex.err, ex.desc);
                        continue;
                    }
                    rethrow;
                }
            }
            if (!ok) {
                if (err) {
                    error("RUNTIME-TYPE-ERROR", err);
                }
                error("RUNTIME-TYPE-ERROR", "no type information available for option %y", i.key);
            }
        }
        return options;
    }

    #! processes search or set values to convert types if necessary
    /** @param h the hash of field values to process
        @param search_options the search options after processing by validateSearchOptions()

        @return processed field values, all non-hash values of \a h are processed with the field's type to ensure
        that the values are valid for the comparison or set operation; hash values are assumed to be provider-
        specific comparison or set operations and should be processed by the provider-specific method
    */
    private *hash<auto> processFieldValues(*hash<auto> h, *hash<auto> search_options) {
        if (!h) {
            return;
        }
        *hash<string, AbstractDataField> rec = getRecordTypeImpl(search_options);
        hash<auto> rv;
        foreach hash<auto> i in (h.pairIterator()) {
            if (!rec{i.key}) {
                error("INVALID-FIELD", "field %y is not a defined field; valid fields: %y", i.key, keys rec);
            }
            # we assume a hash is a provider-specific search option
            if (i.value.typeCode() == NT_HASH) {
                rv{i.key} = i.value;
                continue;
            }
            rv{i.key} = rec{i.key}.acceptsValue(i.value);
        }
        return rv;
    }

    #! thrown an exception
    private error(string err, string fmt) {
        string desc_name;
        # the getName() method cannot be run in the constructor before the object has been created and the name set
        try {
            desc_name = sprintf("%y (%y)", getName(), self.className());
        } catch (hash<ExceptionInfo> ex) {
            # this block interntioanlly left blank
        }
        if (!desc_name) {
            desc_name = sprintf("class %y", self.className());
        }
        throw err, sprintf("data provider %s: %s", desc_name, vsprintf(fmt, argv));
    }

    #! Returns the first record matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()

        @note This default method returns immediately as soon as the first matching record is found
    */
    private *hash<auto> searchFirstRecordImpl(hash<auto> where_cond, *hash<auto> search_options) {
        # scans the whole data set to find a single record
        AbstractDataProviderRecordIterator i = searchRecordsImpl(where_cond, search_options);
        if (i.next()) {
            return i.getValue();
        }
    }

    #! Returns a single record matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()

        @throw MULTIPLE-RECORDS-ERROR multiple records found

        @note This default methods searches the entire data set for a matching record
    */
    private *hash<auto> searchSingleRecordImpl(hash<auto> where_cond, *hash<auto> search_options) {
        # scans the whole data set to find a single record
        *list<*hash<auto>> records = map $1, searchRecordsImpl(where_cond, search_options);
        if (records.lsize() > 1) {
            error("MULTIPLE-RECORDS-ERROR", "%d records found for search: %y", records.size(), where_cond);
        }
        return records[0];
    }

    #! Returns a list of child data provider names, if any
    /** @return a list of child data provider names, if any

        This base class method returns @ref nothing
    */
    private *list<string> getChildProviderNamesImpl() {
        # this method intentionally left empty
    }

    #! Returns the given child provider or @ref nothing if the given child is unknown
    /** @return the given child provider or @ref nothing if the given child is unknown
    */
    private *AbstractDataProvider getChildProviderImpl(string name) {
        # this method intentionally left empty
    }

    #! Returns the description of the record type, if any
    /** @param search_options to be included and processed by validateSearchOptions() if
        \a recordRequiresSearchOptions() is @ref True for this provider, otherwise any value provided in this argument
        is ignored

        @return the record type or @ref NOTHING if the datas provider does not support a record type
    */
    *hash<string, AbstractDataField> getRecordType(*hash<auto> search_options) {
        if (!hasRecord()) {
            return;
        }
        return getRecordTypeImpl(recordRequiresSearchOptions() ? validateSearchOptions(search_options) : NOTHING);
    }

    #! Returns the description of the record type with soft types, if any
    *hash<string, AbstractDataField> getSoftRecordType(*hash<auto> search_options) {
        return map {
            $1.key: $1.value.getSoftType()
        }, getRecordType(search_options).pairIterator();
    }

    #! Returns the description of the record type with "or nothing" types, if any
    *hash<string, AbstractDataField> getOrNothingRecordType(*hash<auto> search_options) {
        return map {
            $1.key: $1.value.getOrNothingType(True)
        }, getRecordType(search_options).pairIterator();
    }

    #! Returns options that can be used for searching
    /** @return a hash of options that can be used for searching; keys are search option names, values describe the
        search option; if @ref nothing is returned, then the provider does not support any search options
    */
    *hash<string, hash<DataProviderOptionInfo>> getSearchOptions() {
        return getStaticInfoImpl().search_options;
    }

    #! Returns options that can be used for creating records
    /** @return a hash of options that can be used for creating records; keys are option names, values describe the
        option; if @ref nothing is returned, then the provider does not support any creation options
    */
    *hash<string, hash<DataProviderOptionInfo>> getCreateOptions() {
        return getStaticInfoImpl().create_options;
    }

    #! Returns options that can be used for upserting records
    /** @return a hash of options that can be used for upserting records; keys are option names, values describe the
        option; if @ref nothing is returned, then the provider does not support any upsert options
    */
    *hash<string, hash<DataProviderOptionInfo>> getUpsertOptions() {
        return getStaticInfoImpl().upsert_options;
    }

    #! Returns options that can be used for requests
    /** @return a hash of options that can be used for requests; keys are request option names, values describe the
        request option; if @ref nothing is returned, then the provider does not support any request options
    */
    *hash<string, hash<DataProviderOptionInfo>> getRequestOptions() {
        return getStaticInfoImpl().request_options;
    }

    #! Returns options that can be used for creating child data providers
    /** @return a hash of options that can be used for creating child data providers; keys are option names, values
        describe the option; if @ref nothing is returned, then the provider does not support any child creation
        options

        @since DataProvider 2.0
    */
    *hash<string, hash<DataProviderOptionInfo>> getChildCreateOptions() {
        return getStaticInfoImpl().child_create_options;
    }

    #! Returns options that can be used for deleting child data providers
    /** @return a hash of options that can be used for delete child data providers; keys are option names, values
        describe the option; if @ref nothing is returned, then the provider does not support any child deletion
        options

        @since DataProvider 2.0
    */
    *hash<string, hash<DataProviderOptionInfo>> getChildDeleteOptions() {
        return getStaticInfoImpl().child_delete_options;
    }

    #! Returns options that can be used for adding new fields to the data provider
    /** @return a hash of options that can be used for adding new fields; keys are option names, values
        describe the option; if @ref nothing is returned, then the provider does not support any add field
        options

        @since DataProvider 2.0
    */
    *hash<string, hash<DataProviderOptionInfo>> getFieldAddOptions() {
        return getStaticInfoImpl().add_field_options;
    }

    #! Returns options that can be used for updating fields of the data provider
    /** @return a hash of options that can be used for updating fields; keys are option names, values
        describe the option; if @ref nothing is returned, then the provider does not support any update field
        options

        @since DataProvider 2.0
    */
    *hash<string, hash<DataProviderOptionInfo>> getFieldUpdateOptions() {
        return getStaticInfoImpl().update_field_options;
    }

    #! Returns options that can be used for deleting fields from the data provider
    /** @return a hash of options that can be used for deleting fields; keys are option names, values
        describe the option; if @ref nothing is returned, then the provider does not support any delete field
        options

        @since DataProvider 2.0
    */
    *hash<string, hash<DataProviderOptionInfo>> getFieldDeleteOptions() {
        return getStaticInfoImpl().delete_field_options;
    }

    #! Returns @ref True if the data provider supports reading
    /**
    */
    bool supportsRead() {
        return getStaticInfoImpl().supports_read;
    }

    #! Returns @ref True if the data provider supports native bulk reading
    /**
    */
    bool supportsBulkRead() {
        return getStaticInfoImpl().supports_bulk_read;
    }

    #! Returns @ref True if the data provider supports the record creation API
    /**
    */
    bool supportsCreate() {
        return getStaticInfoImpl().supports_create;
    }

    #! Returns @ref True if the data provider supports the record update API
    /**
    */
    bool supportsUpdate() {
        return getStaticInfoImpl().supports_update;
    }

    #! Returns @ref True if the data provider supports the record upsert API
    /**
    */
    bool supportsUpsert() {
        return getStaticInfoImpl().supports_upsert;
    }

    #! Returns @ref True if the data provider supports the record deletion API
    /**
    */
    bool supportsDelete() {
        return getStaticInfoImpl().supports_delete;
    }

    #! Returns @ref True if the data provider supports the record search API natively
    /**
    */
    bool supportsNativeSearch() {
        return getStaticInfoImpl().supports_native_search;
    }

    #! Returns True if the data provider supports bulk creation output
    /**
    */
    bool supportsBulkCreate() {
        return getStaticInfoImpl().supports_bulk_create;
    }

    #! Returns True if the data provider supports bulk upserts
    /**
    */
    bool supportsBulkUpsert() {
        return getStaticInfoImpl().supports_bulk_upsert;
    }

    #! Returns True if the data provider supports requests
    /**
    */
    bool supportsRequest() {
        return getStaticInfoImpl().supports_request;
    }

    #! Returns True if the data provider requires search options to retrieve the record type
    /**
    */
    bool recordRequiresSearchOptions() {
        return getStaticInfoImpl().record_requires_search_options;
    }

    #! Returns @ref True if the data provider has a record type
    /** @return @ref True if the data provider has a record type
    */
    bool hasRecord() {
        return getStaticInfoImpl().has_record;
    }

    #! Returns True if the data provider supports creating new child data providers
    /** @since DataProvider 2.0
    */
    bool supportsCreateChild() {
        return getStaticInfoImpl().supports_child_create;
    }

    #! Returns True if the data provider supports creating deleting child data providers
    /** @since DataProvider 2.0
    */
    bool supportsDeleteChild() {
        return getStaticInfoImpl().supports_child_delete;
    }

    #! Returns True if the data provider supports creating adding new fields
    /** @since DataProvider 2.0
    */
    bool supportsAddField() {
        return getStaticInfoImpl().supports_add_field;
    }

    #! Returns True if the data provider supports updating existing fields
    /** @since DataProvider 2.0
    */
    bool supportsUpdateField() {
        return getStaticInfoImpl().supports_update_field;
    }

    #! Returns True if the data provider supports deleting existing fields
    /** @since DataProvider 2.0
    */
    bool supportsDeleteField() {
        return getStaticInfoImpl().supports_delete_field;
    }

    #! Returns the description of the record type, if any
    /** @param search_options to be included and processed by validateSearchOptions() if
        \a recordRequiresSearchOptions() is @ref True for this provider, otherwise any value provided in this argument
        is ignored
    */
    private *hash<string, AbstractDataField> getRecordTypeImpl(*hash<auto> search_options) {
        throwUnimplementedException();
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param block_size the number of records in a read block; must be a positive number
        @param where_cond the search criteria; after processing by processFieldValues()
        @param search_options the search options after processing by validateSearchOptions()

        @throw INVALID-BLOCK-SIZE the block size must be a positive number
        @throw INVALID-OPERATION the data provider does not support reading
    */
    private AbstractDataProviderBulkRecordInterface searchRecordsBulkImpl(int block_size = 1000,
            *hash<auto> where_cond, *hash<auto> search_options) {
        return new DefaultBulkRecordIterface(block_size, searchRecordsImpl(where_cond, search_options));
    }

    #! Returns an iterator for zero or more records matching the search options
    /** @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()

        @see requestSearchRecordsImpl()
    */
    private AbstractDataProviderRecordIterator searchRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        throwUnimplementedException();
    }

    #! Returns an iterator for zero or more records matching the search options according to an API request
    /** @param req the request to serialize and make according to the request type
        @param where_cond the search criteria; after processing by processFieldValues()
        @param search_options the search options after processing by validateSearchOptions()

        This will execute the request and perform a default search on any record(s) returned

        @see searchRecordsImpl()
    */
    private AbstractDataProviderRecordIterator requestSearchRecordsImpl(auto req, *hash<auto> where_cond,
            *hash<auto> search_options) {
        throwUnimplementedException();
    }

    #! Creates the given record to the data provider
    /** @param rec a hash representing a single input record
        @param create_options the create options after processing by validateCreateOptions()

        @return the data written to the data provider

        @throw DUPLICATE-RECORD this exception should be thrown if the provider fails due to an attempt to create a
        duplicate record
    */
    private *hash<auto> createRecordImpl(hash<auto> rec, *hash<auto> create_options) {
        throwUnimplementedException();
    }

    #! Upserts the given record to the data provider
    /** @param rec a hash representing a single input record
        @param upsert_options the create options after processing by validateUpsertOptions()

        @return see @ref db_provider_upsert_results for possible values
    */
    private string upsertRecordImpl(hash<auto> rec, *hash<auto> upsert_options) {
        throwUnimplementedException();
    }

    #! Updates a single record matching the search options
    /** @param set the hash of field data to set
        @param where_cond the search criteria
        @param search_options the search options after processing by validateSearchOptions()
    */
    private bool updateSingleRecordImpl(hash<auto> set, hash<auto> where_cond, *hash<auto> search_options) {
        throwUnimplementedException();
    }

    #! Updates zero or more records matching the search options
    /** @param set the hash of field data to set
        @param where_cond a hash for identifying the record(s) to be updated
        @param search_options the update options after processing by validateSearchOptions()

        @return the number of records updated
    */
    private int updateRecordsImpl(hash<auto> set, hash<auto> where_cond, *hash<auto> search_options) {
        throwUnimplementedException();
    }

    #! Deletes zero or more records
    /**
        @param where_cond a hash for identifying the record(s) to be deleted
        @param search_options the delete options after processing by validateSearchOptions()

        @return the number of records deleted
    */
    private int deleteRecordsImpl(*hash<auto> where_cond, *hash<auto> search_options) {
        throwUnimplementedException();
    }

    #! Makes a request and returns the response
    /** @param req the request to serialize and make according to the request type
        @param request_options the request options; will be processed by validateRequestOptions()

        @return the response to the request
    */
    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        throwUnimplementedException();
    }

    #! Creates a new child data provider and returns it after adding as a child
    /** @param name the name of the new child data provider
        @param fields the fields for the new child data provider
        @param child_create_options the options for creating the new child data provider

        @return the new child data provider

        @since DataProvider 2.0
    */
    private AbstractDataProvider createChildProviderImpl(string name, hash<string, AbstractDataField> fields,
            *hash<auto> child_create_options) {
        throwUnimplementedException();
    }

    #! Deletes a child data provider
    /** @param name the name of the new child data provider
        @param child_delete_options the options for deleting a child data provider

        @since DataProvider 2.0
    */
    private deleteChildProviderImpl(string name, *hash<auto> child_delete_options) {
        throwUnimplementedException();
    }

    #! Creates a new field and returns it after adding it
    /**
        @param field the field to add
        @param field_add_options the options for adding the new field

        @return the new child data provider

        @since DataProvider 2.0
    */
    private AbstractDataProvider addFieldImpl(AbstractDataField field, *hash<auto> field_add_options) {
        throwUnimplementedException();
    }

    #! Updates an existing field
    /** @param name the name of the current field to update
        @param field the new definition of the field
        @param field_update_options the options for adding the new field

        @since DataProvider 2.0
    */
    private updateFieldImpl(string name, AbstractDataField field, *hash<auto> field_update_options) {
        throwUnimplementedException();
    }

    #! Deletes an existing field
    /** @param name the name of the field to delete
        @param field_delete_options the options for deleting the new field

        @since DataProvider 2.0
    */
    private deleteFieldImpl(string name, *hash<auto> field_delete_options) {
        throwUnimplementedException();
    }

    #! Returns the description of a successful request message, if any
    /** @return the request type for this provider

        @note only called if the provider supports the request / response API
    */
    private *AbstractDataProviderType getRequestTypeImpl() {
        throwUnimplementedException();
    }

    #! Returns the description of a response message, if this object represents a response message
    /** @return the response type for this response message

        @note only called if the provider supports the request / response API
    */
    private *AbstractDataProviderType getResponseTypeImpl() {
        throwUnimplementedException();
    }

    #! Returns a hash of error responses, if any
    /** @return a hash of error responses, if any, keyed by response code or string

        @note only called if the provider supports the request / response API
    */
    private *hash<string, AbstractDataProviderType> getErrorResponseTypesImpl() {
        throwUnimplementedException();
    }

    #! Returns the type for the given error code
    /** @param error_code the error code for the response; must be a known error code, or an \c UNKNOWN-ERROR-RESPONSE
        exception is thrown
        @return the type for the given error code

        @throw UNKNOWN-ERROR-RESPONSE the error response given is not known
    */
    private AbstractDataProviderType getErrorResponseTypeImpl(string error_code) {
        # note: this should be reimplemented with a more efficient implementation if creating the error type hash on
        # demand is expensive
        *hash<string, AbstractDataProviderType> err_map = getErrorResponseTypesImpl();
        *AbstractDataProviderType type = err_map{error_code};
        if (!type) {
            throw "UNKNOWN-ERROR-RESPONSE", sprintf("error code %y is unknown; known error codes: %y", error_code,
                keys err_map);
        }
        return type;
    }

    #! Throws an \c INVALID-OPERATION exception
    /** @throw INVALID-OPERATION this exception is thrown unconditionally by this method
    */
    private throwUnimplementedException() {
        # should never be called
        throw "UNIMPLEMENTED", sprintf("data provider %y of class %y has an error; the operation is not supported; "
            "this indicates a bug in the implementation of the data provider; please report this bug to the author "
            "of the data provider",
            getName(), self.className());
    }

    #! Returns the data provider name
    abstract string getName();

    #! Returns data provider static info
    private abstract hash<DataProviderInfo> getStaticInfoImpl();
}
}
