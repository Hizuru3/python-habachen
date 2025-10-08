#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <assert.h>
#include <stdbool.h>
#include <structmember.h>


/* Types */

typedef int BoolPred;

typedef struct {PyObject_VAR_HEAD} PyStrOrSeqObject;


/* Memory Allocation */

#ifdef _MSC_VER
  #include <malloc.h>
  #define ALLOCA _alloca
  #define NO_CANARY __declspec(safebuffers)
#else
  #if !defined(alloca)
    #if defined(__GNUC__)
      #define alloca(x) __builtin_alloca(x)
    #elif defined(HAVE_ALLOCA_H)
      #include <alloca.h>
    #endif
  #endif
  #define ALLOCA alloca
  #define NO_CANARY
#endif


/* Token Concatenation */

#define JOIN_(x, y) x ## y
#define JOIN(x, y) JOIN_(x, y)


/* Backports of Newer CPython APIs */

#ifndef Py_SETREF
  #define Py_SETREF(op, op2) \
do { \
    PyObject *_py_tmp = (PyObject *)(op); \
    (op) = (op2); \
    Py_DECREF(_py_tmp); \
} while (0)
#endif


#ifndef Py_NewRef
static inline PyObject* _Py_NewRef(PyObject *obj)
{
    Py_INCREF(obj);
    return obj;
}
  #define Py_NewRef(obj) _Py_NewRef((PyObject*)(obj))
#endif


#ifndef Py_NO_INLINE
  #if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
    #define Py_NO_INLINE __attribute__ ((noinline))
  #elif defined(_MSC_VER)
    #define Py_NO_INLINE __declspec(noinline)
  #else
    #define Py_NO_INLINE
  #endif
#endif


#ifndef Py_BEGIN_CRITICAL_SECTION
  #define Py_BEGIN_CRITICAL_SECTION(x) {
#endif


#ifndef Py_END_CRITICAL_SECTION
  #define Py_END_CRITICAL_SECTION() }
#endif


// deprecated
#ifndef PyUnicode_READY
  #define PyUnicode_READY(op) (0)
#endif


/* Optimization and Debug */

#if !defined(NDEBUG)
  #define Habachen_assert(expr) assert(expr)
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
  #define Habachen_assert(expr) ({if (expr) {} else __builtin_unreachable();})
#elif defined(__clang__)
  #define Habachen_assert(expr) __builtin_assume(expr)
#elif defined(_MSC_VER) || defined(__INTEL_COMPILER)
  #define Habachen_assert(expr) __assume(expr)
#else
  #define Habachen_assert(expr) assert(expr)
#endif


#define DEBUG_PRINT(fmt, ...) do { \
    PyObject *_obj = PyUnicode_FromFormat(fmt "\n", __VA_ARGS__); \
    PyObject_Print(_obj, stdout, Py_PRINT_RAW); \
    Py_XDECREF(_obj); \
} while (0)


/* Accessing Unicode Strings */

#if PY_VERSION_HEX >= 0x030b0000
static inline Py_ALWAYS_INLINE Py_UCS4
Habachen_MAX_CHAR_VALUE(PyObject *op) {
    return PyUnicode_MAX_CHAR_VALUE(op);
}
#else
  #define Habachen_MAX_CHAR_VALUE(op) PyUnicode_MAX_CHAR_VALUE(op)
#endif


#define Habachen_Unicode_WRITE(kind, data, index, value) \
do { \
    int _kind = (kind); \
    if (_kind == PyUnicode_2BYTE_KIND) { \
        ((Py_UCS2 *)(data))[(index)] = (Py_UCS2)(value); \
    } else if (_kind == PyUnicode_1BYTE_KIND) { \
        ((Py_UCS1 *)(data))[(index)] = (Py_UCS1)(value); \
    } else { \
        Habachen_assert(_kind == PyUnicode_4BYTE_KIND); \
        ((Py_UCS4 *)(data))[(index)] = (Py_UCS4)(value); \
    } \
} while (0)


#define Habachen_Unicode_READ(KIND, data, index) \
    ((Py_UCS4) \
    ((KIND) == PyUnicode_2BYTE_KIND ? \
        ((const Py_UCS2 *)(data))[(index)] : \
        ((KIND) == PyUnicode_1BYTE_KIND ? \
            ((const Py_UCS1 *)(data))[(index)] : \
            ((const Py_UCS4 *)(data))[(index)] \
        ) \
    ))


#define UCSX_KIND(var) JOIN_(var, __KIND)
#define UCSX_DATA(var) JOIN_(var, __DATA)
#define DEF_TAGGED_UCS(var, pystr) \
    int UCSX_KIND(var) = PyUnicode_KIND(pystr); \
    void *UCSX_DATA(var) = PyUnicode_DATA(pystr)
#define UCSX_WRITE(var, i, x) \
    Habachen_Unicode_WRITE(UCSX_KIND(var), UCSX_DATA(var), (i), (x))
#define UCSX_READ(var, i) \
    Habachen_Unicode_READ(UCSX_KIND(var), UCSX_DATA(var), (i))


#define Habachen_Pred_with_KIND(KIND, ...) ( \
    (KIND == PyUnicode_2BYTE_KIND && (__VA_ARGS__)) || \
    (KIND == PyUnicode_1BYTE_KIND ? (__VA_ARGS__) : \
    (KIND != PyUnicode_2BYTE_KIND && \
    (Habachen_assert(KIND == PyUnicode_4BYTE_KIND), (__VA_ARGS__)))) \
)


/* Bit Flags */

enum {
    DIGIT_BIT = 0, ASCII_BIT = 1, KANA_BIT = 2, UNUSED_BIT = 4,
    DIGIT_FLAG = 1 << DIGIT_BIT,
    ASCII_FLAG = 1 << ASCII_BIT,
    KANA_FLAG = 1 << KANA_BIT
};

enum {DAKUTEN_FLAG = 64, HANDAKUTEN_FLAG = 128};


/* Conversion Tables */

