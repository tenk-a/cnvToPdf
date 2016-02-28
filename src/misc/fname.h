/**
 *  @file   fname.h
 *  @brief  �t�@�C�����̏���.
 *  @author Masashi Kitamura
 *  @note
 *  -   c/c++ ���p.
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
// �}�N���֌W.

// Win�ȊO(unix�n)�� �t�@�C�������̑S�p�� \ �΍���������ꍇ�͒�`.
// ������`����Ɗ��ϐ� LANG ���݂� SJIS,gbk,gb2312,big5�Ȃ�MBC����������.
//#define FKS_USE_FNAME_MBC

#if defined _WIN32 || defined _DOS || defined _MSDOS
#define FKS_FNAME_WINDOS
#endif

// �t�@�C�����̃T�C�Y. ���p���̃o�b�t�@�����p.
// ���X�̓p�X�S�̂̐��������������� 1�t�@�C�����̒����ɂȂ��Ă��銴��.
// �� win-api ���̂͊�{�I�Ƀp�X�S�̂ł��̐������󂯂�.
// fname.cpp �ł́Afname_relativePath?? �݂̂����̒l���g��. (���͎Q�Ƃ��Ȃ�)
#ifndef FKS_FNAME_MAX_PATH
 #ifdef _WIN32
  #define FKS_FNAME_MAX_PATH    260/*_MAX_PATH*/        ///< �ʏ�̃p�X���̒���.��winnt�n�̏ꍇ1�t�@�C�����̒���.
 #else
  #define FKS_FNAME_MAX_PATH    1024                    ///< �ʏ�̃p�X���̒���.
 #endif
#endif

// �ꉞurl���������ꍇ�ɂ���Ă͂��ꂪ�p�X�̍ő咷����. win�̏ꍇ �Œ���0x8000 . ����ȊO�͓K��.
// (�� win�ł͓���Ȏw������Ȃ���_PAX_PATH�𒴂��Ă͎g���Ȃ������)
#ifndef FKS_FNAME_MAX_URL
 #ifdef _WIN32
  #define FKS_FNAME_MAX_URL     (0x8000)                    ///< url�Ƃ��Ĉ����ꍇ�̃p�X���T�C�Y.
 #else  // �K���Ɍv�Z.
  #define FKS_FNAME_MAX_URL     (6U*4*FKS_FNAME_MAX_PATH)   ///< url�Ƃ��Ĉ����ꍇ�̃p�X���T�C�Y.
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
#define FKS_FNAME_const             // c++�̏ꍇ�� ��{�͔�const�ŁAconst,��const�Q��ލ��.
#else   // c �̂Ƃ�.
#define FKS_FNAME_const     const   // C�̏ꍇ�� ������const, �Ԃ�l�͔�const �ɂ���ꍇ�Ɏg��.
#endif



// ============================================================================
// char��

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

int     fname_isAbs(const char* path);                                      ///< ��΃p�X���ۂ�(�h���C�u���̗L���͊֌W�Ȃ�)
int     fname_hasDrive(const char* path);                                   ///< �h���C�u�������Ă��邩. (file: �� http:���h���C�u����)

unsigned fname_adjust_size(const char* str, unsigned size);                 ///< (�Ȃ�ׂ��������󂳂Ȃ���)size�����ȓ��̕�������Ԃ�.
char*   fname_cpy(char dst[], unsigned sz, const char* src);                ///< �t�@�C�����̃R�s�[.
char*   fname_cat(char dst[], unsigned sz, const char* src);                ///< �t�@�C����������̘A��.

char*   fname_baseName(FKS_FNAME_const char *adr);                          ///< �t�@�C���p�X�����̃f�B���N�g�����������t�@�C�����̈ʒu��Ԃ�.
char*   fname_ext(FKS_FNAME_const char *name);                              ///< �g���q�̈ʒu��Ԃ�.
char*   fname_skipDrive(FKS_FNAME_const char *name);                        ///< �h���C�u�����X�L�b�v�����ʒu��Ԃ�.
char*   fname_skipDriveRoot(FKS_FNAME_const char* name);                    ///< �h���C�u���ƃ��[�g�w�蕔�����X�L�b�v�����ʒu��Ԃ�.

char*   fname_delExt(char name[]);                                          ///< �g���q���폜����.
char*   fname_getNoExt(char dst[], unsigned sz, const char *src);           ///< �g���q���O�������O���擾.
char*   fname_getBaseNameNoExt(char d[],unsigned sz,const char *s);         ///< �f�B���N�g���Ɗg���q���O�������O���擾.

char*   fname_setExt(char dst[], unsigned sz, const char* src, const char *ext);        ///< �g���q���Aext �ɕύX����.
char*   fname_setDefaultExt(char dst[], unsigned sz, const char* src, const char *ext); ///< �g���q���Ȃ���΁Aext ��ǉ�����.
char*   fname_join(char dst[],unsigned sz,const char *dir,const char *nm);              ///< �f�B���N�g�����ƃt�@�C�����̘A��.

