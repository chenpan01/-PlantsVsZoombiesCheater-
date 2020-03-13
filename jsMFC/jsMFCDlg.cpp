
// jsMFCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "jsMFC.h"
#include "jsMFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CjsMFCDlg 对话框



CjsMFCDlg::CjsMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CjsMFCDlg::IDD, pParent)
	, mBigGuan(_T(""))
	, mSmallGuan(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CjsMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, mBigGuan);
	DDV_MaxChars(pDX, mBigGuan, 1);
	DDX_Text(pDX, IDC_EDIT2, mSmallGuan);
	DDV_MaxChars(pDX, mSmallGuan, 2);
}

BEGIN_MESSAGE_MAP(CjsMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CjsMFCDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CjsMFCDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CjsMFCDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CjsMFCDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CjsMFCDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CjsMFCDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON8, &CjsMFCDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON7, &CjsMFCDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON9, &CjsMFCDlg::OnBnClickedButton9)
END_MESSAGE_MAP()

int base = 0x007794f8;
HANDLE hProcess;

int *get2Point(int base, int p1, int p2)
{
	int iBase, iP1, *iP2;

	if (!ReadProcessMemory(hProcess, (LPVOID)base, &iBase, 4, NULL))
	{
		return NULL;
	}

	if (!ReadProcessMemory(hProcess, (LPVOID)(iBase + p1), &iP1, 4, NULL))
	{
		return NULL;
	}

	iP2 = (int *)(iP1 + p2);
	return iP2;
}

int *get3Point(int base, int p1, int p2, int p3)
{
	int iBase, iP1, iP2, *iP3;

	if (!ReadProcessMemory(hProcess, (LPVOID)base, &iBase, 4, NULL))
	{
		return NULL;
	}

	if (!ReadProcessMemory(hProcess, (LPVOID)(iBase + p1), &iP1, 4, NULL))
	{
		return NULL;
	}

	if (!ReadProcessMemory(hProcess, (LPVOID)(iP1 + p2), &iP2, 4, NULL))
	{
		return NULL;
	}
	iP3 = (int *)(iP2 + p3);
	return iP3;
}

BOOL InitWaigua()
{
	HWND hWnd = FindWindow(NULL, TEXT("Plants vs. Zombies 1.2.0.1073 【传智播客C++学院】"));
	if (NULL == hWnd)
	{
		//AfxMessageBox(TEXT("查找窗口失败"));
		return FALSE;
	}

	DWORD dwProcessId;
	GetWindowThreadProcessId(hWnd, &dwProcessId);

	//获取进程句柄
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (NULL == hProcess)
	{
		//AfxMessageBox(TEXT("打开进程失败"));
		return FALSE;
	}

	return TRUE;
}

void ModifySun()
{
	int *sun = get2Point(base, 0x868, 0x5578);
	int sunValue = 9000;
	WriteProcessMemory(hProcess, sun, &sunValue, 4, NULL);
}

void ModifyMoney()
{
	int *pMoney = get2Point(base, 0x950, 0x50);
	int moneyValue = 500;
	WriteProcessMemory(hProcess, pMoney, &moneyValue, 4, NULL);
}

void ModifyColdTime()
{
	int *pCount = get3Point(base, 0x868, 0x15c, 0x24);
	if (pCount == NULL)
		return ;

	int *pFirst = get3Point(base, 0x868, 0x15c, 0x4c);
	if (pFirst == NULL)
		return ;

	//
	int iCount = 0;
	ReadProcessMemory(hProcess, pCount, &iCount, 4, NULL);
	for (int i = 0; i < iCount; i++)
	{
		//pFirst[0] = pFirst[1];//读和写

		int iRecoveryTime;
		ReadProcessMemory(hProcess, pFirst + 1, &iRecoveryTime, 4, NULL);
		WriteProcessMemory(hProcess, pFirst, &iRecoveryTime, 4, NULL);

		pFirst = (int *)((int)pFirst + 0x50);
	}
}