static Py_UCS2 ascii_h2z_table[128]  = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    [' '] = 0x3000 /* '　' */,
    ['!'] = 0xff01 /* ！ */, ['"'] = 0xff02 /* ＂ */, ['#'] = 0xff03 /* ＃ */,
    ['$'] = 0xff04 /* ＄ */, ['%'] = 0xff05 /* ％ */, ['&'] = 0xff06 /* ＆ */,
    ['\''] = 0xff07 /* ＇ */, ['('] = 0xff08 /* （ */, [')'] = 0xff09 /* ） */,
    ['*'] = 0xff0a /* ＊ */, ['+'] = 0xff0b /* ＋ */, [','] = 0xff0c /* ， */,
    ['-'] = 0xff0d /* － */, ['.'] = 0xff0e /* ． */, ['/'] = 0xff0f /* ／ */,
    ['0'] = '0', ['1'] = '1', ['2'] = '2', ['3'] = '3', ['4'] = '4',
    ['5'] = '5', ['6'] = '6', ['7'] = '7', ['8'] = '8', ['9'] = '9',
    [':'] = 0xff1a /* ： */, [';'] = 0xff1b /* ； */, ['<'] = 0xff1c /* ＜ */,
    ['='] = 0xff1d /* ＝ */, ['>'] = 0xff1e /* ＞ */, ['?'] = 0xff1f /* ？ */,
    ['@'] = 0xff20 /* ＠ */,
    ['A'] = 0xff21 /* Ａ */, ['B'] = 0xff22 /* Ｂ */, ['C'] = 0xff23 /* Ｃ */,
    ['D'] = 0xff24 /* Ｄ */, ['E'] = 0xff25 /* Ｅ */, ['F'] = 0xff26 /* Ｆ */,
    ['G'] = 0xff27 /* Ｇ */, ['H'] = 0xff28 /* Ｈ */, ['I'] = 0xff29 /* Ｉ */,
    ['J'] = 0xff2a /* Ｊ */, ['K'] = 0xff2b /* Ｋ */, ['L'] = 0xff2c /* Ｌ */,
    ['M'] = 0xff2d /* Ｍ */, ['N'] = 0xff2e /* Ｎ */, ['O'] = 0xff2f /* Ｏ */,
    ['P'] = 0xff30 /* Ｐ */, ['Q'] = 0xff31 /* Ｑ */, ['R'] = 0xff32 /* Ｒ */,
    ['S'] = 0xff33 /* Ｓ */, ['T'] = 0xff34 /* Ｔ */, ['U'] = 0xff35 /* Ｕ */,
    ['V'] = 0xff36 /* Ｖ */, ['W'] = 0xff37 /* Ｗ */, ['X'] = 0xff38 /* Ｘ */,
    ['Y'] = 0xff39 /* Ｙ */, ['Z'] = 0xff3a /* Ｚ */,
    ['['] = 0xff3b /* ［ */, ['\\'] = 0xff3c /* ＼ */, [']'] = 0xff3d /* ］ */,
    ['^'] = 0xff3e /* ＾ */, ['_'] = 0xff3f /* ＿ */, ['`'] = 0xff40 /* ｀ */,
    ['a'] = 0xff41 /* ａ */, ['b'] = 0xff42 /* ｂ */, ['c'] = 0xff43 /* ｃ */,
    ['d'] = 0xff44 /* ｄ */, ['e'] = 0xff45 /* ｅ */, ['f'] = 0xff46 /* ｆ */,
    ['g'] = 0xff47 /* ｇ */, ['h'] = 0xff48 /* ｈ */, ['i'] = 0xff49 /* ｉ */,
    ['j'] = 0xff4a /* ｊ */, ['k'] = 0xff4b /* ｋ */, ['l'] = 0xff4c /* ｌ */,
    ['m'] = 0xff4d /* ｍ */, ['n'] = 0xff4e /* ｎ */, ['o'] = 0xff4f /* ｏ */,
    ['p'] = 0xff50 /* ｐ */, ['q'] = 0xff51 /* ｑ */, ['r'] = 0xff52 /* ｒ */,
    ['s'] = 0xff53 /* ｓ */, ['t'] = 0xff54 /* ｔ */, ['u'] = 0xff55 /* ｕ */,
    ['v'] = 0xff56 /* ｖ */, ['w'] = 0xff57 /* ｗ */, ['x'] = 0xff58 /* ｘ */,
    ['y'] = 0xff59 /* ｙ */, ['z'] = 0xff5a /* ｚ */,
    ['{'] = 0xff5b /* ｛ */, ['|'] = 0xff5c /* ｜ */, ['}'] = 0xff5d /* ｝ */,
    ['~'] = 0xff5e /* ～ */, ['\x7f'] = '\x7f'
};

static inline bool
is_ascii_digit(Py_UCS4 c) {return '0' <= c && c <= '9';}

static inline Py_UCS4
digit_h2z(Py_UCS4 c) {
    return c + (0xff10 /* ０ */ - '0');
}

static inline Py_UCS4
is_any_ascii(Py_UCS4 c) {return c < 128;}

static inline Py_UCS4
ascii_h2z(Py_UCS4 c) {
    return ascii_h2z_table[c];
}


enum {H_KANA_OFF = 0xff60, Z_KANA_OFF = 0x3080};

#define H_KANA(c) ((ptrdiff_t)(c) - H_KANA_OFF)
#define Z_KANA(c) ((ptrdiff_t)(c) - Z_KANA_OFF)