char*   fname_getDir(char dir[], unsigned sz, const char *nm);              ///< �f�B���N�g�����̎擾.
char*   fname_getDrive(char drv[], unsigned sz, const char *nm);            ///< �h���C�u�����擾.
char*   fname_getDriveRoot(char dr[],unsigned sz,const char *nm);           ///< �h���C�u�����擾.

char*   fname_checkPosSep(FKS_FNAME_const char* dir, int pos);              ///< pos�̈ʒu��\��/������΂��̃A�h���X���Ȃ����NULL��Ԃ�.
char*   fname_checkLastSep(FKS_FNAME_const char* dir);                      ///< �Ō��\��/������΂��̃A�h���X���Ȃ����NULL��Ԃ�.
char*   fname_delLastSep(char dir[]);                                       ///< ������̍Ō�� \ �� / ������΍폜.
char*   fname_addSep(char dst[], unsigned sz);                              ///< ������̍Ō�� \ / ���Ȃ���Βǉ�.

char*   fname_toUpper(char filename[]);                                     ///< �S�p�Q�o�C�g�ڂ��l������ strupr.
char*   fname_toLower(char filename[]);                                     ///< �S�p�Q�o�C�g�ڂ��l������ strlwr.
char*   fname_backslashToSlash(char filePath[]);                            ///< filePath���� \ �� / �ɒu��.
char*   fname_slashToBackslash(char filePath[]);                            ///< filePath���� / �� \ �ɒu��.

char*   fname_fullpath  (char fullpath[], unsigned sz, const char* path, const char* curDir);       ///< �t���p�X����. os�ˑ�.
char*   fname_fullpathSL(char fullpath[], unsigned sz, const char* path, const char* curDir);       ///< �t���p�X����. / ���.
char*   fname_fullpathBS(char fullpath[], unsigned sz, const char* path, const char* curDir);       ///< �t���p�X����. \ ���.
char*   fname_relativePath  (char relPath[], unsigned sz, const char* path, const char* curDir);    ///< ���΃p�X����. os�ˑ�.
char*   fname_relativePathSL(char relPath[], unsigned sz, const char* path, const char* curDir);    ///< ���΃p�X����. / ���.
char*   fname_relativePathBS(char relPath[], unsigned sz, const char* path, const char* curDir);    ///< ���΃p�X����. \ ���.

int     fname_cmp(const char* l, const char* r);                            ///< �t�@�C�����̑召��r.
int     fname_n_cmp(const char* l, const char* r, unsigned len);            ///< �t�@�C������n�����召��r.
int     fname_digit_cmp(const char* l, const char* r, unsigned len);        ///< ���Ⴂ�̐����𐔒l�Ƃ��ăt�@�C������r.
char*   fname_equLong(FKS_FNAME_const char* fname, const char* baseName);   ///< fname��baseName�Ŏn�܂��Ă��邩�ۂ�.
int     fname_matchWildCard(const char* pattern, const char* str);          ///< ���C���h�J�[�h����(*?)���r. �}�b�`������^.

/// �R�}���h���C��������A;��؂�̕����̃p�X�w�肩��A�P�v�f�擾.
char*   fname_scanArgStr(char arg[],unsigned sz,const char *str, unsigned sepChr);

#ifdef __cplusplus  // �����΍�.
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
// wchar_t* ��

#ifdef _WIN32

#ifdef __cplusplus
#include <wchar.h>

