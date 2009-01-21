/*
 RMutex.cc
 
 recursive lock object
 
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
#include <qore/intern/RMutex.h>

extern pthread_mutexattr_t ma_recursive;

RMutex::RMutex()
{
   // create mutex and set recursive attribute
   pthread_mutex_init(&m, &ma_recursive);
}

RMutex::~RMutex()
{
   pthread_mutex_destroy(&m);
}

int RMutex::enter()
{
   return pthread_mutex_lock(&m);
}

int RMutex::tryEnter()
{
   return pthread_mutex_trylock(&m);
}

int RMutex::exit()
{
   return pthread_mutex_unlock(&m);
}
