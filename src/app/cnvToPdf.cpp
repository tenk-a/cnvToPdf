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
#include <algorithm>
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
	JpgFileToPdf		conv;
	JpgFileToPdf::Opts	conv_opts;
	std::string			outname;

	jpgfiles.reserve(1000);
	for (int i = 1; i < argc; ++i) {
		char* p = argv[i];
		if (*p == '-') {
			++p;
			switch (*p++) {
			case 'o':
				if (*p)
					outname = p;
				break;
			case 't':
				if (*p)
					conv_opts.title = p;
				break;
			case 'a':
				if (*p)
					conv_opts.author = p;
				break;
			case 'r':
				conv_opts.r2l = (*p != '-');
				break;
			case 'm':
				/*	PAGE_LAYOUT_SINGLE			 = 0,
					PAGE_LAYOUT_ONE_COLUMN		 = 1,
					PAGE_LAYOUT_TWO_COLUMN_LEFT = 2,
					PAGE_LAYOUT_TWO_COLUMN_RIGHT= 3,
					PAGE_LAYOUT_TWO_PAGE_LEFT   = 4,
					PAGE_LAYOUT_TWO_PAGE_RIGHT  = 5,
				*/
				if (conv_opts.setLayoutMode(strtoul(p, NULL, 10)) == false) {
					printf("option %s : out of range", argv[i]);
					return 1;
				}
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

	std::sort(jpgfiles.begin(), jpgfiles.end());

	return conv.run(jpgfiles, outname.c_str(), conv_opts) == false;
}
