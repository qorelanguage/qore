/*
 QoreURL.h
 
 Network functions and macros
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006 David Nichols
 
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
   DLLLOCAL void parseIntern(char *url);
   
public:      
   DLLLOCAL QoreURL();
   DLLLOCAL QoreURL(char *url);
   DLLLOCAL QoreURL(class QoreString *url);
   DLLLOCAL ~QoreURL();
   DLLLOCAL int parse(char *url);
   DLLLOCAL int parse(class QoreString *url);
   DLLLOCAL bool isValid() const;
   // returns a hash of the parameters parsed - destructive: zeros out all elements
   DLLLOCAL class Hash *getHash();
      // returns the QoreString without zeroing it out in the class
   DLLLOCAL class QoreString *getProtocol() const;
   DLLLOCAL int getPort() const;
   
   // the "take" methods return the char * pointer for the QoreString members
   // as "taken" from the QoreStrings (QoreStrings are left empty)
   DLLLOCAL char *take_path();
   DLLLOCAL char *take_username();
   DLLLOCAL char *take_password();
   DLLLOCAL char *take_host();
};

#endif