static Py_UCS2 kana_h2z_table[192] = {
    [H_KANA(0xff61 /* ｡ */)] = 0x3002 /* 。 */,
    [H_KANA(0xff62 /* ｢ */)] = 0x300c /* 「 */,
    [H_KANA(0xff63 /* ｣ */)] = 0x300d /* 」 */,
    [H_KANA(0xff64 /* ､ */)] = 0x3001 /* 、 */,
    [H_KANA(0xff65 /* ･ */)] = 0x30fb /* ・ */,
    [H_KANA(0xff66 /* ｦ */)] = 0x30f2 /* ヲ */,
    [H_KANA(0xff66 /* ｦ */) | DAKUTEN_FLAG] = 0x30fa /* ヺ */,
    [H_KANA(0xff67 /* ｧ */)] = 0x30a1 /* ァ */,
    [H_KANA(0xff68 /* ｨ */)] = 0x30a3 /* ィ */,
    [H_KANA(0xff69 /* ｩ */)] = 0x30a5 /* ゥ */,
    [H_KANA(0xff6a /* ｪ */)] = 0x30a7 /* ェ */,
    [H_KANA(0xff6b /* ｫ */)] = 0x30a9 /* ォ */,
    [H_KANA(0xff6c /* ｬ */)] = 0x30e3 /* ャ */,
    [H_KANA(0xff6d /* ｭ */)] = 0x30e5 /* ュ */,
    [H_KANA(0xff6e /* ｮ */)] = 0x30e7 /* ョ */,
    [H_KANA(0xff6f /* ｯ */)] = 0x30c3 /* ッ */,
    [H_KANA(0xff70 /* ｰ */)] = 0x30fc /* ー */,
    [H_KANA(0xff71 /* ｱ */)] = 0x30a2 /* ア */,
    [H_KANA(0xff72 /* ｲ */)] = 0x30a4 /* イ */,
    [H_KANA(0xff73 /* ｳ */)] = 0x30a6 /* ウ */,
    [H_KANA(0xff74 /* ｴ */)] = 0x30a8 /* エ */,
    [H_KANA(0xff75 /* ｵ */)] = 0x30aa /* オ */,
    [H_KANA(0xff73 /* ｳ */) | DAKUTEN_FLAG] = 0x30f4 /* ヴ */,
    [H_KANA(0xff76 /* ｶ */)] = 0x30ab /* カ */,
    [H_KANA(0xff77 /* ｷ */)] = 0x30ad /* キ */,
    [H_KANA(0xff78 /* ｸ */)] = 0x30af /* ク */,
    [H_KANA(0xff79 /* ｹ */)] = 0x30b1 /* ケ */,
    [H_KANA(0xff7a /* ｺ */)] = 0x30b3 /* コ */,
    [H_KANA(0xff76 /* ｶ */) | DAKUTEN_FLAG] = 0x30ac /* ガ */,
    [H_KANA(0xff77 /* ｷ */) | DAKUTEN_FLAG] = 0x30ae /* ギ */,
    [H_KANA(0xff78 /* ｸ */) | DAKUTEN_FLAG] = 0x30b0 /* グ */,
    [H_KANA(0xff79 /* ｹ */) | DAKUTEN_FLAG] = 0x30b2 /* ゲ */,
    [H_KANA(0xff7a /* ｺ */) | DAKUTEN_FLAG] = 0x30b4 /* ゴ */,
    [H_KANA(0xff7b /* ｻ */)] = 0x30b5 /* サ */,
    [H_KANA(0xff7c /* ｼ */)] = 0x30b7 /* シ */,
    [H_KANA(0xff7d /* ｽ */)] = 0x30b9 /* ス */,
    [H_KANA(0xff7e /* ｾ */)] = 0x30bb /* セ */,
    [H_KANA(0xff7f /* ｿ */)] = 0x30bd /* ソ */,
    [H_KANA(0xff7b /* ｻ */) | DAKUTEN_FLAG] = 0x30b6 /* ザ */,
    [H_KANA(0xff7c /* ｼ */) | DAKUTEN_FLAG] = 0x30b8 /* ジ */,
    [H_KANA(0xff7d /* ｽ */) | DAKUTEN_FLAG] = 0x30ba /* ズ */,
    [H_KANA(0xff7e /* ｾ */) | DAKUTEN_FLAG] = 0x30bc /* ゼ */,
    [H_KANA(0xff7f /* ｿ */) | DAKUTEN_FLAG] = 0x30be /* ゾ */,
    [H_KANA(0xff80 /* ﾀ */)] = 0x30bf /* タ */,
    [H_KANA(0xff81 /* ﾁ */)] = 0x30c1 /* チ */,
    [H_KANA(0xff82 /* ﾂ */)] = 0x30c4 /* ツ */,
    [H_KANA(0xff83 /* ﾃ */)] = 0x30c6 /* テ */,
    [H_KANA(0xff84 /* ﾄ */)] = 0x30c8 /* ト */,
    [H_KANA(0xff80 /* ﾀ */) | DAKUTEN_FLAG] = 0x30c0 /* ダ */,
    [H_KANA(0xff81 /* ﾁ */) | DAKUTEN_FLAG] = 0x30c2 /* ヂ */,
    [H_KANA(0xff82 /* ﾂ */) | DAKUTEN_FLAG] = 0x30c5 /* ヅ */,
    [H_KANA(0xff83 /* ﾃ */) | DAKUTEN_FLAG] = 0x30c7 /* デ */,
    [H_KANA(0xff84 /* ﾄ */) | DAKUTEN_FLAG] = 0x30c9 /* ド */,
    [H_KANA(0xff85 /* ﾅ */)] = 0x30ca /* ナ */,
    [H_KANA(0xff86 /* ﾆ */)] = 0x30cb /* ニ */,
    [H_KANA(0xff87 /* ﾇ */)] = 0x30cc /* ヌ */,
    [H_KANA(0xff88 /* ﾈ */)] = 0x30cd /* ネ */,
    [H_KANA(0xff89 /* ﾉ */)] = 0x30ce /* ノ */,
    [H_KANA(0xff8a /* ﾊ */)] = 0x30cf /* ハ */,
    [H_KANA(0xff8b /* ﾋ */)] = 0x30d2 /* ヒ */,
    [H_KANA(0xff8c /* ﾌ */)] = 0x30d5 /* フ */,
    [H_KANA(0xff8d /* ﾍ */)] = 0x30d8 /* ヘ */,
    [H_KANA(0xff8e /* ﾎ */)] = 0x30db /* ホ */,
    [H_KANA(0xff8a /* ﾊ */) | DAKUTEN_FLAG] = 0x30d0 /* バ */,
    [H_KANA(0xff8b /* ﾋ */) | DAKUTEN_FLAG] = 0x30d3 /* ビ */,
    [H_KANA(0xff8c /* ﾌ */) | DAKUTEN_FLAG] = 0x30d6 /* ブ */,
    [H_KANA(0xff8d /* ﾍ */) | DAKUTEN_FLAG] = 0x30d9 /* ベ */,
    [H_KANA(0xff8e /* ﾎ */) | DAKUTEN_FLAG] = 0x30dc /* ボ */,
    [H_KANA(0xff8a /* ﾊ */) | HANDAKUTEN_FLAG] = 0x30d1 /* パ */,
    [H_KANA(0xff8b /* ﾋ */) | HANDAKUTEN_FLAG] = 0x30d4 /* ピ */,
    [H_KANA(0xff8c /* ﾌ */) | HANDAKUTEN_FLAG] = 0x30d7 /* プ */,
    [H_KANA(0xff8d /* ﾍ */) | HANDAKUTEN_FLAG] = 0x30da /* ペ */,
    [H_KANA(0xff8e /* ﾎ */) | HANDAKUTEN_FLAG] = 0x30dd /* ポ */,
    [H_KANA(0xff8f /* ﾏ */)] = 0x30de /* マ */,
    [H_KANA(0xff90 /* ﾐ */)] = 0x30df /* ミ */,
    [H_KANA(0xff91 /* ﾑ */)] = 0x30e0 /* ム */,
    [H_KANA(0xff92 /* ﾒ */)] = 0x30e1 /* メ */,
    [H_KANA(0xff93 /* ﾓ */)] = 0x30e2 /* モ */,
    [H_KANA(0xff94 /* ﾔ */)] = 0x30e4 /* ヤ */,
    [H_KANA(0xff95 /* ﾕ */)] = 0x30e6 /* ユ */,
    [H_KANA(0xff96 /* ﾖ */)] = 0x30e8 /* ヨ */,
    [H_KANA(0xff97 /* ﾗ */)] = 0x30e9 /* ラ */,
    [H_KANA(0xff98 /* ﾘ */)] = 0x30ea /* リ */,
    [H_KANA(0xff99 /* ﾙ */)] = 0x30eb /* ル */,
    [H_KANA(0xff9a /* ﾚ */)] = 0x30ec /* レ */,
    [H_KANA(0xff9b /* ﾛ */)] = 0x30ed /* ロ */,
    [H_KANA(0xff9c /* ﾜ */)] = 0x30ef /* ワ */,
    [H_KANA(0xff9c /* ﾜ */) | DAKUTEN_FLAG] = 0x30f7 /* ヷ */,
    [H_KANA(0xff9d /* ﾝ */)] = 0x30f3 /* ン */,
    [H_KANA(0xff9e /* ﾞ */)] = 0x309b /* ゛ */,
    [H_KANA(0xff9f /* ﾟ */)] = 0x309c /* ゜ */
};


#define SPECIAL(c) ((ptrdiff_t)(c) - 0x3000)
#define ALPHA(c) ((ptrdiff_t)(c) - 0xff00)

#define SEL_SPECIAL(flags) (((size_t)(flags) >> 1) - 1)

static Py_UCS2 special_z2h_tables[][32] = {
    [SEL_SPECIAL(ASCII_FLAG)] = {
        [SPECIAL(0x3000 /* '　' */)] = ' ', [SPECIAL(0x301c /* '〜' */)] = '~',
    },
    [SEL_SPECIAL(KANA_FLAG)] = {
        [SPECIAL(0x3002 /* 。 */)] = 0xff61 /* ｡ */,
        [SPECIAL(0x300c /* 「 */)] = 0xff62 /* ｢ */,
        [SPECIAL(0x300d /* 」 */)] = 0xff63 /* ｣ */,
        [SPECIAL(0x3001 /* 、 */)] = 0xff64 /* ､ */,
    },
    [SEL_SPECIAL(ASCII_FLAG | KANA_FLAG)] = {
        [SPECIAL(0x3000 /* '　' */)] = ' ', [SPECIAL(0x301c /* '〜' */)] = '~',
        [SPECIAL(0x3002 /* 。 */)] = 0xff61 /* ｡ */,
        [SPECIAL(0x300c /* 「 */)] = 0xff62 /* ｢ */,
        [SPECIAL(0x300d /* 」 */)] = 0xff63 /* ｣ */,
        [SPECIAL(0x3001 /* 、 */)] = 0xff64 /* ､ */,
    }
};

static inline bool
is_zenkaku_digit(Py_UCS4 c) {
    return 0xff10 <= c && c <= 0xff19;
}

static inline bool
is_zenkaku_ascii_ex(Py_UCS4 c) {
    return 0xff01 <= c && c <= 0xffe5;
}

