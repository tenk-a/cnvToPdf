/**
 *	@file	cnvToPdf.cpp
 *	@brief	image(jpg) to pdf
 *	@author	tenka@6809.net
 */
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <vector>
#include <string>
#include <hpdf.h>
#include <misc/ExArgv.h>
#include "JpgFileToPdf.hpp"


static int usage() {
	printf("jpg2pdf -oOUTPUT.pdf jpgfile(s)...\n");
	return 1;
}

int main (int argc, char *argv[]) {
	if (argc < 1)
		return usage();
	ExArgv_conv(&argc, &argv);

	std::vector<std::string> jpgfiles;
	std::string outname;
	jpgfiles.reserve(1000);
	for (int i = 1; i < argc; ++i) {
		char* p = argv[i];
		if (*p == '-') {
			switch (*++p) {
			case 'o':
				if (*++p)
					outname = p;
				break;
			default:
				return usage();
			}
		} else {
			jpgfiles.push_back(p);
		}
	}
	if (jpgfiles.empty())
		return 0;
	if (outname.empty())
		outname = "a.pdf";
	JpgFileToPdf	jpgFileToPdf;
	return jpgFileToPdf.run(jpgfiles, outname.c_str()) == false;
}