namespace fks {

inline size_t fname_len(const wchar_t* path) { return /*FKS_STD*/ wcslen(path); }

int         fname_isAbs(const wchar_t* path);                                           ///< ��΃p�X���ۂ�(�h���C�u���̗L���͊֌W�Ȃ�)
int         fname_hasDrive(const wchar_t* path);                                        ///< �h���C�u�������Ă��邩. (file: �� http:���h���C�u����)

unsigned    fname_adjust_size(const wchar_t* str, unsigned size);                       ///< (�Ȃ�ׂ��������󂳂Ȃ���)size�����ȓ��̕�������Ԃ�.
wchar_t*    fname_cpy(wchar_t dst[], unsigned sz, const wchar_t* src);                  ///< �t�@�C�����̃R�s�[.
wchar_t*    fname_cat(wchar_t dst[], unsigned sz, const wchar_t* src);                  ///< �t�@�C����������̘A��.

wchar_t*    fname_baseName(FKS_FNAME_const wchar_t *adr);                               ///< �t�@�C���p�X�����̃f�B���N�g�����������t�@�C�����̈ʒu��Ԃ�.
wchar_t*    fname_ext(FKS_FNAME_const wchar_t *name);                                   ///< �g���q�̈ʒu��Ԃ�.
wchar_t*    fname_skipDrive(FKS_FNAME_const wchar_t *name);                             ///< �h���C�u�����X�L�b�v�����ʒu��Ԃ�.
wchar_t*    fname_skipDriveRoot(FKS_FNAME_const wchar_t* name);                         ///< �h���C�u���ƃ��[�g�w�蕔�����X�L�b�v�����ʒu��Ԃ�.

wchar_t*    fname_delExt(wchar_t name[]);                                               ///< �g���q���폜����.
wchar_t*    fname_getNoExt(wchar_t dst[], unsigned sz, const wchar_t *src);             ///< �g���q���O�������O���擾.
wchar_t*    fname_getBaseNameNoExt(wchar_t d[],unsigned sz,const wchar_t *s);           ///< �f�B���N�g���Ɗg���q���O�������O���擾.

wchar_t*    fname_setExt(wchar_t dst[], unsigned sz, const wchar_t* src, const wchar_t *ext);       ///< �g���q���Aext �ɕύX����.
wchar_t*    fname_setDefaultExt(wchar_t dst[],unsigned sz,const wchar_t* src,const wchar_t *ext);   ///< �g���q���Ȃ���΁Aext ��ǉ�����.
wchar_t*    fname_join(wchar_t dst[],unsigned sz,const wchar_t *dir,const wchar_t *nm);             ///< �f�B���N�g�����ƃt�@�C�����̘A��.

wchar_t*    fname_getDir(wchar_t dir[], unsigned sz, const wchar_t *nm);                ///< �f�B���N�g�����̎擾.
wchar_t*    fname_getDrive(wchar_t drv[], unsigned sz, const wchar_t *nm);              ///< �h���C�u�����擾.
wchar_t*    fname_getDriveRoot(wchar_t dr[],unsigned sz,const wchar_t *nm);             ///< �h���C�u�����擾.

wchar_t*    fname_checkLastSep(FKS_FNAME_const wchar_t* dir);                           ///< �Ō��\��/������΂��̃A�h���X���Ȃ����NULL��Ԃ�.
wchar_t*    fname_delLastSep(wchar_t dir[]);                                            ///< ������̍Ō�� \ �� / ������΍폜.
wchar_t*    fname_addSep(wchar_t dst[], unsigned sz);                                   ///< ������̍Ō�� \ / ���Ȃ���Βǉ�.

wchar_t*    fname_toUpper(wchar_t filename[]);                                          ///< �S�p�Q�o�C�g�ڂ��l������ strupr.
wchar_t*    fname_toLower(wchar_t filename[]);                                          ///< �S�p�Q�o�C�g�ڂ��l������ strlwr.
wchar_t*    fname_backslashToSlash(wchar_t filePath[]);                                 ///< filePath���� \ �� / �ɒu��.
wchar_t*    fname_slashToBackslash(wchar_t filePath[]);                                 ///< filePath���� / �� \ �ɒu��.

wchar_t*    fname_fullpath  (wchar_t fullpath[], unsigned sz, const wchar_t* path, const wchar_t* curDir);      ///< �t���p�X����. os�ˑ�.
wchar_t*    fname_fullpathSL(wchar_t fullpath[], unsigned sz, const wchar_t* path, const wchar_t* curDir);      ///< �t���p�X����. / ���.
wchar_t*    fname_fullpathBS(wchar_t fullpath[], unsigned sz, const wchar_t* path, const wchar_t* curDir);      ///< �t���p�X����. \ ���.
wchar_t*    fname_relativePath  (wchar_t relPath[], unsigned sz, const wchar_t* path, const wchar_t* curDir);   ///< ���΃p�X����. os�ˑ�.
wchar_t*    fname_relativePathSL(wchar_t relPath[], unsigned sz, const wchar_t* path, const wchar_t* curDir);   ///< ���΃p�X����. / ���.
wchar_t*    fname_relativePathBS(wchar_t relPath[], unsigned sz, const wchar_t* path, const wchar_t* curDir);   ///< ���΃p�X����. \ ���.

int         fname_cmp(const wchar_t* l, const wchar_t* r);                              ///< �t�@�C�����̑召��r.
int         fname_n_cmp(const wchar_t* l, const wchar_t* r, unsigned len);              ///< �t�@�C������n�����召��r.
int         fname_digit_cmp(const wchar_t* l, const wchar_t* r, unsigned len);          ///< ���Ⴂ�̐����𐔒l�Ƃ��ăt�@�C������r.
wchar_t*    fname_equLong(FKS_FNAME_const wchar_t* fname, const wchar_t* baseName);     ///< fname��baseName�Ŏn�܂��Ă��邩�ۂ�.
int         fname_matchWildCard(const wchar_t* pattern, const wchar_t* str);            ///< ���C���h�J�[�h����(*?)���r. �}�b�`������^.

/// �R�}���h���C��������A;��؂�̕����̃p�X�w�肩��A�P�v�f�擾.
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
