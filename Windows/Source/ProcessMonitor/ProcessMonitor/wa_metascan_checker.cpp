#include "wa_metascan_checker.h"
#include "wa_utils_json_factory.h"
#include "wa_api_error_codes.h"
#include "wa_utils_string.h"

#pragma comment( lib, "Winhttp" )

void logChecker( std::string data )
{
	FILE* fp = fopen( "E:\\Viet\\4V\\Lab\\ProcessMonitor\\ProcessMonitor\\checker.txt", "a+" );
	if ( fp != NULL )
	{
		fprintf( fp, "%s\n", data.c_str() );
		fclose( fp );
	}
}

WaMetascanChecker::WaMetascanChecker( std::wstring& key, WaMetascanQueue< WaJson* >* p_wait_queue )
	: WaWorker() 
	, m_pWaitQueue( p_wait_queue )
	, m_hSession( NULL )
{
	this->m_sKey = L"apikey: " + key;
}


WaMetascanChecker::~WaMetascanChecker(void)
{
	if ( this->m_hSession != NULL )		WinHttpCloseHandle( this->m_hSession );
}

bool WaMetascanChecker::start()
{
	if ( this->_init() == false )
		return false;

	DWORD dw_threadid;
	HANDLE h_thread = CreateThread( NULL,              // default security
									0,                 // default stack size
									&( WaMetascanChecker::_startWorking ),        // name of the thread function
									( LPVOID )this,              // no thread parameters
									0,                 // default startup flags
									&dw_threadid ); 
	if ( h_thread == NULL )
		return false;

	return true;
}

bool WaMetascanChecker::_init( )
{
	this->m_hSession = WinHttpOpen( L"Metascan agent/1.0", 
									WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
									WINHTTP_NO_PROXY_NAME, 
									WINHTTP_NO_PROXY_BYPASS, 
									WINHTTP_FLAG_ASYNC );
	if ( this->m_hSession == NULL )
		return false;

	BOOL b_result;
	b_result = WinHttpSetTimeouts( this->m_hSession, 10000, 100000, 300000, 300000 );
	if ( b_result == FALSE )
		return false;	

	return true;
}

DWORD WINAPI WaMetascanChecker::_startWorking( LPVOID lpParam )
{
	WaMetascanChecker* p_checker = ( WaMetascanChecker* )lpParam;
	do 
	{		
		DWORD dwWaitResult = WaitForMultipleObjects( 2,						// number of handles in array
													p_checker->m_hEvents,   // array of thread handles
													FALSE,					// wait until all are signaled
													INFINITE );

		switch (dwWaitResult) 
		{
			// All thread objects were signaled
		case WAIT_OBJECT_0:
			p_checker->_doCheck();
			break;

		case WAIT_OBJECT_0 + 1:
			{
				printf( "Checker stops\n" );
				return 1;
			}			

		default: 
			printf("WaitForMultipleObjects failed (%d)\n", GetLastError());
			return 0;
		} 
	} while ( 1 );
	
	return 1;
}

bool WaMetascanChecker::_doCheck()
{
	while ( this->m_pWaitQueue->size() > 0 )
	{
		WaJson* p_info_json = this->m_pWaitQueue->get();

		wa_wstring s_data_id;
		p_info_json->get( L"data_id", s_data_id );

		wa_wstring s_rest_ip;
		p_info_json->get( L"rest_ip", s_rest_ip );

		delete p_info_json;		

		if ( this->_doSend( s_rest_ip, s_data_id ) == true )
			this->m_cache.insert( std::make_pair( s_data_id, s_rest_ip ) );
	}
	ResetEvent( this->m_hEvents[WA_WORKER_EVENT_DO] );

	return true;
}

bool WaMetascanChecker::_doSend( std::wstring& rest_ip, std::wstring& data_id )
{
	BOOL b_result;
	std::wstring s_url;
	std::wstring s_resource;
	this->_breakrestip (rest_ip, s_url, s_resource );

	HINTERNET h_connect = WinHttpConnect( this->m_hSession,
										s_url.c_str(),
										INTERNET_DEFAULT_HTTPS_PORT,
										0);
	if (h_connect == NULL)
		return false;

	s_resource = L"/" + s_resource + L"/file/" + data_id;
	HINTERNET h_request = WinHttpOpenRequest( h_connect,
											L"GET", s_resource.c_str(),
											NULL,
											WINHTTP_NO_REFERER,
											WINHTTP_DEFAULT_ACCEPT_TYPES,
											WINHTTP_FLAG_SECURE);
	if ( h_request == NULL )
		return false;

	WINHTTP_STATUS_CALLBACK p_callback;
	p_callback = WinHttpSetStatusCallback( h_request,
										(WINHTTP_STATUS_CALLBACK)( &WaMetascanChecker::_asynRequest ),
										WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_HANDLES,
										NULL);
	if (p_callback == WINHTTP_INVALID_STATUS_CALLBACK)
		return false;	

	b_result = WinHttpAddRequestHeaders( h_request, this->m_sKey.c_str(), (LONG)-1, WINHTTP_ADDREQ_FLAG_ADD );
	if ( b_result == FALSE )
		return false;

	TCheckerContext* p_context = new TCheckerContext( h_connect, h_request, this );
	
	b_result = WinHttpSendRequest( h_request,
									WINHTTP_NO_ADDITIONAL_HEADERS,
									0,
									WINHTTP_NO_REQUEST_DATA,
									0,
									0,
									( DWORD_PTR )p_context );
	if (b_result == FALSE)
	{
		DWORD err = GetLastError();
		return false;
	}

	return true;
}

