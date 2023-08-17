/**
 *  @file   cnvToPdf.cpp
 *  @brief  image(jpg) to pdf
 *  @author tenka@6809.net
 */
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "ExArgv.h"
#include "fks_fname.h"
#include "JpgFileToPdf.hpp"
#if defined(_WIN32)
#include <windows.h>
#endif

#ifdef _MSC_VER
 #define STR_CASE_CMP(l,r)		_stricmp((l),(r))
 #define STR_N_CASE_CMP(l,r,n)	_strnicmp((l),(r),(n))
#else
 #define STR_CASE_CMP(l,r)		strcasecmp((l),(r))
 #define STR_N_CASE_CMP(l,r,n)	strncasecmp((l),(r),(n))
#endif

using namespace std;
namespace fs = std::filesystem;

class App {
public:
    typedef std::vector<std::string>  VecStr;
	enum Er_t { Ok = 0, Er = 1 };

    App()
        : firstNameToTitle_(true)
        , digitCmp_(true)
    {
    }

    Er_t usage() {
        printf("jpg2pdf [-opts] jpgfile(s)...    // v0.1c  " __DATE__ " by tenk*\n");
        printf(
            "opts:\n"
            "  -o  --output\"FILE.pdf\"    出力するpdfファイル名\n"
            "                            指定無なら最初のjpgのディレクトリ名を-Tで採用\n"
            "  -d  --out-dir\"DIR\"        --out指定のない場合の出力ディレクトリ名\n"
            "      --digit[-]            ファイル名の数字列を数値として比較(*)  [- しない]\n"
            "  -r  --r2l                 右綴じ指定\n"
            "  -r- --l2r                 左綴じ指定(*)\n"
            "      --outline             アウトライン(頁番号一覧)を付加\n"
            "  -t  --title\"タイトル\"     タイトル指定\n"
            "  -a  --author\"著者\"        著者指定\n"
            "  -T  --author-title\"著者 - タイトル\"  ' - 'を挟んで著者とタイトルを指定\n"
            "  -T  --author-title\"[著者] タイトル\"  著者を[]で囲んで著者とタイトルを指定\n"
            "  -mN                                  頁の開き方指定(0～5).下記6つに同じ\n"
            "      --page-layout-single             0:単頁(*)\n"
            "      --page-layout-one-column         1:単頁スクロール\n"
            "      --page-layout-two-column-left    2:見開きスクロール両頁開始\n"
            "      --page-layout-two-column-right   3:見開きスクロール片頁開始\n"
            "      --page-layout-two-page-left      4:見開き両頁開始\n"
            "      --page-layout-two-page-right     5:見開き片頁開始\n"
            "  (*)付はデフォルト指定済\n"
            );
        return Er;
    }


    Er_t main(int argc, char *argv[]) {
        if (argc < 2)
            return usage();

        VecStr          jpgfiles;
        JpgFileToPdf    conv;

        jpgfiles.reserve(1000);
        for (int i = 1; i < argc; ++i) {
            char* p = argv[i];
            if (*p == '-') {
                Er_t rc = scanOpts(p);
                if (rc)
                    return rc;
            } else {
                jpgfiles.push_back(p);
            }
        }

        if (jpgfiles.empty())
            return Ok;

        if (firstNameToTitle_ && conv_opts_.title.empty()) {
            std::string dir = getPathToDir(jpgfiles[0].c_str());
            if (!dir.empty()) {
                getTitleAuthor(dir.c_str(), conv_opts_.title, conv_opts_.author);
                titleAuthor_ = dir;
            }
        }
        if (outname_.empty()) {
            if (titleAuthor_.empty()) {
                if (conv_opts_.author.empty()) {
                    if (conv_opts_.title.empty())
                        titleAuthor_ = "a";
                    else
                        titleAuthor_ = conv_opts_.title;
                } else {
                    titleAuthor_ = "[" + conv_opts_.author + "] " + conv_opts_.title;
                }
            }
            outname_ = dstdir_ + "/" + titleAuthor_ + ".pdf";
        }

        std::sort(jpgfiles.begin(), jpgfiles.end(), FnameCmp(digitCmp_));
        //for (int i = 0; i < jpgfiles.size(); ++i) printf("%s\n", jpgfiles[i].c_str());

        return Er_t(conv.run(jpgfiles, outname_.c_str(), conv_opts_) == false);
    }


