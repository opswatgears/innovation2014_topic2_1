
// ProcessMonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessMonitor.h"
#include "ProcessMonitorDlg.h"
#include "afxdialogex.h"
#include "wa_message_constant.h"
#include "DlgOption.h"
#include "wa_utils_string.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CProcessMonitorDlg dialog




CProcessMonitorDlg::CProcessMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProcessMonitorDlg::IDD, pParent)
	, m_ctrTotalModule(_T(""))
	, m_ctrTotalProcess(_T(""))
	, m_ctrTotalEngine(_T(""))
	, m_pLister( NULL )
	, m_ctrTotalScanned(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REPORT_PROCESS, m_ctrProcessReport);
	DDX_Control(pDX, IDC_REPORT_DETAIL, m_ctrDetailReport);
	DDX_Text(pDX, IDC_STATIC_TOTAL_MODULE, m_ctrTotalModule);
	DDX_Text(pDX, IDC_STATIC_TOTAL_PROCESS, m_ctrTotalProcess);
	DDX_Text(pDX, IDC_STATIC_TOTAL_ENGINE, m_ctrTotalEngine);
	DDX_Text(pDX, IDC_STATIC_TOTAL_SCANNED, m_ctrTotalScanned);
}

BEGIN_MESSAGE_MAP(CProcessMonitorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_MESSAGE( WM_LIST_PROCESS_COMPLETE, &CProcessMonitorDlg::OnProcessListComplete )

	ON_WM_DESTROY()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_REPORT_PROCESS, &CProcessMonitorDlg::OnGetdispinfoProcessList )
	
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &CProcessMonitorDlg::OnHdnItemdblclickReportProcess)
	ON_NOTIFY(NM_DBLCLK, IDC_REPORT_PROCESS, &CProcessMonitorDlg::OnNMDblclkReportProcess)
	ON_BN_CLICKED(IDC_BUTTON1, &CProcessMonitorDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CProcessMonitorDlg message handlers

BOOL CProcessMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here	

	this->_initReports();
	this->_initLister();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProcessMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcessMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProcessMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CProcessMonitorDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	if ( this->m_pLister != NULL )
	{
		this->m_pLister->stop();
		Sleep( 50 );

		delete this->m_pLister;
	}
}

void CProcessMonitorDlg::_initReports()
{
	this->m_ctrProcessReport.SetExtendedStyle( LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE 
								| LVS_EX_UNDERLINEHOT | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	this->m_ctrProcessReport.InsertColumn( 0, _T( "PID" ), LVCFMT_CENTER, 50 );
	this->m_ctrProcessReport.InsertColumn( 1, _T( "Process name" ), LVCFMT_LEFT, 150 );
	this->m_ctrProcessReport.InsertColumn( 2, _T( "Executale path" ), LVCFMT_LEFT, 300 );
	this->m_ctrProcessReport.InsertColumn( 3, _T( "Total thread" ), LVCFMT_CENTER, 50 );
	this->m_ctrProcessReport.InsertColumn( 4, _T( "Meta scan" ), LVCFMT_CENTER, 100 );
	this->m_ctrProcessReport.InsertColumn( 5, _T( "Modules scanned" ), LVCFMT_CENTER, 100 );

	this->m_ctrDetailReport.SetExtendedStyle( LVS_EX_GRIDLINES );
	this->m_ctrDetailReport.InsertColumn( 0, _T( "Process/module" ), LVCFMT_CENTER, 150 );	
	this->m_ctrDetailReport.InsertColumn( 1, _T( "Executale path" ), LVCFMT_LEFT, 300 );
	this->m_ctrDetailReport.InsertColumn( 2, _T( "Meta scan" ), LVCFMT_CENTER, 100 );
}

void CProcessMonitorDlg::_initLister()
{
	this->m_pLister = new WaProcesScanner( this->m_hWnd, &( this->m_processList ) );
	bool b_res = this->m_pLister->start();
	if ( b_res == false )
	{
		AfxMessageBox( L"Error in creating process lister" );
	}
}

void CProcessMonitorDlg::_fillDetailReport( int i_item )
{		
	this->m_ctrDetailReport.DeleteAllItems();

	TProcessInfo& info = this->m_processList[i_item];
	LVCOLUMN lvCol;
	::ZeroMemory((void *)&lvCol, sizeof(LVCOLUMN));
	lvCol.mask=LVCF_TEXT;
	this->m_ctrDetailReport.GetColumn( 0, &lvCol );
	lvCol.pszText = ( LPWSTR ) info.process_name.c_str();
	this->m_ctrDetailReport.SetColumn( 0, &lvCol );

	for ( int i = 0 ; i < info.p_modules.size() ; i++ )
	{
		TModuleInfo& module = info.p_modules[i];
		this->m_ctrDetailReport.InsertItem( i, ( LPCTSTR )module.s_module.c_str() );
		this->m_ctrDetailReport.SetItemText( i, 1, ( LPCTSTR )module.s_exe.c_str() );
		CString s_status = L"scanning";
		if ( module.p_scan->scan_status != L"scanning")
		{
			s_status.Format( L"%s by %d engine(s)", module.p_scan->scan_status.c_str(), module.p_scan->n_engine );
		}		
		this->m_ctrDetailReport.SetItemText( i, 2, s_status );
	}
	this->m_ctrTotalEngine.Format( L"Modules listed: %d", info.p_modules.size() );
	this->UpdateData( FALSE );
}

std::wstring CProcessMonitorDlg::_getResult( TModuleInfo& module )
{
	std::wstring s_text = module.p_scan->scan_status;
	if ( s_text == L"scanning" )
	{
		return s_text;
	}

	wchar_t scan_result[64];
	wsprintf( scan_result, L" by %d engine(s)", module.p_scan->n_engine );
	s_text += scan_result;

	return s_text;
}

// --
HRESULT CProcessMonitorDlg::OnProcessListComplete( WPARAM wParam, LPARAM lParam )
{
	int n_process = this->m_processList.size();
	this->m_ctrTotalProcess.Format( L"Processes listed: %d", n_process );

	int n_module = 0;
	int n_scan = 0;
	for ( int i = 0 ; i < n_process ; i++ )
	{
		TProcessInfo& info = this->m_processList[i];
		n_module += info.p_modules.size();

		for ( int j = 0 ; j < info.p_modules.size() ; j++ )
		{
			TModuleInfo& module = info.p_modules[j];
			if ( module.p_scan->scan_status != L"scanning" )
			{
				n_scan ++;
			}
		}
	}

	this->m_ctrTotalModule.Format( L"Modules listed: %d", n_module );
	this->m_ctrTotalScanned.Format( L"Processes/modules scanned: %d", n_scan );
	this->UpdateData( FALSE );

	this->m_ctrProcessReport.SetItemCount( this->m_processList.size() );

	return 0;
}


void CProcessMonitorDlg::OnGetdispinfoProcessList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	//Create a pointer to the item
	LV_ITEM* pItem= &(pDispInfo)->item;

	//Which item number?
	int itemid = pItem->iItem;
	TProcessInfo& brief = this->m_processList.at( itemid );

	//Do the list need text information?
	if (pItem->mask & LVIF_TEXT)
	{
		CString text;
		switch ( pItem->iSubItem )
		{
		case 0:
			text = ( LPCTSTR )WaStringUtils::intToStrW( brief.pid ).c_str();
			break;
		case 1:
			text = ( LPCTSTR )brief.process_name.c_str();
			break;
		case 2:
			text = ( LPCTSTR )brief.p_modules[0].s_exe.c_str();
			break;
		case 3:
			text = ( LPCTSTR )WaStringUtils::intToStrW( brief.nthread ).c_str();
			break;
		case 4:
			{
				wa_wstring s_scan_result = this->_getResult( brief.p_modules[0] );
				text = ( LPCTSTR )s_scan_result.c_str();
				break;
			}	
		case 5:
			{
				int count = 0;
				int n_module = brief.p_modules.size();
				for ( int i = 0 ; i < n_module ; i++ )
				{
					if ( brief.p_modules[i].p_scan->scan_status != L"scanning" )
						count++;					
				}
				text.Format( L"%d/%d", count, n_module );
				break;
			}
		}				

		//Copy the text to the LV_ITEM structure
		//Maximum number of characters is in pItem->cchTextMax
		lstrcpyn(pItem->pszText, text, pItem->cchTextMax);
	}	

	*pResult = 0;
}

