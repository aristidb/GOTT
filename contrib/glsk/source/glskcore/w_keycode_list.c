/* ANSI-C code produced by gperf version 3.0.1 */
/* Command-line: gperf --output-file=w_keycode_list.c w_keycode_list.gperf  */
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

#line 9 "w_keycode_list.gperf"

#include <glsk/glsk.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#line 15 "w_keycode_list.gperf"
struct keystring_entry { const char* name; unsigned char code; };
enum
  {
    TOTAL_KEYWORDS = 145,
    MIN_WORD_LENGTH = 1,
    MAX_WORD_LENGTH = 12,
    MIN_HASH_VALUE = 1,
    MAX_HASH_VALUE = 481
  };

/* maximum key range = 481, duplicates = 0 */

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
      482, 482, 482, 482, 482, 482, 482, 482, 482, 482,
      482, 482, 482, 482, 482, 482, 482, 482, 482, 482,
      482, 482, 482, 482, 482, 482, 482, 482, 482, 482,
      482, 482, 482, 482, 482, 482, 482, 482, 482, 240,
      482, 482,  35,  25, 482, 190,   0, 185, 170,  90,
       95, 115, 110, 105, 165, 145, 140, 135, 235, 230,
      482, 180, 482, 482, 225,   5,  80,  40,  75,  10,
       70, 175, 160, 100, 200,  65,  20, 120,  45,  60,
       15, 150,  50,   0,  30,  35, 130,  25, 155, 125,
      195, 220, 215, 210, 205, 482, 482,   5,  80,  40,
       75,  10,  70, 175, 160, 100, 200,  65,  20, 120,
       45,  60,  15, 150,  50,   0,  30,  35, 130,  25,
      155, 125, 195, 482, 482, 482, 482, 482
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
#line 47 "w_keycode_list.gperf"
    {"s", DIK_S },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 46 "w_keycode_list.gperf"
    {"a", DIK_A },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 34 "w_keycode_list.gperf"
    {"e", DIK_E },
    {(char*)0}, {(char*)0},
#line 149 "w_keycode_list.gperf"
    {"Apps", DIK_APPS },
    {(char*)0},
#line 17 "w_keycode_list.gperf"
    {"Escape", DIK_ESCAPE },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 73 "w_keycode_list.gperf"
    {"Space", DIK_SPACE },
#line 41 "w_keycode_list.gperf"
    {"p", DIK_P },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 136 "w_keycode_list.gperf"
    {"Pause", DIK_PAUSE },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 151 "w_keycode_list.gperf"
    {"Sleep", DIK_SLEEP },
#line 54 "w_keycode_list.gperf"
    {"l", DIK_L },
    {(char*)0}, {(char*)0},
#line 152 "w_keycode_list.gperf"
    {"Wake", DIK_WAKE },
    {(char*)0}, {(char*)0},
#line 154 "w_keycode_list.gperf"
    {"WebFavorites", DIK_WEBFAVORITES },
    {(char*)0},
#line 119 "w_keycode_list.gperf"
    {"Stop", DIK_STOP },
    {(char*)0},
#line 33 "w_keycode_list.gperf"
    {"w", DIK_W },
#line 131 "w_keycode_list.gperf"
    {"WebHome", DIK_WEBHOME },
    {(char*)0},
#line 127 "w_keycode_list.gperf"
    {"PlayPause", DIK_PLAYPAUSE },
    {(char*)0},
#line 58 "w_keycode_list.gperf"
    {"LShift", DIK_LSHIFT },
#line 156 "w_keycode_list.gperf"
    {"WebStop", DIK_WEBSTOP },
    {(char*)0},
#line 72 "w_keycode_list.gperf"
    {"LAlt", DIK_LMENU },
    {(char*)0},
#line 36 "w_keycode_list.gperf"
    {"t", DIK_T },
    {(char*)0}, {(char*)0},
#line 140 "w_keycode_list.gperf"
    {"Left", DIK_LEFT },
    {(char*)0},
#line 86 "w_keycode_list.gperf"
    {"Scroll", DIK_SCROLL },