static inline Py_UCS4
ASCII_Z2H(Py_UCS4 c) {return c - 0xfee0;}

static inline Py_UCS4
ascii_ex_z2h(Py_UCS4 c) {
    Py_UCS4 res = ASCII_Z2H(c);
    return (0xff5f <= c) ? c : res;
}

static inline bool
is_special_char(Py_UCS4 c) {
    return 0x3000 <= c && c <= 0x301f;
}

static inline Py_UCS4
special_z2h(Py_UCS4 c, int32_t flags) {
    Py_UCS4 res = special_z2h_tables[SEL_SPECIAL(flags)][SPECIAL(c)];
    return !res ? c : res;
}


static unsigned char kana_z2h_table[128] = {
    [Z_KANA(0x3099)] = H_KANA(0xff9e /* ﾞ */),
    [Z_KANA(0x309a)] = H_KANA(0xff9f /* ﾟ */),
    [Z_KANA(0x309b /* ゛ */)] = H_KANA(0xff9e /* ﾞ */),
    [Z_KANA(0x309c /* ゜ */)] = H_KANA(0xff9f /* ﾟ */),
    [Z_KANA(0x30a1 /* ァ */)] = H_KANA(0xff67 /* ｧ */),
    [Z_KANA(0x30a3 /* ィ */)] = H_KANA(0xff68 /* ｨ */),
    [Z_KANA(0x30a5 /* ゥ */)] = H_KANA(0xff69 /* ｩ */),
    [Z_KANA(0x30a7 /* ェ */)] = H_KANA(0xff6a /* ｪ */),
    [Z_KANA(0x30a9 /* ォ */)] = H_KANA(0xff6b /* ｫ */),
    [Z_KANA(0x30a2 /* ア */)] = H_KANA(0xff71 /* ｱ */),
    [Z_KANA(0x30a4 /* イ */)] = H_KANA(0xff72 /* ｲ */),
    [Z_KANA(0x30a6 /* ウ */)] = H_KANA(0xff73 /* ｳ */),
    [Z_KANA(0x30a8 /* エ */)] = H_KANA(0xff74 /* ｴ */),
    [Z_KANA(0x30aa /* オ */)] = H_KANA(0xff75 /* ｵ */),
    [Z_KANA(0x30f4 /* ヴ */)] = H_KANA(0xff73 /* ｳ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30ab /* カ */)] = H_KANA(0xff76 /* ｶ */),
    [Z_KANA(0x30ad /* キ */)] = H_KANA(0xff77 /* ｷ */),
    [Z_KANA(0x30af /* ク */)] = H_KANA(0xff78 /* ｸ */),
    [Z_KANA(0x30b1 /* ケ */)] = H_KANA(0xff79 /* ｹ */),
    [Z_KANA(0x30b3 /* コ */)] = H_KANA(0xff7a /* ｺ */),
    [Z_KANA(0x30ac /* ガ */)] = H_KANA(0xff76 /* ｶ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30ae /* ギ */)] = H_KANA(0xff77 /* ｷ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30b0 /* グ */)] = H_KANA(0xff78 /* ｸ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30b2 /* ゲ */)] = H_KANA(0xff79 /* ｹ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30b4 /* ゴ */)] = H_KANA(0xff7a /* ｺ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30b5 /* サ */)] = H_KANA(0xff7b /* ｻ */),
    [Z_KANA(0x30b7 /* シ */)] = H_KANA(0xff7c /* ｼ */),
    [Z_KANA(0x30b9 /* ス */)] = H_KANA(0xff7d /* ｽ */),
    [Z_KANA(0x30bb /* セ */)] = H_KANA(0xff7e /* ｾ */),
    [Z_KANA(0x30bd /* ソ */)] = H_KANA(0xff7f /* ｿ */),
    [Z_KANA(0x30b6 /* ザ */)] = H_KANA(0xff7b /* ｻ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30b8 /* ジ */)] = H_KANA(0xff7c /* ｼ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30ba /* ズ */)] = H_KANA(0xff7d /* ｽ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30bc /* ゼ */)] = H_KANA(0xff7e /* ｾ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30be /* ゾ */)] = H_KANA(0xff7f /* ｿ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30bf /* タ */)] = H_KANA(0xff80 /* ﾀ */),
    [Z_KANA(0x30c1 /* チ */)] = H_KANA(0xff81 /* ﾁ */),
    [Z_KANA(0x30c4 /* ツ */)] = H_KANA(0xff82 /* ﾂ */),
    [Z_KANA(0x30c6 /* テ */)] = H_KANA(0xff83 /* ﾃ */),
    [Z_KANA(0x30c8 /* ト */)] = H_KANA(0xff84 /* ﾄ */),
    [Z_KANA(0x30c3 /* ッ */)] = H_KANA(0xff6f /* ｯ */),
    [Z_KANA(0x30c0 /* ダ */)] = H_KANA(0xff80 /* ﾀ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30c2 /* ヂ */)] = H_KANA(0xff81 /* ﾁ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30c5 /* ヅ */)] = H_KANA(0xff82 /* ﾂ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30c7 /* デ */)] = H_KANA(0xff83 /* ﾃ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30c9 /* ド */)] = H_KANA(0xff84 /* ﾄ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30ca /* ナ */)] = H_KANA(0xff85 /* ﾅ */),
    [Z_KANA(0x30cb /* ニ */)] = H_KANA(0xff86 /* ﾆ */),
    [Z_KANA(0x30cc /* ヌ */)] = H_KANA(0xff87 /* ﾇ */),
    [Z_KANA(0x30cd /* ネ */)] = H_KANA(0xff88 /* ﾈ */),
    [Z_KANA(0x30ce /* ノ */)] = H_KANA(0xff89 /* ﾉ */),
    [Z_KANA(0x30cf /* ハ */)] = H_KANA(0xff8a /* ﾊ */),
    [Z_KANA(0x30d2 /* ヒ */)] = H_KANA(0xff8b /* ﾋ */),
    [Z_KANA(0x30d5 /* フ */)] = H_KANA(0xff8c /* ﾌ */),
    [Z_KANA(0x30d8 /* ヘ */)] = H_KANA(0xff8d /* ﾍ */),
    [Z_KANA(0x30db /* ホ */)] = H_KANA(0xff8e /* ﾎ */),
    [Z_KANA(0x30d0 /* バ */)] = H_KANA(0xff8a /* ﾊ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30d3 /* ビ */)] = H_KANA(0xff8b /* ﾋ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30d6 /* ブ */)] = H_KANA(0xff8c /* ﾌ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30d9 /* ベ */)] = H_KANA(0xff8d /* ﾍ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30dc /* ボ */)] = H_KANA(0xff8e /* ﾎ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30d1 /* パ */)] = H_KANA(0xff8a /* ﾊ */) | HANDAKUTEN_FLAG,
    [Z_KANA(0x30d4 /* ピ */)] = H_KANA(0xff8b /* ﾋ */) | HANDAKUTEN_FLAG,
    [Z_KANA(0x30d7 /* プ */)] = H_KANA(0xff8c /* ﾌ */) | HANDAKUTEN_FLAG,
    [Z_KANA(0x30da /* ペ */)] = H_KANA(0xff8d /* ﾍ */) | HANDAKUTEN_FLAG,
    [Z_KANA(0x30dd /* ポ */)] = H_KANA(0xff8e /* ﾎ */) | HANDAKUTEN_FLAG,
    [Z_KANA(0x30de /* マ */)] = H_KANA(0xff8f /* ﾏ */),
    [Z_KANA(0x30df /* ミ */)] = H_KANA(0xff90 /* ﾐ */),
    [Z_KANA(0x30e0 /* ム */)] = H_KANA(0xff91 /* ﾑ */),
    [Z_KANA(0x30e1 /* メ */)] = H_KANA(0xff92 /* ﾒ */),
    [Z_KANA(0x30e2 /* モ */)] = H_KANA(0xff93 /* ﾓ */),
    [Z_KANA(0x30e3 /* ャ */)] = H_KANA(0xff6c /* ｬ */),
    [Z_KANA(0x30e5 /* ュ */)] = H_KANA(0xff6d /* ｭ */),
    [Z_KANA(0x30e7 /* ョ */)] = H_KANA(0xff6e /* ｮ */),
    [Z_KANA(0x30e4 /* ヤ */)] = H_KANA(0xff94 /* ﾔ */),
    [Z_KANA(0x30e6 /* ユ */)] = H_KANA(0xff95 /* ﾕ */),
    [Z_KANA(0x30e8 /* ヨ */)] = H_KANA(0xff96 /* ﾖ */),
    [Z_KANA(0x30e9 /* ラ */)] = H_KANA(0xff97 /* ﾗ */),
    [Z_KANA(0x30ea /* リ */)] = H_KANA(0xff98 /* ﾘ */),
    [Z_KANA(0x30eb /* ル */)] = H_KANA(0xff99 /* ﾙ */),
    [Z_KANA(0x30ec /* レ */)] = H_KANA(0xff9a /* ﾚ */),
    [Z_KANA(0x30ed /* ロ */)] = H_KANA(0xff9b /* ﾛ */),
    [Z_KANA(0x30ef /* ワ */)] = H_KANA(0xff9c /* ﾜ */),
    [Z_KANA(0x30f2 /* ヲ */)] = H_KANA(0xff66 /* ｦ */),
    [Z_KANA(0x30f7 /* ヷ */)] = H_KANA(0xff9c /* ﾜ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30fa /* ヺ */)] = H_KANA(0xff66 /* ｦ */) | DAKUTEN_FLAG,
    [Z_KANA(0x30f3 /* ン */)] = H_KANA(0xff9d /* ﾝ */),
    [Z_KANA(0x30fb /* ・ */)] = H_KANA(0xff65 /* ･ */),
    [Z_KANA(0x30fc /* ー */)] = H_KANA(0xff70 /* ｰ */)
};

