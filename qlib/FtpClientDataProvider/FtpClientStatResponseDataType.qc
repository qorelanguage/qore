# -*- mode: qore; indent-tabs-mode: nil -*-

/** FtpClientStatResponseDataType.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! contains all public definitions in the FtpClientDataProvider module
public namespace FtpClientDataProvider {
#! Data type for FTP client stat response calls
public class FtpClientStatResponseDataType inherits HashDataType {
    public {
        #! Markdown descriptions for hashdecl members
        const FieldDescriptions = {
            "name": "The name of the remote file or directory",
            "path": "The full FTP path",
            "type": "The type of remote file; one of:\n- `REGULAR`\n- `DIRECTORY`\n- `UNKNOWN`",
            "mtime": "The last modified timestamp of the file",
            "size": "The size of the remote file in bytes",
        };
    }

    #! Creates the type
    constructor() {
        # add members for base hashdecl
        map addField(new QoreDataField($1.getName(), FieldDescriptions{$1.getName()}, $1.getType(),
            $1.getDefaultValue())), TypedHash::forName("Qore::StatInfo").getMembers();
    }
}
}
