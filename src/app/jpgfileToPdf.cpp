
/**
 *	@file	jpgFile2pdf.cpp
 *	@brief	jpg to pdf
 *	@author	tenka@6809.net
 */
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <hpdf.h>
#include "JpgFileToPdf.hpp"

#ifdef HPDF_DLL
#define CALLBACK_DECL  __stdcall
#else
#define CALLBACK_DECL
#endif


namespace {
	static jmp_buf 	s_jmp_buf_env;

	static void CALLBACK_DECL errorHandlerForHaru(HPDF_STATUS   error_no, HPDF_STATUS   detail_no, void *user_data) {
		printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
		longjmp(s_jmp_buf_env, 1);
	}
}

bool JpgFileToPdf::Opts::setLayoutMode(int mode) {
	this->layout_mode = _HPDF_PageLayout(mode);
	return (HPDF_PAGE_LAYOUT_SINGLE <= mode && mode < HPDF_PAGE_LAYOUT_EOF);
}



bool JpgFileToPdf::run(std::vector<std::string> const& jpgfiles, char const* outname, JpgFileToPdf::Opts const& opts) {
    HPDF_Doc  	pdf = HPDF_New (errorHandlerForHaru, NULL);
    if (!pdf) {
        printf ("error: cannot create PdfDoc object\n");
        return false;
    }
    if (setjmp(s_jmp_buf_env)) {
        HPDF_Free (pdf);
        return false;
    }

    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);

    HPDF_SetPageMode(pdf, HPDF_PAGE_MODE_USE_OUTLINE);
	HPDF_Outline root = HPDF_CreateOutline(pdf, NULL, "test", NULL);
	HPDF_Outline_SetOpened(root, HPDF_FALSE);

	HPDF_SetPageLayout(pdf, opts.layout_mode);
	//HPDF_SetPageLayout(pdf, HPDF_PAGE_LAYOUT_TWO_COLUMN_RIGHT);

	if (opts.r2l)
		addR2L(pdf);

	if (!opts.title.empty())
		HPDF_SetInfoAttr(pdf, HPDF_INFO_TITLE, opts.title.c_str());

	if (!opts.author.empty())
		HPDF_SetInfoAttr(pdf, HPDF_INFO_AUTHOR, opts.author.c_str());

	for (size_t i = 0; i < jpgfiles.size(); ++i)
		addJpgPage(pdf, jpgfiles[i].c_str(), root);

	
	HPDF_SaveToFile (pdf, outname);
    HPDF_Free (pdf);
    return 0;
}


HPDF_STATUS JpgFileToPdf::addR2L(HPDF_Doc pdf)
{
	HPDF_Catalog	catalog		= pdf->catalog;
	HPDF_Dict		preferences = HPDF_Dict_New(catalog->mmgr);
	if (!preferences)
		return catalog->error->error_no;

	HPDF_STATUS ret;
	if ((ret = HPDF_Dict_Add(catalog, "ViewerPreferences", preferences))
		!= HPDF_OK)
		return ret;

	// R2L
	//HPDF_Dict_RemoveElement(preferences, "Direction");
	if ((ret = HPDF_Dict_AddName(preferences, "Direction", "R2L")) != HPDF_OK)
		return ret;

	return HPDF_OK;
}



bool JpgFileToPdf::addJpgPage(HPDF_Doc pdf, char const* jpgname, HPDF_Outline root) {
    HPDF_Page 	page = HPDF_AddPage (pdf);
	if (!page)
		return false;
    HPDF_Image	image	= HPDF_LoadJpegImageFromFile (pdf, jpgname);
	if (!image) {
		printf("load error %s\n", jpgname);
		return false;
	}
	HPDF_REAL img_w = HPDF_REAL(HPDF_Image_GetWidth(image));
	HPDF_REAL img_h = HPDF_REAL(HPDF_Image_GetHeight(image));
    HPDF_Page_SetWidth (page, img_w);
    HPDF_Page_SetHeight(page, img_h);
    HPDF_Page_DrawImage (page, image, 0, 0, img_w, img_h);

	if (root) {
	    HPDF_Outline 	 outline = HPDF_CreateOutline (pdf, root, jpgname, NULL);
	    HPDF_Destination dst = HPDF_Page_CreateDestination (page);
	    HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(page), 1);
	    HPDF_Outline_SetDestination(outline, dst);
	}

	return true;
}
