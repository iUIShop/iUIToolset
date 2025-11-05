#pragma once
#include "afxdialogex.h"


// CMaxLengthLinePage dialog

class CMaxLengthLinePage : public CDialogEx
{
	DECLARE_DYNAMIC(CMaxLengthLinePage)

public:
	CMaxLengthLinePage(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMaxLengthLinePage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAX_LENGTH_LINE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CStatic m_staMaxLengthLine;
};
