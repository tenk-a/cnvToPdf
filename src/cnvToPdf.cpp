/**
 *  @file   cnvToPdf.cpp
 *  @brief  image(jpg) to pdf
 *  @author tenka@6809.net
 */
#if defined(_WIN32)
#include <windows.h>
#endif
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "ExArgv.h"
#include "jpgFileToPdf.hpp"
#include "misc.hpp"

using namespace std;
namespace fs = std::filesystem;
using namespace misc;


class App {
public:
	enum Er_t { Ok = 0, Er = 1 };

    App()
        : firstNameToTitle_(true)
        , digitCmp_(true)
        , vflag_(false)
        , dstdir_(".")
    {
    }

    Er_t usage() {
        printf("jpg2pdf [-opts] jpgfile(s)...    // v0.1c  " __DATE__ " by tenk*\n");
        printf(
            "opts:\n"
            "  -o  --output=\"FILE.pdf\"    出力するpdfファイル名\n"
            "                             指定無なら最初のjpgのディレクトリ名を-Tで採用\n"
            "  -d  --out-dir=\"DIR\"        --out指定のない場合の出力ディレクトリ名\n"
            "  -D  --digit[-]             ファイル名の数字列を数値として比較(*)  [- しない]\n"
            "  -r  --r2l                  右綴じ指定\n"
            "  -r- --l2r                  左綴じ指定(*)\n"
            "      --outline              アウトライン(頁番号一覧)を付加\n"
            "  -t  --title=\"タイトル\"     タイトル指定\n"
            "  -a  --author=\"著者\"        著者指定\n"
            "  -T  --author-title=\"著者 - タイトル\"  ' - 'を挟んで著者とタイトルを指定\n"
            "  -T  --author-title=\"[著者] タイトル\"  著者を[]で囲んで著者とタイトルを指定\n"
            "  -mN                                   頁の開き方指定(0～5).下記6つに同じ\n"
            "      --page-layout-single              0:単頁(*)\n"
            "      --page-layout-one-column          1:単頁スクロール\n"
            "      --page-layout-two-column-left     2:見開きスクロール両頁開始\n"
            "      --page-layout-two-column-right    3:見開きスクロール片頁開始\n"
            "      --page-layout-two-page-left       4:見開き両頁開始\n"
            "      --page-layout-two-page-right      5:見開き片頁開始\n"
            "  (*)付はデフォルト指定済\n"
            );
        return Er;
    }