    struct FnameCmp {
        FnameCmp(bool digit) : digit_(digit) { }
        bool operator()(std::string const& l, std::string const& r) const {
            if (digit_)
                return fks_fnameDigitCmp(l.c_str(), r.c_str()) < 0;
            else
                return fks_fnameCmp(l.c_str(), r.c_str()) < 0;
        }
    private:
        bool digit_;
    };

private:
    Er_t scanOpts(char* arg) {
        char* p = arg;
        assert(p && *p == '-');
		if (checkOpt(p, "-output") || checkOpt(p, "-o")) {
			if (!*p) return optErr(arg);
            outname_ = p;
        } else if (checkOpt(p, "-out-dir") || checkOpt(p, "-d")) {
            if (!*p) return optErr(arg);
            fks_fnameDelLastSep(p);
            dstdir_ = p;
        } else if (checkOpt(p, "-title") || checkOpt(p, "-t")) {
            if (!*p) return optErr(arg);
            conv_opts_.title = p;
            firstNameToTitle_ = false;
		} else if (checkOpt(p, "-author") || checkOpt(p, "-a")) {
            if (!*p) return optErr(arg);
            conv_opts_.author = p;
            firstNameToTitle_ = false;
        } else if (checkOpt(p, "-title-author") || checkOpt(p, "-T")) {
            if (!*p) return optErr(arg);
            titleAuthor_ = p;
            getTitleAuthor(p, conv_opts_.title, conv_opts_.author);
            firstNameToTitle_ = false;
        } else if (checkOpt(p, "-r2l")) {
            conv_opts_.r2l = true;
        } else if (checkOpt(p, "-l2r")) {
            conv_opts_.r2l = false;
        } else if (checkOpt(p, "-digit")) {
            digitCmp_ = (*p != '-');
        } else if (checkOpt(p, "-outline")) {
            conv_opts_.outline = (*p != '-');
        } else if (checkOpt(p, "-m")) {
            if (!*p) return optErr(arg);
                _HPDF_PageLayout mode = _HPDF_PageLayout(strtoul(p, NULL, 10));
            if (HPDF_PAGE_LAYOUT_SINGLE <= mode && mode < HPDF_PAGE_LAYOUT_EOF) {
                conv_opts_.layout_mode = _HPDF_PageLayout(mode);
            } else {
                fprintf(stderr, "option %s : out of range", arg);
                return Er;
            }
        } else if (checkOpt(p, "-page-layout-single")) {
            conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_SINGLE;
        } else if (checkOpt(p, "-page-layout-one-column")) {
            conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_ONE_COLUMN;
        } else if (checkOpt(p, "-page-layout-two-column-left")) {
            conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_TWO_COLUMN_LEFT;
        } else if (checkOpt(p, "-page-layout-two-column-right")) {
            conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_TWO_COLUMN_RIGHT;
        } else if (checkOpt(p, "-page-layout-two-page-left")) {
            conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_TWO_PAGE_LEFT;
        } else if (checkOpt(p, "-page-layout-two-page-right")) {
            conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_TWO_PAGE_RIGHT;
        } else if (checkOpt(p, "-r")) {
            conv_opts_.r2l = (*p != '-');
        } else if (checkOpt(p, "-h") || checkOpt(p, "-?")) {
            return usage();
		} else {
            return optErr(arg);
        }
        return Ok;
    }

	static Er_t optErr(char const* arg) {
        fprintf(stderr, "Incorrect option: %s", arg);
		return Er;
	}

    static void getTitleAuthor(char const* s, std::string& rTitle, std::string& rAuthor) {
        if (*s == '[') {
            char const* t = strchr(s+1, ']');
            if (t) {
                rAuthor.assign(s + 1, t);
                ++t;
                while (*t == ' ')
                    ++t;
                rTitle.assign(t);
                return;
            }
        }
        char const* t = strstr(s, " - ");
        if (t) {
            rAuthor.assign(s, t);
            t += 3;
            while (*t == ' ')
                ++t;
            rTitle.assign(t);
            return;
        }
        rTitle = s;
    }

    static std::string getPathToDir(char const* src) {
        char const* base = fks_fnameBaseName(src);
        if (src < base) {
            --base;
            std::string dirPath(src, base);
            char const* dir = fks_fnameBaseName(dirPath.c_str());
            if (dir < base)
                return std::string(dir, base);
        }
        return std::string();
    }

	static bool strEqu(char const* l, char const* r) {
		if (!l || !r)
			return l == r;
		return STR_CASE_CMP(l, r) == 0;
	}

	static bool strStartsWith(char const* src, char const* last) {
		if (!src || !last)
			return false;
		auto sl = strlen(src);
		auto ll = strlen(last);
		if (sl < ll)
			return false;
		return STR_N_CASE_CMP(src, last, ll) == 0;
	}
	static bool strStartsWith(string const& src, char const* last) {
		return strStartsWith(src.c_str(), last);
	}

	static bool strEndsWith(char const* src, char const* last) {
		if (!src || !last)
			return false;
		auto sl = strlen(src);
		auto ll = strlen(last);
		if (sl < ll)
			return false;
		auto s2 = src + (sl - ll);
		return STR_CASE_CMP(s2, last) == 0;
	}
	static bool strEndsWith(string const& src, char const* last) {
		return strEndsWith(src.c_str(), last);
	}

	static char const* fileBaseName(char const* s) {
		char const* r1 = strrchr(s, '/');
		char const* r2 = strrchr(s, '\\');
		char const* r  = r1;
		if (r1 < r2)
			r = r2;
		if (r)
			++r;
		else
			r = s;
		return r;
	}