void WaMetascanChecker::_processResponse( WaJson* p_response )
{
	WaJson result_json;
	int percentage;

	if (( WAAPI_SUCCESS( p_response->get( L"scan_results", result_json ) ) &&
		( WAAPI_SUCCESS( result_json.get( L"progress_percentage", percentage ) ) ) ) )
	{
		if ( percentage < 100 )
		{
			printf( " - Progress: %d\n", percentage );
			wa_wstring data_id;

			if ( WAAPI_SUCCESS(p_response->get( L"data_id", data_id ) ) )
			{
				auto found = this->m_cache.find( data_id );
				if ( found != this->m_cache.end() )
				{		
					Sleep( 150 );
					this->_doSend( found->second, data_id );
				}				
			}
			delete p_response;			
		}
		else
		{
			//wa_wstring s = p_response->toPrettyString();
			//wprintf( L"Checker:\n%s\n", s.c_str() );									  			
			wa_wstring data_id;
			p_response->get(L"data_id", data_id);
			this->m_cache.erase( data_id );

			this->m_pWorker[WAMETASCAN_CHECKER_REPORTER]->signal( p_response );
		}
	}	
}

void WaMetascanChecker::_breakrestip( std::wstring& rest_ip, std::wstring& url, std::wstring& other )
{
	wchar_t p_directory[_MAX_DIR];
	wchar_t p_filename[_MAX_FNAME];

	_wsplitpath( rest_ip.c_str(), NULL, p_directory, p_filename, NULL );
	std::wstring s_temp = p_directory;
	other = p_filename;

	size_t i = s_temp.length() - 1;
	for ( ; i > 0 ; i-- )
	{
		if ( s_temp[i] == L':')
			break;
	}
	
	if ( i == 0 )
		url = s_temp.substr( 0, s_temp.length() - 1 );
	else
		url = s_temp.substr( 0, i );
}

void WaMetascanChecker::_asynRequest( HINTERNET h_request,
										DWORD_PTR dw_context,
										DWORD dw_status,
										LPVOID lpv_status_info,
										DWORD dw_status_info_lenghth )
{
	TCheckerContext* p_context = ( TCheckerContext* )dw_context;

	switch (dw_status)
	{
	case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
	{
		p_context->buffer.clear();

		if ( WinHttpReceiveResponse( p_context->h_request, 0 ) == FALSE )
			logChecker( " Error in sending request\n" );
		break;
	}
	case WINHTTP_CALLBACK_FLAG_HEADERS_AVAILABLE:
	{
		DWORD dw_byte_avalable;
		WinHttpQueryDataAvailable(p_context->h_request, &dw_byte_avalable);
		// This function will cause WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE event

		break;
	}
	case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:	
	{
		DWORD dw_size = *( DWORD* )lpv_status_info;

		if ( dw_size == 0 )		// There is no data, reading is complete
		{					
			WaJsonFactory factory;
			WaJson* p_response = new WaJson(WAAPI_JSON_NULL);
			if ( WAAPI_SUCCESS( factory.create( WaStringUtils::bytesToWString( 
								(BYTE*)p_context->buffer.c_str(), 
								p_context->buffer.size() ), 
								*p_response ) ) )
			{
				p_context->p_checker->_processResponse(p_response);

				if (p_context->h_request != NULL)		WinHttpCloseHandle(p_context->h_request);
				if (p_context->h_connect != NULL)		WinHttpCloseHandle(p_context->h_request);
			}
			else
				logChecker( "ERROR in creating json" );
		}
		else
		{
			// allocate a new memory and put it into WinHttpReadData 
			char* p_temp_buffer = new char[dw_size];
			ZeroMemory( p_temp_buffer, dw_size );

			if ( WinHttpReadData( p_context->h_request, ( LPVOID )p_temp_buffer, dw_size, NULL ) == FALSE )
			{
				DWORD err = GetLastError();
				delete[] p_temp_buffer;
			}
		}			
		break;
	}		
	case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:		// complete a block of data
		{		
			if ( dw_status_info_lenghth != 0 )
			{
				// append read data to buffer and free the temp buffer
				p_context->buffer.append( ( char* )lpv_status_info, dw_status_info_lenghth );				
				delete[] lpv_status_info;

				// Query for more information
				DWORD dw_byte_avalable;
				WinHttpQueryDataAvailable(p_context->h_request, &dw_byte_avalable);
				// This function will cause WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE event
			}
		
			break;
		}
	case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
		{
			if (p_context->h_request != NULL)
			{
				WinHttpSetStatusCallback(p_context->h_request, NULL, 0, 0);
				WinHttpCloseHandle(p_context->h_request);
			}

			if (p_context->h_connect != NULL)		
				WinHttpCloseHandle(p_context->h_request);

			delete p_context;

			break;
		}
	default:
		break; 
	}
}

void WaMetascanChecker::_processSignal( void* p_data )
{
	WaJson* p_json_data = ( WaJson* )p_data;
	this->m_pWaitQueue->put( p_json_data );
}