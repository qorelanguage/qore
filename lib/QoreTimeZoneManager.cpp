/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreTimeZoneManager.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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

#include <qore/Qore.h>
#include <qore/intern/QoreTimeZoneManager.h>
#include <qore/intern/qore_date_private.h>

#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>

#ifdef HAVE_GLOB_H
#include <glob.h>
#else
#include <qore/intern/glob.h>
#endif

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
   printd(5, "QoreZoneInfo::QoreZoneInfo() this=%p root=%s name=%s\n", this, root.getBuffer(), name.c_str());

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
      xsink->raiseErrnoException("TZINFO-ERROR", errno, "failed to position file at tzinfo header");
      return;
   }

   // file header variables
   unsigned tzh_ttisutccnt,  // The number of UTC/local indicators stored in the file
      tzh_ttisstdcnt,        // The number of standard/wall indicators stored in the file
      tzh_leapcnt,           // The number of leap seconds for which data is stored in the file
      tzh_timecnt,           // The number of "QoreDSTTransition times" for which data is stored in the file
      tzh_typecnt,           // The number of "local time types" for which data is stored in the file (must not be zero)
      tzh_charcnt;           // The number of characters of "timezone abbreviation strings" stored in the file

   // read in header count variables
   if (f.readu4(&tzh_ttisutccnt, xsink))
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

   printd(5, "QoreZoneInfo::QoreZoneInfo() tzh_ttisutccnt=%d tzh_ttisstdcnt=%d tzh_leapcnt=%d tzh_timecnt=%d tzh_typecnt=%d tzh_charcnt=%d\n", tzh_ttisutccnt, tzh_ttisstdcnt, tzh_leapcnt, tzh_timecnt, tzh_typecnt, tzh_charcnt);

   if (tzh_ttisutccnt > tzh_typecnt) {
      xsink->raiseException("TZINFO-ERROR", "tzh_ttisutccnt (%d) > tzh_typecnt (%d)", tzh_ttisutccnt, tzh_typecnt);
      return;
   }

   QoreDSTTransitions.resize(tzh_timecnt);

   // read in QoreDSTTransition time values
   for (unsigned i = 0; i < tzh_timecnt; ++i) {
      if (f.readi4(&QoreDSTTransitions[i].time, xsink))
	 return;

      if (first_pos == -1 && QoreDSTTransitions[i].time >= 0)
         first_pos = i;
      //printd(5, "QoreZoneInfo::QoreZoneInfo() trans_time[%d]: %u\n", i, QoreDSTTransitions[i].time);
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
      //printd(5, "QoreZoneInfo::QoreZoneInfo() trans_type[%d]: %d\n", i, trans_type[i]);
   }

   // allocate QoreTransitionInfo array
   tti.resize(tzh_typecnt);

   // declare temporary abbreviation index array
   std::vector<unsigned char> ai;
   ai.reserve(tzh_typecnt);

   // read in QoreTransitionInfo data
   for (unsigned i = 0; i < tzh_typecnt; ++i) {
      if (f.readi4(&tti[i].utcoff, xsink))
	 return;

      //printd(5, "QoreZoneInfo::QoreZoneInfo() utcoff=%d\n", tti[i].utcoff);

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

   // set QoreDSTTransition pointers and remove invalid bands
   {
      dst_transition_vec_t::iterator di = QoreDSTTransitions.begin();
      for (unsigned i = 0; i < tzh_timecnt; ++i) {
         QoreDSTTransition& t = *di;
         t.trans = &tti[trans_type[i]];
         if (di != QoreDSTTransitions.begin()) {
            dst_transition_vec_t::iterator prev = di;
            --prev;
            if (t.trans->utcoff == prev->trans->utcoff) {
               // invalid transition found
               printd(1, "QoreZoneInfo::QoreZoneInfo() skipping invalid transition [%d] at %d\n", i, t.time);
               QoreDSTTransitions.erase(di);
               di = prev;
               if (i < first_pos)
                 --first_pos;
            }
         }
         ++di;
      }
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

   // read in utc indicator array
   for (unsigned i = 0; i < tzh_ttisutccnt; ++i) {
      unsigned char c;
      if (f.readu1(&c, xsink))
	 return;

      tti[i].isutc = c;
   }

   // assign remaining entries to default false
   for (unsigned i = tzh_ttisutccnt; i < tzh_typecnt; ++i)
      tti[i].isutc = false;

   for (unsigned i = 0; i < tzh_typecnt; ++i) {
      if (utcoff == -1 && !tti[i].isdst)
         utcoff = tti[i].utcoff;
      //printd(5, "QoreZoneInfo::QoreZoneInfo() tti[%d] %s: utcoff=%d isdst=%s isstd=%s isutc=%s\n", i, tti[i].abbr.c_str(), tti[i].utcoff, QB(tti[i].isdst), QB(tti[i].isstd), QB(tti[i].isutc));
   }

#if 0
   for (unsigned i = 0; i < QoreDSTTransitions.size(); ++i) {
      DateTime d((int64)QoreDSTTransitions[i].time);
      str.clear();
      d.format(str, "Dy Mon DD YYYY HH:mm:SS");
      QoreTransitionInfo &trans = *QoreDSTTransitions[i].trans;
      DateTime local(d.getEpochSeconds() + trans.utcoff);
      QoreString lstr;
      local.format(lstr, "Dy Mon DD YYYY HH:mm:SS");
      printd(0, "QoreZoneInfo::QoreZoneInfo() trans[%3d] time=%d %s UTC = %s %s isdst=%d isstd=%d isutc=%d utcoff=%d\n", i, QoreDSTTransitions[i].time, str.getBuffer(), lstr.getBuffer(), trans.abbr.c_str(), trans.isdst, trans.isstd, trans.isutc, trans.utcoff);
   }
#endif

   valid = true;
}

int QoreTimeZoneManager::process(const char *fn) {
   ExceptionSink xsink;

   return processIntern(fn, &xsink);
}

const AbstractQoreZoneInfo *QoreTimeZoneManager::processFile(const char *fn, ExceptionSink *xsink) {
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
   tzmap_t::iterator i = tzmap.find(fn);
   if (i != tzmap.end())
      return i->second;

   //printd(5, "QoreTimeZoneManager::processFile() %s: loading from registry\n", fn);

   std::auto_ptr<QoreWindowsZoneInfo> tzi(new QoreWindowsZoneInfo(fn, xsink));
   if (!*(tzi.get())) {
      return 0;
   }

   //printd(5, "QoreTimeZoneManager::processFile() %s -> %p\n", name.c_str(), tzi.get());
   QoreWindowsZoneInfo *rv = tzi.release();
   tzmap[fn] = rv;
   ++tzsize;

   return rv;
#else
   std::string name = !strncmp(root.getBuffer(), fn, root.strlen()) ? fn + root.strlen() + 1 : fn;
   tzmap_t::iterator i = tzmap.find(name);
   if (i != tzmap.end())
      return i->second;

   std::auto_ptr<QoreZoneInfo> tzi(new QoreZoneInfo(root, name, xsink));
   if (!*(tzi.get())) {
      //printd(1, "skipping %s/%s\n", root.getBuffer(), name.c_str());
      return 0;
   }

   //printd(5, "QoreTimeZoneManager::processFile() %s -> %p\n", name.c_str(), tzi.get());
   QoreZoneInfo *rv = tzi.release();
   tzmap[name] = rv;
   ++tzsize;

   return rv;
#endif
}

int QoreTimeZoneManager::processIntern(const char *fn, ExceptionSink *xsink) {
   // see if it's a directory or a file
   struct stat sbuf;

   if (stat(fn, &sbuf)) {
      printd(1, "error: could not stat() %s: %s\n", fn, strerror(errno));
      return -1;
   }

   if ((sbuf.st_mode & S_IFMT) == S_IFDIR)
      return processDir(fn, xsink);

   return processFile(fn, xsink) ? 0 : -1;
}

int QoreZoneInfo::getUTCOffsetImpl(int64 epoch_offset, bool &is_dst, const char *&zone_name) const {
   unsigned i = 0;
   if (first_pos > 0 && epoch_offset >= QoreDSTTransitions[first_pos - 1].time) {
      i = first_pos - 1;
      unsigned end = QoreDSTTransitions.size() - 1;
      while (i < end) {
         if (QoreDSTTransitions[i].time <= epoch_offset && QoreDSTTransitions[i + 1].time > epoch_offset) {
            zone_name = QoreDSTTransitions[i].trans->abbr.c_str();
            is_dst = QoreDSTTransitions[i].trans->isdst;

            //printf("QoreZoneInfo::getUTCOffsetImpl(epoch=%lld) tt[<=]=%d tt[>]=%d zone_name=%s is_dst=%d utcoff=%d\n", epoch_offset, QoreDSTTransitions[i].time, QoreDSTTransitions[i + 1].time, zone_name, is_dst, QoreDSTTransitions[i].trans->utcoff);
            return QoreDSTTransitions[i].trans->utcoff;
         }
         ++i;
      }
      // not found, time zone unknown
      is_dst = false;
      zone_name = std_abbr;

      //printf("QoreZoneInfo::getUTCOffsetImpl(epoch=%lld) NOT FOUND zone_name=%s is_dst=%d utcoff=%d\n", epoch_offset, zone_name, is_dst, utcoff);

      return utcoff;
   }

   i = first_pos + 1;
   while (i > 0) {
      if (QoreDSTTransitions[i].time > epoch_offset && QoreDSTTransitions[i - 1].time <= epoch_offset) {
         zone_name = QoreDSTTransitions[i].trans->abbr.c_str();
         is_dst = QoreDSTTransitions[i].trans->isdst;

         //printf("QoreZoneInfo::getUTCOffsetImpl(epoch=%lld) tt[<=]=%d tt[>]=%d zone_name=%s is_dst=%d utcoff=%d\n", epoch_offset, QoreDSTTransitions[i].time, QoreDSTTransitions[i + 1].time, zone_name, is_dst, QoreDSTTransitions[i].trans->utcoff);
         return QoreDSTTransitions[i].trans->utcoff;
      }
      --i;
   }
   // not found, time zone unknown
   is_dst = false;
   zone_name = std_abbr;

   //printf("QoreZoneInfo::getUTCOffsetImpl(epoch=%lld) NOT FOUND zone_name=%s is_dst=%d utcoff=%d\n", epoch_offset, zone_name, is_dst, utcoff);

   return utcoff;
}

// format: S00[[:]00[[:]00]] (S is + or -)
const QoreOffsetZoneInfo *QoreTimeZoneManager::findCreateOffsetZone(const char *offset, ExceptionSink *xsink) {
   static const char *fmt = "format must be: +DD[:DD[:DD]] or -DD[:DD[:DD]] where D is a digit from 0 - 9 (the ':' characters are optional)";

   // the caller must verify that the first character is either + or -
   assert(*offset == '-' || *offset == '+');

   if (strlen(offset) < 3) {
      if (xsink)
         xsink->raiseException("PARSE-SET-TIME-ZONE-ERROR", "error setting UTC offset '%s': less than minimum 3 characters long; %s", offset, fmt);
      return 0;
   }

   const char *p = offset + 1;
   if (!isdigit(*p)) {
      if (xsink)
         xsink->raiseException("PARSE-SET-TIME-ZONE-ERROR", "error setting UTC offset '%s': expecting a digit after the offset sign character; %s", offset, fmt);
      return 0;
   }
   int secs = (*p - '0') * SECS_PER_HOUR * 10;
   ++p;
   if (!isdigit(*p)) {
      if (xsink)
         xsink->raiseException("PARSE-SET-TIME-ZONE-ERROR", "error setting UTC offset '%s': expecting a 2 digit value for the hours portion of the UTC offset; %s", offset, fmt);
      return 0;
   }
   secs += (*p - '0') * SECS_PER_HOUR;
   ++p;
   if (*p) {
      if (*p == ':')
         ++p;
      if (!isdigit(*p)) {
         if (xsink)
            xsink->raiseException("PARSE-SET-TIME-ZONE-ERROR", "error setting UTC offset '%s': expecting a digit for the minutes portion of the UTC offset; %s", offset, fmt);
         return 0;
      }
      secs += (*p - '0') * SECS_PER_MINUTE * 10;
      ++p;
      if (!isdigit(*p)) {
         if (xsink)
            xsink->raiseException("PARSE-SET-TIME-ZONE-ERROR", "error setting UTC offset '%s': expecting a 2 digit value for the minutes portion of the UTC offset; %s", offset, fmt);
         return 0;
      }
      secs += (*p - '0') * SECS_PER_MINUTE;
      ++p;
      if (*p) {
         if (*p == ':')
            ++p;
         if (!isdigit(*p)) {
            if (xsink)
               xsink->raiseException("PARSE-SET-TIME-ZONE-ERROR", "error setting UTC offset '%s': expecting a digit for the seconds portion of the UTC offset; %s", offset, fmt);
            return 0;
         }
         secs += (*p - '0') * 10;
         ++p;
         if (!isdigit(*p)) {
            if (xsink)
               xsink->raiseException("PARSE-SET-TIME-ZONE-ERROR", "error setting UTC offset '%s': expecting a 2 digit value for the seconds portion of the UTC offset; %s", offset, fmt);
            return 0;
         }
         secs += *p - '0';
         ++p;
         if (*p) {
            if (xsink)
               xsink->raiseException("PARSE-SET-TIME-ZONE-ERROR", "error setting UTC offset '%s': excess text after the seconds value; %s", offset, fmt);
            return 0;
         }
      }
   }

   //printd(5, "QoreTimeZoneManager::findCreateOffsetZone(%s) secs=%d\n", offset, secs);

   // this is not an error; this is the UTC offset
   if (!secs)
      return 0;

   // we do not need to check for '+' or '-' here; this has been verified by the caller before sending to this function
   if (*offset == '-')
      secs = -secs;

   // first search standard zones (unlocked)
   tzomap_t::iterator i = tzo_std_map.find(secs);
   if (i != tzo_std_map.end())
      return i->second;

   // now search custom zones
   QoreAutoRWWriteLocker al(rwl_offset);
   i = tzomap.find(secs);
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

int QoreTimeZoneManager::processDir(const char *d, ExceptionSink *xsink) {
   std::string dir = d;
   dir += "/*";

   // glob buffer
   glob_t globbuf;
   // free glob buffer on exit
   ON_BLOCK_EXIT(globfree, &globbuf);

   if (glob(dir.c_str(), 0, 0, &globbuf))
      return -1;

   for (unsigned i = 0; i < globbuf.gl_pathc; ++i)
      processIntern(globbuf.gl_pathv[i], xsink);

   return 0;
}

int QoreTimeZoneManager::setLocalTZ(std::string fname, AbstractQoreZoneInfo *tzi) {
   localtz = tzi;
   tzmap[fname] = localtz;
   localtzname = fname;
   ++tzsize;

   printd(1, "QoreTimeZoneManager::setLocalTZ() set zoneinfo from region: %s (%s has_dst=%d utcoff=%d)\n", fname.c_str(), tzi->getRegionName(), tzi->hasDST(), tzi->getUTCOffset());

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

   return setLocalTZ(fname, tzi.release());
}

#define MAKE_STD_ZONE(offset, name) tzo_std_map[offset] = new QoreOffsetZoneInfo((name), (offset))

QoreTimeZoneManager::QoreTimeZoneManager() : tzsize(0), our_utcoffset(0), root(ZONEINFO_LOCATION), localtz(0) {
   // remove trailing "/" characters from root
   root.trim_trailing('/');

   // create offset time zones for current common time zones to make it
   // possible to have unlocked searched for these

   // UTC+01
   MAKE_STD_ZONE(1 * SECS_PER_HOUR, "+01");

   // UTC+02
   MAKE_STD_ZONE(2 * SECS_PER_HOUR, "+02");

   // UTC+03
   MAKE_STD_ZONE(3 * SECS_PER_HOUR, "+03");

   // UTC+03:30
   MAKE_STD_ZONE(3 * SECS_PER_HOUR + 30 * SECS_PER_MINUTE, "+03:30");

   // UTC+04
   MAKE_STD_ZONE(4 * SECS_PER_HOUR, "+04");

   // UTC+04:30
   MAKE_STD_ZONE(4 * SECS_PER_HOUR + 30 * SECS_PER_MINUTE, "+04:30");

   // UTC+05
   MAKE_STD_ZONE(5 * SECS_PER_HOUR, "+05");

   // UTC+05:30
   MAKE_STD_ZONE(5 * SECS_PER_HOUR + 30 * SECS_PER_MINUTE, "+05:30");

   // UTC+05:45
   MAKE_STD_ZONE(5 * SECS_PER_HOUR + 45 * SECS_PER_MINUTE, "+05:45");

   // UTC+06
   MAKE_STD_ZONE(6 * SECS_PER_HOUR, "+06");

   // UTC+06:30
   MAKE_STD_ZONE(6 * SECS_PER_HOUR + 30 * SECS_PER_MINUTE, "+06:30");

   // UTC+07
   MAKE_STD_ZONE(7 * SECS_PER_HOUR, "+07");

   // UTC+08
   MAKE_STD_ZONE(8 * SECS_PER_HOUR, "+08");

   // UTC+09
   MAKE_STD_ZONE(9 * SECS_PER_HOUR, "+09");

   // UTC+09:30
   MAKE_STD_ZONE(9 * SECS_PER_HOUR + 30 * SECS_PER_MINUTE, "+09:30");

   // UTC+10
   MAKE_STD_ZONE(10 * SECS_PER_HOUR, "+10");

   // UTC+10:30
   MAKE_STD_ZONE(10 * SECS_PER_HOUR + 30 * SECS_PER_MINUTE, "+10:30");

   // UTC+11
   MAKE_STD_ZONE(11 * SECS_PER_HOUR, "+11");

   // UTC+11:30
   MAKE_STD_ZONE(11 * SECS_PER_HOUR + 30 * SECS_PER_MINUTE, "+11:30");

   // UTC+12
   MAKE_STD_ZONE(12 * SECS_PER_HOUR, "+12");

   // UTC+12:45
   MAKE_STD_ZONE(12 * SECS_PER_HOUR + 45 * SECS_PER_MINUTE, "+12:45");

   // UTC+13
   MAKE_STD_ZONE(13 * SECS_PER_HOUR, "+13");

   // UTC+14
   MAKE_STD_ZONE(14 * SECS_PER_HOUR, "+14");

   // UTC-01
   MAKE_STD_ZONE(-1 * SECS_PER_HOUR, "-01");

   // UTC-02
   MAKE_STD_ZONE(-2 * SECS_PER_HOUR, "-02");

   // UTC-03
   MAKE_STD_ZONE(-3 * SECS_PER_HOUR, "-03");

   // UTC-03:30
   MAKE_STD_ZONE(-3 * SECS_PER_HOUR - 30 * SECS_PER_MINUTE, "-03:30");

   // UTC-04
   MAKE_STD_ZONE(-4 * SECS_PER_HOUR, "-04");

   // UTC-04:30
   MAKE_STD_ZONE(-4 * SECS_PER_HOUR - 30 * SECS_PER_MINUTE, "-04:30");

   // UTC-05
   MAKE_STD_ZONE(-5 * SECS_PER_HOUR, "-05");

   // UTC-06
   MAKE_STD_ZONE(-6 * SECS_PER_HOUR, "-06");

   // UTC-07
   MAKE_STD_ZONE(-7 * SECS_PER_HOUR, "-07");

   // UTC-08
   MAKE_STD_ZONE(-8 * SECS_PER_HOUR, "-08");

   // UTC-09
   MAKE_STD_ZONE(-9 * SECS_PER_HOUR, "-09");

   // UTC-09:30
   MAKE_STD_ZONE(-9 * SECS_PER_HOUR - 30 * SECS_PER_MINUTE, "-09:30");

   // UTC-10
   MAKE_STD_ZONE(-10 * SECS_PER_HOUR, "-10");

   // UTC-11
   MAKE_STD_ZONE(-11 * SECS_PER_HOUR, "-11");

   // UTC-12
   MAKE_STD_ZONE(-12 * SECS_PER_HOUR, "-12");
}

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
/*
static int wdate2date(const SYSTEMTIME &st, DateTime &dt) {
   dt.setDate(0, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds * 1000);
   return 0;
}
*/
static int wdate2str(const SYSTEMTIME &st, QoreString &str) {
   str.sprintf("year: %d mon: %d day: %d dow: %d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wDayOfWeek, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds * 1000);
   return 0;
}

static int wchar2utf8(const wchar_t *wstr, QoreString &str) {
   size_t len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, 0, 0, 0, 0);
   if (!len)
      return -1;
   str.reserve(len);
   WideCharToMultiByte(CP_UTF8, 0, wstr, -1, (LPSTR)str.getBuffer(), len, 0, 0);
   str.terminate(len);
   return 0;
}

static LONG wopenkey(HKEY hKey, const char *path, REGSAM samDesired, HKEY *pkey) {
   return RegOpenKeyEx(hKey, path, 0, samDesired, pkey);
}

#define WERR_SIZE 2048
QoreStringNode *get_windows_err(LONG rc) {
   QoreStringNode *desc = new QoreStringNode;
   desc->reserve(WERR_SIZE);
   DWORD drc = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, rc, LANG_USER_DEFAULT, (LPTSTR)desc->getBuffer(), WERR_SIZE, 0);
   desc->terminate(drc);
   if (!drc)
      desc->sprintf("FormatMessage() failed to retrieve error message for code %d", rc);
   else
      desc->trim();
   return desc;
}

#define RRF_RT_REG_SZ 0x0000ffff
static int wgetregstr(HKEY hk, const char *name, QoreString &val, ExceptionSink *xsink) {
   DWORD size = 0;
   LONG rc = RegQueryValueEx(hk, name, 0, 0, 0, &size);
   if (rc != ERROR_SUCCESS) {
      xsink->raiseException("TZINFO-ERROR", get_windows_err(rc));
      return -1;
   }
   val.reserve(size);

   rc = RegQueryValueEx(hk, name, 0, 0, (LPBYTE)val.getBuffer(), &size);
   if (rc != ERROR_SUCCESS) {
      xsink->raiseException("TZINFO-ERROR", get_windows_err(rc));
      return -1;
   }
   val.terminate(size);
   //printd(5, "wgetregstr(%s) got: %s\n", name, val.getBuffer());
   return 0;
}

typedef struct _REG_TZI_FORMAT {
   LONG Bias;
   LONG StandardBias;
   LONG DaylightBias;
   SYSTEMTIME StandardDate;
   SYSTEMTIME DaylightDate;
} REG_TZI_FORMAT;

#define WTZ_INFO "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\"

QoreWindowsZoneInfo::QoreWindowsZoneInfo(const char *n_name, ExceptionSink *xsink) : valid(false), rule(false), daylight_first(false), dst_off(0) {
   QoreString key(WTZ_INFO);
   key.concat(n_name);

   HKEY hk;
   LONG rc = wopenkey(HKEY_LOCAL_MACHINE, key.getBuffer(), KEY_QUERY_VALUE, &hk);
   if (rc) {
      QoreStringNode *desc = get_windows_err(rc);
      desc->prepend("': ");
      desc->prepend(key.getBuffer());
      desc->prepend("error opening windows registry key '");
      xsink->raiseException("TZINFO-ERROR", desc);
      return;
   }
   ON_BLOCK_EXIT(RegCloseKey, hk);

   if (wgetregstr(hk, "Display", display, xsink))
      return;
   if (wgetregstr(hk, "Dlt", daylight, xsink))
      return;
   if (wgetregstr(hk, "Std", standard, xsink))
      return;

   // set name from display name
   name = standard.getBuffer();

   // get TZI value
   REG_TZI_FORMAT tzi;
   DWORD size = sizeof(tzi);
   rc = RegQueryValueEx(hk, "TZI", 0, 0, (BYTE *)&tzi, &size);
   if (rc) {
      xsink->raiseException("TZINFO-ERROR", get_windows_err(rc));
      return;
   }

#ifdef DEBUG
   QoreString sd, dd;
   wdate2str(tzi.StandardDate, sd);
   wdate2str(tzi.DaylightDate, dd);
   printd(5, "QoreWindowsZoneInfo::QoreWindowsZoneInfo(%s) bias=%ld standardbias=%ld daylightbias=%ld standarddate=%s daylightdate=%s\n", n_name, tzi.Bias, tzi.StandardBias, tzi.DaylightBias, sd.getBuffer(), dd.getBuffer());
#endif

   // convert to seconds east
   utcoff = tzi.Bias * -60 + tzi.StandardBias * -60;
   if (tzi.DaylightBias) {
      dst_off = tzi.Bias * -60 + tzi.DaylightBias * -60;
      has_dst = true;
   }

   daylight_date = tzi.DaylightDate;
   standard_date = tzi.StandardDate;

   valid = true;
   if (!has_dst)
      return;

   if (!standard_date.wYear && !daylight_date.wYear) {
      rule = true;
      if (daylight_date.wMonth < standard_date.wMonth)
         daylight_first = true;

      // FIXME: change logic to set valid = false
      assert(standard_date.wDay >= 1 && standard_date.wDay <= 5);
      assert(daylight_date.wDay >= 1 && daylight_date.wDay <= 5);
   }
   else {
      // cannot handle this yet
      assert(false);
      valid = false;
   }

   //printd(5, "QoreWindowsZoneInfo::QoreWindowsZoneInfo(%s) utcoff=%d has_dst=%d dst_off=%d\n", n_name, utcoff, has_dst, dst_off);
}

int QoreWindowsZoneInfo::getUTCOffsetImpl(int64 epoch_offset, bool &is_dst, const char *&zone_name) const {
   if (has_dst) {
      int64 dst, std;
      qore_simple_tm tm;
      tm.set(epoch_offset, 0);
      getTransitions(tm.year, dst, std);
      if (daylight_first)
         is_dst = epoch_offset < dst || epoch_offset >= std ? false : true;
      else
         is_dst = epoch_offset < std || epoch_offset >= dst ? false : true;
   }
   else
      is_dst = false;

   zone_name = is_dst ? daylight.getBuffer() : standard.getBuffer();
   return is_dst ? dst_off : utcoff;
}

static int64 wget_trans_date(int year, SYSTEMTIME date, int utc_offset) {
   // get the day of the week of the first day of the transition month (Sun = 0)
   int dow = qore_date_info::getDayOfWeek(year, date.wMonth, 1);

   // get the last day of the month
   int ld = qore_date_info::getLastDayOfMonth(date.wMonth, year);

   // get transition date in month
   // first date date of first occurrence of the day in the month
   int day = date.wDayOfWeek - dow;
   if (day < 0)
      day += 7;
   ++day;

   day += (date.wDay - 1) * 7;
   if (day > ld)
      day -= 7;

   QoreString td;
   wdate2str(date, td);
   //printf("wget_trans_date(year=%d, trans=%s, utc_offset=%d) day=%d (dow=%d, ld=%d)\n", year, td.getBuffer(), utc_offset, day, dow, ld);

   // get epoch offset for this date as UTC
   int64 rc = qore_date_info::getEpochSeconds(year, date.wMonth, day, date.wHour, date.wMinute, date.wSecond);

   // return with local time offset added
   return rc - utc_offset;
}

void QoreWindowsZoneInfo::getTransitions(int year, int64 &dst, int64 &std) const {
   dst = wget_trans_date(year, daylight_date, utcoff);
   std = wget_trans_date(year, standard_date, dst_off);
}
#endif

void QoreTimeZoneManager::init_intern(QoreString &TZ) {
   // unix-style zoneinfo initialization
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
      printd(1, "QoreTimeZoneManager::init_intern(): cannot find zone: %s\n", TZ.getBuffer());
   }
}

