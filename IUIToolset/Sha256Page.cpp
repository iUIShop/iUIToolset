// Sha256Page.cpp : implementation file
//

#include "pch.h"
#include "IUIToolset.h"
#include "afxdialogex.h"
#include "Sha256Page.h"


int g_nCurProgress = -1;
DWORD g_dwTime = 0;
int OnHash256Proc(IUI::HASH256_MSG uMsg, WPARAM wParam, LPARAM lParam, void* pUserData)
{
	CSha256Page* pThis = (CSha256Page*)pUserData;

	switch (uMsg)
	{
	case IUI::HM_ERROR:
	{
		const WCHAR* pszFunName = (const WCHAR*)wParam;
		int nFunIndex = LOWORD(lParam);
		int nError = HIWORD(lParam);

		CStringW strError;
		strError.Format(LR"(错误：接口"%s"中函数"%s"调用失败，错误码：%d。)", nFunIndex == 2 ? L"GetFileHash256" : L"", pszFunName, nError);

		::MessageBox(pThis->GetSafeHwnd(), strError, L"Hash256", IDOK);
	}
	break;
	case IUI::HM_BEGIN:
		break;

	case IUI::HM_PROGRESS:
		if (0 != lParam)
		{
			// 设置进度条
			int nProgress = int(wParam * 100 / lParam);
			if (nProgress != g_nCurProgress)
			{
				pThis->m_prgHash256.SetPos(nProgress);
				g_nCurProgress = nProgress;
			}
		}
		break;

	case IUI::HM_END:
	{
		const WCHAR* pszHash256 = (const WCHAR*)lParam;
		// 显示Hash256
		if (nullptr != pszHash256)
		{
			DWORD dwTime = GetTickCount() - g_dwTime;
			CString strTime;
			strTime.Format(_T("%d ms"), dwTime);
			pThis->m_staTime.SetWindowText(strTime);

			CStringW strHash256 = pszHash256;
			::SetWindowTextW(pThis->m_edtHash256.GetSafeHwnd(), strHash256);
		}

	}
	break;

	default:
		_ASSERT(FALSE);
		break;
	}

	return 0;
}

// https://blog.csdn.net/ayang1986/article/details/79569521
BOOL ChangeWndMessageFilterOk(UINT  nMessage, BOOL  bAllow)
{
	typedef  BOOL(WINAPI* ChangeWindowMessageFilterOkFn)(UINT, DWORD);

	HMODULE  hModUser32 = NULL;
	hModUser32 = LoadLibrary(_T("user32.dll"));
	if (hModUser32 == NULL) {
		return  FALSE;
	}

	ChangeWindowMessageFilterOkFn pfnChangeWindowMessageFilter = (ChangeWindowMessageFilterOkFn)GetProcAddress(hModUser32, "ChangeWindowMessageFilter");
	if (pfnChangeWindowMessageFilter == NULL)
	{
		FreeLibrary(hModUser32);
		return  FALSE;
	}

	FreeLibrary(hModUser32);

	return  pfnChangeWindowMessageFilter(nMessage, bAllow ? MSGFLT_ADD : MSGFLT_REMOVE);
}


// CSha256Page dialog

IMPLEMENT_DYNAMIC(CSha256Page, CDialogEx)

CSha256Page::CSha256Page(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SHA256, pParent)
{

}

CSha256Page::~CSha256Page()
{
}

void CSha256Page::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRG_HASH256, m_prgHash256);
	DDX_Control(pDX, IDC_STA_TIME, m_staTime);
	DDX_Control(pDX, IDC_EDT_HASH256, m_edtHash256);
	DDX_Control(pDX, IDC_EDT_FILE, m_edtFile);
}


BEGIN_MESSAGE_MAP(CSha256Page, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_BROWSER, &CSha256Page::OnBnClickedBtnBrowser)
END_MESSAGE_MAP()


// CSha256Page message handlers

void CSha256Page::OnBnClickedBtnBrowser()
{
	CFileDialog dlg(TRUE);
	INT_PTR nRet = dlg.DoModal();
	if (nRet == IDOK)
	{
		CString strFile = dlg.GetPathName();
		m_edtFile.SetWindowText(strFile);

		GetFileHash256(strFile);
	}
}

int CSha256Page::GetFileHash256(LPCWSTR lpszFile)
{
	g_dwTime = GetTickCount();
	IUI::GetFileHash256Async(lpszFile, TRUE, OnHash256Proc, this);

	return 0;
}

BOOL CSha256Page::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	DragAcceptFiles(TRUE);
	ChangeWndMessageFilterOk(WM_DROPFILES, MSGFLT_ADD);
	ChangeWndMessageFilterOk(0x0049, MSGFLT_ADD);        // 0x0049 == WM_COPYGLOBALDATA
	m_prgHash256.SetRange(0, 100);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CSha256Page::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		m_edtFile.SetWindowText(szFile);

		GetFileHash256(szFile);
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}
