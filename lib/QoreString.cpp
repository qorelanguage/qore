/*
  QoreString.cpp

  QoreString Class Definition

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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
#include "qore/intern/qore_string_private.h"
#include "qore/intern/IconvHelper.h"
#include "qore/intern/StringReaderHelper.h"
#include "qore/minitest.hpp"

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <iconv.h>
#include <ctype.h>

#include <set>
#include <memory>
#include <string>
#include <map>

#ifdef DEBUG_TESTS
#  include "tests/QoreString_tests.cpp"
#endif

// to be used for trim
static char default_whitespace[] = { ' ', '\t', '\n', '\r', '\v', '\0' };

struct code_table {
      char symbol;
      const char* code;
      unsigned len;
};

// complete set of characters to percent-encode (RFC 3986 http://tools.ietf.org/html/rfc3986)
static int_set_t url_reserved;

// maps from entity strings to unicode code points
typedef std::map<std::string, uint32_t> emap_t;
// entity map from strings to unicode code points
static emap_t emap;

// maps from unicode code points to known entity references
typedef std::map<uint32_t, std::string> smap_t;
// entity map from unicode code points to strings
static smap_t smap;

struct unicode_entity {
   // unicode code point for symbol
   uint32_t symbol;
   // entity string
   std::string entity;
};

// contains the 252 character entities in HTML + &apos; for XML
static const struct unicode_entity xhtml_entity_list[] = {
   {34, "quot"},         // HTML 2.0: double quotation mark
   {38, "amp"},          // HTML 2.0: ampersand
   {39, "apos"},         // XHTML 1.0: apostrophe
   {60, "lt"},           // HTML 2.0: less-than sign
   {62, "gt"},           // HTML 2.0: greater-than sign
   {160, "nbsp"},        // HTML 3.2: no-break space (non-breaking space)[d]
   {161, "iexcl"},       // HTML 3.2: inverted exclamation mark
   {162, "cent"},        // HTML 3.2: cent sign
   {163, "pound"},       // HTML 3.2: pound sign
   {164, "curren"},      // HTML 3.2: currency sign
   {165, "yen"},         // HTML 3.2: yen sign (yuan sign)
   {166, "brvbar"},      // HTML 3.2: broken bar (broken vertical bar)
   {167, "sect"},        // HTML 3.2: section sign
   {168, "uml"},         // HTML 3.2: diaeresis (spacing diaeresis); see Germanic umlaut
   {169, "copy"},        // HTML 3.2: copyright symbol
   {170, "ordf"},        // HTML 3.2: feminine ordinal indicator
   {171, "laquo"},       // HTML 3.2: left-pointing double angle quotation mark (left pointing guillemet)
   {172, "not"},         // HTML 3.2: not sign
   {173, "shy"},         // HTML 3.2: soft hyphen (discretionary hyphen)
   {174, "reg"},         // HTML 3.2: registered sign (registered trademark symbol)
   {175, "macr"},        // HTML 3.2: macron (spacing macron, overline, APL overbar)
   {176, "deg"},         // HTML 3.2: degree symbol
   {177, "plusmn"},      // HTML 3.2: plus-minus sign (plus-or-minus sign)
   {178, "sup2"},        // HTML 3.2: superscript two (superscript digit two, squared)
   {179, "sup3"},        // HTML 3.2: superscript three (superscript digit three, cubed)
   {180, "acute"},       // HTML 3.2: acute accent (spacing acute)
   {181, "micro"},       // HTML 3.2: micro sign
   {182, "para"},        // HTML 3.2: pilcrow sign (paragraph sign)
   {183, "middot"},      // HTML 3.2: middle dot (Georgian comma, Greek middle dot)
   {184, "cedil"},       // HTML 3.2: cedilla (spacing cedilla)
   {185, "sup1"},        // HTML 3.2: superscript one (superscript digit one)
   {186, "ordm"},        // HTML 3.2: masculine ordinal indicator
   {187, "raquo"},       // HTML 3.2: right-pointing double angle quotation mark (right pointing guillemet)
   {188, "frac14"},      // HTML 3.2: vulgar fraction one quarter (fraction one quarter)
   {189, "frac12"},      // HTML 3.2: vulgar fraction one half (fraction one half)
   {190, "frac34"},      // HTML 3.2: vulgar fraction three quarters (fraction three quarters)
   {191, "iquest"},      // HTML 3.2: inverted question mark (turned question mark)
   {192, "Agrave"},      // HTML 2.0: Latin capital letter A with grave accent (Latin capital letter A grave)
   {193, "Aacute"},      // HTML 2.0: Latin capital letter A with acute accent
   {194, "Acirc"},       // HTML 2.0: Latin capital letter A with circumflex
   {195, "Atilde"},      // HTML 2.0: Latin capital letter A with tilde
   {196, "Auml"},        // HTML 2.0: Latin capital letter A with diaeresis
   {197, "Aring"},       // HTML 2.0: Latin capital letter A with ring above (Latin capital letter A ring)
   {198, "AElig"},       // HTML 2.0: Latin capital letter AE (Latin capital ligature AE)
   {199, "Ccedil"},      // HTML 2.0: Latin capital letter C with cedilla
   {200, "Egrave"},      // HTML 2.0: Latin capital letter E with grave accent
   {201, "Eacute"},      // HTML 2.0: Latin capital letter E with acute accent
   {202, "Ecirc"},       // HTML 2.0: Latin capital letter E with circumflex
   {203, "Euml"},        // HTML 2.0: Latin capital letter E with diaeresis
   {204, "Igrave"},      // HTML 2.0: Latin capital letter I with grave accent
   {205, "Iacute"},      // HTML 2.0: Latin capital letter I with acute accent
   {206, "Icirc"},       // HTML 2.0: Latin capital letter I with circumflex
   {207, "Iuml"},        // HTML 2.0: Latin capital letter I with diaeresis
   {208, "ETH"},         // HTML 2.0: Latin capital letter Eth
   {209, "Ntilde"},      // HTML 2.0: Latin capital letter N with tilde
   {210, "Ograve"},      // HTML 2.0: Latin capital letter O with grave accent
   {211, "Oacute"},      // HTML 2.0: Latin capital letter O with acute accent
   {212, "Ocirc"},       // HTML 2.0: Latin capital letter O with circumflex
   {213, "Otilde"},      // HTML 2.0: Latin capital letter O with tilde
   {214, "Ouml"},        // HTML 2.0: Latin capital letter O with diaeresis
   {215, "times"},       // HTML 3.2: multiplication sign
   {216, "Oslash"},      // HTML 2.0: Latin capital letter O with stroke (Latin capital letter O slash)
   {217, "Ugrave"},      // HTML 2.0: Latin capital letter U with grave accent
   {218, "Uacute"},      // HTML 2.0: Latin capital letter U with acute accent
   {219, "Ucirc"},       // HTML 2.0: Latin capital letter U with circumflex
   {220, "Uuml"},        // HTML 2.0: Latin capital letter U with diaeresis
   {221, "Yacute"},      // HTML 2.0: Latin capital letter Y with acute accent
   {222, "THORN"},       // HTML 2.0: Latin capital letter THORN
   {223, "szlig"},       // HTML 2.0: Latin small letter sharp s (ess-zed); see German Eszett
   {224, "agrave"},      // HTML 2.0: Latin small letter a with grave accent
   {225, "aacute"},      // HTML 2.0: Latin small letter a with acute accent
   {226, "acirc"},       // HTML 2.0: Latin small letter a with circumflex
   {227, "atilde"},      // HTML 2.0: Latin small letter a with tilde
   {228, "auml"},        // HTML 2.0: Latin small letter a with diaeresis
   {229, "aring"},       // HTML 2.0: Latin small letter a with ring above
   {230, "aelig"},       // HTML 2.0: Latin small letter ae (Latin small ligature ae)
   {231, "ccedil"},      // HTML 2.0: Latin small letter c with cedilla
   {232, "egrave"},      // HTML 2.0: Latin small letter e with grave accent
   {233, "eacute"},      // HTML 2.0: Latin small letter e with acute accent
   {234, "ecirc"},       // HTML 2.0: Latin small letter e with circumflex
   {235, "euml"},        // HTML 2.0: Latin small letter e with diaeresis
   {236, "igrave"},      // HTML 2.0: Latin small letter i with grave accent
   {237, "iacute"},      // HTML 2.0: Latin small letter i with acute accent
   {238, "icirc"},       // HTML 2.0: Latin small letter i with circumflex
   {239, "iuml"},        // HTML 2.0: Latin small letter i with diaeresis
   {240, "eth"},         // HTML 2.0: Latin small letter eth
   {241, "ntilde"},      // HTML 2.0: Latin small letter n with tilde
   {242, "ograve"},      // HTML 2.0: Latin small letter o with grave accent
   {243, "oacute"},      // HTML 2.0: Latin small letter o with acute accent
   {244, "ocirc"},       // HTML 2.0: Latin small letter o with circumflex
   {245, "otilde"},      // HTML 2.0: Latin small letter o with tilde
   {246, "ouml"},        // HTML 2.0: Latin small letter o with diaeresis
   {247, "divide"},      // HTML 3.2: division sign (obelus)
   {248, "oslash"},      // HTML 2.0: Latin small letter o with stroke (Latin small letter o slash)
   {249, "ugrave"},      // HTML 2.0: Latin small letter u with grave accent
   {250, "uacute"},      // HTML 2.0: Latin small letter u with acute accent
   {251, "ucirc"},       // HTML 2.0: Latin small letter u with circumflex
   {252, "uuml"},        // HTML 2.0: Latin small letter u with diaeresis
   {253, "yacute"},      // HTML 2.0: Latin small letter y with acute accent
   {254, "thorn"},       // HTML 2.0: Latin small letter thorn
   {255, "yuml"},        // HTML 2.0: Latin small letter y with diaeresis
   {338, "OElig"},       // HTML 4.0: Latin capital ligature oe[e]
   {339, "oelig"},       // HTML 4.0: Latin small ligature oe[e]
   {352, "Scaron"},      // HTML 4.0: Latin capital letter s with caron
   {353, "scaron"},      // HTML 4.0: Latin small letter s with caron
   {376, "Yuml"},        // HTML 4.0: Latin capital letter y with diaeresis
   {402, "fnof"},        // HTML 4.0: Latin small letter f with hook (function, florin)
   {710, "circ"},        // HTML 4.0: modifier letter circumflex accent
   {732, "tilde"},       // HTML 4.0: small tilde
   {913, "Alpha"},       // HTML 4.0: Greek capital letter Alpha
   {914, "Beta"},        // HTML 4.0: Greek capital letter Beta
   {915, "Gamma"},       // HTML 4.0: Greek capital letter Gamma
   {916, "Delta"},       // HTML 4.0: Greek capital letter Delta
   {917, "Epsilon"},     // HTML 4.0: Greek capital letter Epsilon
   {918, "Zeta"},        // HTML 4.0: Greek capital letter Zeta
   {919, "Eta"},         // HTML 4.0: Greek capital letter Eta
   {920, "Theta"},       // HTML 4.0: Greek capital letter Theta
   {921, "Iota"},        // HTML 4.0: Greek capital letter Iota
   {922, "Kappa"},       // HTML 4.0: Greek capital letter Kappa
   {923, "Lambda"},      // HTML 4.0: Greek capital letter Lambda
   {924, "Mu"},          // HTML 4.0: Greek capital letter Mu
   {925, "Nu"},          // HTML 4.0: Greek capital letter Nu
   {926, "Xi"},          // HTML 4.0: Greek capital letter Xi
   {927, "Omicron"},     // HTML 4.0: Greek capital letter Omicron
   {928, "Pi"},          // HTML 4.0: Greek capital letter Pi
   {929, "Rho"},         // HTML 4.0: Greek capital letter Rho
   {931, "Sigma"},       // HTML 4.0: Greek capital letter Sigma
   {932, "Tau"},         // HTML 4.0: Greek capital letter Tau
   {933, "Upsilon"},     // HTML 4.0: Greek capital letter Upsilon
   {934, "Phi"},         // HTML 4.0: Greek capital letter Phi
   {935, "Chi"},         // HTML 4.0: Greek capital letter Chi
   {936, "Psi"},         // HTML 4.0: Greek capital letter Psi
   {937, "Omega"},       // HTML 4.0: Greek capital letter Omega
   {945, "alpha"},       // HTML 4.0: Greek small letter alpha
   {946, "beta"},        // HTML 4.0: Greek small letter beta
   {947, "gamma"},       // HTML 4.0: Greek small letter gamma
   {948, "delta"},       // HTML 4.0: Greek small letter delta
   {949, "epsilon"},     // HTML 4.0: Greek small letter epsilon
   {950, "zeta"},        // HTML 4.0: Greek small letter zeta
   {951, "eta"},         // HTML 4.0: Greek small letter eta
   {952, "theta"},       // HTML 4.0: Greek small letter theta
   {953, "iota"},        // HTML 4.0: Greek small letter iota
   {954, "kappa"},       // HTML 4.0: Greek small letter kappa
   {955, "lambda"},      // HTML 4.0: Greek small letter lambda
   {956, "mu"},          // HTML 4.0: Greek small letter mu
   {957, "nu"},          // HTML 4.0: Greek small letter nu
   {958, "xi"},          // HTML 4.0: Greek small letter xi
   {959, "omicron"},     // HTML 4.0: Greek small letter omicron
   {960, "pi"},          // HTML 4.0: Greek small letter pi
   {961, "rho"},         // HTML 4.0: Greek small letter rho
   {962, "sigmaf"},      // HTML 4.0: Greek small letter final sigma
   {963, "sigma"},       // HTML 4.0: Greek small letter sigma
   {964, "tau"},         // HTML 4.0: Greek small letter tau
   {965, "upsilon"},     // HTML 4.0: Greek small letter upsilon
   {966, "phi"},         // HTML 4.0: Greek small letter phi
   {967, "chi"},         // HTML 4.0: Greek small letter chi
   {968, "psi"},         // HTML 4.0: Greek small letter psi
   {969, "omega"},       // HTML 4.0: Greek small letter omega
   {977, "thetasym"},    // HTML 4.0: Greek theta symbol
   {978, "upsih"},       // HTML 4.0: Greek Upsilon with hook symbol
   {982, "piv"},         // HTML 4.0: Greek pi symbol
   {8194, "ensp"},       // HTML 4.0: en space[d]
   {8195, "emsp"},       // HTML 4.0: em space[d]
   {8201, "thinsp"},     // HTML 4.0: thin space[d]
   {8204, "zwnj"},       // HTML 4.0: zero-width non-joiner
   {8205, "zwj"},        // HTML 4.0: zero-width joiner
   {8206, "lrm"},        // HTML 4.0: left-to-right mark
   {8207, "rlm"},        // HTML 4.0: right-to-left mark
   {8211, "ndash"},      // HTML 4.0: en dash
   {8212, "mdash"},      // HTML 4.0: em dash
   {8216, "lsquo"},      // HTML 4.0: left single quotation mark
   {8217, "rsquo"},      // HTML 4.0: right single quotation mark
   {8218, "sbquo"},      // HTML 4.0: single low-9 quotation mark
   {8220, "ldquo"},      // HTML 4.0: left double quotation mark
   {8221, "rdquo"},      // HTML 4.0: right double quotation mark
   {8222, "bdquo"},      // HTML 4.0: double low-9 quotation mark
   {8224, "dagger"},     // HTML 4.0: dagger, obelisk
   {8225, "Dagger"},     // HTML 4.0: double dagger, double obelisk
   {8226, "bull"},       // HTML 4.0: bullet (black small circle)[f]
   {8230, "hellip"},     // HTML 4.0: horizontal ellipsis (three dot leader)
   {8240, "permil"},     // HTML 4.0: per mille sign
   {8242, "prime"},      // HTML 4.0: prime (minutes, feet)
   {8243, "Prime"},      // HTML 4.0: double prime (seconds, inches)
   {8249, "lsaquo"},     // HTML 4.0: single left-pointing angle quotation mark[g]
   {8250, "rsaquo"},     // HTML 4.0: single right-pointing angle quotation mark[g]
   {8254, "oline"},      // HTML 4.0: overline (spacing overscore)
   {8260, "frasl"},      // HTML 4.0: fraction slash (solidus)
   {8364, "euro"},       // HTML 4.0: euro sign
   {8465, "image"},      // HTML 4.0: black-letter capital I (imaginary part)
   {8472, "weierp"},     // HTML 4.0: script capital P (power set, Weierstrass p)
   {8476, "real"},       // HTML 4.0: black-letter capital R (real part symbol)
   {8482, "trade"},      // HTML 4.0: trademark symbol
   {8501, "alefsym"},    // HTML 4.0: alef symbol (first transfinite cardinal)[h]
   {8592, "larr"},       // HTML 4.0: leftwards arrow
   {8593, "uarr"},       // HTML 4.0: upwards arrow
   {8594, "rarr"},       // HTML 4.0: rightwards arrow
   {8595, "darr"},       // HTML 4.0: downwards arrow
   {8596, "harr"},       // HTML 4.0: left right arrow
   {8629, "crarr"},      // HTML 4.0: downwards arrow with corner leftwards (carriage return)
   {8656, "lArr"},       // HTML 4.0: leftwards double arrow[i]
   {8657, "uArr"},       // HTML 4.0: upwards double arrow
   {8658, "rArr"},       // HTML 4.0: rightwards double arrow[j]
   {8659, "dArr"},       // HTML 4.0: downwards double arrow
   {8660, "hArr"},       // HTML 4.0: left right double arrow
   {8704, "forall"},     // HTML 4.0: for all
   {8706, "part"},       // HTML 4.0: partial differential
   {8707, "exist"},      // HTML 4.0: there exists
   {8709, "empty"},      // HTML 4.0: empty set (null set); see also U+8960, ⌀
   {8711, "nabla"},      // HTML 4.0: del or nabla (vector differential operator)
   {8712, "isin"},       // HTML 4.0: element of
   {8713, "notin"},      // HTML 4.0: not an element of
   {8715, "ni"},         // HTML 4.0: contains as member
   {8719, "prod"},       // HTML 4.0: n-ary product (product sign)[k]
   {8721, "sum"},        // HTML 4.0: n-ary summation[l]
   {8722, "minus"},      // HTML 4.0: minus sign
   {8727, "lowast"},     // HTML 4.0: asterisk operator
   {8730, "radic"},      // HTML 4.0: square root (radical sign)
   {8733, "prop"},       // HTML 4.0: proportional to
   {8734, "infin"},      // HTML 4.0: infinity
   {8736, "ang"},        // HTML 4.0: angle
   {8743, "and"},        // HTML 4.0: logical and (wedge)
   {8744, "or"},         // HTML 4.0: logical or (vee)
   {8745, "cap"},        // HTML 4.0: intersection (cap)
   {8746, "cup"},        // HTML 4.0: union (cup)
   {8747, "int"},        // HTML 4.0: integral
   {8756, "there4"},     // HTML 4.0: therefore sign
   {8764, "sim"},        // HTML 4.0: tilde operator (varies with, similar to)[m]
   {8773, "cong"},       // HTML 4.0: congruent to
   {8776, "asymp"},      // HTML 4.0: almost equal to (asymptotic to)
   {8800, "ne"},         // HTML 4.0: not equal to
   {8801, "equiv"},      // HTML 4.0: identical to; sometimes used for 'equivalent to'
   {8804, "le"},         // HTML 4.0: less-than or equal to
   {8805, "ge"},         // HTML 4.0: greater-than or equal to
   {8834, "sub"},        // HTML 4.0: subset of
   {8835, "sup"},        // HTML 4.0: superset of[n]
   {8836, "nsub"},       // HTML 4.0: not a subset of
   {8838, "sube"},       // HTML 4.0: subset of or equal to
   {8839, "supe"},       // HTML 4.0: superset of or equal to
   {8853, "oplus"},      // HTML 4.0: circled plus (direct sum)
   {8855, "otimes"},     // HTML 4.0: circled times (vector product)
   {8869, "perp"},       // HTML 4.0: up tack (orthogonal to, perpendicular)[o]
   {8901, "sdot"},       // HTML 4.0: dot operator[p]
   {8968, "lceil"},      // HTML 4.0: left ceiling (APL upstile)
   {8969, "rceil"},      // HTML 4.0: right ceiling
   {8970, "lfloor"},     // HTML 4.0: left floor (APL downstile)
   {8971, "rfloor"},     // HTML 4.0: right floor
   {9001, "lang"},       // HTML 4.0: left-pointing angle bracket (bra)[q]
   {9002, "rang"},       // HTML 4.0: right-pointing angle bracket (ket)[r]
   {9674, "loz"},        // HTML 4.0: lozenge
   {9824, "spades"},     // HTML 4.0: black spade suit[f]
   {9827, "clubs"},      // HTML 4.0: black club suit (shamrock)[f]
   {9829, "hearts"},     // HTML 4.0: black heart suit (valentine)[f]
   {9830, "diams"},      // HTML 4.0: black diamond suit[f]
};

#define HTML_ASCII(c) (c==34||c==38||c==60||c==62)
#define XML_ASCII(c) (HTML_ASCII(c)||c==39)

#define NUM_ENTITIES (sizeof(xhtml_entity_list) / sizeof (struct unicode_entity))

static const struct code_table html_codes[] = {
   { '&', "&amp;", 5 },
   { '<', "&lt;", 4 },
   { '>', "&gt;", 4 },
   { '"', "&quot;", 6 },
};

#define NUM_HTML_CODES (sizeof(html_codes) / sizeof (struct code_table))

void qore_string_init() {
   static int url_reserved_list[] = { '!', '*', '\'', '(', ')', ';', ':', '@', '&', '=', '+', '$', ',', '/', '?', '#', '[', ']' };
#define URLIST_SIZE (sizeof(url_reserved_list) / sizeof(int))

   for (unsigned i = 0; i < URLIST_SIZE; ++i)
      url_reserved.insert(url_reserved_list[i]);

   for (unsigned i = 0; i < NUM_ENTITIES; ++i) {
      assert(emap.find(xhtml_entity_list[i].entity) == emap.end());
      emap[xhtml_entity_list[i].entity] = xhtml_entity_list[i].symbol;
      assert(smap.find(xhtml_entity_list[i].symbol) == smap.end());
      smap[xhtml_entity_list[i].symbol] = xhtml_entity_list[i].entity;
   }
}

static unsigned get_unicode_from_utf8(const char* buf, unsigned bl) {
   if (bl == 1)
      return buf[0];

   if (bl == 2)
      return ((buf[0] & 0x1f) << 6)
         | (buf[1] & 0x3f);

   if (bl == 3)
      return ((buf[0] & 0x0f) << 12)
         | ((buf[1] & 0x3f) << 6)
         | (buf[2] & 0x3f);

   return (((unsigned)(buf[0] & 0x07)) << 18)
      | (((unsigned)(buf[1] & 0x3f)) << 12)
      | ((((unsigned)buf[2] & 0x3f)) << 6)
      | (((unsigned)buf[3] & 0x3f));
}

void qore_string_private::concatUTF8FromUnicode(unsigned code) {
   if (code > 0xffff) { // 4-byte code
      concat(0xf0 | ((code & (0x7 << 18)) >> 18));
      concat(0x80 | ((code & (0x3f << 12)) >> 12));
      concat(0x80 | ((code & (0x3f << 6)) >> 6));
      concat(0x80 | (code & 0x3f));
   }
   else if (code > 0x7ff) { // 3-byte code
      concat(0xe0 | ((code & (0xf << 12)) >> 12));
      concat(0x80 | ((code & (0x3f << 6)) >> 6));
      concat(0x80 | (code & 0x3f));
   }
   else if (code > 0x7f) { // 2-byte code
      concat(0xc0 | ((code & (0x2f << 6)) >> 6));
      concat(0x80 | (code & 0x3f));
   }
   else
      concat((char)code);
}

int qore_string_private::concatDecodeUriIntern(ExceptionSink* xsink, const qore_string_private& str, bool detect_query) {
   bool in_query = false;

   const char* url = str.buf;
   while (*url) {
      int x1 = getHex(url);
      if (x1 >= 0) {
	 // see if a multi-byte char is starting
	 if ((x1 & 0xc0) == 0xc0) {
	    int x2 = getHex(url);
	    if (x2 == -1) {
	       xsink->raiseException("URL-ENCODING-ERROR", "percent encoding indicated a multi-byte UTF-8 character but only one byte was provided");
	       return -1;
	    }
	    // check for a 3-byte sequence
	    if (x1 & 0x20) {
	       int x3 = getHex(url);
	       if (x3 == -1) {
		  xsink->raiseException("URL-ENCODING-ERROR", "percent encoding indicated a %s-byte UTF-8 character but only two bytes were provided", (x1 & 0x10) ? "four" : "three");
		  return -1;
	       }

	       // check for a 4-byte sequence
	       if (x1 & 0x10) {
		  int x4 = getHex(url);
		  if (x4 == -1) {
		     xsink->raiseException("URL-ENCODING-ERROR", "percent encoding indicated a four-byte UTF-8 character but only three bytes were provided");
		     return -1;
		  }
		  if (!(x2 & 0x80 && x3 & 0x80 && x4 & 0x80)) {
		     xsink->raiseException("URL-ENCODING-ERROR", "percent encoding gave an invalid four-byte UTF-8 character");
		     return -1;
		  }

		  // if utf8 then concat directly
		  if (getEncoding() == QCS_UTF8) {
		     concat((char)x1);
		     concat((char)x2);
		     concat((char)x3);
		     concat((char)x4);
		  }
		  else {
		     unsigned cp = (((unsigned)(x1 & 0x07)) << 18)
			| (((unsigned)(x2 & 0x3f)) << 12)
			| ((((unsigned)x3 & 0x3f)) << 6)
			| (((unsigned)x4 & 0x3f));
		     if (concatUnicode(cp, xsink))
			return -1;
		  }
		  continue;
	       }
	       // 3-byte sequence
	       if (!(x2 & 0x80 && x3 & 0x80)) {
		  xsink->raiseException("URL-ENCODING-ERROR", "percent encoding gave an invalid three-byte UTF-8 character");
		  return -1;
	       }

	       // if utf8 then concat directly
	       if (getEncoding() == QCS_UTF8) {
		  concat((char)x1);
		  concat((char)x2);
		  concat((char)x3);
	       }
	       else {
		  unsigned cp = (((unsigned)(x1 & 0x0f)) << 12)
		     | (((unsigned)(x2 & 0x3f)) << 6)
		     | (((unsigned)x3 & 0x3f));

		  if (concatUnicode(cp, xsink))
		     return -1;
	       }
	       continue;
	    }
	    // 2-byte sequence
	    if (!(x2 & 0x80)) {
	       xsink->raiseException("URL-ENCODING-ERROR", "percent encoding gave an invalid two-byte UTF-8 character");
	       return -1;
	    }

	    // if utf8 then concat directly
	    if (getEncoding() == QCS_UTF8) {
	       concat((char)x1);
	       concat((char)x2);
	    }
	    else {
	       unsigned cp = (((unsigned)(x1 & 0x1f)) << 6)
		  | ((unsigned)(x2 & 0x3f));
	       if (concatUnicode(cp, xsink))
		  return -1;
	    }
	    continue;
	 }
	 // single byte
	 concat(x1);
	 continue;
      }

      if (detect_query) {
	 if (!in_query) {
	    if ((*url) == '?')
	       in_query = true;
	 }
	 else {
	    if ((*url) == '+') {
	       ++url;
	       concat(' ');
	       continue;
	    }
	    else if ((*url) == '#') {
	       in_query = false;
	    }
	 }
      }

      concat(*(url++));
   }
   return 0;
}

int qore_string_private::concatEncodeUriRequest(ExceptionSink* xsink, const qore_string_private& str) {
   assert(str.len);

   int state = QUS_PATH;

   const unsigned char* p = (const unsigned char*)str.buf;
   while (*p) {
      if ((*p) == '%')
	 concat("%25");
      else if (*p > 127) {
	 qore_size_t len = q_UTF8_get_char_len((const char*)p, str.len - ((const char*)p - str.buf));
	 if (len <= 0) {
	    xsink->raiseException("INVALID-ENCODING", "invalid UTF-8 encoding found in string");
	    return -1;
	 }
	 // add UTF-8 percent-encoded characters
	 for (qore_size_t i = 0; i < len; ++i)
	    sprintf("%%%X", (unsigned)p[i]);
	 p += len;
	 continue;
      }
      else if (state == QUS_PATH) {
	 if ((*p) == '?') {
	    state = QUS_QUERY;
	    concat(*p);
	 }
	 else if ((*p) == '#') {
	    state = QUS_FRAGMENT;
	    concat(*p);
	 }
	 else if ((*p) == ' ')
	    concat("%20");
	 else
	    concat(*p);
      }
      else if (state == QUS_QUERY) {
	 if ((*p) == ' ')
	    concat('+');
	 else if ((*p) == '+')
	    concat("%2b");
	 else
	    concat(*p);
      }
      else {
	 assert(state == QUS_PATH);
	 if ((*p) == ' ')
	    concat("%20");
	 else
	    concat(*p);
      }

      ++p;
   }

   return 0;
}


// static function
int qore_string_private::convert_encoding_intern(const char* src, qore_size_t src_len, const QoreEncoding* from, QoreString& targ, const QoreEncoding* nccs, ExceptionSink* xsink) {
   assert(targ.priv->getEncoding() == nccs);
   assert(targ.empty());

   //printd(5, "qore_string_private::convert_encoding_intern() %s -> %s len: " QSD " src='%s'\n", from->getCode(), nccs->getCode(), src_len, src);

   IconvHelper c(nccs, from, xsink);
   if (*xsink)
      return -1;

   // now convert value
   qore_size_t al = src_len + STR_CLASS_BLOCK;
   targ.allocate(al + 1);
   while (true) {
      size_t ilen = src_len;
      size_t olen = al;
      char* ib = (char*)src;
      char* ob = targ.priv->buf;
      size_t rc = c.iconv(&ib, &ilen, &ob, &olen);
      if (rc == (size_t)-1) {
         switch (errno) {
            case EINVAL:
            case EILSEQ:
               c.reportIllegalSequence(ib - src, xsink);
               targ.clear();
               return -1;
            case E2BIG:
               al += STR_CLASS_BLOCK;
               targ.allocate(al + 1);
               break;
            default: {
               c.reportUnknownError(xsink);
               targ.clear();
               return -1;
            }
         }
      } else {
         // terminate string
         targ.priv->buf[al - olen] = '\0';
         targ.priv->len = al - olen;
         break;
      }
   }
   /*
   // remove byte order markers at the beginning of UTF16 strings
   if (nccs == QCS_UTF16 && targ.priv->len >= 2 && (signed char)targ.priv->buf[0] == -2 && (signed char)targ.priv->buf[1] == -1)
      targ.splice_simple(0, 2);
   */
   return 0;
}

