/**
 *	@file	fname.cpp
 *	@brief	�t�@�C�����̏���.
 *	@author Masashi Kitamura
 *	@note
 *	-	c/c++ �p.
 *	-	ara_lib �ɓ����ɍۂ��A�g���q�� .cpp �ɂ��Ă��邪 ���e�� .c�͈̔�.
 *	-	win / linux(unix) �p.
 *		win�ł�\ /���A�ȊO��/���Z�p���[�^.
 *		�ꉞdos���肵�Ă��邪�Aint=2byte���l�����Ă��Ȃ��̂Œ���.
 *	-	������0x80������ascii�n�ł��邱�ƑO��.
 *	-	SJIS���̃}���`�o�C�g�����Ή�.
 *		- Win���ł̓}���`�o�C�g������CharNext�Ń|�C���^��i�߂Ă���.
 *		  (CharNext��UTF8�Ή����Ă������悤��.�ł�win���̂�����MBC��DBC�̂�
 *		  ��utf8�Ή����Ă��Ȃ�)
 *		- Win�ȊO�̊��ł́A���ϐ�LANG���݂�, SJIS,BIG5,GBK���l��.
 *		  (�����炭�S�p2�o�C�g�ڂ�0x5c������̂́A���ꂭ�炢?�Ƒz��)
 *		- ��r�֌W�́A���P�[���Ή��s�\���ɂ��딻�肠�肦��?
 *	-	path�̍ő咷���l�����āAsize_t �łȂ� unsigned �ŃT�C�Y�w��.
 */

#include "fname.h"

#ifdef __cplusplus
#include <cstdlib>
#include <cstring>
#else
#include <stdlib.h>
#include <string.h>
#endif
#ifndef assert
#include <assert.h>
#endif

#ifndef FKS_LIB_DECL
#define FKS_LIB_DECL
#endif

#undef FKS_STD
#define FKS_STD
#undef FKS_W32_NS
#define FKS_W32_NS

// os �̈Ⴂ�֌W.
#if defined _WIN32
 //#include "../win/ara_w32.h"
 #include <windows.h>
 #include <malloc.h>
 #if defined _MSC_VER
  #pragma comment(lib, "User32.lib")			// CharNext()�ŕK�v...
 #endif
#else	// linux
 #include <alloca.h>
 // #include <dirent.h>
 // #include <sys/stat.h>
#endif

// fullpath ����alloca���g��Ȃ��ꍇ��`.
//#define FKS_USE_FNAME_ALLOCA

// unicode�Ή�. �� mb�n���g��ꂽ���Ȃ����� tchar.h ���g�킸���O�őΏ�.
#ifdef FKS_FNAME_WCS_COMPILE	// wchar_t �Ή�.
 #define FKS_FNAME_C(x)				L##x
 #define FKS_FNAME_CHAR				wchar_t
 #define FKS_FNAME_R_STR(s,c)		FKS_STD wcsrchr((s),(c))
 #if defined FKS_FNAME_WINDOS		// �召�������ꎋ.
  #define FKS_FNAME_CMP(l,r)		_wcsicmp((l),(r))
  #define FKS_FNAME_N_CMP(l,r,n)	_wcsnicmp((l),(r),(n))
 #else								// �召���.
  #define FKS_FNAME_CMP(l,r)		FKS_STD wcscmp((l),(r))
  #define FKS_FNAME_N_CMP(l,r,n)	FKS_STD wcsncmp((l),(r),(n))
 #endif
  #define FKS_FNAME_STRTOL(s,t,r)	FKS_STD wcstol((s),(t),(r))
#else			// char �x�[�X.
 #define FKS_FNAME_C(x)				x
 #define FKS_FNAME_CHAR				char
 #define FKS_FNAME_R_STR(s,c)		FKS_STD strrchr((s),(c))
 #if defined FKS_FNAME_WINDOS		// �召�������ꎋ.
  #define FKS_FNAME_CMP(l,r)		_stricmp((l),(r))
  #define FKS_FNAME_N_CMP(l,r,n)	_strnicmp((l),(r),(n))
 #else								// �召���.
  #define FKS_FNAME_CMP(l,r)		FKS_STD strcmp((l),(r))
  #define FKS_FNAME_N_CMP(l,r,n)	FKS_STD strncmp((l),(r),(n))
 #endif
  #define FKS_FNAME_STRTOL(s,t,r)	FKS_STD strtol((s),(t),(r))
#endif
#define FKS_FNAME_IS_DIGIT(c)		(('0') <= (c) && (c) <= ('9'))

// c/c++ �΍�.
#ifdef __cplusplus					// c++�̏ꍇ�A�|�C���^����݂̂̊֐���const��,��const�ł�����.
 #define FKS_FNAME_const_CHAR		FKS_FNAME_CHAR		// ���̂��߁A��{�́A��const�֐��ɂ���.
#else								// c�̏ꍇ�͕W�����C�u�����ɂ��킹 ����const�Ŗ߂�l ��const �ɂ���.
 #define FKS_FNAME_const_CHAR		const FKS_FNAME_CHAR
#endif


// ============================================================================
// �}���`�o�C�g������0x5c �΍�֌W.

/// ���� C �� MS�S�p�̂P�o�C�g�ڂ��ۂ�. (utf8��euc�� \ ���͖����̂� 0���A���ok)
#if defined FKS_FNAME_WCS_COMPILE
 #define FKS_FNAME_ISMBBLEAD(c)		(0)
#elif defined _WIN32
 #define FKS_FNAME_ISMBBLEAD(c)		FKS_W32_NS IsDBCSLeadByte(c)
//#elif defined HAVE_MBCTYPE_H
// #define FKS_FNAME_ISMBBLEAD(c)	_ismbblead(c)
#elif defined FKS_USE_FNAME_MBC
 #define FKS_FNAME_ISMBBLEAD(c)		((unsigned)(c) >= 0x80 && fname_is_zenkaku1((c)))
#else
 #define FKS_FNAME_ISMBBLEAD(c)		(0)
#endif

