/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreFile.h

  thread-safe File object

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

#ifndef _QORE_QOREFILE_H

#define _QORE_QOREFILE_H

#include <fcntl.h>

#include <sys/file.h>
#include <unistd.h>

#include <string>

/*
  getchar from stdio.h(included via string on some platforms) 
  is allowed to be defined as a macro and this can cause
  problems here since the getchar mentioned in this file might
  get replaced, so we undefine it if it is defined.
  Undefining getchar is safe since getchar must be defined as 
  a function, so the function is used if the macro is not available.
 */
#ifdef getchar
#undef getchar
#endif

class QoreTermIOS;
class Queue;

//! provides controlled access to file data through Qore data structures
/** Each file has a default character encoding associated with it.  String data
    read from the file will be tagged with this encoding.  String data written to
    the file will be converted to this encoding if necessary before written.
    @see QoreEncoding
 */
class QoreFile {
protected:
   //! private implementation
   struct qore_qf_private *priv;

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreFile(const QoreFile&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreFile& operator=(const QoreFile&);
      
public:
   //! creates the object and sets the default encoding
   DLLEXPORT QoreFile(const QoreEncoding *cs = QCS_DEFAULT);

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
   DLLEXPORT int open(const char *fn, int flags = O_RDONLY, int mode = 0777, const QoreEncoding *cs = QCS_DEFAULT);      

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
   DLLEXPORT int open2(ExceptionSink *xsink, const char *fn, int flags = O_RDONLY, int mode = 0777, const QoreEncoding *cs = QCS_DEFAULT);      

   //! closes the file
   /**
      @return 0 for success, non-zero for error
   */
   DLLEXPORT int close();

   //! sets the encoding for the file
   DLLEXPORT void setEncoding(const QoreEncoding *cs);

   //! returns the encoding used for the file
   DLLEXPORT const QoreEncoding *getEncoding() const;

   //! flushes the write buffer to disk
   DLLEXPORT int sync();

   //! reads string data from the file up to and including the terminating EOL characters (can be \c "\n", \c "\r" or \c "\r\n") and returns the string read with the EOL characters
   /** if an error occurs (file is not open), a Qore-language exception is raised

       @param xsink if an error occurs when opening the file, the Qore-language exception info will be added here
       @return the string read including the terminating EOL characters (if any - if EOF is encountered then there may not be any) or 0 if there is an error or no data could be read.  caller owns the reference count returned

       @see QoreFile::readUntil()
   */
   DLLEXPORT QoreStringNode* readLine(ExceptionSink* xsink);

   //! reads string data from the file up to and optionally including the terminating EOL characters (can be '\n', '\r' or '\r\n') and returns the string read
   /** if an error occurs (file is not open), a Qore-language exception is raised

       @param incl_eol if this parameter is true, then the EOL character(s) read will be written to the string, if false, then they are not
       @param xsink if an error occurs when opening the file, the Qore-language exception info will be added here

       @return the string read including the terminating EOL characters (if any - if EOF is encountered then there may not be any) or 0 if there is an error or no data could be read.  caller owns the reference count returned

       @see QoreFile::readUntil()
   */
   DLLEXPORT QoreStringNode* readLine(bool incl_eol, ExceptionSink* xsink);

   //! clears the string passed, then reads string data from the file up to and including the terminating EOL characters (can be '\n', '\r' or '\r\n'), returns 0 for no error, -1 for EOF, -2 for file not opened
   /** @note string data will be appended to the string with the assumption that the string's encoding is the same as the file's

       @param str the string container that will have the line from the file written into

       @return 0 for no error, -1 for EOF, -2 for file not opened

       @see QoreFile::readUntil()
   */
   DLLEXPORT int readLine(QoreString &str);

   //! clears the string passed, then reads string data from the file up to and optionally including the terminating EOL characters (can be '\n', '\r' or '\r\n'), returns 0 for no error, -1 for EOF, -2 for file not opened
   /** @note string data will be appended to the string with the assumption that the string's encoding is the same as the file's

       @param str the string container that will have the line from the file written into
       @param incl_eol if this parameter is true, then the EOL character(s) read will be written to the string, if false, then they are not

       @return 0 for no error, -1 for EOF, -2 for file not opened

       @see QoreFile::readUntil()
   */
   DLLEXPORT int readLine(QoreString &str, bool incl_eol);

   //! reads string data from the file up to and optionally including the terminating EOL characters passed as an argument and returns the string read
   /** if an error occurs (file is not open), a Qore-language exception is raised

       @param bytes the end of line characters that separate lines
       @param incl_bytes if this parameter is true, then the EOL character(s) read will be written to the string, if false, then they are not
       @param xsink if an error occurs when opening the file, the Qore-language exception info will be added here

       @return the string read including the terminating EOL characters (if any - if EOF is encountered then there may not be any) or 0 if there is an error or no data could be read.  caller owns the reference count returned

       @see QoreFile::readLine()
   */
   DLLEXPORT QoreStringNode* readUntil(const char* bytes, bool incl_bytes, ExceptionSink* xsink);

   //! clears the string passed, then reads string data from the file up to a terminating byte value, returns 0 for no error, -1 for EOF, -2 for file not opened
   /** @note string data will be appended to the string with the assumption that the string's encoding is the same as the file's

       @param byte the end of line byte
       @param str the string to write the line read into
       @param incl_bytes if this parameter is true, then the EOL character(s) read will be written to the string, if false, then they are not

       @return 0 for no error, -1 for EOF, -2 for file not opened

       @see QoreFile::readLine()
   */
   DLLEXPORT int readUntil(char byte, QoreString& str, bool incl_bytes = true);

   //! clears the string passed, then reads string data from the file up to the given byte string, returns 0 for no error, -1 for EOF, -2 for file not opened
   /** @param bytes the byte string giving the end of the data to read
       @param str the string where the data read will be stored in (this string must already be tagged with the QoreFile's encoding)
       @param incl_bytes include "bytes" in the string

       @note string data will be appended to the string with the assumption that the string's encoding is the same as the file's
       @return 0 for no error, -1 for EOF, -2 for file not opened

       @see QoreFile::readLine()
   */
   DLLEXPORT int readUntil(const char* bytes, QoreString& str, bool incl_bytes = true);

   //! writes string data to the file, character encoding is converted if necessary, and returns the number of bytes written
   /** Qore-language exceptions can be thrown if the file is not opened or if encoding conversion fails
       @param str the string to write to the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return returns the number of bytes written (-1 if an exception was thrown)
   */
   DLLEXPORT int write(const QoreString *str, ExceptionSink *xsink);

   //! writes binary data to the file and returns the number of bytes written
   /** Qore-language exceptions can be thrown if the file is not opened
       @param b the binary data to write the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return returns the number of bytes written (-1 if an exception was thrown)
   */
   DLLEXPORT int write(const BinaryNode *b, ExceptionSink *xsink);

   //! writes binary data to the file and returns the number of bytes written
   /** Qore-language exceptions can be thrown if the file is not opened
       @param data the data to write to the file
       @param len the length of data to write to the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return returns the number of bytes written (-1 if an exception was thrown)
   */
   DLLEXPORT int write(const void *data, qore_size_t len, ExceptionSink *xsink);

   //! writes 1-byte binary integer data to the file and returns the number of bytes written (normally 1)
   /** Qore-language exceptions can be thrown if the file is not opened
       @param i the 1-byte integer to write to the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return returns the number of bytes written (-1 if an exception was thrown)
   */
   DLLEXPORT int writei1(char i, ExceptionSink *xsink);

   //! writes 2-byte (16bit) binary integer data in MSB (Most Significant Byte first, big endian) format to the file and returns the number of bytes written (normally 2)
   /** Qore-language exceptions can be thrown if the file is not opened
       @param i the integer to write to the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return returns the number of bytes written (-1 if an exception was thrown)
       @see Qorefile::writei2LSB()
   */
   DLLEXPORT int writei2(short i, ExceptionSink *xsink);

   //! writes 4-byte (32bit) binary integer data in MSB (Most Significant Byte first, big endian) format to the file and returns the number of bytes written (normally 4)
   /** Qore-language exceptions can be thrown if the file is not opened
       @param i the integer to write to the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return returns the number of bytes written (-1 if an exception was thrown)
       @see Qorefile::writei4LSB()
   */
   DLLEXPORT int writei4(int i, ExceptionSink *xsink);

   //! writes 8-byte (64bit) binary integer data in MSB (Most Significant Byte first, big endian) format to the file and returns the number of bytes written (normally 8)
   /** Qore-language exceptions can be thrown if the file is not opened
       @param i the integer to write to the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return returns the number of bytes written (-1 if an exception was thrown)
       @see Qorefile::writei4LSB()
   */
   DLLEXPORT int writei8(int64 i, ExceptionSink *xsink);

   //! writes 2-byte (16bit) binary integer data in LSB (Least Significant Byte first, little endian) format to the file and returns the number of bytes written (normally 2)
   /** Qore-language exceptions can be thrown if the file is not opened
       @param i the integer to write to the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return returns the number of bytes written (-1 if an exception was thrown)
       @see Qorefile::writei2()
   */
   DLLEXPORT int writei2LSB(short i, ExceptionSink *xsink);

   //! writes 4-byte (32bit) binary integer data in LSB (Least Significant Byte first, little endian)format to the file and returns the number of bytes written (normally 4)
   /** Qore-language exceptions can be thrown if the file is not opened
       @param i the integer to write to the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return returns the number of bytes written (-1 if an exception was thrown)
       @see Qorefile::writei4()
   */
   DLLEXPORT int writei4LSB(int i, ExceptionSink *xsink);

   //! writes 8-byte (64bit) binary integer data in LSB (Least Significant Byte first, little endian) format to the file and returns the number of bytes written (normally 8)
   /** Qore-language exceptions can be thrown if the file is not opened
       @param i the integer to write to the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return returns the number of bytes written (-1 if an exception was thrown)
       @see Qorefile::writei4()
   */
   DLLEXPORT int writei8LSB(int64 i, ExceptionSink *xsink);

   //! reads a 1-byte unsigned integer from the file and returns the value read as an output parameter
   /** A Qore-language exception can be thrown if the file is not opened
       @param val output parameter: the integer value read from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK, -1 for error
       @see QoreFile::readi1()
   **/
   DLLEXPORT int readu1(unsigned char *val, ExceptionSink *xsink);

   //! reads a 2-byte unsigned integer from the file in MSB (Most Significant Byte first, big endian) format and returns the value read as an output parameter
   /** A Qore-language exception can be thrown if the file is not opened
       @param val output parameter: the integer value read from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK, -1 for error
       @see QoreFile::readi2LSB()
       @see QoreFile::readi2()
       @see QoreFile::readi2LSB()
   **/
   DLLEXPORT int readu2(unsigned short *val, ExceptionSink *xsink);

   //! reads a 4-byte unsigned integer from the file in MSB (Most Significant Byte first, big endian) format and returns the value read as an output parameter
   /** A Qore-language exception can be thrown if the file is not opened
       @param val output parameter: the integer value read from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK, -1 for error
       @see QoreFile::readu4LSB()
       @see QoreFile::readi4LSB()
       @see QoreFile::readi4()
   **/
   DLLEXPORT int readu4(unsigned int *val, ExceptionSink *xsink);

   //! reads a 2-byte unsigned integer from the file in LSB (Least Significant Byte first, big endian) format and returns the value read as an output parameter
   /** A Qore-language exception can be thrown if the file is not opened
       @param val output parameter: the integer value read from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK, -1 for error
       @see QoreFile::readu2()
       @see QoreFile::readi2()
       @see QoreFile::readi2LSB()
   **/
   DLLEXPORT int readu2LSB(unsigned short *val, ExceptionSink *xsink);

   //! reads a 4-byte unsigned integer from the file in LSB (Least Significant Byte first, big endian) format and returns the value read as an output parameter
   /** A Qore-language exception can be thrown if the file is not opened
       @param val output parameter: the integer value read from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK, -1 for error
       @see QoreFile::readu4()
       @see QoreFile::readi4LSB()
       @see QoreFile::readi4()
   **/
   DLLEXPORT int readu4LSB(unsigned int *val, ExceptionSink *xsink);

   //! reads a 1-byte signed integer from the file and returns the value read as an output parameter
   /** A Qore-language exception can be thrown if the file is not opened
       @param val output parameter: the integer value read from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK, -1 for error
       @see QoreFile::readi1()
   **/
   DLLEXPORT int readi1(char *val, ExceptionSink *xsink);

   //! reads a 2-byte signed integer from the file in MSB (Most Significant Byte first, big endian) format and returns the value read as an output parameter
   /** A Qore-language exception can be thrown if the file is not opened
       @param val output parameter: the integer value read from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK, -1 for error
       @see QoreFile::readi2LSB()
       @see QoreFile::readu2()
       @see QoreFile::readu2LSB()
   **/
   DLLEXPORT int readi2(short *val, ExceptionSink *xsink);

   //! reads a 4-byte signed integer from the file in MSB (Most Significant Byte first, big endian) format and returns the value read as an output parameter
   /** A Qore-language exception can be thrown if the file is not opened
       @param val output parameter: the integer value read from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK, -1 for error
       @see QoreFile::readi4LSB()
       @see QoreFile::readu4LSB()
       @see QoreFile::readu4()
   **/
   DLLEXPORT int readi4(int *val, ExceptionSink *xsink);

   //! reads an 8-byte signed integer from the file in MSB (Most Significant Byte first, big endian) format and returns the value read as an output parameter
   /** A Qore-language exception can be thrown if the file is not opened
       @param val output parameter: the integer value read from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK, -1 for error
       @see QoreFile::readi8LSB()
   **/
   DLLEXPORT int readi8(int64 *val, ExceptionSink *xsink);

   //! reads a 2-byte signed integer from the file in LSB (Least Significant Byte first, big endian) format and returns the value read as an output parameter
   /** A Qore-language exception can be thrown if the file is not opened
       @param val output parameter: the integer value read from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK, -1 for error
       @see QoreFile::readi2()
       @see QoreFile::readu2()
       @see QoreFile::readu2LSB()
   **/
   DLLEXPORT int readi2LSB(short *val, ExceptionSink *xsink);

   //! reads a 4-byte signed integer from the file in LSB (Least Significant Byte first, big endian) format and returns the value read as an output parameter
   /** A Qore-language exception can be thrown if the file is not opened
       @param val output parameter: the integer value read from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK, -1 for error
       @see QoreFile::readi4()
       @see QoreFile::readu4LSB()
       @see QoreFile::readu4()
   **/
   DLLEXPORT int readi4LSB(int *val, ExceptionSink *xsink);

   //! reads an 8-byte signed integer from the file in LSB (Least Significant Byte first, big endian) format and returns the value read as an output parameter
   /** A Qore-language exception can be thrown if the file is not opened
       @param val output parameter: the integer value read from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK, -1 for error
       @see QoreFile::readi8()
   **/
   DLLEXPORT int readi8LSB(int64 *val, ExceptionSink *xsink);

   //! reads string data from the file and returns the string read (caller owns the reference count returned)
   /** A Qore-language exception can be thrown if the file is not opened
       @param size the number of bytes to read from the file, use -1 to read all data from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return the string read (caller owns the reference count returned) or 0 if an error occured
       @note the string will be tagged with the file's default encoding
   */
   DLLEXPORT QoreStringNode *read(qore_offset_t size, ExceptionSink *xsink);

   //! reads string data from the file into a QoreString object (or subclass, such as QoreStringNode) and returns 0 for OK or non-zero for error
   /** A Qore-language exception can be thrown if the file is not opened
       @param str the data read will be placed here
       @param size the number of bytes to read from the file, use -1 to read all data from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK or non-zero for error
       @note the string will be tagged with the file's default encoding
   */
   DLLEXPORT int read(QoreString &str, qore_offset_t size, ExceptionSink *xsink);

   //! reads binary data from the file and returns the data read (caller owns the reference count returned)
   /** A Qore-language exception can be thrown if the file is not opened
       @param size the number of bytes to read from the file, use -1 to read all data from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return the binary data read (caller owns the reference count returned) or 0 if an error occured
   */
   DLLEXPORT BinaryNode *readBinary(qore_offset_t size, ExceptionSink *xsink);

   //! reads binary data from the file into a BinaryNode object and returns 0 for OK or non-zero for error
   /** A Qore-language exception can be thrown if the file is not opened
       @param b the data read will be placed here
       @param size the number of bytes to read from the file, use -1 to read all data from the file
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return 0 for OK or non-zero for error
   */
   DLLEXPORT int readBinary(BinaryNode &b, qore_offset_t size, ExceptionSink *xsink);

   //! reads string data from the file and returns the string read (caller owns the reference count returned)
   /** A Qore-language exception can be thrown if the file is not opened
       @param size the number of bytes to read from the file, use -1 to read all data from the file
       @param timeout_ms the maximum time to read a single block from the file; -1 = never timeout, 0 timeout immediately if no data is available
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return the string read (caller owns the reference count returned) or 0 if an error occured
       @note the string will be tagged with the file's default encoding
   */
   DLLEXPORT QoreStringNode *read(qore_offset_t size, int timeout_ms, ExceptionSink *xsink);

   //! reads binary data from the file and returns the data read (caller owns the reference count returned)
   /** A Qore-language exception can be thrown if the file is not opened
       @param size the number of bytes to read from the file, use -1 to read all data from the file
       @param timeout_ms the maximum time to read a single block from the file; -1 = never timeout, 0 timeout immediately if no data is available
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return the binary data read (caller owns the reference count returned) or 0 if an error occured
   */
   DLLEXPORT BinaryNode *readBinary(qore_offset_t size, int timeout_ms, ExceptionSink *xsink);

   //! sets the absolute file position to "pos"
   /** @param pos the file position in bytes to set (starting with byte position 0)
    */
   DLLEXPORT qore_size_t setPos(qore_size_t pos);

   //! returns the absolute byte position in the file
   /** @return the absolute byte position in the file
    * @deprecated: only the const variant will remain in the future
    */
   DLLEXPORT qore_size_t getPos();

   //! returns the absolute byte position in the file
   /** @return the absolute byte position in the file
    */
   DLLEXPORT qore_size_t getPos() const;

   //! reads a single byte from the file and returns it as a new string, caller owns the reference count returned
   /** note that this function is not guaranteed to read a character if the File's encoding is a multi-byte character encoding
       @return a new string consisting of the single character read from the file; can return 0 if an error occurs
   */
   DLLEXPORT QoreStringNode *getchar();

   //! reads a single character from the file and returns it as a new string, caller owns the reference count returned
   /** This function will return a single character in the File's encoding even for multi-byte character encodings.
       However, a Qore-language exception will be thrown if invalid character data is read from the file (only thrown for multi-byte encodings).
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return a new string consisting of the single character read from the file
   */
   DLLEXPORT QoreStringNode *getchar(ExceptionSink *xsink);

   //! returns the filename of the file being read as a std::string (the string is empty if no file name is set)
   DLLEXPORT std::string getFileNameStr() const;

   //! returns the filename of the file being read (NULL if no file name is set); caller owns the reference count returned
   DLLEXPORT QoreStringNode* getFileName() const;

#ifndef _Q_WINDOWS
   //! changes ownership of the file (if possible)
   DLLEXPORT int chown(uid_t owner, gid_t group, ExceptionSink *xsink);

   //! perform a file lock operation
   DLLEXPORT int lockBlocking(struct flock &fl, ExceptionSink *xsink);

   //! perform a file lock operation, does not block
   DLLEXPORT int lock(const struct flock &fl, ExceptionSink *xsink);

   //! get lock info operation, does not block
   DLLEXPORT int getLockInfo(struct flock &fl, ExceptionSink *xsink);
#endif

   //! returns true if data is available for the file descriptor
   /** @param timeout_ms the maximum time to read a single block from the file; -1 = never timeout, 0 timeout immediately if no data is available
       @param xsink if an error occurs, the Qore-language exception info will be added here
       @return true if data is available in the timeout period, false if not
   **/
   DLLEXPORT bool isDataAvailable(int timeout_ms, ExceptionSink *xsink) const;

   //! returns a QoreListNode with file status information
   /** @param xsink if an error occurs, the Qore-language exception info will be added here
       @return a QoreListNode with file status information
   **/
   DLLEXPORT QoreListNode *stat(ExceptionSink *xsink) const;

   //! returns a QoreHashNode with file status information
   /** @param xsink if an error occurs, the Qore-language exception info will be added here
       @return a QoreHashNode with file status information
   **/
   DLLEXPORT QoreHashNode *hstat(ExceptionSink *xsink) const;

   //! returns a QoreHashNode with filesystem status information
   /** @param xsink if an error occurs, the Qore-language exception info will be added here
       @return a QoreHashNode with filesystem status information
   **/
   DLLEXPORT QoreHashNode *statvfs(ExceptionSink *xsink) const;

#if 0
   //! preallocates storage
   DLLEXPORT int preallocate(fstore_t &fs, ExceptionSink *xsink);
#endif

   //! get file descriptor
   DLLEXPORT int getFD() const;

   //! returns true if the file is open, false if not
   DLLEXPORT bool isOpen() const;

   //! returns true if the file is a tty
   DLLEXPORT bool isTty() const;

   //! sets terminal attributes
   DLLLOCAL int setTerminalAttributes(int action, QoreTermIOS *ios, ExceptionSink *xsink) const;

   //! gets terminal attributes
   DLLLOCAL int getTerminalAttributes(QoreTermIOS *ios, ExceptionSink *xsink) const;

   // NOTE: QoreFile::makeSpecial() can only be called right after the constructor (private API)
   DLLLOCAL void makeSpecial(int sfd);

   //! sets the event queue (not part of the library's pubilc API), must be already referenced before call
   DLLLOCAL void setEventQueue(Queue *cbq, ExceptionSink *xsink);

   //! internal API, must be called before deleting the object if an event queue is set
   DLLLOCAL void cleanup(ExceptionSink *xsink);
};

class QoreFileHelper : QorePrivateObjectAccessHelper {
public:
   DLLEXPORT QoreFileHelper(QoreObject* obj, ExceptionSink* xsink);
   DLLEXPORT ~QoreFileHelper();
   DLLEXPORT QoreFile* operator*() const;
   DLLEXPORT QoreFile* operator->() const;
};

#endif  // _QORE_QOREFILE_H
