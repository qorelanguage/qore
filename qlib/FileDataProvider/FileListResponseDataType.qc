# -*- mode: qore; indent-tabs-mode: nil -*-

/** FileListResponseDataType.qc Copyright 2023 Qore Technologies, s.r.o.

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

#! contains all public definitions in the FileDataProvider module
public namespace FileDataProvider {
#! Data type for stat file response calls
public class FileListResponseDataType inherits HashDataType {
    public {
        #! Markdown descriptions for hashdecl members
        const FieldDescriptions = {
            "dev": "The device inode number the file is on",
            "inode": "The inode of the file",
            "mode": "The file protection mode; a bitfield of file permissions",
            "nlink": "The number of hard links to this file",
            "uid": "The UID of the owner of the file",
            "gid": "The GID of the owner of the file",
            "rdev": "The device type number",
            "size": "The size of the file in bytes",
            "atime": "The last accessed date/time of the file",
            "mtime": "The last modified date/time of the file",
            "ctime": "The created date/time of the file",
            "blksize": "Block size; is zero if the file is zero length or if the platform's internal `stat()` (2) "
                "function does not provide this info",
            "blocks": "Blocks allocated for the file; is zero if the file is zero length or if the platform's "
                "internal `stat()` (2) function does not provide this info",
            "type": "The type of file; one of:\n- `REGULAR`\n- `DIRECTORY`\n- `SYMBOLIC-LINK`\n"
                "- `BLOCK-DEVICE`\n- `CHARACTER-DEVICE`\n- `FIFO`\n- `SYMBOLIC-LINK`\n- `SOCKET`\n"
                "- `UNKNOWN`",
            "perm": "A string giving UNIX-style permissions for the file (ex: `-rwxr-xr-x`)",
            "name": "The name of the file, link, or directory",
            "link": "Symbolic link target (if present)",
        };
    }

    #! Creates the type
    constructor() {
        # add members for base hashdecl
        map addField(new QoreDataField($1.getName(), FieldDescriptions{$1.getName()}, $1.getType(),
            $1.getDefaultValue())), TypedHash::forName("Qore::DirStatInfo").getMembers();
    }
}
}