	static char const* fnameExt(char const* pathname) {
		auto b        = fileBaseName(pathname);
		char const* r = strrchr(b, '.');
		if (r)
			; //++r;
		else
			r = "";
		return r;
	}

	static string getPathDir(char const* pathname) {
		auto e = fileBaseName(pathname);
		if (e <= pathname)
			return string();
		--e;
		return string(pathname, e);
	}

	static string fnameRemoveExt(char const* pathname) {
		auto b        = fileBaseName(pathname);
		char const* r = strrchr(b, '.');
		if (r) {
			return string(pathname).substr(0, r - pathname);
		}
		else
			return string(pathname);
	}

	static uintmax_t getFileBytes(fs::path fname) {
		if (fs::exists(fname))
			return  fs::file_size(fname);
		else
			return 0;
	}

	static string removeLastDirSep(string dir) {
		string d = dir;
		if (d.size() && (d.back() == '/' || d.back() == '\\'))
			d.pop_back();
		return d;
	}

	static string toMsSep(string dpath) {
		for (auto& it : dpath) {
			if (it == '/')
				it = '\\';
		}
		return dpath;
	}

	static string addDq(string const& s) {
		if (!s.empty() && s[0] == '"')
			return s;
		return '"' + s + '"';
	}

	static string subDq(string const& arg) {
		string s = arg;
		if (s.empty() || s.front() != '"')
			return s;
		if (s.back() == '"')
			s.pop_back();
		s.erase(s.begin());
		for (auto& it : s) {
			if (it == '\n' || it == '\t' || it == ',')
				it = ' ';
		}
		return s;
	}

	static bool strContain(string& s, string_view key) {
	   return s.find(key) != string::npos;
	}

	static bool strContain(char const* s, char const* key) {
	   return strstr(s, key) != nullptr;
	}

	static string trim(string_view str) {
		auto b = (char8_t const*)str.data();
		auto e = b + str.size();
		auto p = b;
		while (*p && *p < 0x20 && p < e)
			++p;
		if (p < e) {
			auto q = e-1;
			while (p < q && *q && *q < ' ')
				--q;

			if (p < q)
				return string((char const*)p, q+1 - p);
		}
		return string();
	}

	static bool startsWithGetLen(string const& label, char const* name, size_t& n) {
		if (label.starts_with(name)) {
			n = strlen(name);
			return true;
		}
		return false;
	}

	static bool checkOpt(char*& arg, char const* opt) {
		if (strStartsWith(arg, opt)) {
			arg += strlen(opt);
			if (*arg == '=')
				++arg;
			return true;
		}
		return false;
	}

	static void getResFile(char const* fname, list<string>& args) {
		ifstream ifs(fs::path((char8_t const*)fname), std::ios::in);
		if (ifs.is_open()) {
			while (!ifs.eof()) {
				string line;
				if (getline(ifs, line)) {
					line = trim(line);
					if (line.empty() || line[0] == '#')
						continue;
					args.emplace_back(line);
				}
			}
			if (!args.empty() && args.back().empty()) {
				args.pop_back();
			}
		}
	}

	static bool loadFile(fs::path fname, vector<uint8_t>& dst, size_t sz = 0) {
		if (sz == 0)
			sz = getFileBytes(fname);
		if (sz == 0 || sz == size_t(-1ll))
			return false;
	 #if 0
		FILE* fp = _wfopen(fname.wstring().c_str(), L"rb");
		if (fp == nullptr)
			return false;
		dst.resize(sz);
		auto rsz = fread(dst.data(), 1, sz, fp);
		fclose(fp);
		return sz == size_t(rsz);
	 #else
		ifstream ist(fname.c_str(), std::ios::binary | std::ios::in);
		if (!ist.is_open())
			return false;
		dst.resize(sz);
		ist.read((char*)dst.data(), sz);
		return true;
	 #endif
	}

private:
    JpgFileToPdf::Opts  conv_opts_;
    std::string         outname_;
    std::string         dstdir_;
    std::string         titleAuthor_;
    bool                firstNameToTitle_;
    bool                digitCmp_;
};


#if !defined(_WIN32)
int main(int argc, char *argv[]) {
    ExArgv_conv(&argc, &argv);
    return App().main(argc, argv);
}
#else
int wmain(int argc, wchar_t *argv[]) {
	namespace fs = std::filesystem;
	auto savCP = GetConsoleOutputCP();
	SetConsoleOutputCP(65001);
    ExArgv_conv(&argc, &argv);
    std::vector<char*>       argv2(argc);
    std::vector<std::string> args;
    args.reserve(argc);
	for (std::size_t i = 0; i < argc; ++i) {
		args.emplace_back((char const*)fs::path(argv[i]).u8string().c_str());
		argv2[i] = const_cast<char*>(args.back().c_str());
	}
	int rc = App().main(argc, &argv2[0]);
	SetConsoleOutputCP(savCP);
	return rc;
}
#endif
