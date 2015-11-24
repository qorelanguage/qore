/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreSocket.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_QOREEVENTS_H

#define _QORE_QOREEVENTS_H

//! event source: socket
#define QORE_SOURCE_SOCKET                      1
//! event source: HTTPClient
#define QORE_SOURCE_HTTPCLIENT                  2
//! event source: FtpClient
#define QORE_SOURCE_FTPCLIENT                   3
//! event source: File
#define QORE_SOURCE_FILE                        4

//! event: packet data read
#define QORE_EVENT_PACKET_READ                  1
//! event: packet data sent
#define QORE_EVENT_PACKET_SENT                  2
//! event: http content length received
#define QORE_EVENT_HTTP_CONTENT_LENGTH          3
//! event: http chunked data start
#define QORE_EVENT_HTTP_CHUNKED_START           4
//! event: http chunked data end
#define QORE_EVENT_HTTP_CHUNKED_END             5
//! event: http redirect
#define QORE_EVENT_HTTP_REDIRECT                6
//! event: socket or communications channel closed
#define QORE_EVENT_CHANNEL_CLOSED               7
//! event: object deleted
#define QORE_EVENT_DELETED                      8
//! event: ftp control message to send
#define QORE_EVENT_FTP_SEND_MESSAGE             9
//! event: ftp control message received
#define QORE_EVENT_FTP_MESSAGE_RECEIVED        10
//! event: name being resolved
#define QORE_EVENT_HOSTNAME_LOOKUP             11
//! event: host name resolved
#define QORE_EVENT_HOSTNAME_RESOLVED           12
//! event: sending http request: raised before message sent
#define QORE_EVENT_HTTP_SEND_MESSAGE           13
//! event: read http message: raised after message received
#define QORE_EVENT_HTTP_MESSAGE_RECEIVED       14
//! event: read http headers in footer: raised after message received
#define QORE_EVENT_HTTP_FOOTERS_RECEIVED       15
//! event: chunked data read: raised after data is read
#define QORE_EVENT_HTTP_CHUNKED_DATA_RECEIVED  16
//! event: got next chunk size: raised after data is read
#define QORE_EVENT_HTTP_CHUNK_SIZE             17
//! event: connecting to remote host
#define QORE_EVENT_CONNECTING                  18
//! event: connected to remote host
#define QORE_EVENT_CONNECTED                   19
//! event: negotiating SSL connection
#define QORE_EVENT_START_SSL                   20
//! event: SSL connection established
#define QORE_EVENT_SSL_ESTABLISHED             21
//! event: file being opened
#define QORE_EVENT_OPEN_FILE                   22
//! event: file has been opened
#define QORE_EVENT_FILE_OPENED                 23
//! event: file data read
#define QORE_EVENT_DATA_READ                   24
//! event: file data written
#define QORE_EVENT_DATA_WRITTEN                25

#endif
