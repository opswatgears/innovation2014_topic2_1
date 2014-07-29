#include "wa_metascan_reporter.h"
#include "wa_metascan_namelookup.h"
#include "wa_api_error_codes.h"


WaMetascanReporter::WaMetascanReporter( WaMetascanQueue< WaJson* >* p_result_queue, WaProcessDatabase* p_database )
	: WaWorker()
	, m_pResultQueue( p_result_queue )
	, m_pDatabase( p_database )
{

}


WaMetascanReporter::~WaMetascanReporter(void)
{
}

bool WaMetascanReporter::start()
{
	DWORD dw_threadid;
	HANDLE h_thread = CreateThread( NULL,									// default security
									0,										// default stack size
									&( WaMetascanReporter::_startWorking ), // name of the thread function
									( LPVOID )this,							// no thread parameters
									0,										// default startup flags
									&dw_threadid ); 
	if ( h_thread == NULL )
		return false;

	return true;
}

DWORD WINAPI WaMetascanReporter::_startWorking( LPVOID lpParam )
{
	WaMetascanReporter* p_reporter = ( WaMetascanReporter* )lpParam;
	do 
	{		
		DWORD dwWaitResult = WaitForMultipleObjects( 2,						// number of handles in array
													p_reporter->m_hEvents,  // array of thread handles
													FALSE,					// wait until all are signaled
													INFINITE );

		switch (dwWaitResult) 
		{
			// All thread objects were signaled
		case WAIT_OBJECT_0:
			p_reporter->_reportResult();
			break;

		case WAIT_OBJECT_0 + 1:
			{
				//printf( "Reporter stops, count: %d\n", p_reporter->count );
				return 1;
			}			

		default: 
			printf("WaitForMultipleObjects failed (%d)\n", GetLastError());
			return 0;
		} 
	} while ( 1 );

	return 1;
}

void WaMetascanReporter::_reportResult()
{
	while ( this->m_pResultQueue->size() > 0 )
	{
		WaJson* p_result_json = this->m_pResultQueue->get();	
		wa_wstring s_msg;

		if ( WAAPI_SUCCESS( p_result_json->get( L"err", s_msg ) ) )
		{
			wprintf( L"Error: %s\n",  s_msg.c_str() );
		}
		else
		{
			wa_wstring data_id;
			p_result_json->get( L"data_id", data_id );               
			
			std::wstring s_path = WaMetascanNameLookup::lookup( data_id );

			if ( s_path.empty() )
			{
				WaJson info_json;
				p_result_json->get( L"file_info", info_json );				
				info_json.get( L"display_name", s_path );				
			}

			this->m_pDatabase->insert( s_path, p_result_json );
		}
	}

	ResetEvent( this->m_hEvents[WA_WORKER_EVENT_DO] );
}

void WaMetascanReporter::_processSignal( void* p_data )
{
	WaJson* p_json_data = ( WaJson* )p_data;
	this->m_pResultQueue->put( p_json_data );
}