// JsonPage.cpp : implementation file
//

#include "pch.h"
#include "IUIToolset.h"
#include "afxdialogex.h"
#include "JsonPage.h"


CStringA CJsonHelper::m_strError;


typedef Json::Writer JsonWriter;
typedef Json::Reader JsonReader;
typedef Json::Value  JsonValue;
void print(JsonValue& v)
{
	JsonValue::Members mem = v.getMemberNames();
	for (auto iter = mem.begin(); iter != mem.end(); iter++)
	{
		if (v[*iter].type() == Json::objectValue)
		{
			print(v[*iter]);
		}
		else if (v[*iter].type() == Json::arrayValue)
		{
			auto cnt = v[*iter].size();
			for (UINT i = 0; i < cnt; i++)
			{
				print(v[*iter][i]);
			}
		}
		else if (v[*iter].type() == Json::stringValue)
		{
			Json::Value jvStringToObj;
			bool bRet = CJsonHelper::LoadJsonByStream(v[*iter].asString(), jvStringToObj);
			if (bRet)
			{
				Json::ValueType n = jvStringToObj.type();
				bool b = jvStringToObj.isObject();
				v[*iter] = jvStringToObj;
			}

			//cout << v[*iter].asString() << endl;
		}
		else if (v[*iter].type() == Json::realValue)
		{
			//cout << v[*iter].asDouble() << endl;
		}
		else if (v[*iter].type() == Json::uintValue)
		{
			//cout << v[*iter].asUInt() << endl;
		}
		else
		{
			//cout << v[*iter].asInt() << endl;
		}
	}
	return;
}

// CJsonFormatDlg 消息处理程序
int GetXTwipsFromPixel(int nPixel)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nXDPI = GetDeviceCaps(hDCN, LOGPIXELSX);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	if (nXDPI == 0)
	{
		nXDPI = 96;
	}

	int nRet = MulDiv(nPixel, 1440, nXDPI);

	return nRet;
}

int GetYTwipsFromPixel(int nPixel)
{
	HDC hDCN = ::GetDC(::GetDesktopWindow());
	int nYDPI = GetDeviceCaps(hDCN, LOGPIXELSY);
	::ReleaseDC(::GetDesktopWindow(), hDCN);

	if (nYDPI == 0)
	{
		nYDPI = 96;
	}

	int nRet = MulDiv(nPixel, 1440, nYDPI);

	return nRet;
}


HRESULT GenerateCharFormat(CHARFORMAT2* pcf, LOGFONT& lf)
{
	memset(pcf, 0, sizeof(CHARFORMAT2));

	pcf->cbSize = sizeof(CHARFORMAT2);

	// dwMask的值由两个集合中的宏组合
	// 一个集合中的宏指示要设置或读取CHARFORMAT2中的哪个成员
	// 另一个集合中的宏指示要设置或读取CHARFORMAT2::dwEffects中的哪一位。
	pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_CHARSET | CFM_FACE
		| CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;

	// CFM_SIZE，单位缇
	pcf->yHeight = GetXTwipsFromPixel(-lf.lfHeight);

	// CFM_OFFSET，单位缇
	pcf->yOffset = 0;

	// CFM_CHARSET
	pcf->bCharSet = lf.lfCharSet;

	pcf->bPitchAndFamily = lf.lfPitchAndFamily;

	// CFM_FACE
	StringCchCopy(pcf->szFaceName, LF_FACESIZE, lf.lfFaceName);

	// dwEffects集中的CFM_BOLD
	pcf->dwEffects = 0;
	if (lf.lfWeight >= FW_BOLD)
	{
		pcf->dwEffects |= CFE_BOLD;
	}

	// dwEffects集中的CFM_ITALIC
	if (lf.lfItalic)
	{
		pcf->dwEffects |= CFE_ITALIC;
	}

	// dwEffects集中的CFM_UNDERLINE
	if (lf.lfUnderline)
	{
		pcf->dwEffects |= CFE_UNDERLINE;
	}

	return S_OK;
}


// CJsonPage dialog

IMPLEMENT_DYNAMIC(CJsonPage, CDialogEx)

CJsonPage::CJsonPage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_JSON, pParent)
{

}

CJsonPage::~CJsonPage()
{
}

void CJsonPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_OUTPUT, m_edtOutput);
	DDX_Control(pDX, IDC_RE_JSON, m_reJson);
	DDX_Control(pDX, IDC_STA_STATUS_BAR, m_staStatusBar);
}


BEGIN_MESSAGE_MAP(CJsonPage, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_FORMAT, &CJsonPage::OnBnClickedBtnFormat)
	ON_BN_CLICKED(IDC_BTN_ESCAPE, &CJsonPage::OnBnClickedBtnEscape)
	ON_BN_CLICKED(IDC_BTN_REMOVE_ESCAPE, &CJsonPage::OnBnClickedBtnRemoveEscape)
	ON_BN_CLICKED(IDC_BTN_ONE_LINE, &CJsonPage::OnBnClickedBtnOneLine)
	ON_BN_CLICKED(IDC_BTN_STR_TO_OJB, &CJsonPage::OnBnClickedBtnStrToOjb)
	ON_MESSAGE(WM_RE_CARET_CHANGED, &CJsonPage::OnReCaretChanged)
END_MESSAGE_MAP()


// CJsonPage message handlers

BOOL CJsonPage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	CHARFORMAT2 cf;
	LOGFONT lf;
	CFont* pFont = GetFont();
	pFont->GetLogFont(&lf);
	lf.lfHeight /= 2;
	GenerateCharFormat(&cf, lf);
	//m_reJson.SetDefaultCharFormat(cf);
	m_reJson.LimitText(0x7FFFFFFF);
	m_reJson.SetRect(CRect(0, 0, 0x7fffff, 0x7fffff));
	CRect rc;
	//m_reJson.GetRect(rc);
	HDC hDC = GetWindowDC()->GetSafeHdc();
	//m_reJson.SetTargetDevice(hDC, 0x7fffff);// 最大就支持0x7fffff
	//m_reJson.SetWordWrapMode(WBF_OVERFLOW);

#ifdef _DEBUG
	std::string strJson = "{\"a\":1,}";
	::SetWindowTextA(m_reJson.GetSafeHwnd(), strJson.c_str());
#endif

	::SetFocus(m_reJson.GetSafeHwnd());

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CJsonPage::OnBnClickedBtnFormat()
{
	// 清空输出窗口
	m_edtOutput.SetWindowText(_T(""));

	// 得到需要被格式化的字符串
	CStringA strJson;
	int nLen = ::GetWindowTextLengthA(m_reJson.GetSafeHwnd());
	::GetWindowTextA(m_reJson.GetSafeHwnd(), strJson.GetBufferSetLength(nLen), nLen + 1);
	strJson.ReleaseBuffer();

	// 把原来的json字符串，转成json对象
	Json::Value jvRoot;
	bool bRet = CJsonHelper::LoadJsonByStream((LPCSTR)strJson, jvRoot);
	if (!bRet)
	{
		// 输出出错信息
		CJsonHelper::m_strError.Replace("\n", "\r\n");
		::SetWindowTextA(m_edtOutput.GetSafeHwnd(), CJsonHelper::m_strError);

		return;
	}

	// 把json对象，重新格式化成josn字符串
	CStringA strFormatJson = jvRoot.toStyledString().c_str();
	::SetWindowTextA(m_reJson.GetSafeHwnd(), strFormatJson);
}

void CJsonPage::OnBnClickedBtnEscape()
{
	// 清空输出窗口
	m_edtOutput.SetWindowText(_T(""));

	// 得到需要被转义的字符串
	CStringA strJson;
	int nLen = ::GetWindowTextLengthA(m_reJson.GetSafeHwnd());
	::GetWindowTextA(m_reJson.GetSafeHwnd(), strJson.GetBufferSetLength(nLen), nLen + 1);
	strJson.ReleaseBuffer();

	// 验证需要被转义的字符串是否是合法的josn格式
	Json::Value jvRoot;
	bool bRet = CJsonHelper::LoadJsonByStream((LPCSTR)strJson, jvRoot);
	if (!bRet)
	{
		// 输出出错信息
		CJsonHelper::m_strError.Replace("\n", "\r\n");
		::SetWindowTextA(m_edtOutput.GetSafeHwnd(), CJsonHelper::m_strError);

		return;
	}

	// 转义
	strJson.Replace("\\", "\\\\");
	strJson.Replace("\"", "\\\"");
	::SetWindowTextA(m_reJson.GetSafeHwnd(), strJson);
}

