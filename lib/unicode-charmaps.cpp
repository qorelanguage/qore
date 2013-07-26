/*
  unicode-charmaps.cpp

  Qore Programming Language

  Copyright 2003 - 2013 David Nichols

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
#include <ctype.h>

typedef std::map<unsigned, unsigned> unicodemap_t;

typedef std::map<const char*, const char*, ltstr> charmap_t;
static charmap_t accent_map;

// unicode lower to upper case map
static unicodemap_t lumap;

// unicode upper to lower case map
static unicodemap_t ulmap;

void init_charmaps() {
#define M(k,v) accent_map[(k)] = (v)
   // taken from http://d3s.mff.cuni.cz/~holub/sw/phpaccents/
   M("À","A"); M("Á","A"); M("Â","A"); M("Ã","A"); M("Ä","A"); M("Å","A"); M("Ç","C"); M("È","E");
   M("É","E"); M("Ê","E"); M("Ë","E"); M("Ì","I"); M("Í","I"); M("Î","I"); M("Ï","I"); M("Ñ","N");
   M("Ò","O"); M("Ó","O"); M("Ô","O"); M("Õ","O"); M("Ö","O"); M("Ø","O"); M("Ù","U"); M("Ú","U");
   M("Û","U"); M("Ü","U"); M("Ý","Y"); M("à","a"); M("á","a"); M("â","a"); M("ã","a"); M("ä","a");
   M("å","a"); M("ç","c"); M("è","e"); M("é","e"); M("ê","e"); M("ë","e"); M("ì","i"); M("í","i");
   M("î","i"); M("ï","i"); M("ñ","n"); M("ò","o"); M("ó","o"); M("ô","o"); M("õ","o"); M("ö","o");
   M("ø","o"); M("ù","u"); M("ú","u"); M("û","u"); M("ü","u"); M("ý","y"); M("ÿ","y"); M("Ā","A");
   M("ā","a"); M("Ă","A"); M("ă","a"); M("Ą","A"); M("ą","a"); M("Ć","C"); M("ć","c"); M("Ĉ","C");
   M("ĉ","c"); M("Ċ","C"); M("ċ","c"); M("Č","C"); M("č","c"); M("Ď","D"); M("ď","d"); M("Đ","D");
   M("đ","d"); M("Ē","E"); M("ē","e"); M("Ĕ","E"); M("ĕ","e"); M("Ė","E"); M("ė","e"); M("Ę","E");
   M("ę","e"); M("Ě","E"); M("ě","e"); M("Ĝ","G"); M("ĝ","g"); M("Ğ","G"); M("ğ","g"); M("Ġ","G");
   M("ġ","g"); M("Ģ","G"); M("ģ","g"); M("Ĥ","H"); M("ĥ","h"); M("Ħ","H"); M("ħ","h"); M("Ĩ","I");
   M("ĩ","i"); M("Ī","I"); M("ī","i"); M("Ĭ","I"); M("ĭ","i"); M("Į","I"); M("į","i"); M("İ","I");
   M("ı","i"); M("Ĵ","J"); M("ĵ","j"); M("Ķ","K"); M("ķ","k"); M("Ĺ","L"); M("ĺ","l"); M("Ļ","L");
   M("ļ","l"); M("Ľ","L"); M("ľ","l"); M("Ŀ","L"); M("ŀ","l"); M("Ł","L"); M("ł","l"); M("Ń","N");
   M("ń","n"); M("Ņ","N"); M("ņ","n"); M("Ň","N"); M("ň","n"); M("ŉ","n"); M("Ō","O"); M("ō","o");
   M("Ŏ","O"); M("ŏ","o"); M("Ő","O"); M("ő","o"); M("Ŕ","R"); M("ŕ","r"); M("Ŗ","R"); M("ŗ","r");
   M("Ř","R"); M("ř","r"); M("Ś","S"); M("ś","s"); M("Ŝ","S"); M("ŝ","s"); M("Ş","S"); M("ş","s");
   M("Š","S"); M("š","s"); M("Ţ","T"); M("ţ","t"); M("Ť","T"); M("ť","t"); M("Ŧ","T"); M("ŧ","t");
   M("Ũ","U"); M("ũ","u"); M("Ū","U"); M("ū","u"); M("Ŭ","U"); M("ŭ","u"); M("Ů","U"); M("ů","u");
   M("Ű","U"); M("ű","u"); M("Ų","U"); M("ų","u"); M("Ŵ","W"); M("ŵ","w"); M("Ŷ","Y"); M("ŷ","y");
   M("Ÿ","Y"); M("Ź","Z"); M("ź","z"); M("Ż","Z"); M("ż","z"); M("Ž","Z"); M("ž","z"); M("ƀ","b");
   M("Ɓ","B"); M("Ƃ","B"); M("ƃ","b"); M("Ƈ","C"); M("ƈ","c"); M("Ɗ","D"); M("Ƌ","D"); M("ƌ","d");
   M("Ƒ","F"); M("ƒ","f"); M("Ɠ","G"); M("Ɨ","I"); M("Ƙ","K"); M("ƙ","k"); M("ƚ","l"); M("Ɲ","N");
   M("ƞ","n"); M("Ɵ","O"); M("Ơ","O"); M("ơ","o"); M("Ƥ","P"); M("ƥ","p"); M("ƫ","t"); M("Ƭ","T");
   M("ƭ","t"); M("Ʈ","T"); M("Ư","U"); M("ư","u"); M("Ʋ","V"); M("Ƴ","Y"); M("ƴ","y"); M("Ƶ","Z");
   M("ƶ","z"); M("ǅ","D"); M("ǈ","L"); M("ǋ","N"); M("Ǎ","A"); M("ǎ","a"); M("Ǐ","I"); M("ǐ","i");
   M("Ǒ","O"); M("ǒ","o"); M("Ǔ","U"); M("ǔ","u"); M("Ǖ","U"); M("ǖ","u"); M("Ǘ","U"); M("ǘ","u");
   M("Ǚ","U"); M("ǚ","u"); M("Ǜ","U"); M("ǜ","u"); M("Ǟ","A"); M("ǟ","a"); M("Ǡ","A"); M("ǡ","a");
   M("Ǥ","G"); M("ǥ","g"); M("Ǧ","G"); M("ǧ","g"); M("Ǩ","K"); M("ǩ","k"); M("Ǫ","O"); M("ǫ","o");
   M("Ǭ","O"); M("ǭ","o"); M("ǰ","j"); M("ǲ","D"); M("Ǵ","G"); M("ǵ","g"); M("Ǹ","N"); M("ǹ","n");
   M("Ǻ","A"); M("ǻ","a"); M("Ǿ","O"); M("ǿ","o"); M("Ȁ","A"); M("ȁ","a"); M("Ȃ","A"); M("ȃ","a");
   M("Ȅ","E"); M("ȅ","e"); M("Ȇ","E"); M("ȇ","e"); M("Ȉ","I"); M("ȉ","i"); M("Ȋ","I"); M("ȋ","i");
   M("Ȍ","O"); M("ȍ","o"); M("Ȏ","O"); M("ȏ","o"); M("Ȑ","R"); M("ȑ","r"); M("Ȓ","R"); M("ȓ","r");
   M("Ȕ","U"); M("ȕ","u"); M("Ȗ","U"); M("ȗ","u"); M("Ș","S"); M("ș","s"); M("Ț","T"); M("ț","t");
   M("Ȟ","H"); M("ȟ","h"); M("Ƞ","N"); M("ȡ","d"); M("Ȥ","Z"); M("ȥ","z"); M("Ȧ","A"); M("ȧ","a");
   M("Ȩ","E"); M("ȩ","e"); M("Ȫ","O"); M("ȫ","o"); M("Ȭ","O"); M("ȭ","o"); M("Ȯ","O"); M("ȯ","o");
   M("Ȱ","O"); M("ȱ","o"); M("Ȳ","Y"); M("ȳ","y"); M("ȴ","l"); M("ȵ","n"); M("ȶ","t"); M("ȷ","j");
   M("Ⱥ","A"); M("Ȼ","C"); M("ȼ","c"); M("Ƚ","L"); M("Ⱦ","T"); M("ȿ","s"); M("ɀ","z"); M("Ƀ","B");
   M("Ʉ","U"); M("Ɇ","E"); M("ɇ","e"); M("Ɉ","J"); M("ɉ","j"); M("ɋ","q"); M("Ɍ","R"); M("ɍ","r");
   M("Ɏ","Y"); M("ɏ","y"); M("ɓ","b"); M("ɕ","c"); M("ɖ","d"); M("ɗ","d"); M("ɟ","j"); M("ɠ","g");
   M("ɦ","h"); M("ɨ","i"); M("ɫ","l"); M("ɬ","l"); M("ɭ","l"); M("ɱ","m"); M("ɲ","n"); M("ɳ","n");
   M("ɵ","o"); M("ɼ","r"); M("ɽ","r"); M("ɾ","r"); M("ʂ","s"); M("ʄ","j"); M("ʈ","t"); M("ʉ","u");
   M("ʋ","v"); M("ʐ","z"); M("ʑ","z"); M("ʝ","j"); M("ʠ","q"); M("ͣ","a"); M("ͤ","e"); M("ͥ","i");
   M("ͦ","o"); M("ͧ","u"); M("ͨ","c"); M("ͩ","d"); M("ͪ","h"); M("ͫ","m"); M("ͬ","r"); M("ͭ","t");
   M("ͮ","v"); M("ͯ","x"); M("ᵢ","i"); M("ᵣ","r"); M("ᵤ","u"); M("ᵥ","v"); M("ᵬ","b"); M("ᵭ","d");
   M("ᵮ","f"); M("ᵯ","m"); M("ᵰ","n"); M("ᵱ","p"); M("ᵲ","r"); M("ᵳ","r"); M("ᵴ","s"); M("ᵵ","t");
   M("ᵶ","z"); M("ᵻ","i"); M("ᵽ","p"); M("ᵾ","u"); M("ᶀ","b"); M("ᶁ","d"); M("ᶂ","f"); M("ᶃ","g");
   M("ᶄ","k"); M("ᶅ","l"); M("ᶆ","m"); M("ᶇ","n"); M("ᶈ","p"); M("ᶉ","r"); M("ᶊ","s"); M("ᶌ","v");
   M("ᶍ","x"); M("ᶎ","z"); M("ᶏ","a"); M("ᶑ","d"); M("ᶒ","e"); M("ᶖ","i"); M("ᶙ","u"); M("᷊","r");
   M("ᷗ","c"); M("ᷚ","g"); M("ᷜ","k"); M("ᷝ","l"); M("ᷠ","n"); M("ᷣ","r"); M("ᷤ","s"); M("ᷦ","z");
   M("Ḁ","A"); M("ḁ","a"); M("Ḃ","B"); M("ḃ","b"); M("Ḅ","B"); M("ḅ","b"); M("Ḇ","B"); M("ḇ","b");
   M("Ḉ","C"); M("ḉ","c"); M("Ḋ","D"); M("ḋ","d"); M("Ḍ","D"); M("ḍ","d"); M("Ḏ","D"); M("ḏ","d");
   M("Ḑ","D"); M("ḑ","d"); M("Ḓ","D"); M("ḓ","d"); M("Ḕ","E"); M("ḕ","e"); M("Ḗ","E"); M("ḗ","e");
   M("Ḙ","E"); M("ḙ","e"); M("Ḛ","E"); M("ḛ","e"); M("Ḝ","E"); M("ḝ","e"); M("Ḟ","F"); M("ḟ","f");
   M("Ḡ","G"); M("ḡ","g"); M("Ḣ","H"); M("ḣ","h"); M("Ḥ","H"); M("ḥ","h"); M("Ḧ","H"); M("ḧ","h");
   M("Ḩ","H"); M("ḩ","h"); M("Ḫ","H"); M("ḫ","h"); M("Ḭ","I"); M("ḭ","i"); M("Ḯ","I"); M("ḯ","i");
   M("Ḱ","K"); M("ḱ","k"); M("Ḳ","K"); M("ḳ","k"); M("Ḵ","K"); M("ḵ","k"); M("Ḷ","L"); M("ḷ","l");
   M("Ḹ","L"); M("ḹ","l"); M("Ḻ","L"); M("ḻ","l"); M("Ḽ","L"); M("ḽ","l"); M("Ḿ","M"); M("ḿ","m");
   M("Ṁ","M"); M("ṁ","m"); M("Ṃ","M"); M("ṃ","m"); M("Ṅ","N"); M("ṅ","n"); M("Ṇ","N"); M("ṇ","n");
   M("Ṉ","N"); M("ṉ","n"); M("Ṋ","N"); M("ṋ","n"); M("Ṍ","O"); M("ṍ","o"); M("Ṏ","O"); M("ṏ","o");
   M("Ṑ","O"); M("ṑ","o"); M("Ṓ","O"); M("ṓ","o"); M("Ṕ","P"); M("ṕ","p"); M("Ṗ","P"); M("ṗ","p");
   M("Ṙ","R"); M("ṙ","r"); M("Ṛ","R"); M("ṛ","r"); M("Ṝ","R"); M("ṝ","r"); M("Ṟ","R"); M("ṟ","r");
   M("Ṡ","S"); M("ṡ","s"); M("Ṣ","S"); M("ṣ","s"); M("Ṥ","S"); M("ṥ","s"); M("Ṧ","S"); M("ṧ","s");
   M("Ṩ","S"); M("ṩ","s"); M("Ṫ","T"); M("ṫ","t"); M("Ṭ","T"); M("ṭ","t"); M("Ṯ","T"); M("ṯ","t");
   M("Ṱ","T"); M("ṱ","t"); M("Ṳ","U"); M("ṳ","u"); M("Ṵ","U"); M("ṵ","u"); M("Ṷ","U"); M("ṷ","u");
   M("Ṹ","U"); M("ṹ","u"); M("Ṻ","U"); M("ṻ","u"); M("Ṽ","V"); M("ṽ","v"); M("Ṿ","V"); M("ṿ","v");
   M("Ẁ","W"); M("ẁ","w"); M("Ẃ","W"); M("ẃ","w"); M("Ẅ","W"); M("ẅ","w"); M("Ẇ","W"); M("ẇ","w");
   M("Ẉ","W"); M("ẉ","w"); M("Ẋ","X"); M("ẋ","x"); M("Ẍ","X"); M("ẍ","x"); M("Ẏ","Y"); M("ẏ","y");
   M("Ẑ","Z"); M("ẑ","z"); M("Ẓ","Z"); M("ẓ","z"); M("Ẕ","Z"); M("ẕ","z"); M("ẖ","h"); M("ẗ","t");
   M("ẘ","w"); M("ẙ","y"); M("ẚ","a"); M("Ạ","A"); M("ạ","a"); M("Ả","A"); M("ả","a"); M("Ấ","A");
   M("ấ","a"); M("Ầ","A"); M("ầ","a"); M("Ẩ","A"); M("ẩ","a"); M("Ẫ","A"); M("ẫ","a"); M("Ậ","A");
   M("ậ","a"); M("Ắ","A"); M("ắ","a"); M("Ằ","A"); M("ằ","a"); M("Ẳ","A"); M("ẳ","a"); M("Ẵ","A");
   M("ẵ","a"); M("Ặ","A"); M("ặ","a"); M("Ẹ","E"); M("ẹ","e"); M("Ẻ","E"); M("ẻ","e"); M("Ẽ","E");
   M("ẽ","e"); M("Ế","E"); M("ế","e"); M("Ề","E"); M("ề","e"); M("Ể","E"); M("ể","e"); M("Ễ","E");
   M("ễ","e"); M("Ệ","E"); M("ệ","e"); M("Ỉ","I"); M("ỉ","i"); M("Ị","I"); M("ị","i"); M("Ọ","O");
   M("ọ","o"); M("Ỏ","O"); M("ỏ","o"); M("Ố","O"); M("ố","o"); M("Ồ","O"); M("ồ","o"); M("Ổ","O");
   M("ổ","o"); M("Ỗ","O"); M("ỗ","o"); M("Ộ","O"); M("ộ","o"); M("Ớ","O"); M("ớ","o"); M("Ờ","O");
   M("ờ","o"); M("Ở","O"); M("ở","o"); M("Ỡ","O"); M("ỡ","o"); M("Ợ","O"); M("ợ","o"); M("Ụ","U");
   M("ụ","u"); M("Ủ","U"); M("ủ","u"); M("Ứ","U"); M("ứ","u"); M("Ừ","U"); M("ừ","u"); M("Ử","U");
   M("ử","u"); M("Ữ","U"); M("ữ","u"); M("Ự","U"); M("ự","u"); M("Ỳ","Y"); M("ỳ","y"); M("Ỵ","Y");
   M("ỵ","y"); M("Ỷ","Y"); M("ỷ","y"); M("Ỹ","Y"); M("ỹ","y"); M("Ỿ","Y"); M("ỿ","y"); M("ⁱ","i");
   M("ⁿ","n"); M("ₐ","a"); M("ₑ","e"); M("ₒ","o"); M("ₓ","x"); M("⒜","a"); M("⒝","b"); M("⒞","c");
   M("⒟","d"); M("⒠","e"); M("⒡","f"); M("⒢","g"); M("⒣","h"); M("⒤","i"); M("⒥","j"); M("⒦","k");
   M("⒧","l"); M("⒨","m"); M("⒩","n"); M("⒪","o"); M("⒫","p"); M("⒬","q"); M("⒭","r"); M("⒮","s");
   M("⒯","t"); M("⒰","u"); M("⒱","v"); M("⒲","w"); M("⒳","x"); M("⒴","y"); M("⒵","z"); M("Ⓐ","A");
   M("Ⓑ","B"); M("Ⓒ","C"); M("Ⓓ","D"); M("Ⓔ","E"); M("Ⓕ","F"); M("Ⓖ","G"); M("Ⓗ","H"); M("Ⓘ","I");
   M("Ⓙ","J"); M("Ⓚ","K"); M("Ⓛ","L"); M("Ⓜ","M"); M("Ⓝ","N"); M("Ⓞ","O"); M("Ⓟ","P"); M("Ⓠ","Q");
   M("Ⓡ","R"); M("Ⓢ","S"); M("Ⓣ","T"); M("Ⓤ","U"); M("Ⓥ","V"); M("Ⓦ","W"); M("Ⓧ","X"); M("Ⓨ","Y");
   M("Ⓩ","Z"); M("ⓐ","a"); M("ⓑ","b"); M("ⓒ","c"); M("ⓓ","d"); M("ⓔ","e"); M("ⓕ","f"); M("ⓖ","g");
   M("ⓗ","h"); M("ⓘ","i"); M("ⓙ","j"); M("ⓚ","k"); M("ⓛ","l"); M("ⓜ","m"); M("ⓝ","n"); M("ⓞ","o");
   M("ⓟ","p"); M("ⓠ","q"); M("ⓡ","r"); M("ⓢ","s"); M("ⓣ","t"); M("ⓤ","u"); M("ⓥ","v"); M("ⓦ","w");
   M("ⓧ","x"); M("ⓨ","y"); M("ⓩ","z"); M("Ⱡ","L"); M("ⱡ","l"); M("Ɫ","L"); M("Ᵽ","P"); M("Ɽ","R");
   M("ⱥ","a"); M("ⱦ","t"); M("Ⱨ","H"); M("ⱨ","h"); M("Ⱪ","K"); M("ⱪ","k"); M("Ⱬ","Z"); M("ⱬ","z");
   M("Ɱ","M"); M("ⱱ","v"); M("Ⱳ","W"); M("ⱳ","w"); M("ⱴ","v"); M("ⱸ","e"); M("ⱺ","o"); M("ⱼ","j");
   M("Ꝁ","K"); M("ꝁ","k"); M("Ꝃ","K"); M("ꝃ","k"); M("Ꝅ","K"); M("ꝅ","k"); M("Ꝉ","L"); M("ꝉ","l");
   M("Ꝋ","O"); M("ꝋ","o"); M("Ꝍ","O"); M("ꝍ","o"); M("Ꝑ","P"); M("ꝑ","p"); M("Ꝓ","P"); M("ꝓ","p");
   M("Ꝕ","P"); M("ꝕ","p"); M("Ꝗ","Q"); M("ꝗ","q"); M("Ꝙ","Q"); M("ꝙ","q"); M("Ꝛ","R"); M("ꝛ","r");
   M("Ꝟ","V"); M("ꝟ","v"); M("Ａ","A"); M("Ｂ","B"); M("Ｃ","C"); M("Ｄ","D"); M("Ｅ","E"); M("Ｆ","F");
   M("Ｇ","G"); M("Ｈ","H"); M("Ｉ","I"); M("Ｊ","J"); M("Ｋ","K"); M("Ｌ","L"); M("Ｍ","M"); M("Ｎ","N");
   M("Ｏ","O"); M("Ｐ","P"); M("Ｑ","Q"); M("Ｒ","R"); M("Ｓ","S"); M("Ｔ","T"); M("Ｕ","U"); M("Ｖ","V");
   M("Ｗ","W"); M("Ｘ","X"); M("Ｙ","Y"); M("Ｚ","Z"); M("ａ","a"); M("ｂ","b"); M("ｃ","c"); M("ｄ","d");
   M("ｅ","e"); M("ｆ","f"); M("ｇ","g"); M("ｈ","h"); M("ｉ","i"); M("ｊ","j"); M("ｋ","k"); M("ｌ","l");
   M("ｍ","m"); M("ｎ","n"); M("ｏ","o"); M("ｐ","p"); M("ｑ","q"); M("ｒ","r"); M("ｓ","s"); M("ｔ","t");
   M("ｕ","u"); M("ｖ","v"); M("ｗ","w"); M("ｘ","x"); M("ｙ","y"); M("ｚ","z");
   // additional stuff
   M("æ","ae");
   M("ß","ss");
   M("œ", "oe");

   lumap[0x00e0] = 0x00c0; // latin small letter a grave -> latin capital letter a grave
   lumap[0x00e1] = 0x00c1; // latin small letter a grave -> latin capital letter a acute
   lumap[0x00e2] = 0x00c2; // latin small letter a grave -> latin capital letter a circumflex
   lumap[0x00e3] = 0x00c3; // latin small letter a grave -> latin capital letter a tilde
   lumap[0x00e4] = 0x00c4; // latin small letter a grave -> latin capital letter a diaeresis
   lumap[0x00e5] = 0x00c5; // latin small letter a grave -> latin capital letter a ring
   lumap[0x00e6] = 0x00c6; // latin small letter a grave -> latin capital letter a e
   lumap[0x00e7] = 0x00c7; // latin small letter a grave -> latin capital letter c cedilla
   lumap[0x00e8] = 0x00c8; // latin small letter a grave -> latin capital letter e grave
   lumap[0x00e9] = 0x00c9; // latin small letter a grave -> latin capital letter e acute
   lumap[0x00ea] = 0x00ca; // latin small letter e circumflex -> latin capital letter e circumflex
   lumap[0x00eb] = 0x00cb; // latin small letter e diaeresis -> latin capital letter e diaeresis
   lumap[0x00ec] = 0x00cc; // latin small letter i grave -> latin capital letter i grave
   lumap[0x00ed] = 0x00cd; // latin small letter i acute -> latin capital letter i acute
   lumap[0x00ee] = 0x00ce; // latin small letter i circumflex -> latin capital letter i circumflex
   lumap[0x00ef] = 0x00cf; // latin small letter i diaeresis -> latin capital letter i diaeresis
   lumap[0x00f0] = 0x00d0; // latin small letter eth -> latin capital letter eth
   lumap[0x00f1] = 0x00d1; // latin small letter n tilde -> latin capital letter n tilde
   lumap[0x00f2] = 0x00d2; // latin small letter o grave -> latin capital letter o grave
   lumap[0x00f3] = 0x00d3; // latin small letter o acute -> latin capital letter o acute
   lumap[0x00f4] = 0x00d4; // latin small letter o circumflex -> latin capital letter o circumflex
   lumap[0x00f5] = 0x00d5; // latin small letter o tilde -> latin capital letter o tilde
   lumap[0x00f6] = 0x00d6; // latin small letter o diaeresis -> latin capital letter o diaeresis
   lumap[0x00f8] = 0x00d8; // latin small letter o slash -> latin capital letter o slash
   lumap[0x00f9] = 0x00d9; // latin small letter u grave -> latin capital letter u grave
   lumap[0x00fa] = 0x00da; // latin small letter u acute -> latin capital letter u acute
   lumap[0x00fb] = 0x00db; // latin small letter u circumflex -> latin capital letter u circumflex
   lumap[0x00fc] = 0x00dc; // latin small letter u diaeresis -> latin capital letter u diaeresis
   lumap[0x00fd] = 0x00dd; // latin small letter y acute -> latin capital letter y acute
   lumap[0x00fe] = 0x00de; // latin small letter thorn -> latin capital letter thorn
   lumap[0x00ff] = 0x0178; // latin small letter y diaeresis -> latin capital letter y with diaeresis
   lumap[0x0101] = 0x0100; // latin small letter a with macron -> latin capital letter a with macron
   lumap[0x0103] = 0x0102; // latin small letter a with breve -> latin capital letter a with breve
   lumap[0x0105] = 0x0104; // latin small letter a with ogonek -> latin capital letter a with ogonek
   lumap[0x0107] = 0x0106; // latin small letter c with acute -> latin capital letter c with acute
   lumap[0x0109] = 0x0108; // latin small letter c with circumflex -> latin capital letter c with circumflex
   lumap[0x010b] = 0x010a; // latin small letter c with dot above -> latin capital letter c with dot above
   lumap[0x010d] = 0x010c; // latin small letter c with caron -> latin capital letter c with caron
   lumap[0x010f] = 0x010e; // latin small letter d with caron -> latin capital letter d with caron
   lumap[0x0111] = 0x0110; // latin small letter d with stroke -> latin capital letter d with stroke
   lumap[0x0113] = 0x0112; // latin small letter e with macron -> latin capital letter e with macron
   lumap[0x0115] = 0x0114; // latin small letter e with breve -> latin capital letter e with breve
   lumap[0x0117] = 0x0116; // latin small letter e with dot above -> latin capital letter e with dot above
   lumap[0x0119] = 0x0118; // latin small letter e with ogonek -> latin capital letter e with ogonek
   lumap[0x011b] = 0x011a; // latin small letter e with caron -> latin capital letter e with caron
   lumap[0x011d] = 0x011c; // latin small letter g with circumflex -> latin capital letter g with circumflex
   lumap[0x011f] = 0x011e; // latin small letter g with breve -> latin capital letter g with breve
   lumap[0x0121] = 0x0120; // latin small letter g with dot above -> latin capital letter g with dot above
   lumap[0x0123] = 0x0122; // latin small letter g with cedilla -> latin capital letter g with cedilla
   lumap[0x0125] = 0x0124; // latin small letter h with circumflex -> latin capital letter h with circumflex
   lumap[0x0127] = 0x0126; // latin small letter h with stroke -> latin capital letter h with stroke
   lumap[0x0129] = 0x0128; // latin small letter i with tilde -> latin capital letter i with tilde
   lumap[0x012b] = 0x012a; // latin small letter i with macron -> latin capital letter i with macron
   lumap[0x012d] = 0x012c; // latin small letter i with breve -> latin capital letter i with breve
   lumap[0x012f] = 0x012e; // latin small letter i with ogonek -> latin capital letter i with ogonek
   lumap[0x0131] = 0x0049; // latin small letter dotless i -> latin capital letter i
   lumap[0x0133] = 0x0132; // latin small ligature ij -> latin capital ligature ij
   lumap[0x0135] = 0x0134; // latin small letter j with circumflex -> latin capital letter j with circumflex
   lumap[0x0137] = 0x0136; // latin small letter k with cedilla -> latin capital letter k with cedilla
   lumap[0x013a] = 0x0139; // latin small letter l with acute -> latin capital letter l with acute
   lumap[0x013c] = 0x013b; // latin small letter l with cedilla -> latin capital letter l with cedilla
   lumap[0x013e] = 0x013d; // latin small letter l with caron -> latin capital letter l with caron
   lumap[0x0140] = 0x013f; // latin small letter l with middle dot -> latin capital letter l with middle dot
   lumap[0x0142] = 0x0141; // latin small letter l with stroke -> latin capital letter l with stroke
   lumap[0x0144] = 0x0143; // latin small letter n with acute -> latin capital letter n with acute
   lumap[0x0146] = 0x0145; // latin small letter n with cedilla -> latin capital letter n with cedilla
   lumap[0x0148] = 0x0147; // latin small letter n with caron -> latin capital letter n with caron
   lumap[0x014b] = 0x014a; // latin small letter eng (sami) -> latin capital letter eng (sami)
   lumap[0x014d] = 0x014c; // latin small letter o with macron -> latin capital letter o with macron
   lumap[0x014f] = 0x014e; // latin small letter o with breve -> latin capital letter o with breve
   lumap[0x0151] = 0x0150; // latin small letter o with double acute -> latin capital letter o with double acute
   lumap[0x0153] = 0x0152; // latin small ligature oe -> latin capital ligature oe
   lumap[0x0155] = 0x0154; // latin small letter r with acute -> latin capital letter r with acute
   lumap[0x0157] = 0x0156; // latin small letter r with cedilla -> latin capital letter r with cedilla
   lumap[0x0159] = 0x0158; // latin small letter r with caron -> latin capital letter r with caron
   lumap[0x015b] = 0x015a; // latin small letter s with acute -> latin capital letter s with acute
   lumap[0x015d] = 0x015c; // latin small letter s with circumflex -> latin capital letter s with circumflex
   lumap[0x015f] = 0x015e; // latin small letter s with cedilla -> latin capital letter s with cedilla
   lumap[0x0161] = 0x0160; // latin small letter s with caron -> latin capital letter s with caron
   lumap[0x0163] = 0x0162; // latin small letter t with cedilla -> latin capital letter t with cedilla
   lumap[0x0165] = 0x0164; // latin small letter t with caron -> latin capital letter t with caron
   lumap[0x0167] = 0x0166; // latin small letter t with stroke -> latin capital letter t with stroke
   lumap[0x0169] = 0x0168; // latin small letter u with tilde -> latin capital letter u with tilde
   lumap[0x016b] = 0x016a; // latin small letter u with macron -> latin capital letter u with macron
   lumap[0x016d] = 0x016c; // latin small letter u with breve -> latin capital letter u with breve
   lumap[0x016f] = 0x016e; // latin small letter u with ring above -> latin capital letter u with ring above
   lumap[0x0171] = 0x0170; // latin small letter u with double acute -> latin capital letter u with double acute
   lumap[0x0173] = 0x0172; // latin small letter u with ogonek -> latin capital letter u with ogonek
   lumap[0x0175] = 0x0174; // latin small letter w with circumflex -> latin capital letter w with circumflex
   lumap[0x0177] = 0x0176; // latin small letter y with circumflex -> latin capital letter y with circumflex
   lumap[0x017a] = 0x0179; // latin small letter z with acute -> latin capital letter z with acute
   lumap[0x017c] = 0x017b; // latin small letter z with dot above -> latin capital letter z with dot above
   lumap[0x017e] = 0x017d; // latin small letter z with caron -> latin capital letter z with caron
   lumap[0x0183] = 0x0182; // latin small letter b with topbar -> latin capital letter b with topbar
   lumap[0x0185] = 0x0184; // latin small letter tone six -> latin capital letter tone six
   lumap[0x0188] = 0x0187; // latin small letter c with hook -> latin capital letter c with hook
   lumap[0x018c] = 0x018b; // latin small letter d with topbar -> latin capital letter d with topbar
   lumap[0x0192] = 0x0191; // latin small letter f with hook -> latin capital letter f with hook
   lumap[0x0199] = 0x0198; // latin small letter k with hook -> latin capital letter k with hook
   lumap[0x01a1] = 0x01a0; // latin small letter o with horn -> latin capital letter o with horn
   lumap[0x01a3] = 0x01a2; // latin small letter oi -> latin capital letter oi
   lumap[0x01a5] = 0x01a4; // latin small letter p with hook -> latin capital letter p with hook
   lumap[0x01a8] = 0x01a7; // latin small letter tone two -> latin capital letter tone two
   lumap[0x01ad] = 0x01ac; // latin small letter t with hook -> latin capital letter t with hook
   lumap[0x01b0] = 0x01af; // latin small letter u with horn -> latin capital letter u with horn
   lumap[0x01b4] = 0x01b3; // latin small letter y with hook -> latin capital letter y with hook
   lumap[0x01b6] = 0x01b5; // latin small letter z with stroke -> latin capital letter z with stroke
   lumap[0x01b9] = 0x01b8; // latin small letter ezh reversed -> latin capital letter ezh reversed
   lumap[0x01bd] = 0x01bc; // latin small letter tone five -> latin capital letter tone five
   lumap[0x01c6] = 0x01c4; // latin small letter dz with caron -> latin capital letter dz with caron
   lumap[0x01c9] = 0x01c7; // latin small letter lj -> latin capital letter lj
   lumap[0x01cc] = 0x01ca; // latin small letter nj -> latin capital letter nj
   lumap[0x01ce] = 0x01cd; // latin small letter a with caron -> latin capital letter a with caron
   lumap[0x01d0] = 0x01cf; // latin small letter i with caron -> latin capital letter i with caron
   lumap[0x01d2] = 0x01d1; // latin small letter o with caron -> latin capital letter o with caron
   lumap[0x01d4] = 0x01d3; // latin small letter u with caron -> latin capital letter u with caron
   lumap[0x01d6] = 0x01d5; // latin small letter u with diaeresis and macron -> latin capital letter u with diaeresis and macron
   lumap[0x01d8] = 0x01d7; // latin small letter u with diaeresis and acute -> latin capital letter u with diaeresis and acute
   lumap[0x01da] = 0x01d9; // latin small letter u with diaeresis and caron -> latin capital letter u with diaeresis and caron
   lumap[0x01dc] = 0x01db; // latin small letter u with diaeresis and grave -> latin capital letter u with diaeresis and grave
   lumap[0x01df] = 0x01de; // latin small letter a with diaeresis and macron -> latin capital letter a with diaeresis and macron
   lumap[0x01e1] = 0x01e0; // latin small letter a with dot above and macron -> latin capital letter a with dot above and macron
   lumap[0x01e3] = 0x01e2; // latin small ligature ae with macron -> latin capital ligature ae mth macron
   lumap[0x01e5] = 0x01e4; // latin small letter g with stroke -> latin capital letter g with stroke
   lumap[0x01e7] = 0x01e6; // latin small letter g with caron -> latin capital letter g with caron
   lumap[0x01e9] = 0x01e8; // latin small letter k with caron -> latin capital letter k with caron
   lumap[0x01eb] = 0x01ea; // latin small letter o with ogonek -> latin capital letter o with ogonek
   lumap[0x01ed] = 0x01ec; // latin small letter o with ogonek and macron -> latin capital letter o with ogonek and macron
   lumap[0x01ef] = 0x01ee; // latin small letter ezh with caron -> latin capital letter ezh with caron
   lumap[0x01f3] = 0x01f1; // latin small letter dz -> latin capital letter dz
   lumap[0x01f5] = 0x01f4; // latin small letter g with acute -> latin capital letter g with acute
   lumap[0x01fb] = 0x01fa; // latin small letter a with ring above and acute -> latin capital letter a with ring above and acute
   lumap[0x01fd] = 0x01fc; // latin small ligature ae with acute -> latin capital ligature ae with acute
   lumap[0x01ff] = 0x01fe; // latin small letter o with stroke and acute -> latin capital letter o with stroke and acute
   lumap[0x0201] = 0x0200; // latin small letter a with double grave -> latin capital letter a with double grave
   lumap[0x0203] = 0x0202; // latin small letter a with inverted breve -> latin capital letter a with inverted breve
   lumap[0x0205] = 0x0204; // latin small letter e with double grave -> latin capital letter e with double grave
   lumap[0x0207] = 0x0206; // latin small letter e with inverted breve -> latin capital letter e with inverted breve
   lumap[0x0209] = 0x0208; // latin small letter i with double grave -> latin capital letter i with double grave
   lumap[0x020b] = 0x020a; // latin small letter i with inverted breve -> latin capital letter i with inverted breve
   lumap[0x020d] = 0x020c; // latin small letter o with double grave -> latin capital letter o with double grave
   lumap[0x020f] = 0x020e; // latin small letter o with inverted breve -> latin capital letter o with inverted breve
   lumap[0x0211] = 0x0210; // latin small letter r with double grave -> latin capital letter r with double grave
   lumap[0x0213] = 0x0212; // latin small letter r with inverted breve -> latin capital letter r with inverted breve
   lumap[0x0215] = 0x0214; // latin small letter u with double grave -> latin capital letter u with double grave
   lumap[0x0217] = 0x0216; // latin small letter u with inverted breve -> latin capital letter u with inverted breve
   lumap[0x0253] = 0x0181; // latin small letter b with hook -> latin capital letter b with hook
   lumap[0x0254] = 0x0186; // latin small letter open o -> latin capital letter open o
   lumap[0x0257] = 0x018a; // latin small letter d with hook -> latin capital letter d with hook
   lumap[0x0258] = 0x018e; // latin small letter reversed e -> latin capital letter reversed e
   lumap[0x0259] = 0x018f; // latin small letter schwa -> latin capital letter schwa
   lumap[0x025b] = 0x0190; // latin small letter open e -> latin capital letter open e
   lumap[0x0260] = 0x0193; // latin small letter g with hook -> latin capital letter g with hook
   lumap[0x0263] = 0x0194; // latin small letter gamma -> latin capital letter gamma
   lumap[0x0268] = 0x0197; // latin small letter i with stroke -> latin capital letter i with stroke
   lumap[0x0269] = 0x0196; // latin small letter iota -> latin capital letter iota
   lumap[0x026f] = 0x019c; // latin small letter turned m -> latin capital letter turned m
   lumap[0x0272] = 0x019d; // latin small letter n with left hook -> latin capital letter n with left hook
   lumap[0x0275] = 0x019f; // latin small letter barred o -> latin capital letter o with middle tilde
   lumap[0x0283] = 0x01a9; // latin small letter esh -> latin capital letter esh
   lumap[0x0288] = 0x01ae; // latin small letter t with retroflex hook -> latin capital letter t with retroflex hook
   lumap[0x028a] = 0x01b1; // latin small letter upsilon -> latin capital letter upsilon
   lumap[0x028b] = 0x01b2; // latin small letter v with hook -> latin capital letter v with hook
   lumap[0x0292] = 0x01b7; // latin small letter ezh -> latin capital letter ezh
   lumap[0x03ac] = 0x0386; // greek small letter alpha with tonos -> greek capital letter alpha with tonos
   lumap[0x03ad] = 0x0388; // greek small letter epsilon with tonos -> greek capital letter epsilon with tonos
   lumap[0x03ae] = 0x0389; // greek small letter eta with tonos -> greek capital letter eta with tonos
   lumap[0x03af] = 0x038a; // greek small letter iota with tonos -> greek capital letter iota with tonos
   lumap[0x03b1] = 0x0391; // greek small letter alpha -> greek capital letter alpha
   lumap[0x03b2] = 0x0392; // greek small letter beta -> greek capital letter beta
   lumap[0x03b3] = 0x0393; // greek small letter gamma -> greek capital letter gamma
   lumap[0x03b4] = 0x0394; // greek small letter delta -> greek capital letter delta
   lumap[0x03b5] = 0x0395; // greek small letter epsilon -> greek capital letter epsilon
   lumap[0x03b6] = 0x0396; // greek small letter zeta -> greek capital letter zeta
   lumap[0x03b7] = 0x0397; // greek small letter eta -> greek capital letter eta
   lumap[0x03b8] = 0x0398; // greek small letter theta -> greek capital letter theta
   lumap[0x03b9] = 0x0399; // greek small letter iota -> greek capital letter iota
   lumap[0x03ba] = 0x039a; // greek small letter kappa -> greek capital letter kappa
   lumap[0x03bb] = 0x039b; // greek small letter lamda -> greek capital letter lamda
   lumap[0x03bc] = 0x039c; // greek small letter mu -> greek capital letter mu
   lumap[0x03bd] = 0x039d; // greek small letter nu -> greek capital letter nu
   lumap[0x03be] = 0x039e; // greek small letter xi -> greek capital letter xi
   lumap[0x03bf] = 0x039f; // greek small letter omicron -> greek capital letter omicron
   lumap[0x03c0] = 0x03a0; // greek small letter pi -> greek capital letter pi
   lumap[0x03c1] = 0x03a1; // greek small letter rho -> greek capital letter rho
   lumap[0x03c3] = 0x03a3; // greek small letter sigma -> greek capital letter sigma
   lumap[0x03c4] = 0x03a4; // greek small letter tau -> greek capital letter tau
   lumap[0x03c5] = 0x03a5; // greek small letter upsilon -> greek capital letter upsilon
   lumap[0x03c6] = 0x03a6; // greek small letter phi -> greek capital letter phi
   lumap[0x03c7] = 0x03a7; // greek small letter chi -> greek capital letter chi
   lumap[0x03c8] = 0x03a8; // greek small letter psi -> greek capital letter psi
   lumap[0x03c9] = 0x03a9; // greek small letter omega -> greek capital letter omega
   lumap[0x03ca] = 0x03aa; // greek small letter iota with dialytika -> greek capital letter iota with dialytika
   lumap[0x03cb] = 0x03ab; // greek small letter upsilon with dialytika -> greek capital letter upsilon with dialytika
   lumap[0x03cc] = 0x038c; // greek small letter omicron with tonos -> greek capital letter omicron with tonos
   lumap[0x03cd] = 0x038e; // greek small letter upsilon with tonos -> greek capital letter upsilon with tonos
   lumap[0x03ce] = 0x038f; // greek small letter omega with tonos -> greek capital letter omega with tonos
   lumap[0x03e3] = 0x03e2; // coptic small letter shei -> coptic capital letter shei
   lumap[0x03e5] = 0x03e4; // coptic small letter fei -> coptic capital letter fei
   lumap[0x03e7] = 0x03e6; // coptic small letter khei -> coptic capital letter khei
   lumap[0x03e9] = 0x03e8; // coptic small letter hori -> coptic capital letter hori
   lumap[0x03eb] = 0x03ea; // coptic small letter gangia -> coptic capital letter gangia
   lumap[0x03ed] = 0x03ec; // coptic small letter shima -> coptic capital letter shima
   lumap[0x03ef] = 0x03ee; // coptic small letter dei -> coptic capital letter dei
   lumap[0x0430] = 0x0410; // cyrillic small letter a -> cyrillic capital letter a
   lumap[0x0431] = 0x0411; // cyrillic small letter be -> cyrillic capital letter be
   lumap[0x0432] = 0x0412; // cyrillic small letter ve -> cyrillic capital letter ve
   lumap[0x0433] = 0x0413; // cyrillic small letter ghe -> cyrillic capital letter ghe
   lumap[0x0434] = 0x0414; // cyrillic small letter de -> cyrillic capital letter de
   lumap[0x0435] = 0x0415; // cyrillic small letter ie -> cyrillic capital letter ie
   lumap[0x0436] = 0x0416; // cyrillic small letter zhe -> cyrillic capital letter zhe
   lumap[0x0437] = 0x0417; // cyrillic small letter ze -> cyrillic capital letter ze
   lumap[0x0438] = 0x0418; // cyrillic small letter i -> cyrillic capital letter i
   lumap[0x0439] = 0x0419; // cyrillic small letter short i -> cyrillic capital letter short i
   lumap[0x043a] = 0x041a; // cyrillic small letter ka -> cyrillic capital letter ka
   lumap[0x043b] = 0x041b; // cyrillic small letter el -> cyrillic capital letter el
   lumap[0x043c] = 0x041c; // cyrillic small letter em -> cyrillic capital letter em
   lumap[0x043d] = 0x041d; // cyrillic small letter en -> cyrillic capital letter en
   lumap[0x043e] = 0x041e; // cyrillic small letter o -> cyrillic capital letter o
   lumap[0x043f] = 0x041f; // cyrillic small letter pe -> cyrillic capital letter pe
   lumap[0x0440] = 0x0420; // cyrillic small letter er -> cyrillic capital letter er
   lumap[0x0441] = 0x0421; // cyrillic small letter es -> cyrillic capital letter es
   lumap[0x0442] = 0x0422; // cyrillic small letter te -> cyrillic capital letter te
   lumap[0x0443] = 0x0423; // cyrillic small letter u -> cyrillic capital letter u
   lumap[0x0444] = 0x0424; // cyrillic small letter ef -> cyrillic capital letter ef
   lumap[0x0445] = 0x0425; // cyrillic small letter ha -> cyrillic capital letter ha
   lumap[0x0446] = 0x0426; // cyrillic small letter tse -> cyrillic capital letter tse
   lumap[0x0447] = 0x0427; // cyrillic small letter che -> cyrillic capital letter che
   lumap[0x0448] = 0x0428; // cyrillic small letter sha -> cyrillic capital letter sha
   lumap[0x0449] = 0x0429; // cyrillic small letter shcha -> cyrillic capital letter shcha
   lumap[0x044a] = 0x042a; // cyrillic small letter hard sign -> cyrillic capital letter hard sign
   lumap[0x044b] = 0x042b; // cyrillic small letter yeru -> cyrillic capital letter yeru
   lumap[0x044c] = 0x042c; // cyrillic small letter soft sign -> cyrillic capital letter soft sign
   lumap[0x044d] = 0x042d; // cyrillic small letter e -> cyrillic capital letter e
   lumap[0x044e] = 0x042e; // cyrillic small letter yu -> cyrillic capital letter yu
   lumap[0x044f] = 0x042f; // cyrillic small letter ya -> cyrillic capital letter ya
   lumap[0x0451] = 0x0401; // cyrillic small letter io -> cyrillic capital letter io
   lumap[0x0452] = 0x0402; // cyrillic small letter dje (serbocroatian) -> cyrillic capital letter dje (serbocroatian)
   lumap[0x0453] = 0x0403; // cyrillic small letter gje -> cyrillic capital letter gje
   lumap[0x0454] = 0x0404; // cyrillic small letter ukrainian ie -> cyrillic capital letter ukrainian ie
   lumap[0x0455] = 0x0405; // cyrillic small letter dze -> cyrillic capital letter dze
   lumap[0x0456] = 0x0406; // cyrillic small letter byelorussian-ukrainian i -> cyrillic capital letter byelorussian_ukrainian i
   lumap[0x0457] = 0x0407; // cyrillic small letter yi (ukrainian) -> cyrillic capital letter yi (ukrainian)
   lumap[0x0458] = 0x0408; // cyrillic small letter je -> cyrillic capital letter je
   lumap[0x0459] = 0x0409; // cyrillic small letter lje -> cyrillic capital letter lje
   lumap[0x045a] = 0x040a; // cyrillic small letter nje -> cyrillic capital letter nje
   lumap[0x045b] = 0x040b; // cyrillic small letter tshe (serbocroatian) -> cyrillic capital letter tshe (serbocroatian)
   lumap[0x045c] = 0x040c; // cyrillic small letter kje -> cyrillic capital letter kje
   lumap[0x045e] = 0x040e; // cyrillic small letter short u (byelorussian) -> cyrillic capital letter short u (byelorussian)
   lumap[0x045f] = 0x040f; // cyrillic small letter dzhe -> cyrillic capital letter dzhe
   lumap[0x0461] = 0x0460; // cyrillic small letter omega -> cyrillic capital letter omega
   lumap[0x0463] = 0x0462; // cyrillic small letter yat -> cyrillic capital letter yat
   lumap[0x0465] = 0x0464; // cyrillic small letter iotified e -> cyrillic capital letter iotified e
   lumap[0x0467] = 0x0466; // cyrillic small letter little yus -> cyrillic capital letter little yus
   lumap[0x0469] = 0x0468; // cyrillic small letter iotified little yus -> cyrillic capital letter iotified little yus
   lumap[0x046b] = 0x046a; // cyrillic small letter big yus -> cyrillic capital letter big yus
   lumap[0x046d] = 0x046c; // cyrillic small letter iotified big yus -> cyrillic capital letter iotified big yus
   lumap[0x046f] = 0x046e; // cyrillic small letter ksi -> cyrillic capital letter ksi
   lumap[0x0471] = 0x0470; // cyrillic small letter psi -> cyrillic capital letter psi
   lumap[0x0473] = 0x0472; // cyrillic small letter fita -> cyrillic capital letter fita
   lumap[0x0475] = 0x0474; // cyrillic small letter izhitsa -> cyrillic capital letter izhitsa
   lumap[0x0477] = 0x0476; // cyrillic small letter izhitsa with double grave accent -> cyrillic capital letter izhitsa with double grave accent
   lumap[0x0479] = 0x0478; // cyrillic small letter uk -> cyrillic capital letter uk
   lumap[0x047b] = 0x047a; // cyrillic small letter round omega -> cyrillic capital letter round omega
   lumap[0x047d] = 0x047c; // cyrillic small letter omega with titlo -> cyrillic capital letter omega with titlo
   lumap[0x047f] = 0x047e; // cyrillic small letter ot -> cyrillic capital letter ot
   lumap[0x0481] = 0x0480; // cyrillic small letter koppa -> cyrillic capital letter koppa
   lumap[0x0491] = 0x0490; // cyrillic small letter ghe with upturn -> cyrillic capital letter ghe with upturn
   lumap[0x0493] = 0x0492; // cyrillic small letter ghe with stroke -> cyrillic capital letter ghe with stroke
   lumap[0x0495] = 0x0494; // cyrillic small letter ghe with middle hook -> cyrillic capital letter ghe with middle hook
   lumap[0x0497] = 0x0496; // cyrillic small letter zhe with descender -> cyrillic capital letter zhe with descender
   lumap[0x0499] = 0x0498; // cyrillic small letter ze with descender -> cyrillic capital letter ze with descender
   lumap[0x049b] = 0x049a; // cyrillic small letter ka with descender -> cyrillic capital letter ka with descender
   lumap[0x049d] = 0x049c; // cyrillic small letter ka with vertical stroke -> cyrillic capital letter ka with vertical stroke
   lumap[0x049f] = 0x049e; // cyrillic small letter ka with stroke -> cyrillic capital letter ka with stroke
   lumap[0x04a1] = 0x04a0; // cyrillic small letter eashkir ka -> cyrillic capital letter bashkir ka
   lumap[0x04a3] = 0x04a2; // cyrillic small letter en with descenoer -> cyrillic capital letter en with descender
   lumap[0x04a5] = 0x04a4; // cyrillic small ligature en ghe -> cyrillic capital ligature en ghf
   lumap[0x04a7] = 0x04a6; // cyrillic small letter pe with middle hook (abkhasian) -> cyrillic capital letter pe with middle hook (abkhasian)
   lumap[0x04a9] = 0x04a8; // cyrillic small letter abkhasian ha -> cyrillic capital letter abkhasian ha
   lumap[0x04ab] = 0x04aa; // cyrillic small letter es with descender -> cyrillic capital letter es with descender
   lumap[0x04ad] = 0x04ac; // cyrillic small letter te with descender -> cyrillic capital letter te with descender
   lumap[0x04af] = 0x04ae; // cyrillic small letter straight u -> cyrillic capital letter straight u
   lumap[0x04b1] = 0x04b0; // cyrillic small letter straight u with stroke -> cyrillic capital letter straight u with stroke
   lumap[0x04b3] = 0x04b2; // cyrillic small letter ha with descender -> cyrillic capital letter ha with descender
   lumap[0x04b5] = 0x04b4; // cyrillic small ligature te tse (abkhasian) -> cyrillic capital ligature te tse (abkhasian)
   lumap[0x04b7] = 0x04b6; // cyrillic small letter che with descender -> cyrillic capital letter che with descender
   lumap[0x04b9] = 0x04b8; // cyrillic small letter che with vertical stroke -> cyrillic capital letter che with vertical stroke
   lumap[0x04bb] = 0x04ba; // cyrillic small letter shha -> cyrillic capital letter shha
   lumap[0x04bd] = 0x04bc; // cyrillic small letter abkhasian che -> cyrillic capital letter abkhasian che
   lumap[0x04bf] = 0x04be; // cyrillic small letter abkhasian che with descender -> cyrillic capital letter abkhasian che with descender
   lumap[0x04c2] = 0x04c1; // cyrillic small letter zhe with breve -> cyrillic capital letter zhe with breve
   lumap[0x04c4] = 0x04c3; // cyrillic small letter ka with hook -> cyrillic capital letter ka with hook
   lumap[0x04c8] = 0x04c7; // cyrillic small letter en with hook -> cyrillic capital letter en with hook
   lumap[0x04cc] = 0x04cb; // cyrillic small letter khakassian che -> cyrillic capital letter khakassian che
   lumap[0x04d1] = 0x04d0; // cyrillic small letter a with breve -> cyrillic capital letter a with breve
   lumap[0x04d3] = 0x04d2; // cyrillic small letter a with diaeresis -> cyrillic capital letter a with diaeresis
   lumap[0x04d5] = 0x04d4; // cyrillic small ligature a ie -> cyrillic capital ligature a ie
   lumap[0x04d7] = 0x04d6; // cyrillic small letter ie with breve -> cyrillic capital letter ie with breve
   lumap[0x04d9] = 0x04d8; // cyrillic small letter schwa -> cyrillic capital letter schwa
   lumap[0x04db] = 0x04da; // cyrillic small letter schwa with diaeresis -> cyrillic capital letter schwa with diaeresis
   lumap[0x04dd] = 0x04dc; // cyrillic small letter zhe with diaeresis -> cyrillic capital letter zhe with diaeresis
   lumap[0x04df] = 0x04de; // cyrillic small letter ze with diaeresis -> cyrillic capital letter ze with diaeresis
   lumap[0x04e1] = 0x04e0; // cyrillic small letter abkhasian dze -> cyrillic capital letter abkhasian dze
   lumap[0x04e3] = 0x04e2; // cyrillic small letter i with macron -> cyrillic capital letter i with macron
   lumap[0x04e5] = 0x04e4; // cyrillic small letter i with diaeresis -> cyrillic capital letter i with diaeresis
   lumap[0x04e7] = 0x04e6; // cyrillic small letter o with diaeresis -> cyrillic capital letter o with diaeresis
   lumap[0x04e9] = 0x04e8; // cyrillic small letter barred o -> cyrillic capital letter barred o
   lumap[0x04eb] = 0x04ea; // cyrillic small letter barred o with diaeresis -> cyrillic capital letter barred o with diaeresis
   lumap[0x04ef] = 0x04ee; // cyrillic small letter u with macron -> cyrillic capital letter u with macron
   lumap[0x04f1] = 0x04f0; // cyrillic small letter u with diaeresis -> cyrillic capital letter u with diaeresis
   lumap[0x04f3] = 0x04f2; // cyrillic small letter u with double acute -> cyrillic capital letter u with double acute
   lumap[0x04f5] = 0x04f4; // cyrillic small letter che aith diaeresis -> cyrillic capital letter che with diaeresis
   lumap[0x04f9] = 0x04f8; // cyrillic small letter yeru with diaeresis -> cyrillic capital letter yeru with diaeresis
   lumap[0x0561] = 0x0531; // armenian small letter ayb -> armenian capital letter ayb
   lumap[0x0562] = 0x0532; // armenian small letter ben -> armenian capital letter ben
   lumap[0x0563] = 0x0533; // armenian small letter gim -> armenian capital letter gim
   lumap[0x0564] = 0x0534; // armenian small letter da -> armenian capital letter da
   lumap[0x0565] = 0x0535; // armenian small letter ech -> armenian capital letter ech
   lumap[0x0566] = 0x0536; // armenian small letter za -> armenian capital letter za
   lumap[0x0567] = 0x0537; // armenian small letter eh -> armenian capital letter eh
   lumap[0x0568] = 0x0538; // armenian small letter et -> armenian capital letter et
   lumap[0x0569] = 0x0539; // armenian small letter to -> armenian capital letter to
   lumap[0x056a] = 0x053a; // armenian small letter zhe -> armenian capital letter zhe
   lumap[0x056b] = 0x053b; // armenian small letter ini -> armenian capital letter ini
   lumap[0x056c] = 0x053c; // armenian small letter liwn -> armenian capital letter liwn
   lumap[0x056d] = 0x053d; // armenian small letter xeh -> armenian capital letter xeh
   lumap[0x056e] = 0x053e; // armenian small letter ca -> armenian capital letter ca
   lumap[0x056f] = 0x053f; // armenian small letter ken -> armenian capital letter ken
   lumap[0x0570] = 0x0540; // armenian small letter ho -> armenian capital letter ho
   lumap[0x0571] = 0x0541; // armenian small letter ja -> armenian capital letter ja
   lumap[0x0572] = 0x0542; // armenian small letter ghad -> armenian capital letter ghad
   lumap[0x0573] = 0x0543; // armenian small letter cheh -> armenian capital letter cheh
   lumap[0x0574] = 0x0544; // armenian small letter men -> armenian capital letter men
   lumap[0x0575] = 0x0545; // armenian small letter yi -> armenian capital letter yi
   lumap[0x0576] = 0x0546; // armenian small letter now -> armenian capital letter now
   lumap[0x0577] = 0x0547; // armenian small letter sna -> armenian capital letter sha
   lumap[0x0578] = 0x0548; // armenian small letter vo -> armenian capital letter vo
   lumap[0x0579] = 0x0549; // armenian small letter cha -> armenian capital letter cha
   lumap[0x057a] = 0x054a; // armenian small letter peh -> armenian capital letter peh
   lumap[0x057b] = 0x054b; // armenian small letter jheh -> armenian capital letter jheh
   lumap[0x057c] = 0x054c; // armenian small letter ra -> armenian capital letter ra
   lumap[0x057d] = 0x054d; // armenian small letter seh -> armenian capital letter seh
   lumap[0x057e] = 0x054e; // armenian small letter vew -> armenian capital letter vew
   lumap[0x057f] = 0x054f; // armenian small letter tiwn -> armenian capital letter tiwn
   lumap[0x0580] = 0x0550; // armenian small letter reh -> armenian capital letter reh
   lumap[0x0581] = 0x0551; // armenian small letter co -> armenian capital letter co
   lumap[0x0582] = 0x0552; // armenian small letter yiwn -> armenian capital letter yiwn
   lumap[0x0583] = 0x0553; // armenian small letter piwp -> armenian capital letter piwr
   lumap[0x0584] = 0x0554; // armenian small letter keh -> armenian capital letter keh
   lumap[0x0585] = 0x0555; // armenian small letter oh -> armenian capital letter oh
   lumap[0x0586] = 0x0556; // armenian small letter feh -> armenian capital letter feh
   lumap[0x10d0] = 0x10a0; // georgian letter an -> georgian capital letter an (khutsuri)
   lumap[0x10d1] = 0x10a1; // georgian letter ban -> georgian capital letter ban (khutsuri)
   lumap[0x10d2] = 0x10a2; // georgian letter gan -> georgian capital letter gan (khutsuri)
   lumap[0x10d3] = 0x10a3; // georgian letter don -> georgian capital letter don (khutsuri)
   lumap[0x10d4] = 0x10a4; // georgian letter en -> georgian capital letter en (khutsuri)
   lumap[0x10d5] = 0x10a5; // georgian letter vin -> georgian capital letter vin (khutsuri)
   lumap[0x10d6] = 0x10a6; // georgian letter zen -> georgian capital letter zen (khutsuri)
   lumap[0x10d7] = 0x10a7; // georgian letter tan -> georgian capital letter tan (khutsuri)
   lumap[0x10d8] = 0x10a8; // georgian letter in -> georgian capital letter in (khutsuri)
   lumap[0x10d9] = 0x10a9; // georgian letter kan -> georgian capital letter kan (khutsuri)
   lumap[0x10da] = 0x10aa; // georgian letter las -> georgian capital letter las (khutsuri)
   lumap[0x10db] = 0x10ab; // georgian letter man -> georgian capital letter man (khutsuri)
   lumap[0x10dc] = 0x10ac; // georgian letter nar -> georgian capital letter nar (khutsuri)
   lumap[0x10dd] = 0x10ad; // georgian letter on -> georgian capital letter on (khutsuri)
   lumap[0x10de] = 0x10ae; // georgian letter par -> georgian capital letter par (khutsuri)
   lumap[0x10df] = 0x10af; // georgian letter zhar -> georgian capital letter zhar (khutsuri)
   lumap[0x10e0] = 0x10b0; // georgian letter rae -> georgian capital letter rae (khutsuri)
   lumap[0x10e1] = 0x10b1; // georgian letter san -> georgian capital letter san (khutsuri)
   lumap[0x10e2] = 0x10b2; // georgian letter tar -> georgian capital letter tar (khutsuri)
   lumap[0x10e3] = 0x10b3; // georgian letter un -> georgian capital letter un (khutsuri)
   lumap[0x10e4] = 0x10b4; // georgian letter phar -> georgian capital letter phar (khutsuri)
   lumap[0x10e5] = 0x10b5; // georgian letter khar -> georgian capital letter khar (khutsuri)
   lumap[0x10e6] = 0x10b6; // georgian letter ghan -> georgian capital letter ghan (khutsuri)
   lumap[0x10e7] = 0x10b7; // georgian letter qar -> georgian capital letter qar (khutsuri)
   lumap[0x10e8] = 0x10b8; // georgian letter shin -> georgian capital letter shin (khutsuri)
   lumap[0x10e9] = 0x10b9; // georgian letter chin -> georgian capital letter chin (khutsuri)
   lumap[0x10ea] = 0x10ba; // georgian letter can -> georgian capital letter can (khutsuri)
   lumap[0x10eb] = 0x10bb; // georgian letter jil -> georgian capital letter jil (khutsuri)
   lumap[0x10ec] = 0x10bc; // georgian letter cil -> georgian capital letter cil (khutsuri)
   lumap[0x10ed] = 0x10bd; // georgian letter char -> georgian capital letter char (khutsuri)
   lumap[0x10ee] = 0x10be; // georgian letter xan -> georgian capital letter xan (khutsuri)
   lumap[0x10ef] = 0x10bf; // georgian letter jhan -> georgian capital letter jhan (khutsuri)
   lumap[0x10f0] = 0x10c0; // georgian letter hae -> georgian capital letter hae (khutsuri)
   lumap[0x10f1] = 0x10c1; // georgian letter he -> georgian capital letter he (khutsuri)
   lumap[0x10f2] = 0x10c2; // georgian letter hie -> georgian capital letter hie (khutsuri)
   lumap[0x10f3] = 0x10c3; // georgian letter we -> georgian capital letter we (khutsuri)
   lumap[0x10f4] = 0x10c4; // georgian letter har -> georgian capital letter har (khutsuri)
   lumap[0x10f5] = 0x10c5; // georgian letter hoe -> georgian capital letter hoe (khutsuri)
   lumap[0x1e01] = 0x1e00; // latin small letter a with ring below -> latin capital letter a with ring below
   lumap[0x1e03] = 0x1e02; // latin small letter b with dot above -> latin capital letter b with dot above
   lumap[0x1e05] = 0x1e04; // latin small letter b with dot below -> latin capital letter b with dot below
   lumap[0x1e07] = 0x1e06; // latin small letter b with line below -> latin capital letter b with line below
   lumap[0x1e09] = 0x1e08; // latin small letter c with cedilla and acute -> latin capital letter c with cedilla and acute
   lumap[0x1e0b] = 0x1e0a; // latin small letter d with dot above -> latin capital letter d with dot above
   lumap[0x1e0d] = 0x1e0c; // latin small letter d with dot below -> latin capital letter d with dot below
   lumap[0x1e0f] = 0x1e0e; // latin small letter d with line below -> latin capital letter d with line below
   lumap[0x1e11] = 0x1e10; // latin small letter d with cedilla -> latin capital letter d with cedilla
   lumap[0x1e13] = 0x1e12; // latin small letter d with circumflex below -> latin capital letter d with circumflex below
   lumap[0x1e15] = 0x1e14; // latin small letter e with macron and grave -> latin capital letter e with macron and grave
   lumap[0x1e17] = 0x1e16; // latin small letter e with macron and acute -> latin capital letter e with macron and acute
   lumap[0x1e19] = 0x1e18; // latin small letter e with circumflex below -> latin capital letter e with circumflex below
   lumap[0x1e1b] = 0x1e1a; // latin small letter e with tilde below -> latin capital letter e with tilde below
   lumap[0x1e1d] = 0x1e1c; // latin small letter e with cedilla and breve -> latin capital letter e with cedilla and breve
   lumap[0x1e1f] = 0x1e1e; // latin small letter f with dot above -> latin capital letter f with dot above
   lumap[0x1e21] = 0x1e20; // latin small letter g with macron -> latin capital letter g with macron
   lumap[0x1e23] = 0x1e22; // latin small letter h with dot above -> latin capital letter h with dot above
   lumap[0x1e25] = 0x1e24; // latin small letter h with dot below -> latin capital letter h with dot below
   lumap[0x1e27] = 0x1e26; // latin small letter h with diaeresis -> latin capital letter h with diaeresis
   lumap[0x1e29] = 0x1e28; // latin small letter h with cedilla -> latin capital letter h with cedilla
   lumap[0x1e2b] = 0x1e2a; // latin small letter h with breve below -> latin capital letter h with breve below
   lumap[0x1e2d] = 0x1e2c; // latin small letter i with tilde below -> latin capital letter i with tilde below
   lumap[0x1e2f] = 0x1e2e; // latin small letter i with diaeresis and acute -> latin capital letter i with diaeresis and acute
   lumap[0x1e31] = 0x1e30; // latin small letter k with acute -> latin capital letter k with acute
   lumap[0x1e33] = 0x1e32; // latin small letter k with dot below -> latin capital letter k with dot below
   lumap[0x1e35] = 0x1e34; // latin small letter k with line below -> latin capital letter k with line below
   lumap[0x1e37] = 0x1e36; // latin small letter l with dot below -> latin capital letter l with dot below
   lumap[0x1e39] = 0x1e38; // latin small letter l with dot below and macron -> latin capital letter l with dot below and macron
   lumap[0x1e3b] = 0x1e3a; // latin small letter l with line below -> latin capital letter l with line below
   lumap[0x1e3d] = 0x1e3c; // latin small letter l with circumflex below -> latin capital letter l with circumflex below
   lumap[0x1e3f] = 0x1e3e; // latin small letter m with acute -> latin capital letter m with acute
   lumap[0x1e41] = 0x1e40; // latin small letter m with dot above -> latin capital letter m with dot above
   lumap[0x1e43] = 0x1e42; // latin small letter m with dot below -> latin capital letter m with dot below
   lumap[0x1e45] = 0x1e44; // latin small letter n with dot above -> latin capital letter n with dot above
   lumap[0x1e47] = 0x1e46; // latin small letter n with dot below -> latin capital letter n with dot below
   lumap[0x1e49] = 0x1e48; // latin small letter n with line below -> latin capital letter n with line below
   lumap[0x1e4b] = 0x1e4a; // latin small letter n with circumflex below -> latin capital letter n with circumflex below
   lumap[0x1e4d] = 0x1e4c; // latin small letter o with tilde and acute -> latin capital letter o with tilde and acute
   lumap[0x1e4f] = 0x1e4e; // latin small letter o with tllde and diaeresis -> latin capital letter o with tilde and diaeresis
   lumap[0x1e51] = 0x1e50; // latin small letter o with macron and grave -> latin capital letter o with macron and grave
   lumap[0x1e53] = 0x1e52; // latin small letter o with macron and acute -> latin capital letter o with macron and acute
   lumap[0x1e55] = 0x1e54; // latin small letter p with acute -> latin capital letter p with acute
   lumap[0x1e57] = 0x1e56; // latin small letter p with dot above -> latin capital letter p with dot above
   lumap[0x1e59] = 0x1e58; // latin small letter r with dot above -> latin capital letter r with dot above
   lumap[0x1e5b] = 0x1e5a; // latin small letter r with dot below -> latin capital letter r with dot below
   lumap[0x1e5d] = 0x1e5c; // latin small letter r with dot below and macron -> latin capital letter r with dot below and macron
   lumap[0x1e5f] = 0x1e5e; // latin small letter r with line below -> latin capital letter r with line below
   lumap[0x1e61] = 0x1e60; // latin small letter s with dot above -> latin capital letter s with dot above
   lumap[0x1e63] = 0x1e62; // latin small letter s with dot below -> latin capital letter s with dot below
   lumap[0x1e65] = 0x1e64; // latin small letter s with acute and dot above -> latin capital letter s with acute and dot above
   lumap[0x1e67] = 0x1e66; // latin small letter s with caron and dot above -> latin capital letter s with caron and dot above
   lumap[0x1e69] = 0x1e68; // latin small letter s with dot below and dot above -> latin capital letter s with dot below and dot above
   lumap[0x1e6b] = 0x1e6a; // latin small letter t with dot above -> latin capital letter t with dot above
   lumap[0x1e6d] = 0x1e6c; // latin small letter t with dot below -> latin capital letter t with dot below
   lumap[0x1e6f] = 0x1e6e; // latin small letter t with line below -> latin capital letter t with line below
   lumap[0x1e71] = 0x1e70; // latin small letter t with circumflex below -> latin capital letter t with circumflex below
   lumap[0x1e73] = 0x1e72; // latin small letter u with diaeresis below -> latin capital letter u with diaeresis below
   lumap[0x1e75] = 0x1e74; // latin small letter u with tilde below -> latin capital letter u with tilde below
   lumap[0x1e77] = 0x1e76; // latin small letter u with circumflex below -> latin capital letter u with circumflex below
   lumap[0x1e79] = 0x1e78; // latin small letter u with tilde and acute -> latin capital letter u with tilde and acute
   lumap[0x1e7b] = 0x1e7a; // latin small letter u with macron and diaeresis -> latin capital letter u with macron and diaeresis
   lumap[0x1e7d] = 0x1e7c; // latin small letter v with tilde -> latin capital letter v with tilde
   lumap[0x1e7f] = 0x1e7e; // latin small letter v with dot below -> latin capital letter v with dot below
   lumap[0x1e81] = 0x1e80; // latin small letter w with grave -> latin capital letter w with grave
   lumap[0x1e83] = 0x1e82; // latin small letter w with acute -> latin capital letter w with acute
   lumap[0x1e85] = 0x1e84; // latin small letter w with diaeresis -> latin capital letter w with diaeresis
   lumap[0x1e87] = 0x1e86; // latin small letter w with dot above -> latin capital letter w with dot above
   lumap[0x1e89] = 0x1e88; // latin small letter w with dot below -> latin capital letter w with dot below
   lumap[0x1e8b] = 0x1e8a; // latin small letter x with dot above -> latin capital letter x with dot above
   lumap[0x1e8d] = 0x1e8c; // latin small letter x with diaeresis -> latin capital letter x5 with diaeresis
   lumap[0x1e8f] = 0x1e8e; // latin small letter y with dot above -> latin capital letter y with dot above
   lumap[0x1e91] = 0x1e90; // latin small letter z with circumflex -> latin capital letter z with circumflex
   lumap[0x1e93] = 0x1e92; // latin small letter z with dot below -> latin capital letter z with dot below
   lumap[0x1e95] = 0x1e94; // latin small letter z with line below -> latin capital letter z with line below
   lumap[0x1ea1] = 0x1ea0; // latin small letter a with dot below -> latin capital letter a with dot below
   lumap[0x1ea3] = 0x1ea2; // latin small letter a with hook above -> latin capital letter a with hook above
   lumap[0x1ea5] = 0x1ea4; // latin small letter a with circumflex and acute -> latin capital letter a with circumflex and acute
   lumap[0x1ea7] = 0x1ea6; // latin small letter a with circumflex and grave -> latin capital letter a with circumflex and grave
   lumap[0x1ea9] = 0x1ea8; // latin small letter a with circumflex and hook above -> latin capital letter a with circumflex and hook above
   lumap[0x1eab] = 0x1eaa; // latin small letter a with circumflex and tilde -> latin capital letter a with circumflex and tilde
   lumap[0x1ead] = 0x1eac; // latin small letter a with circumflex and dot below -> latin capital letter a with circumflex and dot below
   lumap[0x1eaf] = 0x1eae; // latin small letter a with breve and acute -> latin capital letter a with breve and acute
   lumap[0x1eb1] = 0x1eb0; // latin small letter a with breve and grave -> latin capital letter a with breve and grave
   lumap[0x1eb3] = 0x1eb2; // latin small letter a with breve and hook above -> latin capital letter a with breve and hook above
   lumap[0x1eb5] = 0x1eb4; // latin small letter a with breve and tilde -> latin capital letter a with breve and tilde
   lumap[0x1eb7] = 0x1eb6; // latin small letter a with breve and dot below -> latin capital letter a with breve and dot below
   lumap[0x1eb9] = 0x1eb8; // latin small letter e with dot below -> latin capital letter e with dot below
   lumap[0x1ebb] = 0x1eba; // latin small letter e with hook above -> latin capital letter e with hook above
   lumap[0x1ebd] = 0x1ebc; // latin small letter e with tilde -> latin capital letter e with tilde
   lumap[0x1ebf] = 0x1ebe; // latin small letter e with circumflex and acute -> latin capital letter e with circumflex and acute
   lumap[0x1ec1] = 0x1ec0; // latin small letter e with circumflex and grave -> latin capital letter e with circumflex and grave
   lumap[0x1ec3] = 0x1ec2; // latin small letter e with circumflex and hook above -> latin capital letter e with circumflex and hook above
   lumap[0x1ec5] = 0x1ec4; // latin small letter e with circumflex and tilde -> latin capital letter e with circumflex and tilde
   lumap[0x1ec7] = 0x1ec6; // latin small letter e with circumflex and dot below -> latin capital letter e with circumflex and dot below
   lumap[0x1ec9] = 0x1ec8; // latin small letter i with hook above -> latin capital letter i with hook above
   lumap[0x1ecb] = 0x1eca; // latin small letter i with dot below -> latin capital letter i with dot below
   lumap[0x1ecd] = 0x1ecc; // latin small letter o with dot below -> latin capital letter o with dot below
   lumap[0x1ecf] = 0x1ece; // latin small letter o with hook above -> latin capital letter o with hook above
   lumap[0x1ed1] = 0x1ed0; // latin small letter o with circumflex and acute -> latin capital letter o with circumflex and acute
   lumap[0x1ed3] = 0x1ed2; // latin small letter o with circumflex and grave -> latin capital letter o with circumflex and grave
   lumap[0x1ed5] = 0x1ed4; // latin small letter o with circumflex and hook above -> latin capital letter o with circumflex and hook above
   lumap[0x1ed7] = 0x1ed6; // latin small letter o with circumflex and tilde -> latin capital letter o with circumflex and tilde
   lumap[0x1ed9] = 0x1ed8; // latin small letter o with circumflex and dot below -> latin capital letter o with circumflex and dot below
   lumap[0x1edb] = 0x1eda; // latin small letter o with horn and acute -> latin capital letter o with horn and acute
   lumap[0x1edd] = 0x1edc; // latin small letter o with horn and grave -> latin capital letter o with horn and grave
   lumap[0x1edf] = 0x1ede; // latin small letter o with horn and hook above -> latin capital letter o with horn and hook above
   lumap[0x1ee1] = 0x1ee0; // latin small letter o with horn and tilde -> latin capital letter o with horn and tilde
   lumap[0x1ee3] = 0x1ee2; // latin small letter o with horn and dot below -> latin capital letter o with horn and dot below
   lumap[0x1ee5] = 0x1ee4; // latin small letter u with dot below -> latin capital letter u with dot below
   lumap[0x1ee7] = 0x1ee6; // latin small letter u with hook above -> latin capital letter u with hook above
   lumap[0x1ee9] = 0x1ee8; // latin small letter u with horn and acute -> latin capital letter u with horn and acute
   lumap[0x1eeb] = 0x1eea; // latin small letter u with horn and grave -> latin capital letter u with horn and grave
   lumap[0x1eed] = 0x1eec; // latin small letter u with horn and hock above -> latin capital letter u with horn and hook above
   lumap[0x1eef] = 0x1eee; // latin small letter u with horn and tilde -> latin capital letter u with horn and tilde
   lumap[0x1ef1] = 0x1ef0; // latin small letter u with horn and dot below -> latin capital letter u with horn and dot below
   lumap[0x1ef3] = 0x1ef2; // latin small letter y with grave -> latin capital letter y with grave
   lumap[0x1ef5] = 0x1ef4; // latin small letter y with dot below -> latin capital letter y with dot below
   lumap[0x1ef7] = 0x1ef6; // latin small letter y with hook above -> latin capital letter y with hook above
   lumap[0x1ef9] = 0x1ef8; // latin small letter y with tilde -> latin capital letter y with tilde
   lumap[0x1f00] = 0x1f08; // greek small letter alpha with psili -> greek capital letter alpha with psili
   lumap[0x1f01] = 0x1f09; // greek small letter alpha with dasia -> greek capital letter alpha with dasia
   lumap[0x1f02] = 0x1f0a; // greek small letter alpha with psili and varia -> greek capital letter alpha with psili and varia
   lumap[0x1f03] = 0x1f0b; // greek small letter alpha with dasia and varia -> greek capital letter alpha with dasia and varia
   lumap[0x1f04] = 0x1f0c; // greek small letter alpha with psili and oxia -> greek capital letter alpha with psili and oxia
   lumap[0x1f05] = 0x1f0d; // greek small letter alpha with dasia and oxia -> greek capital letter alpha with dasia and oxia
   lumap[0x1f06] = 0x1f0e; // greek small letter alpha with psili and perispomeni -> greek capital letter alpha with psili and perispomeni
   lumap[0x1f07] = 0x1f0f; // greek small letter alpha with dasia and perispomeni -> greek capital letter alpha with dasia and perispomeni
   lumap[0x1f10] = 0x1f18; // greek small letter epsilon with psili -> greek capital letter epsilon with psili
   lumap[0x1f11] = 0x1f19; // greek small letter epsilon with dasia -> greek capital letter epsilon with dasia
   lumap[0x1f12] = 0x1f1a; // greek small letter epsilon with psili and varia -> greek capital letter epsilon with psili and varia
   lumap[0x1f13] = 0x1f1b; // greek small letter epsilon with dasia and varia -> greek capital letter epsilon with dasia and varia
   lumap[0x1f14] = 0x1f1c; // greek small letter epsilon with psili and oxia -> greek capital letter epsilon with psili and oxia
   lumap[0x1f15] = 0x1f1d; // greek small letter epsilon with dasia and oxia -> greek capital letter epsilon with dasia and oxia
   lumap[0x1f20] = 0x1f28; // greek small letter eta with psili -> greek capital letter eta with psili
   lumap[0x1f21] = 0x1f29; // greek small letter eta with dasia -> greek capital letter eta with dasia
   lumap[0x1f22] = 0x1f2a; // greek small letter eta with psili and varia -> greek capital letter eta with psili and varia
   lumap[0x1f23] = 0x1f2b; // greek small letter eta with dasia and varia -> greek capital letter eta with dasia and varia
   lumap[0x1f24] = 0x1f2c; // greek small letter eta with psili and oxia -> greek capital letter eta with psili and oxia
   lumap[0x1f25] = 0x1f2d; // greek small letter eta with dasia and oxia -> greek capital letter eta with dasia and oxia
   lumap[0x1f26] = 0x1f2e; // greek small letter eta with psili and perispomeni -> greek capital letter eta with psili and perispomeni
   lumap[0x1f27] = 0x1f2f; // greek small letter eta with dasia and perispomeni -> greek capital letter eta with dasia and perispomeni
   lumap[0x1f30] = 0x1f38; // greek small letter iota with psili -> greek capital letter iota with psili
   lumap[0x1f31] = 0x1f39; // greek small letter iota with dasia -> greek capital letter iota with dasia
   lumap[0x1f32] = 0x1f3a; // greek small letter iota with psili and varia -> greek capital letter iota with psili and varia
   lumap[0x1f33] = 0x1f3b; // greek small letter iota with dasia and varia -> greek capital letter iota with dasia and varia
   lumap[0x1f34] = 0x1f3c; // greek small letter iota with psili and oxia -> greek capital letter iota with psili and oxia
   lumap[0x1f35] = 0x1f3d; // greek small letter iota with dasia and oxia -> greek capital letter iota with dasia and oxia
   lumap[0x1f36] = 0x1f3e; // greek small letter iota with psili and perispomeni -> greek capital letter iota with psili and perispomeni
   lumap[0x1f37] = 0x1f3f; // greek small letter iota with dasia and perispomeni -> greek capital letter iota with dasia and perispomeni
   lumap[0x1f40] = 0x1f48; // greek small letter omicron with psili -> greek capital letter omicron with psili
   lumap[0x1f41] = 0x1f49; // greek small letter omicron with dasia -> greek capital letter omicron with dasia
   lumap[0x1f42] = 0x1f4a; // greek small letter omicron with psili and varia -> greek capital letter omicron with psili and varia
   lumap[0x1f43] = 0x1f4b; // greek small letter omicron with dasia and varia -> greek capital letter omicron with dasia and varia
   lumap[0x1f44] = 0x1f4c; // greek small letter omicron with psili and oxia -> greek capital letter omicron with psili and oxia
   lumap[0x1f45] = 0x1f4d; // greek small letter omicron with dasia and oxia -> greek capital letter omicron with dasia and oxia
   lumap[0x1f51] = 0x1f59; // greek small letter upsilon with dasia -> greek capital letter upsilon with oasis
   lumap[0x1f53] = 0x1f5b; // greek small letter upsilon with dasia and varia -> greek capital letter upsilon with dasia and varia
   lumap[0x1f55] = 0x1f5d; // greek small letter upsilon with dasia and oxia -> greek capital letter upsilon with dasia and oxia
   lumap[0x1f57] = 0x1f5f; // greek small letter upsilon with dasia and perispomeni -> greek capital letter upsilon with dasia and perispomeni
   lumap[0x1f60] = 0x1f68; // greek small letter omega with psili -> greek capital letter omega with psili
   lumap[0x1f61] = 0x1f69; // greek small letter omega with dasia -> greek capital letter omega with dasia
   lumap[0x1f62] = 0x1f6a; // greek small letter omega with psili and varia -> greek capital letter omega with psili and varia
   lumap[0x1f63] = 0x1f6b; // greek small letter omega with dasia and varia -> greek capital letter omega with dasia and varia
   lumap[0x1f64] = 0x1f6c; // greek small letter omega with psili and oxia -> greek capital letter omega with psili and oxia
   lumap[0x1f65] = 0x1f6d; // greek small letter omega with dasia and oxia -> greek capital letter omega with dasia and oxia
   lumap[0x1f66] = 0x1f6e; // greek small letter omega with psili and perispomeni -> greek capital letter omega with psili and perispomeni
   lumap[0x1f67] = 0x1f6f; // greek small letter omega with dasia and perispomeni -> greek capital letter omega with dasia and perispomeni
   lumap[0x1f80] = 0x1f88; // greek small letter alpha with psili and ypogegrammeni -> greek capital letter alpha with psili and prosgegrammeni
   lumap[0x1f81] = 0x1f89; // greek small letter alpha with dasia and ypogegrammeni -> greek capital letter alpha with dasia and prosgegrammeni
   lumap[0x1f82] = 0x1f8a; // greek small letter alpha with psili and varia and ypogegrammeni -> greek capital letter alpha with psili and varia and prosgegrammeni
   lumap[0x1f83] = 0x1f8b; // greek small letter alpha with dasia and varia and ypogegrammeni -> greek capital letter alpha with dasia and varia and prosgegrammeni
   lumap[0x1f84] = 0x1f8c; // greek small letter alpha with psili and oxia and ypogegrammeni -> greek capital letter alpha with psili and oxia and prosgegrammen
   lumap[0x1f85] = 0x1f8d; // greek small letter alpha with dasia and oxia and ypogegrammeni -> greek capital letter alpha with dasia and oxia and prosgegrammen
   lumap[0x1f86] = 0x1f8e; // greek small letter alpha with psili and perispomeni and ypogegrammeni -> greek capital letter alpha with psili and perispomeni and prosgegrammeni
   lumap[0x1f87] = 0x1f8f; // greek small letter alpha with dasia and perispomeni and ypogegrammeni -> greek capital letter alpha with dasia and perispomeni and prosgegrammeni
   lumap[0x1f90] = 0x1f98; // greek small letter eta with psili and ypogegrammeni -> greek capital letter eta with psili and prosgegrammeni
   lumap[0x1f91] = 0x1f99; // greek small letter eta with dasia and ypogegrammeni -> greek capital letter eta with dasia and prosgegrammeni
   lumap[0x1f92] = 0x1f9a; // greek small letter eta with psili and varia and ypogegrammeni -> greek capital letter eta with psili and varia and prosgegrammeni
   lumap[0x1f93] = 0x1f9b; // greek small letter eta with dasia and varia and ypogegrammeni -> greek capital letter eta with dasia and varia and prosgegrammeni
   lumap[0x1f94] = 0x1f9c; // greek small letter eta with psili and oxia and ypogegrammeni -> greek capital letter eta with psili and oxia and prosgegrammeni
   lumap[0x1f95] = 0x1f9d; // greek small letter eta with dasia and oxia and ypogegrammeni -> greek capital letter eta with dasia and oxia and prosgegrammeni
   lumap[0x1f96] = 0x1f9e; // greek small letter eta with psili and perispomeni and ypogegrammeni -> greek capital letter eta with psili and perispomeni and prosgegrammeni
   lumap[0x1f97] = 0x1f9f; // greek small letter eta with dasia and perispomeni and ypogegrammeni -> greek capital letter eta with dasia and perispomeni and prosgegrammeni
   lumap[0x1fa0] = 0x1fa8; // greek small letter omega with psili and ypogegrammeni -> greek capital letter omega with psili and prosgegrammeni
   lumap[0x1fa1] = 0x1fa9; // greek small letter omega with dasia and ypogegrammeni -> greek capital letter omega with dasia and prosgegrammeni
   lumap[0x1fa2] = 0x1faa; // greek small letter omega with psili and varia and ypogegrammeni -> greek capital letter omega with psili and varia and prosgegrammeni
   lumap[0x1fa3] = 0x1fab; // greek small letter omega with dasia and varia and ypogegrammeni -> greek capital letter omega with dasia and varia and prosgegrammeni
   lumap[0x1fa4] = 0x1fac; // greek small letter omega with psili and oxia and ypogegrammeni -> greek capital letter omega with psili and oxia and prosgegrammeni
   lumap[0x1fa5] = 0x1fad; // greek small letter omega with dasia and oxia and ypogegrammeni -> greek capital letter omega with dasia and oxia and prosgegrammeni
   lumap[0x1fa6] = 0x1fae; // greek small letter omega with psili and perispomeni and ypogegrammeni -> greek capital letter omega with psili and perispomeni and prosgegrammeni
   lumap[0x1fa7] = 0x1faf; // greek small letter omega with dasia and pepispomeni and ypogegrammeni -> greek capital letter omeca with dasia and perispomeni and prosgegrammeni
   lumap[0x1fb0] = 0x1fb8; // greek small letter alpha with vrachy -> greek capital letter alpha with vrachy
   lumap[0x1fb1] = 0x1fb9; // greek small letter alpha with macron -> greek capital letter alpha with macron
   lumap[0x1fd0] = 0x1fd8; // greek small letter iota with vrachy -> greek capital letter iota with vrachy
   lumap[0x1fd1] = 0x1fd9; // greek small letter iota with macron -> greek capital letter iota with macron
   lumap[0x1fe0] = 0x1fe8; // greek small letter upsilon with vrachy -> greek capital letter upsilon with vrachy
   lumap[0x1fe1] = 0x1fe9; // greek small letter upsilon with macron -> greek capital letter upsilon with macron
   lumap[0x24d0] = 0x24b6; // circled latin small letter a -> circled latin capital letter a
   lumap[0x24d1] = 0x24b7; // circled latin small letter b -> circled latin capital letter b
   lumap[0x24d2] = 0x24b8; // circled latin small letter c -> circled latin capital letter c
   lumap[0x24d3] = 0x24b9; // circled latin small letter d -> circled latin capital letter d
   lumap[0x24d4] = 0x24ba; // circled latin small letter e -> circled latin capital letter e
   lumap[0x24d5] = 0x24bb; // circled latin small letter f -> circled latin capital letter f
   lumap[0x24d6] = 0x24bc; // circled latin small letter g -> circled latin capital letter g
   lumap[0x24d7] = 0x24bd; // circled latin small letter h -> circled latin capital letter h
   lumap[0x24d8] = 0x24be; // circled latin small letter i -> circled latin capital letter i
   lumap[0x24d9] = 0x24bf; // circled latin small letter j -> circled latin capital letter j
   lumap[0x24da] = 0x24c0; // circled latin small letter k -> circled latin capital letter k
   lumap[0x24db] = 0x24c1; // circled latin small letter l -> circled latin capital letter l
   lumap[0x24dc] = 0x24c2; // circled latin small letter m -> circled latin capital letter m
   lumap[0x24dd] = 0x24c3; // circled latin small letter n -> circled latin capital letter n
   lumap[0x24de] = 0x24c4; // circled latin small letter o -> circled latin capital letter o
   lumap[0x24df] = 0x24c5; // circled latin small letter p -> circled latin capital letter p
   lumap[0x24e0] = 0x24c6; // circled latin small letter q -> circled latin capital letter q
   lumap[0x24e1] = 0x24c7; // circled latin small letter r -> circled latin capital letter r
   lumap[0x24e2] = 0x24c8; // circled latin small letter s -> circled latin capital letter s
   lumap[0x24e3] = 0x24c9; // circled latin small letter t -> circled latin capital letter t
   lumap[0x24e4] = 0x24ca; // circled latin small letter u -> circled latin capital letter u
   lumap[0x24e5] = 0x24cb; // circled latin small letter v -> circled latin capital letter v
   lumap[0x24e6] = 0x24cc; // circled latin small letter w -> circled latin capital letter w
   lumap[0x24e7] = 0x24cd; // circled latin small letter x -> circled latin capital letter x
   lumap[0x24e8] = 0x24ce; // circled latin small letter y -> circled latin capital letter y
   lumap[0x24e9] = 0x24cf; // circled latin small letter z -> circled latin capital letter z
   lumap[0xff41] = 0xff21; // fullwidth latin small letter a -> fullwidth latin capital letter a
   lumap[0xff42] = 0xff22; // fullwidth latin small letter b -> fullwidth latin capital letter b
   lumap[0xff43] = 0xff23; // fullwidth latin small letter c -> fullwidth latin capital letter c
   lumap[0xff44] = 0xff24; // fullwidth latin small letter d -> fullwidth latin capital letter d
   lumap[0xff45] = 0xff25; // fullwidth latin small letter e -> fullwidth latin capital letter e
   lumap[0xff46] = 0xff26; // fullwidth latin small letter f -> fullwidth latin capital letter f
   lumap[0xff47] = 0xff27; // fullwidth latin small letter g -> fullwidth latin capital letter g
   lumap[0xff48] = 0xff28; // fullwidth latin small letter h -> fullwidth latin capital letter h
   lumap[0xff49] = 0xff29; // fullwidth latin small letter i -> fullwidth latin capital letter i
   lumap[0xff4a] = 0xff2a; // fullwidth latin small letter j -> fullwidth latin capital letter j
   lumap[0xff4b] = 0xff2b; // fullwidth latin small letter k -> fullwidth latin capital letter k
   lumap[0xff4c] = 0xff2c; // fullwidth latin small letter l -> fullwidth latin capital letter l
   lumap[0xff4d] = 0xff2d; // fullwidth latin small letter m -> fullwidth latin capital letter m
   lumap[0xff4e] = 0xff2e; // fullwidth latin small letter n -> fullwidth latin capital letter n
   lumap[0xff4f] = 0xff2f; // fullwidth latin small letter o -> fullwidth latin capital letter o
   lumap[0xff50] = 0xff30; // fullwidth latin small letter p -> fullwidth latin capital letter p
   lumap[0xff51] = 0xff31; // fullwidth latin small letter q -> fullwidth latin capital letter q
   lumap[0xff52] = 0xff32; // fullwidth latin small letter r -> fullwidth latin capital letter r
   lumap[0xff53] = 0xff33; // fullwidth latin small letter s -> fullwidth latin capital letter s
   lumap[0xff54] = 0xff34; // fullwidth latin small letter t -> fullwidth latin capital letter t
   lumap[0xff55] = 0xff35; // fullwidth latin small letter u -> fullwidth latin capital letter u
   lumap[0xff56] = 0xff36; // fullwidth latin small letter v -> fullwidth latin capital letter v
   lumap[0xff57] = 0xff37; // fullwidth latin small letter w -> fullwidth latin capital letter w
   lumap[0xff58] = 0xff38; // fullwidth latin small letter x -> fullwidth latin capital letter x
   lumap[0xff59] = 0xff39; // fullwidth latin small letter y -> fullwidth latin capital letter y
   lumap[0xff5a] = 0xff3a; // fullwidth latin small letter z -> fullwidth latin capital letter z

   ulmap[0x00c0] = 0x00e0; // latin capital letter a grave -> latin small letter a grave
   ulmap[0x00c1] = 0x00e1; // latin capital letter a acute -> latin small letter a grave
   ulmap[0x00c2] = 0x00e2; // latin capital letter a circumflex -> latin small letter a grave
   ulmap[0x00c3] = 0x00e3; // latin capital letter a tilde -> latin small letter a grave
   ulmap[0x00c4] = 0x00e4; // latin capital letter a diaeresis -> latin small letter a grave
   ulmap[0x00c5] = 0x00e5; // latin capital letter a ring -> latin small letter a grave
   ulmap[0x00c6] = 0x00e6; // latin capital letter a e -> latin small letter a grave
   ulmap[0x00c7] = 0x00e7; // latin capital letter c cedilla -> latin small letter a grave
   ulmap[0x00c8] = 0x00e8; // latin capital letter e grave -> latin small letter a grave
   ulmap[0x00c9] = 0x00e9; // latin capital letter e acute -> latin small letter a grave
   ulmap[0x00ca] = 0x00ea; // latin capital letter e circumflex -> latin small letter e circumflex
   ulmap[0x00cb] = 0x00eb; // latin capital letter e diaeresis -> latin small letter e diaeresis
   ulmap[0x00cc] = 0x00ec; // latin capital letter i grave -> latin small letter i grave
   ulmap[0x00cd] = 0x00ed; // latin capital letter i acute -> latin small letter i acute
   ulmap[0x00ce] = 0x00ee; // latin capital letter i circumflex -> latin small letter i circumflex
   ulmap[0x00cf] = 0x00ef; // latin capital letter i diaeresis -> latin small letter i diaeresis
   ulmap[0x00d0] = 0x00f0; // latin capital letter eth -> latin small letter eth
   ulmap[0x00d1] = 0x00f1; // latin capital letter n tilde -> latin small letter n tilde
   ulmap[0x00d2] = 0x00f2; // latin capital letter o grave -> latin small letter o grave
   ulmap[0x00d3] = 0x00f3; // latin capital letter o acute -> latin small letter o acute
   ulmap[0x00d4] = 0x00f4; // latin capital letter o circumflex -> latin small letter o circumflex
   ulmap[0x00d5] = 0x00f5; // latin capital letter o tilde -> latin small letter o tilde
   ulmap[0x00d6] = 0x00f6; // latin capital letter o diaeresis -> latin small letter o diaeresis
   ulmap[0x00d8] = 0x00f8; // latin capital letter o slash -> latin small letter o slash
   ulmap[0x00d9] = 0x00f9; // latin capital letter u grave -> latin small letter u grave
   ulmap[0x00da] = 0x00fa; // latin capital letter u acute -> latin small letter u acute
   ulmap[0x00db] = 0x00fb; // latin capital letter u circumflex -> latin small letter u circumflex
   ulmap[0x00dc] = 0x00fc; // latin capital letter u diaeresis -> latin small letter u diaeresis
   ulmap[0x00dd] = 0x00fd; // latin capital letter y acute -> latin small letter y acute
   ulmap[0x00de] = 0x00fe; // latin capital letter thorn -> latin small letter thorn
   ulmap[0x0178] = 0x00ff; // latin capital letter y with diaeresis -> latin small letter y diaeresis
   ulmap[0x0100] = 0x0101; // latin capital letter a with macron -> latin small letter a with macron
   ulmap[0x0102] = 0x0103; // latin capital letter a with breve -> latin small letter a with breve
   ulmap[0x0104] = 0x0105; // latin capital letter a with ogonek -> latin small letter a with ogonek
   ulmap[0x0106] = 0x0107; // latin capital letter c with acute -> latin small letter c with acute
   ulmap[0x0108] = 0x0109; // latin capital letter c with circumflex -> latin small letter c with circumflex
   ulmap[0x010a] = 0x010b; // latin capital letter c with dot above -> latin small letter c with dot above
   ulmap[0x010c] = 0x010d; // latin capital letter c with caron -> latin small letter c with caron
   ulmap[0x010e] = 0x010f; // latin capital letter d with caron -> latin small letter d with caron
   ulmap[0x0110] = 0x0111; // latin capital letter d with stroke -> latin small letter d with stroke
   ulmap[0x0112] = 0x0113; // latin capital letter e with macron -> latin small letter e with macron
   ulmap[0x0114] = 0x0115; // latin capital letter e with breve -> latin small letter e with breve
   ulmap[0x0116] = 0x0117; // latin capital letter e with dot above -> latin small letter e with dot above
   ulmap[0x0118] = 0x0119; // latin capital letter e with ogonek -> latin small letter e with ogonek
   ulmap[0x011a] = 0x011b; // latin capital letter e with caron -> latin small letter e with caron
   ulmap[0x011c] = 0x011d; // latin capital letter g with circumflex -> latin small letter g with circumflex
   ulmap[0x011e] = 0x011f; // latin capital letter g with breve -> latin small letter g with breve
   ulmap[0x0120] = 0x0121; // latin capital letter g with dot above -> latin small letter g with dot above
   ulmap[0x0122] = 0x0123; // latin capital letter g with cedilla -> latin small letter g with cedilla
   ulmap[0x0124] = 0x0125; // latin capital letter h with circumflex -> latin small letter h with circumflex
   ulmap[0x0126] = 0x0127; // latin capital letter h with stroke -> latin small letter h with stroke
   ulmap[0x0128] = 0x0129; // latin capital letter i with tilde -> latin small letter i with tilde
   ulmap[0x012a] = 0x012b; // latin capital letter i with macron -> latin small letter i with macron
   ulmap[0x012c] = 0x012d; // latin capital letter i with breve -> latin small letter i with breve
   ulmap[0x012e] = 0x012f; // latin capital letter i with ogonek -> latin small letter i with ogonek
   ulmap[0x0049] = 0x0131; // latin capital letter i -> latin small letter dotless i
   ulmap[0x0132] = 0x0133; // latin capital ligature ij -> latin small ligature ij
   ulmap[0x0134] = 0x0135; // latin capital letter j with circumflex -> latin small letter j with circumflex
   ulmap[0x0136] = 0x0137; // latin capital letter k with cedilla -> latin small letter k with cedilla
   ulmap[0x0139] = 0x013a; // latin capital letter l with acute -> latin small letter l with acute
   ulmap[0x013b] = 0x013c; // latin capital letter l with cedilla -> latin small letter l with cedilla
   ulmap[0x013d] = 0x013e; // latin capital letter l with caron -> latin small letter l with caron
   ulmap[0x013f] = 0x0140; // latin capital letter l with middle dot -> latin small letter l with middle dot
   ulmap[0x0141] = 0x0142; // latin capital letter l with stroke -> latin small letter l with stroke
   ulmap[0x0143] = 0x0144; // latin capital letter n with acute -> latin small letter n with acute
   ulmap[0x0145] = 0x0146; // latin capital letter n with cedilla -> latin small letter n with cedilla
   ulmap[0x0147] = 0x0148; // latin capital letter n with caron -> latin small letter n with caron
   ulmap[0x014a] = 0x014b; // latin capital letter eng (sami) -> latin small letter eng (sami)
   ulmap[0x014c] = 0x014d; // latin capital letter o with macron -> latin small letter o with macron
   ulmap[0x014e] = 0x014f; // latin capital letter o with breve -> latin small letter o with breve
   ulmap[0x0150] = 0x0151; // latin capital letter o with double acute -> latin small letter o with double acute
   ulmap[0x0152] = 0x0153; // latin capital ligature oe -> latin small ligature oe
   ulmap[0x0154] = 0x0155; // latin capital letter r with acute -> latin small letter r with acute
   ulmap[0x0156] = 0x0157; // latin capital letter r with cedilla -> latin small letter r with cedilla
   ulmap[0x0158] = 0x0159; // latin capital letter r with caron -> latin small letter r with caron
   ulmap[0x015a] = 0x015b; // latin capital letter s with acute -> latin small letter s with acute
   ulmap[0x015c] = 0x015d; // latin capital letter s with circumflex -> latin small letter s with circumflex
   ulmap[0x015e] = 0x015f; // latin capital letter s with cedilla -> latin small letter s with cedilla
   ulmap[0x0160] = 0x0161; // latin capital letter s with caron -> latin small letter s with caron
   ulmap[0x0162] = 0x0163; // latin capital letter t with cedilla -> latin small letter t with cedilla
   ulmap[0x0164] = 0x0165; // latin capital letter t with caron -> latin small letter t with caron
   ulmap[0x0166] = 0x0167; // latin capital letter t with stroke -> latin small letter t with stroke
   ulmap[0x0168] = 0x0169; // latin capital letter u with tilde -> latin small letter u with tilde
   ulmap[0x016a] = 0x016b; // latin capital letter u with macron -> latin small letter u with macron
   ulmap[0x016c] = 0x016d; // latin capital letter u with breve -> latin small letter u with breve
   ulmap[0x016e] = 0x016f; // latin capital letter u with ring above -> latin small letter u with ring above
   ulmap[0x0170] = 0x0171; // latin capital letter u with double acute -> latin small letter u with double acute
   ulmap[0x0172] = 0x0173; // latin capital letter u with ogonek -> latin small letter u with ogonek
   ulmap[0x0174] = 0x0175; // latin capital letter w with circumflex -> latin small letter w with circumflex
   ulmap[0x0176] = 0x0177; // latin capital letter y with circumflex -> latin small letter y with circumflex
   ulmap[0x0179] = 0x017a; // latin capital letter z with acute -> latin small letter z with acute
   ulmap[0x017b] = 0x017c; // latin capital letter z with dot above -> latin small letter z with dot above
   ulmap[0x017d] = 0x017e; // latin capital letter z with caron -> latin small letter z with caron
   ulmap[0x0182] = 0x0183; // latin capital letter b with topbar -> latin small letter b with topbar
   ulmap[0x0184] = 0x0185; // latin capital letter tone six -> latin small letter tone six
   ulmap[0x0187] = 0x0188; // latin capital letter c with hook -> latin small letter c with hook
   ulmap[0x018b] = 0x018c; // latin capital letter d with topbar -> latin small letter d with topbar
   ulmap[0x0191] = 0x0192; // latin capital letter f with hook -> latin small letter f with hook
   ulmap[0x0198] = 0x0199; // latin capital letter k with hook -> latin small letter k with hook
   ulmap[0x01a0] = 0x01a1; // latin capital letter o with horn -> latin small letter o with horn
   ulmap[0x01a2] = 0x01a3; // latin capital letter oi -> latin small letter oi
   ulmap[0x01a4] = 0x01a5; // latin capital letter p with hook -> latin small letter p with hook
   ulmap[0x01a7] = 0x01a8; // latin capital letter tone two -> latin small letter tone two
   ulmap[0x01ac] = 0x01ad; // latin capital letter t with hook -> latin small letter t with hook
   ulmap[0x01af] = 0x01b0; // latin capital letter u with horn -> latin small letter u with horn
   ulmap[0x01b3] = 0x01b4; // latin capital letter y with hook -> latin small letter y with hook
   ulmap[0x01b5] = 0x01b6; // latin capital letter z with stroke -> latin small letter z with stroke
   ulmap[0x01b8] = 0x01b9; // latin capital letter ezh reversed -> latin small letter ezh reversed
   ulmap[0x01bc] = 0x01bd; // latin capital letter tone five -> latin small letter tone five
   ulmap[0x01c4] = 0x01c6; // latin capital letter dz with caron -> latin small letter dz with caron
   ulmap[0x01c7] = 0x01c9; // latin capital letter lj -> latin small letter lj
   ulmap[0x01ca] = 0x01cc; // latin capital letter nj -> latin small letter nj
   ulmap[0x01cd] = 0x01ce; // latin capital letter a with caron -> latin small letter a with caron
   ulmap[0x01cf] = 0x01d0; // latin capital letter i with caron -> latin small letter i with caron
   ulmap[0x01d1] = 0x01d2; // latin capital letter o with caron -> latin small letter o with caron
   ulmap[0x01d3] = 0x01d4; // latin capital letter u with caron -> latin small letter u with caron
   ulmap[0x01d5] = 0x01d6; // latin capital letter u with diaeresis and macron -> latin small letter u with diaeresis and macron
   ulmap[0x01d7] = 0x01d8; // latin capital letter u with diaeresis and acute -> latin small letter u with diaeresis and acute
   ulmap[0x01d9] = 0x01da; // latin capital letter u with diaeresis and caron -> latin small letter u with diaeresis and caron
   ulmap[0x01db] = 0x01dc; // latin capital letter u with diaeresis and grave -> latin small letter u with diaeresis and grave
   ulmap[0x01de] = 0x01df; // latin capital letter a with diaeresis and macron -> latin small letter a with diaeresis and macron
   ulmap[0x01e0] = 0x01e1; // latin capital letter a with dot above and macron -> latin small letter a with dot above and macron
   ulmap[0x01e2] = 0x01e3; // latin capital ligature ae mth macron -> latin small ligature ae with macron
   ulmap[0x01e4] = 0x01e5; // latin capital letter g with stroke -> latin small letter g with stroke
   ulmap[0x01e6] = 0x01e7; // latin capital letter g with caron -> latin small letter g with caron
   ulmap[0x01e8] = 0x01e9; // latin capital letter k with caron -> latin small letter k with caron
   ulmap[0x01ea] = 0x01eb; // latin capital letter o with ogonek -> latin small letter o with ogonek
   ulmap[0x01ec] = 0x01ed; // latin capital letter o with ogonek and macron -> latin small letter o with ogonek and macron
   ulmap[0x01ee] = 0x01ef; // latin capital letter ezh with caron -> latin small letter ezh with caron
   ulmap[0x01f1] = 0x01f3; // latin capital letter dz -> latin small letter dz
   ulmap[0x01f4] = 0x01f5; // latin capital letter g with acute -> latin small letter g with acute
   ulmap[0x01fa] = 0x01fb; // latin capital letter a with ring above and acute -> latin small letter a with ring above and acute
   ulmap[0x01fc] = 0x01fd; // latin capital ligature ae with acute -> latin small ligature ae with acute
   ulmap[0x01fe] = 0x01ff; // latin capital letter o with stroke and acute -> latin small letter o with stroke and acute
   ulmap[0x0200] = 0x0201; // latin capital letter a with double grave -> latin small letter a with double grave
   ulmap[0x0202] = 0x0203; // latin capital letter a with inverted breve -> latin small letter a with inverted breve
   ulmap[0x0204] = 0x0205; // latin capital letter e with double grave -> latin small letter e with double grave
   ulmap[0x0206] = 0x0207; // latin capital letter e with inverted breve -> latin small letter e with inverted breve
   ulmap[0x0208] = 0x0209; // latin capital letter i with double grave -> latin small letter i with double grave
   ulmap[0x020a] = 0x020b; // latin capital letter i with inverted breve -> latin small letter i with inverted breve
   ulmap[0x020c] = 0x020d; // latin capital letter o with double grave -> latin small letter o with double grave
   ulmap[0x020e] = 0x020f; // latin capital letter o with inverted breve -> latin small letter o with inverted breve
   ulmap[0x0210] = 0x0211; // latin capital letter r with double grave -> latin small letter r with double grave
   ulmap[0x0212] = 0x0213; // latin capital letter r with inverted breve -> latin small letter r with inverted breve
   ulmap[0x0214] = 0x0215; // latin capital letter u with double grave -> latin small letter u with double grave
   ulmap[0x0216] = 0x0217; // latin capital letter u with inverted breve -> latin small letter u with inverted breve
   ulmap[0x0181] = 0x0253; // latin capital letter b with hook -> latin small letter b with hook
   ulmap[0x0186] = 0x0254; // latin capital letter open o -> latin small letter open o
   ulmap[0x018a] = 0x0257; // latin capital letter d with hook -> latin small letter d with hook
   ulmap[0x018e] = 0x0258; // latin capital letter reversed e -> latin small letter reversed e
   ulmap[0x018f] = 0x0259; // latin capital letter schwa -> latin small letter schwa
   ulmap[0x0190] = 0x025b; // latin capital letter open e -> latin small letter open e
   ulmap[0x0193] = 0x0260; // latin capital letter g with hook -> latin small letter g with hook
   ulmap[0x0194] = 0x0263; // latin capital letter gamma -> latin small letter gamma
   ulmap[0x0197] = 0x0268; // latin capital letter i with stroke -> latin small letter i with stroke
   ulmap[0x0196] = 0x0269; // latin capital letter iota -> latin small letter iota
   ulmap[0x019c] = 0x026f; // latin capital letter turned m -> latin small letter turned m
   ulmap[0x019d] = 0x0272; // latin capital letter n with left hook -> latin small letter n with left hook
   ulmap[0x019f] = 0x0275; // latin capital letter o with middle tilde -> latin small letter barred o
   ulmap[0x01a9] = 0x0283; // latin capital letter esh -> latin small letter esh
   ulmap[0x01ae] = 0x0288; // latin capital letter t with retroflex hook -> latin small letter t with retroflex hook
   ulmap[0x01b1] = 0x028a; // latin capital letter upsilon -> latin small letter upsilon
   ulmap[0x01b2] = 0x028b; // latin capital letter v with hook -> latin small letter v with hook
   ulmap[0x01b7] = 0x0292; // latin capital letter ezh -> latin small letter ezh
   ulmap[0x0386] = 0x03ac; // greek capital letter alpha with tonos -> greek small letter alpha with tonos
   ulmap[0x0388] = 0x03ad; // greek capital letter epsilon with tonos -> greek small letter epsilon with tonos
   ulmap[0x0389] = 0x03ae; // greek capital letter eta with tonos -> greek small letter eta with tonos
   ulmap[0x038a] = 0x03af; // greek capital letter iota with tonos -> greek small letter iota with tonos
   ulmap[0x0391] = 0x03b1; // greek capital letter alpha -> greek small letter alpha
   ulmap[0x0392] = 0x03b2; // greek capital letter beta -> greek small letter beta
   ulmap[0x0393] = 0x03b3; // greek capital letter gamma -> greek small letter gamma
   ulmap[0x0394] = 0x03b4; // greek capital letter delta -> greek small letter delta
   ulmap[0x0395] = 0x03b5; // greek capital letter epsilon -> greek small letter epsilon
   ulmap[0x0396] = 0x03b6; // greek capital letter zeta -> greek small letter zeta
   ulmap[0x0397] = 0x03b7; // greek capital letter eta -> greek small letter eta
   ulmap[0x0398] = 0x03b8; // greek capital letter theta -> greek small letter theta
   ulmap[0x0399] = 0x03b9; // greek capital letter iota -> greek small letter iota
   ulmap[0x039a] = 0x03ba; // greek capital letter kappa -> greek small letter kappa
   ulmap[0x039b] = 0x03bb; // greek capital letter lamda -> greek small letter lamda
   ulmap[0x039c] = 0x03bc; // greek capital letter mu -> greek small letter mu
   ulmap[0x039d] = 0x03bd; // greek capital letter nu -> greek small letter nu
   ulmap[0x039e] = 0x03be; // greek capital letter xi -> greek small letter xi
   ulmap[0x039f] = 0x03bf; // greek capital letter omicron -> greek small letter omicron
   ulmap[0x03a0] = 0x03c0; // greek capital letter pi -> greek small letter pi
   ulmap[0x03a1] = 0x03c1; // greek capital letter rho -> greek small letter rho
   ulmap[0x03a3] = 0x03c3; // greek capital letter sigma -> greek small letter sigma
   ulmap[0x03a4] = 0x03c4; // greek capital letter tau -> greek small letter tau
   ulmap[0x03a5] = 0x03c5; // greek capital letter upsilon -> greek small letter upsilon
   ulmap[0x03a6] = 0x03c6; // greek capital letter phi -> greek small letter phi
   ulmap[0x03a7] = 0x03c7; // greek capital letter chi -> greek small letter chi
   ulmap[0x03a8] = 0x03c8; // greek capital letter psi -> greek small letter psi
   ulmap[0x03a9] = 0x03c9; // greek capital letter omega -> greek small letter omega
   ulmap[0x03aa] = 0x03ca; // greek capital letter iota with dialytika -> greek small letter iota with dialytika
   ulmap[0x03ab] = 0x03cb; // greek capital letter upsilon with dialytika -> greek small letter upsilon with dialytika
   ulmap[0x038c] = 0x03cc; // greek capital letter omicron with tonos -> greek small letter omicron with tonos
   ulmap[0x038e] = 0x03cd; // greek capital letter upsilon with tonos -> greek small letter upsilon with tonos
   ulmap[0x038f] = 0x03ce; // greek capital letter omega with tonos -> greek small letter omega with tonos
   ulmap[0x03e2] = 0x03e3; // coptic capital letter shei -> coptic small letter shei
   ulmap[0x03e4] = 0x03e5; // coptic capital letter fei -> coptic small letter fei
   ulmap[0x03e6] = 0x03e7; // coptic capital letter khei -> coptic small letter khei
   ulmap[0x03e8] = 0x03e9; // coptic capital letter hori -> coptic small letter hori
   ulmap[0x03ea] = 0x03eb; // coptic capital letter gangia -> coptic small letter gangia
   ulmap[0x03ec] = 0x03ed; // coptic capital letter shima -> coptic small letter shima
   ulmap[0x03ee] = 0x03ef; // coptic capital letter dei -> coptic small letter dei
   ulmap[0x0410] = 0x0430; // cyrillic capital letter a -> cyrillic small letter a
   ulmap[0x0411] = 0x0431; // cyrillic capital letter be -> cyrillic small letter be
   ulmap[0x0412] = 0x0432; // cyrillic capital letter ve -> cyrillic small letter ve
   ulmap[0x0413] = 0x0433; // cyrillic capital letter ghe -> cyrillic small letter ghe
   ulmap[0x0414] = 0x0434; // cyrillic capital letter de -> cyrillic small letter de
   ulmap[0x0415] = 0x0435; // cyrillic capital letter ie -> cyrillic small letter ie
   ulmap[0x0416] = 0x0436; // cyrillic capital letter zhe -> cyrillic small letter zhe
   ulmap[0x0417] = 0x0437; // cyrillic capital letter ze -> cyrillic small letter ze
   ulmap[0x0418] = 0x0438; // cyrillic capital letter i -> cyrillic small letter i
   ulmap[0x0419] = 0x0439; // cyrillic capital letter short i -> cyrillic small letter short i
   ulmap[0x041a] = 0x043a; // cyrillic capital letter ka -> cyrillic small letter ka
   ulmap[0x041b] = 0x043b; // cyrillic capital letter el -> cyrillic small letter el
   ulmap[0x041c] = 0x043c; // cyrillic capital letter em -> cyrillic small letter em
   ulmap[0x041d] = 0x043d; // cyrillic capital letter en -> cyrillic small letter en
   ulmap[0x041e] = 0x043e; // cyrillic capital letter o -> cyrillic small letter o
   ulmap[0x041f] = 0x043f; // cyrillic capital letter pe -> cyrillic small letter pe
   ulmap[0x0420] = 0x0440; // cyrillic capital letter er -> cyrillic small letter er
   ulmap[0x0421] = 0x0441; // cyrillic capital letter es -> cyrillic small letter es
   ulmap[0x0422] = 0x0442; // cyrillic capital letter te -> cyrillic small letter te
   ulmap[0x0423] = 0x0443; // cyrillic capital letter u -> cyrillic small letter u
   ulmap[0x0424] = 0x0444; // cyrillic capital letter ef -> cyrillic small letter ef
   ulmap[0x0425] = 0x0445; // cyrillic capital letter ha -> cyrillic small letter ha
   ulmap[0x0426] = 0x0446; // cyrillic capital letter tse -> cyrillic small letter tse
   ulmap[0x0427] = 0x0447; // cyrillic capital letter che -> cyrillic small letter che
   ulmap[0x0428] = 0x0448; // cyrillic capital letter sha -> cyrillic small letter sha
   ulmap[0x0429] = 0x0449; // cyrillic capital letter shcha -> cyrillic small letter shcha
   ulmap[0x042a] = 0x044a; // cyrillic capital letter hard sign -> cyrillic small letter hard sign
   ulmap[0x042b] = 0x044b; // cyrillic capital letter yeru -> cyrillic small letter yeru
   ulmap[0x042c] = 0x044c; // cyrillic capital letter soft sign -> cyrillic small letter soft sign
   ulmap[0x042d] = 0x044d; // cyrillic capital letter e -> cyrillic small letter e
   ulmap[0x042e] = 0x044e; // cyrillic capital letter yu -> cyrillic small letter yu
   ulmap[0x042f] = 0x044f; // cyrillic capital letter ya -> cyrillic small letter ya
   ulmap[0x0401] = 0x0451; // cyrillic capital letter io -> cyrillic small letter io
   ulmap[0x0402] = 0x0452; // cyrillic capital letter dje (serbocroatian) -> cyrillic small letter dje (serbocroatian)
   ulmap[0x0403] = 0x0453; // cyrillic capital letter gje -> cyrillic small letter gje
   ulmap[0x0404] = 0x0454; // cyrillic capital letter ukrainian ie -> cyrillic small letter ukrainian ie
   ulmap[0x0405] = 0x0455; // cyrillic capital letter dze -> cyrillic small letter dze
   ulmap[0x0406] = 0x0456; // cyrillic capital letter byelorussian_ukrainian i -> cyrillic small letter byelorussian-ukrainian i
   ulmap[0x0407] = 0x0457; // cyrillic capital letter yi (ukrainian) -> cyrillic small letter yi (ukrainian)
   ulmap[0x0408] = 0x0458; // cyrillic capital letter je -> cyrillic small letter je
   ulmap[0x0409] = 0x0459; // cyrillic capital letter lje -> cyrillic small letter lje
   ulmap[0x040a] = 0x045a; // cyrillic capital letter nje -> cyrillic small letter nje
   ulmap[0x040b] = 0x045b; // cyrillic capital letter tshe (serbocroatian) -> cyrillic small letter tshe (serbocroatian)
   ulmap[0x040c] = 0x045c; // cyrillic capital letter kje -> cyrillic small letter kje
   ulmap[0x040e] = 0x045e; // cyrillic capital letter short u (byelorussian) -> cyrillic small letter short u (byelorussian)
   ulmap[0x040f] = 0x045f; // cyrillic capital letter dzhe -> cyrillic small letter dzhe
   ulmap[0x0460] = 0x0461; // cyrillic capital letter omega -> cyrillic small letter omega
   ulmap[0x0462] = 0x0463; // cyrillic capital letter yat -> cyrillic small letter yat
   ulmap[0x0464] = 0x0465; // cyrillic capital letter iotified e -> cyrillic small letter iotified e
   ulmap[0x0466] = 0x0467; // cyrillic capital letter little yus -> cyrillic small letter little yus
   ulmap[0x0468] = 0x0469; // cyrillic capital letter iotified little yus -> cyrillic small letter iotified little yus
   ulmap[0x046a] = 0x046b; // cyrillic capital letter big yus -> cyrillic small letter big yus
   ulmap[0x046c] = 0x046d; // cyrillic capital letter iotified big yus -> cyrillic small letter iotified big yus
   ulmap[0x046e] = 0x046f; // cyrillic capital letter ksi -> cyrillic small letter ksi
   ulmap[0x0470] = 0x0471; // cyrillic capital letter psi -> cyrillic small letter psi
   ulmap[0x0472] = 0x0473; // cyrillic capital letter fita -> cyrillic small letter fita
   ulmap[0x0474] = 0x0475; // cyrillic capital letter izhitsa -> cyrillic small letter izhitsa
   ulmap[0x0476] = 0x0477; // cyrillic capital letter izhitsa with double grave accent -> cyrillic small letter izhitsa with double grave accent
   ulmap[0x0478] = 0x0479; // cyrillic capital letter uk -> cyrillic small letter uk
   ulmap[0x047a] = 0x047b; // cyrillic capital letter round omega -> cyrillic small letter round omega
   ulmap[0x047c] = 0x047d; // cyrillic capital letter omega with titlo -> cyrillic small letter omega with titlo
   ulmap[0x047e] = 0x047f; // cyrillic capital letter ot -> cyrillic small letter ot
   ulmap[0x0480] = 0x0481; // cyrillic capital letter koppa -> cyrillic small letter koppa
   ulmap[0x0490] = 0x0491; // cyrillic capital letter ghe with upturn -> cyrillic small letter ghe with upturn
   ulmap[0x0492] = 0x0493; // cyrillic capital letter ghe with stroke -> cyrillic small letter ghe with stroke
   ulmap[0x0494] = 0x0495; // cyrillic capital letter ghe with middle hook -> cyrillic small letter ghe with middle hook
   ulmap[0x0496] = 0x0497; // cyrillic capital letter zhe with descender -> cyrillic small letter zhe with descender
   ulmap[0x0498] = 0x0499; // cyrillic capital letter ze with descender -> cyrillic small letter ze with descender
   ulmap[0x049a] = 0x049b; // cyrillic capital letter ka with descender -> cyrillic small letter ka with descender
   ulmap[0x049c] = 0x049d; // cyrillic capital letter ka with vertical stroke -> cyrillic small letter ka with vertical stroke
   ulmap[0x049e] = 0x049f; // cyrillic capital letter ka with stroke -> cyrillic small letter ka with stroke
   ulmap[0x04a0] = 0x04a1; // cyrillic capital letter bashkir ka -> cyrillic small letter eashkir ka
   ulmap[0x04a2] = 0x04a3; // cyrillic capital letter en with descender -> cyrillic small letter en with descenoer
   ulmap[0x04a4] = 0x04a5; // cyrillic capital ligature en ghf -> cyrillic small ligature en ghe
   ulmap[0x04a6] = 0x04a7; // cyrillic capital letter pe with middle hook (abkhasian) -> cyrillic small letter pe with middle hook (abkhasian)
   ulmap[0x04a8] = 0x04a9; // cyrillic capital letter abkhasian ha -> cyrillic small letter abkhasian ha
   ulmap[0x04aa] = 0x04ab; // cyrillic capital letter es with descender -> cyrillic small letter es with descender
   ulmap[0x04ac] = 0x04ad; // cyrillic capital letter te with descender -> cyrillic small letter te with descender
   ulmap[0x04ae] = 0x04af; // cyrillic capital letter straight u -> cyrillic small letter straight u
   ulmap[0x04b0] = 0x04b1; // cyrillic capital letter straight u with stroke -> cyrillic small letter straight u with stroke
   ulmap[0x04b2] = 0x04b3; // cyrillic capital letter ha with descender -> cyrillic small letter ha with descender
   ulmap[0x04b4] = 0x04b5; // cyrillic capital ligature te tse (abkhasian) -> cyrillic small ligature te tse (abkhasian)
   ulmap[0x04b6] = 0x04b7; // cyrillic capital letter che with descender -> cyrillic small letter che with descender
   ulmap[0x04b8] = 0x04b9; // cyrillic capital letter che with vertical stroke -> cyrillic small letter che with vertical stroke
   ulmap[0x04ba] = 0x04bb; // cyrillic capital letter shha -> cyrillic small letter shha
   ulmap[0x04bc] = 0x04bd; // cyrillic capital letter abkhasian che -> cyrillic small letter abkhasian che
   ulmap[0x04be] = 0x04bf; // cyrillic capital letter abkhasian che with descender -> cyrillic small letter abkhasian che with descender
   ulmap[0x04c1] = 0x04c2; // cyrillic capital letter zhe with breve -> cyrillic small letter zhe with breve
   ulmap[0x04c3] = 0x04c4; // cyrillic capital letter ka with hook -> cyrillic small letter ka with hook
   ulmap[0x04c7] = 0x04c8; // cyrillic capital letter en with hook -> cyrillic small letter en with hook
   ulmap[0x04cb] = 0x04cc; // cyrillic capital letter khakassian che -> cyrillic small letter khakassian che
   ulmap[0x04d0] = 0x04d1; // cyrillic capital letter a with breve -> cyrillic small letter a with breve
   ulmap[0x04d2] = 0x04d3; // cyrillic capital letter a with diaeresis -> cyrillic small letter a with diaeresis
   ulmap[0x04d4] = 0x04d5; // cyrillic capital ligature a ie -> cyrillic small ligature a ie
   ulmap[0x04d6] = 0x04d7; // cyrillic capital letter ie with breve -> cyrillic small letter ie with breve
   ulmap[0x04d8] = 0x04d9; // cyrillic capital letter schwa -> cyrillic small letter schwa
   ulmap[0x04da] = 0x04db; // cyrillic capital letter schwa with diaeresis -> cyrillic small letter schwa with diaeresis
   ulmap[0x04dc] = 0x04dd; // cyrillic capital letter zhe with diaeresis -> cyrillic small letter zhe with diaeresis
   ulmap[0x04de] = 0x04df; // cyrillic capital letter ze with diaeresis -> cyrillic small letter ze with diaeresis
   ulmap[0x04e0] = 0x04e1; // cyrillic capital letter abkhasian dze -> cyrillic small letter abkhasian dze
   ulmap[0x04e2] = 0x04e3; // cyrillic capital letter i with macron -> cyrillic small letter i with macron
   ulmap[0x04e4] = 0x04e5; // cyrillic capital letter i with diaeresis -> cyrillic small letter i with diaeresis
   ulmap[0x04e6] = 0x04e7; // cyrillic capital letter o with diaeresis -> cyrillic small letter o with diaeresis
   ulmap[0x04e8] = 0x04e9; // cyrillic capital letter barred o -> cyrillic small letter barred o
   ulmap[0x04ea] = 0x04eb; // cyrillic capital letter barred o with diaeresis -> cyrillic small letter barred o with diaeresis
   ulmap[0x04ee] = 0x04ef; // cyrillic capital letter u with macron -> cyrillic small letter u with macron
   ulmap[0x04f0] = 0x04f1; // cyrillic capital letter u with diaeresis -> cyrillic small letter u with diaeresis
   ulmap[0x04f2] = 0x04f3; // cyrillic capital letter u with double acute -> cyrillic small letter u with double acute
   ulmap[0x04f4] = 0x04f5; // cyrillic capital letter che with diaeresis -> cyrillic small letter che aith diaeresis
   ulmap[0x04f8] = 0x04f9; // cyrillic capital letter yeru with diaeresis -> cyrillic small letter yeru with diaeresis
   ulmap[0x0531] = 0x0561; // armenian capital letter ayb -> armenian small letter ayb
   ulmap[0x0532] = 0x0562; // armenian capital letter ben -> armenian small letter ben
   ulmap[0x0533] = 0x0563; // armenian capital letter gim -> armenian small letter gim
   ulmap[0x0534] = 0x0564; // armenian capital letter da -> armenian small letter da
   ulmap[0x0535] = 0x0565; // armenian capital letter ech -> armenian small letter ech
   ulmap[0x0536] = 0x0566; // armenian capital letter za -> armenian small letter za
   ulmap[0x0537] = 0x0567; // armenian capital letter eh -> armenian small letter eh
   ulmap[0x0538] = 0x0568; // armenian capital letter et -> armenian small letter et
   ulmap[0x0539] = 0x0569; // armenian capital letter to -> armenian small letter to
   ulmap[0x053a] = 0x056a; // armenian capital letter zhe -> armenian small letter zhe
   ulmap[0x053b] = 0x056b; // armenian capital letter ini -> armenian small letter ini
   ulmap[0x053c] = 0x056c; // armenian capital letter liwn -> armenian small letter liwn
   ulmap[0x053d] = 0x056d; // armenian capital letter xeh -> armenian small letter xeh
   ulmap[0x053e] = 0x056e; // armenian capital letter ca -> armenian small letter ca
   ulmap[0x053f] = 0x056f; // armenian capital letter ken -> armenian small letter ken
   ulmap[0x0540] = 0x0570; // armenian capital letter ho -> armenian small letter ho
   ulmap[0x0541] = 0x0571; // armenian capital letter ja -> armenian small letter ja
   ulmap[0x0542] = 0x0572; // armenian capital letter ghad -> armenian small letter ghad
   ulmap[0x0543] = 0x0573; // armenian capital letter cheh -> armenian small letter cheh
   ulmap[0x0544] = 0x0574; // armenian capital letter men -> armenian small letter men
   ulmap[0x0545] = 0x0575; // armenian capital letter yi -> armenian small letter yi
   ulmap[0x0546] = 0x0576; // armenian capital letter now -> armenian small letter now
   ulmap[0x0547] = 0x0577; // armenian capital letter sha -> armenian small letter sna
   ulmap[0x0548] = 0x0578; // armenian capital letter vo -> armenian small letter vo
   ulmap[0x0549] = 0x0579; // armenian capital letter cha -> armenian small letter cha
   ulmap[0x054a] = 0x057a; // armenian capital letter peh -> armenian small letter peh
   ulmap[0x054b] = 0x057b; // armenian capital letter jheh -> armenian small letter jheh
   ulmap[0x054c] = 0x057c; // armenian capital letter ra -> armenian small letter ra
   ulmap[0x054d] = 0x057d; // armenian capital letter seh -> armenian small letter seh
   ulmap[0x054e] = 0x057e; // armenian capital letter vew -> armenian small letter vew
   ulmap[0x054f] = 0x057f; // armenian capital letter tiwn -> armenian small letter tiwn
   ulmap[0x0550] = 0x0580; // armenian capital letter reh -> armenian small letter reh
   ulmap[0x0551] = 0x0581; // armenian capital letter co -> armenian small letter co
   ulmap[0x0552] = 0x0582; // armenian capital letter yiwn -> armenian small letter yiwn
   ulmap[0x0553] = 0x0583; // armenian capital letter piwr -> armenian small letter piwp
   ulmap[0x0554] = 0x0584; // armenian capital letter keh -> armenian small letter keh
   ulmap[0x0555] = 0x0585; // armenian capital letter oh -> armenian small letter oh
   ulmap[0x0556] = 0x0586; // armenian capital letter feh -> armenian small letter feh
   ulmap[0x10a0] = 0x10d0; // georgian capital letter an (khutsuri) -> georgian letter an
   ulmap[0x10a1] = 0x10d1; // georgian capital letter ban (khutsuri) -> georgian letter ban
   ulmap[0x10a2] = 0x10d2; // georgian capital letter gan (khutsuri) -> georgian letter gan
   ulmap[0x10a3] = 0x10d3; // georgian capital letter don (khutsuri) -> georgian letter don
   ulmap[0x10a4] = 0x10d4; // georgian capital letter en (khutsuri) -> georgian letter en
   ulmap[0x10a5] = 0x10d5; // georgian capital letter vin (khutsuri) -> georgian letter vin
   ulmap[0x10a6] = 0x10d6; // georgian capital letter zen (khutsuri) -> georgian letter zen
   ulmap[0x10a7] = 0x10d7; // georgian capital letter tan (khutsuri) -> georgian letter tan
   ulmap[0x10a8] = 0x10d8; // georgian capital letter in (khutsuri) -> georgian letter in
   ulmap[0x10a9] = 0x10d9; // georgian capital letter kan (khutsuri) -> georgian letter kan
   ulmap[0x10aa] = 0x10da; // georgian capital letter las (khutsuri) -> georgian letter las
   ulmap[0x10ab] = 0x10db; // georgian capital letter man (khutsuri) -> georgian letter man
   ulmap[0x10ac] = 0x10dc; // georgian capital letter nar (khutsuri) -> georgian letter nar
   ulmap[0x10ad] = 0x10dd; // georgian capital letter on (khutsuri) -> georgian letter on
   ulmap[0x10ae] = 0x10de; // georgian capital letter par (khutsuri) -> georgian letter par
   ulmap[0x10af] = 0x10df; // georgian capital letter zhar (khutsuri) -> georgian letter zhar
   ulmap[0x10b0] = 0x10e0; // georgian capital letter rae (khutsuri) -> georgian letter rae
   ulmap[0x10b1] = 0x10e1; // georgian capital letter san (khutsuri) -> georgian letter san
   ulmap[0x10b2] = 0x10e2; // georgian capital letter tar (khutsuri) -> georgian letter tar
   ulmap[0x10b3] = 0x10e3; // georgian capital letter un (khutsuri) -> georgian letter un
   ulmap[0x10b4] = 0x10e4; // georgian capital letter phar (khutsuri) -> georgian letter phar
   ulmap[0x10b5] = 0x10e5; // georgian capital letter khar (khutsuri) -> georgian letter khar
   ulmap[0x10b6] = 0x10e6; // georgian capital letter ghan (khutsuri) -> georgian letter ghan
   ulmap[0x10b7] = 0x10e7; // georgian capital letter qar (khutsuri) -> georgian letter qar
   ulmap[0x10b8] = 0x10e8; // georgian capital letter shin (khutsuri) -> georgian letter shin
   ulmap[0x10b9] = 0x10e9; // georgian capital letter chin (khutsuri) -> georgian letter chin
   ulmap[0x10ba] = 0x10ea; // georgian capital letter can (khutsuri) -> georgian letter can
   ulmap[0x10bb] = 0x10eb; // georgian capital letter jil (khutsuri) -> georgian letter jil
   ulmap[0x10bc] = 0x10ec; // georgian capital letter cil (khutsuri) -> georgian letter cil
   ulmap[0x10bd] = 0x10ed; // georgian capital letter char (khutsuri) -> georgian letter char
   ulmap[0x10be] = 0x10ee; // georgian capital letter xan (khutsuri) -> georgian letter xan
   ulmap[0x10bf] = 0x10ef; // georgian capital letter jhan (khutsuri) -> georgian letter jhan
   ulmap[0x10c0] = 0x10f0; // georgian capital letter hae (khutsuri) -> georgian letter hae
   ulmap[0x10c1] = 0x10f1; // georgian capital letter he (khutsuri) -> georgian letter he
   ulmap[0x10c2] = 0x10f2; // georgian capital letter hie (khutsuri) -> georgian letter hie
   ulmap[0x10c3] = 0x10f3; // georgian capital letter we (khutsuri) -> georgian letter we
   ulmap[0x10c4] = 0x10f4; // georgian capital letter har (khutsuri) -> georgian letter har
   ulmap[0x10c5] = 0x10f5; // georgian capital letter hoe (khutsuri) -> georgian letter hoe
   ulmap[0x1e00] = 0x1e01; // latin capital letter a with ring below -> latin small letter a with ring below
   ulmap[0x1e02] = 0x1e03; // latin capital letter b with dot above -> latin small letter b with dot above
   ulmap[0x1e04] = 0x1e05; // latin capital letter b with dot below -> latin small letter b with dot below
   ulmap[0x1e06] = 0x1e07; // latin capital letter b with line below -> latin small letter b with line below
   ulmap[0x1e08] = 0x1e09; // latin capital letter c with cedilla and acute -> latin small letter c with cedilla and acute
   ulmap[0x1e0a] = 0x1e0b; // latin capital letter d with dot above -> latin small letter d with dot above
   ulmap[0x1e0c] = 0x1e0d; // latin capital letter d with dot below -> latin small letter d with dot below
   ulmap[0x1e0e] = 0x1e0f; // latin capital letter d with line below -> latin small letter d with line below
   ulmap[0x1e10] = 0x1e11; // latin capital letter d with cedilla -> latin small letter d with cedilla
   ulmap[0x1e12] = 0x1e13; // latin capital letter d with circumflex below -> latin small letter d with circumflex below
   ulmap[0x1e14] = 0x1e15; // latin capital letter e with macron and grave -> latin small letter e with macron and grave
   ulmap[0x1e16] = 0x1e17; // latin capital letter e with macron and acute -> latin small letter e with macron and acute
   ulmap[0x1e18] = 0x1e19; // latin capital letter e with circumflex below -> latin small letter e with circumflex below
   ulmap[0x1e1a] = 0x1e1b; // latin capital letter e with tilde below -> latin small letter e with tilde below
   ulmap[0x1e1c] = 0x1e1d; // latin capital letter e with cedilla and breve -> latin small letter e with cedilla and breve
   ulmap[0x1e1e] = 0x1e1f; // latin capital letter f with dot above -> latin small letter f with dot above
   ulmap[0x1e20] = 0x1e21; // latin capital letter g with macron -> latin small letter g with macron
   ulmap[0x1e22] = 0x1e23; // latin capital letter h with dot above -> latin small letter h with dot above
   ulmap[0x1e24] = 0x1e25; // latin capital letter h with dot below -> latin small letter h with dot below
   ulmap[0x1e26] = 0x1e27; // latin capital letter h with diaeresis -> latin small letter h with diaeresis
   ulmap[0x1e28] = 0x1e29; // latin capital letter h with cedilla -> latin small letter h with cedilla
   ulmap[0x1e2a] = 0x1e2b; // latin capital letter h with breve below -> latin small letter h with breve below
   ulmap[0x1e2c] = 0x1e2d; // latin capital letter i with tilde below -> latin small letter i with tilde below
   ulmap[0x1e2e] = 0x1e2f; // latin capital letter i with diaeresis and acute -> latin small letter i with diaeresis and acute
   ulmap[0x1e30] = 0x1e31; // latin capital letter k with acute -> latin small letter k with acute
   ulmap[0x1e32] = 0x1e33; // latin capital letter k with dot below -> latin small letter k with dot below
   ulmap[0x1e34] = 0x1e35; // latin capital letter k with line below -> latin small letter k with line below
   ulmap[0x1e36] = 0x1e37; // latin capital letter l with dot below -> latin small letter l with dot below
   ulmap[0x1e38] = 0x1e39; // latin capital letter l with dot below and macron -> latin small letter l with dot below and macron
   ulmap[0x1e3a] = 0x1e3b; // latin capital letter l with line below -> latin small letter l with line below
   ulmap[0x1e3c] = 0x1e3d; // latin capital letter l with circumflex below -> latin small letter l with circumflex below
   ulmap[0x1e3e] = 0x1e3f; // latin capital letter m with acute -> latin small letter m with acute
   ulmap[0x1e40] = 0x1e41; // latin capital letter m with dot above -> latin small letter m with dot above
   ulmap[0x1e42] = 0x1e43; // latin capital letter m with dot below -> latin small letter m with dot below
   ulmap[0x1e44] = 0x1e45; // latin capital letter n with dot above -> latin small letter n with dot above
   ulmap[0x1e46] = 0x1e47; // latin capital letter n with dot below -> latin small letter n with dot below
   ulmap[0x1e48] = 0x1e49; // latin capital letter n with line below -> latin small letter n with line below
   ulmap[0x1e4a] = 0x1e4b; // latin capital letter n with circumflex below -> latin small letter n with circumflex below
   ulmap[0x1e4c] = 0x1e4d; // latin capital letter o with tilde and acute -> latin small letter o with tilde and acute
   ulmap[0x1e4e] = 0x1e4f; // latin capital letter o with tilde and diaeresis -> latin small letter o with tllde and diaeresis
   ulmap[0x1e50] = 0x1e51; // latin capital letter o with macron and grave -> latin small letter o with macron and grave
   ulmap[0x1e52] = 0x1e53; // latin capital letter o with macron and acute -> latin small letter o with macron and acute
   ulmap[0x1e54] = 0x1e55; // latin capital letter p with acute -> latin small letter p with acute
   ulmap[0x1e56] = 0x1e57; // latin capital letter p with dot above -> latin small letter p with dot above
   ulmap[0x1e58] = 0x1e59; // latin capital letter r with dot above -> latin small letter r with dot above
   ulmap[0x1e5a] = 0x1e5b; // latin capital letter r with dot below -> latin small letter r with dot below
   ulmap[0x1e5c] = 0x1e5d; // latin capital letter r with dot below and macron -> latin small letter r with dot below and macron
   ulmap[0x1e5e] = 0x1e5f; // latin capital letter r with line below -> latin small letter r with line below
   ulmap[0x1e60] = 0x1e61; // latin capital letter s with dot above -> latin small letter s with dot above
   ulmap[0x1e62] = 0x1e63; // latin capital letter s with dot below -> latin small letter s with dot below
   ulmap[0x1e64] = 0x1e65; // latin capital letter s with acute and dot above -> latin small letter s with acute and dot above
   ulmap[0x1e66] = 0x1e67; // latin capital letter s with caron and dot above -> latin small letter s with caron and dot above
   ulmap[0x1e68] = 0x1e69; // latin capital letter s with dot below and dot above -> latin small letter s with dot below and dot above
   ulmap[0x1e6a] = 0x1e6b; // latin capital letter t with dot above -> latin small letter t with dot above
   ulmap[0x1e6c] = 0x1e6d; // latin capital letter t with dot below -> latin small letter t with dot below
   ulmap[0x1e6e] = 0x1e6f; // latin capital letter t with line below -> latin small letter t with line below
   ulmap[0x1e70] = 0x1e71; // latin capital letter t with circumflex below -> latin small letter t with circumflex below
   ulmap[0x1e72] = 0x1e73; // latin capital letter u with diaeresis below -> latin small letter u with diaeresis below
   ulmap[0x1e74] = 0x1e75; // latin capital letter u with tilde below -> latin small letter u with tilde below
   ulmap[0x1e76] = 0x1e77; // latin capital letter u with circumflex below -> latin small letter u with circumflex below
   ulmap[0x1e78] = 0x1e79; // latin capital letter u with tilde and acute -> latin small letter u with tilde and acute
   ulmap[0x1e7a] = 0x1e7b; // latin capital letter u with macron and diaeresis -> latin small letter u with macron and diaeresis
   ulmap[0x1e7c] = 0x1e7d; // latin capital letter v with tilde -> latin small letter v with tilde
   ulmap[0x1e7e] = 0x1e7f; // latin capital letter v with dot below -> latin small letter v with dot below
   ulmap[0x1e80] = 0x1e81; // latin capital letter w with grave -> latin small letter w with grave
   ulmap[0x1e82] = 0x1e83; // latin capital letter w with acute -> latin small letter w with acute
   ulmap[0x1e84] = 0x1e85; // latin capital letter w with diaeresis -> latin small letter w with diaeresis
   ulmap[0x1e86] = 0x1e87; // latin capital letter w with dot above -> latin small letter w with dot above
   ulmap[0x1e88] = 0x1e89; // latin capital letter w with dot below -> latin small letter w with dot below
   ulmap[0x1e8a] = 0x1e8b; // latin capital letter x with dot above -> latin small letter x with dot above
   ulmap[0x1e8c] = 0x1e8d; // latin capital letter x5 with diaeresis -> latin small letter x with diaeresis
   ulmap[0x1e8e] = 0x1e8f; // latin capital letter y with dot above -> latin small letter y with dot above
   ulmap[0x1e90] = 0x1e91; // latin capital letter z with circumflex -> latin small letter z with circumflex
   ulmap[0x1e92] = 0x1e93; // latin capital letter z with dot below -> latin small letter z with dot below
   ulmap[0x1e94] = 0x1e95; // latin capital letter z with line below -> latin small letter z with line below
   ulmap[0x1ea0] = 0x1ea1; // latin capital letter a with dot below -> latin small letter a with dot below
   ulmap[0x1ea2] = 0x1ea3; // latin capital letter a with hook above -> latin small letter a with hook above
   ulmap[0x1ea4] = 0x1ea5; // latin capital letter a with circumflex and acute -> latin small letter a with circumflex and acute
   ulmap[0x1ea6] = 0x1ea7; // latin capital letter a with circumflex and grave -> latin small letter a with circumflex and grave
   ulmap[0x1ea8] = 0x1ea9; // latin capital letter a with circumflex and hook above -> latin small letter a with circumflex and hook above
   ulmap[0x1eaa] = 0x1eab; // latin capital letter a with circumflex and tilde -> latin small letter a with circumflex and tilde
   ulmap[0x1eac] = 0x1ead; // latin capital letter a with circumflex and dot below -> latin small letter a with circumflex and dot below
   ulmap[0x1eae] = 0x1eaf; // latin capital letter a with breve and acute -> latin small letter a with breve and acute
   ulmap[0x1eb0] = 0x1eb1; // latin capital letter a with breve and grave -> latin small letter a with breve and grave
   ulmap[0x1eb2] = 0x1eb3; // latin capital letter a with breve and hook above -> latin small letter a with breve and hook above
   ulmap[0x1eb4] = 0x1eb5; // latin capital letter a with breve and tilde -> latin small letter a with breve and tilde
   ulmap[0x1eb6] = 0x1eb7; // latin capital letter a with breve and dot below -> latin small letter a with breve and dot below
   ulmap[0x1eb8] = 0x1eb9; // latin capital letter e with dot below -> latin small letter e with dot below
   ulmap[0x1eba] = 0x1ebb; // latin capital letter e with hook above -> latin small letter e with hook above
   ulmap[0x1ebc] = 0x1ebd; // latin capital letter e with tilde -> latin small letter e with tilde
   ulmap[0x1ebe] = 0x1ebf; // latin capital letter e with circumflex and acute -> latin small letter e with circumflex and acute
   ulmap[0x1ec0] = 0x1ec1; // latin capital letter e with circumflex and grave -> latin small letter e with circumflex and grave
   ulmap[0x1ec2] = 0x1ec3; // latin capital letter e with circumflex and hook above -> latin small letter e with circumflex and hook above
   ulmap[0x1ec4] = 0x1ec5; // latin capital letter e with circumflex and tilde -> latin small letter e with circumflex and tilde
   ulmap[0x1ec6] = 0x1ec7; // latin capital letter e with circumflex and dot below -> latin small letter e with circumflex and dot below
   ulmap[0x1ec8] = 0x1ec9; // latin capital letter i with hook above -> latin small letter i with hook above
   ulmap[0x1eca] = 0x1ecb; // latin capital letter i with dot below -> latin small letter i with dot below
   ulmap[0x1ecc] = 0x1ecd; // latin capital letter o with dot below -> latin small letter o with dot below
   ulmap[0x1ece] = 0x1ecf; // latin capital letter o with hook above -> latin small letter o with hook above
   ulmap[0x1ed0] = 0x1ed1; // latin capital letter o with circumflex and acute -> latin small letter o with circumflex and acute
   ulmap[0x1ed2] = 0x1ed3; // latin capital letter o with circumflex and grave -> latin small letter o with circumflex and grave
   ulmap[0x1ed4] = 0x1ed5; // latin capital letter o with circumflex and hook above -> latin small letter o with circumflex and hook above
   ulmap[0x1ed6] = 0x1ed7; // latin capital letter o with circumflex and tilde -> latin small letter o with circumflex and tilde
   ulmap[0x1ed8] = 0x1ed9; // latin capital letter o with circumflex and dot below -> latin small letter o with circumflex and dot below
   ulmap[0x1eda] = 0x1edb; // latin capital letter o with horn and acute -> latin small letter o with horn and acute
   ulmap[0x1edc] = 0x1edd; // latin capital letter o with horn and grave -> latin small letter o with horn and grave
   ulmap[0x1ede] = 0x1edf; // latin capital letter o with horn and hook above -> latin small letter o with horn and hook above
   ulmap[0x1ee0] = 0x1ee1; // latin capital letter o with horn and tilde -> latin small letter o with horn and tilde
   ulmap[0x1ee2] = 0x1ee3; // latin capital letter o with horn and dot below -> latin small letter o with horn and dot below
   ulmap[0x1ee4] = 0x1ee5; // latin capital letter u with dot below -> latin small letter u with dot below
   ulmap[0x1ee6] = 0x1ee7; // latin capital letter u with hook above -> latin small letter u with hook above
   ulmap[0x1ee8] = 0x1ee9; // latin capital letter u with horn and acute -> latin small letter u with horn and acute
   ulmap[0x1eea] = 0x1eeb; // latin capital letter u with horn and grave -> latin small letter u with horn and grave
   ulmap[0x1eec] = 0x1eed; // latin capital letter u with horn and hook above -> latin small letter u with horn and hock above
   ulmap[0x1eee] = 0x1eef; // latin capital letter u with horn and tilde -> latin small letter u with horn and tilde
   ulmap[0x1ef0] = 0x1ef1; // latin capital letter u with horn and dot below -> latin small letter u with horn and dot below
   ulmap[0x1ef2] = 0x1ef3; // latin capital letter y with grave -> latin small letter y with grave
   ulmap[0x1ef4] = 0x1ef5; // latin capital letter y with dot below -> latin small letter y with dot below
   ulmap[0x1ef6] = 0x1ef7; // latin capital letter y with hook above -> latin small letter y with hook above
   ulmap[0x1ef8] = 0x1ef9; // latin capital letter y with tilde -> latin small letter y with tilde
   ulmap[0x1f08] = 0x1f00; // greek capital letter alpha with psili -> greek small letter alpha with psili
   ulmap[0x1f09] = 0x1f01; // greek capital letter alpha with dasia -> greek small letter alpha with dasia
   ulmap[0x1f0a] = 0x1f02; // greek capital letter alpha with psili and varia -> greek small letter alpha with psili and varia
   ulmap[0x1f0b] = 0x1f03; // greek capital letter alpha with dasia and varia -> greek small letter alpha with dasia and varia
   ulmap[0x1f0c] = 0x1f04; // greek capital letter alpha with psili and oxia -> greek small letter alpha with psili and oxia
   ulmap[0x1f0d] = 0x1f05; // greek capital letter alpha with dasia and oxia -> greek small letter alpha with dasia and oxia
   ulmap[0x1f0e] = 0x1f06; // greek capital letter alpha with psili and perispomeni -> greek small letter alpha with psili and perispomeni
   ulmap[0x1f0f] = 0x1f07; // greek capital letter alpha with dasia and perispomeni -> greek small letter alpha with dasia and perispomeni
   ulmap[0x1f18] = 0x1f10; // greek capital letter epsilon with psili -> greek small letter epsilon with psili
   ulmap[0x1f19] = 0x1f11; // greek capital letter epsilon with dasia -> greek small letter epsilon with dasia
   ulmap[0x1f1a] = 0x1f12; // greek capital letter epsilon with psili and varia -> greek small letter epsilon with psili and varia
   ulmap[0x1f1b] = 0x1f13; // greek capital letter epsilon with dasia and varia -> greek small letter epsilon with dasia and varia
   ulmap[0x1f1c] = 0x1f14; // greek capital letter epsilon with psili and oxia -> greek small letter epsilon with psili and oxia
   ulmap[0x1f1d] = 0x1f15; // greek capital letter epsilon with dasia and oxia -> greek small letter epsilon with dasia and oxia
   ulmap[0x1f28] = 0x1f20; // greek capital letter eta with psili -> greek small letter eta with psili
   ulmap[0x1f29] = 0x1f21; // greek capital letter eta with dasia -> greek small letter eta with dasia
   ulmap[0x1f2a] = 0x1f22; // greek capital letter eta with psili and varia -> greek small letter eta with psili and varia
   ulmap[0x1f2b] = 0x1f23; // greek capital letter eta with dasia and varia -> greek small letter eta with dasia and varia
   ulmap[0x1f2c] = 0x1f24; // greek capital letter eta with psili and oxia -> greek small letter eta with psili and oxia
   ulmap[0x1f2d] = 0x1f25; // greek capital letter eta with dasia and oxia -> greek small letter eta with dasia and oxia
   ulmap[0x1f2e] = 0x1f26; // greek capital letter eta with psili and perispomeni -> greek small letter eta with psili and perispomeni
   ulmap[0x1f2f] = 0x1f27; // greek capital letter eta with dasia and perispomeni -> greek small letter eta with dasia and perispomeni
   ulmap[0x1f38] = 0x1f30; // greek capital letter iota with psili -> greek small letter iota with psili
   ulmap[0x1f39] = 0x1f31; // greek capital letter iota with dasia -> greek small letter iota with dasia
   ulmap[0x1f3a] = 0x1f32; // greek capital letter iota with psili and varia -> greek small letter iota with psili and varia
   ulmap[0x1f3b] = 0x1f33; // greek capital letter iota with dasia and varia -> greek small letter iota with dasia and varia
   ulmap[0x1f3c] = 0x1f34; // greek capital letter iota with psili and oxia -> greek small letter iota with psili and oxia
   ulmap[0x1f3d] = 0x1f35; // greek capital letter iota with dasia and oxia -> greek small letter iota with dasia and oxia
   ulmap[0x1f3e] = 0x1f36; // greek capital letter iota with psili and perispomeni -> greek small letter iota with psili and perispomeni
   ulmap[0x1f3f] = 0x1f37; // greek capital letter iota with dasia and perispomeni -> greek small letter iota with dasia and perispomeni
   ulmap[0x1f48] = 0x1f40; // greek capital letter omicron with psili -> greek small letter omicron with psili
   ulmap[0x1f49] = 0x1f41; // greek capital letter omicron with dasia -> greek small letter omicron with dasia
   ulmap[0x1f4a] = 0x1f42; // greek capital letter omicron with psili and varia -> greek small letter omicron with psili and varia
   ulmap[0x1f4b] = 0x1f43; // greek capital letter omicron with dasia and varia -> greek small letter omicron with dasia and varia
   ulmap[0x1f4c] = 0x1f44; // greek capital letter omicron with psili and oxia -> greek small letter omicron with psili and oxia
   ulmap[0x1f4d] = 0x1f45; // greek capital letter omicron with dasia and oxia -> greek small letter omicron with dasia and oxia
   ulmap[0x1f59] = 0x1f51; // greek capital letter upsilon with oasis -> greek small letter upsilon with dasia
   ulmap[0x1f5b] = 0x1f53; // greek capital letter upsilon with dasia and varia -> greek small letter upsilon with dasia and varia
   ulmap[0x1f5d] = 0x1f55; // greek capital letter upsilon with dasia and oxia -> greek small letter upsilon with dasia and oxia
   ulmap[0x1f5f] = 0x1f57; // greek capital letter upsilon with dasia and perispomeni -> greek small letter upsilon with dasia and perispomeni
   ulmap[0x1f68] = 0x1f60; // greek capital letter omega with psili -> greek small letter omega with psili
   ulmap[0x1f69] = 0x1f61; // greek capital letter omega with dasia -> greek small letter omega with dasia
   ulmap[0x1f6a] = 0x1f62; // greek capital letter omega with psili and varia -> greek small letter omega with psili and varia
   ulmap[0x1f6b] = 0x1f63; // greek capital letter omega with dasia and varia -> greek small letter omega with dasia and varia
   ulmap[0x1f6c] = 0x1f64; // greek capital letter omega with psili and oxia -> greek small letter omega with psili and oxia
   ulmap[0x1f6d] = 0x1f65; // greek capital letter omega with dasia and oxia -> greek small letter omega with dasia and oxia
   ulmap[0x1f6e] = 0x1f66; // greek capital letter omega with psili and perispomeni -> greek small letter omega with psili and perispomeni
   ulmap[0x1f6f] = 0x1f67; // greek capital letter omega with dasia and perispomeni -> greek small letter omega with dasia and perispomeni
   ulmap[0x1f88] = 0x1f80; // greek capital letter alpha with psili and prosgegrammeni -> greek small letter alpha with psili and ypogegrammeni
   ulmap[0x1f89] = 0x1f81; // greek capital letter alpha with dasia and prosgegrammeni -> greek small letter alpha with dasia and ypogegrammeni
   ulmap[0x1f8a] = 0x1f82; // greek capital letter alpha with psili and varia and prosgegrammeni -> greek small letter alpha with psili and varia and ypogegrammeni
   ulmap[0x1f8b] = 0x1f83; // greek capital letter alpha with dasia and varia and prosgegrammeni -> greek small letter alpha with dasia and varia and ypogegrammeni
   ulmap[0x1f8c] = 0x1f84; // greek capital letter alpha with psili and oxia and prosgegrammen -> greek small letter alpha with psili and oxia and ypogegrammeni
   ulmap[0x1f8d] = 0x1f85; // greek capital letter alpha with dasia and oxia and prosgegrammen -> greek small letter alpha with dasia and oxia and ypogegrammeni
   ulmap[0x1f8e] = 0x1f86; // greek capital letter alpha with psili and perispomeni and prosgegrammeni -> greek small letter alpha with psili and perispomeni and ypogegrammeni
   ulmap[0x1f8f] = 0x1f87; // greek capital letter alpha with dasia and perispomeni and prosgegrammeni -> greek small letter alpha with dasia and perispomeni and ypogegrammeni
   ulmap[0x1f98] = 0x1f90; // greek capital letter eta with psili and prosgegrammeni -> greek small letter eta with psili and ypogegrammeni
   ulmap[0x1f99] = 0x1f91; // greek capital letter eta with dasia and prosgegrammeni -> greek small letter eta with dasia and ypogegrammeni
   ulmap[0x1f9a] = 0x1f92; // greek capital letter eta with psili and varia and prosgegrammeni -> greek small letter eta with psili and varia and ypogegrammeni
   ulmap[0x1f9b] = 0x1f93; // greek capital letter eta with dasia and varia and prosgegrammeni -> greek small letter eta with dasia and varia and ypogegrammeni
   ulmap[0x1f9c] = 0x1f94; // greek capital letter eta with psili and oxia and prosgegrammeni -> greek small letter eta with psili and oxia and ypogegrammeni
   ulmap[0x1f9d] = 0x1f95; // greek capital letter eta with dasia and oxia and prosgegrammeni -> greek small letter eta with dasia and oxia and ypogegrammeni
   ulmap[0x1f9e] = 0x1f96; // greek capital letter eta with psili and perispomeni and prosgegrammeni -> greek small letter eta with psili and perispomeni and ypogegrammeni
   ulmap[0x1f9f] = 0x1f97; // greek capital letter eta with dasia and perispomeni and prosgegrammeni -> greek small letter eta with dasia and perispomeni and ypogegrammeni
   ulmap[0x1fa8] = 0x1fa0; // greek capital letter omega with psili and prosgegrammeni -> greek small letter omega with psili and ypogegrammeni
   ulmap[0x1fa9] = 0x1fa1; // greek capital letter omega with dasia and prosgegrammeni -> greek small letter omega with dasia and ypogegrammeni
   ulmap[0x1faa] = 0x1fa2; // greek capital letter omega with psili and varia and prosgegrammeni -> greek small letter omega with psili and varia and ypogegrammeni
   ulmap[0x1fab] = 0x1fa3; // greek capital letter omega with dasia and varia and prosgegrammeni -> greek small letter omega with dasia and varia and ypogegrammeni
   ulmap[0x1fac] = 0x1fa4; // greek capital letter omega with psili and oxia and prosgegrammeni -> greek small letter omega with psili and oxia and ypogegrammeni
   ulmap[0x1fad] = 0x1fa5; // greek capital letter omega with dasia and oxia and prosgegrammeni -> greek small letter omega with dasia and oxia and ypogegrammeni
   ulmap[0x1fae] = 0x1fa6; // greek capital letter omega with psili and perispomeni and prosgegrammeni -> greek small letter omega with psili and perispomeni and ypogegrammeni
   ulmap[0x1faf] = 0x1fa7; // greek capital letter omeca with dasia and perispomeni and prosgegrammeni -> greek small letter omega with dasia and pepispomeni and ypogegrammeni
   ulmap[0x1fb8] = 0x1fb0; // greek capital letter alpha with vrachy -> greek small letter alpha with vrachy
   ulmap[0x1fb9] = 0x1fb1; // greek capital letter alpha with macron -> greek small letter alpha with macron
   ulmap[0x1fd8] = 0x1fd0; // greek capital letter iota with vrachy -> greek small letter iota with vrachy
   ulmap[0x1fd9] = 0x1fd1; // greek capital letter iota with macron -> greek small letter iota with macron
   ulmap[0x1fe8] = 0x1fe0; // greek capital letter upsilon with vrachy -> greek small letter upsilon with vrachy
   ulmap[0x1fe9] = 0x1fe1; // greek capital letter upsilon with macron -> greek small letter upsilon with macron
   ulmap[0x24b6] = 0x24d0; // circled latin capital letter a -> circled latin small letter a
   ulmap[0x24b7] = 0x24d1; // circled latin capital letter b -> circled latin small letter b
   ulmap[0x24b8] = 0x24d2; // circled latin capital letter c -> circled latin small letter c
   ulmap[0x24b9] = 0x24d3; // circled latin capital letter d -> circled latin small letter d
   ulmap[0x24ba] = 0x24d4; // circled latin capital letter e -> circled latin small letter e
   ulmap[0x24bb] = 0x24d5; // circled latin capital letter f -> circled latin small letter f
   ulmap[0x24bc] = 0x24d6; // circled latin capital letter g -> circled latin small letter g
   ulmap[0x24bd] = 0x24d7; // circled latin capital letter h -> circled latin small letter h
   ulmap[0x24be] = 0x24d8; // circled latin capital letter i -> circled latin small letter i
   ulmap[0x24bf] = 0x24d9; // circled latin capital letter j -> circled latin small letter j
   ulmap[0x24c0] = 0x24da; // circled latin capital letter k -> circled latin small letter k
   ulmap[0x24c1] = 0x24db; // circled latin capital letter l -> circled latin small letter l
   ulmap[0x24c2] = 0x24dc; // circled latin capital letter m -> circled latin small letter m
   ulmap[0x24c3] = 0x24dd; // circled latin capital letter n -> circled latin small letter n
   ulmap[0x24c4] = 0x24de; // circled latin capital letter o -> circled latin small letter o
   ulmap[0x24c5] = 0x24df; // circled latin capital letter p -> circled latin small letter p
   ulmap[0x24c6] = 0x24e0; // circled latin capital letter q -> circled latin small letter q
   ulmap[0x24c7] = 0x24e1; // circled latin capital letter r -> circled latin small letter r
   ulmap[0x24c8] = 0x24e2; // circled latin capital letter s -> circled latin small letter s
   ulmap[0x24c9] = 0x24e3; // circled latin capital letter t -> circled latin small letter t
   ulmap[0x24ca] = 0x24e4; // circled latin capital letter u -> circled latin small letter u
   ulmap[0x24cb] = 0x24e5; // circled latin capital letter v -> circled latin small letter v
   ulmap[0x24cc] = 0x24e6; // circled latin capital letter w -> circled latin small letter w
   ulmap[0x24cd] = 0x24e7; // circled latin capital letter x -> circled latin small letter x
   ulmap[0x24ce] = 0x24e8; // circled latin capital letter y -> circled latin small letter y
   ulmap[0x24cf] = 0x24e9; // circled latin capital letter z -> circled latin small letter z
   ulmap[0xff21] = 0xff41; // fullwidth latin capital letter a -> fullwidth latin small letter a
   ulmap[0xff22] = 0xff42; // fullwidth latin capital letter b -> fullwidth latin small letter b
   ulmap[0xff23] = 0xff43; // fullwidth latin capital letter c -> fullwidth latin small letter c
   ulmap[0xff24] = 0xff44; // fullwidth latin capital letter d -> fullwidth latin small letter d
   ulmap[0xff25] = 0xff45; // fullwidth latin capital letter e -> fullwidth latin small letter e
   ulmap[0xff26] = 0xff46; // fullwidth latin capital letter f -> fullwidth latin small letter f
   ulmap[0xff27] = 0xff47; // fullwidth latin capital letter g -> fullwidth latin small letter g
   ulmap[0xff28] = 0xff48; // fullwidth latin capital letter h -> fullwidth latin small letter h
   ulmap[0xff29] = 0xff49; // fullwidth latin capital letter i -> fullwidth latin small letter i
   ulmap[0xff2a] = 0xff4a; // fullwidth latin capital letter j -> fullwidth latin small letter j
   ulmap[0xff2b] = 0xff4b; // fullwidth latin capital letter k -> fullwidth latin small letter k
   ulmap[0xff2c] = 0xff4c; // fullwidth latin capital letter l -> fullwidth latin small letter l
   ulmap[0xff2d] = 0xff4d; // fullwidth latin capital letter m -> fullwidth latin small letter m
   ulmap[0xff2e] = 0xff4e; // fullwidth latin capital letter n -> fullwidth latin small letter n
   ulmap[0xff2f] = 0xff4f; // fullwidth latin capital letter o -> fullwidth latin small letter o
   ulmap[0xff30] = 0xff50; // fullwidth latin capital letter p -> fullwidth latin small letter p
   ulmap[0xff31] = 0xff51; // fullwidth latin capital letter q -> fullwidth latin small letter q
   ulmap[0xff32] = 0xff52; // fullwidth latin capital letter r -> fullwidth latin small letter r
   ulmap[0xff33] = 0xff53; // fullwidth latin capital letter s -> fullwidth latin small letter s
   ulmap[0xff34] = 0xff54; // fullwidth latin capital letter t -> fullwidth latin small letter t
   ulmap[0xff35] = 0xff55; // fullwidth latin capital letter u -> fullwidth latin small letter u
   ulmap[0xff36] = 0xff56; // fullwidth latin capital letter v -> fullwidth latin small letter v
   ulmap[0xff37] = 0xff57; // fullwidth latin capital letter w -> fullwidth latin small letter w
   ulmap[0xff38] = 0xff58; // fullwidth latin capital letter x -> fullwidth latin small letter x
   ulmap[0xff39] = 0xff59; // fullwidth latin capital letter y -> fullwidth latin small letter y
   ulmap[0xff3a] = 0xff5a; // fullwidth latin capital letter z -> fullwidth latin small letter z
}

int do_unaccent(QoreString& str, ExceptionSink* xsink) {
   assert(str.getEncoding() == QCS_UTF8);

   char cbuf[5];

   // loop through non-ascii characters and substitute any accented ones
   for (const char* p = (char*)str.getBuffer(), *e = p + str.size(); p < e; ++p) {
      if (!((*p) & 0x80))
	 continue;

      // find character length
      qore_size_t cl = q_get_char_len(QCS_UTF8, p, e - p, xsink);
      if (*xsink)
	 return -1;
      
      assert(cl < 5);
      memcpy(cbuf, p, cl);
      // terminate string
      cbuf[cl] = 0;
      charmap_t::const_iterator i = accent_map.find(cbuf);
      if (i == accent_map.end()) {
	 p += cl - 1;
	 continue;
      }
      unsigned nl = strlen(i->second);
      
      qore_size_t offset = p - str.getBuffer();
      
      // replace the accented character with the new character(s)
      str.replace(offset, cl, i->second);
      
      // we have to recalculate p & e because the buffer may have been reallocated by replace()
      p = str.getBuffer() + offset + nl - 1;
      e = str.getBuffer() + str.size();	      
   }

   return 0;
}

typedef int (*ascii_func_t)(int c);
static int q_ascii_tolower(int c) {
   return c > 64 && c < 91 ? c + 32 : c;
}
static int q_ascii_toupper(int c) {
   return c > 96 && c < 123 ? c - 32 : c;
}

static int apply_unicode_map(const unicodemap_t& umap, ascii_func_t func, QoreString& str, const QoreString& src, ExceptionSink* xsink) {
   assert(str.empty());
   assert(str.getEncoding() == src.getEncoding());

   //printd(5, "apply_unicode_map() source: '%s' (%s)\n", src.getBuffer(), src.getEncoding()->getCode());

   for (const char* p = src.getBuffer(), *e = p + src.size(); p < e; ++p) {
      // if we discover a non-ASCII character, then we have to start worrying about conversions
      if ((*p) & 0x80) {
	 unsigned len;
	 unsigned uc = src.getUnicodePointFromBytePos(p - src.getBuffer(), len, xsink);
	 if (*xsink)
	    return -1;
	 // see if there is an upper->lower mapping
	 unicodemap_t::const_iterator i = umap.find(uc);
	 // if the character was not found, then just add the original character
	 if (i == umap.end()) {
	    //printd(5, "apply_unicode_map() no match found for %x (%d)\n", uc, ulmap.size());
	    for (unsigned j = 0; j < len; ++j) {
	       str.concat(*(p + j));
	    }
	 }
	 else {
	    // otherwise concatenate the new character
	    str.concatUnicode(i->second, xsink);
	    if (*xsink)
	       return -1;
	 }
	 p += (len - 1);
	 continue;
      }
      str.concat(func(*p));
   }

   return 0;
}

int do_tolower(QoreString& str, const QoreString& src, ExceptionSink* xsink) {
   return apply_unicode_map(ulmap, q_ascii_tolower, str, src, xsink);
}

int do_toupper(QoreString& str, const QoreString& src, ExceptionSink* xsink) {
   return apply_unicode_map(lumap, q_ascii_toupper, str, src, xsink);
}
