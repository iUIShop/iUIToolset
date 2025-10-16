#pragma once
#include "afxdialogex.h"


// CSha256Page dialog

class CSha256Page : public CDialogEx
{
	DECLARE_DYNAMIC(CSha256Page)

public:
	CSha256Page(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSha256Page();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHA256 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnBrowser();
	int GetFileHash256(LPCWSTR lpszFile);
	virtual BOOL OnInitDialog();
	CProgressCtrl m_prgHash256;
	CStatic m_staTime;
	CEdit m_edtHash256;
	CEdit m_edtFile;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
