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

// CJsonFormatDlg ��Ϣ�������
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

	// dwMask��ֵ�����������еĺ����
	// һ�������еĺ�ָʾҪ���û��ȡCHARFORMAT2�е��ĸ���Ա
	// ��һ�������еĺ�ָʾҪ���û��ȡCHARFORMAT2::dwEffects�е���һλ��
	pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_CHARSET | CFM_FACE
		| CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;

	// CFM_SIZE����λ�
	pcf->yHeight = GetXTwipsFromPixel(-lf.lfHeight);

	// CFM_OFFSET����λ�
	pcf->yOffset = 0;

	// CFM_CHARSET
	pcf->bCharSet = lf.lfCharSet;

	pcf->bPitchAndFamily = lf.lfPitchAndFamily;

	// CFM_FACE
	StringCchCopy(pcf->szFaceName, LF_FACESIZE, lf.lfFaceName);

	// dwEffects���е�CFM_BOLD
	pcf->dwEffects = 0;
	if (lf.lfWeight >= FW_BOLD)
	{
		pcf->dwEffects |= CFE_BOLD;
	}

	// dwEffects���е�CFM_ITALIC
	if (lf.lfItalic)
	{
		pcf->dwEffects |= CFE_ITALIC;
	}

	// dwEffects���е�CFM_UNDERLINE
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
	//m_reJson.SetTargetDevice(hDC, 0x7fffff);// ����֧��0x7fffff
	//m_reJson.SetWordWrapMode(WBF_OVERFLOW);

#ifdef _DEBUG
	std::string strJson = "{\"a\":1,}";
	::SetWindowTextA(m_reJson.GetSafeHwnd(), strJson.c_str());
#endif

	::SetFocus(m_reJson.GetSafeHwnd());

	return FALSE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CJsonPage::OnBnClickedBtnFormat()
{
	// ����������
	m_edtOutput.SetWindowText(_T(""));

	// �õ���Ҫ����ʽ�����ַ���
	CStringA strJson;
	int nLen = ::GetWindowTextLengthA(m_reJson.GetSafeHwnd());
	::GetWindowTextA(m_reJson.GetSafeHwnd(), strJson.GetBufferSetLength(nLen), nLen + 1);
	strJson.ReleaseBuffer();

	// ��ԭ����json�ַ�����ת��json����
	Json::Value jvRoot;
	bool bRet = CJsonHelper::LoadJsonByStream((LPCSTR)strJson, jvRoot);
	if (!bRet)
	{
		// ���������Ϣ
		CJsonHelper::m_strError.Replace("\n", "\r\n");
		::SetWindowTextA(m_edtOutput.GetSafeHwnd(), CJsonHelper::m_strError);

		return;
	}

	// ��json�������¸�ʽ����josn�ַ���
	CStringA strFormatJson = jvRoot.toStyledString().c_str();
	::SetWindowTextA(m_reJson.GetSafeHwnd(), strFormatJson);
}

void CJsonPage::OnBnClickedBtnEscape()
{
	// ����������
	m_edtOutput.SetWindowText(_T(""));

	// �õ���Ҫ��ת����ַ���
	CStringA strJson;
	int nLen = ::GetWindowTextLengthA(m_reJson.GetSafeHwnd());
	::GetWindowTextA(m_reJson.GetSafeHwnd(), strJson.GetBufferSetLength(nLen), nLen + 1);
	strJson.ReleaseBuffer();

	// ��֤��Ҫ��ת����ַ����Ƿ��ǺϷ���josn��ʽ
	Json::Value jvRoot;
	bool bRet = CJsonHelper::LoadJsonByStream((LPCSTR)strJson, jvRoot);
	if (!bRet)
	{
		// ���������Ϣ
		CJsonHelper::m_strError.Replace("\n", "\r\n");
		::SetWindowTextA(m_edtOutput.GetSafeHwnd(), CJsonHelper::m_strError);

		return;
	}

	// ת��
	strJson.Replace("\\", "\\\\");
	strJson.Replace("\"", "\\\"");
	::SetWindowTextA(m_reJson.GetSafeHwnd(), strJson);
}

