/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreTimeZoneManager.cpp

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/QoreTimeZoneManager.h>
#include <qore/intern/qore_date_private.h>

#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <glob.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#include <memory>

#define QB(x) ((x) ? "true" : "false")

QoreZoneInfo::QoreZoneInfo(QoreString &root, std::string &n_name, ExceptionSink *xsink) : AbstractQoreZoneInfo(n_name), first_pos(-1), valid(false), std_abbr(0) {
   //printd(0, "QoreZoneInfo::QoreZoneInfo() this=%p root=%s name=%s\n", this, root.getBuffer(), name.c_str());
   
   std::string fn = root.getBuffer();
   fn += "/" + name;

   QoreFile f;
   if (f.open2(xsink, fn.c_str()))
      return;

   // data buffer
   QoreString str;
   if (f.read(str, 4, xsink))
      return;

   if (strcmp("TZif", str.getBuffer())) {
      xsink->raiseException("TZINFO-ERROR", "%s: invalid file magic", fn.c_str());
      return;
   }

   // skip 16 reserved bytes
   if (f.setPos(20) != 20) {
      xsink->raiseException("TZINFO-ERROR", "failed to position file at tzinfo header: %s", strerror(errno));
      return;
   }

   // file header variables
   unsigned tzh_ttisgmtcnt,  // The number of UTC/local indicators stored in the file
      tzh_ttisstdcnt,        // The number of standard/wall indicators stored in the file
      tzh_leapcnt,           // The number of leap seconds for which data is stored in the file
      tzh_timecnt,           // The number of "QoreDSTTransition times" for which data is stored in the file
      tzh_typecnt,           // The number of "local time types" for which data is stored in the file (must not be zero)
      tzh_charcnt;           // The number of characters of "timezone abbreviation strings" stored in the file
   
   // read in header count variables
   if (f.readu4(&tzh_ttisgmtcnt, xsink))
      return;

   if (f.readu4(&tzh_ttisstdcnt, xsink))
      return;

   if (f.readu4(&tzh_leapcnt, xsink))
      return;

   if (f.readu4(&tzh_timecnt, xsink))
      return;

   if (f.readu4(&tzh_typecnt, xsink))
      return;

   if (f.readu4(&tzh_charcnt, xsink))
      return;

   //printd(0, "QoreZoneInfo::QoreZoneInfo() tzh_ttisgmtcnt=%d tzh_ttisstdcnt=%d tzh_leapcnt=%d tzh_timecnt=%d tzh_typecnt=%d tzh_charcnt=%d\n", tzh_ttisgmtcnt, tzh_ttisstdcnt, tzh_leapcnt, tzh_timecnt, tzh_typecnt, tzh_charcnt);

   if (tzh_ttisgmtcnt > tzh_typecnt) {
      xsink->raiseException("TZINFO-ERROR", "tzh_ttisgmtcnt (%d) > tzh_typecnt (%d)", tzh_ttisgmtcnt, tzh_typecnt);
      return;
   }

   QoreDSTTransitions.resize(tzh_timecnt);

   // read in QoreDSTTransition time values
   for (unsigned i = 0; i < tzh_timecnt; ++i) {
      if (f.readi4(&QoreDSTTransitions[i].time, xsink))
	 return;
      
      if (first_pos == -1 && QoreDSTTransitions[i].time >= 0)
         first_pos = i;
      //printf("trans_time[%d]: %u\n", i, QoreDSTTransitions[i].time);
   }

   // for QoreDSTTransition type pointers
   std::vector<unsigned char> trans_type;
   trans_type.resize(tzh_timecnt);

   // read in QoreDSTTransition type array
   for (unsigned i = 0; i < tzh_timecnt; ++i) {
      if (f.readu1(&trans_type[i], xsink))
	 return;
      if (trans_type[i] >= tzh_typecnt) {
	 xsink->raiseException("TZINFO-ERROR", "QoreDSTTransition type index %d (%d) is greater than tzh_typecnt (%d)", i, trans_type[i], tzh_typecnt);
	 return;
      }
      //printf("trans_type[%d]: %d\n", i, trans_type[i]);
   }

   // allocate QoreTransitionInfo array
   tti.resize(tzh_typecnt);

   // declare temporary abbreviation index array
   std::vector<unsigned char> ai;
   ai.reserve(tzh_typecnt);

   // read in QoreTransitionInfo data
   for (unsigned i = 0; i < tzh_typecnt; ++i) {
      if (f.readi4(&tti[i].gmtoff, xsink))
	 return;

      //printf("gmtoff=%d\n", tti[i].gmtoff);

      unsigned char c;
      if (f.readu1(&c, xsink))
	 return;

      tti[i].isdst = c;
      if (!has_dst && c)
         has_dst = true;

      if (f.readu1(&c, xsink))
	 return;

      ai.push_back(c);
   }

   // set QoreDSTTransition pointers
   for (unsigned i = 0; i < tzh_timecnt; ++i) {
      QoreDSTTransitions[i].trans = &tti[trans_type[i]];
   }

   // read in abbreviation list
   if (f.read(str, tzh_charcnt, xsink))
      return;

   // set abbreviations
   for (unsigned i = 0; i < tzh_typecnt; ++i) {
      tti[i].abbr = str.getBuffer() + ai[i];
      if (!std_abbr && !tti[i].isdst)
         std_abbr = tti[i].abbr.c_str();
   }

   // read in leap info
   leapinfo.resize(tzh_leapcnt);
   for (unsigned i = 0; i < tzh_leapcnt; ++i) {
      if (f.readi4(&leapinfo[i].ttime, xsink) 
	  || f.readi4(&leapinfo[i].total, xsink))
	 return;
   }

   // read in std indicator array
   for (unsigned i = 0; i < tzh_ttisstdcnt; ++i) {
      unsigned char c;
      if (f.readu1(&c, xsink))
	 return;

      tti[i].isstd = c;
   }

   // assign remaining entries to default false
   for (unsigned i = tzh_ttisstdcnt; i < tzh_typecnt; ++i)
      tti[i].isstd = false;

   // read in gmt indicator array
   for (unsigned i = 0; i < tzh_ttisgmtcnt; ++i) {
      unsigned char c;
      if (f.readu1(&c, xsink))
	 return;

      tti[i].isgmt = c;
   }

   // assign remaining entries to default false
   for (unsigned i = tzh_ttisgmtcnt; i < tzh_typecnt; ++i)
      tti[i].isgmt = false;

   for (unsigned i = 0; i < tzh_typecnt; ++i) {
      if (gmtoff == -1 && !tti[i].isdst)
         gmtoff = tti[i].gmtoff;
      //printd(0, "tti[%d] %s: gmtoff=%d isdst=%s isstd=%s isgmt=%s\n", i, tti[i].abbr.c_str(), tti[i].gmtoff, QB(tti[i].isdst), QB(tti[i].isstd), QB(tti[i].isgmt));
   }

/*
   for (unsigned i = 0; i < tzh_timecnt; ++i) {
      DateTime d((int64)QoreDSTTransitions[i].time);
      str.clear();
      d.format(str, "Dy Mon DD YYYY HH:mm:SS");
      QoreTransitionInfo &trans = *QoreDSTTransitions[i].trans; 
      DateTime local(d.getEpochSeconds() + trans.gmtoff);
      QoreString lstr;
      local.format(lstr, "Dy Mon DD YYYY HH:mm:SS");
      //printd(0, "trans[%3d] time=%d %s UTC = %s %s isdst=%d isstd=%d isgmt=%d gmtoff=%d\n", i, QoreDSTTransitions[i].time, str.getBuffer(), lstr.getBuffer(), trans.abbr.c_str(), trans.isdst, trans.isstd, trans.isgmt, trans.gmtoff);
   }
*/

   valid = true;
}

