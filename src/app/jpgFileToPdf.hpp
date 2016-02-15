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
	bool run(std::vector<std::string> const& jpgfiles, char const* outname);

private:
	bool addJpgPage(HPDF_Doc pdf, char const* jpgname);
};

#endif
