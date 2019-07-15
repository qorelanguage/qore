/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  FileInputStream.h

  Qore Programming Language

  Copyright (C) 2016 - 2018 Qore Technologies, s.r.o.

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

#ifndef _QORE_FILEINPUTSTREAM_H
#define _QORE_FILEINPUTSTREAM_H

#include <stdint.h>
#include "qore/InputStream.h"

/**
 * @brief Private data for the Qore::FileInputStream class.
 */
class FileInputStream : public InputStream {
public:
    DLLLOCAL FileInputStream(const QoreStringNode *fileName, int64 timeout, int flags, ExceptionSink *xsink) : timeout(timeout) {
        f.open2(xsink, fileName->getBuffer(), O_RDONLY | flags);
    }

    DLLLOCAL FileInputStream(int fd) : timeout(-1) {
        f.makeSpecial(fd);
    }

    DLLLOCAL const char *getName() override {
        return "FileInputStream";
    }

    DLLLOCAL int64 read(void *ptr, int64 limit, ExceptionSink *xsink) override {
        assert(limit > 0);
        return f.read(ptr, limit, timeout, xsink);
    }

    DLLLOCAL int64 peek(ExceptionSink *xsink) override {
        qore_size_t pos = f.getPos(); // Save initial position.
        unsigned char c;
        qore_size_t rc = f.read(&c, 1, -1, xsink);
        if (*xsink)
            return -2;
        if (rc == 0)
            return -1;
        f.setPos(pos); // Restore initial position.
        return c;
    }

    DLLLOCAL QoreFile& getFile() { return f; }

    DLLLOCAL int64 getTimeout() const { return timeout; }

private:
    QoreFile f;
    int64 timeout;
};

#endif // _QORE_FILEINPUTSTREAM_H