int QoreTimeZoneManager::process(const char *fn) {
   // see if it's a directory or a file
   struct stat sbuf;

   if (stat(fn, &sbuf)) {
      printd(1, "error: could not stat() %s: %s\n", fn, strerror(errno));
      return -1;
   }

   if ((sbuf.st_mode & S_IFMT) == S_IFDIR)
      return processDir(fn);

   std::string name = !strncmp(root.getBuffer(), fn, root.strlen()) ? fn + root.strlen() + 1 : fn;
   if (tzmap.find(name) != tzmap.end())
      return 0;

   ExceptionSink xsink;
   std::auto_ptr<QoreZoneInfo> tzi(new QoreZoneInfo(root, name, &xsink));
   if (!*(tzi.get())) {
      //xsink.handleExceptions();
      xsink.clear();
      //printd(1, "skipping %s/%s\n", root.getBuffer(), name.c_str());
      return -1;
   }

   //printd(5, "QoreTimeZoneManager::process() %s -> %p\n", name.c_str(), tzi.get());
   tzmap[name] = tzi.release();
   ++tzsize;

   return 0;
}

int QoreZoneInfo::getGMTOffsetImpl(int64 epoch_offset, bool &is_dst, const char *&zone_name) const {
   unsigned i = 0;
   if (epoch_offset >= 0) {
      i = first_pos - 1;
      if (i < 0)
         i = 0;
      while ((i + 1) < QoreDSTTransitions.size()) {
         if (QoreDSTTransitions[i].time <= epoch_offset && QoreDSTTransitions[i + 1].time > epoch_offset) {
            zone_name = QoreDSTTransitions[i].trans->abbr.c_str();
            is_dst = QoreDSTTransitions[i].trans->isdst;
            return QoreDSTTransitions[i].trans->gmtoff;
         }
         ++i;
      }
      // not found, time zone unknown
      is_dst = false;
      zone_name = std_abbr;
      return gmtoff;
   }

   i = first_pos + 1;
   while (i > 0) {
      if (QoreDSTTransitions[i].time > epoch_offset && QoreDSTTransitions[i - 1].time <= epoch_offset) {
         zone_name = QoreDSTTransitions[i].trans->abbr.c_str();
         is_dst = QoreDSTTransitions[i].trans->isdst;
         return QoreDSTTransitions[i].trans->gmtoff;
      }
      --i;
   }
   // not found, time zone unknown
   is_dst = false;
   zone_name = std_abbr;
   return gmtoff;
}

