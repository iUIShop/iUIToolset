// Base64Page.cpp : implementation file
//

#include "pch.h"
#include "IUIToolset.h"
#include "afxdialogex.h"
#include "Base64Page.h"
#include "../../Publib/String/String.h"

#pragma warning(disable:4996)


// CBase64Page dialog

IMPLEMENT_DYNAMIC(CBase64Page, CDialogEx)

CBase64Page::CBase64Page(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BASE64, pParent)
{

}

CBase64Page::~CBase64Page()
{
}

void CBase64Page::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_SOURCE, m_edtSource);
	DDX_Control(pDX, IDC_EDT_RESULT, m_edtResult);
	DDX_Control(pDX, IDC_STA_FILE_TO_BASE64, m_staDropFileToBase64);
	DDX_Control(pDX, IDC_STA_BASE64_FILE_TO_FILE, m_staDropBase64FileToFile);
}


BEGIN_MESSAGE_MAP(CBase64Page, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CLEAR_SOURCE, &CBase64Page::OnBnClickedBtnClearSource)
	ON_BN_CLICKED(IDC_BTN_CLEAR_RESULT, &CBase64Page::OnBnClickedBtnClearResult)
	ON_BN_CLICKED(IDC_BTN_ENCODE, &CBase64Page::OnBnClickedBtnEncode)
	ON_BN_CLICKED(IDC_BTN_DECODE, &CBase64Page::OnBnClickedBtnDecode)
	ON_BN_CLICKED(IDC_BTN_SAVE_RESULT_TO_FILE, &CBase64Page::OnBnClickedBtnSaveResultToFile)
END_MESSAGE_MAP()


// CBase64Page message handlers

void CBase64Page::OnBnClickedBtnClearSource()
{
	m_edtSource.SetWindowText(L"");
}

void CBase64Page::OnBnClickedBtnClearResult()
{
	m_edtResult.SetWindowText(L"");
}

void CBase64Page::OnBnClickedBtnEncode()
{
	CString strSource;
	m_edtSource.GetWindowText(strSource);
	if (strSource.IsEmpty())
	{
		AfxMessageBox(L"请输入要编码的内容。");
		return;
	}
	std::string strUtf8 = IUI::UnicodeToUTF8(strSource);
	std::string strBase64 = IUI::Base64Encode((const byte *)strUtf8.c_str(), (int)strUtf8.size());
	::SetWindowTextA(m_edtResult.GetSafeHwnd(), strBase64.c_str());
}

void CBase64Page::OnBnClickedBtnDecode()
{
	CString strSource;
	m_edtSource.GetWindowText(strSource);
	if (strSource.IsEmpty())
	{
		AfxMessageBox(L"请输入要解码的内容。");
		return;
	}
	std::string strBase64 = IUI::UnicodeToUTF8(strSource);
	std::string strDecode = IUI::Base64Decode(strBase64.c_str());
	std::string strUtf8 = std::string(strDecode.c_str(), strDecode.size());
	std::wstring strResult = IUI::UTF8ToUnicode(strUtf8.c_str());
	::SetWindowTextW(m_edtResult.GetSafeHwnd(), strResult.c_str());
}

LRESULT CBase64Page::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	if (WM_DROPFILE == message)
	{
		HWND hWndDrop = (HWND)wParam;
		wchar_t* pszFilePath = (wchar_t*)lParam;
		if (pszFilePath && wcslen(pszFilePath) > 0)
		{
			FILE* fp = _wfopen(pszFilePath, L"rb");

			if (m_staDropFileToBase64.GetSafeHwnd() == hWndDrop)
			{
				// 读取二进制文件内容，并转换为base64字符串。
				fseek(fp, 0, SEEK_END);
				long lFileSize = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				BYTE* pData = new BYTE[lFileSize];
				fread(pData, 1, lFileSize, fp);
				std::string strBase64 = IUI::Base64Encode(pData, (int)lFileSize);
				delete[] pData;
				pData = NULL;
				::SetWindowTextA(m_edtResult.GetSafeHwnd(), strBase64.c_str());
			}
			else if (m_staDropBase64FileToFile.GetSafeHwnd() == hWndDrop)
			{
				// 读取base64字符串，转换为二进制数据，并保存为文件。
				fseek(fp, 0, SEEK_END);
				long lFileSize = ftell(fp);
				fseek(fp, 0, SEEK_SET);

				char* pszBase64 = new char[lFileSize + 1];
				fread(pszBase64, 1, lFileSize, fp);
				pszBase64[lFileSize] = 0;

				int nLenDecode = (int)strlen(pszBase64);
				std::vector<BYTE> vOriData;
				vOriData.resize(nLenDecode);
				BOOL bRet2 = ATL::Base64Decode(pszBase64, nLenDecode, &vOriData[0], &nLenDecode);
				vOriData.resize(nLenDecode);
				delete[] pszBase64;
				pszBase64 = NULL;

				// 保存为文件，保存到当前exe目录下。
				wchar_t szModulePath[MAX_PATH] = { 0 };
				GetModuleFileNameW(NULL, szModulePath, MAX_PATH);
				PathRemoveFileSpecW(szModulePath);
				PathCchAppend(szModulePath, MAX_PATH, L"base64orgfile");

				FILE* fpOut = _wfopen(szModulePath, L"wb");
				if (fpOut)
				{
					fwrite(&vOriData[0], 1, vOriData.size(), fpOut);
					fclose(fpOut);

					// 打开所在的文件夹，并选中base64orgfile文件
					ShellExecuteW(NULL, L"open", L"explorer.exe", (L"/select,\"" + std::wstring(szModulePath) + L"\"").c_str(), NULL, SW_SHOWNORMAL);
				}
			}

			fclose(fp);
		}
		return 0;
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}

void CBase64Page::OnBnClickedBtnSaveResultToFile()
{
	// 把结果保存为二进制文件
	CString strResult;
	m_edtResult.GetWindowText(strResult);
	if (strResult.IsEmpty())
	{
		AfxMessageBox(L"结果内容为空，无法保存。");
		return;
	}

	// 保存为文件，保存到当前exe目录下。
	wchar_t szModulePath[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, szModulePath, MAX_PATH);
	PathRemoveFileSpecW(szModulePath);
	PathCchAppend(szModulePath, MAX_PATH, L"resultfile");

	FILE* fpOut = _wfopen(szModulePath, L"wb");
	if (fpOut)
	{
		fwrite((LPCWSTR)strResult, 1, strResult.GetLength() * sizeof(WCHAR), fpOut);
		fclose(fpOut);

		// 打开所在的文件夹，并选中resultfile文件
		ShellExecuteW(NULL, L"open", L"explorer.exe", (L"/select,\"" + std::wstring(szModulePath) + L"\"").c_str(), NULL, SW_SHOWNORMAL);
	}
}