unsigned int qore_string_private::getUnicodePointFromBytePos(qore_size_t offset, unsigned& clen, ExceptionSink* xsink) const {
   if (getEncoding() == QCS_UTF8) {
      clen = QCS_UTF8->getByteLen(buf + offset, buf + len, 1, xsink);
      if (*xsink)
	 return 0;

      return get_unicode_from_utf8(buf + offset, clen);
   }

   assert(!getEncoding()->isMultiByte());
   clen = 1;
   QoreString tmp(QCS_UTF8);
   if (convert_encoding_intern(buf + offset, 1, getEncoding(), tmp, QCS_UTF8, xsink))
      return 0;

   qore_size_t bl = QCS_UTF8->getByteLen(tmp.priv->buf, tmp.priv->buf + tmp.priv->len, 1, xsink);
   if (*xsink)
      return 0;

   return get_unicode_from_utf8(tmp.priv->buf, bl);
}

int qore_string_private::concatEncode(ExceptionSink* xsink, const QoreString& str, unsigned code) {
   //printd(5, "qore_string_private::concatEncode() '%s' code: 0x%x\n", str.getBuffer(), code);

   // if it's not a null string
   if (!str.priv->len)
      return 0;

   // if no encoding should be done, then just concat normally
   if (!code) {
      concat(&str, xsink);
      return *xsink ? -1 : 0;
   }

   qore_string_private* p = 0;
   TempEncodingHelper cstr;

   // convert encoding if we are not going to encode all non-ascii characters anyway
   if (!(code & CE_NONASCII)) {
      if (!cstr.set(&str, getEncoding(), xsink))
	 return -1;
      p = cstr->priv;
   }
   else
      p = str.priv;

   //printd(5, "qore_string_private::concatEncode() p: %p '%s' len: %d\n", p, p->buf, p->len);

   allocate(len + p->len + p->len / 10 + 10); // avoid reallocations inside the loop, value guesstimated
   for (qore_size_t i = 0; i < p->len; ++i) {
      // see if we are dealing with a non-ascii character
      const unsigned char c = p->buf[i];
      if ((c & 0x80)) {
	 unsigned len = 0;
	 unsigned cp = p->getUnicodePointFromBytePos(i, len, xsink);
	 if ((code & CE_HTML)) {
	    if (*xsink)
	       return -1;
	    assert(len);
	    assert(cp);
	    // see if it's an known special character
	    smap_t::iterator si = smap.find(cp);
	    //printd(5, "qore_string_private::concatEncode() i: %u c: %u cp: %u len: %u found: %s (%s)\n", i, c, cp, len, si != smap.end() ? "true" : "false", getEncoding()->getCode());
	    if (si != smap.end()) {
	       sprintf("&%s;", si->second.c_str());
	       i += len - 1;
	       continue;
	    }
	 }

	 if (code & CE_NONASCII) {
	    // otherwise just output the character entity
	    sprintf("&#%u;", cp);
	    i += len - 1;
	    continue;
	 }
      }
      else if (((code & CE_HTML) && HTML_ASCII(c))
	       || ((code & CE_XML) && XML_ASCII(c))) {
	 smap_t::iterator it = smap.find(c);
	 assert(it != smap.end());
	 concat('&');
	 concat(it->second.c_str());
	 concat(';');
	 continue;
      }

      concat(p->buf[i]);
   }
   return 0;
}

