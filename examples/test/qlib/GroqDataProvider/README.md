# Groq integration tests

Run the three Qore test scripts with `--enable-debug` after rebuilding
`GroqDataProvider-qmod`. The offline suites use mock transports and local HTTP
servers; they need no Groq credentials.

Live checks use `GROQ_LIVE=1` and `GROQ_APIKEY`, or a saved connection named by
`GROQ_CONNECTION` with its connection provider available on the module path.
`GROQ_CHAT_MODEL` defaults to `openai/gpt-oss-20b`; chat and Responses are limited
to 128 output tokens each.

- `GROQ_AUDIO_TESTS=1`: transcribe and translate the bundled recording.
- `GROQ_SPEECH_TESTS=1`: generate a short WAV with Orpheus, after accepting the
  model terms in the Groq console.
- `GROQ_TOOL_TESTS=1`: request a function call and run a small calculation with
  the Responses code execution tool (up to 256 and 512 output tokens).
- `GROQ_FILE_TESTS=1`: upload, inspect, download, and delete a temporary batch file.
  This requires an eligible account. The test does not submit a batch.
- `GROQ_RESTRICTED_PLAN_TESTS=1`: verify explicit `403 not_available_for_plan`
  errors for file, batch, and LoRA listings on accounts without these features.
  Do not enable this flag for an account that has access.

`order.wav` contains the synthetic phrase "Your order has shipped." It was
created for these tests with eSpeak NG (`espeak-ng -s 155 -w order.wav
'Your order has shipped.'`). It contains no personal or third-party recording.

Copyright 2026 Qore Technologies, s.r.o.
