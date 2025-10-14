#pragma once

#include "afxdialogex.h"
#include "DropStatic.h"


// CBase64Page dialog

class CBase64Page : public CDialogEx
{
	DECLARE_DYNAMIC(CBase64Page)

public:
	CBase64Page(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CBase64Page();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BASE64 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnClearSource();
	afx_msg void OnBnClickedBtnClearResult();
	afx_msg void OnBnClickedBtnEncode();
	afx_msg void OnBnClickedBtnDecode();
	CEdit m_edtSource;
	CEdit m_edtResult;
	CDropStatic m_staDropFileToBase64;
	CDropStatic m_staDropBase64FileToFile;
	afx_msg void OnBnClickedBtnSaveResultToFile();
};
