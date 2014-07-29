#pragma once
#include "wa_worker.h"
#include "wa_metascan_queue.h"
#include "wa_utils_json.h"
#include <map>
#include <winhttp.h>

#define WAMETASCAN_CHECKER_REPORTER		0

class WaMetascanChecker: public WaWorker
{
	struct TCheckerContext
	{
		HANDLE h_connect;
		HANDLE h_request;		
		std::string buffer;
		WaMetascanChecker* p_checker;

		TCheckerContext( HANDLE connect, HANDLE request, WaMetascanChecker* checker )
		{
			h_connect = connect;
			h_request = request;			
			p_checker = checker;
		}
	};

public:

	WaMetascanChecker( std::wstring& key, WaMetascanQueue< WaJson* >* p_wait_queue );
	virtual ~WaMetascanChecker(void);

	virtual bool start();	

private:

	bool					_init();
	static DWORD WINAPI		_startWorking( LPVOID lpParam );
	bool					_doCheck();
	void					_processResponse( WaJson* p_response );
	void					_processSignal( void* p_data );

	void					_breakrestip( std::wstring& rest_ip, std::wstring& url, std::wstring& other );
	bool					_doSend( std::wstring& rest_ip, std::wstring& data_id );
	static void	CALLBACK	_asynRequest(HINTERNET h_connect,
									DWORD_PTR dw_context,
									DWORD dw_status,
									LPVOID lpv_status_info,
									DWORD dw_status_info_lenghth);
private:
	
	WaMetascanQueue< WaJson* >*			m_pWaitQueue;	
	HINTERNET							m_hSession;	
	std::wstring						m_sKey;

	// cache
	std::map< wa_wstring, wa_wstring >    m_cache;	
};

