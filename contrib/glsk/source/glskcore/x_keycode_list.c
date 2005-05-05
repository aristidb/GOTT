/* ANSI-C code produced by gperf version 3.0.1 */
/* Command-line: gperf --output-file=x_keycode_list.c x_keycode_list.gperf  */
/* Computed positions: -k'1-2,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 9 "x_keycode_list.gperf"

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xdefs.h>
#include <X11/keysym.h>
#include <glsk/glsk.h>
#include "x_input.h"
#line 18 "x_keycode_list.gperf"
struct keystring_entry { const char* name; unsigned char code; };
enum
  {
    TOTAL_KEYWORDS = 144,
    MIN_WORD_LENGTH = 1,
    MAX_WORD_LENGTH = 12,
    MIN_HASH_VALUE = 1,
    MAX_HASH_VALUE = 471
  };

/* maximum key range = 471, duplicates = 0 */

#ifndef GPERF_DOWNCASE
#define GPERF_DOWNCASE 1
static unsigned char gperf_downcase[256] =
  {
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
     30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
     45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
     60,  61,  62,  63,  64,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106,
    107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
    122,  91,  92,  93,  94,  95,  96,  97,  98,  99, 100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134,
    135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
    165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194,
    195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
    225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254,
    255
  };
#endif

#ifndef GPERF_CASE_STRCMP
#define GPERF_CASE_STRCMP 1
static int
gperf_case_strcmp (register const char *s1, register const char *s2)
{
  for (;;)
    {
      unsigned char c1 = gperf_downcase[(unsigned char)*s1++];
      unsigned char c2 = gperf_downcase[(unsigned char)*s2++];
      if (c1 != 0 && c1 == c2)
        continue;
      return (int)c1 - (int)c2;
    }
}
#endif

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      472, 472, 472, 472, 472, 472, 472, 472, 472, 472,
      472, 472, 472, 472, 472, 472, 472, 472, 472, 472,
      472, 472, 472, 472, 472, 472, 472, 472, 472, 472,
      472, 472, 472, 472, 472, 472, 472, 472, 472, 235,
      472, 472,  35,  25, 472, 190,   0, 185, 170,  90,
       95, 115, 110, 105, 165, 145, 140, 135, 230, 225,
      472, 180, 472, 472, 220,   5,  80,  40,  75,  10,
       70, 175, 160, 100, 200,  65,  20, 120,  45,  60,
       15, 150,  50,   0,  30,  35, 130,  25, 155, 125,
      195, 215, 210, 205, 472, 472, 472,   5,  80,  40,
       75,  10,  70, 175, 160, 100, 200,  65,  20, 120,
       45,  60,  15, 150,  50,   0,  30,  35, 130,  25,
      155, 125, 195, 472, 472, 472, 472, 472
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

static const struct keystring_entry wordlist[] =
  {
    {(char*)0},
#line 51 "x_keycode_list.gperf"
    {"s", KEY_S },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 50 "x_keycode_list.gperf"
    {"a", KEY_A },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 38 "x_keycode_list.gperf"
    {"e", KEY_E },
    {(char*)0}, {(char*)0},
#line 153 "x_keycode_list.gperf"
    {"Apps", KEY_APPS },
    {(char*)0},
#line 21 "x_keycode_list.gperf"
    {"Escape", KEY_ESCAPE },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 77 "x_keycode_list.gperf"
    {"Space", KEY_SPACE },
#line 45 "x_keycode_list.gperf"
    {"p", KEY_P },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 140 "x_keycode_list.gperf"
    {"Pause", KEY_PAUSE },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 155 "x_keycode_list.gperf"
    {"Sleep", KEY_SLEEP },
#line 58 "x_keycode_list.gperf"
    {"l", KEY_L },
    {(char*)0}, {(char*)0},
#line 156 "x_keycode_list.gperf"
    {"Wake", KEY_WAKE },
    {(char*)0}, {(char*)0},
#line 158 "x_keycode_list.gperf"
    {"WebFavorites", KEY_WEBFAVORITES },
    {(char*)0},
#line 123 "x_keycode_list.gperf"
    {"Stop", KEY_STOP },
    {(char*)0},
#line 37 "x_keycode_list.gperf"
    {"w", KEY_W },
#line 135 "x_keycode_list.gperf"
    {"WebHome", KEY_WEBHOME },
    {(char*)0},
#line 131 "x_keycode_list.gperf"
    {"PlayPause", KEY_PLAYPAUSE },
    {(char*)0},
#line 62 "x_keycode_list.gperf"
    {"LShift", KEY_LSHIFT },
#line 160 "x_keycode_list.gperf"
    {"WebStop", KEY_WEBSTOP },
    {(char*)0},
#line 76 "x_keycode_list.gperf"
    {"LAlt", KEY_ALT },
    {(char*)0},
#line 40 "x_keycode_list.gperf"
    {"t", KEY_T },
    {(char*)0}, {(char*)0},
#line 144 "x_keycode_list.gperf"
    {"Left", KEY_LEFT },
    {(char*)0},
#line 90 "x_keycode_list.gperf"
    {"Scroll", KEY_SCROLL },
#line 142 "x_keycode_list.gperf"
    {"Up", KEY_UP },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 42 "x_keycode_list.gperf"
    {"u", KEY_U },
#line 78 "x_keycode_list.gperf"
    {"Capital", KEY_CAPITAL },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0},