void QoreTimeZoneManager::init() {
   QoreString TZ(QCS_USASCII);

#if (!defined _WIN32 && !defined __WIN32__) || defined __CYGWIN__
   init_intern(TZ);
#endif

#ifdef SOLARIS
   // on solaris try to parse /etc/TIMEZONE if TZ is not set
   if (!localtz) {
      QoreFile f;

      if (!f.open("/etc/TIMEZONE")) {
         while (!f.readLine(TZ)) {
            if (!strncmp(TZ.getBuffer(), "TZ=", 3)) {
               // remove "TZ=" from string
               TZ.splice(0, 3, NULL);
               // remove trailing whitespace and '\n' from string
               TZ.trim_trailing();
               // set local time zone region
               setLocalTZ(TZ.getBuffer());

               break;
            }
         }
      }
   }
#endif
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__
   TIME_ZONE_INFORMATION tzi;
   int rc = GetTimeZoneInformation(&tzi);
   // assume UTC if no zone info is available
   if (rc == TIME_ZONE_ID_UNKNOWN) {
      printd(0, "QoreTimeZoneManager::init() Windows GetTimeZoneInformation returned TIME_ZONE_ID_UNKNOWN, assuming UTC\n");
   }
   else {
      ExceptionSink xsink;

      if (!SysEnv.get("TZ", TZ)) {
         std::auto_ptr<QoreWindowsZoneInfo> twzi(new QoreWindowsZoneInfo(TZ.getBuffer(), &xsink));
         if (!*(twzi.get())) {
            xsink.clear();
            printd(1, "error reading windows registry while setting local time zone: %s\n", TZ.getBuffer());
         }
         else
            setLocalTZ(TZ.getBuffer(), twzi.release());
         return;
      }

      QoreString sn;
      wchar2utf8(tzi.StandardName, sn);

      // try to set the local time zone from the standard zone name
      std::auto_ptr<QoreWindowsZoneInfo> twzi(new QoreWindowsZoneInfo(sn.getBuffer(), &xsink));
      if (!*(twzi.get())) {
         //xsink.handleExceptions();
         xsink.clear();
         printd(1, "error reading windows registry while setting local time zone: %s\n", sn.getBuffer());
         return;
      }

      setLocalTZ(sn.getBuffer(), twzi.release());
   }
#endif

   // if no local time zone has been set, then set to UTC
   if (!localtz)
      setLocalTZ("UTC");
}

