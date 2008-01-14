/*
 QoreURL.h
 
 Network functions and macros
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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

#ifndef _QORE_QOREURL_H

#define _QORE_QOREURL_H

class QoreURL {
   private:
      struct qore_url_private *priv;

      DLLLOCAL void zero();
      DLLLOCAL void reset();
      DLLLOCAL void parseIntern(const char *url);

   public:      
      DLLEXPORT QoreURL();
      DLLEXPORT QoreURL(const char *url);
      DLLEXPORT QoreURL(const class QoreString *url);
      DLLEXPORT ~QoreURL();
      DLLEXPORT int parse(const char *url);
      DLLEXPORT int parse(const class QoreString *url);
      DLLEXPORT bool isValid() const;
      // returns a hash of the parameters parsed - destructive: zeros out all elements
      DLLEXPORT class QoreHash *getHash();
      DLLEXPORT const class QoreString *getHost() const;
      DLLEXPORT const class QoreString *getUserName() const;
      DLLEXPORT const class QoreString *getPassword() const;
      DLLEXPORT const class QoreString *getPath() const;
      DLLEXPORT const class QoreString *getProtocol() const;
      DLLEXPORT int getPort() const;
      
      // the "take" methods return the char * pointers for the data
      // the caller owns the memory
      DLLEXPORT char *take_path();
      DLLEXPORT char *take_username();
      DLLEXPORT char *take_password();
      DLLEXPORT char *take_host();
};

#endif