/*void CProcessMonitorDlg::OnGetdispinfoDetailList( NMHDR* pNMHDR, LRESULT* pResult )
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	//Create a pointer to the item
	LV_ITEM* pItem= &(pDispInfo)->item;

	//Which item number?
	int itemid = pItem->iItem;
	
	CString text;
	if ( pItem->iSubItem == 0 )
	{
		text = ( LPCTSTR )this->m_pSelProcess->p_modules[itemid].s_module.c_str();
	}
	else
	{
		text = L"";
		TCHAR szText[128];
		LVCOLUMN lvColInfo ;	
		ZeroMemory(&lvColInfo, sizeof(lvColInfo));
		lvColInfo.pszText = szText;
		lvColInfo.cchTextMax = 128;
		lvColInfo.mask = LVCF_TEXT;
		this->m_ctrDetailReport.GetColumn( pItem->iSubItem, &lvColInfo );

		std::wstring engine = szText;
		WaJson* p_detail = this->m_pSelProcess->p_modules[itemid].p_scan->p_detail;
		WaJson scan_result_json;	
		WaJson scan_detail_json;
		WaJson result_json;
		int i_result;
		int i_time;

		if ( WAAPI_SUCCESS( p_detail->get( L"scan_results", scan_result_json ) ) &&
			WAAPI_SUCCESS( scan_result_json.get( L"scan_details", scan_detail_json ) ) &&
			WAAPI_SUCCESS( scan_detail_json.get( engine, result_json ) ) &&
			WAAPI_SUCCESS( result_json.get( L"scan_result_i", i_result ) ) &&
			WAAPI_SUCCESS( result_json.get( L"scan_time", i_time ) ) )
		{
			text.Format( L"%d in %d ms", i_result, i_time );
		}
	}
	//Do the list need text information?
				

	//Copy the text to the LV_ITEM structure
	//Maximum number of characters is in pItem->cchTextMax
	lstrcpyn(pItem->pszText, text, pItem->cchTextMax);
	

	*pResult = 0;

}*/

void CProcessMonitorDlg::OnHdnItemdblclickReportProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	//NMITEMACTIVATE* nm=(NMITEMACTIVATE*)pNMHDR;
	

	*pResult = 0;
}


void CProcessMonitorDlg::OnNMDblclkReportProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	this->_fillDetailReport( pNMItemActivate->iItem );

	*pResult = 0;
}


void CProcessMonitorDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CDlgOption option;
	option.DoModal();
}
