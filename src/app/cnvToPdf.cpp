/**
 *	@file	cnvToPdf.cpp
 *	@brief	image(jpg) to pdf
 *	@author	tenka@6809.net
 */
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <assert.h>
#include <misc/ExArgv.h>
#include <misc/fname.h>
#include "JpgFileToPdf.hpp"


class App {
public:
	typedef std::vector<std::string>  VecStr;

	App()
		: firstNameToTitle_(true)
		, digitCmp_(true)
	{
	}

	int usage() {
		printf("jpg2pdf [-opts] jpgfile(s)...    // " __DATE__ " by tenk*\n");
		printf(
			"opts:\n"
			"  -o  --output\"FILE.pdf\"    �o�͂���pdf�t�@�C����\n"
			"                            �w�薳�Ȃ�ŏ���jpg�̃f�B���N�g������-T�ō̗p\n"
			"  -d  --out-dir\"DIR\"        --out�w��̂Ȃ��ꍇ�̏o�̓f�B���N�g����\n"
			"      --digit[-]            �t�@�C�����̐�����𐔒l�Ƃ��Ĕ�r(*)  [- ���Ȃ�]\n"
			"  -r  --r2l                 �E�Ԃ��w��\n"
			"  -r- --l2r                 ���Ԃ��w��(*)\n"
			"      --outline             �A�E�g���C��(�Ŕԍ��ꗗ)��t��\n"
			"  -t  --title\"�^�C�g��\"     �^�C�g���w��\n"
			"  -a  --author\"����\"        ���Ҏw��\n"
			"  -T  --author-title\"���� - �^�C�g��\"  ' - '������Œ��҂ƃ^�C�g�����w��\n"
			"  -T  --author-title\"[����] �^�C�g��\"  ���҂�[]�ň͂�Œ��҂ƃ^�C�g�����w��\n"
			"  -mN                                  �ł̊J�����w��(0�`5).���L6�ɓ���\n"
			"      --page-layout-single             0:�P��(*)\n"
			"      --page-layout-one-column         1:�P�ŃX�N���[��\n"
			"      --page-layout-two-column-left    2:���J���X�N���[�����ŊJ�n\n"
			"      --page-layout-two-column-right   3:���J���X�N���[���ЕŊJ�n\n"
			"      --page-layout-two-page-left      4:���J�����ŊJ�n\n"
			"      --page-layout-two-page-right     5:���J���ЕŊJ�n\n"
			"  (*)�t�̓f�t�H���g�w���\n"
			);
		return 1;
	}


	int main(int argc, char *argv[]) {
		if (argc < 2)
			return usage();

		ExArgv_conv(&argc, &argv);

		VecStr			jpgfiles;
		JpgFileToPdf	conv;

		jpgfiles.reserve(1000);
		for (int i = 1; i < argc; ++i) {
			char* p = argv[i];
			if (*p == '-') {
				int rc = scanOpts(p);
				if (rc)
					return rc;
			} else {
				jpgfiles.push_back(p);
			}
		}

		if (jpgfiles.empty())
			return 0;

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

		return conv.run(jpgfiles, outname_.c_str(), conv_opts_) == false;
	}


