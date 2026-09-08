# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GroqInferenceDataTypes class definition

/** GroqInferenceDataTypes.qc Copyright 2026 Qore Technologies, s.r.o.

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

public namespace GroqDataProvider {
#! FunctionDefinition configuration for Groq inference
public class GroqFunctionDefinitionDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "name": {
                "display_name": "Function Name",
                "short_desc": "Function Name",
                "desc": "The function to invoke in your application.",
                "type": AbstractDataProviderTypeMap."string",
                "example_value": "lookup_order",
                "required": True,
                "preselected": True,
            },
            "description": {
                "display_name": "Description",
                "short_desc": "Description",
                "desc": "Explain when the model should use this function.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "Look up the delivery status of an order.",
            },
            "parameters": {
                "display_name": "Parameters",
                "short_desc": "Parameters",
                "desc": "JSON Schema describing the function arguments.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "strict": {
                "display_name": "Strict",
                "short_desc": "Strict",
                "desc": "Require arguments to follow the parameter schema exactly.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqFunctionDefinitionDataType = new GroqFunctionDefinitionDataType();

#! ChatTool configuration for Groq inference
public class GroqChatToolDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "type": {
                "display_name": "Tool Type",
                "short_desc": "Tool Type",
                "desc": "Choose a custom function or a built-in tool supported by the model.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values": (<AllowedValueInfo>{"value": "function", "display_name": "Function"}, <AllowedValueInfo>{"value": "browser_search", "display_name": "Browser Search"}, <AllowedValueInfo>{"value": "code_interpreter", "display_name": "Code Interpreter"},),
            },
            "function": {
                "display_name": "Function",
                "short_desc": "Function",
                "desc": "Function definition; required for `function` tools.",
                "type": new GroqFunctionDefinitionDataType(True),
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqChatToolDataType = new GroqChatToolDataType();

#! CodeContainer configuration for Groq inference
public class GroqCodeContainerDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "type": {
                "display_name": "Container Type",
                "short_desc": "Container Type",
                "desc": "Groq creates the code execution container automatically.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values": (<AllowedValueInfo>{"value": "auto", "display_name": "Auto"},),
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqCodeContainerDataType = new GroqCodeContainerDataType();

#! ResponseTool configuration for Groq inference
public class GroqResponseToolDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "type": {
                "display_name": "Tool Type",
                "short_desc": "Tool Type",
                "desc": "Choose a function, built-in tool, or remote MCP server.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values": (<AllowedValueInfo>{"value": "function", "display_name": "Function"}, <AllowedValueInfo>{"value": "browser_search", "display_name": "Browser Search"}, <AllowedValueInfo>{"value": "code_interpreter", "display_name": "Code Execution"}, <AllowedValueInfo>{"value": "mcp", "display_name": "Remote MCP Server"},),
            },
            "name": {
                "display_name": "Function Name",
                "short_desc": "Function Name",
                "desc": "Required for `function` tools.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "lookup_order",
            },
            "description": {
                "display_name": "Description",
                "short_desc": "Description",
                "desc": "Explain when to use the function.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "Look up the delivery status of an order.",
            },
            "parameters": {
                "display_name": "Parameters",
                "short_desc": "Parameters",
                "desc": "JSON Schema describing function arguments.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "strict": {
                "display_name": "Strict",
                "short_desc": "Strict",
                "desc": "Require function arguments to follow the schema exactly.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
            "container": {
                "display_name": "Container",
                "short_desc": "Container",
                "desc": "Automatic container for the `code_interpreter` tool.",
                "type": new GroqCodeContainerDataType(True),
            },
            "server_label": {
                "display_name": "Server Label",
                "short_desc": "Server Label",
                "desc": "Required for `mcp` tools; identifies the server in results.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "OrderTools",
            },
            "server_url": {
                "display_name": "Server URL",
                "short_desc": "Server URL",
                "desc": "Required HTTPS MCP endpoint. Groq discovers and executes tools on this server.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "https://example.com/mcp",
            },
            "server_description": {
                "display_name": "Server Description",
                "short_desc": "Server Description",
                "desc": "Help the model decide when to call this MCP server.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "Retrieve customer order status.",
            },
            "headers": {
                "display_name": "Server Headers",
                "short_desc": "Server Headers",
                "desc": "Authentication and other HTTP headers sent by Groq to the MCP server.",
                "type": AbstractDataProviderTypeMap."*hash",
                "sensitive": True,
            },
            "allowed_tools": {
                "display_name": "Allowed Tools",
                "short_desc": "Allowed Tools",
                "desc": "Restrict MCP execution to named tools from your server. Omit to expose all discovered tools.",
                "type": new ListDataType("GroqAllowedTools", AbstractDataProviderTypeMap."string", True),
                "example_value": ("lookup_order",),
            },
            "require_approval": {
                "display_name": "Require Approval",
                "short_desc": "Require Approval",
                "desc": "Choose whether MCP calls need approval. With `always`, inspect the response and explicitly supply approval results in a later request.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (<AllowedValueInfo>{"value": "never", "display_name": "Never"}, <AllowedValueInfo>{"value": "always", "display_name": "Always"},),
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqResponseToolDataType = new GroqResponseToolDataType();

#! SearchSettings configuration for Groq inference
public class GroqSearchSettingsDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "country": {
                "display_name": "Country",
                "short_desc": "Country",
                "desc": "Country code for localized web results.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "us",
            },
            "include_domains": {
                "display_name": "Include Domains",
                "short_desc": "Include Domains",
                "desc": "Restrict web results to these domains.",
                "type": new ListDataType("GroqIncludeDomains", AbstractDataProviderTypeMap."string", True),
                "example_value": ("qore.org",),
            },
            "exclude_domains": {
                "display_name": "Exclude Domains",
                "short_desc": "Exclude Domains",
                "desc": "Exclude these domains from web results.",
                "type": new ListDataType("GroqExcludeDomains", AbstractDataProviderTypeMap."string", True),
                "example_value": ("example.com",),
            },
            "include_images": {
                "display_name": "Include Images",
                "short_desc": "Include Images",
                "desc": "Include image URLs in search results.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqSearchSettingsDataType = new GroqSearchSettingsDataType();

#! CompoundModels configuration for Groq inference
public class GroqCompoundModelsDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "answering_model": {
                "display_name": "Answering Model",
                "short_desc": "Answering Model",
                "desc": "Select a model for this role in Compound inference.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "chat-models",
                },
            },
            "reasoning_model": {
                "display_name": "Reasoning Model",
                "short_desc": "Reasoning Model",
                "desc": "Select a model for this role in Compound inference.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "chat-models",
                },
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqCompoundModelsDataType = new GroqCompoundModelsDataType();

#! WolframSettings configuration for Groq inference
public class GroqWolframSettingsDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "authorization": {
                "display_name": "Wolfram Authorization",
                "short_desc": "Wolfram Authorization",
                "desc": "Authorization credential for Wolfram Alpha computations.",
                "type": AbstractDataProviderTypeMap."*string",
                "sensitive": True,
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqWolframSettingsDataType = new GroqWolframSettingsDataType();

#! CompoundTools configuration for Groq inference
public class GroqCompoundToolsDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "enabled_tools": {
                "display_name": "Enabled Tools",
                "short_desc": "Enabled Tools",
                "desc": "Limit the tools Compound can execute. `visit_website` and `wolfram_alpha` require the `latest` Compound version; set `Groq-Model-Version` in connection headers.",
                "type": new ListDataType("GroqEnabledTools", AbstractDataProviderTypeMap."string", True),
                "element_allowed_values": (<AllowedValueInfo>{"value": "web_search", "display_name": "Web Search"}, <AllowedValueInfo>{"value": "visit_website", "display_name": "Visit Website"}, <AllowedValueInfo>{"value": "code_interpreter", "display_name": "Code Execution"}, <AllowedValueInfo>{"value": "wolfram_alpha", "display_name": "Wolfram Alpha"},),
            },
            "wolfram_settings": {
                "display_name": "Wolfram Settings",
                "short_desc": "Wolfram Settings",
                "desc": "Credentials for the Wolfram Alpha tool.",
                "type": new GroqWolframSettingsDataType(True),
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqCompoundToolsDataType = new GroqCompoundToolsDataType();

#! Compound configuration for Groq inference
public class GroqCompoundDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "models": {
                "display_name": "Models",
                "short_desc": "Models",
                "desc": "Select models for answering and reasoning.",
                "type": new GroqCompoundModelsDataType(True),
            },
            "tools": {
                "display_name": "Tools",
                "short_desc": "Tools",
                "desc": "Choose tools available to Compound inference.",
                "type": new GroqCompoundToolsDataType(True),
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqCompoundDataType = new GroqCompoundDataType();

#! TextDocumentSource configuration for Groq inference
public class GroqTextDocumentSourceDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "type": {
                "display_name": "Source Type",
                "short_desc": "Source Type",
                "desc": "Inline text document.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values": (<AllowedValueInfo>{"value": "text", "display_name": "Text"},),
            },
            "text": {
                "display_name": "Text",
                "short_desc": "Text",
                "desc": "Document contents used to ground the answer.",
                "type": AbstractDataProviderTypeMap."string",
                "example_value": "Order 123 shipped on Monday.",
                "required": True,
                "preselected": True,
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqTextDocumentSourceDataType = new GroqTextDocumentSourceDataType();

#! JsonDocumentSource configuration for Groq inference
public class GroqJsonDocumentSourceDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "type": {
                "display_name": "Source Type",
                "short_desc": "Source Type",
                "desc": "Inline JSON document.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values": (<AllowedValueInfo>{"value": "json", "display_name": "Json"},),
            },
            "data": {
                "display_name": "Data",
                "short_desc": "Data",
                "desc": "Structured document contents.",
                "type": AbstractDataProviderTypeMap."hash",
                "example_value": {
                    "order_id": "123",
                    "status": "shipped",
                },
                "required": True,
                "preselected": True,
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqJsonDocumentSourceDataType = new GroqJsonDocumentSourceDataType();

#! Document configuration for Groq inference
public class GroqDocumentDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "id": {
                "display_name": "Document ID",
                "short_desc": "Document ID",
                "desc": "Your identifier for citations from this input document.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "order-123",
            },
            "source": {
                "display_name": "Source",
                "short_desc": "Source",
                "desc": "Inline text or JSON used to ground the response.",
                "type": new QoreUnionDataType((new GroqTextDocumentSourceDataType(), new GroqJsonDocumentSourceDataType(),)),
                "required": True,
                "preselected": True,
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqDocumentDataType = new GroqDocumentDataType();

#! JsonSchema configuration for Groq inference
public class GroqJsonSchemaDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "name": {
                "display_name": "Schema Name",
                "short_desc": "Schema Name",
                "desc": "A name for the structured output contract.",
                "type": AbstractDataProviderTypeMap."string",
                "example_value": "order_status",
                "required": True,
                "preselected": True,
            },
            "description": {
                "display_name": "Description",
                "short_desc": "Description",
                "desc": "Explain the intended structured response.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "Delivery status for a customer order.",
            },
            "schema": {
                "display_name": "Schema",
                "short_desc": "Schema",
                "desc": "JSON Schema defining the structured response.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "strict": {
                "display_name": "Strict",
                "short_desc": "Strict",
                "desc": "Require the result to follow the schema exactly.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqJsonSchemaDataType = new GroqJsonSchemaDataType();

#! ChatResponseFormat configuration for Groq inference
public class GroqChatResponseFormatDataType inherits HashDataType {
    public {
        #! Input field definitions
        const Fields = {
            "type": {
                "display_name": "Format",
                "short_desc": "Format",
                "desc": "Choose text, JSON mode, or structured output.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values": (<AllowedValueInfo>{"value": "text", "display_name": "Text"}, <AllowedValueInfo>{"value": "json_object", "display_name": "JSON Object"}, <AllowedValueInfo>{"value": "json_schema", "display_name": "JSON Schema"},),
            },
            "json_schema": {
                "display_name": "JSON Schema",
                "short_desc": "JSON Schema",
                "desc": "Schema configuration; required when `type` is `json_schema`.",
                "type": new GroqJsonSchemaDataType(True),
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqChatResponseFormatDataType = new GroqChatResponseFormatDataType();
}