/// ���̕����փ|�C���^��i�߂�. ��CharNext()���T���Q�[�g�y�A��utf8�Ή����Ă���Ă��炢���ȂƊ���(�ʖڂ���������)
#if  defined _WIN32
 #ifdef FKS_FNAME_WCS_COMPILE
  #define FKS_FNAME_CHARNEXT(p)		(FKS_FNAME_CHAR*)FKS_W32_NS CharNextW((FKS_FNAME_CHAR*)(p))
 #else
  #define FKS_FNAME_CHARNEXT(p)		(FKS_FNAME_CHAR*)FKS_W32_NS CharNextA((FKS_FNAME_CHAR*)(p))
 #endif
#else
 #if defined FKS_FNAME_WCS_COMPILE || !defined(FKS_USE_FNAME_MBC)
  #define FKS_FNAME_CHARNEXT(p)		((p) + 1)
 #else
  #define FKS_FNAME_CHARNEXT(p)		((p) + 1 + (FKS_FNAME_ISMBBLEAD(*(unsigned char*)(p)) && (p)[1]))
 #endif
#endif


// ----------------------------------
// win �ȊO�ŁAsjis�Ή�����ꍇ�̏���.

#if defined(FKS_USE_FNAME_MBC) && !defined(_WIN32) && !defined(FKS_FNAME_WCS_COMPILE)	//&& !defined(HAVE_MBCTYPE_H)	// win32�ȊO��, LANG�Ή�����ꍇ.

#ifdef __cplusplus
namespace fks {
#endif

/** �Ƃ肠�����A0x5c�֌W�̑Ώ��p.
 */
static FKS_LIB_DECL int fname_mbc_init(void)
{
	const char* 		lang = FKS_STD getenv("LANG");
	const char* 		p;
	if (lang == 0)
		return 0;
	//s_fname_locale_ctype = strdup(lang);
	// ja_JP.SJIS �̂悤�Ȍ`���ł��邱�Ƃ�O���SJIS,big5,gbk�����`�F�b�N.
	p = FKS_STD strrchr(lang, '.');
	if (p) {
		const char* enc = p + 1;
		// 0x5c�΍􂪕K�v��encoding�����`�F�b�N.
		if (strncasecmp(enc, "sjis", 4) == 0) {
			return 1;
		} else if (strncasecmp(enc, "gbk", 3) == 0 || strncasecmp(enc, "gb2312", 6) == 0) {
			return 2;
		} else if (strncasecmp(enc, "big5", 4) == 0) {
			return 3;
		}
	}
	return 0;
}

static FKS_LIB_DECL int fname_is_zenkaku1(unsigned c) {
	static int	  s_fname_shift_char_mode = -1;
	if (s_fname_shift_char_mode < 0)
		s_fname_shift_char_mode = fname_mbc_init();
	switch (s_fname_shift_char_mode) {
	case 1 /* sjis */: return ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC));
	case 2 /* GBK  */: return  (c >= 0x81 && c <= 0xFE);
	case 3 /* BIG5 */: return ((c >= 0xA1 && c <= 0xC6) || (c >= 0xC9 && c <= 0xF9));
	default:		   return 0;
	}
}

#ifdef __cplusplus
}	// fks
#endif

#endif	// !defined(_WIN32) && !defined(FKS_FNAME_WCS_COMPILE)



