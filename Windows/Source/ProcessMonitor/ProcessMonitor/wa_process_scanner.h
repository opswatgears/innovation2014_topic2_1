#pragma once

#include "wa_worker.h"
#include <string>
#include <vector>
#include "wa_process_info.h"
#include "wa_metascan_manager.h"


class WaProcesScanner: public WaWorker
{
public:
	WaProcesScanner( HWND h_wnd, wa_process_list* p_process_list );
	virtual ~WaProcesScanner(void);

	virtual bool start();

private:

	static DWORD WINAPI		_startWorking( LPVOID lpParam );
	//static VOID CALLBACK	_timerSignal( LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue );

	bool _init();
	void _setupTimer();
	bool _doScan();
	bool _getModule( TProcessInfo& process_info );
	void _scanModule( TProcessInfo& process_info );

private:

	HWND m_hWnd;
	wa_process_list*	m_pProcessList;		
	WaMetascanManager*	m_pScanManager;
};

