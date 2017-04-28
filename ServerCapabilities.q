# -*- mode: qore; indent-tabs-mode: nil -*-

/*  ServerCapabilities.q Copyright 2017 Qore Technologies, s.r.o.

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

#! LSP TextDocumentSyncKind definition
const TextDocumentSyncKind = {
    /**
     * Documents should not be synced at all.
     */
    "None": 0,

    /**
     * Documents are synced by always sending the full content
     * of the document.
     */
    "Full": 1,

    /**
     * Documents are synced by sending the full content on open.
     * After that only incremental updates to the document are
     * send.
     */
    "Incremental": 2
};

#! QLS LSP ServerCapabilities
const QLSServerCapabilities = {
    /**
     * Defines how text documents are synced. Is either a detailed structure defining each notification or
     * for backwards compatibility the TextDocumentSyncKind number.
     */
    "textDocumentSync": TextDocumentSyncKind.Full,

    /**
     * The server provides hover support.
     */
    "hoverProvider": True,

    /**
     * The server provides completion support.
     */
    /*"completionProvider": {
        "resolveProvider": False
    },*/

    /**
     * The server provides signature help support.
     */
    #"signatureHelpProvider": SignatureHelpOptions,

    /**
     * The server provides goto definition support.
     */
    "definitionProvider": False,

    /**
     * The server provides find references support.
     */
    "referencesProvider": True,

    /**
     * The server provides document highlight support.
     */
    "documentHighlightProvider": False,

    /**
     * The server provides document symbol support.
     */
    "documentSymbolProvider": False,

    /**
     * The server provides workspace symbol support.
     */
    "workspaceSymbolProvider": False,

    /**
     * The server provides code actions.
     */
    "codeActionProvider": False,

    /**
     * The server provides code lens.
     */
    /*"codeLensProvider": {
        "resolveProvider": False
    },*/

    /**
     * The server provides document formatting.
     */
    "documentFormattingProvider": False,

    /**
     * The server provides document range formatting.
     */
    "documentRangeFormattingProvider": False,

    /**
     * The server provides document formatting on typing.
     */
    #"documentOnTypeFormattingProvider": DocumentOnTypeFormattingOptions,

    /**
     * The server provides rename support.
     */
    "renameProvider": False,

    /**
     * The server provides document link support.
     */
    /*"documentLinkProvider": {
        "resolveProvider": False
    },*/

    /**
     * The server provides execute command support.
     */
    #"executeCommandProvider": ExecuteCommandOptions,
};