#line 110 "x_keycode_list.gperf"
    {"Kana", KEY_KANA },
    {(char*)0},
#line 66 "x_keycode_list.gperf"
    {"c", KEY_C },
    {(char*)0}, {(char*)0},
#line 103 "x_keycode_list.gperf"
    {"Num.", KEY_DECIMAL },
#line 49 "x_keycode_list.gperf"
    {"LCtrl", KEY_LCONTROL },
#line 74 "x_keycode_list.gperf"
    {"RShift", KEY_RSHIFT },
    {(char*)0}, {(char*)0},
#line 139 "x_keycode_list.gperf"
    {"RAlt", KEY_ALT_GR },
    {(char*)0},
#line 69 "x_keycode_list.gperf"
    {"n", KEY_N },
    {(char*)0}, {(char*)0},
#line 151 "x_keycode_list.gperf"
    {"LWin", KEY_LWIN },
    {(char*)0},
#line 136 "x_keycode_list.gperf"
    {"NumpadComma", KEY_NUMPADCOMMA },
    {(char*)0}, {(char*)0},
#line 121 "x_keycode_list.gperf"
    {"UnderLine", KEY_UNDERLINE },
    {(char*)0},
#line 39 "x_keycode_list.gperf"
    {"r", KEY_R },
    {(char*)0}, {(char*)0},
#line 34 "x_keycode_list.gperf"
    {"Backspace", KEY_BACK },
#line 130 "x_keycode_list.gperf"
    {"Calculator", KEY_CALCULATOR },
#line 72 "x_keycode_list.gperf"
    {"Period", KEY_PERIOD },
#line 162 "x_keycode_list.gperf"
    {"WebBack", KEY_WEBBACK },
#line 150 "x_keycode_list.gperf"
    {"Del", KEY_DELETE },
#line 98 "x_keycode_list.gperf"
    {"Num+", KEY_ADD },
#line 71 "x_keycode_list.gperf"
    {"Comma", KEY_COMMA },
#line 48 "x_keycode_list.gperf"
    {"Return", KEY_RETURN },
    {(char*)0}, {(char*)0},
#line 128 "x_keycode_list.gperf"
    {"RCtl", KEY_RCONTROL },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 35 "x_keycode_list.gperf"
    {"Tab", KEY_TAB },
#line 75 "x_keycode_list.gperf"
    {"Num*", KEY_MULTIPLY },
#line 161 "x_keycode_list.gperf"
    {"WebForward", KEY_WEBFORWARD },
#line 44 "x_keycode_list.gperf"
    {"o", KEY_O },
    {(char*)0}, {(char*)0},
#line 152 "x_keycode_list.gperf"
    {"RWin", KEY_RWIN },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 126 "x_keycode_list.gperf"
    {"NextTrack", KEY_NEXTTRACK },
#line 154 "x_keycode_list.gperf"
    {"Power", KEY_POWER },
#line 57 "x_keycode_list.gperf"
    {"k", KEY_K },
    {(char*)0},
#line 146 "x_keycode_list.gperf"
    {"End", KEY_END },
#line 127 "x_keycode_list.gperf"
    {"NumReturn", KEY_NUMPADENTER },
    {(char*)0}, {(char*)0},
#line 112 "x_keycode_list.gperf"
    {"Convert", KEY_CONVERT },
    {(char*)0},
#line 117 "x_keycode_list.gperf"
    {"PrevTrack", KEY_PREVTRACK },
    {(char*)0},
#line 53 "x_keycode_list.gperf"
    {"f", KEY_F },
    {(char*)0}, {(char*)0},
#line 113 "x_keycode_list.gperf"
    {"NoConvert", KEY_NOCONVERT },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 149 "x_keycode_list.gperf"
    {"Ins", KEY_INSERT },
