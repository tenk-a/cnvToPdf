/**
 *	@file	misc.hpp
 */
#ifndef MISC_INCLUDED__
#define MISC_INCLUDED__

#include <string>
#include <vector>
#include <fstream>
#include <type_traits>

namespace misc {

	// Instead of sts::make_unsigned
	template<std::size_t N> struct nbytes_to_uchar {};
 #if __cplusplus < 202002L
	template<> struct 		nbytes_to_uchar<1> { typedef unsigned char type; };
 #else
	template<> struct 		nbytes_to_uchar<1> { typedef char8_t  type; };
 #endif
	template<> struct 		nbytes_to_uchar<2> { typedef char16_t type; };
	template<> struct 		nbytes_to_uchar<4> { typedef char32_t type; };
	template<typename C> struct char_to_uchar { typedef typename nbytes_to_uchar<sizeof(C)>::type type; };

	template<typename C> inline std::size_t
	strLen(C const* s) {
		return std::char_traits<C>::length(s);
	}

	// skip space
	template<class C> C*
	skipSpc(C* src) {
		typedef typename char_to_uchar<C>::type UC;
		UC* s = (UC*)(src);
		while ((*s && *s <= UC(' ')) || *s == UC(0x7f))
			++s;
		return (C*)(s);
	}
	template<class C> C*
	skipSpc(C* src, C* end) {
		typedef typename char_to_uchar<C>::type UC;
		UC* s = (UC*)(src);
		while (s < end && (*s <= UC(' ') || *s == UC(0x7f)))
			++s;
		return (C*)(s);
	}

	template<typename C> std::basic_string<C>
	addDq(C const* s, std::size_t len) {
		if (len && *s == C('"'))
			return std::basic_string<C>(s, len);
		std::basic_string<C> d;
		d.reserve(len + 2);
		d += C('"');
		d.append(s, len);
		d += C('"');
		return d;
	}
	template<typename C> std::basic_string<C>
	addDq(C const* s) { return addDq(s, strLen(s)); }
	template<typename STR> std::basic_string<typename STR::value_type>
	addDq(STR const& s) { return addDq(s.data(), s.size()); }

	template<typename C> std::basic_string<C>
	subDq(C const* s, std::size_t len) {
		if (len > 0 && *s == C('"')) {
			--len;
			++s;
			if (len > 0 && s[len-1] == C('"'))
				--len;
		}
		return std::basic_string<C>(s, len);
	}
	template<typename C> std::basic_string<C>
	subDq(C const* s) { return subDq(s, strLen(s)); }
	template<typename STR> std::basic_string<typename STR::value_type>
	subDq(STR const& s) { return subDq(s.data(), s.size()); }


	template<typename C> std::basic_string<C>
	trim(C const* str, std::size_t len) {
		typedef typename char_to_uchar<C>::type UC;
		UC const* b = static_cast<UC const*>(str);
		UC const* e = b + len;
		UC const* p = b;
		while (p < e && *p && (*p <= 0x20u || *p == 0x7fu))
			++p;
		if (p < e) {
			UC const* q = e - 1;
			while (p < q && *q && (*q <= 0x20u || *q == 0x7fu))
				--q;
			if (p < q)
				return std::basic_string<C>(static_cast<char const*>(p), q+1 - p);
		}
		return std::basic_string<C>();
	}
	template<typename C> std::basic_string<C>
	trim(C const* s) { return trim(s, strLen(s)); }
	template<typename STR> std::basic_string<typename STR::value_type>
	trim(STR const& s) { return trim(s.data(), s.size()); }


	template<class ITE, class VVS> void
	csvParse(ITE b, ITE e, VVS& rResult, char sep=',') {
		typedef typename nbytes_to_uchar<sizeof(*b)>::type UC;
	    rResult.clear();
	    rResult.emplace_back();
	    bool df = false;
	    ITE  s  = b;
	    while (s != e) {
	        UC c = UC( *s++ );
	        if (c == UC('"')) {
	            df = !df;
	        } else if (!df && (c == UC(sep) || c == UC('\n') || c == UC('\r'))) {
	            std::size_t l = s-1 - b;
	            if (l)
	                rResult.back().emplace_back(&*b, l);
	            if (c == UC('\r') && *s == UC('\n')) {
	                c = UC('\n');
	                ++s;
	            }
	            b = s;
	            if (c == UC('\n'))
	                rResult.emplace_back();
	        }
	    }
	    if (s != b)
	        rResult.back().emplace_back(&*b, s - b);
	    if (rResult.back().empty())
	        rResult.pop_back();
	}

	template<class STR, class VVS>
	void csvParse(STR const& s, VVS& rResult, char sep=',') {
	    csvParse(&*s.begin(), &*s.end(), rResult, sep);
	}


