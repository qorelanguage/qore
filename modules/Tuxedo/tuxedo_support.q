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

%requires tuxedo

namespace Tuxedo {

const MAXTIDENT = 30;  # Tuxedo limit on values passed into tpinit(), see atmi.h

# ---------------------------------------------------------------------
# Wrapper over class TuxedoAdapter (C++) designed for yet easier use.
#
class TuxedoAdapter2
{
# ------------------------------------------------------------------
# Connect server, optionally use the settings hash.
constructor() {
  $.adapter = new Tuxedo::TuxedoAdapter();
  if (elements $argv ==  0) {
    $.adapter.init();
    return;
  }

  if (elements $argv != 1) {
    throw "TuxedoAdapter2::constructor", "Only one optional parameter: the hash with Tuxedo settings.";
  }
  my $hash = $argv[0];
  if (type($hash) != Qore::Type::Hash) {
    throw "TuxedoAdapter2::constructor", "The Tuxedo settings parameters needs to be a hash.";
  }

  my $connection_flags = 0; # either set one individual flag after another or with settings 'flags'

  # process all settings
  foreach my $key in (keys $hash) {
  switch ($key) {
    # --------------------------------------
    case "TUXDIR" :
      my $dir = $hash.$key;  
      if (type($dir) != Qore::Type::String) {
        throw "TuxedoAdapter2::constructor", "Settings value \"TUXDIR\" needs to be a string (directory where Tuxedo is installed).";
      }
      # TBD an does_directory_exists() would be handy here
      $.adapter.setEnvironmentVariable("TUXDIR", $dir);
      break;
    # --------------------------------------    
    case "TUXCONFIG":
      my $file = $hash.$key;
      if (type($file) != Qore::Type::String) {
        throw "TuxedoAdapter2::constructor", "Settings \"TUXCONFIG\" needs to be a string (file with binary configuration).";
      }
      # TBD an does_file_exist() would be handy here
      $.adapter.setEnvironmentVariable("TUXCONFIG", $file);
      break;
    # --------------------------------------
   case "Username":
   case "username":
   case "UseName":
     my $username = $hash.$key;
     if (type($username) != Qore::Type::String) {
       throw "TuxedoAdapter2::constructor", "Settings \"username\" needs to be a string.";
     }
     if (length($username) > 0) {
       if (length($username) > Tuxedo::MAXTIDENT) {
         throw "TuxedoAdapter2::constructor", "Value of settings \"username\" exceeds allowed length limit.";
       }
       $.adapter.setUserName($username);
     }
     break;
    # --------------------------------------
    case "Password":
    case "password":
      my $password = $hash.$key;
      if (type($password) != Qore::Type::String) {
        throw "TuxedoAdapter2::constructor", "Settings \"password\" needs to be a string.";
      }
      if (length($password) > 0) {
        if (length($password) > Tuxedo::MAXTIDENT) {
          throw "TuxedoAdapter2::constructor", "Value of settings \"password\" exceeds allowed length limit.";
        }
        $.adapter.setPassword($password);
      }
      break;
    # --------------------------------------
    case "Clientname":
    case "clientname":
    case "ClientName":
      my $clientname = $hash.$key;
      if (type($clientname) != Qore::Type::String) {
       throw "TuxedoAdapter2::constructor", "Settings \"clientname\" needs to be a string.";
      }
      if (length($clientname) > 0) {
        if (length($clientname) > Tuxedo::MAXTIDENT) {
          throw "TuxedoAdapter2::constructor", "Value of settings \"clientname\" exceeds allowed length limit.";
        }
        $.adapter.setClientName($clientname);
      }
      break;
    # --------------------------------------
    case "Groupname":
    case "GroupName":
    case "groupname":
     my $groupname = $hash.$key;
      if (type($groupname) != Qore::Type::String) {
       throw "TuxedoAdapter2::constructor", "Settings \"groupname\" needs to be a string.";
      }
      if (length($groupname) > 0) {
        if (length($groupname) > Tuxedo::MAXTIDENT) {
          throw "TuxedoAdapter2::constructor", "Value of settings \"groupname\" exceeds allowed length limit.";
        }
        $.adapter.setGroupName($groupname);
      }
      break;    
    # --------------------------------------
    case "binary_data":
    case "binarydata":
    case "BinaryData":
    case "binaryData":
      my $binary_data = $hash.$key;
      if (type($binary_data) != Qore::Type::Binary) {
        throw "TuxedoAdapter2::constructor", "Settings \"binary_data\" needs to be a binary.";
      }
      $.adapter.setBinaryConnectionData($binary_data);
      break; 
    # --------------------------------------
    case "flags":
    case "Flags":
      my $flags = $hash.$key;
      if (type($flags) != Qore::Type::Int) {
        throw "TuxedoAdapter2::constructor", "Settings \"flags\" needs to be an integer.";
      }
      $connection_flags = $flags;
      break;
    # --------------------------------------
    case "TPU_SIG":
      my $tpu_sig = $hash.$key;
      if (type($tpu_sig) != Qore::Type::Boolean) {
        throw "TuxedoAdapter2::constructor", "Settings of flag \"TPU_SIG\" needs to be a boolean.";
      }
      if ($tpu_sig == True) {
        # only one flag of these could be set (see tpinit() docs)
        if (($connection_flags & Tuxedo::TPU_IGN) || ($connection_flags & Tuxedo::TPU_DIP) || ($connection_flags & Tuxedo::TPU_THREAD)) {
          throw "TuxedoAdapter2::constructor", "Settings value \"TPU_SIG\" collides with an existing flag.";
        }
        $connection_flags |= Tuxedo::TPU_SIG;
      }
      break;
    # --------------------------------------
    case "TPU_DIP":
      my $tpu_dip = $hash.$key;
      if (type($tpu_dip) != Qore::Type::Boolean) {
        throw "TuxedoAdapter2::constructor", "Settings of flag \"TPU_DIP\" needs to be a boolean.";
      }
      if ($tpu_dip == True) {
        # only one flag of these could be set (see tpinit() docs)
        if (($connection_flags & Tuxedo::TPU_IGN) || ($connection_flags & Tuxedo::TPU_SIG) || ($connection_flags & Tuxedo::TPU_THREAD)) {
          throw "TuxedoAdapter2::constructor", "Settings value \"TPU_DIP\" collides with an existing flag.";
        }
        $connection_flags |= Tuxedo::TPU_DIP;
      }
      break;
    # --------------------------------------
    case "TPU_IGN":
      my $tpu_ign = $hash.$key;
      if (type($tpu_ign) != Qore::Type::Boolean) {
        throw "TuxedoAdapter2::constructor", "Settings of flag \"TPU_IGN\" needs to be a boolean.";
      }
      if ($tpu_ign == True) {
        # only one flag of these could be set (see tpinit() docs)
        if (($connection_flags & Tuxedo::TPU_DIP) || ($connection_flags & Tuxedo::TPU_SIG) || ($connection_flags & Tuxedo::TPU_THREAD)) {
          throw "TuxedoAdapter2::constructor", "Settings value \"TPU_IGN\" collides with an existing flag.";
        }
        $connection_flags |= Tuxedo::TPU_DIP;
      }
      break;
    # --------------------------------------
    case "TPU_THREAD":
      my $tpu_thread = $hash.$key;
      if (type($tpu_thread) != Qore::Type::Boolean) {
        throw "TuxedoAdapter2::constructor", "Settings of flag \"TPU_THREAD\" needs to be a boolean.";
      }
      if ($tpu_thread == True) {
        # only one flag of these could be set (see tpinit() docs)
        if (($connection_flags & Tuxedo::TPU_DIP) || ($connection_flags & Tuxedo::TPU_SIG) || ($connection_flags & Tuxedo::TPU_IGN)) {
          throw "TuxedoAdapter2::constructor", "Settings value \"TPU_THREAD\" collides with an existing flag.";
        }
        $connection_flags |= Tuxedo::TPU_THREAD;
      }
      break;
    # --------------------------------------
    case "TPMULTICONTEXTS":
      my $tpumulticontexts = $hash.$key;
      if (type($tpumulticontexts) != Qore::Type::Boolean) {
        throw "TuxedoAdapter2::constructor", "Settings of flag \"TPUMULTICONTEXTS\" needs to be a boolean.";
      }
      if ($tpumulticontexts == True) {
        $connection_flags |= Tuxedo::TPMULTICONTEXTS;
      }
      break;
    # --------------------------------------
    case "TPSA_FASTPATH":
      my $tpsa_fastpath = $hash.$key;
      if (type($tpsa_fastpath) != Qore::Type::Boolean) {
        throw "TuxedoAdapter2::constructor", "Settings of flag \"TPSA_FASTPATH\" needs to be a boolean.";
      }
      if ($connection_flags & Tuxedo::TPSA_PROTECTED) {
        throw "TuxedoAdapter2::constructor", "Settings value \"TPSA_FASTPATH\" collides with an existing flag.";
      }
      if ($tpsa_fastpath == True) {
        $connection_flags |= Tuxedo::TPSA_FASTPATH;
      }
      break;
    # --------------------------------------
    case "TPSA_PROTECTED":
      my $tpsa_protected = $hash.$key;
      if (type($tpsa_protected) != Qore::Type::Boolean) {
        throw "TuxedoAdapter2::constructor", "Settings of flag \"TPSA_PROTECTED\" needs to be a boolean.";
      }
      if ($connection_flags & Tuxedo::TPSA_FASTPATH) {
        throw "TuxedoAdapter2::constructor", "Settings value \"TPSA_PROTECTED\" collides with an existing flag.";
      }
      if ($tpsa_protected == True) {
        $connection_flags |= Tuxedo::TPSA_PROTECTED;
      }
      break;
    # --------------------------------------
    case "WSENVFILE":
      my $wsenvfile = $hash.$key;
      if (type($wsenvfile) != Qore::Type::String) {
        throw "TuxedoAdapter2::constructor", "Settings \"WSENVFILE\" needs to be a string (filename).";
      }
      # TBD - check file existence
      $.adapter.setEnvironmentVariable("WSENVFILE", $wsenvfile);
      break;
    # --------------------------------------
    case "WSNADDR":
      my $wsnaddr = $hash.$key;
      if (type($wsnaddr) != Qore::Type::String) {
        throw "TuxedoAdapter2::constructor", "Settings \"WSNADDR\" needs to be a string (see tpinit() docs for syntax).";
      }
      $.adapter.setEnvironmentVariable("WSNADDR", $wsnaddr);
      break;
    # --------------------------------------
    case "WSFADDR":
      my $wsfaddr = $hash.$key;
      if (type($wsfaddr) != Qore::Type::String) {
        throw "TuxedoAdapter2::constructor", "Settings \"WSFADDR\" needs to be a string.";
      }
      $.adapter.setEnvironmentVariable("WSFADDR", $wsfaddr);
      break;
    # --------------------------------------
    case "WSFRANGE":
      my $wsfrange = $hash.$key;
      if (type($wsfaddr) != Qore::Type::Int) {
        throw "TuxedoAdapter2::constructor", "Settings \"WSFRANGE\" needs to be an integer 1..65535.";
      }
      if ($wsfrange < 1 || $wsfrange > 65535) {
        throw "TuxedoAdapter2::constructor", "Settings \"WSFRANGE\" needs to be an integer 1..65535.";
      }
      $.adapter.setEnvironmentVariable("WSFRANGE", f_sprintf("%d", $wsfrange));
      break;
    # --------------------------------------
    case "WSDEVICE":
      my $wsdevice = $hash.$key;
      if (type($wsdevice) != Qore::Type::String) {
        throw "TuxedoAdapter2::constructor", "Settings \"WSDEVICE\" needs to be a string.";
      }
      $.adapter.setEnvironmentVariable("WSDEVICE", $wsdevice);
      break;
    # --------------------------------------
    case "WSTYPE":
      my $wstype = $hash.$key;
      if (type($wstype) != Qore::Type::String) {
        throw "TuxedoAdapter2::constructor", "Settings \"WSTYPE\" needs to be a string.";
      }
      $.adapter.setEnvironmentVariable("WSTYPE", $wstype);
      break;
    # --------------------------------------
    case "WSRPLYMAX":
      my $wsrplymax = $hash.$key;
      if (type($wsrplymax) != Qore::Type::Int) {
        throw "TuxedoAdapter2::constructor", "Settings \"WSRPLYMAX\" needs to be an integer.";
      }
      $.adapter.setEnvironmentVariable("WSRPLYMAX", f_sprintf("%d", $wsrplymax));
      break;
    # --------------------------------------
    case "TPMINENCRYPTBITS":
      my $tpminencryptbits = $hash.$key;
      if (type($tpminencryptbits) != Qore::Type::Int) {
        throw "TuxedoAdapter2::constructor", "Settings \"TPMINENCRYPTBITS\" needs to be an integer.";
      }
      if ($tpminencryptbits != 0 && $tpminencryptbits != 40 && $tpminencryptbits != 56 && $tpminencryptbits != 128) {
        throw "TuxedoAdapter2::constructor", "Settings \"TPMINENCRYPTBITS\" could be integer 0, 40, 56 or 128.";
      }
      $.adapter.setEnvironmentVariable("TPMINENCRYPTBITS", f_sprintf("%d", $tpminencryptbits));
      break;
    # --------------------------------------
    case "TPMAXENCRYPTBITS":
      my $tpmaxencryptbits = $hash.$key;
      if (type($tpmaxencryptbits) != Qore::Type::Int) {
        throw "TuxedoAdapter2::constructor", "Settings \"TPMAXENCRYPTBITS\" needs to be an integer.";
      }
      if ($tpmaxencryptbits != 0 && $tpmaxencryptbits != 40 && $tpmaxencryptbits != 56 && $tpmaxencryptbits != 128) {
        throw "TuxedoAdapter2::constructor", "Settings \"TPMAXENCRYPTBITS\" could be integer 0, 40, 56 or 128.";
      }
      $.adapter.setEnvironmentVariable("TPMAXENCRYPTBITS", f_sprintf("%d", $tpmaxencryptbits));
      break;
    # --------------------------------------
    default: 
      throw "TuxedoAdapter2::constructor", f_sprintf("Settings value [ %s ] is not recognized.", $key);
    } # switch
  } # foreach

  if ($connection_flags != 0) {
    $.adapter.setConnectionFlags($connection_flags);
  }
}

# -----------------------------------------------------------------
destructor() {
  $.adapter.close();
}

} # class Adapter
} # namespace Tuxedo

# for testing
printf("Started\n");
our $h = (
  "TUXDIR" : "/opt/bea/tuxedo9.1",
  "TUXCONFIG" : "/home/pavel/tuxedo_tests/tuxedo_simple_app/tuxconfig" 
);
our $a = new Tuxedo::TuxedoAdapter2($h);
printf("Done\n");

# EOF