    int main(int argc, char *argv[]) {
        if (argc < 2)
            return usage();

    	vector<string>	jpgfiles;
        JpgFileToPdf    conv;

        jpgfiles.reserve(1000);
        for (int i = 1; i < argc; ++i) {
            char* arg = argv[i];
            if (arg[0] == '-') {
                Er_t rc = scanOpts(arg);
                if (rc)
                    return rc;
            } else {
                jpgfiles.push_back(arg);
            }
        }

        if (jpgfiles.empty())
            return Ok;

        if (firstNameToTitle_ && conv_opts_.title.empty()) {
            string dir = pathToStr(fs::path(jpgfiles[0]).parent_path());
            if (!dir.empty()) {
                titleAuthor_ = dir;
                getTitleAuthor(titleAuthor_, conv_opts_.title, conv_opts_.author);
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
            outname_ = strToPath(titleAuthor_ + ".pdf");
            outname_ = dstdir_ / outname_;
        }

        sort(jpgfiles.begin(), jpgfiles.end(), FnameCmp(digitCmp_));
        if (vflag_) {
        	for (int i = 0; i < jpgfiles.size(); ++i)
        		printf("%s\n", jpgfiles[i].c_str());
        }

        return conv.run(jpgfiles, outname_.string().c_str(), conv_opts_) == false;
    }


    struct FnameCmp {
        FnameCmp(bool digit) : digit_(digit) { }
        bool operator()(string const& l, string const& r) const {
            if (digit_)
                return fnameDigCmp(l, r) < 0;
            else
                return fnameCmp(l, r) < 0;
        }
    private:
        bool digit_;
    };

private:
    Er_t scanOpts(char* arg) {
        char* a = arg;
        assert(a && *a == '-');
        if (checkOpt(a, "--outline")) {
            conv_opts_.outline = (*a != '-');
        } else if (checkOpt(a, "--output") || checkOpt(a, "-o")) {
			if (!*a) return optErr(arg);
            outname_ = strToPath(a);
        } else if (checkOpt(a, "--digit") || checkOpt(a, "-D")) {
            digitCmp_ = (*a != '-');
        } else if (checkOpt(a, "--out-dir") || checkOpt(a, "-d")) {
            if (!*a) return optErr(arg);
            dstdir_ = strToPath(removeLastDirSep(a));
        } else if (checkOpt(a, "--title-author") || checkOpt(a, "-T")) {
            if (!*a) return optErr(arg);
            titleAuthor_ = a;
            getTitleAuthor(titleAuthor_, conv_opts_.title, conv_opts_.author);
            firstNameToTitle_ = false;
        } else if (checkOpt(a, "--title") || checkOpt(a, "-t")) {
            if (!*a) return optErr(arg);
            conv_opts_.title = a;
            firstNameToTitle_ = false;
		} else if (checkOpt(a, "--author") || checkOpt(a, "-a")) {
            if (!*a) return optErr(arg);
            conv_opts_.author = a;
            firstNameToTitle_ = false;
        } else if (checkOpt(a, "-r2l")) {
            conv_opts_.r2l = true;
        } else if (checkOpt(a, "-l2r")) {
            conv_opts_.r2l = false;
        } else if (checkOpt(a, "-r")) {
            conv_opts_.r2l = (a[0] != '-');
        } else if (checkOpt(a, "-m")) {
            if (!*a) return optErr(arg);
                _HPDF_PageLayout mode = _HPDF_PageLayout(strtoul(a, NULL, 10));
            if (HPDF_PAGE_LAYOUT_SINGLE <= mode && mode < HPDF_PAGE_LAYOUT_EOF) {
                conv_opts_.layout_mode = _HPDF_PageLayout(mode);
            } else {
                fprintf(stderr, "option %s : out of range", arg);
                return Er;
            }
        } else if (checkOpt(a, "--page-layout-single")) {
            conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_SINGLE;
        } else if (checkOpt(a, "--page-layout-one-column")) {
            conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_ONE_COLUMN;
        } else if (checkOpt(a, "--page-layout-two-column-left")) {
            conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_TWO_COLUMN_LEFT;
        } else if (checkOpt(a, "--page-layout-two-column-right")) {
            conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_TWO_COLUMN_RIGHT;
        } else if (checkOpt(a, "--page-layout-two-page-left")) {
            conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_TWO_PAGE_LEFT;
        } else if (checkOpt(a, "--page-layout-two-page-right")) {
            conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_TWO_PAGE_RIGHT;
        } else if (checkOpt(a, "-v")) {
            vflag_ = (*a != '-');
        } else if (checkOpt(a, "-h") || checkOpt(a, "-?")) {
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

	static bool checkOpt(char*& arg, string_view opt) {
		if (strStartsWith(string_view(arg), opt)) {
			arg += opt.size();
			if (*arg == '=')
				++arg;
			return true;
		}
		return false;
	}

    static void getTitleAuthor(string& str, string& rTitle, string& rAuthor) {
        char const* s = str.data();
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

private:
    JpgFileToPdf::Opts  conv_opts_;
    fs::path    		outname_;
    fs::path    		dstdir_;
    string				titleAuthor_;
    bool        		firstNameToTitle_;
    bool        		digitCmp_;
    bool                vflag_;
};


#if !defined(_WIN32)
int main(int argc, char *argv[]) {
    ExArgv_conv(&argc, &argv);
    return App().main(argc, argv);
}
#else
int wmain(int argc, wchar_t *argv[]) {
	namespace fs = filesystem;
	auto savCP = GetConsoleOutputCP();
	SetConsoleOutputCP(65001);
    ExArgv_conv(&argc, &argv);
    vector<char*>       argv2(argc);
    vector<string> args;
    args.reserve(argc);
	for (size_t i = 0; i < argc; ++i) {
		args.emplace_back(pathToStr(fs::path(argv[i])));
		argv2[i] = const_cast<char*>(args.back().c_str());
	}
	int rc = App().main(argc, &argv2[0]);
	SetConsoleOutputCP(savCP);
	return rc;
}
#endif