// format: S00[[:]00[[:]00]]
const QoreOffsetZoneInfo *QoreTimeZoneManager::findCreateOffsetZone(const char *offset) {
   unsigned len = strlen(offset);
   assert(len > 1);

   const char *p = offset + 1;
   assert(isdigit(*p));
   int secs = (*p - '0') * SECS_PER_HOUR * 10;
   ++p;
   assert(isdigit(*p));
   secs += (*p - '0') * SECS_PER_HOUR;
   ++p;
   if (*p) {
      if (*p == ':')
         ++p;
      assert(isdigit(*p));
      secs += (*p - '0') * SECS_PER_MINUTE * 10;
      ++p;
      assert(isdigit(*p));
      secs += (*p - '0') * SECS_PER_MINUTE;
      ++p;
      if (*p) {
         if (*p == ':')
            ++p;
         assert(isdigit(*p));
         secs += (*p - '0') * 10;
         ++p;
         assert(isdigit(*p));
         secs += *p - '0';
      }
   }

   //printd(5, "QoreTimeZoneManager::findCreateOffsetZone(%s) secs=%d\n", offset, secs);

   if (!secs)
      return 0;

   if (*offset == '-')
      secs = -secs;

   QoreAutoRWWriteLocker al(rwl_offset);
   tzomap_t::iterator i = tzomap.find(secs);
   if (i != tzomap.end())
      return i->second;

   QoreString tmp;
   concatOffset(secs, tmp);
   QoreOffsetZoneInfo *ozi = new QoreOffsetZoneInfo(tmp.getBuffer(), secs);
   tzomap[secs] = ozi;
   //printd(5, "QoreTimeZoneManager::findCreateOffsetZone(%s) secs=%d returning %p\n", offset, secs, ozi);
   return ozi;
}

const QoreOffsetZoneInfo *QoreTimeZoneManager::findCreateOffsetZone(int seconds_east) {
   if (!seconds_east)
      return 0;

   QoreAutoRWWriteLocker al(rwl_offset);
   tzomap_t::iterator i = tzomap.find(seconds_east);
   if (i != tzomap.end())
      return i->second;

   QoreString tmp;
   concatOffset(seconds_east, tmp);
   QoreOffsetZoneInfo *ozi = new QoreOffsetZoneInfo(tmp.getBuffer(), seconds_east);
   tzomap[seconds_east] = ozi;
   return ozi;
}