#line 138 "w_keycode_list.gperf"
    {"Up", DIK_UP },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 38 "w_keycode_list.gperf"
    {"u", DIK_U },
#line 74 "w_keycode_list.gperf"
    {"Capital", DIK_CAPITAL },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0},
#line 106 "w_keycode_list.gperf"
    {"Kana", DIK_KANA },
    {(char*)0},
#line 62 "w_keycode_list.gperf"
    {"c", DIK_C },
    {(char*)0}, {(char*)0},
#line 99 "w_keycode_list.gperf"
    {"Num.", DIK_DECIMAL },
#line 45 "w_keycode_list.gperf"
    {"LCtrl", DIK_LCONTROL },
#line 70 "w_keycode_list.gperf"
    {"RShift", DIK_RSHIFT },
    {(char*)0}, {(char*)0},
#line 135 "w_keycode_list.gperf"
    {"RAlt", DIK_RMENU },
    {(char*)0},
#line 65 "w_keycode_list.gperf"
    {"n", DIK_N },
    {(char*)0}, {(char*)0},
#line 147 "w_keycode_list.gperf"
    {"LWin", DIK_LWIN },
    {(char*)0},
#line 132 "w_keycode_list.gperf"
    {"NumpadComma", DIK_NUMPADCOMMA },
    {(char*)0}, {(char*)0},
#line 117 "w_keycode_list.gperf"
    {"UnderLine", DIK_UNDERLINE },
    {(char*)0},
#line 35 "w_keycode_list.gperf"
    {"r", DIK_R },
    {(char*)0}, {(char*)0},
#line 30 "w_keycode_list.gperf"
    {"Backspace", DIK_BACK },
#line 126 "w_keycode_list.gperf"
    {"Calculator", DIK_CALCULATOR },
#line 68 "w_keycode_list.gperf"
    {"Period", DIK_PERIOD },
#line 158 "w_keycode_list.gperf"
    {"WebBack", DIK_WEBBACK },
#line 146 "w_keycode_list.gperf"
    {"Del", DIK_DELETE },
#line 94 "w_keycode_list.gperf"
    {"Num+", DIK_ADD },
#line 67 "w_keycode_list.gperf"
    {"Comma", DIK_COMMA },
#line 44 "w_keycode_list.gperf"
    {"Return", DIK_RETURN },
    {(char*)0}, {(char*)0},
#line 124 "w_keycode_list.gperf"
    {"RCtl", DIK_RCONTROL },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 31 "w_keycode_list.gperf"
    {"Tab", DIK_TAB },
#line 71 "w_keycode_list.gperf"
    {"Num*", DIK_MULTIPLY },
#line 157 "w_keycode_list.gperf"
    {"WebForward", DIK_WEBFORWARD },
#line 40 "w_keycode_list.gperf"
    {"o", DIK_O },
    {(char*)0}, {(char*)0},
#line 148 "w_keycode_list.gperf"
    {"RWin", DIK_RWIN },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 122 "w_keycode_list.gperf"
    {"NextTrack", DIK_NEXTTRACK },
#line 150 "w_keycode_list.gperf"
    {"Power", DIK_POWER },
#line 53 "w_keycode_list.gperf"
    {"k", DIK_K },
    {(char*)0},
#line 142 "w_keycode_list.gperf"
    {"End", DIK_END },
#line 123 "w_keycode_list.gperf"
    {"NumReturn", DIK_NUMPADENTER },
    {(char*)0}, {(char*)0},
#line 108 "w_keycode_list.gperf"
    {"Convert", DIK_CONVERT },
    {(char*)0},
#line 113 "w_keycode_list.gperf"
    {"PrevTrack", DIK_PREVTRACK },
    {(char*)0},
#line 49 "w_keycode_list.gperf"
    {"f", DIK_F },
    {(char*)0}, {(char*)0},
#line 109 "w_keycode_list.gperf"
    {"NoConvert", DIK_NOCONVERT },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 145 "w_keycode_list.gperf"
    {"Ins", DIK_INSERT },
#line 160 "w_keycode_list.gperf"
    {"Mail", DIK_MAIL},
    {(char*)0},
#line 48 "w_keycode_list.gperf"
    {"d", DIK_D },