	struct FnameCmp {
		FnameCmp(bool digit) : digit_(digit) { }
		bool operator()(std::string const& l, std::string const& r) const {
			if (digit_)
				return fks::fname_digit_cmp(l.c_str(), r.c_str(), unsigned(-1)) < 0;
			else
				return fks::fname_cmp(l.c_str(), r.c_str()) < 0;
		}
	private:
		bool digit_;
	};

private:
	int scanOpts(char* arg) {
		char* p = arg;
		assert(p && *p == '-');
		++p;
		if (*p == '-') {
			++p;
			std::string para = p;
			if (paramEquLong(p, "title", p)) {
				if (!*p) goto OPT_ERR;
				conv_opts_.title = p;
				firstNameToTitle_ = false;
			} else if (paramEquLong(p, "author", p)) {
				if (!*p) goto OPT_ERR;
				conv_opts_.author = p;
				firstNameToTitle_ = false;
			} else if (paramEquLong(p, "title-author", p)) {
				if (!*p) goto OPT_ERR;
				titleAuthor_ = p;
				getTitleAuthor(p, conv_opts_.title, conv_opts_.author);
				firstNameToTitle_ = false;
			} else if (paramEquLong(p, "r2l", p)) {
				conv_opts_.r2l = true;
			} else if (paramEquLong(p, "l2r", p)) {
				conv_opts_.r2l = false;
			} else if (paramEquLong(p, "digit", p)) {
				digitCmp_ = (*p != '-');
			} else if (paramEquLong(p, "outline", p)) {
				conv_opts_.outline = (*p != '-');
			} else if (paramEquLong(p, "output", p)) {
				if (!*p) goto OPT_ERR;
				outname_ = p;
			} else if (paramEquLong(p, "out-dir", p)) {
				if (!*p) goto OPT_ERR;
				fks::fname_delLastSep(p);
				dstdir_ = p;
			} else if (para == "page-layout-single") {
				conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_SINGLE;
			} else if (para == "page-layout-one-column") {
				conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_ONE_COLUMN;
			} else if (para == "page-layout-two-column-left") {
				conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_TWO_COLUMN_LEFT;
			} else if (para == "page-layout-two-column-right") {
				conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_TWO_COLUMN_RIGHT;
			} else if (para == "page-layout-two-page-left") {
				conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_TWO_PAGE_LEFT;
			} else if (para == "page-layout-two-page-right") {
				conv_opts_.layout_mode = HPDF_PAGE_LAYOUT_TWO_PAGE_RIGHT;
			}
		} else {
			switch (*p++) {
			case 'o':
				if (!*p) goto OPT_ERR;
				outname_ = p;
				break;
			case 'd':
				if (!*p) goto OPT_ERR;
				fks::fname_delLastSep(p);
				dstdir_ = p;
				break;
			case 't':
				if (!*p) goto OPT_ERR;
				conv_opts_.title = p;
				firstNameToTitle_ = false;
				break;
			case 'a':
				if (!*p) goto OPT_ERR;
				conv_opts_.author = p;
				firstNameToTitle_ = false;
				break;
			case 'T':
				if (!*p) goto OPT_ERR;
				titleAuthor_ = p;
				getTitleAuthor(p, conv_opts_.title, conv_opts_.author);
				firstNameToTitle_ = false;
				break;
			case 'r':
				conv_opts_.r2l = (*p != '-');
				break;
			case 'm':
				{
					if (!*p) goto OPT_ERR;
						_HPDF_PageLayout mode = _HPDF_PageLayout(strtoul(p, NULL, 10));
					if (HPDF_PAGE_LAYOUT_SINGLE <= mode && mode < HPDF_PAGE_LAYOUT_EOF) {
						conv_opts_.layout_mode = _HPDF_PageLayout(mode);
					} else {
						fprintf(stderr, "option %s : out of range", arg);
					}
					break;
				}
			case '?':
			case 'h':
				return usage();
			default:
	 OPT_ERR:
				fprintf(stderr, "Incorrect option: %s", arg);
				return 1;
			}
		}
		return 0;
	}

	bool paramEquLong(char const* s, char const* t, char* &result_s) {
		return paramEquLong(s, t, (char const* &)result_s);
	}

	bool paramEquLong(char const* s, char const* t, char const* &result_s) {
		size_t tlen = strlen(t);
		if (strncmp(s, t, tlen) == 0) {
			s += tlen;
			result_s = s;
			return true;
		}
		return false;
	}

	void getTitleAuthor(char const* s, std::string& rTitle, std::string& rAuthor) {
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

	std::string getPathToDir(char const* src) {
		char const* base = fks::fname_baseName(src);
		if (src < base) {
			--base;
			std::string dirPath(src, base);
			char const* dir = fks::fname_baseName(dirPath.c_str());
			if (dir < base)
				return std::string(dir, base);
		}
		return std::string();
	}

private:
	JpgFileToPdf::Opts	conv_opts_;
	std::string			outname_;
	std::string			dstdir_;
	std::string			titleAuthor_;
	bool				firstNameToTitle_;
	bool				digitCmp_;
};


int main(int argc, char *argv[]) {
	return App().main(argc, argv);
}
