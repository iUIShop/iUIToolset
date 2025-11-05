// MaxLengthLinePage.cpp : implementation file
//

#include "pch.h"
#include "IUIToolset.h"
#include "afxdialogex.h"
#include "MaxLengthLinePage.h"


int GetMaxLengthOfFileLines(LPCWSTR lpszFilePath, int *pnMaxLineIndex, int *pnMaxLineLength)
{
	if (nullptr == lpszFilePath || nullptr == pnMaxLineIndex || nullptr == pnMaxLineLength)
	{
		return -1;
	}

	*pnMaxLineLength = 0;
	CStdioFile file;
	if (!file.Open(lpszFilePath, CFile::modeRead | CFile::typeText | CFile::shareDenyNone))
	{
		return -1;
	}
	int nLineIndex = 1;
	CString strLine;
	while (file.ReadString(strLine))
	{
		int nLineLength = strLine.GetLength();
		if (nLineLength > *pnMaxLineLength)
		{
			*pnMaxLineLength = nLineLength;
			*pnMaxLineIndex = nLineIndex;
		}
		nLineIndex++;
	}
	file.Close();
	return 0;
}


// CMaxLengthLinePage dialog

IMPLEMENT_DYNAMIC(CMaxLengthLinePage, CDialogEx)

CMaxLengthLinePage::CMaxLengthLinePage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAX_LENGTH_LINE, pParent)
{

}

CMaxLengthLinePage::~CMaxLengthLinePage()
{
}

void CMaxLengthLinePage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STA_MAX_LENGTH_LINE, m_staMaxLengthLine);
}


BEGIN_MESSAGE_MAP(CMaxLengthLinePage, CDialogEx)
END_MESSAGE_MAP()


// CMaxLengthLinePage message handlers

LRESULT CMaxLengthLinePage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (WM_DROPFILES == message)
	{
		HANDLE hDrop = (HANDLE)wParam;

		UINT uCount = DragQueryFile((HDROP)hDrop, 0xFFFFFFFF, NULL, 0);
		if (uCount != 1)
		{
			AfxMessageBox(_T("只能拖入一个文件进行计算！"), MB_ICONWARNING);
			return 0;
		}

		TCHAR szFile[1024] = { 0 };
		UINT nSize = DragQueryFile((HDROP)hDrop, 0, szFile, 1024);
		_ASSERT(nSize < 1024);

		// Use szFile...
		int nMaxLineIndex = 0;
		int nMaxLineLength = 0;
		GetMaxLengthOfFileLines(szFile, &nMaxLineIndex, &nMaxLineLength);

		CString strInfo;
		strInfo.Format(_T("文件：'%s'\r\n最长行所在行号：%d，最长行长度：%d"), szFile, nMaxLineIndex, nMaxLineLength);
		m_staMaxLengthLine.SetWindowText(strInfo);
	}


	return CDialogEx::WindowProc(message, wParam, lParam);
}