// CjsMFCDlg 消息处理程序
BOOL IsSunUnlimite = FALSE, IsMoneyUnlimite = FALSE, IsColdTimeZero = FALSE;
DWORD WINAPI WaiguaThread(LPVOID lpParameter)
{
	CjsMFCDlg *dlg = (CjsMFCDlg *)lpParameter;
	while (1)
	{
		Sleep(1000);
		//检测游戏是否启动
		if (InitWaigua())
		{
			//检测到了游戏
			dlg->SetDlgItemText((int)IDC_STATIC10, TEXT("已经检测到游戏启动"));

			//保存原来的三个字节
			//ReadProcessMemory(hProcess, pCode, oldOpCode, 3, NULL);
		}
		else
		{
			//设置窗口状态，没有检测到游戏
			dlg->SetDlgItemText((int)IDC_STATIC10, TEXT("未检测到游戏启动"));
			continue;
		}


		if (IsSunUnlimite)
		{
			ModifySun();
		}
		if (IsMoneyUnlimite)
		{
			ModifyMoney();
		}
		if (IsColdTimeZero)
		{
			ModifyColdTime();
		}
	}


	return 0;
}

unsigned char *pCode = (unsigned char *)0x00472B50;
unsigned char oldOpCode[3];
unsigned char newOpCode[] = "\xC2\x04\x00";

BOOL CjsMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MINIMIZE);

	// TODO:  在此添加额外的初始化代码
	//创建线程来不停修改阳光。。。。。

	CreateThread(0, 0, WaiguaThread, this, 0, 0);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CjsMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CjsMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CjsMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CjsMFCDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	IsSunUnlimite = TRUE;
}


void CjsMFCDlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	IsSunUnlimite = FALSE;
}


void CjsMFCDlg::OnBnClickedButton3()
{
	// TODO:  在此添加控件通知处理程序代码
	IsMoneyUnlimite = TRUE;
}


void CjsMFCDlg::OnBnClickedButton4()
{
	// TODO:  在此添加控件通知处理程序代码
	IsMoneyUnlimite = FALSE;
}


void CjsMFCDlg::OnBnClickedButton5()
{
	// TODO:  在此添加控件通知处理程序代码
	IsColdTimeZero = TRUE;
}


void CjsMFCDlg::OnBnClickedButton6()
{
	// TODO:  在此添加控件通知处理程序代码
	IsColdTimeZero = FALSE;
}

//禁用暂停

void NoPause()
{
	DWORD dwOldProtect;
	VirtualProtectEx(hProcess, pCode, 3, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, pCode, newOpCode, 3, NULL);
	VirtualProtectEx(hProcess, pCode, 3, dwOldProtect, NULL);
}

void YesPause()
{
	DWORD dwOldProtect;
	VirtualProtectEx(hProcess, pCode, 3, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, pCode, oldOpCode, 3, NULL);
	VirtualProtectEx(hProcess, pCode, 3, dwOldProtect, NULL);
}

void CjsMFCDlg::OnBnClickedButton8()
{
	// TODO:  在此添加控件通知处理程序代码
	NoPause();
}


void CjsMFCDlg::OnBnClickedButton7()
{
	// TODO:  在此添加控件通知处理程序代码
	YesPause();
}

void ModifyGuanka(int iGuanka)
{
	int *pGuanka = get2Point(base, 0x950, 0x4C);
	WriteProcessMemory(hProcess, pGuanka, &iGuanka, 4, NULL);
}

void CjsMFCDlg::OnBnClickedButton9()
{
	// TODO:  在此添加控件通知处理程序代码

	UpdateData();

	TCHAR *pStopstring;
	long lBigGuan = wcstol(mBigGuan, &pStopstring, 10);
	long lSmallGuan = wcstol(mSmallGuan, &pStopstring, 10);
	if (lBigGuan < 1 || lBigGuan > 6)
	{
		AfxMessageBox(TEXT("大关关卡不合格"));
		return;
	}
	if (lSmallGuan < 1 || lSmallGuan > 10)
	{
		AfxMessageBox(TEXT("小关关卡不合格"));
		return;
	}

	int iGuanka = (lBigGuan - 1) * 10 + lSmallGuan;
	ModifyGuanka(iGuanka);

	//AfxMessageBox(mBigGuan + mSmallGuan);
}
