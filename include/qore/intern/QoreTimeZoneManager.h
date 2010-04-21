/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreTimeZoneManager.h

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

#ifndef QORE_TIMEZONEMANAGER_H

#define QORE_TIMEZONEMANAGER_H

#include <string>
#include <vector>
#include <map>

#ifndef LOCALTIME_LOCATION
#define LOCALTIME_LOCATION "/etc/localtime"
#endif

#ifndef ZONEINFO_LOCATION
#define ZONEINFO_LOCATION "/usr/share/zoneinfo"
#endif

DLLLOCAL extern const char *STATIC_UTC;

// transition info structure
struct QoreTransitionInfo {
   int         gmtoff;  // GMT offset in seconds east (negative for west)
   std::string abbr;    // time zone abbreviation (i.e. "EST")
   bool        isdst;   // is daylight standard time?
   bool        isstd;   // transition is standard time (true) or wall clock time (false)
   bool        isgmt;   // transition is UTC (true) or local time (false)
};

typedef std::vector<QoreTransitionInfo> trans_vec_t;

struct QoreLeapInfo {
   int ttime; // transition time
   int total; // total correction after transition time
};

struct QoreDSTTransition {
   int time;
   struct QoreTransitionInfo *trans;
};

class AbstractQoreZoneInfo {
protected:
   // region or time zone locale name (i.e. "Europe/Prague" or "-06:00" for UTC - 06:00)
   std::string name;
   // GMT offset in seconds east; -1 = unknown
   int gmtoff;
   // true if the zone ever has daylight savings time, false if not
   bool has_dst;

   // returns the GMT offset and local time zone name for the given time given as seconds from the epoch (1970-01-01Z)
   DLLLOCAL virtual int getGMTOffsetImpl(int64 epoch_offset, bool &is_dst, const char *&zone_name) const = 0;

public:
   DLLLOCAL AbstractQoreZoneInfo() : gmtoff(-1), has_dst(false) {
   }

   DLLLOCAL AbstractQoreZoneInfo(const std::string &n_name, int n_gmtoff = -1) : name(n_name), gmtoff(n_gmtoff), has_dst(false) {
   }

   virtual DLLLOCAL ~AbstractQoreZoneInfo() {
   }

   // returns general GMT offset for the time zone's standard time in seconds east
   DLLLOCAL int getGMTOffset() const {
      return !this || gmtoff == -1 ? 0 : gmtoff;
   }

   // returns the GMT offset for the given time given as seconds from the epoch (1970-01-01Z)
   DLLLOCAL int getGMTOffset(int64 epoch_offset) const {
      if (!this)
         return 0;

      const char *temp;
      bool is_dst;
      return getGMTOffsetImpl(epoch_offset, is_dst, temp);
   }

   // returns the GMT offset and local time zone name for the given time given as seconds from the epoch (1970-01-01Z)
   DLLLOCAL int getGMTOffset(int64 epoch_offset, bool &is_dst) const {
      if (!this) {
         is_dst = false;
         return 0;
      }

      const char *temp;
      return getGMTOffsetImpl(epoch_offset, is_dst, temp);
   }

   // returns the GMT offset and local time zone name for the given time given as seconds from the epoch (1970-01-01Z)
   DLLLOCAL int getGMTOffset(int64 epoch_offset, bool &is_dst, const char *&zone_name) const {
      if (!this) {
         is_dst = false;
         zone_name = "UTC";
         return 0;
      }

      return getGMTOffsetImpl(epoch_offset, is_dst, zone_name);
   }

   // returns true if the zone has daylight savings time ever
   DLLLOCAL bool hasDST() const {
      return this ? has_dst : false;
   }

   DLLLOCAL const char *getRegionName() const {
      if (!this)
         return STATIC_UTC;

      return name.c_str();
   }
};

// offsets are normally in the range of -12 to +14 UTC
// implements a simple offset from GMT
class QoreOffsetZoneInfo : public AbstractQoreZoneInfo {
protected:
   DLLLOCAL virtual int getGMTOffsetImpl(int64 epoch_offset, bool &is_dst, const char *&zone_name) const {
      zone_name = name.c_str();
      is_dst = false;
      return gmtoff;
   }
public:
   DLLLOCAL QoreOffsetZoneInfo(std::string &n_name, int seconds_east) : AbstractQoreZoneInfo(n_name, seconds_east) {
   }
   DLLLOCAL QoreOffsetZoneInfo(const char *n_name, int seconds_east) : AbstractQoreZoneInfo(n_name, seconds_east) {
   }
};

