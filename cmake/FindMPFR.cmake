# Try to find the MPFR librairies
#  MPFR_FOUND - system has MPFR lib
#  MPFR_INCLUDE_DIRS - the MPFR include directories
#  MPFR_LIBRARIES - Libraries needed to use MPFR

# Copyright (c) 2012, Evan Teran <evan.teran@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products 
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



if (MPFR_INCLUDE_DIRS AND MPFR_LIBRARIES)
  # Already in cache, be silent
  set(MPFR_FIND_QUIETLY TRUE)
endif (MPFR_INCLUDE_DIRS AND MPFR_LIBRARIES)

find_path(MPFR_INCLUDE_DIR NAMES mpfr.h PATH_SUFFIXES mpfr)
find_path(MPFRGMP_INCLUDE_DIR NAMES gmp.h PATH_SUFFIXES gmp)
find_library(MPFR_LIBRARIES NAMES mpfr libmpfr)

if (MPFR_INCLUDE_DIR STREQUAL MPFRGMP_INCLUDE_DIR)
    set(MPFR_INCLUDE_DIRS ${MPFR_INCLUDE_DIR})
else ()
    set(MPFR_INCLUDE_DIRS ${MPFRGMP_INCLUDE_DIR} ${MPFR_INCLUDE_DIR})
endif ()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MPFR DEFAULT_MSG MPFR_INCLUDE_DIRS MPFR_LIBRARIES MPFRGMP_INCLUDE_DIR MPFR_INCLUDE_DIR)

mark_as_advanced(MPFR_INCLUDE_DIR MPFR_LIBRARIES MPFR_INCLUDE_DIRS MPFRGMP_INCLUDE_DIR)

