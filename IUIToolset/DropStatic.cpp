// DropStatic.cpp : implementation file
//

#include "pch.h"
#include "IUIToolset.h"
#include "DropStatic.h"


// CDropStatic

IMPLEMENT_DYNAMIC(CDropStatic, CStatic)

CDropStatic::CDropStatic()
{

}

CDropStatic::~CDropStatic()
{
}


BEGIN_MESSAGE_MAP(CDropStatic, CStatic)
END_MESSAGE_MAP()



// CDropStatic message handlers

LRESULT CDropStatic::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (WM_DROPFILES == message)
	{
		HDROP hDrop = (HDROP)wParam;
		UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
		if (nFiles == 1)
		{
			wchar_t szFilePath[MAX_PATH] = { 0 };
			if (DragQueryFile(hDrop, 0, szFilePath, MAX_PATH) > 0)
			{
				GetParent()->SendMessageW(WM_DROPFILE, (WPARAM)m_hWnd, (LPARAM)szFilePath);
			}
		}
		else
		{
			AfxMessageBox(L"Please drop only one file.");
		}
		DragFinish(hDrop);
		return 0;
	}

	return CStatic::WindowProc(message, wParam, lParam);
}
