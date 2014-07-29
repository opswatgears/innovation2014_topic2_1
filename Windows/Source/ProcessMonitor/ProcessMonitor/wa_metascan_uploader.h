#pragma once

#include "wa_worker.h"
#include "wa_metascan_queue.h"
#include "wa_utils_json.h"
#include <winhttp.h>
#include <string>

#define WAMETASCAN_UPLOADER_CHECKER	0

class WaMetascanUploader: public WaWorker
{
	struct TUploaderContext
	{
		HANDLE h_request;		
		WaMetascanUploader* p_uploader;
		std::string buffer;

		TUploaderContext( HANDLE request, WaMetascanUploader* uploader )
		{
			h_request = request;
			p_uploader = uploader;			
		}		
	};

public:
	WaMetascanUploader( std::wstring& key, WaMetascanQueue< std::wstring >* p_file_queue );
	virtual ~WaMetascanUploader(void);	
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
	bool					_readContent( std::wstring& file_name, std::string& content );	

private:

	WaMetascanQueue< std::wstring >*	m_pFileQueue;	
	HINTERNET							m_hSession;
	HINTERNET							m_hConnect;
	std::wstring						m_sKey;
};

