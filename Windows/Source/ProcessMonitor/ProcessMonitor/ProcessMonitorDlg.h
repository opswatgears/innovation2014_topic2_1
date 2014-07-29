
// ProcessMonitorDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include <TlHelp32.h>
#include "wa_process_scanner.h"
#include "wa_process_info.h"

// CProcessMonitorDlg dialog
class CProcessMonitorDlg : public CDialogEx
{
// Construction
public:

	HRESULT OnProcessListComplete( WPARAM wParam, LPARAM lParam );

	CProcessMonitorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROCESSMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

private:

	CListCtrl m_ctrProcessReport;
	CListCtrl m_ctrDetailReport;
	CString m_ctrTotalModule;
	CString m_ctrTotalProcess;
	CString m_ctrTotalEngine;

private:

	void _initReports();
	void _initLister();
	void _fillDetailReport( int i_item );
	std::wstring _getResult( TModuleInfo& module );
	void OnGetdispinfoProcessList( NMHDR* pNMHDR, LRESULT* pResult );

private:

	WaProcesScanner* m_pLister;
	wa_process_list	 m_processList;
	
public:	

	CString m_ctrTotalScanned;
	afx_msg void OnHdnItemdblclickReportProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkReportProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
};
