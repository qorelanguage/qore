# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore UnstructuredDataProvider module: Platform API record tables

/** UnstructuredPlatformRecordTables.qc Copyright 2026 Qore Technologies, s.r.o.

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

public namespace UnstructuredDataProvider {
#! The Unstructured Platform API collections exposed as record tables
/** A table is a declaration over actions the manifest already exports; none of them makes a request of
    its own.

    @note No table declares \c supports_native_search.  The Platform API has no filter-expression
    language: its listings narrow by query argument, and everything else is evaluated client-side.
    Declaring native search would mean \c where_cond never reaching the server while appearing to.

    @since %UnstructuredDataProvider 1.0
*/
public class UnstructuredPlatformRecordTables {
    public {
        #! Workflow listing paging; the only listing in this API that pages at all
        const WorkflowPaging = <RestSchemaRecordPagingInfo>{
            "mode": RSRP_PAGE,
            "page_option": "page",
            "size_option": "page_size",
            "first_page": 1,
        };

        #! Paging for the listings that return everything in one response
        /** The job, source and destination listings take no paging arguments at all and return the
            whole collection, so saying so is the accurate declaration rather than an omission.
        */
        const NoPaging = <RestSchemaRecordPagingInfo>{
            "mode": RSRP_NONE,
        };

        #! Workflows as records
        const WorkflowsTable = <RestSchemaRecordTableInfo>{
            "name": "workflows",
            "display_name": "Workflows",
            "short_desc": "Unstructured workflows as records",
            "desc": "Workflows as records.  A workflow ties a source connector to a destination "
                "connector through a chain of processing nodes, and is what runs when documents are "
                "ingested.",
            "list_action": "list-workflows",
            "get_action": "get-workflow",
            "create_action": "create-workflow",
            "update_action": "update-workflow",
            "delete_action": "delete-workflow",
            "id_field": "id",
            "id_option": "workflow_id",
            "paging": WorkflowPaging,
            "filters": (
                <RestSchemaRecordFilterInfo>{
                    "field": "name", "operator": DP_SEARCH_OP_EQ, "option": "name", "exact": True,
                },
                <RestSchemaRecordFilterInfo>{
                    "field": "status", "operator": DP_SEARCH_OP_EQ, "option": "status", "exact": True,
                },
                <RestSchemaRecordFilterInfo>{
                    "field": "created_at", "operator": DP_SEARCH_OP_GE, "option": "created_since",
                    "exact": True,
                },
                <RestSchemaRecordFilterInfo>{
                    "field": "created_at", "operator": DP_SEARCH_OP_LT, "option": "created_before",
                    "exact": True,
                },
            ),
            "search_options": ("source_id", "destination_id", "sort_by", "sort_direction"),
        };

        #! Jobs as records
        /** Jobs are created by running a workflow rather than by writing a record, so the table is
            read-only; \c run-workflow is the action that starts one.
        */
        const JobsTable = <RestSchemaRecordTableInfo>{
            "name": "jobs",
            "display_name": "Jobs",
            "short_desc": "Unstructured workflow runs as records",
            "desc": "Workflow runs as records, read-only.  A job is started by running a workflow, "
                "not by creating a record; use the **Run Workflow** action for that.",
            "list_action": "list-jobs",
            "get_action": "get-job",
            "id_field": "id",
            "id_option": "job_id",
            "paging": NoPaging,
            "filters": (
                <RestSchemaRecordFilterInfo>{
                    "field": "workflow_id", "operator": DP_SEARCH_OP_EQ, "option": "workflow_id",
                    "exact": True,
                },
                <RestSchemaRecordFilterInfo>{
                    "field": "status", "operator": DP_SEARCH_OP_EQ, "option": "status", "exact": True,
                },
            ),
        };

        #! Source connectors as records
        const SourcesTable = <RestSchemaRecordTableInfo>{
            "name": "sources",
            "display_name": "Source Connectors",
            "short_desc": "Unstructured source connectors as records",
            "desc": "Source connectors as records.  A source connector is where a workflow reads "
                "documents from.",
            "list_action": "list-sources",
            "get_action": "get-source",
            "create_action": "create-source",
            "update_action": "update-source",
            "delete_action": "delete-source",
            "id_field": "id",
            "id_option": "source_id",
            "paging": NoPaging,
            "filters": (
                <RestSchemaRecordFilterInfo>{
                    "field": "type", "operator": DP_SEARCH_OP_EQ, "option": "source_type",
                    "exact": True,
                },
            ),
        };

        #! Destination connectors as records
        const DestinationsTable = <RestSchemaRecordTableInfo>{
            "name": "destinations",
            "display_name": "Destination Connectors",
            "short_desc": "Unstructured destination connectors as records",
            "desc": "Destination connectors as records.  A destination connector is where a workflow "
                "writes its processed output, typically a vector store.",
            "list_action": "list-destinations",
            "get_action": "get-destination",
            "create_action": "create-destination",
            "update_action": "update-destination",
            "delete_action": "delete-destination",
            "id_field": "id",
            "id_option": "destination_id",
            "paging": NoPaging,
            "filters": (
                <RestSchemaRecordFilterInfo>{
                    "field": "type", "operator": DP_SEARCH_OP_EQ, "option": "destination_type",
                    "exact": True,
                },
            ),
        };

        #! Every table this application publishes
        const Tables = (WorkflowsTable, JobsTable, SourcesTable, DestinationsTable);
    }
}
}
