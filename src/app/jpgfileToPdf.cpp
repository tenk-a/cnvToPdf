
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
	    printf ("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
	    longjmp(s_jmp_buf_env, 1);
	}
}


bool JpgFileToPdf::run(std::vector<std::string> const& jpgfiles, char const* outname) {
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
	for (size_t i = 0; i < jpgfiles.size(); ++i)
		addJpgPage(pdf, jpgfiles[i].c_str());
    HPDF_SaveToFile (pdf, outname);
    HPDF_Free (pdf);
    return 0;
}


bool JpgFileToPdf::addJpgPage(HPDF_Doc pdf, char const* jpgname) {
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
	return true;
}
