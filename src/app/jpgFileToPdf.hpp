/**
 *	@file	jpgFileTopdf.cpp
 *	@brief	jpg to pdf
 *	@author	tenka@6809.net
 */
#ifndef JPGFILETOPDF_HPP
#define JPGFILETOPDF_HPP

#include <setjmp.h>
#include <vector>
#include <string>
#include <hpdf.h>

//#pragma execution_character_set("utf-8")

class JpgFileToPdf {
public:
	struct Opts {
		Opts() : r2l(false), jap(false), outline(false), layout_mode(HPDF_PAGE_LAYOUT_SINGLE) {}
	public:
		std::string		title;
		std::string		author;
		bool			r2l;
		bool			jap;
		bool			outline;
		HPDF_PageLayout	layout_mode;
	};

	JpgFileToPdf();

	bool run(std::vector<std::string> const& jpgfiles, char const* outname, Opts const& opts);

private:
	bool addJpgPage(HPDF_Doc pdf, char const* jpgname, HPDF_Outline root);
	
	HPDF_STATUS addR2L(HPDF_Doc pdf);
	
	static bool hasOver0x80(char const* s);
	//static std::string toUtf8(std::string const& src);

private:
	size_t		page_count_;
	bool		is_jap_;
};

#endif