#line 164 "x_keycode_list.gperf"
    {"Mail", KEY_MAIL},
    {(char*)0},
#line 52 "x_keycode_list.gperf"
    {"d", KEY_D },
#line 89 "x_keycode_list.gperf"
    {"NumLock", KEY_NUMLOCK },
    {(char*)0},
#line 132 "x_keycode_list.gperf"
    {"MediaStop", KEY_MEDIASTOP },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0},
#line 68 "x_keycode_list.gperf"
    {"b", KEY_B },
    {(char*)0}, {(char*)0},
#line 125 "x_keycode_list.gperf"
    {"UnLabeled", KEY_UNLABELED },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 129 "x_keycode_list.gperf"
    {"Mute", KEY_MUTE },
    {(char*)0},
#line 165 "x_keycode_list.gperf"
    {"MediaSelect", KEY_MEDIASELECT},
    {(char*)0}, {(char*)0},
#line 99 "x_keycode_list.gperf"
    {"Num1", KEY_NUMPAD1 },
#line 122 "x_keycode_list.gperf"
    {"Kanji", KEY_KANJI },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 100 "x_keycode_list.gperf"
    {"Num2", KEY_NUMPAD2 },
    {(char*)0},
#line 22 "x_keycode_list.gperf"
    {"1", KEY_1 },
#line 111 "x_keycode_list.gperf"
    {"Abnt_C1", KEY_ABNT_C1 },
#line 114 "x_keycode_list.gperf"
    {"Yen", KEY_YEN },
#line 147 "x_keycode_list.gperf"
    {"Down", KEY_DOWN },
#line 145 "x_keycode_list.gperf"
    {"Right", KEY_RIGHT },
    {(char*)0},
#line 115 "x_keycode_list.gperf"
    {"Abnt_C2", KEY_ABNT_C2 },
    {(char*)0},
#line 96 "x_keycode_list.gperf"
    {"Num5", KEY_NUMPAD5 },
    {(char*)0},
#line 23 "x_keycode_list.gperf"
    {"2", KEY_2 },
    {(char*)0},
#line 104 "x_keycode_list.gperf"
    {"OEM", KEY_OEM_102 },
