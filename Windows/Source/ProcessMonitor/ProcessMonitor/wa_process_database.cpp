#include "StdAfx.h"
#include "wa_process_database.h"
#include "wa_utils_string.h"

WaProcessDatabase::WaProcessDatabase(void)
{
	::InitializeCriticalSection( &( this->m_tCriticalSection ) );
}


WaProcessDatabase::~WaProcessDatabase(void)
{
	EnterCriticalSection( &( this->m_tCriticalSection ) );
	for ( auto iter = this->m_pMap.begin() ;
			iter != this->m_pMap.end() ;
			iter ++ )
	{
		delete iter->second->p_detail;
		delete iter->second;
	}
	this->m_pMap.clear();
	LeaveCriticalSection( &( this->m_tCriticalSection ) );

	::DeleteCriticalSection( &( this->m_tCriticalSection ) );
}

void WaProcessDatabase::insert( std::wstring& s_path, WaJson* p_detail )
{
	EnterCriticalSection( &( this->m_tCriticalSection ) );
	auto found = this->m_pMap.find( s_path );
	if ( found != this->m_pMap.end() )
	{
		WaJson result_json;
		p_detail->get( L"scan_results", result_json );

		wa_wstring s_result;
		result_json.get( L"scan_all_result_a", s_result );

		found->second->scan_status = s_result;
		result_json.get( L"total_avs", found->second->n_engine );

		found->second->p_detail = p_detail;
	}
	LeaveCriticalSection( &( this->m_tCriticalSection ) );
}

TScanDetail* WaProcessDatabase::get( std::wstring& s_path, bool& do_scan )
{
	TScanDetail* p_detail = nullptr;

	EnterCriticalSection( &( this->m_tCriticalSection ) );
	auto found = this->m_pMap.find( s_path );
	if ( found != this->m_pMap.end() )
	{
		p_detail = found->second;
		do_scan = false;
	}
	else
	{
		p_detail = new TScanDetail;
		this->m_pMap.insert( std::make_pair( s_path, p_detail ) );
		do_scan = true;
	}
	LeaveCriticalSection( &( this->m_tCriticalSection ) );

	return p_detail;
}
