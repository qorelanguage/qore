/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_TermIOS.h

  Qore Programming Language

  Copyright (C) 2003 - 2014 Qore Technologies

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

#ifndef _QORE_QC_TERMIOS_H
#define _QORE_QC_TERMIOS_H

DLLLOCAL extern QoreClass *QC_TERMIOS;
DLLEXPORT extern qore_classid_t CID_TERMIOS;
DLLLOCAL QoreClass *initTermIOSClass(QoreNamespace& ns);

#ifdef HAVE_TERMIOS_H

#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>

class QoreTermIOS : public AbstractPrivateData {
protected:
    struct termios ios;
    
    DLLLOCAL int check_offset(int64 offset, ExceptionSink *xsink) {
       if (offset < 0) {
	  xsink->raiseException("TERMIOS-CC-ERROR", "cc offset (%lld) is < 0", offset);
	  return -1;
       }
       
       if (offset > NCCS) {
	  xsink->raiseException("TERMIOS-CC-ERROR", "cc offset (%lld) is > NCCS (%d)", offset, NCCS);
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
	    xsink->raiseException("TERMIOS-GET-ERROR", q_strerror(errno));
	    return rc;
	}
	return 0;
    }

    DLLLOCAL int set(int fd, int action, ExceptionSink *xsink) {
	// WARNING!: tcsetattr returns 0 if any changes were made, not if all
	//           changes were made
	int rc = tcsetattr(fd, action, &ios);
	if (rc) {
	    xsink->raiseException("TERMIOS-SET-ERROR", q_strerror(errno));
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
	  xsink->raiseErrnoException("TERMIOS-GET-WINDOW-SIZE-ERROR", errno, "cannot open controlling terminal");
	  return -1;
       }

       if (ioctl(fd, TIOCGWINSZ, &ws)) {
          xsink->raiseErrnoException("TERMIOS-GET-WINDOW-SIZE-ERROR", errno, "error reading window size");

          if (close(fd))
             xsink->raiseErrnoException("TERMIOS-GET-WINDOW-SIZE-ERROR", errno, "error closing controlling terminal");

	  return -1;
       }

       if (close(fd)) {
          xsink->raiseErrnoException("TERMIOS-GET-WINDOW-SIZE-ERROR", errno, "error closing controlling terminal");
          return -1;
       }

       rows = ws.ws_row;
       columns = ws.ws_col;
       return 0;
    }
};

#endif // HAVE_TERMIOS_H

#endif // _QORE_QC_TERMIOS_H
