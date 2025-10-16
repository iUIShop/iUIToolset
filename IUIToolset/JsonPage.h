#pragma once

#include "afxdialogex.h"
#include "JsonRichEditCtrl.h"


// CJsonPage dialog

class CJsonPage : public CDialogEx
{
	DECLARE_DYNAMIC(CJsonPage)

public:
	CJsonPage(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CJsonPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JSON };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnFormat();
	afx_msg void OnBnClickedBtnEscape();
	afx_msg void OnBnClickedBtnRemoveEscape();
	afx_msg void OnBnClickedBtnOneLine();
	afx_msg void OnBnClickedBtnStrToOjb();
	afx_msg LRESULT OnReCaretChanged(WPARAM wParam, LPARAM lParam);

	CEdit m_edtOutput;
	CJsonRichEditCtrl m_reJson;
	CStatic m_staStatusBar;
	virtual BOOL OnInitDialog();
};