// ############################################################################
#ifdef __cplusplus
namespace fks {
#endif

// ============================================================================

#define FKS_FNAME_TO_UPPER(c)		(((c) >= FKS_FNAME_C('a') && (c) <= FKS_FNAME_C('z')) ? (c) - FKS_FNAME_C('a') + FKS_FNAME_C('A') : (c))
#define ATA_FNAME_TO_LOWER(c)		(((c) >= FKS_FNAME_C('A') && (c) <= FKS_FNAME_C('Z')) ? (c) - FKS_FNAME_C('A') + FKS_FNAME_C('a') : (c))


/** ��΃p�X���ۂ�(�h���C�u���̗L���͊֌W�Ȃ�)
 */
FKS_LIB_DECL int	fname_isAbs(const FKS_FNAME_CHAR* path)
{
	if (path == 0)
		return 0;
	path = fname_skipDrive(path);
	return fname_isSep(path[0]);
}


/** �h���C�u�������Ă��邩.
 */
FKS_LIB_DECL int		fname_hasDrive(const FKS_FNAME_CHAR* path)
{
  #if 1	// �擪��"������:"���h���C�u���Ƃ݂Ȃ�.
	const FKS_FNAME_CHAR* s = path;
	if (s == 0)
		return 0;
	if (*s && *s != FKS_FNAME_C(':')) {
		while (*s && !fname_isSep(*s)) {
			if (*s == FKS_FNAME_C(':')) {
				return 1;
			}
			++s;
		}
	}
	return 0;
  #else	// �ꎚ�h���C�u���̂ݑΉ�����ꍇ.
   #if defined FKS_FNAME_WINDOS
	if (path == 0)
		return 0;
	return (path[0] && path[1] == FKS_FNAME_C(':'));
   #else
	return 0;
   #endif
  #endif
}


// ============================================================================

/** size�Ɏ��܂镶����̕�������Ԃ�. \0���܂܂Ȃ�.
 *	(win���ł͂Ȃ�ׂ��}���`�o�C�g�����̓r���ŏI���Ȃ��悤�ɂ���.
 *	 ���ǁA�p�r�I�ɂ͐؂��ȏ゠�܂�Ӗ��Ȃ�...)
 */
FKS_LIB_DECL unsigned	fname_adjust_size(const FKS_FNAME_CHAR* str, unsigned size)
{
	const FKS_FNAME_CHAR* s = str;
	const FKS_FNAME_CHAR* b = s;
	const FKS_FNAME_CHAR* e = s + size;
	assert(str != 0 && size > 0);
	while (s < e) {
		if (*s == 0)
			return s - str;
		b = s;
		s = FKS_FNAME_CHARNEXT((FKS_FNAME_CHAR*)s);
	}
	return b - str;
}


/** �t�@�C�����̃R�s�[. mbc�̎��͕��������Ȃ������܂�. dst == src ��ok.
 */
FKS_LIB_DECL FKS_FNAME_CHAR*	fname_cpy(FKS_FNAME_CHAR dst[], unsigned dstSz, const FKS_FNAME_CHAR* src)
{
	unsigned 	l;
	assert(dst != NULL && dstSz > 0);

	if (src == NULL)
		return NULL;
	l = fname_adjust_size(src, dstSz);

	// �A�h���X�������Ȃ�A���������킹��̂�.
	if (dst == src) {
		dst[l] = 0;
		return dst;
	}

	// �R�s�[.
	{
		const FKS_FNAME_CHAR*	s = src;
		const FKS_FNAME_CHAR*	e = s + l;
		FKS_FNAME_CHAR*			d = dst;
		while (s < e)
			*d++ = *s++;
		*d = 0;
	}

	return dst;
}


/** �t�@�C����������̘A��.
 */
FKS_LIB_DECL FKS_FNAME_CHAR*	fname_cat(FKS_FNAME_CHAR dst[], unsigned dstSz, const FKS_FNAME_CHAR* src)
{
	unsigned l;
	assert(dst != NULL && dstSz > 0);
	if (src == 0)
		return NULL;
	assert(src != 0 && dst != src);
	l = fname_len(dst);
	if (l >= dstSz) {	// ���������]���悪���t�Ȃ�T�C�Y�����̂�.
		return fname_cpy(dst, dstSz, dst);
	}
	fname_cpy(dst+l, dstSz - l, src);
	return dst;
}


// ============================================================================

/** �h���C�u��������΂�����X�L�b�v�����|�C���^��Ԃ�.
 *	 �� c:�������łȂ�http:���X�L�b�v���邽�� "������:" ���X�L�b�v.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_skipDrive(FKS_FNAME_const_CHAR* path)
{
  #if 1
	const FKS_FNAME_CHAR* s = path;
	if (*s && *s != FKS_FNAME_C(':')) {
		while (*s && !fname_isSep(*s)) {
			if (*s == FKS_FNAME_C(':')) {
				return (FKS_FNAME_CHAR*)s+1;
			}
			++s;
		}
	}
 #else	// 1���h���C�u���݂̂̑Ή��Ȃ炱����.
  #if defined FKS_FNAME_WINDOS
	if (path[0] && path[1] == FKS_FNAME_C(':'))		// �h���C�u���t��������
		return (FKS_FNAME_CHAR*) path + 2;
	#endif
 #endif
	return (FKS_FNAME_CHAR*)path;
}


/** ������擪�̃h���C�u��,���[�g�w����X�L�b�v�����|�C���^��Ԃ�.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_skipDriveRoot(FKS_FNAME_const_CHAR* path)
{
	FKS_FNAME_CHAR* p = fname_skipDrive(path);
	while (fname_isSep(*p))
		++p;
	return p;
}


/** �t�@�C���p�X�����̃f�B���N�g�����������t�@�C�����̈ʒu��Ԃ�.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_baseName(FKS_FNAME_const_CHAR *adr)
{
	const FKS_FNAME_CHAR *p = adr;
	assert(adr != 0);
	while (*p) {
		if (*p == FKS_FNAME_C(':') || fname_isSep(*p))
			adr = (FKS_FNAME_CHAR*)p + 1;
		p = FKS_FNAME_CHARNEXT(p);
	}
	return (FKS_FNAME_CHAR*)adr;
}


/**	�g���q�̈ʒu��Ԃ�. '.'�͊܂�. �Ȃ���Ε�����̍Ō��Ԃ�.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_ext(FKS_FNAME_const_CHAR *name)
{
	const FKS_FNAME_CHAR *p;
	assert(name != 0);
	name = fname_baseName(name);
	p	 = FKS_FNAME_R_STR(name, FKS_FNAME_C('.'));
	if (p) {
		return (FKS_FNAME_CHAR*)(p);
	}
	return (FKS_FNAME_CHAR*)name + fname_len(name);
}


// ============================================================================

/** �t�@�C���p�X�����̊g���q���폜����.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_delExt(FKS_FNAME_CHAR buf[])
{
	FKS_FNAME_CHAR *t;
	FKS_FNAME_CHAR *p;
	assert(buf != 0);
	t = fname_baseName(buf);
	p = FKS_FNAME_R_STR(t, FKS_FNAME_C('.'));
	if (p == 0) {
		p = t + fname_len(t);
	}
	*p = 0;
	return buf;
}


/** �t�@�C���p�X�����̊g���q�������������̎擾.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_getNoExt(FKS_FNAME_CHAR dst[], unsigned size, const FKS_FNAME_CHAR *src)
{
	const FKS_FNAME_CHAR *s;
	const FKS_FNAME_CHAR *e;
	unsigned		 l = 0;
	assert(dst != 0 && size > 1 && src != 0);
	//if (dst == 0 || size == 0 || src == 0) return 0;
	s = fname_baseName(src);
	e = FKS_FNAME_R_STR(s, FKS_FNAME_C('.'));
	if (e == 0) {
		e = s + fname_len(s);
	}
	l = e - src + 1;
	if (l > size)
		l = size;
	fname_cpy(dst, l, src);
	return dst;
}


/** �t�@�C���p�X�����̃f�B���N�g���Ɗg���q���������t�@�C�����̎擾.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_getBaseNameNoExt(FKS_FNAME_CHAR dst[], unsigned size, const FKS_FNAME_CHAR *src)
{
	const FKS_FNAME_CHAR *s;
	const FKS_FNAME_CHAR *e;
	unsigned		 l = 0;
	assert(dst != 0 && size > 1 && src != 0);
	//if (dst == 0 || size == 0 || src == 0) return 0;
	s = fname_baseName(src);
	e = FKS_FNAME_R_STR(s, FKS_FNAME_C('.'));
	if (e == 0) {
		e = s + fname_len(s);
	}
	l = e - s + 1;
	if (l > size)
		l = size;
	fname_cpy(dst, l, s);
	return dst;
}


/**	�g���q���Aext �ɕύX����. dst == src �ł��悢.
 *	ext = NULL or "" �̂Ƃ��� �g���q�폜.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_setExt(FKS_FNAME_CHAR dst[], unsigned size, const FKS_FNAME_CHAR* src, const FKS_FNAME_CHAR *ext)
{
	assert(dst != 0 && size > 0 && src != 0);
	fname_getNoExt(dst, size, src);
	if (ext && ext[0]) {
		if (ext[0] != FKS_FNAME_C('.'))
			fname_cat(dst, size, FKS_FNAME_C("."));
		fname_cat(dst, size, ext);
	}
	return dst;
}


/**	�g���q���Ȃ��ꍇ�A�g���q��ǉ�����.(����Ή������Ȃ�). dst == src �ł��悢.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_setDefaultExt(FKS_FNAME_CHAR dst[], unsigned size, const FKS_FNAME_CHAR* src, const FKS_FNAME_CHAR *ext)
{
	FKS_FNAME_CHAR*	p;
	assert(dst != 0 && size > 0 && src != 0);

	fname_cpy(dst, size, src);
	if (ext == 0)
		return dst;
	p = fname_baseName(dst);
	p = FKS_FNAME_R_STR(p, FKS_FNAME_C('.'));
	if (p) {
		if (p[1])
			return dst;
		*p = 0;
	}
	if (ext[0]) {
		if (ext[0] != FKS_FNAME_C('.'))
			fname_cat(dst, size, FKS_FNAME_C("."));
		fname_cat(dst, size, ext);
	}
	return dst;
}


// ============================================================================

/**	�h���C�u���������擾. :��. �� file:���̑Ώ��̂���"������:"���h���C�u����
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_getDrive(FKS_FNAME_CHAR drive[], unsigned size, const FKS_FNAME_CHAR *name)
{
	const FKS_FNAME_CHAR*	s;
	unsigned			 	l;
	assert(drive && size > 0 && name);
	drive[0] = 0;
	s = fname_skipDrive(name);
	l = s - name;
	if (l > 0) {
		++l;
		if (l > size)
			l = size;
		fname_cpy(drive, l, name);
	}
	return drive;
}


/**	�h���C�u���ƃ��[�g�w�蕔�����擾.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_getDriveRoot(FKS_FNAME_CHAR dr[], unsigned size, const FKS_FNAME_CHAR *name)
{
	const FKS_FNAME_CHAR*	s;
	unsigned	 	  		l;
	assert(dr && size > 0 && name);
	dr[0] = 0;
	s = fname_skipDriveRoot(name);
	l = s - name;
	if (l > 0) {
		++l;
		if (l > size)
			l = size;
		fname_cpy(dr, l, name);
	}
	return dr;
}


/**	�f�B���N�g��������Ԃ�. �Ō�̃f�B���N�g���Z�p���[�^�͊O��.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_getDir(FKS_FNAME_CHAR dir[], unsigned size, const FKS_FNAME_CHAR *name)
{
	const FKS_FNAME_CHAR*	p;
	size_t		 			l;

	assert(dir	!= 0 && size > 0 && name != 0);

	p = fname_baseName(name);
	l = p - name + 1;
	if (l > size) {
		l = size;
	}
	if (l && dir != name)
		fname_cpy(dir, l, name);
	dir[l] = 0;
	if (l > 0)
		fname_delLastSep(dir);
	return dir;
}



// ============================================================================

/** ������̍Ō�� \ �� / ������΂��̈ʒu��Ԃ��A�Ȃ����NULL��Ԃ�.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_checkPosSep(FKS_FNAME_const_CHAR* dir, int ofs)
{
	assert(dir != 0);
	if (dir) {
		const FKS_FNAME_CHAR*	s 	= dir;
		if (ofs >= 0) {
			const FKS_FNAME_CHAR* p = s + ofs;
			if (*p == FKS_FNAME_C('/')) {
				return (FKS_FNAME_CHAR *)p;
			}
		  #if (defined FKS_FNAME_WINDOS)
			else if (*p == FKS_FNAME_C('\\')) {
			  #ifdef FKS_FNAME_WCS_COMPILE
				return (FKS_FNAME_CHAR *)p;
			  #else		// adjust_size�̌��ʂ�ofs�����ɂȂ��Ă���*p�̓}���`�o�C�g�����̈ꕔ.
				if (fname_adjust_size(s, ofs+1) == ofs)
					return (FKS_FNAME_CHAR *)p;
			  #endif
			}
		  #endif
		}
	}
	return NULL;
}


/** ������̍Ō�� \ �� / ������΂��̈ʒu��Ԃ��A�Ȃ����NULL��Ԃ�.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_checkLastSep(FKS_FNAME_const_CHAR* dir)
{
	unsigned l = fname_len(dir);
	if (l == 0) return 0;
	return fname_checkPosSep(dir, l - 1);
}


/** ������̍Ō�� \ �� / ������΍폜
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_delLastSep(FKS_FNAME_CHAR dir[])
{
	FKS_FNAME_CHAR*	p = fname_checkLastSep(dir);
	if (p) {
		*p = 0;
	}
	return dir;
}


/** ������̍Ō�� �f�B���N�g���Z�p���[�^�������Ȃ���Βǉ�.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_addSep(FKS_FNAME_CHAR dir[], unsigned size)
{
	FKS_FNAME_CHAR*	e = dir + size;
	FKS_FNAME_CHAR*	p = fname_checkLastSep(dir);
	if (p == 0) {
		p = dir + fname_len(dir);
		if (p+1 < e) {
			*p++ = FKS_FNAME_SEP_CHR;
			*p = 0;
		}
	}
	return dir;
}


/** �f�B���N�g�����ƃt�@�C��������������. fname_cat �ƈႢ�A\ / ���Ԃɕt��.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_join(FKS_FNAME_CHAR buf[], unsigned bufSz, const FKS_FNAME_CHAR *dir, const FKS_FNAME_CHAR *name)
{
	fname_cpy(buf, bufSz, dir);
	if (buf[0])
		fname_addSep(buf, bufSz);
	fname_cat(buf, bufSz, name);
	return buf;
}


// ============================================================================

/** �S�p�Q�o�C�g�ڂ��l������ strupr
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_toUpper(FKS_FNAME_CHAR name[])
{
	FKS_FNAME_CHAR *p = name;
	assert(name != NULL);
	while (*p) {
		unsigned c = *p;
		*p = FKS_FNAME_TO_UPPER(c);
		p  = FKS_FNAME_CHARNEXT(p);
	}
	return name;
}


/** �S�p�Q�o�C�g�ڂ��l������ strlwr
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_toLower(FKS_FNAME_CHAR name[])
{
	FKS_FNAME_CHAR *p = name;
	assert(name != NULL);

	while (*p) {
		unsigned c = *p;
		*p = ATA_FNAME_TO_LOWER(c);
		p  = FKS_FNAME_CHARNEXT(p);
	}
	return name;
}


/** filePath���� \ �� / �ɒu��.
 */
FKS_LIB_DECL FKS_FNAME_CHAR 	*fname_backslashToSlash(FKS_FNAME_CHAR filePath[])
{
	FKS_FNAME_CHAR *p = filePath;
	assert(filePath != NULL);
	while (*p != FKS_FNAME_C('\0')) {
		if (*p == FKS_FNAME_C('\\')) {
			*p = FKS_FNAME_C('/');
		}
		p = FKS_FNAME_CHARNEXT(p);
	}
	return filePath;
}


/** filePath���� / �� \ �ɒu��.
 */
FKS_LIB_DECL FKS_FNAME_CHAR 	*fname_slashToBackslash(FKS_FNAME_CHAR filePath[])
{
	FKS_FNAME_CHAR *p;
	assert(filePath != NULL);
	for (p = filePath; *p; ++p) {
		if (*p == FKS_FNAME_C('/')) {
			*p = FKS_FNAME_C('\\');
		}
	}
	return filePath;
}


// ============================================================================

FKS_LIB_DECL FKS_FNAME_CHAR*	fname_fullpath(FKS_FNAME_CHAR dst[], unsigned size, const FKS_FNAME_CHAR* path, const FKS_FNAME_CHAR* currentDir) {
  #if defined FKS_FNAME_WINDOS
	return fname_fullpathBS(dst, size, path, currentDir);
  #else
	return fname_fullpathSL(dst, size, path, currentDir);
  #endif
}


/** �t���p�X����. �f�B���N�g���Z�p���[�^��\\�ɂ��ĕԂ��o�[�W����.
 */
FKS_LIB_DECL FKS_FNAME_CHAR*	fname_fullpathBS(FKS_FNAME_CHAR dst[], unsigned size, const FKS_FNAME_CHAR* path, const FKS_FNAME_CHAR* currentDir)
{
	fname_fullpathSL(dst, size, path, currentDir);
	fname_slashToBackslash(dst);
	return dst;
}


/** �t���p�X����. �����񑀍�̂�. �J�����g�p�X�͈����œn��.
 *	currentDir �͐�΃p�X�ł��邱��. �����łȂ��ꍇ�̋����͕s��.
 *	'\'�����΍�ŁA�Z�p���[�^��'/'�ɒu�����Ă���.
 */
FKS_LIB_DECL FKS_FNAME_CHAR*	fname_fullpathSL(FKS_FNAME_CHAR dst[], unsigned size, const FKS_FNAME_CHAR* path, const FKS_FNAME_CHAR* currentDir)
{
	FKS_FNAME_CHAR*		wk;
	unsigned 			wkSz;

	assert(dst != 0 && size > 2 && path != 0);
	if (dst == 0 || size <= 2 || path == 0)
		return 0;
	if (currentDir == 0)
		currentDir = FKS_FNAME_C("/");	// DIRSEP_STR;
	assert(fname_isAbs(currentDir));

  #if !(defined FKS_USE_FNAME_ALLOCA)
	// �������ݐ�T�C�Y���\���łȂ���΍�Ɨp���������m��.
	{
		unsigned pl = fname_len(path);
		unsigned cl = fname_len(currentDir);
		wkSz = pl + cl + 4;
		if (wkSz >= size) {		// dst�T�C�Y�����A���������Ȃ烏�[�N��p��.
			assert( wkSz <= FKS_FNAME_MAX_URL * sizeof(FKS_FNAME_CHAR) );
			wk = (FKS_FNAME_CHAR*)alloca(wkSz*sizeof(FKS_FNAME_CHAR));
			if (wk == 0) {
				wk	 = dst;
				wkSz = size;
			}
		} else {
			wkSz = size;
			wk	 = dst;
		}
	}
  #else	// alloca��S���g��Ȃ��ꍇ�͏o�͐�𒼐ڎg������.
	wkSz = size;
	wk	 = dst;
  #endif

	// ��Ɨp�̐�΃p�X�����.
	{
		unsigned hasDrive = fname_hasDrive(path);
		unsigned isAbs	  = fname_isAbs(path);
		wk[0] = 0;
		if (isAbs && hasDrive) {	// �h���C�u�t����΃p�X�Ȃ�A���̂܂�.
			fname_cpy(wk, wkSz, path);
		} else if (isAbs) {
			if (fname_hasDrive(currentDir)) {	// ��΃p�X�����ǃh���C�u���Ȃ��ꍇ��currentDir����h���C�u������������.
				fname_getDrive(wk, wkSz, currentDir);
			}
			fname_cat(wk, wkSz, path);
		} else {
			if (hasDrive) {			// �h���C�u�t�����΃p�X�ŁA
				if (!fname_hasDrive(currentDir))	// �J�����g���Ƀh���C�u���Ȃ����
					fname_getDrive(wk, wkSz, path);	// path�̃h���C�u����ݒ�. �������΃J�����g���̃h���C�u���ɂȂ�.
			}
			fname_cat(wk, wkSz, currentDir);
			fname_addSep(wk, wkSz);
			fname_cat(wk, wkSz, fname_skipDrive(path));
		}
	}

  #if defined FKS_FNAME_WINDOS
	// �������ȒP�ɂ��邽�߁A�p�X�̋�؂����U / �ɕϊ�.
	fname_backslashToSlash(wk);
  #endif

	// "." �� ".." ����菜��.
	{
		// ���̎��_��wk�͕K����΃p�X�ɂȂ��Ă���.(currentDir���ᔽ���Ă��ꍇ�̋����͕s�舵��).
		FKS_FNAME_CHAR*		s	  = fname_skipDrive(wk);	// �h���C�u���͘M��Ȃ��̂ŃX�L�b�v.
		FKS_FNAME_CHAR*		d	  = s;
		FKS_FNAME_CHAR*		top   = d;
		unsigned			first = 1;
		while (*s) {
			unsigned c = *s++;
			if (c == FKS_FNAME_C('/')) {
				if (first) {	// ����� / �� "//" "///" ������... ���Ƃ�*d++=c����̂ł����ł�2��܂�.
					unsigned i;
					for (i = 0; i < 2 && *s == FKS_FNAME_C('/'); ++i) {
						*d++ = *s++;
					}
				}
				first = 0;
				// '/'�̘A���͈��'/'����.
			  RETRY:
				while (*s == FKS_FNAME_C('/'))
					++s;
				if (*s == FKS_FNAME_C('.')) {
					if (s[1] == 0) {					// "." �݂͖̂���.
						s += 1;
						goto RETRY;
					} else if (s[1] == FKS_FNAME_C('/')) {		// "./" �͖���.
						s += 2;
						goto RETRY;
					} else if (s[1] == FKS_FNAME_C('.') && (s[2] == FKS_FNAME_C('/') || s[2] == 0)) {	// "../" ".." �̂Ƃ�.
						s += 2 + (s[2] != 0);
						while (d > top && *--d != FKS_FNAME_C('/')) {	// �o�͐�̃f�B���N�g���K�w���P���炷.
							;
						}
						goto RETRY;
					}
				}
			}
			*d++ = c;
		}
		*d = 0;
	}

  #if !(defined FKS_USE_FNAME_ALLOCA)
	if (wk != dst) {	// ���[�N��alloca���Ă��̂Ȃ�A�R�s�[.
		fname_cpy(dst, size, wk);
	}
  #endif

	return dst;
}


// ============================================================================
#if 1	// �{���͕ʃ��W���[����������.
/** �R�}���h���C��������A;��؂�̕����p�X�w����A��������̂Ɏg��.
 *	""��win�R�}���h���C���ɂ��킹������.
 *	sepChr�ŋ�؂�ꂽ������(�t�@�C����)���擾. 0x20�ȊO�̋󔒂͖�����0x20����.
 *	@return �X�L�����X�V��̃A�h���X��Ԃ��Bstr��EOS��������NULL��Ԃ�.
 */
FKS_LIB_DECL FKS_FNAME_CHAR *fname_scanArgStr(FKS_FNAME_CHAR arg[], unsigned argSz, const FKS_FNAME_CHAR *str, unsigned sepChr)
{
  #ifdef FKS_FNAME_WCS_COMPILE
	const FKS_FNAME_CHAR*	s = str;
  #else
	const unsigned char* 	s = (const unsigned char*)str;
  #endif
	FKS_FNAME_CHAR*	  		d = arg;
	FKS_FNAME_CHAR*			e = d + argSz;
	unsigned 			 	f = 0;
	unsigned			 	c;

	assert( str != 0 && arg != 0 && argSz > 1 );

	// 0x20�ȊO�̋󔒂ƃZ�p���[�^���X�L�b�v.
	while ( *s == sepChr || (*s > 0U && *s < 0x20U) || *s == 0x7fU) {
		++s;
	}
	if (*s == 0) {	// EOS��������A������Ȃ������Ƃ���NULL��Ԃ�.
		arg[0] = 0;
		return NULL;
	}

	do {
		c = *s++;
		if (c == FKS_FNAME_C('"')) {
			f ^= 1;							// "�̑΂̊Ԃ͋󔒂��t�@�C�����ɋ���.���߂̃t���O.

			// ������ƋC�����������AWin(XP)��cmd.exe�̋����ɍ��킹�Ă݂�.
			// (�ق�Ƃɂ����Ă邩�A�\���ɂ͒��ׂĂȂ�)
			if (*s == FKS_FNAME_C('"') && f == 0)	// ��"�̒���ɂ����"������΁A����͂��̂܂ܕ\������.
				++s;
			else
				continue;					// �ʏ�� " �͏Ȃ��Ă��܂�.
		}
		if ((c > 0 && c < 0x20) || c == 0x7f)
			c = FKS_FNAME_C(' ');
		if (d < e) {
			*d++ = (FKS_FNAME_CHAR)c;
		}
	} while (c >= 0x20 && c != 0x7f && (f || (c != sepChr)));
	*--d  = FKS_FNAME_C('\0');
	--s;
	return (FKS_FNAME_CHAR *)s;
}
#endif


// ============================================================================

// �t�@�C����������̃|�C���^p����1�����擾����c�ɂ����}�N��.
// os��win�Ȃ�2�o�C�g�����Ή��ŏ�������. utf8�͔j�].
#ifdef FKS_FNAME_WCS_COMPILE
 #if defined FKS_FNAME_WINDOS
  #define FKS_FNAME_GET_C(c, p)		((c) = *((p)++), (c) = ATA_FNAME_TO_LOWER(c))
 #else
  #define FKS_FNAME_GET_C(c, p)		((c) = *((p)++))
 #endif
#else
 #if defined FKS_FNAME_WINDOS
  #define FKS_FNAME_GET_C(c, p)	do {						\
		(c) = *(unsigned char*)((p)++);						\
		if (FKS_FNAME_ISMBBLEAD(c) && *(p))					\
			(c) = ((c) << 8) | *(unsigned char*)((p)++);	\
		else												\
			(c) = ATA_FNAME_TO_LOWER(c);					\
	} while (0)
 #else
  #define FKS_FNAME_GET_C(c, p)	do {						\
		(c) = *(unsigned char*)((p)++);						\
		if (FKS_FNAME_ISMBBLEAD(c) && *(p))					\
			(c) = ((c) << 8) | *(unsigned char*)((p)++);	\
	} while (0)
 #endif
