#pragma once


// CJsonRichEditCtrl

class CJsonRichEditCtrl : public CRichEditCtrl
{
    DECLARE_DYNAMIC(CJsonRichEditCtrl)

public:
    CJsonRichEditCtrl();
    virtual ~CJsonRichEditCtrl();

protected:
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnEnSetfocus();
    afx_msg void OnEnUpdate();
    DECLARE_MESSAGE_MAP()

protected:
    int NotifyCaretChanged();
};
