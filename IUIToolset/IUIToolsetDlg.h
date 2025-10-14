
// IUIToolsetDlg.h : header file
//

#pragma once

#include "Base64Page.h"


enum TREE_ITEM_TYPE
{
	TREE_ITEM_TYPE_BASE64 = 1,
};

// CIUIToolsetDlg dialog
class CIUIToolsetDlg : public CDialogEx
{
// Construction
public:
	CIUIToolsetDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IUITOOLSET_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CBase64Page m_PageBase64;
	CTreeCtrl m_treNavigate;
	afx_msg void OnSelchangedTreNavigate(NMHDR* pNMHDR, LRESULT* pResult);
	CStatic m_staPageArea;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