	template<typename T, typename C>	// C: char,char8_t,char16_t,char32_t
	T digStrToUint(C const* bgn, C const* end = nullptr, C const** pPos = nullptr) {
		if (!end)
			end = bgn + strLen(bgn);
		T  val = 0; 
		C const* s;
		for (s = bgn; s != end; ++s) {
			C c = *s;
			if (c < C('0') || c > C('9'))
				break;
			val = val * 10 + (c - C('0'));
		}
		if (pPos)
			*pPos = s;
		return val;
	}

	/// String compare.
	/// @param C		char,char8_t,char16_t,char32_t
	/// @param flags    bit0:Ignore str case  bit1:Identifies \ and /  bit2:numerically
	template<typename C, unsigned int flags> int
	strCmpEx(C const* l, std::size_t llen, C const* r, std::size_t rlen, std::size_t len=std::size_t(-1)) {
		typedef typename char_to_uchar<C>::type UC;
		enum { asc_icase = 1, sep = 2, dig = 4, };
		if (!l)
			return r != nullptr;
		else if (!r)
			return -1;
		std::ptrdiff_t cc   = llen - rlen;
		std::size_t    maxl = (cc < 0) ? llen : rlen;
		if (maxl >= len) {
		    maxl = len;
		    cc   = 0;
		}
		UC const* lp = (UC const*)(l);
		UC const* rp = (UC const*)(r);
		UC const* le = lp + maxl;
		UC const* re = rp + maxl;

	    while (lp < le && rp < re) {
	        UC lc = *lp++;
	        UC rc = *rp++;

			if (flags & sep) {
		        if ((lc == UC('/') && rc == UC('\\')) || (lc == UC('\\') && rc == UC('/')))
		            continue;
			}
			if (flags & asc_icase) {
				lc = (lc >= UC('A') && lc <= UC('Z')) ? (lc - UC('A') + UC('a')) : lc;
				rc = (rc >= UC('A') && rc <= UC('Z')) ? (rc - UC('A') + UC('a')) : rc;
			}

	        if ((flags & dig) && (UC('0') <= lc && lc <= UC('9')) && (UC('0') <= rc && rc <= UC('9'))) {
	            std::uintmax_t lv = digStrToUint<std::uintmax_t>(lp-1, le, &lp);
	            std::uintmax_t rv = digStrToUint<std::uintmax_t>(rp-1, re, &rp);
	            if (lv != rv)
	            	return (lv < rv) ? -1 : 1;
	        } else {
		        if (lc != rc)
		        	return (lc < rc) ? -1 : 1;
		    }
	    }
	    return (cc < 0) ? -1 : (cc > 0) ? 1 : 0;
	}


	template<typename STR> bool
	strStartsWith(STR const& l, STR const& r) {
		return strCmpEx<typename STR::value_type,(0|0|0)>(l.data(), l.size(), r.data(), r.size(), r.size()) == 0;
	}
	template<typename C> bool
	strStartsWith(C const* l, C const* r) {
		std::size_t rlen = strLen(r);
		return strCmpEx<C, (0|0|0)>(l, strLen(l), r, rlen, rlen) == 0;
	}


	template<typename STR> int
	strCaseCmp(STR const& l, STR const& r, std::size_t len=std::size_t(-1)) {
		return strCmpEx<typename STR::value_type,(0|0|1)>(l.data(), l.size(), r.data(), r.size(), len);
	}
	template<typename C> int
	strCaseCmp(C const* l, C const* r, std::size_t len=std::size_t(-1)) {
		return strCmpEx<C,(0|0|1)>(l, strLen(l), r, strLen(r), len);
	}


	template<typename STR> bool
	strCaseStartsWith(STR const& l, STR const& r) {
		return strCmpEx<typename STR::value_type, (0|0|1)>(l.data(), l.size(), r.data(), r.size(), r.size()) == 0;
	}
	template<typename C> bool
	strCaseStartsWith(C const* l, C const* r) {
		std::size_t rlen = strLen(r);
		return strCmpEx<C,(0|0|1)>(l, strLen(l), r, rlen, rlen) == 0;
	}


	template<typename C> int
	fnameCmp(C const* lp, std::size_t llen, C const* rp, std::size_t rlen, std::size_t len=std::size_t(-1)) {
	  #if defined(_WIN32)
		return strCmpEx<C,(0|2|1)>(lp,llen,rp,rlen,len);
	  #elif defined(APPLE)
		return strCmpEx<C,(0|0|1)>(lp,llen,rp,rlen,len);
	  #else
		return strCmpEx<C,(0|0|0)>(lp,llen,rp,rlen,len);
	  #endif
	}
	template<typename STR> int
	fnameCmp(STR const& l, STR const& r, std::size_t len=std::size_t(-1)) {
		return fnameCmp(l.data(), l.size(), r.data(), r.size(), len);
	}
	template<typename C> int
	fnameCmp(C const* l, C const* r, std::size_t len=std::size_t(-1)) {
		return fnameCmp(l, strLen(l), r, strLen(r), len);
	}


