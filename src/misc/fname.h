/**
 *  @file   fname.h
 *  @brief  ファイル名の処理.
 *  @author Masashi Kitamura
 *  @note
 *  -   c/c++ 両用.
 */
#ifndef FKS_FNAME_H_INCLUDED
#define FKS_FNAME_H_INCLUDED

//#include "../ara_config.h"

//#ifdef __cplusplus
//#include <cstring>
//#else
#include <string.h>
//#endif


// ============================================================================
// マクロ関係.

// Win以外(unix系)で ファイル名中の全角の \ 対策をしたい場合は定義.
// これを定義すると環境変数 LANG をみて SJIS,gbk,gb2312,big5ならMBC処理をする.
//#define FKS_USE_FNAME_MBC

#if defined _WIN32 || defined _DOS || defined _MSDOS
#define FKS_FNAME_WINDOS
#endif

// ファイル名のサイズ. 利用側のバッファ準備用.
// 元々はパス全体の制限だったが現状 1ファイル名の長さになっている感じ.
// ※ win-api 自体は基本的にパス全体でこの制限を受ける.
// fname.cpp では、fname_relativePath?? のみがこの値を使う. (他は参照しない)
#ifndef FKS_FNAME_MAX_PATH
 #ifdef _WIN32
  #define FKS_FNAME_MAX_PATH    260/*_MAX_PATH*/        ///< 通常のパス名の長さ.※winnt系の場合1ファイル名の長さ.
 #else
  #define FKS_FNAME_MAX_PATH    1024                    ///< 通常のパス名の長さ.
 #endif
#endif

// 一応url向けだが場合によってはこれがパスの最大長かも. winの場合 最長約0x8000 . それ以外は適当.
// (※ winでは特殊な指定をしないと_PAX_PATHを超えては使えないけれど)
#ifndef FKS_FNAME_MAX_URL
 #ifdef _WIN32
  #define FKS_FNAME_MAX_URL     (0x8000)                    ///< urlとして扱う場合のパス名サイズ.
 #else  // 適当に計算.
  #define FKS_FNAME_MAX_URL     (6U*4*FKS_FNAME_MAX_PATH)   ///< urlとして扱う場合のパス名サイズ.
 #endif
#endif

#if defined FKS_FNAME_WINDOS
#define FKS_FNAME_SEP_CHR       '\\'
#define FKS_FNAME_SEP_STR       "\\"
#define FKS_FNAME_SEP_WCS       L"\\"
#define FKS_FNAME_SEP_TCS       _T("\\")
#else
#define FKS_FNAME_SEP_CHR       '/'
#define FKS_FNAME_SEP_STR       "/"
#define FKS_FNAME_SEP_WCS       L"/"
#define FKS_FNAME_SEP_TCS       "/"
#endif

#ifdef __cplusplus
#define FKS_FNAME_const             // c++の場合は 基本は非constで、const,非const２種類作る.
#else   // c のとき.
#define FKS_FNAME_const     const   // Cの場合で 引数はconst, 返り値は非const にする場合に使う.
#endif



// ============================================================================
// char版