#line 95 "x_keycode_list.gperf"
    {"Num4", KEY_NUMPAD4 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 101 "x_keycode_list.gperf"
    {"Num3", KEY_NUMPAD3 },
    {(char*)0},
#line 43 "x_keycode_list.gperf"
    {"i", KEY_I },
    {(char*)0}, {(char*)0},
#line 157 "x_keycode_list.gperf"
    {"WebSearch", KEY_WEBSEARCH },
#line 159 "x_keycode_list.gperf"
    {"WebRefresh", KEY_WEBREFRESH },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 143 "x_keycode_list.gperf"
    {"PgUp", KEY_PRIOR },
    {(char*)0},
#line 26 "x_keycode_list.gperf"
    {"5", KEY_5 },
    {(char*)0},
#line 134 "x_keycode_list.gperf"
    {"VolumeUp", KEY_VOLUMEUP },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 93 "x_keycode_list.gperf"
    {"Num9", KEY_NUMPAD9 },
    {(char*)0},
#line 25 "x_keycode_list.gperf"
    {"4", KEY_4 },
    {(char*)0}, {(char*)0},
#line 92 "x_keycode_list.gperf"
    {"Num8", KEY_NUMPAD8 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 91 "x_keycode_list.gperf"
    {"Num7", KEY_NUMPAD7 },
    {(char*)0},
#line 24 "x_keycode_list.gperf"
    {"3", KEY_3 },
    {(char*)0}, {(char*)0},
#line 141 "x_keycode_list.gperf"
    {"Home", KEY_HOME },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 148 "x_keycode_list.gperf"
    {"PgDn", KEY_NEXT },
#line 61 "x_keycode_list.gperf"
    {"Grave", KEY_GRAVE },
#line 70 "x_keycode_list.gperf"
    {"m", KEY_M },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 133 "x_keycode_list.gperf"
    {"VolumeDown", KEY_VOLUMEDOWN },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 97 "x_keycode_list.gperf"
    {"Num6", KEY_NUMPAD6 },
    {(char*)0},
#line 41 "x_keycode_list.gperf"
    {"y", KEY_Y },
#line 79 "x_keycode_list.gperf"
    {"F1", KEY_F1 },
#line 105 "x_keycode_list.gperf"
    {"F11", KEY_F11 },
#line 102 "x_keycode_list.gperf"
    {"Num0", KEY_NUMPAD0 },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 106 "x_keycode_list.gperf"
    {"F12", KEY_F12 },
    {(char*)0}, {(char*)0},
#line 67 "x_keycode_list.gperf"
    {"v", KEY_V },
#line 80 "x_keycode_list.gperf"
    {"F2", KEY_F2 },
    {(char*)0},
#line 116 "x_keycode_list.gperf"
    {"Num=", KEY_NUMPADEQUALS },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 109 "x_keycode_list.gperf"
    {"F15", KEY_F15 },
#line 137 "x_keycode_list.gperf"
    {"Num/", KEY_DIVIDE },
    {(char*)0},
#line 30 "x_keycode_list.gperf"
    {"9", KEY_9 },
    {(char*)0},
#line 108 "x_keycode_list.gperf"
    {"F14", KEY_F14 },
#line 94 "x_keycode_list.gperf"
    {"Num-", KEY_SUBTRACT },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 107 "x_keycode_list.gperf"
    {"F13", KEY_F13 },
    {(char*)0},
#line 138 "x_keycode_list.gperf"
    {"SysRq", KEY_SYSRQ },
#line 29 "x_keycode_list.gperf"
    {"8", KEY_8 },
#line 83 "x_keycode_list.gperf"
    {"F5", KEY_F5 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 28 "x_keycode_list.gperf"
    {"7", KEY_7 },
#line 82 "x_keycode_list.gperf"
    {"F4", KEY_F4 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 36 "x_keycode_list.gperf"
    {"q", KEY_Q },
#line 81 "x_keycode_list.gperf"
    {"F3", KEY_F3 },
    {(char*)0}, {(char*)0},
#line 163 "x_keycode_list.gperf"
    {"MyComputer", KEY_MYCOMPUTER},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 65 "x_keycode_list.gperf"
    {"x", KEY_X },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 124 "x_keycode_list.gperf"
    {"AX", KEY_AX },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 55 "x_keycode_list.gperf"
    {"h", KEY_H },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 27 "x_keycode_list.gperf"
    {"6", KEY_6 },
    {(char*)0},
#line 88 "x_keycode_list.gperf"
    {"F10", KEY_F10 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0},
#line 31 "x_keycode_list.gperf"
    {"0", KEY_0 },
#line 87 "x_keycode_list.gperf"
    {"F9", KEY_F9 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 54 "x_keycode_list.gperf"
    {"g", KEY_G },
#line 86 "x_keycode_list.gperf"
    {"F8", KEY_F8 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 33 "x_keycode_list.gperf"
    {"=", KEY_EQUALS },
#line 85 "x_keycode_list.gperf"
    {"F7", KEY_F7 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 73 "x_keycode_list.gperf"
    {"/", KEY_SLASH },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 32 "x_keycode_list.gperf"
    {"-", KEY_MINUS },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 64 "x_keycode_list.gperf"
    {"z", KEY_Z },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 56 "x_keycode_list.gperf"
    {"j", KEY_J },
#line 84 "x_keycode_list.gperf"
    {"F6", KEY_F6 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 47 "x_keycode_list.gperf"
    {"]", KEY_RBRACKET },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 63 "x_keycode_list.gperf"
    {"\\", KEY_BACKSLASH },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 46 "x_keycode_list.gperf"
    {"[", KEY_LBRACKET },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 119 "x_keycode_list.gperf"
    {"@", KEY_AT },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 59 "x_keycode_list.gperf"
    {";", KEY_SEMICOLON },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 120 "x_keycode_list.gperf"
    {":", KEY_COLON },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 60 "x_keycode_list.gperf"
    {"'", KEY_APOSTROPHE }
  };

#ifdef __GNUC__
__inline
#endif
const struct keystring_entry *
_glsk_keycode_list_lookup (register const char *str, register unsigned int len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (s && (((unsigned char)*str ^ (unsigned char)*s) & ~32) == 0 && !gperf_case_strcmp (str, s))
            return &wordlist[key];
        }
    }
  return 0;
}
#line 166 "x_keycode_list.gperf"

int glsk_keyboard_get_keycode( const char *str )
{
	const struct keystring_entry* result = 0;
	
	if ( str == 0 )
		return 0;

	result = _glsk_keycode_list_lookup( str, (unsigned int)strlen( str ) );

	return (result) ? result->code : 0;
}

const char* glsk_keyboard_get_keyname( int keycode )
{
	int n = sizeof( wordlist ) / sizeof( struct keystring_entry );
	int i;

	for ( i = 0; i < n; ++i )
	{
		if ( wordlist[ i ].name && ( wordlist[ i ].code == keycode ) )
			return wordlist[ i ].name;
	}

	return "";
}