	template<typename C> int
	fnameDigCmp(C const* lp, std::size_t llen, C const* rp, std::size_t rlen, std::size_t len=std::size_t(-1)) {
	  #if defined(_WIN32)
		return strCmpEx<C,(4|2|1)>(lp,llen,rp,rlen,len);
	  #elif defined(APPLE)
		return strCmpEx<C,(4|0|1)>(lp,llen,rp,rlen,len);
	  #else
		return strCmpEx<C,(4|0|0)>(lp,llen,rp,rlen,len);
	  #endif
	}
	template<typename STR> int
	fnameDigCmp(STR const& l, STR const& r, std::size_t len=std::size_t(-1)) {
		return fnameDigCmp(l.data(), l.size(), r.data(), r.size(), len);
	}
	template<typename C> int
	fnameDigCmp(C const* l, C const* r, std::size_t len=std::size_t(-1)) {
		return fnameDigCmp(l, strLen(l), r, strLen(r), len);
	}

	template<typename C> std::basic_string_view<C>
	removeLastDirSep(C const* dir, std::size_t len) {
		if (len > 0) {
			C c = dir[len-1];
			if (c == C('/') || c == C('\\'))
				--len;
		}
		return std::basic_string_view<C>(dir, len);
	}
	template<typename C> inline std::basic_string_view<C>
	removeLastDirSep(C const* dir) {
		return removeLastDirSep(dir, strLen(dir));
	}
	template<typename STR> inline std::basic_string_view<typename STR::value_type>
	removeLastDirSep(STR const& dir) {
		return removeLastDirSep(dir.data(), dir.size());
	}

}	// misc



#if 1 //defined(__has_include) && __has_include(<filesystem>)
#include <filesystem>

namespace misc {

	inline std::string pathToStr(std::filesystem::path const& fpath) {
	 #if defined _WIN32
	 	return std::string((char const*)(fpath.u8string().c_str()));
	 	//return reinterpret_cast<std::string const&&>(fpath.u8string());
	 #else
	 	return fpath.string();
	 #endif
	}


 #if defined _WIN32
	inline std::filesystem::path strToPath(std::string_view const& s) {
	  #if __cplusplus < 202002L
	 	return std::filesystem::u8path(s);
	  #else
	 	return std::filesystem::path(std::basic_string_view<char8_t>((char8_t const*)s.data(), s.size()));
	  #endif
	}
	template<typename STR, typename = std::enable_if< (sizeof(typename STR::value_type) > 1) >::type >
	inline std::filesystem::path strToPath(STR const& s) {
	 	return std::filesystem::path(s);
	}
 #else
	template<typename STR>
	inline std::filesystem::path strToPath(STR const& s) {
		return std::filesystem::path(s);
	}
 #endif

	inline std::uintmax_t getFileBytes(std::filesystem::path const& fpath) {
		if (std::filesystem::exists(fpath))
			return  std::filesystem::file_size(fpath);
		else
			return 0;
	}

}	// misc
#endif


namespace misc {

	template<typename STR, typename VEC>
	bool fileLoad(STR const& fname, VEC& dst, std::size_t max_size = 0) {
		dst.clear();
		std::ifstream ist(fname, std::ios::binary | std::ios::ate);
		if (ist.is_open()) {
			std::size_t elem_bytes= sizeof(dst[0]);
			std::size_t bytes 	  = std::size_t(ist.tellg());
			if (max_size && bytes > max_size * elem_bytes)
				bytes = max_size * elem_bytes;
			std::size_t	elem_size = (bytes + elem_bytes - 1) / elem_bytes;
			dst.resize(elem_size + 1);
			dst.resize(elem_size);
			if (elem_size) {
				ist.seekg(0, std::ifstream::beg);
				ist.read(reinterpret_cast<char*>(&dst[0]), bytes);
			}
			return true;
		}
		return false;
	}

	template<typename STR, typename VEC>
	bool fileSave(STR const& fname, void const* src, std::size_t src_bytes, std::size_t max_bytes = 0) {
		std::ofstream ost(fname, std::ios::binary | std::ios::out);
		if (ost.is_open()) {
			std::size_t sz = (max_bytes && sz > max_bytes) ? max_bytes : src_bytes;
			if (sz)
				ost.write(reinterpret_cast<char const*>(src), sz);
			return !ost.bad();
		}
		return false;
	}
	template<typename STR, typename VEC>
	bool fileSave(STR const& fname, VEC const& src, std::size_t max_size = 0) {
		enum { unit = sizeof(*src.begin()) };
		return fileSave(fname, &*src.begin(), src.size() * unit, max_size * unit);
	}

}	// misc

#endif