int QoreTimeZoneManager::processDir(const char *d) {
   std::string dir = d;
   dir += "/*";

   // glob buffer
   glob_t globbuf;
   // free glob buffer on exit
   ON_BLOCK_EXIT(globfree, &globbuf);

   if (glob(dir.c_str(), 0, 0, &globbuf))
      return -1;

   for (unsigned i = 0; i < globbuf.gl_pathc; ++i)
      process(globbuf.gl_pathv[i]);

   return 0;
}

// to set the local time zone information from a file 
int QoreTimeZoneManager::setLocalTZ(std::string fname) {
   if (fname.empty())
      return -1;

   ExceptionSink xsink;
   QoreString dummy;

   if (fname[0] != '/')
      dummy = root;
   else if (!strncmp(root.getBuffer(), fname.c_str(), root.strlen())) {
      fname = fname.c_str() + root.strlen() + 1;
      if (fname.empty())
	 return -1;
      dummy = root;
   }

   std::auto_ptr<QoreZoneInfo> tzi(new QoreZoneInfo(dummy, fname, &xsink));
   if (!*(tzi.get())) {
      //xsink.handleExceptions();
      xsink.clear();
      printd(1, "cannot read in localtime file %s%s%s\n", dummy.getBuffer(), dummy.strlen() ? "/" : "", fname.c_str());
      return -1;
   }

   localtz = tzi.release();
   tzmap[fname] = localtz;
   localtzname = fname;
   ++tzsize;

   printd(1, "QoreTimeZoneManager::setLocalTZ() set zoneinfo from region: %s\n", fname.c_str());

   return 0;
}

QoreTimeZoneManager::QoreTimeZoneManager() : tzsize(0), our_gmtoffset(0), root(ZONEINFO_LOCATION), localtz(0) {
   // remove trailing "/" characters from root
   root.trim_trailing('/');
}

void QoreTimeZoneManager::init_intern(QoreString &TZ) {
   if (SysEnv.get("TZ", TZ)) {
      setFromLocalTimeFile();
      return;
   }
   
   if (!TZ.strlen())
      return;

   if (TZ.getBuffer()[0] == ':') {
      TZ.trim_single_leading(':');
      setLocalTZ(TZ.getBuffer());
      return;
   }

   if (setLocalTZ(TZ.getBuffer())) {
      // try to interpret as time zone rule specification
      printd(0, "QoreTimeZoneManager::init_intern(): cannot find zone: %s\n", TZ.getBuffer());
   }
}

void QoreTimeZoneManager::init() {
   QoreString TZ;

   init_intern(TZ);
   // if no local time zone has been set, then set to UTC
   if (!localtz)
      setLocalTZ("UTC");
}

void QoreTimeZoneManager::setFromLocalTimeFile() {
   // determine local region
   struct stat sbuf;
   if (!lstat(LOCALTIME_LOCATION, &sbuf)) {
      // normally this file is a symlink
      //printd(0, "QoreTimeZoneManager::QoreTimeZoneManager() %s: %d (%d)\n", LOCALTIME_LOCATION, sbuf.st_mode & S_IFMT, S_IFLNK);
      if ((sbuf.st_mode & S_IFMT) == S_IFLNK) {
	 char buf[QORE_PATH_MAX + 1];
	 qore_offset_t len = readlink(LOCALTIME_LOCATION, buf, QORE_PATH_MAX);
	 if (len > 0) {
	    buf[len] = '\0';
	    setLocalTZ(buf);
	 }
#ifdef DEBUG
	 else
	    printd(1, "QoreTimeZoneManager::QoreTimeZoneManager() failed to read %s link: %s\n", LOCALTIME_LOCATION, strerror(errno));
#endif
      }
      else
	 setLocalTZ(LOCALTIME_LOCATION);
   }
#ifdef DEBUG
   else {
      printd(1, "cannot determine local time region: could not lstat() %s: %s\n", LOCALTIME_LOCATION, strerror(errno));
   }
#endif
}

int QoreTimeZoneManager::readAll(ExceptionSink *xsink) {
   if (processDir(root.getBuffer())) {
      printd(1, "no time zone information available; glob(%s) failed: %s", root.getBuffer(), strerror(errno));
      return -1;
   }

   printd(1, "QoreTimeZoneManager::QoreTimeZoneManager() %d regions cached\n", tzsize);

   return 0;
}
