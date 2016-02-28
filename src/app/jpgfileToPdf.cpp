/**
 *  @file   jpgFile2pdf.cpp
 *  @brief  jpg to pdf
 *  @author tenka@6809.net
 */
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <hpdf.h>
#include "JpgFileToPdf.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef HPDF_DLL
#define CALLBACK_DECL  __stdcall
#else
#define CALLBACK_DECL
#endif


namespace {
    static jmp_buf  s_jmp_buf_env;

    /** エラーハンドラー
     */
    static void CALLBACK_DECL errorHandlerForHaru(HPDF_STATUS   error_no, HPDF_STATUS   detail_no, void *user_data) {
        printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
        longjmp(s_jmp_buf_env, 1);
    }
}

/** デフォルトコンストラクタ
 */
JpgFileToPdf::JpgFileToPdf()
    : page_count_(1)
    , is_jap_(false)
{

}

/** jpg画像を読み込んでをpdfファイルに変換
 */
bool JpgFileToPdf::run(std::vector<std::string> const& jpgfiles, char const* outname, JpgFileToPdf::Opts const& opts) {
    HPDF_Doc    pdf = HPDF_New (errorHandlerForHaru, NULL);
    if (!pdf) {
        printf ("error: cannot create PdfDoc object\n");
        return false;
    }
    if (setjmp(s_jmp_buf_env)) {
        HPDF_Free (pdf);
        return false;
    }

    HPDF_SetCompressionMode (pdf, HPDF_COMP_ALL);   // 圧縮モード

    is_jap_ = opts.jap || hasOver0x80(opts.title.c_str()) || hasOver0x80(opts.author.c_str());
    if (is_jap_) {      // 日本語扱う場合
        HPDF_UseJPEncodings(pdf);
        HPDF_UseJPFonts(pdf);
        HPDF_SetCurrentEncoder(pdf, "90msp-RKSJ-H");    // カレントのエンコーダ設定しないと著者名とかタイトルとか文字化けする
    }

    HPDF_Outline root = NULL;
    if (opts.outline) {         // アウトラインつける場合
        HPDF_SetPageMode(pdf, HPDF_PAGE_MODE_USE_OUTLINE);
        root = HPDF_CreateOutline(pdf, NULL, "page", NULL);
        HPDF_Outline_SetOpened(root, HPDF_FALSE);
    }

    HPDF_SetPageLayout(pdf, opts.layout_mode);      // 見開きとか単頁とかのpdf開いた時の状態設定

    if (opts.r2l)
        addR2L(pdf);            // 右綴じにする場合

    if (!opts.author.empty())   // 著者名設定
        HPDF_SetInfoAttr(pdf, HPDF_INFO_AUTHOR, opts.author.c_str());

    if (!opts.title.empty())    // タイトル設定
        HPDF_SetInfoAttr(pdf, HPDF_INFO_TITLE, opts.title.c_str());

    for (size_t i = 0; i < jpgfiles.size(); ++i)
        addJpgPage(pdf, jpgfiles[i].c_str(), root); // jpg ファイルをページ化

    HPDF_SaveToFile (pdf, outname);     // pdfファイル書き出し
    HPDF_Free(pdf);                     // 作業終了
    return 0;
}


/** 右綴じ設定
 */
HPDF_STATUS JpgFileToPdf::addR2L(HPDF_Doc pdf) {
    HPDF_Catalog    catalog     = pdf->catalog;
    HPDF_Dict       preferences = HPDF_Dict_New(catalog->mmgr);
    if (!preferences)
        return catalog->error->error_no;

    HPDF_STATUS ret = HPDF_Dict_Add(catalog, "ViewerPreferences", preferences);
    if (ret != HPDF_OK)
        return ret;

    //HPDF_Dict_RemoveElement(preferences, "Direction");
    ret = HPDF_Dict_AddName(preferences, "Direction", "R2L");
    if (ret != HPDF_OK)
        return ret;

    return HPDF_OK;
}


/** jpgファイルを頁化
 */
bool JpgFileToPdf::addJpgPage(HPDF_Doc pdf, char const* jpgname, HPDF_Outline root) {
    HPDF_Page   page = HPDF_AddPage (pdf);
    if (!page)
        return false;
    HPDF_Image  image   = HPDF_LoadJpegImageFromFile (pdf, jpgname);
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
        if (is_jap_) {
            HPDF_Font font = HPDF_GetFont(pdf, "MS-PGothic", "90msp-RKSJ-H");
            if (font)
                HPDF_Page_SetFontAndSize(page, font, 9);
        }
        char str[130];  // 文字列化したint整数分には十分なサイズを指定.
        sprintf(str, "%d", page_count_);
        HPDF_Outline     outline = HPDF_CreateOutline (pdf, root, str, NULL);
        HPDF_Destination dst = HPDF_Page_CreateDestination (page);
        HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(page), 1);
        HPDF_Outline_SetDestination(outline, dst);
    }
    ++page_count_;

    return true;
}

/** 文字列に 0x80以上のコードが含まれているか否か
 */
bool JpgFileToPdf::hasOver0x80(char const* src) {
    signed char const* s = (signed char const*)src;
    while (*s > 0)
        ++s;
    return *s != 0;
}


#if 0
std::string JpgFileToPdf::toUtf8(std::string const& src) {
     #ifdef _WIN32
        size_t   wlen = src.size() + 1;
        wchar_t* wbuf = (wchar_t*)alloca(wlen * 6);
        int      rc   = MultiByteToWideChar(0, 0, src.c_str(), src.size(), wbuf, wlen);
        if (rc > 0) {
            wbuf[rc]    = L'\0';
            size_t len  = rc + 1;
            size_t capa = len * 6 + 1;
            char*  buf  = (char*)alloca(len * 6 + 1);
            rc = WideCharToMultiByte(65001, 0, wbuf, len, buf, capa, NULL, NULL);
            if (rc > 0) {
                buf[rc] = '\0';
                //rc = MultiByteToWideChar(65001, 0, buf, rc, wbuf, wlen);
                return buf;
            }
        }
        return src;
    #else
        return src;
    #endif
}
#endif
