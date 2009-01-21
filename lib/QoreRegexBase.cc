/*
 QoreRegexBase.cc
 
 regular expression substitution node definition
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
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

#include <qore/Qore.h>
#include <qore/intern/QoreRegexBase.h>

void QoreRegexBase::setCaseInsensitive()
{
   options |= PCRE_CASELESS;
}

void QoreRegexBase::setDotAll()
{
   options |= PCRE_DOTALL;
}

void QoreRegexBase::setExtended()
{
   options |= PCRE_EXTENDED;
}

void QoreRegexBase::setMultiline()
{
   options |= PCRE_MULTILINE;
}      