class QoreZoneInfo : public AbstractQoreZoneInfo {
protected:
   // first positive transition entry (after the epoch)
   int first_pos;
   bool valid;
   const char *std_abbr;  // standard time abbreviation

   // QoreDSTTransition times
   typedef std::vector<QoreDSTTransition> dst_transition_vec_t;
   dst_transition_vec_t QoreDSTTransitions;

   // QoreTransitionInfo array
   trans_vec_t tti;

   // leap info vector
   typedef std::vector<QoreLeapInfo> leap_vec_t;
   leap_vec_t leapinfo;

   // returns the GMT offset and local time zone name for the given time given as seconds from the epoch (1970-01-01Z)
   DLLLOCAL virtual int getGMTOffsetImpl(int64 epoch_offset, bool &is_dst, const char *&zone_name) const;

public:
   DLLLOCAL QoreZoneInfo(QoreString &root, std::string &n_name, ExceptionSink *xsink);

   DLLLOCAL virtual ~QoreZoneInfo() {
   }

   DLLLOCAL operator bool() const {
      return valid;
   }

   DLLLOCAL const trans_vec_t &getTransitionList() const {
      return tti;
   }
};

class QoreTimeZoneManager {
protected:
   // read-write lock to manage real (non-offset) zone info objects
   mutable QoreRWLock rwl;

   // read-write lock to guard access to offset custom zone info objects
   mutable QoreRWLock rwl_offset;

   // time zone info map (ex: "Europe/Prague" -> QoreZoneInfo*)
   typedef std::map<std::string, QoreZoneInfo *> tzmap_t;

   // offset map type
   typedef std::map<int, QoreOffsetZoneInfo *> tzomap_t;

   unsigned tzsize;

   // our gmt offset in seconds east of GMT
   int our_gmtoffset;

   QoreString root;
   tzmap_t tzmap;

   // standard (unlocked) zone offset map
   tzomap_t tzo_std_map;

   // custom (locked) zone offset map
   tzomap_t tzomap;

   // pointer to our regional time information
   QoreZoneInfo *localtz;
   std::string localtzname;

   DLLLOCAL int processIntern(const char *fn, ExceptionSink *xsink);
   DLLLOCAL int process(const char *fn);

   DLLLOCAL const QoreZoneInfo *processFile(const char *fn, ExceptionSink *xsink);
   DLLLOCAL int processDir(const char *d, ExceptionSink *xsink);

   // to set the local time zone information from a file 
   DLLLOCAL int setLocalTZ(std::string fname);

   DLLLOCAL void setFromLocalTimeFile();

   DLLLOCAL void init_intern(QoreString &TZ);

public:
   DLLLOCAL QoreTimeZoneManager();

   DLLLOCAL ~QoreTimeZoneManager() {
      for (tzmap_t::iterator i = tzmap.begin(), e = tzmap.end(); i != e; ++i)
	 delete i->second;

      for (tzomap_t::iterator i = tzo_std_map.begin(), e = tzo_std_map.end(); i != e; ++i) {
         //printd(0, "QoreTimeZoneManager::~QoreTimeZoneManager() deleting %d: %s\n", i->first, i->second->getRegionName());
	 delete i->second;
      }

      for (tzomap_t::iterator i = tzomap.begin(), e = tzomap.end(); i != e; ++i)
	 delete i->second;
   }

   DLLLOCAL QoreZoneInfo *getZone(const char *name) {
      QoreAutoRWReadLocker al(rwl);
      tzmap_t::iterator i = tzmap.find(name);
      return i == tzmap.end() ? 0 : i->second;
   }

   DLLLOCAL const QoreOffsetZoneInfo *findCreateOffsetZone(int seconds_east);
   DLLLOCAL const QoreOffsetZoneInfo *findCreateOffsetZone(const char *offset);

   DLLLOCAL int readAll(ExceptionSink *xsink);

   DLLLOCAL void init();

   DLLLOCAL const AbstractQoreZoneInfo *getLocalZoneInfo() const {
      return localtz;
   }

   DLLLOCAL const char *getLocalRegion() const {
      return localtzname.empty() ? 0 : localtzname.c_str();
   }

   DLLLOCAL const AbstractQoreZoneInfo *findLoadRegion(const char *name, ExceptionSink *xsink);
};

DLLLOCAL extern QoreTimeZoneManager QTZM;

#endif