#line 85 "w_keycode_list.gperf"
    {"NumLock", DIK_NUMLOCK },
    {(char*)0},
#line 128 "w_keycode_list.gperf"
    {"MediaStop", DIK_MEDIASTOP },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0},
#line 64 "w_keycode_list.gperf"
    {"b", DIK_B },
    {(char*)0}, {(char*)0},
#line 121 "w_keycode_list.gperf"
    {"UnLabeled", DIK_UNLABELED },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 125 "w_keycode_list.gperf"
    {"Mute", DIK_MUTE },
    {(char*)0},
#line 161 "w_keycode_list.gperf"
    {"MediaSelect", DIK_MEDIASELECT},
    {(char*)0}, {(char*)0},
#line 95 "w_keycode_list.gperf"
    {"Num1", DIK_NUMPAD1 },
#line 118 "w_keycode_list.gperf"
    {"Kanji", DIK_KANJI },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 96 "w_keycode_list.gperf"
    {"Num2", DIK_NUMPAD2 },
    {(char*)0},
#line 18 "w_keycode_list.gperf"
    {"1", DIK_1 },
#line 107 "w_keycode_list.gperf"
    {"Abnt_C1", DIK_ABNT_C1 },
#line 110 "w_keycode_list.gperf"
    {"Yen", DIK_YEN },
#line 143 "w_keycode_list.gperf"
    {"Down", DIK_DOWN },
#line 141 "w_keycode_list.gperf"
    {"Right", DIK_RIGHT },
    {(char*)0},
#line 111 "w_keycode_list.gperf"
    {"Abnt_C2", DIK_ABNT_C2 },
    {(char*)0},
#line 92 "w_keycode_list.gperf"
    {"Num5", DIK_NUMPAD5 },
    {(char*)0},
#line 19 "w_keycode_list.gperf"
    {"2", DIK_2 },
    {(char*)0},
#line 100 "w_keycode_list.gperf"
    {"OEM", DIK_OEM_102 },