#endif


/** �t�@�C�����̑召��r.
 *	win/dos�n�͑召���ꎋ. �f�B���N�g���Z�p���[�^ \ / �����ꎋ.
 *	�ȊO�͒P���ɕ������r.
 */
FKS_LIB_DECL int fname_cmp(const FKS_FNAME_CHAR* l, const FKS_FNAME_CHAR* r)
{
	return fname_n_cmp(l, r, (unsigned)-1);
}


/** �t�@�C�����̑召��r.
 *	win/dos�n�͑召���ꎋ. �f�B���N�g���Z�p���[�^ \ / �����ꎋ.
 *	�ȊO�͒P���ɕ������r.
 */
FKS_LIB_DECL int   fname_n_cmp(const FKS_FNAME_CHAR* l, const FKS_FNAME_CHAR* r, unsigned len)
{
 #if defined FKS_FNAME_WINDOS
	const FKS_FNAME_CHAR* e = l + len;
	assert( l != 0 && r != 0 );
	if (e < l)
		e = (const FKS_FNAME_CHAR*)-1;
	while (l < e) {
		int		 n;
		unsigned lc;
		unsigned rc;

		FKS_FNAME_GET_C(lc, l);
		FKS_FNAME_GET_C(rc, r);

		n  = (int)(lc - rc);
		if (n == 0) {
			if (lc == 0)
				return 0;
			continue;
		}

		if ((lc == FKS_FNAME_C('/') && rc == FKS_FNAME_C('\\')) || (lc == FKS_FNAME_C('\\') && rc == FKS_FNAME_C('/'))) {
			continue;
		}
		return n;
	}
	return 0;
 #else
	int i;
	assert( l != 0 && r != 0 );
  #if 1
	return FKS_FNAME_N_CMP(l, r, len);
  #else
	char* 	orig = setlocale(LC_CTYPE, s_fname_locale_ctype);
	i = FKS_FNAME_CMP(l, r);
	setlocale(orig);
	return i;
  #endif
 #endif
}