int qore_string_private::concatDecode(ExceptionSink* xsink, const QoreString& str, unsigned code) {
   // if it's not a null string
   if (!str.priv->len)
      return 0;

   // if no decoding should be done, then just concat normally
   if (!code) {
      concat(&str, xsink);
      return *xsink ? -1 : 0;
   }

   const QoreEncoding* enc = getEncoding();

   TempEncodingHelper cstr(str, enc, xsink);
   if (!cstr)
      return -1;

   qore_string_private* p = cstr->priv;
   //printd(5, "qore_string_private::concatDecode() p: %p '%s' len: %d\n", p, p->buf, p->len);

   // decode all entity references
   bool dall = (code & CD_XHTML) == CD_XHTML;

   // try to avoid reallocations inside the loop
   allocate(len + p->len + 1);
   for (qore_size_t i = 0; i < p->len; ++i) {
      // see if we are dealing with a non-ascii character
      const char* s = p->buf + i;
      if (*s != '&') {
         concat(*s);
         continue;
      }
      // concatenate translated character
      // check for unicode character references
      if ((*(s + 1) == '#')) {
	 if (code & CD_NUM_REF) {
	    s += 2;
	    // find end of character sequence
	    const char* e = strchr(s, ';');
	    // if not found or the number is too big, then don't try to decode it
	    if (e && (e - s) < 8) {
	       unsigned code;
	       if (*s == 'x')
		  code = strtoul(s + 1, 0, 16);
	       else
		  code = strtoul(s, 0, 10);

	       if (!concatUnicode(code)) {
		  i = e - p->buf;
		  continue;
	       }
	       // error occurred, so back out
	       s -= 2;
	    }
	 }
      }
      else if (isascii(*(s + 1)) && (code & CD_XHTML)) {
         s += 1;
         // find end of character sequence
         const char* e = strchr(s, ';');
         // if not found or the number is too big, then don't try to decode it
         if (e && (e - s) < 16) {
            // find unicode code point
            std::string entity(s, e - s);
            //printd(0, "entity: %s\n", entity.c_str());
            emap_t::iterator it = emap.find(entity);
            if (it != emap.end()) {
	       bool ok = dall;
	       // if XML only
	       if (!ok && !(code & CD_HTML))
		  ok = it->second < 0x80;
	       // if HTML only
	       else if (!ok && !(code & CD_XML))
		  ok = it->second != 0x27;
	       else
		  ok = true;
	       if (ok && !concatUnicode(it->second)) {
		  i = e - p->buf;
		  continue;
	       }
	    }
         }
         // not found or error occurred concatenating char, so back out
         s -= 1;
      }

      // concatentate ASCII characters directly
      if ((unsigned char)(*s) < 0x80) {
	 concat(*s);
	 continue;
      }
      // concatenate character as a single unit
      size_t cl = enc->getByteLen(s, p->buf + p->len, 1, xsink);
      if (*xsink)
	 return -1;
      concat_intern(s, cl);
      i += cl - 1;
   }

   return 0;
}

int qore_string_private::concat(const QoreString* str, ExceptionSink* xsink) {
   //printd(5, "concat() buf='%s' str='%s'\n", buf, str->priv->buf);
   // if it's not a null string
   if (!str || !str->priv->len)
      return 0;

   TempEncodingHelper cstr(str, getEncoding(), xsink);
   if (*xsink)
      return -1;

   // if priv->buffer needs to be resized
   check_char(cstr->priv->len + len + STR_CLASS_EXTRA);
   // concatenate new string
   memcpy(buf + len, cstr->priv->buf, cstr->priv->len);
   len += cstr->priv->len;
   buf[len] = '\0';
   return 0;
}

int qore_string_private::concatUnicode(unsigned code) {
   if (getEncoding() == QCS_UTF8) {
      concatUTF8FromUnicode(code);
      return 0;
   }
   QoreString tmp(QCS_UTF8);
   tmp.priv->concatUTF8FromUnicode(code);

   ExceptionSink xsink;

   TempString ns(tmp.convertEncoding(getEncoding(), &xsink));
   if (xsink) {
      // ignore exceptions
      xsink.clear();
      return -1;
   }

   concat(ns);
   return 0;
}

