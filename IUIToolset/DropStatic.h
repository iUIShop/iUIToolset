#pragma once


// CDropStatic

class CDropStatic : public CStatic
{
	DECLARE_DYNAMIC(CDropStatic)

public:
	CDropStatic();
	virtual ~CDropStatic();

protected:
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