/** �t�@�C�����̑召��r. ���l���������ꍇ�A�����Ⴂ�̐��l���m�̑召�𔽉f
 *	win/dos�n�͑召���ꎋ. �f�B���N�g���Z�p���[�^ \ / �����ꎋ.
 *	�ȊO�͒P���ɕ������r.
 */
FKS_LIB_DECL int   fname_digit_cmp(const FKS_FNAME_CHAR* l, const FKS_FNAME_CHAR* r, unsigned len)
{
	const FKS_FNAME_CHAR* e = l + len;
	assert( l != 0 && r != 0 );
	if (e < l)
		e = (const FKS_FNAME_CHAR*)-1;
	while (l < e) {
		ptrdiff_t	n;
		unsigned 	lc;
		unsigned 	rc;

		FKS_FNAME_GET_C(lc, l);
		FKS_FNAME_GET_C(rc, r);

		n  = (ptrdiff_t)(lc - rc);
		if (n == 0) {
			if (lc == 0)
				return 0;
			continue;
		}

	  #ifdef FKS_FNAME_WINDOS
		if ((lc == FKS_FNAME_C('/') && rc == FKS_FNAME_C('\\')) || (lc == FKS_FNAME_C('\\') && rc == FKS_FNAME_C('/'))) {
			continue;
		}
	  #endif

		if (lc <= 0x80 && FKS_FNAME_IS_DIGIT(lc) && rc <= 0x80 && FKS_FNAME_IS_DIGIT(rc)) {
			ptrdiff_t	lv = FKS_FNAME_STRTOL(l-1, (char**)&l, 10);
			ptrdiff_t	rv = FKS_FNAME_STRTOL(r-1, (char**)&r, 10);
			n = lv - rv;
			if (n == 0)
				continue;
		}

		return (n < 0) ? -1 : 1;
	}
	return 0;
}