static inline bool
is_hankaku_kana(Py_UCS4 c) {
    return 0xff61 <= c && c <= 0xff9f;
}

static inline bool
is_in_kana_table_z(Py_UCS4 c) {
    return Z_KANA_OFF + 24 <= c && c <= Z_KANA_OFF + 127; 
}

static inline bool
is_reg_hiragana(Py_UCS4 c) {
    size_t x = (size_t)c - 0x3041;
    if (x <= (0x3096 - 0x3041)) {return true;}
    return (x & ~1) == (0x309d - 0x3041);
}
static inline bool
is_in_hiragana_range(Py_UCS4 c) {
    return 0x3041 <= c && c <= 0x309e;
}

static inline bool
is_convertible_to_hkana(Py_UCS4 c) {
    size_t x = (size_t)c - 0x3041;
    if (0x3094 - 0x3041 < x) {return false;}
    if (x <= 0x308d - 0x3041) {return true;}
    x = c - 0x308e;
    return (114 /* 0b1110010 */ >> x) & 1;
} 

static inline bool
is_reg_katakana(Py_UCS4 c) {
    size_t x = (size_t)c - 0x30a1;
    if (x <= (0x30f6 - 0x30a1)) {return true;}
    return (x & ~1) == (0x30fd - 0x30a1);
}
static inline bool
is_in_katakana_range(Py_UCS4 c) {
    return 0x30a1 <= c && c <= 0x30fe;
}

#define HIRAGANA_ID(c) ((ptrdiff_t)(c) - 0x3040)
#define KATAKANA_ID(c) ((ptrdiff_t)(c) - 0x30a0)

// Prevent excessive Common Subexpression Eliminations by modern compilers,
// which lead to cause unnecessary register spills.
#define SUPPRESS_CSE(flg) do {(flg) ^= 1 << UNUSED_BIT;} while (0)


/* Habachen Functions */

static int32_t
Habachen_parse_args(
    PyObject **text_p, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"ascii", "digit", "kana", NULL};

    Py_ssize_t nargs = PyTuple_GET_SIZE(args);
    if (!nargs) {
        PyErr_SetString(PyExc_TypeError,
            "function takes exactly 1 positional argument (0 given)");
        return -1;
    }
    PyObject *text = PyTuple_GET_ITEM(args, 0);
    *text_p = text;
    if (!PyUnicode_Check(text)) {
        PyErr_Format(PyExc_TypeError,
            "argument 1 must be str, not %.200s",
            text == Py_None ? "None" : Py_TYPE(text)->tp_name);
        return -1;
    }
    if (nargs > 1) {
        PyErr_Format(PyExc_TypeError,
            "function takes at most 1 positional argument "
            "(%zu given)", nargs);
        return -1;
    }
    BoolPred ascii = true, digit = true, kana = true;
    if (kwds) {
        args = PyTuple_New(0);
        if (!args) {return -1;}
        BoolPred status = PyArg_ParseTupleAndKeywords(
            args, kwds, "|$ppp", kwlist, &ascii, &digit, &kana);
        Py_DECREF(args);
        if (!status) {return -1;}
    }

    int32_t flags;
    flags = digit << DIGIT_BIT;
    flags |= ascii << ASCII_BIT;
    flags |= kana << KANA_BIT;
    return flags;
}


static PyObject*
Habachen_zenkaku_to_hankaku(
    PyObject *Py_UNUSED(_unused), PyObject *args, PyObject *kwds)
{
    PyObject *text;
    int32_t flags = Habachen_parse_args(&text, args, kwds);
    if (flags == -1) {return NULL;}
    if (PyUnicode_READY(text) == -1) {return NULL;}

    Py_ssize_t length = PyUnicode_GET_LENGTH(text);
    if (!flags || !length) {return PyUnicode_FromObject(text);}

    int kind = PyUnicode_KIND(text);
    if (kind == PyUnicode_1BYTE_KIND) {
        return PyUnicode_FromObject(text);
    }
#define WITH_ANY_KIND(pred) Habachen_Pred_with_KIND(kind, pred)

    Py_UCS4 maxchar = Habachen_MAX_CHAR_VALUE(text);
    size_t buf_size = flags & KANA_FLAG ? (size_t)length * 2 : (size_t)length;
    if (buf_size > PY_SSIZE_T_MAX) {return PyErr_NoMemory();}
    PyObject *result = PyUnicode_New((Py_ssize_t)buf_size, maxchar);
    if (!result) {return NULL;}
{ /* got ownership: result */
    void *source = PyUnicode_DATA(text);
    void *target = PyUnicode_DATA(result);

    Py_ssize_t i = 0, j = 0;
    maxchar = 0;
    Py_UCS4 c = Habachen_Unicode_READ(kind, source, i), tc;
    goto loop_enter;

loop_start:
    maxchar |= tc;
    Habachen_Unicode_WRITE(kind, target, j++, tc);
    if (WITH_ANY_KIND(++i >= length)) {goto loop_end;}
    c = Habachen_Unicode_READ(kind, source, i);

loop_enter:
    if (is_zenkaku_digit(c)) {
        tc = (flags & DIGIT_FLAG) ? ASCII_Z2H(c) : c;
        goto loop_start;
    }
    if (!(flags & (ASCII_FLAG | KANA_FLAG))) {
        tc = c;
        SUPPRESS_CSE(flags);
        goto loop_start;
    }
    if ((flags & ASCII_FLAG) && is_zenkaku_ascii_ex(c)) {
        tc = ascii_ex_z2h(c);
        goto loop_start;
    }
    if (is_special_char(c)) {
        tc = special_z2h(c, flags);
        goto loop_start;
    }
    tc = c;
    if ((flags & KANA_FLAG) && is_in_kana_table_z(c)) {
        c = kana_z2h_table[Z_KANA(c)];
        Py_UCS4 dots = c & (DAKUTEN_FLAG | HANDAKUTEN_FLAG);
        if (c) {
            tc = c + H_KANA_OFF;
            if (dots) {
                tc -= dots;
                Habachen_Unicode_WRITE(kind, target, j++, tc);
                tc = 0xff9e | (dots >> 7);
            }
        }
    }
    goto loop_start;

loop_end:
    length = PyUnicode_GET_LENGTH(result);
    if (maxchar <= 0xff) {
        PyObject *new;
        if (j == 1) {
            maxchar = Habachen_Unicode_READ(kind, target, 0);
            new = PyUnicode_FromOrdinal(maxchar);
            if (!new) {goto error;}
        } else {
            new = PyUnicode_New(j, maxchar);
            if (!new) {goto error;}
            if (PyUnicode_CopyCharacters(new, 0, result, 0, j) == -1) {
                Py_DECREF(new);
                goto error;
            }
        }
        Py_SETREF(result, new);
    } else if (j != length) {
        if (PyUnicode_Resize(&result, j) == -1) {goto error;}
        return result;
    }
    return result;

#undef WITH_ANY_KIND
}
error:
    Py_DECREF(result);
    return NULL;
}


