/*
  QoreFile.h

  File object

  Qore Programming Language

  Copyright (C) 2005 David Nichols
  
  FIXME: normalize methods to raise exceptions when file operations are attempted on a closed file

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

#ifndef _QORE_QOREFILE_H

#define _QORE_QOREFILE_H

#include <qore/common.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

extern class QoreEncoding *QCS_DEFAULT;

class QoreFile {
   private:
      int fd;
      bool is_open;
      bool special_file;
      class QoreEncoding *charset;
      char *filename;

      DLLLOCAL int readChar();
      // reads a buffer of the given size
      DLLLOCAL char *readBlock(int &size);
      // returns -1 for error
      DLLLOCAL int check_read_open(class ExceptionSink *xsink);
      // returns -1 for error
      DLLLOCAL int check_write_open(class ExceptionSink *xsink);
      
   public:
      DLLEXPORT QoreFile(class QoreEncoding *cs = QCS_DEFAULT);
      DLLEXPORT ~QoreFile();
      // NOTE: QoreFile::makeSpecial() can only be called right after the constructor
      DLLEXPORT void makeSpecial(int sfd);
      DLLEXPORT int open(const char *fn, int flags = O_RDONLY, int mode = 0777, class QoreEncoding *cs = QCS_DEFAULT);      
      DLLEXPORT int close();
      DLLEXPORT void setEncoding(class QoreEncoding *cs);
      DLLEXPORT class QoreEncoding *getEncoding() const;
      DLLEXPORT int sync();
      DLLEXPORT class QoreString *readLine(class ExceptionSink *xsink);
      DLLEXPORT int write(class QoreString *str, class ExceptionSink *xsink);
      DLLEXPORT int write(class BinaryObject *b, class ExceptionSink *xsink);
      DLLEXPORT int writei1(char i, class ExceptionSink *xsink);
      DLLEXPORT int writei2(short i, class ExceptionSink *xsink);
      DLLEXPORT int writei4(int i, class ExceptionSink *xsink);
      DLLEXPORT int writei8(int64 i, class ExceptionSink *xsink);
      DLLEXPORT int writei2LSB(short i, class ExceptionSink *xsink);
      DLLEXPORT int writei4LSB(int i, class ExceptionSink *xsink);
      DLLEXPORT int writei8LSB(int64 i, class ExceptionSink *xsink);
      DLLEXPORT int readu1(unsigned char *val, class ExceptionSink *xsink);
      DLLEXPORT int readu2(unsigned short *val, class ExceptionSink *xsink);
      DLLEXPORT int readu4(unsigned int *val, class ExceptionSink *xsink);
      DLLEXPORT int readu2LSB(unsigned short *val, class ExceptionSink *xsink);
      DLLEXPORT int readu4LSB(unsigned int *val, class ExceptionSink *xsink);
      DLLEXPORT int readi1(char *val, class ExceptionSink *xsink);
      DLLEXPORT int readi2(short *val, class ExceptionSink *xsink);
      DLLEXPORT int readi4(int *val, class ExceptionSink *xsink);
      DLLEXPORT int readi8(int64 *val, class ExceptionSink *xsink);
      DLLEXPORT int readi2LSB(short *val, class ExceptionSink *xsink);
      DLLEXPORT int readi4LSB(int *val, class ExceptionSink *xsink);
      DLLEXPORT int readi8LSB(int64 *val, class ExceptionSink *xsink);
      DLLEXPORT class QoreString *read(int size, class ExceptionSink *xsink);
      DLLEXPORT class BinaryObject *readBinary(int size, class ExceptionSink *xsink);
      DLLEXPORT int setPos(int pos);
      DLLEXPORT int getPos();
      DLLEXPORT class QoreString *getchar();
      DLLEXPORT char *getFileName() const;
};

#endif  // _QORE_QOREFILE_H