/** fname��baseName�Ŏn�܂��Ă���΁Afname�̗]���̐擪�̃A�h���X��Ԃ�.
 *	�}�b�`���Ă��Ȃ����NULL��Ԃ�.
 */
FKS_LIB_DECL FKS_FNAME_CHAR*	 fname_equLong(FKS_FNAME_const_CHAR* fname, const FKS_FNAME_CHAR* baseName)
{
	unsigned l;
	assert(fname && baseName);
	l = fname_len(baseName);
	if (l == 0)
		return (FKS_FNAME_CHAR*)fname;
	return (fname_n_cmp(fname, baseName, l) == 0) ? (FKS_FNAME_CHAR*)fname+l : 0;
}


/** ?,* �݂̂�(dos/win��)���C���h�J�[�h�������r.
 * *,? �̓Z�p���[�^�ɂ̓}�b�`���Ȃ�.
 * �������g������ ** �̓Z�p���[�^�ɂ��}�b�`����.
 * ��unDonut�̃\�[�X���R�s�y����.
 *	 ���̌���http://www.hidecnet.ne.jp/~sinzan/tips/main.htm�炵���������N��.
 *	@param ptn	�p�^�[��(*?�w��\)
 *	@param tgt	�^�[�Q�b�g������.
 */
FKS_LIB_DECL int fname_matchWildCard(const FKS_FNAME_CHAR* ptn, const FKS_FNAME_CHAR* tgt)
{
	unsigned 				tc;
	const FKS_FNAME_CHAR*	tgt2 = tgt;
	FKS_FNAME_GET_C(tc, tgt2);	// 1���擾& tgt�|�C���^�X�V.
	switch (*ptn) {
	case FKS_FNAME_C('\0'):
		return tc == FKS_FNAME_C('\0');

  #if defined FKS_FNAME_WINDOWS
	case FKS_FNAME_C('\\'):
	case FKS_FNAME_C('/'):
		return (tc == FKS_FNAME_C('/') || tc == FKS_FNAME_C('\\')) && fname_matchWildCard(ptn+1, tgt2);
  #endif

	case FKS_FNAME_C('?'):
		return tc && !fname_isSep(tc) && fname_matchWildCard( ptn+1, tgt2 );

	case FKS_FNAME_C('*'):
		if (ptn[1] == FKS_FNAME_C('*'))	// ** �Ȃ�Z�p���[�^�ɂ��}�b�`.
			return fname_matchWildCard(ptn+2, tgt) || (tc && fname_matchWildCard( ptn, tgt2 ));
		return fname_matchWildCard(ptn+1, tgt) || (tc && !fname_isSep(tc) && fname_matchWildCard( ptn, tgt2 ));

	default:
		{
			unsigned pc;
			FKS_FNAME_GET_C(pc, ptn);	// 1���擾& ptn�|�C���^�X�V.
			return (pc == tc) && fname_matchWildCard(ptn, tgt2);
		}
	}
}


