#include "StdAfx.h"
#include "wa_process_scanner.h"
#include "wa_process_database.h"
#include "wa_message_constant.h"
#include "wa_metascan_manager.h"
#include "wa_utils_string.h"
#include <TlHelp32.h>

WaProcesScanner::WaProcesScanner( HWND h_wnd, wa_process_list* p_process_list )
	: WaWorker()
	, m_hWnd( h_wnd )
	, m_pProcessList( p_process_list )
{	
	this->m_pScanManager = new WaMetascanManager();
}


WaProcesScanner::~WaProcesScanner(void)
{
	if ( this->m_pScanManager != nullptr )
	{
		delete this->m_pScanManager;
		this->m_pScanManager = nullptr;
	}
}

bool WaProcesScanner::start()
{
	if ( this->_init() == false )
		return false;
	
	DWORD dw_threadid;
	HANDLE h_thread = CreateThread( NULL,									// default security
									0,										// default stack size
									&( WaProcesScanner::_startWorking ), // name of the thread function
									( LPVOID )this,							// no thread parameters
									0,										// default startup flags
									&dw_threadid ); 
	if ( h_thread == NULL )
		return false;

	return true;
}

DWORD WINAPI WaProcesScanner::_startWorking(LPVOID lpParam)
{
	WaProcesScanner* p_scanner = ( WaProcesScanner* )lpParam;
	p_scanner->_setupTimer();

	do 
	{		
		DWORD dwWaitResult = WaitForMultipleObjects( 2,						// number of handles in array
			p_scanner->m_hEvents,	// array of thread handles
			FALSE,					// wait until all are signaled
			INFINITE );

		switch (dwWaitResult) 
		{
			// All thread objects were signaled
		case WAIT_OBJECT_0:
			{
				p_scanner->_doScan();
				break;
			}

		case WAIT_OBJECT_0 + 1:
			{
				printf( "Uploader stops\n" );
				return 1;
			}

		default: 
			printf("WaitForMultipleObjects failed (%d)\n", GetLastError());
			return 0;
		}
	} while ( 1 );

	return 1;
}

bool WaProcesScanner::_init()
{
	CloseHandle( this->m_hEvents[WA_WORKER_EVENT_DO] );

	this->m_hEvents[WA_WORKER_EVENT_DO] = CreateWaitableTimer( NULL, FALSE, L"" );
	if ( this->m_hEvents[WA_WORKER_EVENT_DO] == NULL )
		return false;

	std::wstring key = L"f409330ead0c59cec0a07675a5f39fe8";
	this->m_pScanManager->start( key );

	return true;

}

#define _SECOND 1000000
void WaProcesScanner::_setupTimer()
{
	LARGE_INTEGER   liDueTime;
	__int64         qwDueTime = -5 * _SECOND;
	liDueTime.LowPart  = (DWORD) ( qwDueTime & 0xFFFFFFFF );
	liDueTime.HighPart = (LONG)  ( qwDueTime >> 32 );

	BOOL b_result = SetWaitableTimer( this->m_hEvents[WA_WORKER_EVENT_DO],	// Handle to the timer object
									&liDueTime,								// When timer will become signaled
									5000,									// Periodic timer interval of 5 seconds
									NULL,									// Completion routine
									NULL,									// Argument to the completion routine
									FALSE ); 
}

bool WaProcesScanner::_doScan()
{
	HANDLE h_process = INVALID_HANDLE_VALUE;
	h_process = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if ( h_process == INVALID_HANDLE_VALUE )
		return false;

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof( PROCESSENTRY32 );	

	BOOL b_res = Process32First( h_process, &entry );
	wa_process_list temp_list;

	while ( b_res == TRUE )
	{				
		PROCESSENTRY32* p_process = new PROCESSENTRY32;
		*p_process = entry;
		
		TProcessInfo brief( entry.th32ProcessID, entry.cntThreads, std::wstring( entry.szExeFile ) );
		if ( this->_getModule( brief ) == true )
		{
			this->_scanModule( brief );
			temp_list.push_back( brief );
		}
		
		b_res = Process32Next( h_process, &entry );			
		//b_res = FALSE;
	}

	*( this->m_pProcessList ) = temp_list;

	CloseHandle( h_process );	
	::PostMessageA( this->m_hWnd, WM_LIST_PROCESS_COMPLETE, NULL, NULL );

	return true;
}

bool WaProcesScanner::_getModule( TProcessInfo& process_info )
{
	std::wstring s_path;
	HANDLE h_module = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, process_info.pid );
	if ( h_module == INVALID_HANDLE_VALUE )
	{		
		int n_try = 0;
		while ( ( GetLastError() == ERROR_BAD_LENGTH ) && ( n_try < 10 ) )
		{
			h_module = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, process_info.pid );
			n_try ++;
		}

		if ( h_module == INVALID_HANDLE_VALUE )				
			return false;
	}

	MODULEENTRY32 entry;
	entry.dwSize = sizeof( MODULEENTRY32 );

	BOOL b_res = Module32First( h_module, &entry );
	while ( b_res == TRUE)
	{
		std::wstring module_name = entry.szModule;
		std::wstring exe_path = entry.szExePath;	
		WaStringUtils::toLower( exe_path );

		if ( !exe_path.empty() )
		{		
			TModuleInfo module = { module_name, exe_path, NULL };
			process_info.p_modules.push_back( module );

			b_res = Module32Next( h_module, &entry );
		}
		else
			break;				
		//b_res = FALSE;
	}
	CloseHandle( h_module );

	return true;
}

void WaProcesScanner::_scanModule( TProcessInfo& process_info )
{
	for ( int i = 0 ; i < process_info.p_modules.size() ; i++ )
	{
		TModuleInfo& info = process_info.p_modules[i];
		info.p_scan = this->m_pScanManager->scan( info.s_exe );
	}
}