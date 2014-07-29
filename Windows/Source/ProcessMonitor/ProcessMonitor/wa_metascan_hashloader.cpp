#include "wa_metascan_hashloader.h"
#include "wa_metascan_namelookup.h"
#include "wa_utils_json_factory.h"
#include "wa_utils_string.h"

#pragma comment( lib, "Winhttp" )

WaMetascanHashLoader::WaMetascanHashLoader( std::wstring& key, WaMetascanQueue< std::wstring >* p_file_queue )
: WaWorker()
, m_pFileQueue( p_file_queue )
, m_hSession( NULL )
, m_hConnect( NULL )
{
	this->m_sKey = L"apikey: " + key;
	::InitializeCriticalSection( &( this->m_tCriticalSection ) );
}


WaMetascanHashLoader::~WaMetascanHashLoader(void)
{
	if ( this->m_hSession != NULL )		WinHttpCloseHandle( this->m_hSession );
	if ( this->m_hConnect != NULL )		WinHttpCloseHandle( this->m_hConnect );
	::DeleteCriticalSection( &( this->m_tCriticalSection ) );
}

bool WaMetascanHashLoader::start()
{
	if ( this->_init() == false )
		return false;

	DWORD dw_threadid;
	HANDLE h_thread = CreateThread( NULL,									// default security
									0,										// default stack size
									&( WaMetascanHashLoader::_startWorking ), // name of the thread function
									( LPVOID )this,							// no thread parameters
									0,										// default startup flags
									&dw_threadid ); 
	if ( h_thread == NULL )
		return false;

	return true;
}

bool WaMetascanHashLoader::_init()
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

	this->m_hConnect = WinHttpConnect( this->m_hSession, 
										L"scan.metascan-online.com", 
										INTERNET_DEFAULT_HTTPS_PORT, 
										0 );
	if ( this->m_hConnect == NULL )
		return false;

	return true;
}

