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

#include <fcntl.h>

#include <sys/file.h>
#include <unistd.h>

//! provides controlled access to file data through Qore data structures
/** Each file has a default character encoding associated with it.  String data
    read from the file will be tagged with this encoding.  String data written to
    the file will be converted to this encoding if necessary before written.
    @see QoreEncoding
 */
class QoreFile {
   private:
      //! private implementation
      struct qore_qf_private *priv;

      DLLLOCAL int readChar();
      // reads a buffer of the given size
      DLLLOCAL char *readBlock(int &size);
      // returns -1 for error
      DLLLOCAL int check_read_open(class ExceptionSink *xsink);
      // returns -1 for error
      DLLLOCAL int check_write_open(class ExceptionSink *xsink);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreFile(const QoreFile&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreFile& operator=(const QoreFile&);
      
   public:
      //! creates the object and sets the default encoding
      DLLEXPORT QoreFile(const class QoreEncoding *cs = QCS_DEFAULT);

      //! closes the file and frees all memory allocated to the object
      DLLEXPORT ~QoreFile();

      //! opens the file and returns 0 for success, non-zero for error
      /**
	 @param fn the file name to open
	 @param flags the flags to use when opening the file
	 @param mode the mode mask to use when opening the file
	 @param cs the encoding to use for the file
	 @return 0 for success, non-zero for error
	 @note for a version that raises a Qore-language exception when an error occurs opening the file, see QoreFile::open2()
	 @see QoreFile::open2()
       */
      DLLEXPORT int open(const char *fn, int flags = O_RDONLY, int mode = 0777, const class QoreEncoding *cs = QCS_DEFAULT);      

      //! opens the file and raises a Qore-language exception if an error occurs
      /**
	 @param xsink if an error occurs when opening the file, the Qore-language exception info will be added here
	 @param fn the file name to open
	 @param flags the flags to use when opening the file
	 @param mode the mode mask to use when opening the file
	 @param cs the encoding to use for the file
	 @return 0 for success, non-zero for error, meaning that an exception was raised
	 @note for a version that does not raise a Qore exception when errors occur, see QoreFile::open2()
	 @see QoreFile::open()
       */
      DLLEXPORT int open2(class ExceptionSink *xsink, const char *fn, int flags = O_RDONLY, int mode = 0777, const class QoreEncoding *cs = QCS_DEFAULT);      

      //! closes the file
      /**
	 @return 0 for success, non-zero for error
       */
      DLLEXPORT int close();

      //! sets the encoding for the file
      DLLEXPORT void setEncoding(const class QoreEncoding *cs);

      //! returns the encoding used for the file
      DLLEXPORT const class QoreEncoding *getEncoding() const;

      //! flushes the write buffer to disk
      DLLEXPORT int sync();

      //! reads string data from the file up to a terminating '\\n' character and returns the string read
      /** if an error occurs (file is not open), a Qore-language exception is raised
	  @param xsink if an error occurs when opening the file, the Qore-language exception info will be added here
	  @return the string read including the terminating '\\n' (if any - if EOF is encountered then there may not be one) or 0 if there is an error or no data could be read.  caller owns the reference count returned
       */
      DLLEXPORT class QoreStringNode *readLine(class ExceptionSink *xsink);

      //! writes string data to the file, character encoding is converted if necessary, and returns the number of bytes written
      /** Qore-language exceptions can be thrown if the file is not opened or if encoding conversion fails
	  @param str the string to write to the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return returns the number of bytes written (-1 if an exception was thrown)
       */
      DLLEXPORT int write(const class QoreString *str, class ExceptionSink *xsink);

      //! writes binary data to the file and returns the number of bytes written
      /** Qore-language exceptions can be thrown if the file is not opened
	  @param b the binary data to write the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return returns the number of bytes written (-1 if an exception was thrown)
       */
      DLLEXPORT int write(const class BinaryNode *b, class ExceptionSink *xsink);

      //! writes binary data to the file and returns the number of bytes written
      /** Qore-language exceptions can be thrown if the file is not opened
	  @param data the data to write to the file
	  @param len the length of data to write to the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return returns the number of bytes written (-1 if an exception was thrown)
       */
      DLLEXPORT int write(const void *data, unsigned len, class ExceptionSink *xsink);

      //! writes 1-byte binary integer data to the file and returns the number of bytes written (normally 1)
      /** Qore-language exceptions can be thrown if the file is not opened
	  @param i the 1-byte integer to write to the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return returns the number of bytes written (-1 if an exception was thrown)
       */
      DLLEXPORT int writei1(char i, class ExceptionSink *xsink);

      //! writes 2-byte (16bit) binary integer data in MSB (Most Significant Byte first, big endian) format to the file and returns the number of bytes written (normally 2)
      /** Qore-language exceptions can be thrown if the file is not opened
	  @param i the integer to write to the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return returns the number of bytes written (-1 if an exception was thrown)
	  @see Qorefile::writei2LSB()
       */
      DLLEXPORT int writei2(short i, class ExceptionSink *xsink);

      //! writes 4-byte (32bit) binary integer data in MSB (Most Significant Byte first, big endian) format to the file and returns the number of bytes written (normally 4)
      /** Qore-language exceptions can be thrown if the file is not opened
	  @param i the integer to write to the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return returns the number of bytes written (-1 if an exception was thrown)
	  @see Qorefile::writei4LSB()
       */
      DLLEXPORT int writei4(int i, class ExceptionSink *xsink);

      //! writes 8-byte (64bit) binary integer data in MSB (Most Significant Byte first, big endian) format to the file and returns the number of bytes written (normally 8)
      /** Qore-language exceptions can be thrown if the file is not opened
	  @param i the integer to write to the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return returns the number of bytes written (-1 if an exception was thrown)
	  @see Qorefile::writei4LSB()
       */
      DLLEXPORT int writei8(int64 i, class ExceptionSink *xsink);

      //! writes 2-byte (16bit) binary integer data in LSB (Least Significant Byte first, little endian) format to the file and returns the number of bytes written (normally 2)
      /** Qore-language exceptions can be thrown if the file is not opened
	  @param i the integer to write to the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return returns the number of bytes written (-1 if an exception was thrown)
	  @see Qorefile::writei2()
       */
      DLLEXPORT int writei2LSB(short i, class ExceptionSink *xsink);

      //! writes 4-byte (32bit) binary integer data in LSB (Least Significant Byte first, little endian)format to the file and returns the number of bytes written (normally 4)
      /** Qore-language exceptions can be thrown if the file is not opened
	  @param i the integer to write to the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return returns the number of bytes written (-1 if an exception was thrown)
	  @see Qorefile::writei4()
       */
      DLLEXPORT int writei4LSB(int i, class ExceptionSink *xsink);

      //! writes 8-byte (64bit) binary integer data in LSB (Least Significant Byte first, little endian) format to the file and returns the number of bytes written (normally 8)
      /** Qore-language exceptions can be thrown if the file is not opened
	  @param i the integer to write to the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return returns the number of bytes written (-1 if an exception was thrown)
	  @see Qorefile::writei4()
       */
      DLLEXPORT int writei8LSB(int64 i, class ExceptionSink *xsink);

      //! reads a 1-byte unsigned integer from the file and returns the value read as an output parameter
      /** A Qore-language exception can be thrown if the file is not opened
	  @param val output parameter: the integer value read from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return 0 for OK, -1 for error
	  @see QoreFile::readi1()
       **/
      DLLEXPORT int readu1(unsigned char *val, class ExceptionSink *xsink);

      //! reads a 2-byte unsigned integer from the file in MSB (Most Significant Byte first, big endian) format and returns the value read as an output parameter
      /** A Qore-language exception can be thrown if the file is not opened
	  @param val output parameter: the integer value read from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return 0 for OK, -1 for error
	  @see QoreFile::readi2LSB()
	  @see QoreFile::readi2()
	  @see QoreFile::readi2LSB()
       **/
      DLLEXPORT int readu2(unsigned short *val, class ExceptionSink *xsink);

      //! reads a 4-byte unsigned integer from the file in MSB (Most Significant Byte first, big endian) format and returns the value read as an output parameter
      /** A Qore-language exception can be thrown if the file is not opened
	  @param val output parameter: the integer value read from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return 0 for OK, -1 for error
	  @see QoreFile::readu4LSB()
	  @see QoreFile::readi4LSB()
	  @see QoreFile::readi4()
       **/
      DLLEXPORT int readu4(unsigned int *val, class ExceptionSink *xsink);

      //! reads a 2-byte unsigned integer from the file in LSB (Least Significant Byte first, big endian) format and returns the value read as an output parameter
      /** A Qore-language exception can be thrown if the file is not opened
	  @param val output parameter: the integer value read from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return 0 for OK, -1 for error
	  @see QoreFile::readu2()
	  @see QoreFile::readi2()
	  @see QoreFile::readi2LSB()
       **/
      DLLEXPORT int readu2LSB(unsigned short *val, class ExceptionSink *xsink);

      //! reads a 4-byte unsigned integer from the file in LSB (Least Significant Byte first, big endian) format and returns the value read as an output parameter
      /** A Qore-language exception can be thrown if the file is not opened
	  @param val output parameter: the integer value read from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return 0 for OK, -1 for error
	  @see QoreFile::readu4()
	  @see QoreFile::readi4LSB()
	  @see QoreFile::readi4()
       **/
      DLLEXPORT int readu4LSB(unsigned int *val, class ExceptionSink *xsink);

      //! reads a 1-byte signed integer from the file and returns the value read as an output parameter
      /** A Qore-language exception can be thrown if the file is not opened
	  @param val output parameter: the integer value read from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return 0 for OK, -1 for error
	  @see QoreFile::readi1()
       **/
      DLLEXPORT int readi1(char *val, class ExceptionSink *xsink);

      //! reads a 2-byte signed integer from the file in MSB (Most Significant Byte first, big endian) format and returns the value read as an output parameter
      /** A Qore-language exception can be thrown if the file is not opened
	  @param val output parameter: the integer value read from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return 0 for OK, -1 for error
	  @see QoreFile::readi2LSB()
	  @see QoreFile::readu2()
	  @see QoreFile::readu2LSB()
       **/
      DLLEXPORT int readi2(short *val, class ExceptionSink *xsink);

      //! reads a 4-byte signed integer from the file in MSB (Most Significant Byte first, big endian) format and returns the value read as an output parameter
      /** A Qore-language exception can be thrown if the file is not opened
	  @param val output parameter: the integer value read from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return 0 for OK, -1 for error
	  @see QoreFile::readi4LSB()
	  @see QoreFile::readu4LSB()
	  @see QoreFile::readu4()
       **/
      DLLEXPORT int readi4(int *val, class ExceptionSink *xsink);

      //! reads an 8-byte signed integer from the file in MSB (Most Significant Byte first, big endian) format and returns the value read as an output parameter
      /** A Qore-language exception can be thrown if the file is not opened
	  @param val output parameter: the integer value read from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return 0 for OK, -1 for error
	  @see QoreFile::readi8LSB()
       **/
      DLLEXPORT int readi8(int64 *val, class ExceptionSink *xsink);

      //! reads a 2-byte signed integer from the file in LSB (Least Significant Byte first, big endian) format and returns the value read as an output parameter
      /** A Qore-language exception can be thrown if the file is not opened
	  @param val output parameter: the integer value read from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return 0 for OK, -1 for error
	  @see QoreFile::readi2()
	  @see QoreFile::readu2()
	  @see QoreFile::readu2LSB()
       **/
      DLLEXPORT int readi2LSB(short *val, class ExceptionSink *xsink);

      //! reads a 4-byte signed integer from the file in LSB (Least Significant Byte first, big endian) format and returns the value read as an output parameter
      /** A Qore-language exception can be thrown if the file is not opened
	  @param val output parameter: the integer value read from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return 0 for OK, -1 for error
	  @see QoreFile::readi4()
	  @see QoreFile::readu4LSB()
	  @see QoreFile::readu4()
       **/
      DLLEXPORT int readi4LSB(int *val, class ExceptionSink *xsink);

      //! reads an 8-byte signed integer from the file in LSB (Least Significant Byte first, big endian) format and returns the value read as an output parameter
      /** A Qore-language exception can be thrown if the file is not opened
	  @param val output parameter: the integer value read from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return 0 for OK, -1 for error
	  @see QoreFile::readi8()
       **/
      DLLEXPORT int readi8LSB(int64 *val, class ExceptionSink *xsink);

      //! reads string data from the file and returns the string read (caller owns the reference count returned)
      /** A Qore-language exception can be thrown if the file is not opened
	  @param size the number of bytes to read from the file, use -1 to read all data from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return the string read (caller owns the reference count returned) or 0 if an error occured
	  @note the string will be tagged with the file's default encoding
       */
      DLLEXPORT class QoreStringNode *read(int size, class ExceptionSink *xsink);

      //! reads binary data from the file and returns the data read (caller owns the reference count returned)
      /** A Qore-language exception can be thrown if the file is not opened
	  @param size the number of bytes to read from the file, use -1 to read all data from the file
	  @param xsink if an error occurs, the Qore-language exception info will be added here
	  @return the binary data read (caller owns the reference count returned) or 0 if an error occured
       */
      DLLEXPORT class BinaryNode *readBinary(int size, class ExceptionSink *xsink);

      //! sets the absolute file position to "pos"
      /** @param pos the file position in bytes to set (starting with byte position 0)
       */
      DLLEXPORT int setPos(int pos);

      //! returns the absolute byte position in the file
      /** @return the absolute byte position in the file
       */
      DLLEXPORT int getPos();

      //! reads a single character from the file and returns it as a new string, caller owns the reference count returned
      /** can returns 0 if an error occurs
	 FIXME: reads a single byte, not a character
	 @return a new string consisting of the single character read from the file
       */
      DLLEXPORT class QoreStringNode *getchar();

      //! returns the filename of the file being read
      DLLEXPORT const char *getFileName() const;

      //! changes ownership of the file (if possible)
      DLLEXPORT int chown(uid_t owner, gid_t group, ExceptionSink *xsink);

      //! perform a file lock operation
      DLLEXPORT int lockBlocking(struct flock &fl, ExceptionSink *xsink);

      //! perform a file lock operation, does not block
      DLLEXPORT int lock(const struct flock &fl, ExceptionSink *xsink);

      //! get lock info operation, does not block
      DLLEXPORT int getLockInfo(struct flock &fl, ExceptionSink *xsink);

      //! preallocates storage
      DLLEXPORT int preallocate(fstore_t &fs, ExceptionSink *xsink);

      // NOTE: QoreFile::makeSpecial() can only be called right after the constructor (private API)
      DLLLOCAL void makeSpecial(int sfd);
};

#endif  // _QORE_QOREFILE_H