QoreStringMaker::QoreStringMaker(const char* fmt, ...) {
   va_list args;

   while (true) {
      va_start(args, fmt);
      int rc = vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
}

QoreStringMaker::QoreStringMaker(const QoreEncoding* enc, const char* fmt, ...) : QoreString(enc) {
   va_list args;

   while (true) {
      va_start(args, fmt);
      int rc = vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
}

QoreString::QoreString() : priv(new qore_string_private) {
   priv->len = 0;
   priv->allocated = STR_CLASS_BLOCK;
   priv->buf = (char*)malloc(priv->allocated * sizeof(char));
   priv->buf[0] = '\0';
   priv->charset = QCS_DEFAULT;
}

// FIXME: this is not very efficient with the array offsets...
QoreString::QoreString(const char* str) : priv(new qore_string_private) {
   priv->len = 0;
   priv->allocated = STR_CLASS_BLOCK;
   priv->buf = (char*)malloc(priv->allocated * sizeof(char));
   if (str) {
      while (str[priv->len]) {
	 priv->check_char(priv->len);
	 priv->buf[priv->len] = str[priv->len];
	 priv->len++;
      }
      priv->check_char(priv->len);
      priv->buf[priv->len] = '\0';
   }
   else
      priv->buf[0] = '\0';
   priv->charset = QCS_DEFAULT;
}

// FIXME: this is not very efficient with the array offsets...
QoreString::QoreString(const char* str, const QoreEncoding* new_qorecharset) : priv(new qore_string_private) {
   priv->len = 0;
   priv->allocated = STR_CLASS_BLOCK;
   priv->buf = (char*)malloc(priv->allocated * sizeof(char));
   if (str) {
      while (str[priv->len]) {
	 priv->check_char(priv->len);
	 priv->buf[priv->len] = str[priv->len];
	 priv->len++;
      }
      priv->check_char(priv->len);
      priv->buf[priv->len] = '\0';
   }
   else
      priv->buf[0] = '\0';
   priv->charset = new_qorecharset;
}

QoreString::QoreString(const std::string& str, const QoreEncoding* new_encoding) : priv(new qore_string_private) {
   priv->allocated = str.size() + 1 + STR_CLASS_BLOCK;
   priv->buf = (char*)malloc(priv->allocated * sizeof(char));
   memcpy(priv->buf, str.c_str(), str.size() + 1);
   priv->len = str.size();
   priv->charset = new_encoding;
}

QoreString::QoreString(const QoreEncoding* new_qorecharset) : priv(new qore_string_private) {
   priv->len = 0;
   priv->allocated = STR_CLASS_BLOCK;
   priv->buf = (char*)malloc(priv->allocated * sizeof(char));
   priv->buf[0] = '\0';
   priv->charset = new_qorecharset;
}

QoreString::QoreString(const char* str, qore_size_t size, const QoreEncoding* new_qorecharset) : priv(new qore_string_private) {
   priv->len = size;
   priv->allocated = size + STR_CLASS_EXTRA;
   priv->buf = (char*)malloc(sizeof(char) * priv->allocated);
   memcpy(priv->buf, str, size);
   priv->buf[size] = '\0';
   priv->charset = new_qorecharset;
}

QoreString::QoreString(const QoreString* str) : priv(new qore_string_private(*(str->priv))) {
}

QoreString::QoreString(const QoreString& str) : priv(new qore_string_private(*(str.priv))) {
}

QoreString::QoreString(const QoreString* str, qore_size_t size) : priv(new qore_string_private) {
   if (size >= str->priv->len)
      size = str->priv->len;
   priv->len = size;
   priv->allocated = size + STR_CLASS_EXTRA;
   priv->buf = (char*)malloc(sizeof(char) * priv->allocated);
   if (size)
      memcpy(priv->buf, str->priv->buf, size);
   priv->buf[size] = '\0';
   priv->charset = str->priv->charset;
}

QoreString::QoreString(char c) : priv(new qore_string_private) {
   priv->len = 1;
   priv->allocated = STR_CLASS_BLOCK;
   priv->buf = (char*)malloc(sizeof(char) * priv->allocated);
   priv->buf[0] = c;
   priv->buf[1] = '\0';
   priv->charset = QCS_DEFAULT;
}

QoreString::QoreString(int64 i) : priv(new qore_string_private) {
   priv->allocated = MAX_BIGINT_STRING_LEN + 1;
   priv->buf = (char*)malloc(sizeof(char) * priv->allocated);
   priv->len = ::snprintf(priv->buf, MAX_BIGINT_STRING_LEN, QLLD, i);
   // terminate string just in case
   priv->buf[MAX_BIGINT_STRING_LEN] = '\0';
   priv->charset = QCS_DEFAULT;
}

QoreString::QoreString(bool b) : priv(new qore_string_private) {
   priv->allocated = 2;
   priv->buf = (char*)malloc(sizeof(char) * priv->allocated);
   priv->buf[0] = b ? '1' : '0';
   priv->buf[1] = 0;
   priv->len = 1;
   priv->charset = QCS_DEFAULT;
}

QoreString::QoreString(double f) : priv(new qore_string_private) {
   priv->allocated = MAX_FLOAT_STRING_LEN + 1;
   priv->buf = (char*)malloc(sizeof(char) * priv->allocated);
   priv->len = ::snprintf(priv->buf, MAX_FLOAT_STRING_LEN, "%.9g", f);
   // snprintf() always terminates the string
   priv->charset = QCS_DEFAULT;
   // issue 1556: external modules that call setlocale() can change
   // the decimal point character used here from '.' to ','
   q_fix_decimal(this);
}

QoreString::QoreString(const DateTime *d) : priv(new qore_string_private) {
   priv->allocated = 15;
   priv->buf = (char*)malloc(sizeof(char) * priv->allocated);

   qore_tm info;
   d->getInfo(info);
   priv->len = ::sprintf(priv->buf, "%04d%02d%02d%02d%02d%02d", info.year, info.month,
			 info.day, info.hour, info.minute, info.second);
   priv->charset = QCS_DEFAULT;
}

QoreString::QoreString(const BinaryNode *b) : priv(new qore_string_private) {
   priv->allocated = b->size() + (b->size() * 4) / 10 + 10; // estimate for base64 encoding
   priv->buf = (char*)malloc(sizeof(char) * priv->allocated);
   priv->len = 0;
   priv->charset = QCS_DEFAULT;
   concatBase64(b, -1);
}

QoreString::QoreString(const BinaryNode *b, qore_size_t maxlinelen) : priv(new qore_string_private) {
   priv->allocated = b->size() + (b->size() * 4) / 10 + 10; // estimate for base64 encoding
   priv->buf = (char*)malloc(sizeof(char) * priv->allocated);
   priv->len = 0;
   priv->charset = QCS_DEFAULT;
   concatBase64(b, maxlinelen);
}

QoreString::QoreString(char* nbuf, qore_size_t nlen, qore_size_t nallocated, const QoreEncoding* enc) : priv(new qore_string_private) {
   assert(nallocated >= nlen);
   priv->buf = nbuf;
   priv->len = nlen;
   priv->allocated = nallocated;
   if (nallocated == nlen) {
      priv->check_char(nlen);
      priv->buf[nlen] = '\0';
   }
   priv->charset = enc;
}

// FIXME: remove this function
// private constructor
QoreString::QoreString(struct qore_string_private *p) : priv(p) {
}

QoreString::~QoreString() {
   delete priv;
}

// NULL values sorted at end
int QoreString::compare(const QoreString* str) const {
   // empty strings are always equal even if the character encoding is different
   if (!priv->len) {
      if (!str->priv->len)
         return 0;
      return 1;
   }

   if (str->priv->getEncoding() != priv->getEncoding())
      return 1;


   int rc = memcmp(priv->buf, str->priv->buf, QORE_MIN(priv->len, str->size()));
   if (rc < 0)
      return -1;
   return !rc ? 0 : 1;
}

int QoreString::compare(const char* str) const {
   // empty strings are always equal even if the character encoding is different
   if (!priv->len) {
      if (!str)
         return 0;
      else
         return 1;
   }

   return strcmp(priv->buf, str);
}

bool QoreString::equal(const QoreString& str) const {
   if (priv->len != str.priv->len)
      return false;

   // empty strings are always equal even if the character encoding is different
   if (!priv->len)
      return true;

   if (priv->getEncoding() != str.priv->getEncoding())
      return false;

   return !memcmp(priv->buf, str.priv->buf, priv->len);
}

bool QoreString::equalPartial(const QoreString& str) const {
   // empty strings are always equal even if the character encoding is different
   if (!priv->len) {
      if (!str.priv->len)
         return true;
      return false;
   }
   if (!str.priv->len)
      return false;

   if (priv->getEncoding() != str.priv->getEncoding())
      return false;

   if (priv->len < str.priv->len)
      return false;

   return !memcmp(priv->buf, str.priv->buf, str.priv->len);
}

bool QoreString::equal(const char* str) const {
   // empty strings are always equal even if the character encoding is different
   if (!str || !str[0]) {
      if (!priv->len)
         return true;
      return false;
   }
   if (!priv->len)
      return false;

   return !strcmp(priv->buf, str);
}

bool QoreString::equalPartial(const char* str) const {
   // empty strings are always equal even if the character encoding is different
   if (!str || !str[0]) {
      if (!priv->len)
         return true;
      return false;
   }
   if (!priv->len)
      return false;

   return !strncmp(priv->buf, str, ::strlen(str));
}

bool QoreString::equalSoft(const QoreString& str, ExceptionSink* xsink) const {
   // empty strings are always equal even if the character encoding is different
   if (!priv->len) {
      if (!str.priv->len)
         return true;
      return false;
   }
   if (!str.priv->len)
      return false;

   // if the encodings are equal or equivalent and the lenghts are different then the strings are not equal
   if ((priv->getEncoding() == str.priv->getEncoding() || (!priv->getEncoding()->isMultiByte() && !str.priv->getEncoding()->isMultiByte())) && priv->len != str.priv->len)
      return false;

   // optionally convert both strings to the same encoding
   const QoreEncoding* enc = priv->getEncoding();
   if (enc == QCS_USASCII)
      enc = str.priv->getEncoding();
   TempEncodingHelper a(this, enc, xsink);
   if (xsink && *xsink)
      return false;
   TempEncodingHelper b(str, enc, xsink);
   if (xsink && *xsink)
      return false;

   if (a->size() != b->size())
      return false;

   return !memcmp(a->getBuffer(), b->getBuffer(), a->size());
}

bool QoreString::equalPartialSoft(const QoreString& str, ExceptionSink* xsink) const {
   // empty strings are always equal even if the character encoding is different
   if (!priv->len) {
      if (!str.priv->len)
         return true;
      return false;
   }
   if (!str.priv->len)
      return false;

   // if the encodings are equal or equivalent and the lengths are different then the strings are not equal
   if ((priv->getEncoding() == str.priv->getEncoding() || (!priv->getEncoding()->isMultiByte() && !str.priv->getEncoding()->isMultiByte())) && priv->len < str.priv->len)
      return false;

   // optionally convert both strings to the same encoding
   const QoreEncoding* enc = priv->getEncoding();
   if (enc == QCS_USASCII)
      enc = str.priv->getEncoding();
   TempEncodingHelper a(this, enc, xsink);
   if (xsink && *xsink)
      return false;
   TempEncodingHelper b(str, enc, xsink);
   if (xsink && *xsink)
      return false;

   if (a->size() < b->size())
      return false;

   return !memcmp(a->getBuffer(), b->getBuffer(), b->size());
}

bool QoreString::equalPartialPath(const QoreString& str, ExceptionSink* xsink) const {
   // empty strings are always equal even if the character encoding is different
   if (!priv->len) {
      if (!str.priv->len)
         return true;
      return false;
   }
   if (!str.priv->len)
      return false;

   // if the encodings are equal or equivalent and the lenghts are different then the strings are not equal
   if ((priv->getEncoding() == str.priv->getEncoding() || (!priv->getEncoding()->isMultiByte() && !str.priv->getEncoding()->isMultiByte())) && priv->len < str.priv->len)
      return false;

   // optionally convert both strings to the same encoding
   const QoreEncoding* enc = priv->getEncoding();
   if (enc == QCS_USASCII)
      enc = str.priv->getEncoding();
   TempEncodingHelper a(this, enc, xsink);
   if (xsink && *xsink)
      return false;
   TempEncodingHelper b(str, enc, xsink);
   if (xsink && *xsink)
      return false;

   if (a->size() < b->size())
      return false;

   int rc = !memcmp(a->getBuffer(), b->getBuffer(), b->size());
   if (!rc)
      return false;

   if (a->priv->len == b->priv->len)
      return true;

   // NOTE: does not work with UTF-16 or other non-ASCII-compatible multi-byte encodings
   if (a->priv->buf[b->priv->len] == '/' || a->priv->buf[b->priv->len] == '?')
      return true;
   return false;
}

void QoreString::terminate(qore_size_t size) {
   if (size > priv->len)
      priv->check_char(size);
   priv->len = size;
   priv->buf[size] = '\0';
}

void QoreString::reserve(qore_size_t size) {
   // leave room for the terminator char '\0'
   ++size;
   if (size > priv->len)
      priv->check_char(size);
}

void QoreString::take(char* str) {
   if (priv->buf)
      free(priv->buf);
   priv->buf = str;
   if (str) {
      priv->len = ::strlen(str);
      priv->allocated = priv->len + 1;
   }
   else {
      priv->allocated = 0;
      priv->len = 0;
   }
}

void QoreString::take(char* str, const QoreEncoding* new_qorecharset) {
   take(str);
   priv->charset = new_qorecharset;
}

void QoreString::take(char* str, qore_size_t size) {
   if (priv->buf)
      free(priv->buf);
   priv->buf = str;
   priv->len = size;
   priv->allocated = size + 1;
}

void QoreString::take(char* str, qore_size_t size, const QoreEncoding* enc) {
   if (priv->buf)
      free(priv->buf);
   priv->buf = str;
   priv->len = size;
   priv->allocated = size + 1;

   priv->charset = enc;
}

void QoreString::takeAndTerminate(char* str, qore_size_t size) {
   if (priv->buf)
      free(priv->buf);
   priv->buf = str;
   priv->len = size;
   priv->allocated = size + 1;
   priv->check_char(size);
   priv->buf[size] = '\0';
}

void QoreString::takeAndTerminate(char* str, qore_size_t size, const QoreEncoding* enc) {
   takeAndTerminate(str, size);
   priv->charset = enc;
}

// NOTE: could be dangerous if we refer to the priv->buffer after this
// call and it's NULL (the only way the priv->buffer can become NULL)
char* QoreString::giveBuffer() {
   char* rv = priv->buf;
   priv->buf = 0;
   priv->len = 0;
   priv->allocated = 0;
   // reset character set, just in case the string will be reused
   // (normally not after this call)
   priv->charset = QCS_DEFAULT;
   return rv;
}

void QoreString::clear() {
   if (priv->allocated) {
      priv->len = 0;
      priv->buf[0] = '\0';
   }
}

void QoreString::reset() {
   char* b = giveBuffer();
   if (b)
      free(b);
   priv->check_char(0);
   priv->buf[0] = '\0';
}

void QoreString::set(const char* str, const QoreEncoding* new_qorecharset) {
   priv->len = 0;
   priv->charset = new_qorecharset;
   if (!str) {
      if (priv->buf)
	 priv->buf[0] = '\0';
   }
   else
      concat(str);
}

void QoreString::set(const QoreString* str) {
   priv->len = str->priv->len;
   priv->charset = str->priv->getEncoding();
   allocate(str->priv->len + 1);
   // copy string and trailing null
   memcpy(priv->buf, str->priv->buf, str->priv->len + 1);
}

void QoreString::set(const QoreString& str) {
   set(&str);
}

void QoreString::set(const std::string& str, const QoreEncoding* ne) {
   priv->len = str.size();
   priv->charset = ne;
   allocate(priv->len + 1);
   // copy string and trailing null
   memcpy(priv->buf, str.c_str(), priv->len + 1);
}

void QoreString::set(char* nbuf, size_t nlen, size_t nallocated, const QoreEncoding* enc) {
   if (priv->buf)
      free(priv->buf);

   assert(nallocated >= nlen);
   priv->buf = nbuf;
   priv->len = nlen;
   priv->allocated = nallocated;
   if (nallocated == nlen) {
      priv->check_char(nlen);
      priv->buf[nlen] = '\0';
   }
   priv->charset = enc;
}

void QoreString::setEncoding(const QoreEncoding* new_encoding) {
   priv->charset = new_encoding;
}

void QoreString::replaceAll(const char* old_str, const char* new_str) {
   assert(old_str);
   assert(new_str);

   int old_len = ::strlen(old_str);
   int new_len = ::strlen(new_str);

   qore_offset_t start = 0;
   while (true) {
      qore_offset_t i = bindex(old_str, start);
      if (i < 0)
         break;

      replace(i, old_len, new_str);
      start = i + new_len;
   }
}

void QoreString::replace(qore_size_t offset, qore_size_t dlen, const char* str) {
   if (str && str[0])
      splice_simple(offset, dlen, str, ::strlen(str));
   else
      splice_simple(offset, dlen);
}

void QoreString::replace(qore_size_t offset, qore_size_t dlen, const QoreString* str) {
   if (str->getEncoding() != priv->getEncoding())
      return;

   if (str && str->strlen())
      splice_simple(offset, dlen, str->getBuffer(), str->strlen());
   else
      splice_simple(offset, dlen);
}

void QoreString::replace(qore_size_t offset, qore_size_t dlen, const QoreString* str, ExceptionSink* xsink) {
   if (str && str->strlen()) {
      TempEncodingHelper tmp(str, priv->getEncoding(), xsink);
      if (!tmp)
         return;
      splice_simple(offset, dlen, tmp->getBuffer(), tmp->strlen());
      return;
   }

   splice_simple(offset, dlen);
}

void QoreString::replaceChar(qore_size_t offset, char c) {
   if (priv->len <= offset)
      return;

   priv->buf[offset] = c;
}

void QoreString::splice(qore_offset_t offset, ExceptionSink* xsink) {
   if (!priv->getEncoding()->isMultiByte()) {
      qore_size_t n_offset = priv->check_offset(offset);
      if (n_offset == priv->len)
         return;

      splice_simple(n_offset, priv->len - n_offset, 0);
      return;
   }
   splice_complex(offset, xsink, 0);
}

void QoreString::splice(qore_offset_t offset, qore_offset_t num, ExceptionSink* xsink) {
   if (!priv->getEncoding()->isMultiByte()) {
      qore_size_t n_offset, n_num;
      priv->check_offset(offset, num, n_offset, n_num);
      if (n_offset == priv->len || !n_num)
         return;

      splice_simple(n_offset, n_num, 0);
      return;
   }
   splice_complex(offset, num, xsink, 0);
}

void QoreString::splice(qore_offset_t offset, qore_offset_t num, const QoreString& str, ExceptionSink* xsink) {
   TempEncodingHelper tmp(&str, priv->getEncoding(), xsink);
   if (!tmp)
       return;

   if (priv->getEncoding()->isMultiByte()) {
      splice_complex(offset, num, *tmp, xsink, 0);
      return;
   }

   qore_size_t n_offset, n_num;
   priv->check_offset(offset, num, n_offset, n_num);
   if (n_offset == priv->len) {
      if (!tmp->priv->len)
         return;
      n_num = 0;
   }
   splice_simple(n_offset, n_num, tmp->getBuffer(), tmp->strlen(), 0);
}

void QoreString::splice(qore_offset_t offset, qore_offset_t num, const AbstractQoreNode *strn, ExceptionSink* xsink) {
   QoreStringNodeValueHelper sv(strn);

   if (!sv->strlen()) {
      splice(offset, num, xsink);
      return;
   }

   splice(offset, num, **sv, xsink);
}

QoreString* QoreString::extract(qore_offset_t offset, ExceptionSink* xsink) {
   QoreString* str = new QoreString(priv->getEncoding());
   if (!priv->getEncoding()->isMultiByte()) {
      qore_size_t n_offset = priv->check_offset(offset);
      if (n_offset != priv->len)
         splice_simple(n_offset, priv->len - n_offset, str);
   }
   else
      splice_complex(offset, xsink, str);
   return str;
}

QoreString* QoreString::extract(qore_offset_t offset, qore_offset_t num, ExceptionSink* xsink) {
   QoreString* str = new QoreString(priv->getEncoding());
   if (!priv->getEncoding()->isMultiByte()) {
      qore_size_t n_offset, n_num;
      priv->check_offset(offset, num, n_offset, n_num);
      if (n_offset != priv->len && n_num)
         splice_simple(n_offset, n_num, str);
   }
   else
      splice_complex(offset, num, xsink, str);
   return str;
}

QoreString* QoreString::extract(qore_offset_t offset, qore_offset_t num, const AbstractQoreNode *strn, ExceptionSink* xsink) {
   QoreStringNodeValueHelper sv(strn);

   if (!sv->strlen())
      return extract(offset, num, xsink);

   const QoreStringNode *str = *sv;
   TempEncodingHelper tmp(str, priv->getEncoding(), xsink);
   if (!tmp)
       return 0;

   QoreString* rv = new QoreString(priv->getEncoding());
   if (!priv->getEncoding()->isMultiByte()) {
      qore_size_t n_offset, n_num;
      priv->check_offset(offset, num, n_offset, n_num);
      if (n_offset == priv->len) {
         if (!tmp->priv->len)
            return rv;
         n_num = 0;
      }
      splice_simple(n_offset, n_num, tmp->getBuffer(), tmp->strlen(), rv);
   }
   else
      splice_complex(offset, num, *tmp, xsink, rv);
   return rv;
}

// removes a single trailing newline
qore_size_t QoreString::chomp() {
   if (priv->len && priv->buf[priv->len - 1] == '\n') {
      terminate(priv->len - 1);
      if (priv->len && priv->buf[priv->len - 1] == '\r') {
         terminate(priv->len - 1);
         return 2;
      }
      return 1;
   }
   return 0;
}

QoreString* QoreString::convertEncoding(const QoreEncoding* nccs, ExceptionSink* xsink) const {
   printd(5, "QoreString::convertEncoding() from \"%s\" to \"%s\"\n", priv->getEncoding()->getCode(), nccs->getCode());

   if (nccs == priv->getEncoding())
      return copy();

   std::unique_ptr<QoreString> targ(new QoreString(nccs));

   if (priv->len) {
      if (qore_string_private::convert_encoding_intern(priv->buf, priv->len, priv->getEncoding(), *targ, nccs, xsink))
         return 0;

      // remove BOM bytes (invisible non-breaking space) at the beginning of a string when converting to UTF-8
      if (nccs == QCS_UTF8 && targ->priv->len >= 3 && (unsigned char)targ->priv->buf[0] == 0xef && (unsigned char)targ->priv->buf[1] == 0xbb && (unsigned char)targ->priv->buf[2] == 0xbf) {
         printd(0, "QoreString::convertEncoding() found BOM, removing\n");
         targ->splice_simple(0, 3);
      }
   }

   return targ.release();
}

static void base64_concat(QoreString& str, unsigned char c, qore_size_t& linelen, qore_size_t maxlinelen) {
   str.concat(table64[c]);

   ++linelen;
   if (maxlinelen > 0 && linelen == maxlinelen) {
      str.concat("\r\n");
      linelen = 0;
   }
}

// endian-agnostic binary object -> base64 string function
// NOTE: not very high-performance - high-performance versions
//       would likely be endian-aware and operate directly on 32-bit words
void QoreString::concatBase64(const char* bbuf, qore_size_t size, qore_size_t maxlinelen) {
   //printf("bbuf=%p, size=" QSD "\n", bbuf, size);
   if (!size)
      return;

   qore_size_t linelen = 0;

   unsigned char* p = (unsigned char*)bbuf;
   unsigned char* endbuf = p + size;
   while (p < endbuf) {
      // get 6 bits of data
      unsigned char c = p[0] >> 2;

      // byte 1: concat 1st 6-bit value
      base64_concat(*this, c, linelen, maxlinelen);

      // byte 1: use remaining 2 bits in low order position
      c = (p[0] & 3) << 4;

      // check end
      if ((endbuf - p) == 1) {
	 base64_concat(*this, c, linelen, maxlinelen);
	 concat("==");
	 break;
      }

      // byte 2: get 4 bits to make next 6-bit unit
      c |= p[1] >> 4;

      // concat 2nd 6-bit value
      base64_concat(*this, c, linelen, maxlinelen);

      // byte 2: get 4 low bits
      c = (p[1] & 15) << 2;

      if ((endbuf - p) == 2) {
	 base64_concat(*this, c, linelen, maxlinelen);
	 concat('=');
	 break;
      }

      // byte 3: get high 2 bits to make next 6-bit unit
      c |= p[2] >> 6;

      // concat 3rd 6-bit value
      base64_concat(*this, c, linelen, maxlinelen);

      // byte 3: concat final 6 bits
      base64_concat(*this, p[2] & 63, linelen, maxlinelen);
      p += 3;
   }
}

void QoreString::concatBase64(const BinaryNode *b, qore_size_t maxlinelen) {
   concatBase64((char*)b->getPtr(), b->size(), maxlinelen);
}

void QoreString::concatBase64(const QoreString* str, qore_size_t maxlinelen) {
   concatBase64(str->priv->buf, str->priv->len, maxlinelen);
}

void QoreString::concatBase64(const BinaryNode *b) {
   concatBase64((char*)b->getPtr(), b->size(), -1);
}

void QoreString::concatBase64(const QoreString* str) {
   concatBase64(str->priv->buf, str->priv->len, -1);
}

void QoreString::concatBase64(const char* bbuf, qore_size_t size) {
   concatBase64(bbuf, size, -1);
}

#define DO_HEX_CHAR(b) ((b) + (((b) > 9) ? 87 : 48))

void QoreString::concatHex(const char* binbuf, qore_size_t size) {
   //printf("priv->buf=%p, size=" QSD "\n", binbuf, size);
   if (!size)
      return;

   unsigned char* p = (unsigned char*)binbuf;
   unsigned char* endbuf = p + size;
   while (p < endbuf) {
      char c = (*p & 0xf0) >> 4;
      concat(DO_HEX_CHAR(c));
      c = *p & 0x0f;
      concat(DO_HEX_CHAR(c));
      p++;
   }
}

int QoreString::concatEncode(ExceptionSink* xsink, const QoreString& str, unsigned code) {
   return priv->concatEncode(xsink, str, code);
}

int QoreString::concatDecode(ExceptionSink* xsink, const QoreString& str, unsigned code) {
   return priv->concatDecode(xsink, str, code);
}

void QoreString::concatAndHTMLEncode(const QoreString* str, ExceptionSink* xsink) {
   priv->concatEncode(xsink, str, CE_HTML);
}

// FIXME: this is slow, each concatenated character gets terminated as well
void QoreString::concatAndHTMLEncode(const char* str) {
   // if it's not a null string
   if (str) {
      qore_size_t i = 0;
      // iterate through new string
      while (str[i]) {
	 // concatenate translated character
	 qore_size_t j;
	 for (j = 0; j < NUM_HTML_CODES; j++)
	    if (str[i] == html_codes[j].symbol) {
	       concat(html_codes[j].code);
	       break;
	    }
	 // otherwise concatenate untranslated symbol
	 if (j == NUM_HTML_CODES)
	    concat(str[i]);
	 ++i;
      }
   }
}

void QoreString::concatAndHTMLDecode(const QoreString* str) {
   if (!str || !str->priv->len)
      return;

   concatAndHTMLDecode(str->getBuffer(), str->priv->len);
}

void QoreString::concatAndHTMLDecode(const char* str) {
   if (str)
      concatAndHTMLDecode(str, ::strlen(str));
}

// FIXME: this is slow, each concatenated character gets terminated as well
void QoreString::concatAndHTMLDecode(const char* str, size_t slen) {
   if (!slen)
      return;

   allocate(priv->len + slen); // avoid reallocations within the loop

   qore_size_t i = 0;
   while (str[i]) {
      if (str[i] != '&') {
	 concat(str[i++]);
	 continue;
      }

      // concatenate translated character
      const char* s = str + i;
      // check for unicode character references
      if (*(s + 1) == '#') {
	 s += 2;
	 // find end of character sequence
	 const char* e = strchr(s, ';');
	 // if not found or the number is too big, then don't try to decode it
	 if (e && (e - s) < 8) {
	    unsigned code;
	    if (*s == 'x')
	       code = strtoul(s + 1, 0, 16);
	    else
	       code = strtoul(s, 0, 10);

	    if (!concatUnicode(code)) {
	       i = e - str + 1;
	       continue;
	    }
	    // error occurred, so back out
	    s -= 2;
	 }
      }
      else if (isascii(*(s + 1))) {
	 s += 1;
	 // find end of character sequence
	 const char* e = strchr(s, ';');
	 // if not found or the number is too big, then don't try to decode it
	 if (e && (e - s) < 16) {
	    // find unicode code point
	    std::string entity(s, e - s);
	    //printd(0, "entity: %s\n", entity.c_str());
	    emap_t::iterator it = emap.find(entity);
	    if (it != emap.end() && !concatUnicode(it->second)) {
	       i = e - str + 1;
	       continue;
	    }
	 }
	 // not found or error occurred concatenating char, so back out
	 s -= 1;
      }
      concat(str[i++]);
   }
}

// deprecated, does not support RFC-3986
void QoreString::concatDecodeUrl(const char* url) {
  if (!url)
      return;

   while (*url) {
      if (*url == '%' && isxdigit(*(url + 1)) && isxdigit(*(url + 2))) {
	 char x[3] = { *(url + 1), *(url + 2), '\0' };
	 char code = strtol(x, 0, 16);
	 concat(code);
	 url += 3;
	 continue;
      }
      concat(*url);
      ++url;
   }
}

// assume encoding according to http://tools.ietf.org/html/rfc3986#section-2.1
int QoreString::concatDecodeUrl(const QoreString& url_str, ExceptionSink* xsink) {
   assert(xsink);
   TempEncodingHelper str(url_str, priv->getEncoding(), xsink);
   if (*xsink)
      return -1;

   return priv->concatDecodeUriIntern(xsink, *str->priv);
}

// assume encoding according to http://tools.ietf.org/html/rfc3986#section-2.1
int QoreString::concatEncodeUrl(ExceptionSink* xsink, const QoreString& url, bool encode_all) {
   assert(xsink);
   if (!url.size())
      return 0;

   TempEncodingHelper str(url, QCS_UTF8, xsink);
   if (*xsink)
      return -1;

   const unsigned char* p = (const unsigned char*)str->getBuffer();
   while (*p) {
      if ((*p) == '%')
	 concat("%25");
      else if ((*p) == ' ')
	 concat("%20");
      else if (*p > 127) {
	 qore_size_t len = q_UTF8_get_char_len((const char*)p, str->size() - ((const char*)p - str->getBuffer()));
	 if (len <= 0) {
	    xsink->raiseException("INVALID-ENCODING", "invalid UTF-8 encoding found in string");
	    return -1;
	 }
	 // add UTF-8 percent-encoded characters
	 for (qore_size_t i = 0; i < len; ++i)
	    sprintf("%%%X", (unsigned)p[i]);
	 p += len;
	 continue;
      }
      else if (encode_all && url_reserved.find(*p) != url_reserved.end()) {
	 sprintf("%%%X", (unsigned)*p);
      }
      else
	 concat(*p);

      ++p;
   }

   return 0;
}

int QoreString::concatEncodeUriRequest(ExceptionSink* xsink, const QoreString& url) {
   assert(xsink);
   if (!url.size())
      return 0;

   TempEncodingHelper str(url, QCS_UTF8, xsink);
   if (*xsink)
      return -1;

   return priv->concatEncodeUriRequest(xsink, *url.priv);
}

int QoreString::concatDecodeUriRequest(const QoreString& url_str, ExceptionSink* xsink) {
   assert(xsink);
   TempEncodingHelper str(url_str, priv->getEncoding(), xsink);
   if (*xsink)
      return -1;

   return priv->concatDecodeUriIntern(xsink, *str->priv, true);
}

// return 0 for success
int QoreString::vsprintf(const char* fmt, va_list args) {
   return priv->vsprintf(fmt, args);
}

void QoreString::concat(const char* str) {
   priv->concat(str);
}

void QoreString::concat(const std::string& str) {
   priv->check_char(priv->len + str.size());
   memcpy(priv->buf + priv->len, str.c_str(), str.size());
   priv->len += str.size();
   priv->buf[priv->len] = '\0';
}

void QoreString::concat(const char* str, qore_size_t size) {
   priv->check_char(priv->len + size);
   memcpy(priv->buf + priv->len, str, size);
   priv->len += size;
   priv->buf[priv->len] = '\0';
}

void QoreString::concat(const QoreString* str) {
   if (str)
      priv->concat(str->priv);
}

/*
void QoreString::concat(const QoreString* str, qore_size_t size) {
   // if it's not a null string
   if (str && str->priv->len) {
      // if priv->buffer needs to be resized
      priv->check_char(str->priv->len + size);
      // concatenate new string
      memcpy(priv->buf + priv->len, str->priv->buf, size);
      priv->len += size;
      priv->buf[priv->len] = '\0';
   }
}
*/

void QoreString::concat(const QoreString* str, ExceptionSink* xsink) {
   priv->concat(str, xsink);
}

void QoreString::concat(const QoreString* str, qore_size_t size, ExceptionSink* xsink) {
   assert(xsink);
   // if it's not a null string
   if (str && str->priv->len) {
      TempEncodingHelper cstr(str, priv->getEncoding(), xsink);
      if (*xsink)
         return;

      // adjust size for number of characters if this is a multi-byte character set
      if (priv->getEncoding()->isMultiByte()) {
         size = priv->getEncoding()->getByteLen(cstr->priv->buf, cstr->priv->buf + cstr->priv->len, size, xsink);
         if (*xsink)
            return;
      }

      // if priv->buffer needs to be resized
      priv->check_char(cstr->priv->len + size + STR_CLASS_EXTRA);
      // concatenate new string
      memcpy(priv->buf + priv->len, cstr->priv->buf, size);
      priv->len += size;
      priv->buf[priv->len] = '\0';
   }
}

int QoreString::concat(const QoreString& str, qore_offset_t pos, ExceptionSink* xsink) {
   assert(xsink);
   if (str.empty())
      return 0;

   TempEncodingHelper cstr(str, priv->getEncoding(), xsink);
   if (*xsink)
      return -1;

   return priv->concat(*(cstr->priv), pos, xsink);
}

int QoreString::concat(const QoreString& str, qore_offset_t pos, qore_offset_t len, ExceptionSink* xsink) {
   assert(xsink);
   if (str.empty() || !len)
      return 0;

   TempEncodingHelper cstr(str, priv->getEncoding(), xsink);
   if (*xsink)
      return -1;

   return priv->concat(*(cstr->priv), pos, len, xsink);
}

void QoreString::concat(char c) {
   priv->concat(c);
}

int QoreString::vsnprintf(size_t size, const char* fmt, va_list args) {
   // ensure minimum space is free
   if ((priv->allocated - priv->len) < (unsigned)size) {
      priv->allocated += (size + STR_CLASS_EXTRA);
      // resize priv->buffer
      priv->buf = (char*)realloc(priv->buf, priv->allocated * sizeof(char));
   }
   // copy formatted string to priv->buffer
   int i = ::vsnprintf(priv->buf + priv->len, size, fmt, args);
   priv->len += i;
   return i;
}

// returns 0 for success
int QoreString::sprintf(const char* fmt, ...) {
   va_list args;
   while (true) {
      va_start(args, fmt);
      int rc = vsprintf(fmt, args);
      va_end(args);
      if (!rc)
         break;
   }
   return 0;
}

int QoreString::snprintf(size_t size, const char* fmt, ...) {
   va_list args;
   va_start(args, fmt);
   int i = vsnprintf(size, fmt, args);
   va_end(args);
   return i;
}

int QoreString::substr_simple(QoreString* ns, qore_offset_t offset, qore_offset_t length) const {
   printd(5, "QoreString::substr_simple(offset=" QSD ", length=" QSD ") string=\"%s\" (this=%p priv->len=" QSD ")\n",
	  offset, length, priv->buf, this, priv->len);

   qore_size_t n_offset;
   if (offset < 0)
      n_offset = priv->len + offset;
   else
      n_offset = offset;
   if (n_offset >= priv->len)  // if offset outside of string, return nothing
      return -1;

   qore_size_t n_length;
   if (length < 0) {
      length = priv->len - n_offset + length;
      if (length < 0)
         n_length = 0;
      else
         n_length = length;
   }
   else if ((qore_size_t)length > (priv->len - n_offset))
      n_length = priv->len - n_offset;
   else
      n_length = length;

   ns->concat(priv->buf + n_offset, n_length);
   return 0;
}

int QoreString::substr_simple(QoreString* ns, qore_offset_t offset) const {
   printd(5, "QoreString::substr_simple(offset=" QSD ") string=\"%s\" (this=%p priv->len=" QSD ")\n",
	  offset, priv->buf, this, priv->len);

   qore_size_t n_offset;
   if (offset < 0)
      n_offset = priv->len + offset;
   else
      n_offset = offset;
   if (n_offset >= priv->len)  // if offset outside of string, return nothing
      return -1;

   // add length to ensure that the entire string is copied even if it has embedded nulls
   ns->concat(priv->buf + n_offset, priv->len - n_offset);
   return 0;
}

int QoreString::substr_complex(QoreString* ns, qore_offset_t offset, qore_offset_t length, ExceptionSink* xsink) const {
   assert(xsink);
   QORE_TRACE("QoreString::substr_complex(offset, length)");
   printd(5, "QoreString::substr_complex(offset=" QSD ", length=" QSD ") string=\"%s\" (this=%p priv->len=" QSD ")\n",
	  offset, length, priv->buf, this, priv->len);

   char* pend = priv->buf + priv->len;
   if (offset < 0) {
      int clength = priv->getEncoding()->getLength(priv->buf, pend, xsink);
      if (*xsink)
         return -1;

      offset = clength + offset;

      if ((offset < 0) || (offset >= clength))  // if offset outside of string, return nothing
         return -1;
   }

   qore_size_t start = priv->getEncoding()->getByteLen(priv->buf, pend, offset, xsink);
   if (*xsink)
      return -1;

   if (start == priv->len)
      return -1;

   if (length < 0) {
      length = priv->getEncoding()->getLength(priv->buf + start, pend, xsink) + length;
      if (*xsink)
         return -1;

      if (length < 0)
         length = 0;
   }
   qore_size_t end = priv->getEncoding()->getByteLen(priv->buf + start, pend, length, xsink);
   if (*xsink)
      return -1;

   ns->concat(priv->buf + start, end);
   return 0;
}

int QoreString::substr_complex(QoreString* ns, qore_offset_t offset, ExceptionSink* xsink) const {
   assert(xsink);
   //printd(5, "QoreString::substr_complex(offset=" QSD ") string=\"%s\" (this=%p priv->len=" QSD ")\n", offset, priv->buf, this, priv->len);
   char* pend = priv->buf + priv->len;
   if (offset < 0) {
      qore_size_t clength = priv->getEncoding()->getLength(priv->buf, pend, xsink);
      if (*xsink)
         return -1;

      offset = clength + offset;

      if ((offset < 0) || ((qore_size_t)offset >= clength)) {  // if offset outside of string, return nothing
         //printd(5, "this=%p, priv->len=" QSD ", offset=" QSD ", clength=" QSD ", priv->buf=%s\n", this, priv->len, offset, clength, priv->buf);
         return -1;
      }
   }

   qore_size_t start = priv->getEncoding()->getByteLen(priv->buf, pend, offset, xsink);
   if (*xsink)
      return -1;

   //printd(5, "offset=" QSD ", start=" QSD "\n", offset, start);
   if (start == priv->len) {
      //printd(5, "this=%p, priv->len=" QSD ", offset=" QSD ", priv->buf=%p, start=" QSD ", %s\n", this, priv->len, offset, priv->buf, start, priv->buf);
      return -1;
   }

   // calculate byte offset
   ns->concat(priv->buf + start, priv->len - start);
   return 0;
}

void QoreString::splice_simple(qore_size_t offset, qore_size_t num, QoreString* extract) {
   //printd(5, "splice_intern(offset=" QSD ", num=" QSD ", priv->len=" QSD ")\n", offset, num, priv->len);
   qore_size_t end;
   if (num > (priv->len - offset)) {
      end = priv->len;
      num = priv->len - offset;
   }
   else
      end = offset + num;

   // add to extract string if any
   if (extract && num)
      extract->concat(priv->buf + offset, num);

   // move down entries if necessary
   if (end != priv->len)
      memmove(priv->buf + offset, priv->buf + end, sizeof(char) * (priv->len - end));

   // calculate new length
   priv->len -= num;
   // set last entry to NULL
   priv->buf[priv->len] = '\0';
}

void QoreString::splice_simple(qore_size_t offset, qore_size_t num, const char* str, qore_size_t str_len, QoreString* extract) {
   //printd(5, "splice_intern(offset=" QSD ", num=" QSD ", priv->len=" QSD ")\n", offset, num, priv->len);

   qore_size_t end;
   if (num > (priv->len - offset)) {
      end = priv->len;
      num = priv->len - offset;
   }
   else
      end = offset + num;

   // add to extract string if any
   if (extract && num)
      extract->concat(priv->buf + offset, num);

   // get number of entries to insert
   if (str_len > num) { // make bigger
      qore_size_t ol = priv->len;
      priv->check_char(priv->len - num + str_len);
      // move trailing entries forward if necessary
      if (end != ol)
         memmove(priv->buf + (end - num + str_len), priv->buf + end, sizeof(char) * (ol - end));
   }
   else if (num > str_len) // make list smaller
      memmove(priv->buf + offset + str_len, priv->buf + offset + num, sizeof(char) * (priv->len - offset - str_len));

   memcpy(priv->buf + offset, str, str_len);

   // calculate new length
   priv->len = priv->len - num + str_len;
   // set last entry to NULL
   priv->buf[priv->len] = '\0';
}

void QoreString::splice_complex(qore_offset_t offset, ExceptionSink* xsink, QoreString* extract) {
   assert(xsink);
   // get length in chars
   qore_size_t clen = priv->getEncoding()->getLength(priv->buf, priv->buf + priv->len, xsink);
   if (*xsink)
      return;

   //printd(0, "splice_complex(offset=" QSD ") clen=" QSD "\n", offset, clen);
   if (offset < 0) {
      offset = clen + offset;
      if (offset < 0)
         offset = 0;
   }
   else if ((qore_size_t)offset >= clen)
      return;

   // calculate byte offset
   qore_size_t n_offset = offset ? priv->getEncoding()->getByteLen(priv->buf, priv->buf + priv->len, offset, xsink) : 0;
   if (*xsink)
      return;

   // add to extract string if any
   if (extract && n_offset < priv->len)
      extract->concat(priv->buf + n_offset);

   // truncate string at offset
   priv->len = n_offset;
   priv->buf[priv->len] = '\0';
}

void QoreString::splice_complex(qore_offset_t offset, qore_offset_t num, ExceptionSink* xsink, QoreString* extract) {
   assert(xsink);
   //printd(5, "splice_complex(offset=" QSD ", num=" QSD ", priv->len=" QSD ")\n", offset, num, priv->len);

   // get length in chars
   qore_size_t clen = priv->getEncoding()->getLength(priv->buf, priv->buf + priv->len, xsink);
   if (*xsink)
      return;

   if (offset < 0) {
      offset = clen + offset;
      if (offset < 0)
         offset = 0;
   }
   else if ((qore_size_t)offset >= clen)
      return;

   if (num < 0) {
      num = clen + num - offset;
      if (num < 0)
         num = 0;
   }

   qore_size_t end;
   if ((qore_size_t)num > (clen - offset)) {
      end = clen;
      num = clen - offset;
   }
   else
      end = offset + num;

   // get character positions
   offset = priv->getEncoding()->getByteLen(priv->buf, priv->buf + priv->len, offset, xsink);
   if (*xsink)
      return;

   end = priv->getEncoding()->getByteLen(priv->buf, priv->buf + priv->len, end, xsink);
   if (*xsink)
      return;

   num = priv->getEncoding()->getByteLen(priv->buf + offset, priv->buf + priv->len, num, xsink);
   if (*xsink)
      return;

   // add to extract string if any
   if (extract && num)
      extract->concat(priv->buf + offset, num);

   // move down entries if necessary
   if (end != priv->len)
      memmove(priv->buf + offset, priv->buf + end, sizeof(char) * (priv->len - end));

   // calculate new length
   priv->len -= num;

   // set last entry to NULL
   priv->buf[priv->len] = '\0';
}

void QoreString::splice_complex(qore_offset_t offset, qore_offset_t num, const QoreString* str, ExceptionSink* xsink, QoreString* extract) {
   assert(xsink);
   // get length in chars
   qore_size_t clen = priv->getEncoding()->getLength(priv->buf, priv->buf + priv->len, xsink);
   if (*xsink)
      return;

   //printd(5, "splice_complex(offset=" QSD ", num=" QSD ", str='%s', priv->len=" QSD ") clen=" QSD " priv->buf='%s'\n", offset, num, str->getBuffer(), priv->len, clen, priv->buf);

   if (offset >= (qore_offset_t)clen)
      offset = clen;
   else if (offset < 0) {
      offset = clen + offset;
      if (offset < 0)
         offset = 0;
   }

   if (num < 0) {
      num = clen + num - offset;
      if (num < 0)
         num = 0;
   }

   qore_size_t end;
   if ((qore_size_t)num > (clen - offset)) {
      end = clen;
      num = clen - offset;
   }
   else
      end = offset + num;

   // get character positions
   char* endp = priv->buf + priv->len;
   offset = priv->getEncoding()->getByteLen(priv->buf, endp, offset, xsink);
   if (*xsink)
      return;

   end = priv->getEncoding()->getByteLen(priv->buf, endp, end, xsink);
   if (*xsink)
      return;

   num = priv->getEncoding()->getByteLen(priv->buf + offset, endp, num, xsink);
   if (*xsink)
      return;

   // add to extract string if any
   if (extract && num)
      extract->concat(priv->buf + offset, num);

   //printd(5, "offset=" QSD ", end=" QSD ", num=" QSD "\n", offset, end, num);
   // get number of entries to insert
   if (str->priv->len > (qore_size_t)num) { // make bigger
      qore_size_t ol = priv->len;
      priv->check_char(priv->len - num + str->priv->len);
      // move trailing entries forward if necessary
      //printd(5, "priv->buf='%s'(" QSD "), str='%s'(" QSD "), end=" QSD ", num=" QSD ", newlen=" QSD "\n", priv->buf, ol, str->priv->buf, str->priv->len, end, num, priv->len);
      if (end != ol)
         memmove(priv->buf + (end - num + str->priv->len), priv->buf + end, ol - end);
   }
   else if ((qore_size_t)num > str->priv->len) // make string smaller
      memmove(priv->buf + offset + str->priv->len, priv->buf + offset + num, sizeof(char) * (priv->len - offset - str->priv->len));

   memcpy(priv->buf + offset, str->priv->buf, str->priv->len);

   // calculate new length
   priv->len = priv->len - num + str->priv->len;

   // set last entry to NULL
   priv->buf[priv->len] = '\0';
}

// NULL values sorted at end
int QoreString::compareSoft(const QoreString* str, ExceptionSink* xsink) const {
   // empty strings are always equal even if the character encoding is different
   if (!priv->len) {
      if (!str->priv->len)
         return 0;
      else
         return 1;
   }

   TempEncodingHelper t(str, priv->getEncoding(), xsink);
   if (xsink && *xsink)
      return 1;

   int rc = memcmp(priv->buf, t->priv->buf, QORE_MIN(priv->len, t->size()));
   if (rc < 0)
      return -1;
   return !rc ? 0 : 1;
}

void QoreString::concatEscape(const char* str, char c, char esc_char) {
   // if it's not a null string
   if (str) {
      qore_size_t i = 0;
      // iterate through new string
      while (str[i]) {
         if (str[i] == c || str[i] == esc_char) {
            // check for space in priv->buffer
            priv->check_char(priv->len + 1);
            priv->buf[priv->len++] = esc_char;
         }
         else
            priv->check_char(priv->len);
         // concatenate one character at a time
         priv->buf[priv->len++] = str[i++];
      }
      // see if priv->buffer needs to be resized for '\0'
      priv->check_char(priv->len);
      // terminate string
      priv->buf[priv->len] = '\0';
   }
}

void QoreString::concatEscape(const QoreString* str, char c, char esc_char, ExceptionSink* xsink) {
   // if it's not a null string
   if (str && str->priv->len) {
      TempEncodingHelper cstr(str, priv->getEncoding(), xsink);
      if (xsink && *xsink)
         return;

      // if priv->buffer needs to be resized
      priv->check_char(cstr->priv->len + priv->len);

      concatEscape(cstr->priv->buf, c, esc_char);
   }
}

QoreString* QoreString::substr(qore_offset_t offset, ExceptionSink* xsink) const {
   TempString str(new QoreString(priv->getEncoding()));

   int rc;
   if (!priv->getEncoding()->isMultiByte())
      rc = substr_simple(*str, offset);
   else
      rc = substr_complex(*str, offset, xsink);

   return rc ? 0 : str.release();
}

QoreString* QoreString::substr(qore_offset_t offset, qore_offset_t length, ExceptionSink* xsink) const {
   TempString str(new QoreString(priv->getEncoding()));

   int rc;
   if (!priv->getEncoding()->isMultiByte())
      rc = substr_simple(*str, offset, length);
   else
      rc = substr_complex(*str, offset, length, xsink);

   return rc ? 0 : str.release();
}

qore_size_t QoreString::length() const {
   if (priv->getEncoding()->isMultiByte() && priv->buf) {
      bool invalid;
      return priv->getEncoding()->getLength(priv->buf, priv->buf + priv->len, invalid);
   }
   return priv->len;
}

void QoreString::concat(const DateTime *d) {
   qore_tm info;
   d->getInfo(info);
   sprintf("%04d%02d%02d%02d%02d%02d", info.year, info.month, info.day, info.hour, info.minute, info.second);
}

void QoreString::concatISO8601DateTime(const DateTime *d) {
   qore_tm info;
   d->getInfo(currentTZ(), info);
   sprintf("%04d%02d%02dT%02d:%02d:%02d", info.year, info.month, info.day, info.hour, info.minute, info.second);
}

void QoreString::concatHex(const BinaryNode *b) {
   concatHex((char*)b->getPtr(), b->size());
}

void QoreString::concatHex(const QoreString* str) {
   concatHex(str->priv->buf, str->priv->len);
}

// endian-agnostic base64 string -> binary object function
BinaryNode *QoreString::parseBase64(ExceptionSink* xsink) const {
   return ::parseBase64(priv->buf, priv->len, xsink);
}

QoreString* QoreString::parseBase64ToString(const QoreEncoding* qe, ExceptionSink* xsink) const {
   SimpleRefHolder<BinaryNode> b(::parseBase64(priv->buf, priv->len, xsink));
   if (!b)
      return 0;

   if (b->empty())
      return new QoreStringNode;

   qore_string_private *p = new qore_string_private;
   p->len = b->size() - 1;
   p->buf = (char*)b->giveBuffer();
   p->charset = qe;

   // free binary object
   b = 0;

   // check for null termination
   if (p->buf[p->len]) {
      ++p->len;
      p->buf = (char*)realloc(p->buf, p->len + 1);
      p->buf[p->len] = '\0';
   }

   p->allocated = p->len + 1;
   return new QoreString(p);
}

QoreString* QoreString::parseBase64ToString(ExceptionSink* xsink) const {
   return parseBase64ToString(QCS_DEFAULT, xsink);
}

BinaryNode *QoreString::parseHex(ExceptionSink* xsink) const {
   return ::parseHex(priv->buf, priv->len, xsink);
}

void QoreString::allocate(unsigned requested_size) {
   priv->allocate(requested_size);
}

const QoreEncoding* QoreString::getEncoding() const {
   return priv->getEncoding();
}

QoreString* QoreString::copy() const {
   return new QoreString(*this);
}

void QoreString::tolwr() {
   char* c = priv->buf;
   while (*c) {
      *c = ::tolower(*c);
      c++;
   }
}

void QoreString::toupr() {
   char* c = priv->buf;
   while (*c) {
      *c = ::toupper(*c);
      c++;
   }
}

// returns number of bytes
qore_size_t QoreString::strlen() const {
   return priv->len;
}

qore_size_t QoreString::size() const {
   return priv->len;
}

qore_size_t QoreString::capacity() const {
   return priv->allocated;
}

const char* QoreString::getBuffer() const {
   return priv->buf;
}

const char* QoreString::c_str() const {
   return priv->buf;
}

void QoreString::addch(char c, unsigned times) {
   priv->check_char(priv->len + times); // more data will follow the padding
   memset(priv->buf + priv->len, c, times);
   priv->len += times;
   priv->buf[priv->len] = 0;
}

int QoreString::insertch(char c, qore_size_t pos, unsigned times) {
   //printd(5, "QoreString::insertch(c: %c pos: " QLLD " times: %d) this: %p\n", c, pos, times, this);
   if (pos > priv->len || !times)
      return -1;

   priv->check_char(priv->len + times); // more data will follow the padding
   if (pos < priv->len)
      memmove(priv->buf + pos + times, priv->buf + pos, priv->len - pos);
   memset(priv->buf + pos, c, times);
   priv->len += times;
   priv->buf[priv->len] = 0;
   return 0;
}

int QoreString::insert(const char* str, qore_size_t pos) {
   if (pos > priv->len)
      return -1;

   size_t sl = ::strlen(str);

   priv->check_char(priv->len + sl); // more data will follow the padding
   if (pos < priv->len)
      memmove(priv->buf + pos + sl, priv->buf + pos, priv->len - pos);
   strncpy(priv->buf + pos, str, sl);
   priv->len += sl;
   priv->buf[priv->len] = 0;
   return 0;
}

int QoreString::concatUnicode(unsigned code, ExceptionSink* xsink) {
   return priv->concatUnicode(code, xsink);
}

int QoreString::concatUnicode(unsigned code) {
   return priv->concatUnicode(code);
}

void QoreString::concatUTF8FromUnicode(unsigned code) {
   priv->concatUTF8FromUnicode(code);
}

unsigned int QoreString::getUnicodePointFromUTF8(qore_offset_t offset) const {
   // get length in chars
   bool invalid;
   char* endp = priv->buf + priv->len;
   qore_size_t clen = priv->getEncoding()->getLength(priv->buf, endp, invalid);
   if (invalid)
      return 0;

   //printd(0, "splice_complex(offset=" QSD ") clen=" QSD "\n", offset, clen);
   if (offset < 0) {
      offset = clen + offset;
      if (offset < 0)
         offset = 0;
   }
   else if ((qore_size_t)offset >= clen)
      return 0;

   // calculate byte offset
   if (offset) {
      offset = priv->getEncoding()->getByteLen(priv->buf, endp, offset, invalid);
      if (invalid)
         return 0;
   }

   qore_size_t bl = priv->getEncoding()->getByteLen(priv->buf + offset, endp, 1, invalid);
   if (invalid)
      return 0;

   return get_unicode_from_utf8(priv->buf + offset, bl);
}

unsigned int QoreString::getUnicodePoint(qore_offset_t offset, ExceptionSink* xsink) const {
   if (offset >= 0 || !priv->getEncoding()->isMultiByte()) {
      if (offset < 0) {
         offset = priv->len + offset;
         if (offset < 0)
            offset = 0;
      }
      qore_size_t bl = priv->getEncoding()->getByteLen(priv->buf, priv->buf + priv->len, offset, xsink);
      if (*xsink)
         return 0;

      unsigned len;
      return getUnicodePointFromBytePos(bl, len, xsink);
   }

   assert(priv->getEncoding() == QCS_UTF8);
   return getUnicodePointFromUTF8(offset);
}

unsigned int QoreString::getUnicodePointFromBytePos(qore_size_t offset, unsigned& len, ExceptionSink* xsink) const {
   return priv->getUnicodePointFromBytePos(offset, len, xsink);
}

QoreString* QoreString::reverse() const {
   QoreString* str = new QoreString(priv->getEncoding());
   concat_reverse(str);
   return str;
}

// remove trailing char
void QoreString::trim_trailing(char c) {
   if (!priv->len)
      return;

   char* p = priv->buf + priv->len - 1;
   while (p >= priv->buf && (*p) == c)
      --p;

   terminate(p + 1 - priv->buf);
}

// remove single trailing char
void QoreString::trim_single_trailing(char c) {
   if (priv->len && priv->buf[priv->len - 1] == c)
      terminate(priv->len - 1);
}

// remove leading char
void QoreString::trim_leading(char c) {
   if (!priv->len)
      return;

   qore_size_t i = 0;
   while (i < priv->len && priv->buf[i] == c)
      ++i;
   if (!i)
      return;

   memmove(priv->buf, priv->buf + i, priv->len + 1 - i);
   priv->len -= i;
}

// remove single leading char
void QoreString::trim_single_leading(char c) {
   if (priv->len && priv->buf[0] == c) {
      memmove(priv->buf, priv->buf + 1, priv->len);
      priv->len -= 1;
   }
}

// remove leading and trailing char
void QoreString::trim(char c) {
   trim_trailing(c);
   trim_leading(c);
}

// remove trailing chars
void QoreString::trim_trailing(const char* chars) {
   if (!priv->len)
      return;

   char* p = priv->buf + priv->len - 1;
   if (!chars) // use an alternate path here so we can check for embedded nulls as well
      while (p >= priv->buf && strnchr(default_whitespace, sizeof(default_whitespace), *p))
         --p;
   else
      while (p >= priv->buf && strchr(chars, *p))
         --p;

   terminate(p + 1 - priv->buf);
}

// remove leading char
void QoreString::trim_leading(const char* chars) {
   if (!priv->len)
      return;

   qore_size_t i = 0;
   if (!chars)
      while (i < priv->len && strnchr(default_whitespace, sizeof(default_whitespace), priv->buf[i]))
         ++i;
   else
      while (i < priv->len && strchr(chars, priv->buf[i]))
         ++i;
   if (!i)
      return;

   memmove(priv->buf, priv->buf + i, priv->len + 1 - i);
   priv->len -= i;
}

// remove leading and trailing blanks
void QoreString::trim(const char* chars) {
   trim_trailing(chars);
   trim_leading(chars);
}

// writes a new QoreString with the characters reversed of the "this" QoreString
// assumes the encoding is the same and the length is 0
void QoreString::concat_reverse(QoreString* str) const {
   assert(str->priv->getEncoding() == priv->getEncoding());
   assert(!str->priv->len);

   str->priv->check_char(priv->len);
   if (priv->getEncoding()->isMultiByte()) {
      char* p = priv->buf;
      char* end = str->priv->buf + priv->len;
      while (*p) {
         bool invalid;
         int bl = priv->getEncoding()->getByteLen(p, end, 1, invalid);
         if (invalid) // if we hit an invalid encoding, then we just copy bytes
            bl = 1;
         end -= bl;
         // in case of corrupt data, make sure we don't go off the beginning of the string
         if (end < str->priv->buf)
            break;
         strncpy(end, p, bl);
         p += bl;
      }
   }
   else
      for (qore_size_t i = 0; i < priv->len; ++i)
         str->priv->buf[i] = priv->buf[priv->len - i - 1];

   str->priv->buf[priv->len] = 0;
   str->priv->len = priv->len;
}

QoreString& QoreString::operator=(const QoreString& other) {
   set(other);
   return *this;
}

QoreString& QoreString::operator=(const char* other) {
   set(other);
   return *this;
}

QoreString& QoreString::operator=(const std::string& other) {
   set(other);
   return *this;
}

bool QoreString::operator==(const QoreString& other) const {
   if (other.priv->getEncoding() != priv->getEncoding() || other.priv->len != priv->len)
      return false;
   return !memcmp(other.priv->buf, priv->buf, priv->len);
}

bool QoreString::operator==(const std::string& other) const {
   if (other.size() != priv->len)
      return false;
   return !memcmp(other.c_str(), priv->buf, priv->len);
}

bool QoreString::operator==(const char* other) const {
   // NOTE: does not work with UTF-16 or other non-ASCII-compatible multi-byte encodings
   return !strcmp(other, priv->buf);
}

QoreString& QoreString::operator+=(const char* str) {
   concat(str);
   return *this;
}

QoreString& QoreString::operator+=(const std::string& str) {
   concat(str);
   return *this;
}

char QoreString::operator[](qore_offset_t pos) const {
   if (pos < 0) {
      pos = priv->len + pos;
      if (pos < 0)
         return '\0';
   }
   else if ((qore_size_t)pos >= priv->len)
      return '\0';

   return priv->buf[pos];
}

bool QoreString::empty() const {
   return !priv->len;
}

void QoreString::prepend(const char* str) {
   prepend(str, ::strlen(str));
}

void QoreString::prepend(const char* str, qore_size_t size) {
   priv->check_char(priv->len + size + 1);
   // move memory forward
   memmove((char*)priv->buf + size, priv->buf, priv->len + 1);
   // copy new memory to beginning
   memcpy((char*)priv->buf, str, size);
   priv->len += size;
}

qore_offset_t QoreString::index(const QoreString& needle, qore_offset_t pos, ExceptionSink* xsink) const {
   return priv->index(needle, pos, xsink);
}

qore_offset_t QoreString::bindex(const QoreString& needle, qore_offset_t pos) const {
   return priv->bindex(needle, pos);
}

qore_offset_t QoreString::bindex(const char* needle, qore_offset_t pos) const {
   return priv->bindex(needle, pos);
}

qore_offset_t QoreString::bindex(const std::string& needle, qore_offset_t pos) const {
   return priv->bindex(needle, pos);
}

qore_offset_t QoreString::rindex(const QoreString& needle, qore_offset_t pos, ExceptionSink* xsink) const {
   return priv->rindex(needle, pos, xsink);
}

qore_offset_t QoreString::brindex(const QoreString& needle, qore_offset_t pos) const {
   return priv->brindex(needle, pos);
}

qore_offset_t QoreString::brindex(const char* needle, qore_offset_t pos) const {
   return priv->brindex(needle, ::strlen(needle), pos);
}

qore_offset_t QoreString::brindex(const std::string& needle, qore_offset_t pos) const {
   return priv->brindex(needle, pos);
}

qore_offset_t QoreString::find(char c, qore_offset_t pos) const {
   return priv->find(c, pos);
}

qore_offset_t QoreString::rfind(char c, qore_offset_t pos) const {
   return priv->rfind(c, pos);
}

qore_offset_t QoreString::findAny(const char* str, qore_offset_t pos) const {
   return priv->findAny(str, pos);
}

qore_offset_t QoreString::rfindAny(const char* str, qore_offset_t pos) const {
   return priv->rfindAny(str, pos);
}

bool QoreString::isDataPrintableAscii() const {
   return priv->isDataPrintableAscii();
}

bool QoreString::isDataAscii() const {
   return priv->isDataAscii();
}

int64 QoreString::toBigInt() const {
   return strtoll(priv->buf, 0, 10);
}

qore_offset_t QoreString::getByteOffset(qore_size_t i, ExceptionSink* xsink) const {
   return priv->getByteOffset(i, xsink);
}

void TempEncodingHelper::removeBom() {
   if (!str || str->getEncoding()->isAsciiCompat())
      return;
   if (!temp) {
      str = new QoreString(*str);
      temp = true;
   }
   q_remove_bom_utf16(str, qore_string_private::get(*str)->charset);
}