static PyObject*
Habachen_hankaku_to_zenkaku(
    PyObject *Py_UNUSED(_unused), PyObject *args, PyObject *kwds)
{
    PyObject *text;
    int32_t flags = Habachen_parse_args(&text, args, kwds);
    if (flags == -1) {return NULL;}
    if (PyUnicode_READY(text) == -1) {return NULL;}

    Py_ssize_t length = PyUnicode_GET_LENGTH(text);
    if (!flags || !length) {return PyUnicode_FromObject(text);}

    Py_UCS4 maxchar = Habachen_MAX_CHAR_VALUE(text);
    PyObject *result = PyUnicode_New(length, Py_MAX(maxchar, 0xffff));
    if (!result) {return NULL;}
{ /* got ownership: result */
    DEF_TAGGED_UCS(source, text);
    DEF_TAGGED_UCS(target, result);
    Habachen_assert(UCSX_KIND(target) != PyUnicode_1BYTE_KIND);
    Py_ssize_t i = 0, j = 0;
    Py_UCS4 c = UCSX_READ(source, i), tc;
    goto loop_enter;

loop_start:
    UCSX_WRITE(target, j, tc); ++j;
    if (++i >= length) {goto loop_end;}
    c = UCSX_READ(source, i);

loop_enter:
    if ((flags & DIGIT_FLAG) && is_ascii_digit(c)) {
        tc = digit_h2z(c);
        maxchar |= tc;
        goto loop_start;
    }
    if ((flags & ASCII_FLAG) && is_any_ascii(c)) {
        tc = ascii_h2z(c);
        maxchar |= tc;
        goto loop_start;
    }
    Py_UCS4 nc = UCSX_READ(source, i+1);
    if ((flags & KANA_FLAG) && is_hankaku_kana(c)) {
        Py_ssize_t dots = \
            (Py_ssize_t)(nc - 0xff9e <= 1) << (nc & 7);
        if ((tc = kana_h2z_table[H_KANA(c) + dots]) != 0) {
            if (dots) {
                ++i;
                goto loop_start;
            }
            c = tc;
        } else {
            c = kana_h2z_table[H_KANA(c)];
            SUPPRESS_CSE(flags);
        }
    }
    UCSX_WRITE(target, j++, c);
    c = nc;
    if (++i < length) {goto loop_enter;}

loop_end:
    if (maxchar <= 0xff) {
        PyObject *new;
        if (j == 1) {
            c = UCSX_READ(target, 0);
            new = PyUnicode_FromOrdinal(c);
            if (!new) {goto error;}
        } else {
            new = PyUnicode_New(j, maxchar);
            if (!new) {goto error;}
            if (PyUnicode_CopyCharacters(new, 0, result, 0, j) == -1) {
                Py_DECREF(new);
                goto error;
            }
        }
        Py_SETREF(result, new);
    } else if (j != length) {
        if (PyUnicode_Resize(&result, j) == -1) {goto error;}
        return result;
    }
    return result;
}
error:
    Py_DECREF(result);
    return NULL;
}


typedef int (*ListFiller)(int, Py_ssize_t, void*, unsigned char *);
typedef Py_ssize_t (*KanaConverter)(
    int, Py_ssize_t, void*, void*, unsigned char*);

Py_NO_INLINE static int
Habachen_invalid_character_error(Py_UCS4 c) {
    PyObject *err_ch = PyUnicode_FromOrdinal(c);
    if (!err_ch) {return -1;}
    PyErr_Format(PyExc_ValueError,
        "unconvertible character in 'ignore' string: '%U'", err_ch);
    Py_DECREF(err_ch);
    return -1;
}


static int
Habachen_add_ignored_hiragana(
    int kind, Py_ssize_t length, void *data, unsigned char *arrlist)
{
    Py_ssize_t i = 0;
    Py_UCS4 c;
    if (kind == PyUnicode_1BYTE_KIND) {
        c = Habachen_Unicode_READ(kind, data, i);
        goto invalid;
    }
    do {
        c = Habachen_Unicode_READ(kind, data, i);
        if (!is_reg_hiragana(c)) {goto invalid;}
        arrlist[HIRAGANA_ID(c)-1] = 0x60;
    } while (++i < length);

    return 0;

invalid:
    return Habachen_invalid_character_error(c);
}

static int
Habachen_add_ignored_hiragana_hk(
    int kind, Py_ssize_t length, void *data, unsigned char *arrlist)
{
    Py_ssize_t i = 0;
    Py_UCS4 c;
    if (kind == PyUnicode_1BYTE_KIND) {
        c = Habachen_Unicode_READ(kind, data, i);
        goto invalid;
    }
    do {
        c = Habachen_Unicode_READ(kind, data, i);
        if (!is_convertible_to_hkana(c)) {goto invalid;}
        arrlist[HIRAGANA_ID(c)-1] = 0x60;
    } while (++i < length);

    return 0;

invalid:
    return Habachen_invalid_character_error(c);
}

static int
Habachen_add_ignored_katakana(
    int kind, Py_ssize_t length, void *data, unsigned char *arrlist)
{
    Py_ssize_t i = 0;
    Py_UCS4 c;
    if (kind == PyUnicode_1BYTE_KIND) {
        c = Habachen_Unicode_READ(kind, data, i);
        goto invalid;
    }
    do {
        c = Habachen_Unicode_READ(kind, data, i);
        if (!is_reg_katakana(c)) {goto invalid;}
        arrlist[KATAKANA_ID(c)-1] = 0x60;
    } while (++i < length);

    return 0;

invalid:
    return Habachen_invalid_character_error(c);
}


static const char *ignore_err_msg = \
    "'ignore' option must be a string or an iterable of strings";

