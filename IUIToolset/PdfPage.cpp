// PdfPage.cpp : implementation file
//

#include "pch.h"
#include "IUIToolset.h"
#include "afxdialogex.h"
#include "PdfPage.h"
#include "pdfium-win-x64/include/fpdfview.h"
#include "../../Publib/String/String.h"
#include <gdiplus.h>
using namespace Gdiplus;
#pragma warning(disable:4996)

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

bool SaveBitmapAsPNG_GDIPlus(FPDF_BITMAP bitmap, const std::string& filename) {
    if (!bitmap) return false;

    int width = FPDFBitmap_GetWidth(bitmap);
    int height = FPDFBitmap_GetHeight(bitmap);
    void* buffer = FPDFBitmap_GetBuffer(bitmap);
    int stride = FPDFBitmap_GetStride(bitmap);

    if (!buffer || width <= 0 || height <= 0) return false;

    // 初始化 GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // 转换到 RGBA
    std::vector<BYTE> rgba_buffer(width * height * 4);
    BYTE* src = static_cast<BYTE*>(buffer);
    BYTE* dst = rgba_buffer.data();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int src_idx = y * stride + x * 4;
            int dst_idx = (y * width + x) * 4;

            // BGRA 转 RGBA
            dst[dst_idx + 0] = src[src_idx + 2];  // R
            dst[dst_idx + 1] = src[src_idx + 1];  // G
            dst[dst_idx + 2] = src[src_idx + 0];  // B
            dst[dst_idx + 3] = src[src_idx + 3];  // A
        }
    }

    // 创建 Bitmap
    Bitmap bmp(width, height, width * 4, PixelFormat32bppARGB, rgba_buffer.data());

    // 保存为 PNG
    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);

    std::wstring wfilename(filename.begin(), filename.end());
    Status status = bmp.Save(wfilename.c_str(), &pngClsid, NULL);

    GdiplusShutdown(gdiplusToken);
    return status == Ok;
}

// 获取编码器 CLSID
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;
    GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;

    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)malloc(size);
    if (!pImageCodecInfo) return -1;

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT i = 0; i < num; ++i) {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[i].Clsid;
            free(pImageCodecInfo);
            return i;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

// Edit from Deepseek
bool pdfToImagesWithPDFium(const std::string& pdfPath, const std::string& outputDir)
{
    // 初始化PDFium
    FPDF_LIBRARY_CONFIG config;
    config.version = 2;
    config.m_pUserFontPaths = nullptr;
    config.m_pIsolate = nullptr;
    config.m_v8EmbedderSlot = 0;
    FPDF_InitLibraryWithConfig(&config);

    // 加载PDF文档
    FPDF_DOCUMENT doc = FPDF_LoadDocument(pdfPath.c_str(), nullptr);
    if (!doc)
    {
        FPDF_DestroyLibrary();
        return false;
    }

    int pageCount = FPDF_GetPageCount(doc);

    for (int i = 0; i < pageCount; ++i)
    {
        FPDF_PAGE page = FPDF_LoadPage(doc, i);
        if (!page) continue;

        // 获取页面尺寸
        double width = FPDF_GetPageWidth(page);
        double height = FPDF_GetPageHeight(page);

        // 创建位图
        FPDF_BITMAP bitmap = FPDFBitmap_Create(
            (int)width, (int)height, 0);

        if (bitmap)
        {
            FPDFBitmap_FillRect(bitmap, 0, 0,
                (int)width, (int)height, 0xFFFFFFFF);

            // 渲染页面
            FPDF_RenderPageBitmap(bitmap, page, 0, 0,
                (int)width, (int)height, 0, 0);

            // 保存为BMP（需要转换为其他格式可使用其他库）
			CStringA strFilename;
			strFilename.Format("%spage_%d.png", outputDir.c_str(), i + 1);
			SaveBitmapAsPNG_GDIPlus(bitmap, (LPCSTR)strFilename);

            FPDFBitmap_Destroy(bitmap);
        }

        FPDF_ClosePage(page);
    }

    FPDF_CloseDocument(doc);
    FPDF_DestroyLibrary();

    // 打开文件夹
	ShellExecuteA(NULL, "open", outputDir.c_str(), NULL, NULL, SW_SHOW);

    return true;
}

// CPdfPage dialog

IMPLEMENT_DYNAMIC(CPdfPage, CDialogEx)

CPdfPage::CPdfPage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PDF, pParent)
{

}

CPdfPage::~CPdfPage()
{
}

void CPdfPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_PDF_PATH, m_edtPdfPath);
	DDX_Control(pDX, IDC_STA_DROP_PDF, m_staDropPdf);
}


BEGIN_MESSAGE_MAP(CPdfPage, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_BROWSER_PDF, &CPdfPage::OnBnClickedBtnBrowserPdf)
	ON_BN_CLICKED(IDC_BTN_CONVERT_TO_IMAGES, &CPdfPage::OnBnClickedBtnConvertToImages)
END_MESSAGE_MAP()


// CPdfPage message handlers

void CPdfPage::OnBnClickedBtnBrowserPdf()
{
	CFileDialog fileDlg(TRUE, L".pdf", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, L"PDF Files (*.pdf)|*.pdf||", this);
	if (fileDlg.DoModal() == IDOK)
	{
		CStringW strPath = fileDlg.GetPathName();
		m_edtPdfPath.SetWindowTextW(strPath);
	}
}

void CPdfPage::OnBnClickedBtnConvertToImages()
{
    CStringW strPdfPath;
    m_edtPdfPath.GetWindowTextW(strPdfPath);

    if (strPdfPath.IsEmpty())
    {
        AfxMessageBox(L"Please select a PDF file.", MB_ICONERROR);
        return;
	}

    // 得到"下载"文件夹路径
    CStringW strDownloadPath;
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &path);
    if (SUCCEEDED(hr))
    {
        strDownloadPath = path;
        CoTaskMemFree(path);
    }
    else
    {
		AfxMessageBox(L"Failed to get Downloads folder path.", MB_ICONERROR);
        return;
    }

    // 生成一个GUID字符串作为子文件夹名称
    GUID guid;
    CoCreateGuid(&guid);
    WCHAR guidString[40] = { 0 };
    swprintf_s(guidString, L"%08lX-%04X-%04X-%04X-%012llX",
        guid.Data1, guid.Data2, guid.Data3,
        (guid.Data4[0] << 8) | guid.Data4[1],
        ((unsigned long long)guid.Data4[2] << 40) |
        ((unsigned long long)guid.Data4[3] << 32) |
        ((unsigned long long)guid.Data4[4] << 24) |
        ((unsigned long long)guid.Data4[5] << 16) |
        ((unsigned long long)guid.Data4[6] << 8) |
        ((unsigned long long)guid.Data4[7]));
    // 创建子文件夹
    CStringW strOutputDir = strDownloadPath + L"\\" + guidString + L"\\";
    CreateDirectoryW(strOutputDir, NULL);

	// 调用转换函数
	std::string strPdfPathU8 = IUI::UnicodeToUTF8(strPdfPath);
    std::string strTargetPathU8 = IUI::UnicodeToUTF8(strOutputDir);
    pdfToImagesWithPDFium(strPdfPathU8, strTargetPathU8);
}
