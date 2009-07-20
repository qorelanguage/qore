/*
  QC_TermIOS.h

  Qore Programming Language

  Copyright (C) 2003 - 2009 Qore Technologies

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

#ifndef _QORE_QC_TERMIOS_H
#define _QORE_QC_TERMIOS_H

#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>

DLLEXPORT extern qore_classid_t CID_TERMIOS;
DLLLOCAL QoreClass *initTermIOSClass();

class QoreTermIOS : public AbstractPrivateData {
protected:
    struct termios ios;
    
    DLLLOCAL int check_offset(int64 offset, ExceptionSink *xsink) {
       if (offset < 0) {
	  xsink->raiseException("TERMIOS-SET-CC-ERROR", "cc offset (%lld) is < 0", offset);
	  return -1;
       }
       
       if (offset > NCCS) {
	  xsink->raiseException("TERMIOS-SET-CC-ERROR", "cc offset (%lld) is > NCCS (%d)", offset, NCCS);
	  return -1;
       }
       return 0;
    }

public:
    DLLLOCAL QoreTermIOS() {
    }

    DLLLOCAL QoreTermIOS(const QoreTermIOS &cp) {
	memcpy(&ios, &cp.ios, sizeof(struct termios));
    }

    DLLLOCAL bool is_equal(const QoreTermIOS *qtios) {
       return !memcmp(&ios, &qtios->ios, sizeof(struct termios));
    }

    DLLLOCAL int get(int fd, ExceptionSink *xsink) {
	int rc = tcgetattr(fd, &ios);
	if (rc) {
	    xsink->raiseException("TERMIOS-GET-ERROR", strerror(errno));
	    return rc;
	}
	return 0;
    }

    DLLLOCAL int set(int fd, int action, ExceptionSink *xsink) {
	// WARNING!: tcsetattr returns 0 if any changes were made, not if all
	//           changes were made
	int rc = tcsetattr(fd, action, &ios);
	if (rc) {
	    xsink->raiseException("TERMIOS-SET-ERROR", strerror(errno));
	    return rc;
	}
	return 0;
    }

    DLLLOCAL void set_lflag(tcflag_t val) {
	ios.c_lflag = val;
    }
    
    DLLLOCAL void set_cflag(tcflag_t val) {
	ios.c_cflag = val;
    }
    
    DLLLOCAL void set_oflag(tcflag_t val) {
	ios.c_oflag = val;
    }
    
    DLLLOCAL void set_iflag(tcflag_t val) {
	ios.c_iflag = val;
    }

    DLLLOCAL tcflag_t get_lflag() const {
	return ios.c_lflag;
    }

    DLLLOCAL tcflag_t get_cflag() const {
	return ios.c_cflag;
    }

    DLLLOCAL tcflag_t get_oflag() const {
	return ios.c_oflag;
    }

    DLLLOCAL tcflag_t get_iflag() const {
	return ios.c_iflag;
    }
    
    DLLLOCAL cc_t get_cc(int64 offset, ExceptionSink *xsink) {
       if (check_offset(offset, xsink))
	  return -1;

       return ios.c_cc[offset];
    }

    DLLLOCAL int set_cc(int64 offset, cc_t val, ExceptionSink *xsink) {
       if (check_offset(offset, xsink))
	  return -1;

       ios.c_cc[offset] = val;
       return 0;
    }

    DLLLOCAL static int getWindowSize(int &rows, int &columns, ExceptionSink *xsink) {
       struct winsize ws;
       
       int fd = open("/dev/tty", O_RDONLY);
       if (fd == -1) {
	  xsink->raiseException("TERMIOS-GET-WINDOW-SIZE-ERROR", "cannot open controlling terminal: %s", strerror(errno));
	  return -1;
       }

       if (ioctl(fd, TIOCGWINSZ, &ws)) {
	  xsink->raiseException("TERMIOS-GET-WINDOW-SIZE-ERROR", "error reading window size: %s", strerror(errno));
	  return -1;
       }

       rows = ws.ws_row;
       columns = ws.ws_col;
       return 0;
    }
};

#endif

// EOF


