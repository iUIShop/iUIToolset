#pragma once
#include "afxdialogex.h"


// CPdfPage dialog

class CPdfPage : public CDialogEx
{
	DECLARE_DYNAMIC(CPdfPage)

public:
	CPdfPage(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPdfPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PDF };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edtPdfPath;
	CStatic m_staDropPdf;
	afx_msg void OnBnClickedBtnBrowserPdf();
	afx_msg void OnBnClickedBtnConvertToImages();
};