#ifdef __cplusplus
namespace fks {
#endif

enum {
    FNAME_MAX_PATH  = FKS_FNAME_MAX_PATH,
    FNAME_MAX_URL   = FKS_FNAME_MAX_URL,
};

#if defined FKS_FNAME_WINDOS
inline int      fname_isSep(unsigned c) { return c == '\\' || c == '/'; }
#else
inline int      fname_isSep(unsigned c) { return c == '/'; }
#endif

inline size_t   fname_len(const char* path) { return /*FKS_STD*/ strlen(path); }

int     fname_isAbs(const char* path);                                      ///< 絶対パスか否か(ドライブ名の有無は関係なし)
int     fname_hasDrive(const char* path);                                   ///< ドライブ名がついているか. (file: や http:もドライブ扱い)

unsigned fname_adjust_size(const char* str, unsigned size);                 ///< (なるべく文字を壊さないで)size文字以内の文字数を返す.
char*   fname_cpy(char dst[], unsigned sz, const char* src);                ///< ファイル名のコピー.
char*   fname_cat(char dst[], unsigned sz, const char* src);                ///< ファイル名文字列の連結.

char*   fname_baseName(FKS_FNAME_const char *adr);                          ///< ファイルパス名中のディレクトリを除いたファイル名の位置を返す.
char*   fname_ext(FKS_FNAME_const char *name);                              ///< 拡張子の位置を返す.
char*   fname_skipDrive(FKS_FNAME_const char *name);                        ///< ドライブ名をスキップした位置を返す.
char*   fname_skipDriveRoot(FKS_FNAME_const char* name);                    ///< ドライブ名とルート指定部分をスキップした位置を返す.

char*   fname_delExt(char name[]);                                          ///< 拡張子を削除する.
char*   fname_getNoExt(char dst[], unsigned sz, const char *src);           ///< 拡張子を外した名前を取得.
char*   fname_getBaseNameNoExt(char d[],unsigned sz,const char *s);         ///< ディレクトリと拡張子を外した名前を取得.

char*   fname_setExt(char dst[], unsigned sz, const char* src, const char *ext);        ///< 拡張子を、ext に変更する.
char*   fname_setDefaultExt(char dst[], unsigned sz, const char* src, const char *ext); ///< 拡張子がなければ、ext を追加する.
char*   fname_join(char dst[],unsigned sz,const char *dir,const char *nm);              ///< ディレクトリ名とファイル名の連結.

char*   fname_getDir(char dir[], unsigned sz, const char *nm);              ///< ディレクトリ名の取得.
char*   fname_getDrive(char drv[], unsigned sz, const char *nm);            ///< ドライブ名を取得.
char*   fname_getDriveRoot(char dr[],unsigned sz,const char *nm);           ///< ドライブ名を取得.

char*   fname_checkPosSep(FKS_FNAME_const char* dir, int pos);              ///< posの位置に\か/があればそのアドレスをなければNULLを返す.
char*   fname_checkLastSep(FKS_FNAME_const char* dir);                      ///< 最後に\か/があればそのアドレスをなければNULLを返す.
char*   fname_delLastSep(char dir[]);                                       ///< 文字列の最後に \ か / があれば削除.
char*   fname_addSep(char dst[], unsigned sz);                              ///< 文字列の最後に \ / がなければ追加.

char*   fname_toUpper(char filename[]);                                     ///< 全角２バイト目を考慮した strupr.
char*   fname_toLower(char filename[]);                                     ///< 全角２バイト目を考慮した strlwr.
char*   fname_backslashToSlash(char filePath[]);                            ///< filePath中の \ を / に置換.
char*   fname_slashToBackslash(char filePath[]);                            ///< filePath中の / を \ に置換.

char*   fname_fullpath  (char fullpath[], unsigned sz, const char* path, const char* curDir);       ///< フルパス生成. os依存.
char*   fname_fullpathSL(char fullpath[], unsigned sz, const char* path, const char* curDir);       ///< フルパス生成. / 区切.
char*   fname_fullpathBS(char fullpath[], unsigned sz, const char* path, const char* curDir);       ///< フルパス生成. \ 区切.
char*   fname_relativePath  (char relPath[], unsigned sz, const char* path, const char* curDir);    ///< 相対パス生成. os依存.
char*   fname_relativePathSL(char relPath[], unsigned sz, const char* path, const char* curDir);    ///< 相対パス生成. / 区切.
char*   fname_relativePathBS(char relPath[], unsigned sz, const char* path, const char* curDir);    ///< 相対パス生成. \ 区切.

int     fname_cmp(const char* l, const char* r);                            ///< ファイル名の大小比較.
int     fname_n_cmp(const char* l, const char* r, unsigned len);            ///< ファイル名のn文字大小比較.
int     fname_digit_cmp(const char* l, const char* r, unsigned len);        ///< 桁違いの数字を数値としてファイル名比較.
char*   fname_equLong(FKS_FNAME_const char* fname, const char* baseName);   ///< fnameがbaseNameで始まっているか否か.
int     fname_matchWildCard(const char* pattern, const char* str);          ///< ワイルドカード文字(*?)列比較. マッチしたら真.

/// コマンドライン引数や、;区切りの複数のパス指定から、１要素取得.
char*   fname_scanArgStr(char arg[],unsigned sz,const char *str, unsigned sepChr);

#ifdef __cplusplus  // 引数対策.
static inline const char*   fname_baseName(const char *p)                               { return fname_baseName((char*)p); }
static inline const char*   fname_ext(const char *name)                                 { return fname_ext((char*)name); }
static inline const char*   fname_skipDrive(const char *name)                           { return fname_skipDrive((char*)name); }
static inline const char*   fname_skipDriveRoot(const char *name)                       { return fname_skipDriveRoot((char*)name); }
static inline const char*   fname_checkPosSep(const char* dir, int pos)                 { return fname_checkPosSep((char*)dir,pos); }
static inline const char*   fname_checkLastSep(const char* dir)                         { return fname_checkLastSep((char*)dir); }
static inline const char*   fname_equLong(const char* fname, const char* baseName)      { return fname_equLong((char*)fname, baseName); }

static inline char*         fname_scanArgStr(char arg[],unsigned sz,const char *str)    { return fname_scanArgStr(arg,sz,str, ' '); }
//static inline char*       fname_setExt(char name[], unsigned sz, const char *ext)     { return fname_setExt(name, sz, name, ext); }
//static inline char*       fname_setDefaultExt(char name[],unsigned sz,const char *ext){ return fname_setDefaultExt(name, sz, name, ext); }
#endif

#ifdef __cplusplus
}   // fks
#endif



