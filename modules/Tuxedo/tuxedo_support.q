#!/usr/bin/qore
#
#  modules/Tuxedo/tuxedo_support.q
#
#  Tuxedo integration to QORE
#
#  Qore Programming Language
#
#  Copyright (C) 2006 Qore Technologies
#
#   This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

%requires Tuxedo

namespace Tuxedo {

# --------------------------------------------------------------------
# Convert Tuxedo error (the tperrno value) into description
sub error2text($err)
{
  my $text;

  # add possibly missing error codes
  switch ($res) {
  case Tuxedo::TPEINVAL:
    $text = "TPEINVAL - invalid arguments";

  case Tuxedo::TPENOENT:
    $text = "TPENOEXT - Tuxedo call specific error";

  case Tuxedo:TPEPERM:
    $text = "TPEPERM - permission denied";

  case Tuxedo::TPEPROTO:
    $text = "TPEPROTO - called improperly";

  case Tuxedo::TPESYSTEM:
    $text = "TPESYSTEM - Tuxedo error (see Tuxedo logs for details)";

  case Tuxedo::TPEOS:
    $text = f_sprintf("TPEOS - system error (errno = %d)", errno());

  default:
    $text = f_sprintf("unknown Tuxedo error code %d.", $err);    
  }  

  return $text
}

# ---------------------------------------------------------------------
# Class representing connection to a Tuxedo server.
#
class Connection
{
  # ------------------------------------------------------------------
  # Connect server. Uses environment variables, no authentication
  constructor() {
    my $res = tpinit();
    if ($res == 0) {
      return;
    }
    my $tuxdir = getenv("TUXDIR");
    my $tuxconfig = getenv("TUXCONFIG");
    my $err = error2text($res);
      
    my $desc = f_sprintf("Connection to Tuxedo server by tpinit(0) specified by environment variables "
      "TUXDIR = %s and TUXCONFIG = %s failed with error %s. "
      "Please verify that the server is running and all the settings are correct.",
      $tuxdir, $tuxconfig, $err);
    
    throw ("Connection to Tuxedo failed.", $desc);
  }

  # -----------------------------------------------------------------
  destructor() {
    my $res = tpterm();
    if ($res == 0) {
      return;
    }
    my $err = error2text($res);
    my $desc = f_sprintf("Disconnect from Tuxedo server by tpterm() failed with error %s. "
      "The server may be down.", $err);

    throw ("Disconnect from Tuxedo failed.", $desc);
  }

} # class Connection

} # namespace Tuxedo

printf("Started\n");
$a = new Tuxedo::Connection();
printf("Done\n");

# EOF

