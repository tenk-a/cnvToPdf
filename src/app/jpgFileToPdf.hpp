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

class JpgFileToPdf {
public:
	struct Opts {
		Opts() : r2l(false), layout_mode(HPDF_PAGE_LAYOUT_SINGLE) {}
		bool setLayoutMode(int mode);
	public:
		std::string		title;
		std::string		author;
		bool			r2l;
		HPDF_PageLayout	layout_mode;
	};
	bool run(std::vector<std::string> const& jpgfiles, char const* outname, Opts const& opts);

private:
	bool addJpgPage(HPDF_Doc pdf, char const* jpgname, HPDF_Outline root);
	HPDF_STATUS addR2L(HPDF_Doc pdf);
};

#endif