void QoreTimeZoneManager::setFromLocalTimeFile() {
   // determine local region
   struct stat sbuf;
#ifdef HAVE_LSTAT
   if (!lstat(LOCALTIME_LOCATION, &sbuf)) {
#else
   if (!stat(LOCALTIME_LOCATION, &sbuf)) {
#endif
      // normally this file is a symlink - we need the target file name for the name of the time zone region
#ifdef S_IFLNK
      printd(1, "QoreTimeZoneManager::QoreTimeZoneManager() %s: %d (%d)\n", LOCALTIME_LOCATION, sbuf.st_mode & S_IFMT, S_IFLNK);
      if ((sbuf.st_mode & S_IFMT) == S_IFLNK) {
        char buf[QORE_PATH_MAX + 1];
        qore_offset_t len = readlink(LOCALTIME_LOCATION, buf, QORE_PATH_MAX);
        if (len > 0) {
           buf[len] = '\0';
           if (buf[0] == '.' && buf[1] == '.') {
              char* dn = q_dirname(LOCALTIME_LOCATION);
              ON_BLOCK_EXIT(free, dn);
              QoreString path(dn);
              path.concat('/');
              path.concat(buf);
              //printd(5, "QoreTimeZoneManager::QoreTimeZoneManager() path: '%s'\n", path.getBuffer());
              setLocalTZ(path.getBuffer());
           }
           else
              setLocalTZ(buf);
        }
#ifdef DEBUG
        else
           printd(1, "QoreTimeZoneManager::QoreTimeZoneManager() failed to read %s link: %s\n", LOCALTIME_LOCATION, strerror(errno));
#endif // DEBUG
      }
      else
#endif // S_IFLNK
        setLocalTZ(LOCALTIME_LOCATION);
   }
#ifdef DEBUG
   else {
      printd(1, "cannot determine local time region: could not lstat() %s: %s\n", LOCALTIME_LOCATION, strerror(errno));
   }
#endif
}

int QoreTimeZoneManager::readAll(ExceptionSink *xsink) {
   if (processDir(root.getBuffer(), xsink)) {
      xsink->clear();
      printd(1, "no time zone information available; glob(%s) failed: %s", root.getBuffer(), strerror(errno));
      return -1;
   }

   printd(1, "QoreTimeZoneManager::QoreTimeZoneManager() %d regions cached\n", tzsize);

   return 0;
}

const AbstractQoreZoneInfo *QoreTimeZoneManager::findLoadRegion(const char *name, ExceptionSink *xsink) {
   QoreAutoRWWriteLocker al(rwl);
   // find or load region
   return processFile(name, xsink);
}

const AbstractQoreZoneInfo *findCreateOffsetZone(int seconds_east) {
   return QTZM.findCreateOffsetZone(seconds_east);
}

const AbstractQoreZoneInfo* find_create_timezone(const char* name, ExceptionSink* xsink) {
   assert(name);
   // see if it's a UTC offset
   if ((name[0] == '+' || name[0] == '-')
         && isdigit(name[1] && isdigit(name[2])))
      return QTZM.findCreateOffsetZone(name, xsink);

   return QTZM.findLoadRegion(name, xsink);
}

int tz_get_utc_offset(const AbstractQoreZoneInfo* tz, int64 epoch_offset, bool &is_dst, const char *&zone_name) {
   return tz->getUTCOffset(epoch_offset, is_dst, zone_name);
}

bool tz_has_dst(const AbstractQoreZoneInfo* tz) {
   return tz->hasDST();
}

const char* tz_get_region_name(const AbstractQoreZoneInfo* tz) {
   return tz->getRegionName();
}
