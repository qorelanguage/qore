# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore UnstructuredDataProvider class definitions

/** UnstructuredDataProvider.qc Copyright 2026 Qore Technologies, s.r.o.

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
public class UnstructuredDataProviderCommon inherits DataProvider::AbstractDataProvider {
    public {
        const ConstructorOptions = {
            "apikey": <DataProviderOptionInfo>{
                "display_name": "API Key",
                "short_desc": "Unstructured.io API key",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "Unstructured.io API key (sent as `unstructured-api-key` header).",
                "required": True,
                "sensitive": True,
            },
            "url": <DataProviderOptionInfo>{
                "display_name": "Server URL",
                "short_desc": "Unstructured.io REST API URL",
                "type": AbstractDataProviderTypeMap."string",
                "desc": "URL of the Unstructured.io REST API.",
                "default_value": UnstructuredRestClient::UnstructuredRestClientBase::DefaultUrl,
            },
            "restclient": <DataProviderOptionInfo>{
                "display_name": "REST Client",
                "short_desc": "The REST client object to use",
                "type": AbstractDataProviderType::get(
                    new Type("UnstructuredRestClient::UnstructuredRestClientIo"),
                    NOTHING, {DTT_ClientOnly: True}),
                "desc": "The Unstructured REST client to use",
            },
        };
    }

    private {
        *UnstructuredRestClient::UnstructuredRestClientIo rest;
    }

    constructor(UnstructuredRestClient::UnstructuredRestClientIo rest) {
        self.rest = rest;
    }

    constructor(*hash<auto> options) {
        *hash<auto> copts = checkOptions("CONSTRUCTOR-ERROR", ConstructorOptions, options);
        if (copts.restclient) {
            rest = copts.restclient;
        } else {
            if (!copts.apikey.val()) {
                throw "CONSTRUCTOR-ERROR",
                    "missing required 'apikey' argument for Unstructured API "
                    "communication";
            }
            rest = new UnstructuredRestClient::UnstructuredRestClientIo({
                "url": copts.url,
                "apikey": copts.apikey,
            });
        }
    }
}

public class UnstructuredDataProvider inherits UnstructuredDataProviderCommon {
    public {
        const ProviderInfo = <DataProviderInfo>{
            "name": "unstructured",
            "type": "UnstructuredDataProvider",
            "supports_children": True,
            "constructor_options": ConstructorOptions,
            "children_can_support_apis": True,
        };
    }

    private {
        static hash<string, Reflection::Class> childMap = {
            "partition-text":
                Class::forName("UnstructuredDataProvider::UnstructuredPartitionTextDataProvider"),
        };
    }

    constructor(UnstructuredRestClient::UnstructuredRestClientIo rest)
            : UnstructuredDataProviderCommon(rest) {}
    constructor(*hash<auto> options) : UnstructuredDataProviderCommon(options) {}

    string getName() { return ProviderInfo.name; }

    *list<hash<DataProviderSummaryInfo>> getChildProviderSummaryInfo() {
        return map $1.getConstant("ProviderSummaryInfo").getValue(), childMap.iterator();
    }
    private *list<string> getChildProviderNamesImpl() { return keys childMap; }
    private *AbstractDataProvider getChildProviderImpl(string name) {
        *Class cls = childMap{name};
        return cls ? cls.newObject(rest) : NOTHING;
    }
    private hash<DataProviderInfo> getStaticInfoImpl() { return ProviderInfo; }
}

public class UnstructuredDataProviderFactory inherits AbstractDataProviderFactory {
    private {
        static Class cls = new Class("UnstructuredDataProvider");
        const FactoryInfo = <DataProviderFactoryInfo>{
            "name": "unstructured",
            "desc": "Unstructured.io data provider factory",
            "children_can_support_apis": True,
        };
    }
    private hash<DataProviderFactoryInfo> getInfoImpl() { return FactoryInfo; }
    private hash<DataProviderInfo> getProviderInfoImpl() { return UnstructuredDataProvider::ProviderInfo; }
    private Class getClassImpl() { return cls; }
}

public class UnstructuredPartitionTextRequestDataType inherits HashDataType {
    public {
        const Fields = {
            "text": {
                "display_name": "Text",
                "short_desc": "Document text content to partition",
                "desc": "Raw text content of the document to partition.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "example_value": "Section One\n\nHello world, this is a test document.",
            },
            "strategy": {
                "display_name": "Strategy",
                "short_desc": "Partitioning strategy",
                "desc": "Partitioning strategy — `auto`, `fast`, `hi_res`, or `ocr_only`.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": (
                    <AllowedValueInfo>{"value": "auto", "display_name": "Auto"},
                    <AllowedValueInfo>{"value": "fast", "display_name": "Fast"},
                    <AllowedValueInfo>{"value": "hi_res", "display_name": "Hi-Res"},
                    <AllowedValueInfo>{"value": "ocr_only", "display_name": "OCR Only"},
                ),
            },
            "chunking_strategy": {
                "display_name": "Chunking Strategy",
                "short_desc": "Server-side chunking strategy",
                "desc": "Server-side chunking strategy — `by_title` or `basic`.  When "
                    "set, Unstructured groups elements into chunks server-side.",
                "type": AbstractDataProviderTypeMap."*softstring",
                "allowed_values": (
                    <AllowedValueInfo>{"value": "by_title", "display_name": "By Title"},
                    <AllowedValueInfo>{"value": "basic", "display_name": "Basic"},
                ),
            },
            "languages": {
                "display_name": "Languages",
                "short_desc": "Document language hints (ISO codes)",
                "desc": "List of ISO 639 language codes present in the document, used to guide "
                    "partitioning and OCR.",
                "type": AbstractDataProviderTypeMap."*softlist",
                "example_value": ("eng",),
            },
            "coordinates": {
                "display_name": "Coordinates",
                "short_desc": "Include layout coordinates in each element",
                "desc": "When `True`, each element includes layout coordinates in "
                    "its metadata.",
                "type": AbstractDataProviderTypeMap."*softbool",
            },
        };
    }

    constructor() : HashDataType("UnstructuredPartitionTextRequest") {
        addQoreFields(Fields);
    }
}

public class UnstructuredElementMetadataDataType inherits HashDataType {
    private {
        const Fields = {
            "filename": {
                "display_name": "File Name",
                "short_desc": "Name of the source document",
                "desc": "Name of the source document the element came from.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "filetype": {
                "display_name": "File Type",
                "short_desc": "MIME type of the source document",
                "desc": "MIME type Unstructured detected for the source document, "
                    "for example `application/pdf`.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "languages": {
                "display_name": "Languages",
                "short_desc": "Languages detected in the element",
                "desc": "ISO 639 language codes detected in the element's text.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "page_number": {
                "display_name": "Page Number",
                "short_desc": "1-based page the element appears on",
                "desc": "1-based page the element appears on; present for paginated formats "
                    "such as PDF, DOCX and PPTX.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "parent_id": {
                "display_name": "Parent Element ID",
                "short_desc": "ID of the element this one is nested under",
                "desc": "`element_id` of the element this one is nested under, normally the "
                    "`Title` that heads the section.  Use it to rebuild the document "
                    "hierarchy from the flat element list.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "category_depth": {
                "display_name": "Category Depth",
                "short_desc": "Nesting depth of the element's category",
                "desc": "Nesting depth of the element within its category; `0` is the "
                    "outermost heading or list level.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "text_as_html": {
                "display_name": "Text as HTML",
                "short_desc": "HTML rendering of a table element",
                "desc": "HTML rendering of a `Table` element, preserving its row and column "
                    "structure.  Returned when the table structure was inferred; see "
                    "`pdf_infer_table_structure`.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "image_base64": {
                "display_name": "Image Data",
                "short_desc": "Base64-encoded image extracted from the element",
                "desc": "Base64-encoded image data for the element, returned only for the "
                    "element types named in `extract_image_block_types`.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "image_mime_type": {
                "display_name": "Image MIME Type",
                "short_desc": "MIME type of the extracted image",
                "desc": "MIME type of the data in `image_base64`.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "coordinates": {
                "display_name": "Coordinates",
                "short_desc": "Layout coordinates of the element on the page",
                "desc": "Layout coordinates of the element, returned when the `coordinates` "
                    "option is enabled.  Carries the bounding-box `points` and the "
                    "`system`, `layout_width` and `layout_height` they are expressed in.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "links": {
                "display_name": "Links",
                "short_desc": "Hyperlinks found in the element",
                "desc": "Hyperlinks found in the element's text.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "emphasized_text_contents": {
                "display_name": "Emphasized Text",
                "short_desc": "Emphasized runs of text in the element",
                "desc": "Runs of text that carry emphasis (bold, italic) in the source "
                    "document.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "emphasized_text_tags": {
                "display_name": "Emphasis Tags",
                "short_desc": "Emphasis markers for the emphasized text runs",
                "desc": "Emphasis markers corresponding to `emphasized_text_contents`, for "
                    "example `b` and `i`.",
                "type": AbstractDataProviderTypeMap."*softlist",
            },
            "orig_elements": {
                "display_name": "Original Elements",
                "short_desc": "Compressed elements a chunk was built from",
                "desc": "The elements consolidated to form a `CompositeElement` chunk, as a "
                    "compressed base64 string.  Returned only when a chunking strategy is "
                    "used with `include_orig_elements` enabled.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "url": {
                "display_name": "Source URL",
                "short_desc": "URL the source document was retrieved from",
                "desc": "URL the source document was retrieved from, when applicable.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "last_modified": {
                "display_name": "Last Modified",
                "short_desc": "Last modification timestamp of the source document",
                "desc": "Last modification timestamp reported for the source document.",
                "type": AbstractDataProviderTypeMap."*date",
            },
        };
    }

    constructor() : HashDataType("UnstructuredElementMetadata") {
        addQoreFields(Fields);
        # Unstructured's Element schema declares additionalProperties, and the metadata keys
        # returned depend on the source format and the options used, so unknown keys are kept
        setDefaultOtherFieldType(AbstractDataProviderTypeMap."any");
    }
}

public class UnstructuredElementDataType inherits HashDataType {
    private {
        const Fields = {
            "type": {
                "display_name": "Type",
                "short_desc": "The element's structural type",
                "desc": "Element type assigned by Unstructured, such as `Title`, "
                    "`NarrativeText`, `ListItem`, `Table`, `Image` or `CompositeElement` "
                    "(returned when a chunking strategy is used).",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "text": {
                "display_name": "Text",
                "short_desc": "The element's text content",
                "desc": "Element text content.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "element_id": {
                "display_name": "Element ID",
                "short_desc": "Unstructured-assigned element ID",
                "desc": "Unstructured-assigned element ID; stable for the same content unless "
                    "`unique_element_ids` is set.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "metadata": {
                "display_name": "Metadata",
                "short_desc": "Per-element metadata",
                "desc": "Per-element metadata; see "
                    "@ref UnstructuredDataProvider::UnstructuredElementMetadataDataType "
                    "\"UnstructuredElementMetadataDataType\".",
                "type": new UnstructuredElementMetadataDataType(),
            },
        };
    }

    constructor() : HashDataType("UnstructuredElement") {
        addQoreFields(Fields);
    }
}

public class UnstructuredPartitionTextResponseDataType inherits HashDataType {
    private {
        const Fields = {
            "elt_list": {
                "display_name": "Elements",
                "short_desc": "The typed elements the document was partitioned into",
                "desc": "List of typed elements returned by Unstructured.io, in document order.\n\n"
                    "When a chunking strategy is used, the elements are `CompositeElement` "
                    "chunks rather than the raw element tree.",
                "type": new ListDataType("UnstructuredElementList",
                    new UnstructuredElementDataType(), True),
            },
        };
    }

    constructor() : HashDataType("UnstructuredPartitionTextResponse") {
        addQoreFields(Fields);
    }
}

public class UnstructuredPartitionTextDataProvider inherits UnstructuredDataProviderCommon {
    public {
        const ProviderInfo = <DataProviderInfo>{
            "name": "partition-text",
            "type": "UnstructuredPartitionTextDataProvider",
            "constructor_options": ConstructorOptions,
            "supports_request": True,
        };
        const ProviderSummaryInfo = cast<hash<DataProviderSummaryInfo>>(ProviderInfo{
            AbstractDataProvider::DataProviderSummaryInfoKeys
        });
        static UnstructuredPartitionTextRequestDataType RequestType =
            new UnstructuredPartitionTextRequestDataType();
        static UnstructuredPartitionTextResponseDataType ResponseType =
            new UnstructuredPartitionTextResponseDataType();
    }

    constructor(UnstructuredRestClient::UnstructuredRestClientIo rest)
            : UnstructuredDataProviderCommon(rest) {}
    constructor(*hash<auto> options) : UnstructuredDataProviderCommon(options) {}

    string getName() { return ProviderInfo.name; }
    private *DataProvider::AbstractDataProviderType getRequestTypeImpl() { return RequestType; }
    private *DataProvider::AbstractDataProviderType getResponseTypeImpl() { return ResponseType; }
    private hash<DataProviderInfo> getStaticInfoImpl() { return ProviderInfo; }

    private auto doRequestImpl(auto req, *hash<auto> request_options) {
        string text = req.text;
        hash<auto> opts = req - "text";
        list<hash<auto>> elt_list = rest.partitionText(text, opts);
        return {"elt_list": elt_list};
    }
}
}
