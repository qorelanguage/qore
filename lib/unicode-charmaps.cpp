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

   for (unicodemap_t::const_iterator i = lumap.begin(), e = lumap.end(); i != e; ++i)
      ulmap[i->second] = i->first;
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
