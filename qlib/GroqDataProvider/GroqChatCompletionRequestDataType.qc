# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GroqChatCompletionRequestDataType class definition

/** GroqChatCompletionRequestDataType.qc Copyright 2026 Qore Technologies, s.r.o.

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

#! contains all public definitions in the GroqDataProvider module
public namespace GroqDataProvider {
#! Data type for a single tool call produced by a Groq assistant message
public class GroqChatToolCallDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "id": {
                "display_name": "Tool Call ID",
                "short_desc": "Unique identifier for the tool call",
                "desc": "Unique identifier for this tool call.  The same `id` must be echoed back "
                    "in the corresponding `tool` role message that returns the tool's result.",
                "type": AbstractDataProviderTypeMap."string",
            },
            "type": {
                "display_name": "Tool Type",
                "short_desc": "Type of tool invoked",
                "desc": "Type of tool invoked.  `function` is the only supported type for "
                    "OpenAI-compatible tool calling.",
                "type": AbstractDataProviderTypeMap."string",
                "example_value": "function",
                "allowed_values": (<AllowedValueInfo>{"value": "function", "display_name": "Function"},),
            },
            "function": {
                "display_name": "Function Call",
                "short_desc": "Function name and JSON-encoded arguments",
                "desc": "Hash with `name` (string) and `arguments` (JSON-encoded string) "
                    "describing the function the model wants to invoke.",
                "type": AbstractDataProviderTypeMap."hash",
            },
        };
    }

    #! Creates the object
    constructor() {
        addQoreFields(Fields);
    }
}

#! Data type for a single chat message
public class GroqChatMessageDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "role": {
                "display_name": "Role",
                "short_desc": "The role of the message author",
                "desc": "The role of the author of this message.\n\n"
                    "- `system` — high-level instructions and persona for the assistant\n"
                    "- `user` — input from the end user\n"
                    "- `assistant` — previous model output (used for multi-turn conversations)\n"
                    "- `tool` — result returned by a tool invocation (paired with a `tool_call_id`)",
                "type": AbstractDataProviderTypeMap."string",
                "example_value": "user",
                "allowed_values": (
                    <AllowedValueInfo>{"value": "system",
                        "display_name": "System - instructions and persona"},
                    <AllowedValueInfo>{"value": "user",
                        "display_name": "User - end-user input"},
                    <AllowedValueInfo>{"value": "assistant",
                        "display_name": "Assistant - previous model output"},
                    <AllowedValueInfo>{"value": "tool",
                        "display_name": "Tool - tool invocation result"},
                ),
            },
            "content": {
                "display_name": "Content",
                "short_desc": "The text content of the message",
                "desc": "The text content of the message.  May be `NOTHING` on assistant messages "
                    "that only contain `tool_calls`.",
                "type": new QoreUnionDataType((AbstractDataProviderTypeMap."*string",
                    new ListDataType("GroqMessageContent", AbstractDataProviderTypeMap."hash", False),)),
                "example_value": "Summarize this invoice.",
            },
            "name": {
                "display_name": "Author Name",
                "short_desc": "Optional name of the author",
                "desc": "Optional name identifying the specific author of the message.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "tool_call_id": {
                "display_name": "Tool Call ID",
                "short_desc": "ID of the tool call this message responds to",
                "desc": "Required when `role` is `tool` — identifies which of the preceding "
                    "assistant message's `tool_calls` this message is returning the result for.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "tool_calls": {
                "display_name": "Tool Calls",
                "short_desc": "Tool invocations emitted by the assistant",
                "desc": "List of tool calls emitted by the assistant.  Present only on assistant "
                    "messages that chose to invoke a tool.",
                "type": new ListDataType("GroqChatToolCallList",
                    new GroqChatToolCallDataType(), True),
            },
        };
    }

    #! Creates the object
    constructor() {
        addQoreFields(Fields);
    }
}

#! Data type for Groq chat completion requests (OpenAI-compatible)
public class GroqChatCompletionRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "model": {
                "display_name": "Model",
                "short_desc": "The Groq-hosted model to use",
                "desc": "ID of the Groq-hosted model to use.  See the "
                    "[Groq models documentation](https://console.groq.com/docs/models) for the "
                    "currently-available models; the `list-models` action returns the live list "
                    "served by the API.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "chat-models",
                },
            },
            "messages": {
                "display_name": "Messages",
                "short_desc": "List of chat messages forming the conversation",
                "desc": "The conversation history as a list of message objects.  Each message has "
                    "a `role` (`system`, `user`, `assistant`, or `tool`) and `content` (text).",
                "type": new ListDataType("GroqChatMessageList",
                    new GroqChatMessageDataType(), False),
                "required": True,
                "example_value": (
                    {"role": "system", "content": "You are a helpful assistant."},
                    {"role": "user", "content": "Hello!"},
                ),
            },
            "max_completion_tokens": {
                "display_name": "Maximum Completion Tokens",
                "short_desc": "Maximum Completion Tokens",
                "desc": "Upper token limit including reasoning and visible output.",
                "type": AbstractDataProviderTypeMap."*int",
                "example_value": 1024,
            },
            "parallel_tool_calls": {
                "display_name": "Parallel Tool Calls",
                "short_desc": "Parallel Tool Calls",
                "desc": "Allow multiple tool calls in a single response.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
            "disable_tool_validation": {
                "display_name": "Disable Tool Validation",
                "short_desc": "Disable Tool Validation",
                "desc": "Allow tool calls absent from `tools`; useful for external tool dispatch.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
            "include_reasoning": {
                "display_name": "Include Reasoning",
                "short_desc": "Include Reasoning",
                "desc": "Include reasoning in the response; mutually exclusive with `reasoning_format`.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
            "reasoning_effort": {
                "display_name": "Reasoning Effort",
                "short_desc": "Reasoning Effort",
                "desc": "Reasoning effort; supported choices depend on the selected model.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (<AllowedValueInfo>{
                        "value": "none",
                        "display_name": "None",
                    }, <AllowedValueInfo>{
                        "value": "default",
                        "display_name": "Default",
                    }, <AllowedValueInfo>{
                        "value": "minimal",
                        "display_name": "Minimal",
                    }, <AllowedValueInfo>{
                        "value": "low",
                        "display_name": "Low",
                    }, <AllowedValueInfo>{
                        "value": "medium",
                        "display_name": "Medium",
                    }, <AllowedValueInfo>{
                        "value": "high",
                        "display_name": "High",
                    }, <AllowedValueInfo>{
                        "value": "xhigh",
                        "display_name": "Xhigh",
                    }, <AllowedValueInfo>{
                        "value": "max",
                        "display_name": "Max",
                    },),
            },
            "reasoning_format": {
                "display_name": "Reasoning Format",
                "short_desc": "Reasoning Format",
                "desc": "How to expose reasoning; mutually exclusive with `include_reasoning`.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (<AllowedValueInfo>{
                        "value": "hidden",
                        "display_name": "Hidden",
                    }, <AllowedValueInfo>{
                        "value": "raw",
                        "display_name": "Raw",
                    }, <AllowedValueInfo>{
                        "value": "parsed",
                        "display_name": "Parsed",
                    },),
            },
            "service_tier": {
                "display_name": "Service Tier",
                "short_desc": "Service Tier",
                "desc": "Select processing capacity and latency handling.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (<AllowedValueInfo>{
                        "value": "auto",
                        "display_name": "Auto",
                    }, <AllowedValueInfo>{
                        "value": "on_demand",
                        "display_name": "On Demand",
                    }, <AllowedValueInfo>{
                        "value": "flex",
                        "display_name": "Flex",
                    }, <AllowedValueInfo>{
                        "value": "performance",
                        "display_name": "Performance",
                    },),
            },
            "compound_custom": {
                "display_name": "Compound Configuration",
                "short_desc": "Compound Configuration",
                "desc": "Configure Compound models and built-in tools with `models` and `tools`.",
                "type": new GroqCompoundDataType(True),
            },
            "documents": {
                "display_name": "Documents",
                "short_desc": "Documents",
                "desc": "Context documents with a text or JSON `source` and optional citation `id`.",
                "type": new ListDataType("GroqDocuments", new GroqDocumentDataType(), True),
            },
            "search_settings": {
                "display_name": "Search Settings",
                "short_desc": "Search Settings",
                "desc": "Configure web search with `include_domains`, `exclude_domains`, and `country`.",
                "type": new GroqSearchSettingsDataType(True),
            },
            "citation_options": {
                "display_name": "Citation Options",
                "short_desc": "Citation Options",
                "desc": "Include citations for document and search grounding.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (<AllowedValueInfo>{
                        "value": "enabled",
                        "display_name": "Enabled",
                    }, <AllowedValueInfo>{
                        "value": "disabled",
                        "display_name": "Disabled",
                    },),
            },
            "temperature": {
                "display_name": "Temperature",
                "short_desc": "Sampling temperature (0-2); higher values give more varied output",
                "desc": "Controls randomness of the output.  Range `0.0` to `2.0`.  Defaults to "
                    "`1.0`.",
                "type": AbstractDataProviderTypeMap."*softfloat",
                "example_value": 0.7,
            },
            "max_tokens": {
                "display_name": "Max Tokens",
                "short_desc": "Maximum tokens to emit in the completion",
                "desc": "Legacy output limit. Prefer `max_completion_tokens`; do not supply both.",
                "type": AbstractDataProviderTypeMap."*softint",
                "example_value": 1024,
            },
            "top_p": {
                "display_name": "Top-P",
                "short_desc": "Nucleus sampling cutoff (0-1)",
                "desc": "Nucleus sampling cutoff.  Range `0.0` to `1.0`.  Usually left at `1.0`.",
                "type": AbstractDataProviderTypeMap."*softfloat",
            },
            "frequency_penalty": {
                "display_name": "Frequency Penalty",
                "short_desc": "Penalize repeated tokens proportional to their frequency",
                "desc": "Reserved by Groq; currently unsupported by hosted models.",
                "type": AbstractDataProviderTypeMap."*softfloat",
            },
            "presence_penalty": {
                "display_name": "Presence Penalty",
                "short_desc": "Penalize any token that has already appeared",
                "desc": "Reserved by Groq; currently unsupported by hosted models.",
                "type": AbstractDataProviderTypeMap."*softfloat",
            },
            "stop": {
                "display_name": "Stop Sequences",
                "short_desc": "String or list of strings; generation halts when emitted",
                "desc": "One or more stop sequences.  Can be a single string or a list (Groq "
                    "supports up to 4 stop sequences).",
                "type": new QoreUnionDataType((AbstractDataProviderTypeMap."*string",
                    new ListDataType("GroqStopSequences", AbstractDataProviderTypeMap."string", False),)),
            },
            "stream": {
                "display_name": "Stream",
                "short_desc": "Return a completed response",
                "desc": "Use `False` or omit this option. Use `GroqLlmProvider` for SSE streaming.",
                "type": AbstractDataProviderTypeMap."*bool",
                "allowed_values": (<AllowedValueInfo>{"value": False, "display_name": "Completed Response"},),
            },
            "tools": {
                "display_name": "Tools",
                "short_desc": "OpenAI-style tool definitions the model may call",
                "desc": "Custom function definitions or supported built-in tools. Use `function` with a "
                    "`function` definition, or select `browser_search` or `code_interpreter` for a compatible model.",
                "type": new ListDataType("GroqToolsList", new GroqChatToolDataType(), True),
            },
            "tool_choice": {
                "display_name": "Tool Choice",
                "short_desc": "Whether and which tool the model must invoke",
                "desc": "Controls tool invocation.\n\n"
                    "- `none` — never call a tool (text-only reply)\n"
                    "- `auto` — model decides (default when `tools` is set)\n"
                    "- `required` — model must call at least one tool",
                "type": new QoreUnionDataType((AbstractDataProviderTypeMap."*string",
                    AbstractDataProviderTypeMap."hash",)),
                "allowed_values_creatable": True,
                "allowed_values": (
                    <AllowedValueInfo>{"value": "none",
                        "display_name": "None - never call a tool"},
                    <AllowedValueInfo>{"value": "auto",
                        "display_name": "Auto - model decides"},
                    <AllowedValueInfo>{"value": "required",
                        "display_name": "Required - must call a tool"},
                ),
            },
            "response_format": {
                "display_name": "Response Format",
                "short_desc": "Force structured output (e.g. JSON)",
                "desc": "Hash that forces a structured response shape.  The most common use is "
                    "JSON mode: `{type: \"json_object\"}`.",
                "type": new GroqChatResponseFormatDataType(True),
                "example_value": {"type": "json_object"},
            },
            "seed": {
                "display_name": "Seed",
                "short_desc": "Deterministic sampling seed",
                "desc": "Integer seed for deterministic sampling.",
                "type": AbstractDataProviderTypeMap."*softint",
            },
            "user": {
                "display_name": "User ID",
                "short_desc": "Opaque end-user identifier for abuse tracking",
                "desc": "Opaque identifier representing the end user.  Logged server-side for "
                    "abuse tracking and usage attribution; does not affect generation.",
                "type": AbstractDataProviderTypeMap."*softstring",
            },
        };
    }

    #! Creates the object
    constructor() {
        addQoreFields(Fields);
    }
}
}
