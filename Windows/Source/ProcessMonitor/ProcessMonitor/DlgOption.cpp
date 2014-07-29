// DlgOption.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessMonitor.h"
#include "DlgOption.h"
#include "afxdialogex.h"


// CDlgOption dialog

IMPLEMENT_DYNAMIC(CDlgOption, CDialogEx)

CDlgOption::CDlgOption(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgOption::IDD, pParent)
{

}

CDlgOption::~CDlgOption()
{
}

void CDlgOption::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgOption, CDialogEx)
END_MESSAGE_MAP()


// CDlgOption message handlers