Py_NO_INLINE static int
Habachen_build_ignore_list(
    PyStrOrSeqObject *obj, unsigned char *arrlist, ListFiller list_filler)
{
    PyObject *seq = (PyObject *)obj;
    Habachen_assert(seq != NULL);
    if (PyUnicode_Check(seq)) {
        if (PyUnicode_READY(seq) == -1) {return -1;}

        Py_ssize_t length = PyUnicode_GET_LENGTH(seq);
        if (!length) {return 0;}
        int status = list_filler(
            PyUnicode_KIND(seq), length, PyUnicode_DATA(seq), arrlist);
        return (status == -1) ? -1 : 1;
    }

    assert(PyList_CheckExact(seq) || PyTuple_CheckExact(seq));

    int result;
    // Make sure the sequence won't be mutated during iteration.
    Py_BEGIN_CRITICAL_SECTION(seq);
    Py_ssize_t seqlen = PySequence_Fast_GET_SIZE(seq);
    PyObject **items = PySequence_Fast_ITEMS(seq);
    result = 0;
    for (Py_ssize_t i = 0; i < seqlen;) {
        PyObject *u = items[i]; ++i;
        if (!PyUnicode_Check(u)) {
            PyErr_SetString(PyExc_TypeError, ignore_err_msg);
            result = -1;
            break;
        }
        if (PyUnicode_READY(u) == -1) {
            result = -1;
            break;
        }

        Py_ssize_t length = PyUnicode_GET_LENGTH(u);
        if (!length) {continue;}
        int status = list_filler(
            PyUnicode_KIND(u), length, PyUnicode_DATA(u), arrlist);
        if (status == -1) {
            result = -1;
            break;
        }
        result = 1;
    }
    Py_END_CRITICAL_SECTION();
    return result;
}


static inline Py_ssize_t
Habachen_hr2hkt_converter(
    int kind, Py_ssize_t length,
    void *source, void *target, unsigned char *exlist)
{
    Py_ssize_t i = 0, j = 0;
    Py_UCS4 c, tc;

loop_start:
    c = Habachen_Unicode_READ(kind, source, i); ++i;
    if (!exlist ? is_reg_hiragana(c) : is_in_hiragana_range(c)) {
        tc = !exlist ? \
            kana_z2h_table[Z_KANA(c) + 0x60] : !exlist[HIRAGANA_ID(c)-1] ? \
                kana_z2h_table[Z_KANA(c) + 0x60] : 0;
        Py_UCS4 dots = tc & (DAKUTEN_FLAG | HANDAKUTEN_FLAG);
        if (tc) {
            c = tc + H_KANA_OFF;
            if (dots) {
                c -= dots;
                Habachen_Unicode_WRITE(kind, target, j++, c);
                c = 0xff9e | (dots >> 7);
            }
        }
    }
    Habachen_Unicode_WRITE(kind, target, j++, c);
    if (i < length) {goto loop_start;}
    return j;
}

#define CHECK_CONVERT(c, tval, fval, off) ( \
    (off) + 92u <= (c) && (c) <= (off) + 93u ? \
        (tval) : !((off) + 0u <= (c) && (c) <= (off) + 85u) ? (fval) : (tval))

enum {HIRAGANA_OFF = 0x3041, KATAKANA_OFF = 0x30a1};

static inline Py_ssize_t
Habachen_hr2kt_converter(
    int kind, Py_ssize_t length,
    void *source, void *target, unsigned char *exlist)
{
    Py_ssize_t i = 0;
    Py_UCS4 c, d;

loop_start:
    c = Habachen_Unicode_READ(kind, source, i); ++i;
    if (!exlist) {
#if defined(__GNUC__)
        d = 0x60;
        c += CHECK_CONVERT(c, d, 0, HIRAGANA_OFF);
#else
        d = c + 0x60;
        c = CHECK_CONVERT(c, d, c, HIRAGANA_OFF);
#endif
    } else if (is_in_hiragana_range(c)) {
        c -= exlist[HIRAGANA_ID(c)-1];
        c += 0x60;
    }
    Habachen_Unicode_WRITE(kind, target, i-1, c);
    if (i < length) {goto loop_start;}
    return i;
}

static inline Py_ssize_t
Habachen_kt2hr_converter(
    int kind, Py_ssize_t length,
    void *source, void *target, unsigned char *exlist)
{
    Py_ssize_t i = 0;
    Py_UCS4 c;

loop_start:
    c = Habachen_Unicode_READ(kind, source, i); ++i;
#if defined(__GNUC__)
    if (!exlist) {
        c -= CHECK_CONVERT(c, 0x60, 0, KATAKANA_OFF);
    } else if (is_in_katakana_range(c)) {
        c += exlist[KATAKANA_ID(c)-1];
        c -= 0x60;
    }
#else
    if (!exlist ? is_reg_katakana(c) : is_in_katakana_range(c)) {
        c += !exlist ? 0 : exlist[KATAKANA_ID(c)-1];
        c -= 0x60;
    }
#endif
    Habachen_Unicode_WRITE(kind, target, i-1, c);
    if (i < length) {goto loop_start;}
    return i;
}


static inline int
Habachen_str_or_seq(
    PyStrOrSeqObject **s, PyObject *obj, const char *err_msg)
{
    if (PyUnicode_Check(obj)) {
        *s = (PyStrOrSeqObject *)Py_NewRef(obj);
        return 0;
    }
    *s = (PyStrOrSeqObject *)PySequence_Fast(obj, err_msg);
    return *s ? 0 : -1;
}


static PyObject *
Habachen_hira_to_hkata_impl(
    PyObject *text, unsigned char *exlist)
{
    const KanaConverter converter = Habachen_hr2hkt_converter;

    if (PyUnicode_CheckExact(text)) {
        if (PyUnicode_READY(text) == -1) {return NULL;}
        Py_INCREF(text);
    } else {
        text = PyUnicode_FromObject(text);
        if (!text) {return NULL;}
    }
{ /* got ownership: text? */
    PyObject *result;
    Py_ssize_t length = PyUnicode_GET_LENGTH(text);
    if (!length) {
        result = text;
        goto finish;
    }

    int kind = PyUnicode_KIND(text);
    if (kind == PyUnicode_1BYTE_KIND) {
        result = text;
        goto finish;
    }
#define WITH_ANY_KIND(pred) Habachen_Pred_with_KIND(kind, pred)

    Py_UCS4 maxchar = Habachen_MAX_CHAR_VALUE(text);
    size_t buf_size = (size_t)length * 2;
    if (buf_size > PY_SSIZE_T_MAX) {
        PyErr_NoMemory();
        goto error;
    }
    result = PyUnicode_New((Py_ssize_t)buf_size, maxchar);
    if (!result) {goto error;}

    void *source = PyUnicode_DATA(text);
    void *target = PyUnicode_DATA(result);

    Py_ssize_t size;
    if (exlist) {
        WITH_ANY_KIND(
            size = converter(kind, length, source, target, exlist));
    } else {
        WITH_ANY_KIND(
            size = converter(kind, length, source, target, NULL));
    }
    length = PyUnicode_GET_LENGTH(result);
    Py_CLEAR(text);
    if (size != length) {
        if (PyUnicode_Resize(&result, size) == -1) {goto error;}
    }

finish:
    return result;

#undef WITH_ANY_KIND
}
error:
    Py_XDECREF(text);
    return NULL;
}


