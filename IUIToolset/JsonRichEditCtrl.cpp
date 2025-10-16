// JsonRichEditCtrl.cpp : implementation file
//

#include "pch.h"
#include "IUIToolset.h"
#include "JsonRichEditCtrl.h"


// CJsonRichEditCtrl

IMPLEMENT_DYNAMIC(CJsonRichEditCtrl, CRichEditCtrl)

CJsonRichEditCtrl::CJsonRichEditCtrl()
{

}

CJsonRichEditCtrl::~CJsonRichEditCtrl()
{
}


BEGIN_MESSAGE_MAP(CJsonRichEditCtrl, CRichEditCtrl)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_CONTROL_REFLECT(EN_SETFOCUS, &CJsonRichEditCtrl::OnEnSetfocus)
	ON_CONTROL_REFLECT(EN_UPDATE, &CJsonRichEditCtrl::OnEnUpdate)
END_MESSAGE_MAP()



// CJsonRichEditCtrl message handlers




void CJsonRichEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
	NotifyCaretChanged();
}

void CJsonRichEditCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRichEditCtrl::OnLButtonDown(nFlags, point);
	NotifyCaretChanged();
}

void CJsonRichEditCtrl::OnEnSetfocus()
{
	// TODO: Add your control notification handler code here
	NotifyCaretChanged();
}

void CJsonRichEditCtrl::OnEnUpdate()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CRichEditCtrl::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
	NotifyCaretChanged();
}

int CJsonRichEditCtrl::NotifyCaretChanged()
{
	return (int)(INT_PTR)GetParent()->SendMessage(WM_RE_CARET_CHANGED, 0, LPARAM(m_hWnd));
}