void CJsonPage::OnBnClickedBtnRemoveEscape()
{
	// 清空输出窗口
	m_edtOutput.SetWindowText(_T(""));

	// 得到需要被去转义的字符串
	CStringA strJson;
	int nLen = ::GetWindowTextLengthA(m_reJson.GetSafeHwnd());
	::GetWindowTextA(m_reJson.GetSafeHwnd(), strJson.GetBufferSetLength(nLen), nLen + 1);
	strJson.ReleaseBuffer();

	// 去转义
	strJson.Replace("\\\\", "\\");
	strJson.Replace("\\\"", "\"");
	::SetWindowTextA(m_reJson.GetSafeHwnd(), strJson);
}

void CJsonPage::OnBnClickedBtnOneLine()
{
	// 清空输出窗口
	m_edtOutput.SetWindowText(_T(""));

	// 得到需要被格式化的字符串
	CStringA strJson;
	int nLen = ::GetWindowTextLengthA(m_reJson.GetSafeHwnd());
	::GetWindowTextA(m_reJson.GetSafeHwnd(), strJson.GetBufferSetLength(nLen), nLen + 1);
	strJson.ReleaseBuffer();

	// 把原来的json字符串，转成json对象
	Json::Value jvRoot;
	bool bRet = CJsonHelper::LoadJsonByStream((LPCSTR)strJson, jvRoot);
	if (!bRet)
	{
		// 输出出错信息
		CJsonHelper::m_strError.Replace("\n", "\r\n");
		::SetWindowTextA(m_edtOutput.GetSafeHwnd(), CJsonHelper::m_strError);

		return;
	}

	// 把json对象，重新格式化成josn字符串
	std::string strFormatJson;
	CJsonHelper::FastWriteJson(jvRoot, strFormatJson);
	::SetWindowTextA(m_reJson.GetSafeHwnd(), strFormatJson.c_str());
}

void CJsonPage::OnBnClickedBtnStrToOjb()
{
	// 清空输出窗口
	m_edtOutput.SetWindowText(_T(""));

	// 得到需要被格式化的字符串
	CStringA strJson;
	int nLen = ::GetWindowTextLengthA(m_reJson.GetSafeHwnd());
	::GetWindowTextA(m_reJson.GetSafeHwnd(), strJson.GetBufferSetLength(nLen), nLen + 1);
	strJson.ReleaseBuffer();

	// 把原来的json字符串，转成json对象，验证json语法
	Json::Value jvRoot;
	bool bRet = CJsonHelper::LoadJsonByStream((LPCSTR)strJson, jvRoot);
	if (!bRet)
	{
		// 输出出错信息
		CJsonHelper::m_strError.Replace("\n", "\r\n");
		::SetWindowTextA(m_edtOutput.GetSafeHwnd(), CJsonHelper::m_strError);

		return;
	}

	// 遍历里面的key，如果值是字符串，就尝试把字符串转成json对象。
	print(jvRoot);

	// 把json对象，重新格式化成josn字符串
	CStringA strFormatJson = jvRoot.toStyledString().c_str();
	::SetWindowTextA(m_reJson.GetSafeHwnd(), strFormatJson);
}

LRESULT CJsonPage::OnReCaretChanged(WPARAM wParam, LPARAM lParam)
{
	HWND hSender = HWND(lParam);
	if (hSender == m_reJson.GetSafeHwnd())
	{
		// 得到当标所在行的行首的cp
		int nLineCp = m_reJson.LineIndex();
		// 通过行首cp，得到行索引
		int nLine = m_reJson.LineFromChar(nLineCp);
		// 得到光标cp
		long nStartCp = -1;
		long nEndCp = -1;
		m_reJson.GetSel(nStartCp, nEndCp);

		CString strStatus;
		strStatus.Format(_T("当前行 %d\t列 %d"), nLine + 1, nEndCp + 1 - nLineCp);
		m_staStatusBar.SetWindowText(strStatus);
	}

	return 0;
}