DWORD WINAPI WaMetascanHashLoader::_startWorking(LPVOID lpParam)
{
	WaMetascanHashLoader* p_uploader = ( WaMetascanHashLoader* )lpParam;
	do 
	{		
		DWORD dwWaitResult = WaitForMultipleObjects( 2,						// number of handles in array
													p_uploader->m_hEvents,	// array of thread handles
													FALSE,					// wait until all are signaled
													INFINITE );

		switch (dwWaitResult) 
		{
			// All thread objects were signaled
		case WAIT_OBJECT_0:
			{
				p_uploader->_doUpload();				
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

bool WaMetascanHashLoader::_doUpload()
{
	// need to rewrite
	BOOL b_result;

	while ( this->m_pFileQueue->size() > 0 )
	{	
		HINTERNET h_request = WinHttpOpenRequest( this->m_hConnect, L"POST", L"/v2/hash", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE );		
		if ( h_request == NULL )
			return false;

		b_result = WinHttpAddRequestHeaders( h_request, this->m_sKey.c_str(), ( LONG ) - 1, WINHTTP_ADDREQ_FLAG_ADD );
		if ( b_result == FALSE )
			return false;

		WINHTTP_STATUS_CALLBACK p_callback = WinHttpSetStatusCallback( h_request, ( WINHTTP_STATUS_CALLBACK )( &WaMetascanHashLoader::_asynRequest ), WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_HANDLES, NULL );
		if ( p_callback == WINHTTP_INVALID_STATUS_CALLBACK  )			
			return false;				

		int counter = 0;
		WaJson hash_array( WAAPI_JSON_ARRAY );
		while ( this->m_pFileQueue->size() > 0 && counter < 50 )
		{
			std::wstring s_path = this->m_pFileQueue->get();			
			this->m_hasher.HashFile( s_path.c_str() );
			this->m_hasher.Final();
			std::wstring s_hash;
			this->m_hasher.ReportHashStl( s_hash, CSHA1::REPORT_HEX_SHORT );
			this->m_hasher.Reset();

			hash_array.add( s_hash.c_str() );
			
			::EnterCriticalSection( &( this->m_tCriticalSection ) );
			this->m_cache.insert( std::make_pair( s_hash, s_path ) );						
			::LeaveCriticalSection( &( this->m_tCriticalSection ) );

			counter ++;
		}

		WaJson hash_object( WAAPI_JSON_OBJECT );
		hash_object.put( L"hash", hash_array );
		
		//wprintf( L"Hash:\n%s\n", hash_object.toPrettyString().c_str() );

		std::string s_data = WaStringUtils::wstrToStr( hash_object.toString() );
		THashLoaderContext* p_context = new THashLoaderContext( h_request, this, s_data );

		b_result = WinHttpSendRequest( h_request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (void*)p_context->buffer.c_str(), 
										p_context->buffer.size(), p_context->buffer.size(), ( DWORD_PTR )p_context );
		if (b_result == FALSE)
		{
			DWORD err = GetLastError();
			return false;
		}		
	}

	ResetEvent( this->m_hEvents[WA_WORKER_EVENT_DO] );

	return true;
}

void WaMetascanHashLoader::_processResponse( WaJson* p_response )
{
	// need to rewrite
	//wa_wstring s = p_response->toPrettyString();
	//wprintf( L"Hash Result:\n%s\n", s.c_str() );

	for ( size_t i = 0 ; i < p_response->size() ; i++ )
	{
		WaJson json_entry = p_response->at( i );
		wa_wstring s_hash;
		json_entry.get( L"hash", s_hash );				

		wa_wstring s_dataid;

		::EnterCriticalSection( &( this->m_tCriticalSection ) );
		auto found = this->m_cache.find( s_hash );
		::LeaveCriticalSection( &( this->m_tCriticalSection ) );

		if ( WAAPI_FAILED( json_entry.get( L"data_id", s_dataid ) ) )
		{			
			if ( found != this->m_cache.end() )
			{		
				this->m_pWorker[WAMETASCAN_HASHLOADER_UPLOADER]->signal( ( void* )&( found->second ) );						
			}			
		}
		else
		{			
			WaMetascanNameLookup::insert( found->second, s_dataid );

			WaJson* p_detail_json = new WaJson( WAAPI_JSON_OBJECT );

			p_detail_json->put( L"rest_ip", L"scan.metascan-online.com/v2" );
			p_detail_json->put( L"data_id", s_dataid.c_str() );

			this->m_pWorker[WAMETASCAN_HASHLOADER_CHECKER]->signal( ( void* )p_detail_json );			
		}
		//this->m_cache.erase( found );

		/*auto found = this->m_cache.find( s_hash );
		if ( found != this->m_cache.end() )
		{			
			wprintf( L"File :%s, hash: %s\n", found->second.c_str(), s_hash.c_str() );
		}*/
	}
	delete p_response;
}

void WaMetascanHashLoader::_processSignal( void* p_data )
{
	std::wstring* p_str_data = ( std::wstring* )p_data;
	this->m_pFileQueue->put( *p_str_data );
}

void WaMetascanHashLoader::_asynRequest( HINTERNET h_request, DWORD_PTR dw_context, DWORD dw_status, LPVOID lpv_status_info, DWORD dw_status_info_lenghth )
{
	THashLoaderContext* p_context = ( THashLoaderContext* )dw_context;

	switch (dw_status)
	{
	case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
		{
			if ( WinHttpReceiveResponse( p_context->h_request, 0 ) == FALSE )
				printf( " - Error in sending request\n" );

			p_context->buffer.clear();

			break;
		}
	case WINHTTP_CALLBACK_FLAG_HEADERS_AVAILABLE:
		{
			DWORD dw_byte_avalable;
			WinHttpQueryDataAvailable(p_context->h_request, &dw_byte_avalable);

			break;
		}
	case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:	
		{
			DWORD dw_size = *( DWORD* )lpv_status_info;

			if (dw_size == 0)		// There is no data, reading is complete
			{
				WaJsonFactory factory;
				WaJson* p_response = new WaJson( WAAPI_JSON_NULL );
				if (WAAPI_SUCCESS( factory.create(WaStringUtils::bytesToWString(
								( BYTE* )p_context->buffer.c_str(), p_context->buffer.size() ),*p_response ) ) )
				{
					p_context->p_uploader->_processResponse( p_response );

					if (p_context->h_request != NULL)		WinHttpCloseHandle(p_context->h_request);				
				}
				else
					printf("ERROR in creating json\n");
			}
			else
			{
				// allocate a new memory and put it into WinHttpReadData 
				char* p_temp_buffer = new char[dw_size];
				ZeroMemory( p_temp_buffer, dw_size );

				if (WinHttpReadData( p_context->h_request, ( LPVOID )p_temp_buffer, dw_size, NULL ) == FALSE)
				{
					DWORD err = GetLastError();
					delete[] p_temp_buffer;
				}
			}
			break;
		}		
	case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
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
			WinHttpSetStatusCallback( p_context->h_request, NULL, 0, 0 );			
			WinHttpCloseHandle( p_context->h_request );

			delete p_context;
			break;
		}
	default:
		break; 
	}
}