void CJsonPage::OnBnClickedBtnRemoveEscape()
{
	// ����������
	m_edtOutput.SetWindowText(_T(""));

	// �õ���Ҫ��ȥת����ַ���
	CStringA strJson;
	int nLen = ::GetWindowTextLengthA(m_reJson.GetSafeHwnd());
	::GetWindowTextA(m_reJson.GetSafeHwnd(), strJson.GetBufferSetLength(nLen), nLen + 1);
	strJson.ReleaseBuffer();

	// ȥת��
	strJson.Replace("\\\\", "\\");
	strJson.Replace("\\\"", "\"");
	::SetWindowTextA(m_reJson.GetSafeHwnd(), strJson);
}

void CJsonPage::OnBnClickedBtnOneLine()
{
	// ����������
	m_edtOutput.SetWindowText(_T(""));

	// �õ���Ҫ����ʽ�����ַ���
	CStringA strJson;
	int nLen = ::GetWindowTextLengthA(m_reJson.GetSafeHwnd());
	::GetWindowTextA(m_reJson.GetSafeHwnd(), strJson.GetBufferSetLength(nLen), nLen + 1);
	strJson.ReleaseBuffer();

	// ��ԭ����json�ַ�����ת��json����
	Json::Value jvRoot;
	bool bRet = CJsonHelper::LoadJsonByStream((LPCSTR)strJson, jvRoot);
	if (!bRet)
	{
		// ���������Ϣ
		CJsonHelper::m_strError.Replace("\n", "\r\n");
		::SetWindowTextA(m_edtOutput.GetSafeHwnd(), CJsonHelper::m_strError);

		return;
	}

	// ��json�������¸�ʽ����josn�ַ���
	std::string strFormatJson;
	CJsonHelper::FastWriteJson(jvRoot, strFormatJson);
	::SetWindowTextA(m_reJson.GetSafeHwnd(), strFormatJson.c_str());
}

void CJsonPage::OnBnClickedBtnStrToOjb()
{
	// ����������
	m_edtOutput.SetWindowText(_T(""));

	// �õ���Ҫ����ʽ�����ַ���
	CStringA strJson;
	int nLen = ::GetWindowTextLengthA(m_reJson.GetSafeHwnd());
	::GetWindowTextA(m_reJson.GetSafeHwnd(), strJson.GetBufferSetLength(nLen), nLen + 1);
	strJson.ReleaseBuffer();

	// ��ԭ����json�ַ�����ת��json������֤json�﷨
	Json::Value jvRoot;
	bool bRet = CJsonHelper::LoadJsonByStream((LPCSTR)strJson, jvRoot);
	if (!bRet)
	{
		// ���������Ϣ
		CJsonHelper::m_strError.Replace("\n", "\r\n");
		::SetWindowTextA(m_edtOutput.GetSafeHwnd(), CJsonHelper::m_strError);

		return;
	}

	// ���������key�����ֵ���ַ������ͳ��԰��ַ���ת��json����
	print(jvRoot);

	// ��json�������¸�ʽ����josn�ַ���
	CStringA strFormatJson = jvRoot.toStyledString().c_str();
	::SetWindowTextA(m_reJson.GetSafeHwnd(), strFormatJson);
}

LRESULT CJsonPage::OnReCaretChanged(WPARAM wParam, LPARAM lParam)
{
	HWND hSender = HWND(lParam);
	if (hSender == m_reJson.GetSafeHwnd())
	{
		// �õ����������е����׵�cp
		int nLineCp = m_reJson.LineIndex();
		// ͨ������cp���õ�������
		int nLine = m_reJson.LineFromChar(nLineCp);
		// �õ����cp
		long nStartCp = -1;
		long nEndCp = -1;
		m_reJson.GetSel(nStartCp, nEndCp);

		CString strStatus;
		strStatus.Format(_T("��ǰ�� %d\t�� %d"), nLine + 1, nEndCp + 1 - nLineCp);
		m_staStatusBar.SetWindowText(strStatus);
	}

	return 0;
}
