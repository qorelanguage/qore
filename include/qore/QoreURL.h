/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreURL.h

  Network functions and macros

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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

#ifndef _QORE_QOREURL_H

#define _QORE_QOREURL_H

//! helps with parsing URLs and provides access to URL components through Qore data structures
class QoreURL {
private:
   //! private implementation of the class
   struct qore_url_private *priv;

   DLLLOCAL void zero();
   DLLLOCAL void reset();
   DLLLOCAL void parseIntern(const char* url, ExceptionSink* xsink);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreURL(const QoreURL&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL QoreURL& operator=(const QoreURL&);

public:
   //! creates an empty structure
   /** @see QoreURL::parse()
    */
   DLLEXPORT QoreURL();

   //! parses the URL string passed
   /** you can check if the URL was valid by calling QoreURL::isValid() after this call
       @param url the URL string to parse
    */
   DLLEXPORT QoreURL(const char* url);

   //! parses the URL string passed
   /** you can check if the URL was valid by calling QoreURL::isValid() after this call
       @param url the URL string to parse
    */
   DLLEXPORT QoreURL(const QoreString* url);

   //! parses the URL string passed
   /** you can check if the URL was valid by calling QoreURL::isValid() after this call
       @param url the URL string to parse
       @param keep_brackets if this argument is true then if the hostname or address is enclosed in square brackets, then the brackets will be included in the \c "host" key output as well
    */
   DLLEXPORT QoreURL(const char* url, bool keep_brackets);

   //! parses the URL string passed
   /** you can check if the URL was valid by calling QoreURL::isValid() after this call
       @param url the URL string to parse
       @param keep_brackets if this argument is true then if the hostname or address is enclosed in square brackets, then the brackets will be included in the \c "host" key output as well
    */
   DLLEXPORT QoreURL(const QoreString* url, bool keep_brackets);

   //! parses the URL string passed
   /**
       you can check if the URL was valid by calling QoreURL::isValid() after this call
       @param url the URL string to parse
       @param keep_brackets if this argument is true then if the hostname or address is enclosed in square brackets, then the brackets will be included in the \c "host" key output as well

       @note the input string will be converted to UTF-8 before parsing

       @since Qore 0.8.12.8
    */
   DLLEXPORT QoreURL(const QoreString* url, bool keep_brackets, ExceptionSink* xsink);

   //! frees all memory and destroys the structure
   DLLEXPORT ~QoreURL();

   //! parses the URL string passed
   /** If a url was already parsed previously, all memory is freed before parsing the new string.
       You can check if the URL was valid by calling QoreURL::isValid() after this call
       @param url the URL string to parse
    */
   DLLEXPORT int parse(const char* url);

   //! parses the URL string passed
   /** If a url was already parsed previously, all memory is freed before parsing the new string.
       You can check if the URL was valid by calling QoreURL::isValid() after this call
       @param url the URL string to parse
    */
   DLLEXPORT int parse(const QoreString* url);

   //! parses the URL string passed
   /** If a url was already parsed previously, all memory is freed before parsing the new string.
       You can check if the URL was valid by calling QoreURL::isValid() after this call

       @param url the URL string to parse
       @param keep_brackets if this argument is true then if the hostname or address is enclosed in square brackets, then the brackets will be included in the \c "host" key output as well

       @return 0 if the URL was parsed successfully, -1 if not
    */
   DLLEXPORT int parse(const char* url, bool keep_brackets);

   //! parses the URL string passed
   /** If a url was already parsed previously, all memory is freed before parsing the new string.
       You can check if the URL was valid by calling QoreURL::isValid() after this call

       @param url the URL string to parse
       @param keep_brackets if this argument is true then if the hostname or address is enclosed in square brackets, then the brackets will be included in the \c "host" key output as well

       @return 0 if the URL was parsed successfully, -1 if not
    */
   DLLEXPORT int parse(const QoreString* url, bool keep_brackets);

   //! parses the URL string passed
   /** If a url was already parsed previously, all memory is freed before parsing the new string.
       You can check if the URL was valid by calling QoreURL::isValid() after this call

       @param url the URL string to parse
       @param keep_brackets if this argument is true then if the hostname or address is enclosed in square brackets, then the brackets will be included in the \c "host" key output as well
       @param xsink for Qore-language exceptions

       @return 0 if the URL was parsed successfully, -1 if not

       @note the input string will be converted to UTF-8 before parsing

       @since Qore 0.8.12.8
    */
   DLLEXPORT int parse(const QoreString* url, bool keep_brackets, ExceptionSink* xsink);

   //! returns true if the URL string parsed is valid
   /** @return true if the URL string parsed is valid
    */
   DLLEXPORT bool isValid() const;

   //! returns a hash of the parameters parsed, destructive: zeros out all elements, caller owns the reference count returned
   /** hash keys are:
       - protocol
       - path
       - username
       - password
       - host
       - port
       .
       each key is either a QoreStringNode or 0 except for port which is a QoreBigIntNode
       @note the caller must call QoreHashNode::deref() manually on the value returned if it's not 0 (or use the ReferenceHolder helper class)
       @return a hash of the parameters parsed
    */
   DLLEXPORT QoreHashNode* getHash();

   //! returns the hostname of the URL
   /** @return the hostname of the URL
    */
   DLLEXPORT const QoreString* getHost() const;

   //! returns the user name in the URL or 0 if none given
   /** @return the user name in the URL or 0 if none given
    */
   DLLEXPORT const QoreString* getUserName() const;

   //! returns the password in the URL or 0 if none given
   /** @return the password in the URL or 0 if none given
    */
   DLLEXPORT const QoreString* getPassword() const;

   //! returns the path component of the URL or 0 if none given
   /** @return the path component of the URL or 0 if none given
    */
   DLLEXPORT const QoreString* getPath() const;

   //! returns the protocol component of the URL or 0 if none given
   DLLEXPORT const QoreString* getProtocol() const;

   //! returns the port number given in the URL or 0 if none present
   /** @return the port number given in the URL or 0 if none present
    */
   DLLEXPORT int getPort() const;

   // the "take" methods return the char*  pointers for the data
   // the caller owns the memory

   //! returns a pointer to the path (0 if none present), caller owns the memory returned
   /** if this function returns a non-zero pointer, the memory must be manually freed by the caller
       @return a pointer to the path (0 if none present), caller owns the memory returned
    */
   DLLEXPORT char* take_path();

   //! returns a pointer to the username in the URL (0 if none present), caller owns the memory returned
   /** if this function returns a non-zero pointer, the memory must be manually freed by the caller
       @return a pointer to the username (0 if none present), caller owns the memory returned
    */
   DLLEXPORT char* take_username();

   //! returns a pointer to the password in the URL (0 if none present), caller owns the memory returned
   /** if this function returns a non-zero pointer, the memory must be manually freed by the caller
       @return a pointer to the password (0 if none present), caller owns the memory returned
    */
   DLLEXPORT char* take_password();

   //! returns a pointer to the hostname in the URL (0 if none present), caller owns the memory returned
   /** if this function returns a non-zero pointer, the memory must be manually freed by the caller
       @return a pointer to the hostname (0 if none present), caller owns the memory returned
    */
   DLLEXPORT char* take_host();
};

#endif