// ============================================================================
// wchar_t* 版

#ifdef _WIN32

#ifdef __cplusplus
#include <wchar.h>

namespace fks {

inline size_t fname_len(const wchar_t* path) { return /*FKS_STD*/ wcslen(path); }

int         fname_isAbs(const wchar_t* path);                                           ///< 絶対パスか否か(ドライブ名の有無は関係なし)
int         fname_hasDrive(const wchar_t* path);                                        ///< ドライブ名がついているか. (file: や http:もドライブ扱い)

unsigned    fname_adjust_size(const wchar_t* str, unsigned size);                       ///< (なるべく文字を壊さないで)size文字以内の文字数を返す.
wchar_t*    fname_cpy(wchar_t dst[], unsigned sz, const wchar_t* src);                  ///< ファイル名のコピー.
wchar_t*    fname_cat(wchar_t dst[], unsigned sz, const wchar_t* src);                  ///< ファイル名文字列の連結.

wchar_t*    fname_baseName(FKS_FNAME_const wchar_t *adr);                               ///< ファイルパス名中のディレクトリを除いたファイル名の位置を返す.
wchar_t*    fname_ext(FKS_FNAME_const wchar_t *name);                                   ///< 拡張子の位置を返す.
wchar_t*    fname_skipDrive(FKS_FNAME_const wchar_t *name);                             ///< ドライブ名をスキップした位置を返す.
wchar_t*    fname_skipDriveRoot(FKS_FNAME_const wchar_t* name);                         ///< ドライブ名とルート指定部分をスキップした位置を返す.

wchar_t*    fname_delExt(wchar_t name[]);                                               ///< 拡張子を削除する.
wchar_t*    fname_getNoExt(wchar_t dst[], unsigned sz, const wchar_t *src);             ///< 拡張子を外した名前を取得.
wchar_t*    fname_getBaseNameNoExt(wchar_t d[],unsigned sz,const wchar_t *s);           ///< ディレクトリと拡張子を外した名前を取得.

wchar_t*    fname_setExt(wchar_t dst[], unsigned sz, const wchar_t* src, const wchar_t *ext);       ///< 拡張子を、ext に変更する.
wchar_t*    fname_setDefaultExt(wchar_t dst[],unsigned sz,const wchar_t* src,const wchar_t *ext);   ///< 拡張子がなければ、ext を追加する.
wchar_t*    fname_join(wchar_t dst[],unsigned sz,const wchar_t *dir,const wchar_t *nm);             ///< ディレクトリ名とファイル名の連結.

wchar_t*    fname_getDir(wchar_t dir[], unsigned sz, const wchar_t *nm);                ///< ディレクトリ名の取得.
wchar_t*    fname_getDrive(wchar_t drv[], unsigned sz, const wchar_t *nm);              ///< ドライブ名を取得.
wchar_t*    fname_getDriveRoot(wchar_t dr[],unsigned sz,const wchar_t *nm);             ///< ドライブ名を取得.

wchar_t*    fname_checkLastSep(FKS_FNAME_const wchar_t* dir);                           ///< 最後に\か/があればそのアドレスをなければNULLを返す.
wchar_t*    fname_delLastSep(wchar_t dir[]);                                            ///< 文字列の最後に \ か / があれば削除.
wchar_t*    fname_addSep(wchar_t dst[], unsigned sz);                                   ///< 文字列の最後に \ / がなければ追加.

wchar_t*    fname_toUpper(wchar_t filename[]);                                          ///< 全角２バイト目を考慮した strupr.
wchar_t*    fname_toLower(wchar_t filename[]);                                          ///< 全角２バイト目を考慮した strlwr.
wchar_t*    fname_backslashToSlash(wchar_t filePath[]);                                 ///< filePath中の \ を / に置換.
wchar_t*    fname_slashToBackslash(wchar_t filePath[]);                                 ///< filePath中の / を \ に置換.

wchar_t*    fname_fullpath  (wchar_t fullpath[], unsigned sz, const wchar_t* path, const wchar_t* curDir);      ///< フルパス生成. os依存.
wchar_t*    fname_fullpathSL(wchar_t fullpath[], unsigned sz, const wchar_t* path, const wchar_t* curDir);      ///< フルパス生成. / 区切.
wchar_t*    fname_fullpathBS(wchar_t fullpath[], unsigned sz, const wchar_t* path, const wchar_t* curDir);      ///< フルパス生成. \ 区切.
wchar_t*    fname_relativePath  (wchar_t relPath[], unsigned sz, const wchar_t* path, const wchar_t* curDir);   ///< 相対パス生成. os依存.
wchar_t*    fname_relativePathSL(wchar_t relPath[], unsigned sz, const wchar_t* path, const wchar_t* curDir);   ///< 相対パス生成. / 区切.
wchar_t*    fname_relativePathBS(wchar_t relPath[], unsigned sz, const wchar_t* path, const wchar_t* curDir);   ///< 相対パス生成. \ 区切.

int         fname_cmp(const wchar_t* l, const wchar_t* r);                              ///< ファイル名の大小比較.
int         fname_n_cmp(const wchar_t* l, const wchar_t* r, unsigned len);              ///< ファイル名のn文字大小比較.
int         fname_digit_cmp(const wchar_t* l, const wchar_t* r, unsigned len);          ///< 桁違いの数字を数値としてファイル名比較.
wchar_t*    fname_equLong(FKS_FNAME_const wchar_t* fname, const wchar_t* baseName);     ///< fnameがbaseNameで始まっているか否か.
int         fname_matchWildCard(const wchar_t* pattern, const wchar_t* str);            ///< ワイルドカード文字(*?)列比較. マッチしたら真.

/// コマンドライン引数や、;区切りの複数のパス指定から、１要素取得.
wchar_t*    fname_scanArgStr(wchar_t arg[],unsigned sz,const wchar_t *str, unsigned sepChr);

static inline const wchar_t* fname_baseName(const wchar_t *p)                                   { return fname_baseName((wchar_t*)p); }
static inline const wchar_t* fname_ext(const wchar_t *name)                                     { return fname_ext((wchar_t*)name); }
static inline const wchar_t* fname_skipDrive(const wchar_t *name)                               { return fname_skipDrive((wchar_t*)name); }
static inline const wchar_t* fname_skipDriveRoot(const wchar_t *name)                           { return fname_skipDriveRoot((wchar_t*)name); }
static inline const wchar_t* fname_checkLastSep(const wchar_t* dir)                             { return fname_checkLastSep((wchar_t*)dir); }
static inline const wchar_t* fname_equLong(const wchar_t* fname, const wchar_t* baseName)       { return fname_equLong((wchar_t*)fname, baseName); }

static inline wchar_t*       fname_scanArgStr(wchar_t arg[],unsigned sz,const wchar_t *str)     { return fname_scanArgStr(arg,sz,str,L' '); }
//static inline wchar_t*     fname_setExt(wchar_t name[], unsigned sz, const wchar_t *ext)      { return fname_setExt(name, sz, name, ext); }
//static inline wchar_t*     fname_setDefaultExt(wchar_t name[],unsigned sz,const wchar_t *ext) { return fname_setDefaultExt(name, sz, name, ext); }

#endif  // _WIN32

#undef FKS_FNAME_const

}   // fks
#endif

#endif      // FKS_FNAME_H_INCLUDED.
