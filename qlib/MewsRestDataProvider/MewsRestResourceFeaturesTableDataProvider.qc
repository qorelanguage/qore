# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore MewsRestDataProvider module definition

/** MewsRestResourceFeaturesTableDataProvider.qc Copyright 2023 - 2024 Qore Technologies, s.r.o.

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

#! contains all public definitions in the MewsRestDataProvider module
public namespace MewsRestDataProvider {
#! The MewsRestResourceFeaturesTableDataProvider data provider class
public class MewsRestResourceFeaturesTableDataProvider inherits MewsRestTableDataProvider {
    public {
        #! Provider info
        const ProviderInfo = <DataProviderInfo>{
            "type": "MewsRestResourceFeaturesTableDataProvider",
            "desc": "Mews resourcefeatures data",
            "has_record": True,
            "supports_read": True,
            "supports_bulk_read": True,
            "supports_search_expressions": True,
            "supports_native_search": True,
            "constructor_options": ConstructorOptions,
            "search_options": SearchOptions,
            "expressions": Expressions,
        };

        #! Fields supporting >= and <= operators
        const ComparisonFields = {
            "UpdatedUtc": True,
        };

        #! Fields supporting list equality comparisons
        const EqualityListFields = {
            "EnterpriseId": True,
            "ResourceFeatureId": True,
            "ServiceId": True,
            "ActivityState": True,
        };

        #! Table API config
        const TableApiConfig = <MewsTableApiConfig>{
            "table": "resourcefeatures",
            "path": "api/connector/v1/resourceFeatures/getAll/POST",
            "body_key": "ResourceFeatures",
            "comparison_fields": ComparisonFields,
            "equality_list_fields": EqualityListFields,
        };

        #! Constructor options
        const ConstructorOptions = MewsRestDataProvider::ConstructorOptions;
    }

    #! Creates the object from constructor options
    constructor(*hash<auto> options) : MewsRestTableDataProvider(options, TableApiConfig) {
    }

    #! Creates the object from the arguments
    constructor(MewsRestClient rest) : MewsRestTableDataProvider(rest, TableApiConfig){
    }

    #! Returns data provider static info
    private hash<DataProvider::DataProviderInfo> getStaticInfoImpl() {
        return ProviderInfo;
    }
}
}
