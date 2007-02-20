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
   class QoreString *protocol, *path, *username, *password, *host;
   int port;

   DLLLOCAL void zero();
   DLLLOCAL void reset();
   DLLLOCAL void parseIntern(const char *url);

public:      
   DLLEXPORT QoreURL();
   DLLEXPORT QoreURL(const char *url);
   DLLEXPORT QoreURL(class QoreString *url);
   DLLEXPORT ~QoreURL();
   DLLEXPORT int parse(const char *url);
   DLLEXPORT int parse(class QoreString *url);
   DLLEXPORT bool isValid() const;
   // returns a hash of the parameters parsed - destructive: zeros out all elements
   DLLEXPORT class Hash *getHash();
      // returns the QoreString without zeroing it out in the class
   DLLEXPORT class QoreString *getHost() const;
   DLLEXPORT class QoreString *getUserName() const;
   DLLEXPORT class QoreString *getPassword() const;
   DLLEXPORT class QoreString *getPath() const;
   DLLEXPORT class QoreString *getProtocol() const;
   DLLEXPORT int getPort() const;

   // the "take" methods return the char * pointer for the QoreString members
   // as "taken" from the QoreStrings (QoreStrings are left empty)
   DLLEXPORT char *take_path();
   DLLEXPORT char *take_username();
   DLLEXPORT char *take_password();
   DLLEXPORT char *take_host();
};

#endif