// ============================================================================

FKS_LIB_DECL FKS_FNAME_CHAR*	fname_relativePath(FKS_FNAME_CHAR dst[], unsigned size, const FKS_FNAME_CHAR* path, const FKS_FNAME_CHAR* currentDir) {
  #if defined FKS_FNAME_WINDOS
	return fname_relativePathBS(dst, size, path, currentDir);
  #else
	return fname_relativePathSL(dst, size, path, currentDir);
  #endif
}


/** ���΃p�X����. �f�B���N�g���Z�p���[�^��\\�ɂ��ĕԂ��o�[�W����.
 */
FKS_LIB_DECL FKS_FNAME_CHAR*	fname_relativePathBS(FKS_FNAME_CHAR dst[], unsigned size, const FKS_FNAME_CHAR* path, const FKS_FNAME_CHAR* currentDir)
{
	fname_relativePathSL(dst, size, path, currentDir);
	fname_slashToBackslash(dst);
	return dst;
}


/** currentDir����̑��΃p�X����.
 *	currentDir �͐�΃p�X�ł��邱��. �����łȂ��ꍇ�̋����͕s��.
 *	'\'�����΍�ŁA�Z�p���[�^��'/'�ɒu�����Ă���.
 *	�J�����g�f�B���N�g������n�܂�ꍇ�A"./"�͂��Ȃ�.
 */
