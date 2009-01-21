/*
  QoreSocket.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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
