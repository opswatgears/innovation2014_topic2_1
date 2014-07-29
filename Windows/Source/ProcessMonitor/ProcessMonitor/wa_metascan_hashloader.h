#pragma once

#include "wa_worker.h"
#include "wa_metascan_queue.h"
#include "wa_utils_json.h"
#include <winhttp.h>
#include <string>
#include <map>
#include "SHA1.h"

#define WAMETASCAN_HASHLOADER_UPLOADER	0
#define WAMETASCAN_HASHLOADER_CHECKER	1

class WaMetascanHashLoader: public WaWorker
{
	struct THashLoaderContext
	{		
		HINTERNET	h_request;	

		WaMetascanHashLoader* p_uploader;
		std::string buffer;

		THashLoaderContext( HINTERNET request, WaMetascanHashLoader* uploader, std::string& data )
		{			
			h_request = request;
			p_uploader = uploader;			
			buffer = data;
		}		
	};

public:

	WaMetascanHashLoader( std::wstring& key, WaMetascanQueue< std::wstring >* p_file_queue );
	virtual ~WaMetascanHashLoader(void);	

	virtual bool start();	

private:

	static DWORD WINAPI		_startWorking( LPVOID lpParam );
	static void	CALLBACK	_asynRequest( HINTERNET h_request, 
										DWORD_PTR dw_context, 
										DWORD dw_status, 
										LPVOID lpv_status_info,
										DWORD dw_status_info_lenghth );	
	void					_processResponse( WaJson* p_response );
	void					_processSignal( void* p_data );

	bool					_init();
	bool					_doUpload();			

private:

	WaMetascanQueue< std::wstring >*	m_pFileQueue;		
	HINTERNET							m_hSession;
	HINTERNET							m_hConnect;
	std::wstring						m_sKey;
	CSHA1								m_hasher;
	std::map< wa_wstring, wa_wstring >  m_cache;		// hash code <-> process path
	CRITICAL_SECTION	m_tCriticalSection;
};

