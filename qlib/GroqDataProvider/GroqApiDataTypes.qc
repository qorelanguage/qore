# -*- mode: qore; indent-tabs-mode: nil -*-
#! Qore GroqApiDataTypes class definition

/** GroqApiDataTypes.qc Copyright 2026 Qore Technologies, s.r.o.

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
#! ModelRequest fields for the Groq API
public class GroqModelRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "model": {
                "display_name": "Model",
                "short_desc": "Model",
                "desc": "Select the existing resource to inspect or manage.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "models",
                },
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqModelRequestDataType = new GroqModelRequestDataType();

#! FileRequest fields for the Groq API
public class GroqFileRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "file_id": {
                "display_name": "File",
                "short_desc": "File",
                "desc": "Select the existing resource to inspect or manage.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "files",
                },
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqFileRequestDataType = new GroqFileRequestDataType();

#! BatchRequest fields for the Groq API
public class GroqBatchRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "batch_id": {
                "display_name": "Batch",
                "short_desc": "Batch",
                "desc": "Select the existing resource to inspect or manage.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "batches",
                },
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqBatchRequestDataType = new GroqBatchRequestDataType();

#! FineTuningRequest fields for the Groq API
public class GroqFineTuningRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "id": {
                "display_name": "LoRA Adapter",
                "short_desc": "LoRA Adapter",
                "desc": "Select the existing resource to inspect or manage.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "fine-tunings",
                },
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqFineTuningRequestDataType = new GroqFineTuningRequestDataType();

#! UploadFileRequest fields for the Groq API
public class GroqUploadFileRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "file": {
                "display_name": "File",
                "short_desc": "File",
                "desc": "Named file with `name`, raw binary or base64 `content`, and optional `mime_type`.",
                "type": FileDataType,
                "required": True,
                "preselected": True,
            },
            "purpose": {
                "display_name": "Purpose",
                "short_desc": "Purpose",
                "desc": "Upload `batch` JSONL input, or a `fine_tuning` ZIP containing a pre-trained LoRA adapter. **LoRA requires enterprise access.**",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values": (<AllowedValueInfo>{
                        "value": "batch",
                        "display_name": "Batch Input",
                    }, <AllowedValueInfo>{
                        "value": "fine_tuning",
                        "display_name": "LoRA Adapter",
                    },),
                "default_value": "batch",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqUploadFileRequestDataType = new GroqUploadFileRequestDataType();

#! CreateBatchRequest fields for the Groq API
public class GroqCreateBatchRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "input_file_id": {
                "display_name": "Input File",
                "short_desc": "Input File",
                "desc": "Select a JSONL file uploaded with `purpose` set to `batch`.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "batch-files",
                },
            },
            "endpoint": {
                "display_name": "Endpoint",
                "short_desc": "Endpoint",
                "desc": "The inference operation used by every request in the batch.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values": (<AllowedValueInfo>{
                        "value": "/v1/chat/completions",
                        "display_name": "Chat Completions",
                    }, <AllowedValueInfo>{
                        "value": "/v1/audio/transcriptions",
                        "display_name": "Audio Transcriptions",
                    }, <AllowedValueInfo>{
                        "value": "/v1/audio/translations",
                        "display_name": "Audio Translations",
                    },),
                "default_value": "/v1/chat/completions",
            },
            "completion_window": {
                "display_name": "Completion Window",
                "short_desc": "Completion Window",
                "desc": "Maximum processing duration, from `24h` through `7d`, for example `36h`.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "example_value": "24h",
                "default_value": "24h",
            },
            "metadata": {
                "display_name": "Metadata",
                "short_desc": "Metadata",
                "desc": "Custom key/value metadata for tracking this batch.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqCreateBatchRequestDataType = new GroqCreateBatchRequestDataType();

#! CreateFineTuningRequest fields for the Groq API
public class GroqCreateFineTuningRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "input_file_id": {
                "display_name": "Adapter File",
                "short_desc": "Adapter File",
                "desc": "A ZIP uploaded with `purpose` set to `fine_tuning`.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "adapter-files",
                },
            },
            "name": {
                "display_name": "Name",
                "short_desc": "Name",
                "desc": "A recognizable name for this pre-trained adapter.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "example_value": "invoice-extractor",
            },
            "base_model": {
                "display_name": "Base Model",
                "short_desc": "Base Model",
                "desc": "Exact Groq base model version used when training the adapter externally.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "chat-models",
                },
            },
            "type": {
                "display_name": "Adapter Type",
                "short_desc": "Adapter Type",
                "desc": "The pre-trained adapter format. Groq hosts adapters; it does not train them.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values": (<AllowedValueInfo>{
                        "value": "lora",
                        "display_name": "LoRA",
                    },),
                "default_value": "lora",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqCreateFineTuningRequestDataType = new GroqCreateFineTuningRequestDataType();

#! Reasoning fields for the Groq API
public class GroqReasoningDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "effort": {
                "display_name": "Effort",
                "short_desc": "Effort",
                "desc": "Reasoning depth: `none` disables reasoning on Qwen3.8; GPT-OSS supports `low`, `medium`, and `high`.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (<AllowedValueInfo>{"value": "none", "display_name": "None"}, <AllowedValueInfo>{"value": "low", "display_name": "Low"}, <AllowedValueInfo>{"value": "medium", "display_name": "Medium"}, <AllowedValueInfo>{"value": "high", "display_name": "High"},),
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
public const GroqReasoningDataType = new GroqReasoningDataType();

#! ResponseFormat fields for the Groq API
public class GroqResponseFormatDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "type": {
                "display_name": "Format",
                "short_desc": "Format",
                "desc": "Choose text, JSON mode, or strict JSON schema output.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values": (<AllowedValueInfo>{
                        "value": "text",
                        "display_name": "Text",
                    }, <AllowedValueInfo>{
                        "value": "json_object",
                        "display_name": "JSON Object",
                    }, <AllowedValueInfo>{
                        "value": "json_schema",
                        "display_name": "JSON Schema",
                    },),
            },
            "name": {
                "display_name": "Schema Name",
                "short_desc": "Schema Name",
                "desc": "Schema name for Responses structured output.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "invoice",
            },
            "schema": {
                "display_name": "Schema",
                "short_desc": "Schema",
                "desc": "JSON Schema describing the required Responses output.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "strict": {
                "display_name": "Strict",
                "short_desc": "Strict",
                "desc": "Whether to enforce the output schema exactly.",
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
public const GroqResponseFormatDataType = new GroqResponseFormatDataType();

#! ResponseText fields for the Groq API
public class GroqResponseTextDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "format": {
                "display_name": "Format",
                "short_desc": "Format",
                "desc": "Configure text or structured JSON output.",
                "type": new GroqResponseFormatDataType(True),
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
public const GroqResponseTextDataType = new GroqResponseTextDataType();

#! CreateResponseRequest fields for the Groq API
public class GroqCreateResponseRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "model": {
                "display_name": "Model",
                "short_desc": "Model",
                "desc": "Choose an active model suited to this action from the live Groq catalog.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "response-models",
                },
            },
            "input": {
                "display_name": "Input",
                "short_desc": "Input",
                "desc": "Text or a list of input messages and tool results.",
                "type": new QoreUnionDataType((AbstractDataProviderTypeMap."string", new ListDataType("GroqResponseInput", AbstractDataProviderTypeMap."hash", False),)),
                "required": True,
                "preselected": True,
                "example_value": "Summarize the attached invoice.",
            },
            "instructions": {
                "display_name": "Instructions",
                "short_desc": "Instructions",
                "desc": "Instructions guiding the response.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "Return a concise summary.",
            },
            "max_output_tokens": {
                "display_name": "Maximum Output Tokens",
                "short_desc": "Maximum Output Tokens",
                "desc": "Upper limit including reasoning and visible output.",
                "type": AbstractDataProviderTypeMap."*int",
                "example_value": 256,
            },
            "temperature": {
                "display_name": "Temperature",
                "short_desc": "Temperature",
                "desc": "Output randomness between `0` and `2`.",
                "type": AbstractDataProviderTypeMap."*float",
            },
            "top_p": {
                "display_name": "Top P",
                "short_desc": "Top P",
                "desc": "Nucleus sampling probability between `0` and `1`.",
                "type": AbstractDataProviderTypeMap."*float",
            },
            "tools": {
                "display_name": "Tools",
                "short_desc": "Tools",
                "desc": "Functions, built-in browser or code tools, and remote MCP servers. See the [Responses guide](https://console.groq.com/docs/responses-api).",
                "type": new ListDataType("GroqResponseTools", new GroqResponseToolDataType(), True),
            },
            "tool_choice": {
                "display_name": "Tool Choice",
                "short_desc": "Tool Choice",
                "desc": "Choose `none`, `auto`, `required`, or a named tool object.",
                "type": new QoreUnionDataType((AbstractDataProviderTypeMap."*string", AbstractDataProviderTypeMap."hash",)),
                "allowed_values": (<AllowedValueInfo>{
                        "value": "none",
                        "display_name": "None",
                    }, <AllowedValueInfo>{
                        "value": "auto",
                        "display_name": "Auto",
                    }, <AllowedValueInfo>{
                        "value": "required",
                        "display_name": "Required",
                    },),
                "allowed_values_creatable": True,
            },
            "parallel_tool_calls": {
                "display_name": "Parallel Tool Calls",
                "short_desc": "Parallel Tool Calls",
                "desc": "Allow more than one tool call in a response.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
            "reasoning": {
                "display_name": "Reasoning",
                "short_desc": "Reasoning",
                "desc": "Configure reasoning for a compatible model.",
                "type": new GroqReasoningDataType(True),
            },
            "text": {
                "display_name": "Text",
                "short_desc": "Text",
                "desc": "Choose text or JSON output.",
                "type": new GroqResponseTextDataType(True),
            },
            "metadata": {
                "display_name": "Metadata",
                "short_desc": "Metadata",
                "desc": "Up to sixteen metadata pairs.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "service_tier": {
                "display_name": "Service Tier",
                "short_desc": "Service Tier",
                "desc": "Choose latency and capacity handling.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (<AllowedValueInfo>{
                        "value": "auto",
                        "display_name": "Auto",
                    }, <AllowedValueInfo>{
                        "value": "default",
                        "display_name": "Default",
                    }, <AllowedValueInfo>{
                        "value": "flex",
                        "display_name": "Flex",
                    },),
            },
            "user": {
                "display_name": "User ID",
                "short_desc": "User ID",
                "desc": "An application-specific end-user identifier.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "customer-123",
            },
            "store": {
                "display_name": "Store",
                "short_desc": "Store",
                "desc": "Groq currently supports only `False`.",
                "type": AbstractDataProviderTypeMap."*bool",
                "allowed_values": (<AllowedValueInfo>{
                        "value": False,
                        "display_name": "Do Not Store",
                    },),
            },
            "stream": {
                "display_name": "Stream",
                "short_desc": "Stream",
                "desc": "Completed responses require `False`. Use `GroqLlmProvider` for streaming chat.",
                "type": AbstractDataProviderTypeMap."*bool",
                "allowed_values": (<AllowedValueInfo>{
                        "value": False,
                        "display_name": "Completed Response",
                    },),
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqCreateResponseRequestDataType = new GroqCreateResponseRequestDataType();

#! TranscriptionRequest fields for the Groq API
public class GroqTranscriptionRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "file": {
                "display_name": "File",
                "short_desc": "File",
                "desc": "Named file with `name`, raw binary or base64 `content`, and optional `mime_type`.",
                "type": FileOrNothingDataType,
                "required": False,
                "preselected": True,
                "attr": {
                    "required_groups": ("audio_input",),
                },
            },
            "url": {
                "display_name": "Audio URL",
                "short_desc": "Audio URL",
                "desc": "Public audio URL or a base64 data URL. Supply exactly one of `file` or `url`.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "https://example.com/customer-call.wav",
                "attr": {
                    "required_groups": ("audio_input",),
                },
            },
            "model": {
                "display_name": "Model",
                "short_desc": "Model",
                "desc": "Choose an active model suited to this action from the live Groq catalog.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "transcription-models",
                },
            },
            "prompt": {
                "display_name": "Prompt",
                "short_desc": "Prompt",
                "desc": "Spelling hints or context for the recording.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "Qore Technologies customer support call.",
            },
            "response_format": {
                "display_name": "Response Format",
                "short_desc": "Response Format",
                "desc": "Return JSON, timestamped verbose JSON, or plain text.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (<AllowedValueInfo>{
                        "value": "json",
                        "display_name": "Json",
                    }, <AllowedValueInfo>{
                        "value": "verbose_json",
                        "display_name": "Verbose Json",
                    }, <AllowedValueInfo>{
                        "value": "text",
                        "display_name": "Text",
                    },),
                "default_value": "json",
            },
            "temperature": {
                "display_name": "Temperature",
                "short_desc": "Temperature",
                "desc": "Sampling temperature between `0` and `1`.",
                "type": AbstractDataProviderTypeMap."*float",
            },
            "language": {
                "display_name": "Language",
                "short_desc": "Language",
                "desc": "ISO 639-1 input language code; omit to detect it.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "en",
            },
            "timestamp_granularities": {
                "display_name": "Timestamp Granularities",
                "short_desc": "Timestamp Granularities",
                "desc": "Word and/or segment timestamps; requires `response_format` of `verbose_json`.",
                "type": new ListDataType("GroqTimestampGranularities", AbstractDataProviderTypeMap."string", True),
                "element_allowed_values": (<AllowedValueInfo>{
                        "value": "word",
                        "display_name": "Word",
                    }, <AllowedValueInfo>{
                        "value": "segment",
                        "display_name": "Segment",
                    },),
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqTranscriptionRequestDataType = new GroqTranscriptionRequestDataType();

#! TranslationRequest fields for the Groq API
public class GroqTranslationRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "file": {
                "display_name": "File",
                "short_desc": "File",
                "desc": "Named file with `name`, raw binary or base64 `content`, and optional `mime_type`.",
                "type": FileOrNothingDataType,
                "required": False,
                "preselected": True,
                "attr": {
                    "required_groups": ("audio_input",),
                },
            },
            "url": {
                "display_name": "Audio URL",
                "short_desc": "Audio URL",
                "desc": "Public audio URL or a base64 data URL. Supply exactly one of `file` or `url`.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "https://example.com/customer-call.wav",
                "attr": {
                    "required_groups": ("audio_input",),
                },
            },
            "model": {
                "display_name": "Model",
                "short_desc": "Model",
                "desc": "Choose an active model suited to this action from the live Groq catalog.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "translation-models",
                },
            },
            "prompt": {
                "display_name": "Prompt",
                "short_desc": "Prompt",
                "desc": "Spelling hints or context for the recording.",
                "type": AbstractDataProviderTypeMap."*string",
                "example_value": "Qore Technologies customer support call.",
            },
            "response_format": {
                "display_name": "Response Format",
                "short_desc": "Response Format",
                "desc": "Return JSON, timestamped verbose JSON, or plain text.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (<AllowedValueInfo>{
                        "value": "json",
                        "display_name": "Json",
                    }, <AllowedValueInfo>{
                        "value": "verbose_json",
                        "display_name": "Verbose Json",
                    }, <AllowedValueInfo>{
                        "value": "text",
                        "display_name": "Text",
                    },),
                "default_value": "json",
            },
            "temperature": {
                "display_name": "Temperature",
                "short_desc": "Temperature",
                "desc": "Sampling temperature between `0` and `1`.",
                "type": AbstractDataProviderTypeMap."*float",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqTranslationRequestDataType = new GroqTranslationRequestDataType();

#! SpeechRequest fields for the Groq API
public class GroqSpeechRequestDataType inherits HashDataType {
    public {
        #! Field definitions
        const Fields = {
            "model": {
                "display_name": "Model",
                "short_desc": "Model",
                "desc": "Choose an active model suited to this action from the live Groq catalog.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "speech-models",
                },
            },
            "input": {
                "display_name": "Text",
                "short_desc": "Text",
                "desc": "Text to turn into speech.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "example_value": "Your order has shipped.",
            },
            "voice": {
                "display_name": "Voice",
                "short_desc": "Voice",
                "desc": "Voice for the selected model. Orpheus voice choices follow the published model guide.",
                "type": AbstractDataProviderTypeMap."string",
                "required": True,
                "preselected": True,
                "allowed_values_creatable": True,
                "attr": {
                    "ref_data": "voices",
                    "depends_on": ("model",),
                },
            },
            "response_format": {
                "display_name": "Audio Format",
                "short_desc": "Audio Format",
                "desc": "Orpheus supports WAV output. Other formats depend on the model.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (<AllowedValueInfo>{
                        "value": "wav",
                        "display_name": "Wav",
                    }, <AllowedValueInfo>{
                        "value": "flac",
                        "display_name": "Flac",
                    }, <AllowedValueInfo>{
                        "value": "mp3",
                        "display_name": "Mp3",
                    }, <AllowedValueInfo>{
                        "value": "mulaw",
                        "display_name": "Mulaw",
                    }, <AllowedValueInfo>{
                        "value": "ogg",
                        "display_name": "Ogg",
                    },),
                "default_value": "wav",
            },
            "sample_rate": {
                "display_name": "Sample Rate",
                "short_desc": "Sample Rate",
                "desc": "Output samples per second, where supported by the model.",
                "type": AbstractDataProviderTypeMap."*int",
                "allowed_values": (<AllowedValueInfo>{
                        "value": 8000,
                        "display_name": "8000",
                    }, <AllowedValueInfo>{
                        "value": 16000,
                        "display_name": "16000",
                    }, <AllowedValueInfo>{
                        "value": 22050,
                        "display_name": "22050",
                    }, <AllowedValueInfo>{
                        "value": 24000,
                        "display_name": "24000",
                    }, <AllowedValueInfo>{
                        "value": 32000,
                        "display_name": "32000",
                    }, <AllowedValueInfo>{
                        "value": 44100,
                        "display_name": "44100",
                    }, <AllowedValueInfo>{
                        "value": 48000,
                        "display_name": "48000",
                    },),
            },
            "speed": {
                "display_name": "Speed",
                "short_desc": "Speed",
                "desc": "Speech speed from `0.5` to `5`, where supported by the model.",
                "type": AbstractDataProviderTypeMap."*float",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
    }
}

#! Shared type instance
public const GroqSpeechRequestDataType = new GroqSpeechRequestDataType();

#! FileResponse fields for the Groq API
public class GroqFileResponseDataType inherits HashDataType {
    private {
        #! Field definitions
        const Fields = {
            "id": {
                "display_name": "File ID",
                "short_desc": "File ID",
                "desc": "Uploaded file identifier.",
                "type": AbstractDataProviderTypeMap."string",
            },
            "object": {
                "display_name": "Object Type",
                "short_desc": "Object Type",
                "desc": "Resource discriminator.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "bytes": {
                "display_name": "Bytes",
                "short_desc": "Bytes",
                "desc": "Size of the file in bytes.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "created_at": {
                "display_name": "Created At",
                "short_desc": "Created At",
                "desc": "Creation time in Unix seconds.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "filename": {
                "display_name": "Filename",
                "short_desc": "Filename",
                "desc": "Original filename.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "purpose": {
                "display_name": "Purpose",
                "short_desc": "Purpose",
                "desc": "Purpose of the uploaded file.",
                "type": AbstractDataProviderTypeMap."*string",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
        setDefaultOtherFieldType(AbstractDataProviderTypeMap."auto");
    }
}

#! Shared type instance
public const GroqFileResponseDataType = new GroqFileResponseDataType();

#! DeleteResponse fields for the Groq API
public class GroqDeleteResponseDataType inherits HashDataType {
    private {
        #! Field definitions
        const Fields = {
            "id": {
                "display_name": "Resource ID",
                "short_desc": "Resource ID",
                "desc": "Identifier of the deleted resource.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "object": {
                "display_name": "Object Type",
                "short_desc": "Object Type",
                "desc": "Resource discriminator.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "deleted": {
                "display_name": "Deleted",
                "short_desc": "Deleted",
                "desc": "Whether the resource was deleted.",
                "type": AbstractDataProviderTypeMap."bool",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
        setDefaultOtherFieldType(AbstractDataProviderTypeMap."auto");
    }
}

#! Shared type instance
public const GroqDeleteResponseDataType = new GroqDeleteResponseDataType();

#! BatchResponse fields for the Groq API
public class GroqBatchResponseDataType inherits HashDataType {
    private {
        #! Field definitions
        const Fields = {
            "id": {
                "display_name": "Batch ID",
                "short_desc": "Batch ID",
                "desc": "Identifier of this batch.",
                "type": AbstractDataProviderTypeMap."string",
            },
            "object": {
                "display_name": "Object Type",
                "short_desc": "Object Type",
                "desc": "Resource discriminator.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "status": {
                "display_name": "Status",
                "short_desc": "Status",
                "desc": "Current processing state.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (<AllowedValueInfo>{
                        "value": "validating",
                        "display_name": "Validating",
                    }, <AllowedValueInfo>{
                        "value": "failed",
                        "display_name": "Failed",
                    }, <AllowedValueInfo>{
                        "value": "in_progress",
                        "display_name": "In Progress",
                    }, <AllowedValueInfo>{
                        "value": "finalizing",
                        "display_name": "Finalizing",
                    }, <AllowedValueInfo>{
                        "value": "completed",
                        "display_name": "Completed",
                    }, <AllowedValueInfo>{
                        "value": "expired",
                        "display_name": "Expired",
                    }, <AllowedValueInfo>{
                        "value": "cancelling",
                        "display_name": "Cancelling",
                    }, <AllowedValueInfo>{
                        "value": "cancelled",
                        "display_name": "Cancelled",
                    },),
            },
            "endpoint": {
                "display_name": "Endpoint",
                "short_desc": "Endpoint",
                "desc": "Inference endpoint for this batch.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "completion_window": {
                "display_name": "Completion Window",
                "short_desc": "Completion Window",
                "desc": "Maximum processing duration.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "input_file_id": {
                "display_name": "Input File",
                "short_desc": "Input File",
                "desc": "Batch input file identifier.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "output_file_id": {
                "display_name": "Output File",
                "short_desc": "Output File",
                "desc": "Completed results file identifier.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "error_file_id": {
                "display_name": "Error File",
                "short_desc": "Error File",
                "desc": "Error results file identifier.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "errors": {
                "display_name": "Errors",
                "short_desc": "Errors",
                "desc": "Batch validation errors.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "metadata": {
                "display_name": "Metadata",
                "short_desc": "Metadata",
                "desc": "Application metadata.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "request_counts": {
                "display_name": "Request Counts",
                "short_desc": "Request Counts",
                "desc": "Counts of `total`, `completed`, and `failed` requests.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "created_at": {
                "display_name": "Created At",
                "short_desc": "Created At",
                "desc": "Event time in Unix seconds, when available.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "in_progress_at": {
                "display_name": "In Progress At",
                "short_desc": "In Progress At",
                "desc": "Event time in Unix seconds, when available.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "expires_at": {
                "display_name": "Expires At",
                "short_desc": "Expires At",
                "desc": "Event time in Unix seconds, when available.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "finalizing_at": {
                "display_name": "Finalizing At",
                "short_desc": "Finalizing At",
                "desc": "Event time in Unix seconds, when available.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "completed_at": {
                "display_name": "Completed At",
                "short_desc": "Completed At",
                "desc": "Event time in Unix seconds, when available.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "failed_at": {
                "display_name": "Failed At",
                "short_desc": "Failed At",
                "desc": "Event time in Unix seconds, when available.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "expired_at": {
                "display_name": "Expired At",
                "short_desc": "Expired At",
                "desc": "Event time in Unix seconds, when available.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "cancelling_at": {
                "display_name": "Cancelling At",
                "short_desc": "Cancelling At",
                "desc": "Event time in Unix seconds, when available.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "cancelled_at": {
                "display_name": "Cancelled At",
                "short_desc": "Cancelled At",
                "desc": "Event time in Unix seconds, when available.",
                "type": AbstractDataProviderTypeMap."*int",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
        setDefaultOtherFieldType(AbstractDataProviderTypeMap."auto");
    }
}

#! Shared type instance
public const GroqBatchResponseDataType = new GroqBatchResponseDataType();

#! FineTuning fields for the Groq API
public class GroqFineTuningDataType inherits HashDataType {
    private {
        #! Field definitions
        const Fields = {
            "id": {
                "display_name": "Id",
                "short_desc": "Id",
                "desc": "Adapter identifier.",
                "type": AbstractDataProviderTypeMap."string",
            },
            "name": {
                "display_name": "Name",
                "short_desc": "Name",
                "desc": "Adapter name.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "base_model": {
                "display_name": "Base Model",
                "short_desc": "Base Model",
                "desc": "Base model identifier.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "type": {
                "display_name": "Type",
                "short_desc": "Type",
                "desc": "Adapter format.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "input_file_id": {
                "display_name": "Input File Id",
                "short_desc": "Input File Id",
                "desc": "Uploaded adapter file.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "created_at": {
                "display_name": "Created At",
                "short_desc": "Created At",
                "desc": "Creation time in Unix seconds.",
                "type": AbstractDataProviderTypeMap."*softfloat",
            },
            "fine_tuned_model": {
                "display_name": "Fine Tuned Model",
                "short_desc": "Fine Tuned Model",
                "desc": "Model identifier to use for inference.",
                "type": AbstractDataProviderTypeMap."*string",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
        setDefaultOtherFieldType(AbstractDataProviderTypeMap."auto");
    }
}

#! Shared type instance
public const GroqFineTuningDataType = new GroqFineTuningDataType();

#! FineTuningResponse fields for the Groq API
public class GroqFineTuningResponseDataType inherits HashDataType {
    private {
        #! Field definitions
        const Fields = {
            "id": {
                "display_name": "ID",
                "short_desc": "ID",
                "desc": "Response identifier.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "object": {
                "display_name": "Object Type",
                "short_desc": "Object Type",
                "desc": "Resource discriminator.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "data": {
                "display_name": "Adapter",
                "short_desc": "Adapter",
                "desc": "Adapter metadata.",
                "type": new GroqFineTuningDataType(),
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
        setDefaultOtherFieldType(AbstractDataProviderTypeMap."auto");
    }
}

#! Shared type instance
public const GroqFineTuningResponseDataType = new GroqFineTuningResponseDataType();

#! ListFilesResponse fields for the Groq API
public class GroqListFilesResponseDataType inherits HashDataType {
    private {
        #! Field definitions
        const Fields = {
            "object": {
                "display_name": "Object Type",
                "short_desc": "Object Type",
                "desc": "List discriminator.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "data": {
                "display_name": "Records",
                "short_desc": "Records",
                "desc": "Resources returned by the service. Further results may be available if `has_more` is set.",
                "type": new ListDataType("GroqFileRecords", new GroqFileResponseDataType(), False),
            },
            "first_id": {
                "display_name": "First ID",
                "short_desc": "First ID",
                "desc": "First returned resource identifier.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "last_id": {
                "display_name": "Last ID",
                "short_desc": "Last ID",
                "desc": "Last returned resource identifier.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "has_more": {
                "display_name": "Has More",
                "short_desc": "Has More",
                "desc": "Whether more resources are available.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
        setDefaultOtherFieldType(AbstractDataProviderTypeMap."auto");
    }
}

#! Shared type instance
public const GroqListFilesResponseDataType = new GroqListFilesResponseDataType();

#! ListBatchesResponse fields for the Groq API
public class GroqListBatchesResponseDataType inherits HashDataType {
    private {
        #! Field definitions
        const Fields = {
            "object": {
                "display_name": "Object Type",
                "short_desc": "Object Type",
                "desc": "List discriminator.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "data": {
                "display_name": "Records",
                "short_desc": "Records",
                "desc": "Resources returned by the service. Further results may be available if `has_more` is set.",
                "type": new ListDataType("GroqBatchRecords", new GroqBatchResponseDataType(), False),
            },
            "first_id": {
                "display_name": "First ID",
                "short_desc": "First ID",
                "desc": "First returned resource identifier.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "last_id": {
                "display_name": "Last ID",
                "short_desc": "Last ID",
                "desc": "Last returned resource identifier.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "has_more": {
                "display_name": "Has More",
                "short_desc": "Has More",
                "desc": "Whether more resources are available.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
        setDefaultOtherFieldType(AbstractDataProviderTypeMap."auto");
    }
}

#! Shared type instance
public const GroqListBatchesResponseDataType = new GroqListBatchesResponseDataType();

#! ListFineTuningsResponse fields for the Groq API
public class GroqListFineTuningsResponseDataType inherits HashDataType {
    private {
        #! Field definitions
        const Fields = {
            "object": {
                "display_name": "Object Type",
                "short_desc": "Object Type",
                "desc": "List discriminator.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "data": {
                "display_name": "Records",
                "short_desc": "Records",
                "desc": "Resources returned by the service. Further results may be available if `has_more` is set.",
                "type": new ListDataType("GroqFineTuningRecords", new GroqFineTuningDataType(), False),
            },
            "first_id": {
                "display_name": "First ID",
                "short_desc": "First ID",
                "desc": "First returned resource identifier.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "last_id": {
                "display_name": "Last ID",
                "short_desc": "Last ID",
                "desc": "Last returned resource identifier.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "has_more": {
                "display_name": "Has More",
                "short_desc": "Has More",
                "desc": "Whether more resources are available.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
        setDefaultOtherFieldType(AbstractDataProviderTypeMap."auto");
    }
}

#! Shared type instance
public const GroqListFineTuningsResponseDataType = new GroqListFineTuningsResponseDataType();

#! AudioResponse fields for the Groq API
public class GroqAudioResponseDataType inherits HashDataType {
    private {
        #! Field definitions
        const Fields = {
            "text": {
                "display_name": "Text",
                "short_desc": "Text",
                "desc": "Transcribed or translated recording.",
                "type": AbstractDataProviderTypeMap."string",
            },
            "task": {
                "display_name": "Task",
                "short_desc": "Task",
                "desc": "Operation performed.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "language": {
                "display_name": "Language",
                "short_desc": "Language",
                "desc": "Detected language.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "duration": {
                "display_name": "Duration",
                "short_desc": "Duration",
                "desc": "Audio duration in seconds.",
                "type": AbstractDataProviderTypeMap."*float",
            },
            "segments": {
                "display_name": "Segments",
                "short_desc": "Segments",
                "desc": "Timestamped audio segments.",
                "type": new ListDataType("GroqAudioSegments", AbstractDataProviderTypeMap."hash", True),
            },
            "words": {
                "display_name": "Words",
                "short_desc": "Words",
                "desc": "Timestamped words.",
                "type": new ListDataType("GroqAudioWords", AbstractDataProviderTypeMap."hash", True),
            },
            "x_groq": {
                "display_name": "Groq Metadata",
                "short_desc": "Groq Metadata",
                "desc": "Service request metadata.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
        setDefaultOtherFieldType(AbstractDataProviderTypeMap."auto");
    }
}

#! Shared type instance
public const GroqAudioResponseDataType = new GroqAudioResponseDataType();

#! Response fields for the Groq API
public class GroqResponseDataType inherits HashDataType {
    private {
        #! Field definitions
        const Fields = {
            "id": {
                "display_name": "Response ID",
                "short_desc": "Response ID",
                "desc": "Unique response identifier.",
                "type": AbstractDataProviderTypeMap."string",
            },
            "object": {
                "display_name": "Object Type",
                "short_desc": "Object Type",
                "desc": "Resource discriminator.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "status": {
                "display_name": "Status",
                "short_desc": "Status",
                "desc": "Completion state.",
                "type": AbstractDataProviderTypeMap."*string",
                "allowed_values": (<AllowedValueInfo>{
                        "value": "completed",
                        "display_name": "Completed",
                    }, <AllowedValueInfo>{
                        "value": "failed",
                        "display_name": "Failed",
                    }, <AllowedValueInfo>{
                        "value": "in_progress",
                        "display_name": "In Progress",
                    }, <AllowedValueInfo>{
                        "value": "incomplete",
                        "display_name": "Incomplete",
                    },),
            },
            "created_at": {
                "display_name": "Created At",
                "short_desc": "Created At",
                "desc": "Creation time in Unix seconds.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "model": {
                "display_name": "Model",
                "short_desc": "Model",
                "desc": "Model used for inference.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "background": {
                "display_name": "Background",
                "short_desc": "Background",
                "desc": "Whether generation ran in the background.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
            "instructions": {
                "display_name": "Instructions",
                "short_desc": "Instructions",
                "desc": "Instructions used for generation.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "max_output_tokens": {
                "display_name": "Maximum Output Tokens",
                "short_desc": "Maximum Output Tokens",
                "desc": "Configured generation token limit.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "max_tool_calls": {
                "display_name": "Maximum Tool Calls",
                "short_desc": "Maximum Tool Calls",
                "desc": "Configured tool-call limit.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "metadata": {
                "display_name": "Metadata",
                "short_desc": "Metadata",
                "desc": "Application metadata attached to the response.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "parallel_tool_calls": {
                "display_name": "Parallel Tool Calls",
                "short_desc": "Parallel Tool Calls",
                "desc": "Whether parallel tool calls were enabled.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
            "previous_response_id": {
                "display_name": "Previous Response ID",
                "short_desc": "Previous Response ID",
                "desc": "Reserved continuation identifier; currently absent.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "reasoning": {
                "display_name": "Reasoning",
                "short_desc": "Reasoning",
                "desc": "Reasoning configuration used by the model.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "service_tier": {
                "display_name": "Service Tier",
                "short_desc": "Service Tier",
                "desc": "Processing tier used for this request.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "store": {
                "display_name": "Stored",
                "short_desc": "Stored",
                "desc": "Whether the response was stored.",
                "type": AbstractDataProviderTypeMap."*bool",
            },
            "temperature": {
                "display_name": "Temperature",
                "short_desc": "Temperature",
                "desc": "Sampling temperature used for this request.",
                "type": AbstractDataProviderTypeMap."*float",
            },
            "text": {
                "display_name": "Text",
                "short_desc": "Text",
                "desc": "Output format configuration.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "tool_choice": {
                "display_name": "Tool Choice",
                "short_desc": "Tool Choice",
                "desc": "Tool-selection configuration.",
                "type": new QoreUnionDataType((AbstractDataProviderTypeMap."*string", AbstractDataProviderTypeMap."hash",)),
            },
            "tools": {
                "display_name": "Tools",
                "short_desc": "Tools",
                "desc": "Tool definitions supplied to the model.",
                "type": new ListDataType("GroqResponseAvailableTools", AbstractDataProviderTypeMap."hash", True),
            },
            "top_logprobs": {
                "display_name": "Top Log Probabilities",
                "short_desc": "Top Log Probabilities",
                "desc": "Number of top log probabilities requested.",
                "type": AbstractDataProviderTypeMap."*int",
            },
            "top_p": {
                "display_name": "Top P",
                "short_desc": "Top P",
                "desc": "Nucleus sampling cutoff.",
                "type": AbstractDataProviderTypeMap."*float",
            },
            "truncation": {
                "display_name": "Truncation",
                "short_desc": "Truncation",
                "desc": "Context truncation strategy.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "user": {
                "display_name": "User ID",
                "short_desc": "User ID",
                "desc": "Application-specific user identifier.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "output_text": {
                "display_name": "Output Text",
                "short_desc": "Output Text",
                "desc": "Convenience text output when returned by the service.",
                "type": AbstractDataProviderTypeMap."*string",
            },
            "x_groq": {
                "display_name": "Groq Metadata",
                "short_desc": "Groq Metadata",
                "desc": "Service request metadata.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "output": {
                "display_name": "Output",
                "short_desc": "Output",
                "desc": "Generated messages, reasoning, and tool calls.",
                "type": new ListDataType("GroqResponseOutput", AbstractDataProviderTypeMap."hash", False),
            },
            "usage": {
                "display_name": "Usage",
                "short_desc": "Usage",
                "desc": "Input, output, and total token usage.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "error": {
                "display_name": "Error",
                "short_desc": "Error",
                "desc": "Service error information, if generation failed.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
            "incomplete_details": {
                "display_name": "Incomplete Details",
                "short_desc": "Incomplete Details",
                "desc": "Reason generation did not complete.",
                "type": AbstractDataProviderTypeMap."*hash",
            },
        };
    }

    #! Creates the typed fields
    constructor() {
        addQoreFields(Fields);
        setDefaultOtherFieldType(AbstractDataProviderTypeMap."auto");
    }
}

#! Shared type instance
public const GroqResponseDataType = new GroqResponseDataType();
}
