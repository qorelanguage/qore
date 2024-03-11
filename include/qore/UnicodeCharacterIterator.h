/*
    UnicodeCharacterIterator.h

    Qore Programming Language

    Copyright (C) 2003 - 2024 Qore Technologies, s.r.o.

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

#ifndef _QORE_UNICODECHARACTERITERATOR_H
#define _QORE_UNICODECHARACTERITERATOR_H

class UnicodeCharacterIterator {
public:
    //! creates the object
    DLLEXPORT UnicodeCharacterIterator(const QoreString& str);

    //! destroys the object
    DLLEXPORT ~UnicodeCharacterIterator();

    //! returns true if there are more characters to iterate
    DLLEXPORT bool next(ExceptionSink* xsink);

    //! returns the current character code or -1 if the iterator is invalid
    DLLEXPORT int getValue() const;

    //! returns true if the iterator is valid
    DLLEXPORT bool valid() const;

protected:
    const QoreString& str;
    int current_code = -1;
    size_t byte_pos = 0;

private:
    UnicodeCharacterIterator(const UnicodeCharacterIterator&) = delete;
    UnicodeCharacterIterator& operator=(const UnicodeCharacterIterator&) = delete;
};

#endif