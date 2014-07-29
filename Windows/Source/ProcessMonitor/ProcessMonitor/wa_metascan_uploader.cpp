#include "StdAfx.h"
#include "wa_metascan_uploader.h"
#include "wa_utils_json_factory.h"
#include "wa_utils_string.h"

#pragma comment( lib, "Winhttp" )

void logUpload( std::string data )
{
	FILE* fp = fopen( "E:\\Viet\\4V\\Lab\\ProcessMonitor\\ProcessMonitor\\uploader.txt", "a+" );
	if ( fp != NULL )
	{
		fprintf( fp, "%s\n", data.c_str() );
		fclose( fp );
	}
}

WaMetascanUploader::WaMetascanUploader( std::wstring& key, WaMetascanQueue< std::wstring >* p_file_queue )
	: WaWorker()
	, m_pFileQueue( p_file_queue )
	, m_hSession( NULL )
	, m_hConnect( NULL )
{	
	this->m_sKey = L"apikey: " + key;
}


WaMetascanUploader::~WaMetascanUploader(void)
{	
	if ( this->m_hSession != NULL )		WinHttpCloseHandle( this->m_hSession );
	if ( this->m_hConnect != NULL )		WinHttpCloseHandle( this->m_hConnect );	
}

bool WaMetascanUploader::start()
{
	if ( this->_init() == false )
		return false;

	DWORD dw_threadid;
	HANDLE h_thread = CreateThread( NULL,									// default security
									0,										// default stack size
									&( WaMetascanUploader::_startWorking ), // name of the thread function
									( LPVOID )this,							// no thread parameters
									0,										// default startup flags
									&dw_threadid ); 
	if ( h_thread == NULL )
		return false;

	return true;
}

bool WaMetascanUploader::_init()
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

DWORD WINAPI WaMetascanUploader::_startWorking(LPVOID lpParam)
{
	WaMetascanUploader* p_uploader = ( WaMetascanUploader* )lpParam;
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

bool WaMetascanUploader::_doUpload()
{
	BOOL b_result;

	//while ( this->m_pFileQueue->size() > 0 )
	{
		std::wstring s_path = this->m_pFileQueue->get();

		//WinHttpConnect does not result in an actual connection to the HTTP server until a request is made for a specific resource.
		// we have to set WINHTTP_FLAG_SECURE in final parameter, otherwise we will fail in sending and receiving data
		HINTERNET h_request = WinHttpOpenRequest( this->m_hConnect, 
												L"POST", L"/v2/file", 
												NULL, 
												WINHTTP_NO_REFERER, 
												WINHTTP_DEFAULT_ACCEPT_TYPES, 
												WINHTTP_FLAG_SECURE );
		if ( h_request == NULL )
			return false;
		
		WINHTTP_STATUS_CALLBACK p_callback = WinHttpSetStatusCallback( h_request, 
																		( WINHTTP_STATUS_CALLBACK )( &WaMetascanUploader::_asynRequest ), 
																		WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_HANDLES, 
																		NULL );
		if ( p_callback == WINHTTP_INVALID_STATUS_CALLBACK  )			
			return false;
 
		b_result = WinHttpAddRequestHeaders( h_request, this->m_sKey.c_str(), ( LONG ) - 1, WINHTTP_ADDREQ_FLAG_ADD );
		if ( b_result == FALSE )
			return false;

		std::wstring s_filename = L"filename: " + s_path;
		b_result = WinHttpAddRequestHeaders( h_request, s_filename.c_str(), ( LONG ) - 1, WINHTTP_ADDREQ_FLAG_ADD );
		if ( b_result == FALSE )
			return false;
		
		TUploaderContext* p_context = new TUploaderContext( h_request, this );

		if ( this->_readContent( s_path, p_context->buffer ) == false )
			return false;		
		
		int size = p_context->buffer.size();
		b_result = WinHttpSendRequest( h_request, 
										WINHTTP_NO_ADDITIONAL_HEADERS, 
										0, 
										(void*)p_context->buffer.c_str(), 
										size, 
										size, 
										( DWORD_PTR )p_context );
		if (b_result == FALSE)
		{
			DWORD err = GetLastError();
			return false;
		}		
	}

	ResetEvent( this->m_hEvents[WA_WORKER_EVENT_DO] );

	return true;
}

bool WaMetascanUploader::_readContent( std::wstring& file_name, std::string& content )
{
	FILE* fp = NULL;
	_wfopen_s( &fp, file_name.c_str(), L"rb" );

	if ( fp != NULL )
	{
		char data[1024];
		while( !feof( fp ) )
		{
			DWORD n_read;
			n_read = fread(data, sizeof( char ), 1024, fp);			
			content.append(data, n_read );
		}
		
		fclose( fp );

		return true;
	}

	return false;
}

void WaMetascanUploader::_processResponse( WaJson* p_response )
{
	wa_wstring s = p_response->toPrettyString();
		
	this->m_pWorker[WAMETASCAN_UPLOADER_CHECKER]->signal( p_response );
}

void WaMetascanUploader::_asynRequest( HINTERNET h_request,
									DWORD_PTR dw_context,
									DWORD dw_status,
									LPVOID lpv_status_info,
									DWORD dw_status_info_lenghth )
{
	TUploaderContext* p_context = ( TUploaderContext* )dw_context;

	switch (dw_status)
	{
	case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
	{
		p_context->buffer.clear();

		if ( WinHttpReceiveResponse( p_context->h_request, 0 ) == FALSE )
			logUpload( " - Error in sending request\n" );

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
											( BYTE* )p_context->buffer.c_str(),
											p_context->buffer.size() ),
											*p_response ) ) )
			{
				p_context->p_uploader->_processResponse( p_response );

				if (p_context->h_request != NULL)		WinHttpCloseHandle(p_context->h_request);				
			}
			else
				logUpload( p_context->buffer );
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

void WaMetascanUploader::_processSignal( void* p_data )
{
	std::wstring* p_str_data = ( std::wstring* )p_data;
	this->m_pFileQueue->put( *p_str_data );
}