#line 91 "w_keycode_list.gperf"
    {"Num4", DIK_NUMPAD4 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 97 "w_keycode_list.gperf"
    {"Num3", DIK_NUMPAD3 },
    {(char*)0},
#line 39 "w_keycode_list.gperf"
    {"i", DIK_I },
    {(char*)0}, {(char*)0},
#line 153 "w_keycode_list.gperf"
    {"WebSearch", DIK_WEBSEARCH },
#line 155 "w_keycode_list.gperf"
    {"WebRefresh", DIK_WEBREFRESH },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 139 "w_keycode_list.gperf"
    {"PgUp", DIK_PRIOR },
    {(char*)0},
#line 22 "w_keycode_list.gperf"
    {"5", DIK_5 },
    {(char*)0},
#line 130 "w_keycode_list.gperf"
    {"VolumeUp", DIK_VOLUMEUP },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 89 "w_keycode_list.gperf"
    {"Num9", DIK_NUMPAD9 },
    {(char*)0},
#line 21 "w_keycode_list.gperf"
    {"4", DIK_4 },
    {(char*)0}, {(char*)0},
#line 88 "w_keycode_list.gperf"
    {"Num8", DIK_NUMPAD8 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 87 "w_keycode_list.gperf"
    {"Num7", DIK_NUMPAD7 },
    {(char*)0},
#line 20 "w_keycode_list.gperf"
    {"3", DIK_3 },
    {(char*)0}, {(char*)0},
#line 137 "w_keycode_list.gperf"
    {"Home", DIK_HOME },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 144 "w_keycode_list.gperf"
    {"PgDn", DIK_NEXT },
#line 57 "w_keycode_list.gperf"
    {"Grave", DIK_GRAVE },
#line 66 "w_keycode_list.gperf"
    {"m", DIK_M },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 129 "w_keycode_list.gperf"
    {"VolumeDown", DIK_VOLUMEDOWN },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 93 "w_keycode_list.gperf"
    {"Num6", DIK_NUMPAD6 },
    {(char*)0},
#line 37 "w_keycode_list.gperf"
    {"y", DIK_Y },
#line 75 "w_keycode_list.gperf"
    {"F1", DIK_F1 },
#line 101 "w_keycode_list.gperf"
    {"F11", DIK_F11 },
#line 98 "w_keycode_list.gperf"
    {"Num0", DIK_NUMPAD0 },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 102 "w_keycode_list.gperf"
    {"F12", DIK_F12 },
    {(char*)0}, {(char*)0},
#line 63 "w_keycode_list.gperf"
    {"v", DIK_V },
#line 76 "w_keycode_list.gperf"
    {"F2", DIK_F2 },
    {(char*)0},
#line 112 "w_keycode_list.gperf"
    {"Num=", DIK_NUMPADEQUALS },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 105 "w_keycode_list.gperf"
    {"F15", DIK_F15 },
#line 133 "w_keycode_list.gperf"
    {"Num/", DIK_DIVIDE },
    {(char*)0},
#line 26 "w_keycode_list.gperf"
    {"9", DIK_9 },
    {(char*)0},
#line 104 "w_keycode_list.gperf"
    {"F14", DIK_F14 },
#line 90 "w_keycode_list.gperf"
    {"Num-", DIK_SUBTRACT },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 103 "w_keycode_list.gperf"
    {"F13", DIK_F13 },
    {(char*)0},
#line 134 "w_keycode_list.gperf"
    {"SysRq", DIK_SYSRQ },
#line 25 "w_keycode_list.gperf"
    {"8", DIK_8 },
#line 79 "w_keycode_list.gperf"
    {"F5", DIK_F5 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 24 "w_keycode_list.gperf"
    {"7", DIK_7 },
#line 78 "w_keycode_list.gperf"
    {"F4", DIK_F4 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 32 "w_keycode_list.gperf"
    {"q", DIK_Q },
#line 77 "w_keycode_list.gperf"
    {"F3", DIK_F3 },
    {(char*)0}, {(char*)0},
#line 159 "w_keycode_list.gperf"
    {"MyComputer", DIK_MYCOMPUTER},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 61 "w_keycode_list.gperf"
    {"x", DIK_X },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 120 "w_keycode_list.gperf"
    {"AX", DIK_AX },
    {(char*)0}, {(char*)0}, {(char*)0},
#line 51 "w_keycode_list.gperf"
    {"h", DIK_H },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 23 "w_keycode_list.gperf"
    {"6", DIK_6 },
    {(char*)0},
#line 84 "w_keycode_list.gperf"
    {"F10", DIK_F10 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0},
#line 27 "w_keycode_list.gperf"
    {"0", DIK_0 },
#line 83 "w_keycode_list.gperf"
    {"F9", DIK_F9 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 50 "w_keycode_list.gperf"
    {"g", DIK_G },
#line 82 "w_keycode_list.gperf"
    {"F8", DIK_F8 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 29 "w_keycode_list.gperf"
    {"=", DIK_EQUALS },
#line 81 "w_keycode_list.gperf"
    {"F7", DIK_F7 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 69 "w_keycode_list.gperf"
    {"/", DIK_SLASH },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 28 "w_keycode_list.gperf"
    {"-", DIK_MINUS },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 60 "w_keycode_list.gperf"
    {"z", DIK_Z },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 52 "w_keycode_list.gperf"
    {"j", DIK_J },
#line 80 "w_keycode_list.gperf"
    {"F6", DIK_F6 },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
#line 114 "w_keycode_list.gperf"
    {"^", DIK_CIRCUMFLEX },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 43 "w_keycode_list.gperf"
    {"]", DIK_RBRACKET },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 59 "w_keycode_list.gperf"
    {"\\", DIK_BACKSLASH },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 42 "w_keycode_list.gperf"
    {"[", DIK_LBRACKET },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 115 "w_keycode_list.gperf"
    {"@", DIK_AT },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 55 "w_keycode_list.gperf"
    {";", DIK_SEMICOLON },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 116 "w_keycode_list.gperf"
    {":", DIK_COLON },
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0}, {(char*)0}, {(char*)0}, {(char*)0},
    {(char*)0},
#line 56 "w_keycode_list.gperf"
    {"'", DIK_APOSTROPHE }
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
#line 162 "w_keycode_list.gperf"

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
