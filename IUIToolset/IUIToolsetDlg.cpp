
// IUIToolsetDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "IUIToolset.h"
#include "IUIToolsetDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIUIToolsetDlg dialog



CIUIToolsetDlg::CIUIToolsetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IUITOOLSET_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIUIToolsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRE_NAVIGATE, m_treNavigate);
	DDX_Control(pDX, IDC_STA_PAGE_AREA, m_staPageArea);
}

BEGIN_MESSAGE_MAP(CIUIToolsetDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TRE_NAVIGATE, &CIUIToolsetDlg::OnSelchangedTreNavigate)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CIUIToolsetDlg message handlers

BOOL CIUIToolsetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	HTREEITEM hTreeItem = m_treNavigate.InsertItem(_T("Base64 Encode/Decode"), 0, 0);
	m_treNavigate.SetItemData(hTreeItem, TREE_ITEM_TYPE_BASE64);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIUIToolsetDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIUIToolsetDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIUIToolsetDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CIUIToolsetDlg::OnSelchangedTreNavigate(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	CRect rcPage;
	m_staPageArea.GetWindowRect(&rcPage);
	ScreenToClient(&rcPage);

	HTREEITEM hSelItem = m_treNavigate.GetSelectedItem();
	if (hSelItem)
	{
		DWORD dwItemType = (DWORD)m_treNavigate.GetItemData(hSelItem);
		switch (dwItemType)
		{
		case TREE_ITEM_TYPE_BASE64:
			m_PageBase64.Create(IDD_BASE64, this);
			m_PageBase64.MoveWindow(&rcPage);
			m_PageBase64.ShowWindow(SW_SHOW);
			break;
		default:
			break;
		}
	}

	*pResult = 0;
}

void CIUIToolsetDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CRect rcPage;
	if (m_staPageArea.GetSafeHwnd())
	{
		m_staPageArea.GetWindowRect(&rcPage);
		ScreenToClient(&rcPage);

		m_PageBase64.MoveWindow(&rcPage);
	}
}
