# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore DeepSeekChatCompletionRequestDataType class definition

/** DeepSeekChatCompletionRequestDataType.qc Copyright 2026 Qore Technologies, s.r.o.

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

#! Public DeepSeek request contracts
public namespace DeepSeekDataProvider {

#! ChatFunction data contract
public class DeepSeekChatFunctionDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "name": {
                "display_name": "Function Name",
                "short_desc": "Function Name",
                "desc": "The function to invoke in your application.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "example_value": "lookup_order",
            },
            "arguments": {
                "display_name": "Arguments",
                "short_desc": "Arguments",
                "desc": "JSON-encoded function arguments.",
                "type": AbstractDataProviderTypeMap."string",
                "example_value": "{\"order_id\":\"123\"}",
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared DeepSeekChatFunctionDataType instance
public const DeepSeekChatFunctionDataType = new DeepSeekChatFunctionDataType();

#! ChatToolCall data contract
public class DeepSeekChatToolCallDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "id": {
                "display_name": "Tool Call ID",
                "short_desc": "Tool Call ID",
                "desc": "Pair this identifier with the corresponding `tool_call_id`.",
                "type": AbstractDataProviderTypeMap."string",
                "example_value": "call_123",
            },
            "type": {
                "display_name": "Tool Type",
                "short_desc": "Tool Type",
                "desc": "Tool Type.",
                "type": AbstractDataProviderTypeMap."string",
                "allowed_values": (
                    <AllowedValueInfo>{"value": "function", "display_name": "Function"},
                ),
            },
            "function": {
                "display_name": "Function Call",
                "short_desc": "Function Call",
                "desc": "Function Call.",
                "type": new DeepSeekChatFunctionDataType(),
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared DeepSeekChatToolCallDataType instance
public const DeepSeekChatToolCallDataType = new DeepSeekChatToolCallDataType();

#! ImageUrl data contract
public class DeepSeekImageUrlDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "url": {
                "display_name": "Image URL",
                "short_desc": "Image URL",
                "desc": "Public image URL or a base64 data URL. Supported formats: JPEG, PNG, GIF, WebP.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "example_value": "https://example.com/product.png",
            },
            "detail": {
                "display_name": "Image Detail",
                "short_desc": "Image Detail",
                "desc": "Choose image processing quality.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (
                    <AllowedValueInfo>{"value": "low", "display_name": "Low"},
                    <AllowedValueInfo>{"value": "high", "display_name": "High"},
                    <AllowedValueInfo>{"value": "original", "display_name": "Original"},
                    <AllowedValueInfo>{"value": "auto", "display_name": "Automatic"},
                ),
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared DeepSeekImageUrlDataType instance
public const DeepSeekImageUrlDataType = new DeepSeekImageUrlDataType();

#! ChatContent data contract
public class DeepSeekChatContentDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "type": {
                "display_name": "Content Type",
                "short_desc": "Content Type",
                "desc": "Content Type.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "allowed_values": (
                    <AllowedValueInfo>{"value": "text", "display_name": "Text"},
                    <AllowedValueInfo>{"value": "image_url", "display_name": "Image URL"},
                    <AllowedValueInfo>{"value": "file", "display_name": "File"},
                ),
            },
            "text": {
                "display_name": "Text",
                "short_desc": "Text",
                "desc": "Text for a `text` content part.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "Describe this product.",
            },
            "image_url": {
                "display_name": "Image",
                "short_desc": "Image",
                "desc": "Image.",
                "type": new DeepSeekImageUrlDataType(True),
            },
            "file_id": {
                "display_name": "File ID",
                "short_desc": "File ID",
                "desc": "Uploaded image identifier; mutually exclusive with `file_data`.",
                "type": AbstractDataProviderTypeMap."*string",
                "ref_data": "files",
                "allowed_values_creatable": True,
            },
            "file_data": {
                "display_name": "File Data",
                "short_desc": "File Data",
                "desc": "Base64 image data URL; mutually exclusive with `file_id`.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "filename": {
                "display_name": "Filename",
                "short_desc": "Filename",
                "desc": "Optional image filename, used only with `file_data`.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "product.png",
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared DeepSeekChatContentDataType instance
public const DeepSeekChatContentDataType = new DeepSeekChatContentDataType();

#! ChatMessage data contract
public class DeepSeekChatMessageDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "role": {
                "display_name": "Role",
                "short_desc": "Role",
                "desc": "The message author role.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "example_value": "user",
                "allowed_values": (
                    <AllowedValueInfo>{"value": "system", "display_name": "System"},
                    <AllowedValueInfo>{"value": "user", "display_name": "User"},
                    <AllowedValueInfo>{"value": "assistant", "display_name": "Assistant"},
                    <AllowedValueInfo>{"value": "tool", "display_name": "Tool"},
                ),
            },
            "content": {
                "display_name": "Content",
                "short_desc": "Content",
                "desc": "Text or image content parts. Assistant tool calls may omit text.",
                "type": new QoreUnionDataType((AbstractDataProviderTypeMap."*string", new ListDataType("DeepSeekChatContentDataTypeList", new DeepSeekChatContentDataType(), False),)),
                "example_value": "Summarize this order.",
            },
            "name": {
                "display_name": "Author Name",
                "short_desc": "Author Name",
                "desc": "Optional participant name.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "customer",
            },
            "tool_call_id": {
                "display_name": "Tool Call ID",
                "short_desc": "Tool Call ID",
                "desc": "Required for a `tool` message; copy the preceding assistant tool call ID.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "call_123",
            },
            "tool_calls": {
                "display_name": "Tool Calls",
                "short_desc": "Tool Calls",
                "desc": "Tool calls from a previous assistant turn.",
                "type": new ListDataType("DeepSeekChatToolCallDataTypeList", new DeepSeekChatToolCallDataType(), True),
            },
            "prefix": {
                "display_name": "Prefix Completion",
                "short_desc": "Prefix Completion",
                "desc": "Continue the final assistant message. Routes this request to `/beta/chat/completions`.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
            "reasoning_content": {
                "display_name": "Reasoning Content",
                "short_desc": "Reasoning Content",
                "desc": "Preserve reasoning from a previous assistant tool call or supply reasoning for a beta prefix.",
                "type": AbstractDataProviderTypeMap."*string",
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared DeepSeekChatMessageDataType instance
public const DeepSeekChatMessageDataType = new DeepSeekChatMessageDataType();

#! FunctionDefinition data contract
public class DeepSeekFunctionDefinitionDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "name": {
                "display_name": "Function Name",
                "short_desc": "Function Name",
                "desc": "Function identifier, at most 64 characters: letters, digits, underscores, or dashes.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "example_value": "lookup_order",
            },
            "description": {
                "display_name": "Description",
                "short_desc": "Description",
                "desc": "Explain when to call this function.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "Look up delivery status for an order.",
            },
            "parameters": {
                "display_name": "Parameters",
                "short_desc": "Parameters",
                "desc": "JSON Schema describing function arguments.",
                "type": AbstractDataProviderTypeMap."*hash",
                "example_value": {
                    "type": "object",
                    "properties": {
                        "order_id": {
                            "type": "string",
                        },
                    },
                    "required": ("order_id",),
                },
            },
            "strict": {
                "display_name": "Strict Arguments",
                "short_desc": "Strict Arguments",
                "desc": "Require arguments to match the schema. Routes this request to the beta API.",
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

#! Shared DeepSeekFunctionDefinitionDataType instance
public const DeepSeekFunctionDefinitionDataType = new DeepSeekFunctionDefinitionDataType();

#! ToolDefinition data contract
public class DeepSeekToolDefinitionDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "type": {
                "display_name": "Tool Type",
                "short_desc": "Tool Type",
                "desc": "Tool Type.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "allowed_values": (
                    <AllowedValueInfo>{"value": "function", "display_name": "Function"},
                ),
            },
            "function": {
                "display_name": "Function",
                "short_desc": "Function",
                "desc": "Function.",
                "type": new DeepSeekFunctionDefinitionDataType(),
                "required": True,
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared DeepSeekToolDefinitionDataType instance
public const DeepSeekToolDefinitionDataType = new DeepSeekToolDefinitionDataType();

#! Thinking data contract
public class DeepSeekThinkingDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "type": {
                "display_name": "Thinking Mode",
                "short_desc": "Thinking Mode",
                "desc": "Enable reasoning or generate an immediate answer.",
                "type": AbstractDataProviderTypeMap."string",
                "allowed_values": (
                    <AllowedValueInfo>{"value": "enabled", "display_name": "Enabled"},
                    <AllowedValueInfo>{"value": "disabled", "display_name": "Disabled"},
                ),
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared DeepSeekThinkingDataType instance
public const DeepSeekThinkingDataType = new DeepSeekThinkingDataType();

#! ResponseFormat data contract
public class DeepSeekResponseFormatDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "type": {
                "display_name": "Output Format",
                "short_desc": "Output Format",
                "desc": "For JSON output, also instruct the model to produce JSON in a message.",
                "type": AbstractDataProviderTypeMap."string",
                "allowed_values": (
                    <AllowedValueInfo>{"value": "text", "display_name": "Text"},
                    <AllowedValueInfo>{"value": "json_object", "display_name": "JSON Object"},
                ),
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared DeepSeekResponseFormatDataType instance
public const DeepSeekResponseFormatDataType = new DeepSeekResponseFormatDataType();

#! ChatCompletionRequest data contract
public class DeepSeekChatCompletionRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "model": {
                "display_name": "Model",
                "short_desc": "Model",
                "desc": "Select an available model for this request.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "ref_data": "models",
                "allowed_values_creatable": True,
            },
            "messages": {
                "display_name": "Messages",
                "short_desc": "Messages",
                "desc": "Conversation history in chronological order.",
                "type": new ListDataType("DeepSeekChatMessageDataTypeList", new DeepSeekChatMessageDataType(), False),
                "required": True,
                "example_value": ({
                    "role": "user",
                    "content": "Summarize the benefits of automated invoicing.",
                },),
            },
            "max_tokens": {
                "display_name": "Max Tokens",
                "short_desc": "Max Tokens",
                "desc": "Maximum generated tokens, including reasoning. Check the selected model limits.",
                "type": AbstractDataProviderTypeMap."*softint",
                "example_value": 2048,
                "preselected": True,
            },
            "temperature": {
                "display_name": "Temperature",
                "short_desc": "Temperature",
                "desc": "Sampling temperature from `0` to `2`. Has no effect in thinking mode.",
                "type": AbstractDataProviderTypeMap."*softfloat",
                "example_value": 1.0,
            },
            "thinking": {
                "display_name": "Thinking",
                "short_desc": "Thinking",
                "desc": "Enable or disable reasoning for this request.",
                "type": new DeepSeekThinkingDataType(True),
            },
            "reasoning_effort": {
                "display_name": "Reasoning Effort",
                "short_desc": "Reasoning Effort",
                "desc": "Amount of reasoning to apply when thinking is enabled.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (
                    <AllowedValueInfo>{"value": "low", "display_name": "Low"},
                    <AllowedValueInfo>{"value": "high", "display_name": "High"},
                    <AllowedValueInfo>{"value": "max", "display_name": "Maximum"},
                ),
            },
            "top_p": {
                "display_name": "Top-P",
                "short_desc": "Top-P",
                "desc": "Nucleus sampling cutoff from `0` to `1`. Has no effect in thinking mode.",
                "type": AbstractDataProviderTypeMap."*softfloat",
            },
            "stop": {
                "display_name": "Stop Sequences",
                "short_desc": "Stop Sequences",
                "desc": "Up to 16 strings that stop generation before the matching sequence.",
                "type": new QoreUnionDataType((AbstractDataProviderTypeMap."*string", new ListDataType("DeepSeekStopList", StringType),)),
                "example_value": ("END",),
            },
            "tools": {
                "display_name": "Tools",
                "short_desc": "Tools",
                "desc": "Functions available to the assistant; at most 128.",
                "type": new ListDataType("DeepSeekToolDefinitionDataTypeList", new DeepSeekToolDefinitionDataType(), True),
            },
            "tool_choice": {
                "display_name": "Tool Choice",
                "short_desc": "Tool Choice",
                "desc": "Choose a tool policy or provide a named function object to force a specific tool.",
                "type": new QoreUnionDataType((AbstractDataProviderTypeMap."*string", AbstractDataProviderTypeMap."hash",)),
                "allowed_values": (
                    <AllowedValueInfo>{"value": "none", "display_name": "None"},
                    <AllowedValueInfo>{"value": "auto", "display_name": "Automatic"},
                    <AllowedValueInfo>{"value": "required", "display_name": "Required"},
                ),
                "allowed_values_creatable": True,
                "example_value": "auto",
            },
            "response_format": {
                "display_name": "Response Format",
                "short_desc": "Response Format",
                "desc": "Choose plain text or JSON output. JSON mode requires a JSON instruction.",
                "type": new DeepSeekResponseFormatDataType(True),
                "example_value": {
                    "type": "json_object",
                },
            },
            "logprobs": {
                "display_name": "Log Probabilities",
                "short_desc": "Log Probabilities",
                "desc": "Include token log probabilities in the response.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
            "top_logprobs": {
                "display_name": "Top Log Probabilities",
                "short_desc": "Top Log Probabilities",
                "desc": "Number of token alternatives from `0` to `20`; requires `logprobs` set to `True`.",
                "type": AbstractDataProviderTypeMap."*softint",
            },
            "user_id": {
                "display_name": "User ID",
                "short_desc": "User ID",
                "desc": "Pseudonymous identifier for cache and scheduling isolation. Use letters, digits, underscores, or dashes; at most 512 characters. Exclude personal information.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "customer-123",
            },
            "stream": {
                "display_name": "Stream",
                "short_desc": "Stream",
                "desc": "Completed-response actions accept `False`. Use `DeepSeekLlmProvider` for streaming chat.",
                "type": AbstractDataProviderTypeMap."*bool",
                "allowed_values": (
                    <AllowedValueInfo>{"value": False, "display_name": "Complete Response"},
                ),
            },
        };
    }

    #! Creates the typed fields
    /** @param optional whether the object can be absent */
    constructor(bool optional = False) : HashDataType(optional ? AutoHashOrNothingType : AutoHashType) {
        addQoreFields(Fields);
    }
}

#! Shared DeepSeekChatCompletionRequestDataType instance
public const DeepSeekChatCompletionRequestDataType = new DeepSeekChatCompletionRequestDataType();
}