FKS_LIB_DECL FKS_FNAME_CHAR*	fname_relativePathSL(FKS_FNAME_CHAR dst[], unsigned size, const FKS_FNAME_CHAR* path, const FKS_FNAME_CHAR* currentDir)
{
	enum { CHECK_MAX_PATH = sizeof(char[FNAME_MAX_PATH >= 16 ? 1 : -1]) };	// �R���p�C�����̃T�C�Y�`�F�b�N.
	enum { CHECK_MAX_URL  = sizeof(char[FNAME_MAX_URL  >= 16 ? 1 : -1]) };	// �R���p�C�����̃T�C�Y�`�F�b�N.
	FKS_FNAME_CHAR		curDir	[ FNAME_MAX_URL + 1 ];
	FKS_FNAME_CHAR		fullName[ FNAME_MAX_URL + 1 ];
	FKS_FNAME_CHAR*		cp;
	FKS_FNAME_CHAR*		cpSav;
	FKS_FNAME_CHAR*		fp;
	unsigned			cl;
	unsigned			fl;

	assert(dst != 0 && size > 2 && path != 0);
	if (dst == 0 || size <= 2 || path == 0)
		return 0;

	// �܂��A�J�����g�f�B���N�g�����t���p�X��(/) & �Ō��/��t��.
	assert(fname_isAbs(currentDir));
	fname_fullpathSL(curDir, FNAME_MAX_URL, currentDir, FKS_FNAME_C("/"));
	cp = fname_checkLastSep(curDir);
	if (cp == 0) {
		cp = curDir + fname_len(curDir);
		if (cp+1 < curDir+FNAME_MAX_URL) {
			*cp++ = FKS_FNAME_C('/');
			*cp = 0;
		}
	}

	// �Ώۂ� path ���t���p�X��. \\�͖ʓ|�Ȃ̂�/���������.
	fname_fullpathSL(fullName, FNAME_MAX_URL, path, curDir);

	// �}�b�`���Ă���f�B���N�g���������X�L�b�v.
	cp	  = fname_skipDriveRoot(curDir);
	fp	  = fname_skipDriveRoot(fullName);

	// �h���C�u��������Ă����瑊�΃p�X�����Ȃ�.
	cl	  = cp - curDir;
	fl	  = fp - fullName;
	if (cl != fl || fname_n_cmp(curDir, fullName, fl) != 0) {
		fname_cpy(dst, size, fullName);
		return dst;

	}

	// �����������`�F�b�N.
	cpSav = cp;
	while (*cp && *fp) {
		unsigned cc;
		unsigned fc;
		FKS_FNAME_GET_C(cc, cp);
		FKS_FNAME_GET_C(fc, fp);
		if (cc != fc)
			break;
		if (*cp == FKS_FNAME_C('/')) {
			cpSav = (FKS_FNAME_CHAR*)cp + 1;
		}
	}
	fp 		= fp - (cp - cpSav);
	cp 		= cpSav;

	// �J�����g�ʒu�����ւ̈ړ�����../�𐶐�.
	{
		FKS_FNAME_CHAR*	d = dst;
		FKS_FNAME_CHAR*	e = dst + size - 1;
		while (*cp) {
			if (*cp == FKS_FNAME_C('/')) {
				if (d < e) *d++ = FKS_FNAME_C('.');
				if (d < e) *d++ = FKS_FNAME_C('.');
				if (d < e) *d++ = FKS_FNAME_C('/');
			}
			++cp;
		}
		*d = FKS_FNAME_C('\0');
	}

	// �J�����g�ʒu�ȉ��̕������R�s�[
	fname_cat(dst, size, fp);

	return dst;
}


// ============================================================================
#ifdef __cplusplus
}	// fks
#endif