static PyObject *
Habachen_hira_to_kata_impl(
    PyObject *text, unsigned char *exlist)
{
    const KanaConverter converter = Habachen_hr2kt_converter;

    if (PyUnicode_CheckExact(text)) {
        if (PyUnicode_READY(text) == -1) {return NULL;}
        Py_INCREF(text);
    } else {
        text = PyUnicode_FromObject(text);
        if (!text) {return NULL;}
    }
{ /* got ownership: text */
    PyObject *result;
    Py_ssize_t length = PyUnicode_GET_LENGTH(text);
    if (!length) {
        result = text;
        goto finish;
    }

    int kind = PyUnicode_KIND(text);
    if (kind == PyUnicode_1BYTE_KIND) {
        result = text;
        goto finish;
    }
#define WITH_ANY_KIND(pred) Habachen_Pred_with_KIND(kind, pred)

    Py_UCS4 maxchar = Habachen_MAX_CHAR_VALUE(text);
    result = PyUnicode_New(length, maxchar);
    if (!result) {goto error;}

    void *source = PyUnicode_DATA(text);
    void *target = PyUnicode_DATA(result);

    if (exlist) {
        WITH_ANY_KIND(
            converter(kind, length, source, target, exlist));
    } else {
        WITH_ANY_KIND(
            converter(kind, length, source, target, NULL));
    }
    Py_DECREF(text);

finish:
    return result;

#undef WITH_ANY_KIND
}
error:
    Py_DECREF(text);
    return NULL;
}


enum {EXLIST_SIZE = 0x60};

static NO_CANARY PyObject *
Habachen_hiragana_to_katakana(
    PyObject *Py_UNUSED(_unused), PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"", "ignore", "hankaku", NULL};

    PyObject *text;
    unsigned char *exlist = NULL;
    Py_ssize_t nargs = PyTuple_GET_SIZE(args);
    if (!kwds && nargs == 1) {
        text = PyTuple_GET_ITEM(args, 0);
        if (!PyUnicode_Check(text)) {
            PyErr_Format(PyExc_TypeError,
                "argument 1 must be str, not %.200s",
                text == Py_None ? "None" : Py_TYPE(text)->tp_name);
            return NULL;
        }
    } else {
        PyObject *ignore = NULL;
        BoolPred hankaku = false;
        if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "U|O$p", kwlist, &text, &ignore, &hankaku)
        ) {return NULL;}
        if (ignore) {
            PyStrOrSeqObject *strs;
            if (Habachen_str_or_seq(&strs, ignore, ignore_err_msg) < 0) {
                return NULL;
            }
            exlist = ALLOCA(EXLIST_SIZE);
            memset(exlist, 0, EXLIST_SIZE);
            int status = Habachen_build_ignore_list(
                strs, exlist, hankaku ? Habachen_add_ignored_hiragana_hk
                : Habachen_add_ignored_hiragana);
            Py_CLEAR(strs);
            if (status < 0) {return NULL;}
            if (status == 0) {
                exlist = NULL;
            } else {
                memset(exlist+0x56, 0x60, 6);
            }
        }
        if (hankaku) {
            return Habachen_hira_to_hkata_impl(text, exlist);
        }
    }
    return Habachen_hira_to_kata_impl(text, exlist);
}


static PyObject *
Habachen_kata_to_hira_impl(
    PyObject *text, unsigned char *exlist)
{
    const KanaConverter converter = Habachen_kt2hr_converter;

    if (PyUnicode_CheckExact(text)) {
        if (PyUnicode_READY(text) == -1) {return NULL;}
        Py_INCREF(text);
    } else {
        text = PyUnicode_FromObject(text);
        if (!text) {return NULL;}
    }
{ /* got ownership: text */
    PyObject *result;
    Py_ssize_t length = PyUnicode_GET_LENGTH(text);
    if (!length) {
        result = text;
        goto finish;
    }

    int kind = PyUnicode_KIND(text);
    if (kind == PyUnicode_1BYTE_KIND) {
        result = text;
        goto finish;
    }
#define WITH_ANY_KIND(pred) Habachen_Pred_with_KIND(kind, pred)

    Py_UCS4 maxchar = Habachen_MAX_CHAR_VALUE(text);
    result = PyUnicode_New(length, maxchar);
    if (!result) {goto error;}

    void *source = PyUnicode_DATA(text);
    void *target = PyUnicode_DATA(result);

    if (exlist) {
        WITH_ANY_KIND(
            converter(kind, length, source, target, exlist));
    } else {
        WITH_ANY_KIND(
            converter(kind, length, source, target, NULL));
    }
    Py_DECREF(text);

finish:
    return result;

#undef WITH_ANY_KIND
}
error:
    Py_DECREF(text);
    return NULL;
}


static NO_CANARY PyObject *
Habachen_katakana_to_hiragana(
    PyObject *Py_UNUSED(_unused), PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"", "ignore", NULL};

    PyObject *text;
    unsigned char *exlist = NULL;
    Py_ssize_t nargs = PyTuple_GET_SIZE(args);
    if (!kwds && nargs == 1) {
        text = PyTuple_GET_ITEM(args, 0);
        if (!PyUnicode_Check(text)) {
            PyErr_Format(PyExc_TypeError,
                "argument 1 must be str, not %.200s",
                text == Py_None ? "None" : Py_TYPE(text)->tp_name);
            return NULL;
        }
    } else {
        PyObject *ignore = NULL;
        if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "U|O", kwlist, &text, &ignore)
        ) {return NULL;}
        if (ignore) {
            PyStrOrSeqObject *strs;
            if (Habachen_str_or_seq(&strs, ignore, ignore_err_msg) < 0) {
                return NULL;
            }
            exlist = ALLOCA(EXLIST_SIZE);
            memset(exlist, 0, EXLIST_SIZE);
            memset(exlist+0x56, 0x60, 6);
            int status = Habachen_build_ignore_list(
                strs, exlist, Habachen_add_ignored_katakana);
            Py_CLEAR(strs);
            if (status < 0) {return NULL;}
            if (status == 0) {exlist = NULL;}
        }
    }
    return Habachen_kata_to_hira_impl(text, exlist);
}


static PyMethodDef habachen_methods[] = {
    {"hankaku_to_zenkaku", (PyCFunction)Habachen_hankaku_to_zenkaku,
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR(
     "hankaku_to_zenkaku(text, /, *, ascii=True, digit=True, kana=True)\n"
     "--\n\n"
     "Convert hankaku characters to their corresponding zenkaku ones.")},
    {"zenkaku_to_hankaku", (PyCFunction)Habachen_zenkaku_to_hankaku,
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR(
     "zenkaku_to_hankaku(text, /, *, ascii=True, digit=True, kana=True)\n"
     "--\n\n"
     "Convert zenkaku characters to their corresponding hankaku ones.")},
    {"hiragana_to_katakana", (PyCFunction)Habachen_hiragana_to_katakana,
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR(
     "hiragana_to_katakana(text, /, ignore='', *, hankaku=False)\n"
     "--\n\n"
     "Convert hiragana to katakana.")},
    {"katakana_to_hiragana", (PyCFunction)Habachen_katakana_to_hiragana,
     METH_VARARGS | METH_KEYWORDS, PyDoc_STR(
     "katakana_to_hiragana(text, /, ignore='')\n"
     "--\n\n"
     "Convert full-width katakana to hiragana.")},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef habachenmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_habachen",
    .m_size = -1,
    .m_methods = habachen_methods
};


PyMODINIT_FUNC
PyInit__habachen(void) {
#ifdef Py_GIL_DISABLED
    PyObject *m = PyModule_Create(&habachenmodule);
    if (!m) {return NULL;}
    PyUnstable_Module_SetGIL(m, Py_MOD_GIL_NOT_USED);
    return m;
#else
    return PyModule_Create(&habachenmodule);
#endif
}
