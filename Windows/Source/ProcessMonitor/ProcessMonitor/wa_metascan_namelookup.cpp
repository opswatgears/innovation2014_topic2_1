#include "StdAfx.h"
#include "wa_metascan_namelookup.h"

std::map< std::wstring, std::wstring> WaMetascanNameLookup::m_pathDataMap;
CRITICAL_SECTION WaMetascanNameLookup::m_tCriticalSection;

void WaMetascanNameLookup::initialize()
{
	::InitializeCriticalSection( &( WaMetascanNameLookup::m_tCriticalSection ) );
}

void WaMetascanNameLookup::destroy()
{
	::DeleteCriticalSection( &( WaMetascanNameLookup::m_tCriticalSection ) );
}

std::wstring  WaMetascanNameLookup::lookup( std::wstring& data_id )
{
	std::wstring s_path( L"" );

	::EnterCriticalSection( &( WaMetascanNameLookup::m_tCriticalSection ) );
	auto found = WaMetascanNameLookup::m_pathDataMap.find( data_id );
	if ( found != WaMetascanNameLookup::m_pathDataMap.end() )
	{
		s_path = found->second;
		WaMetascanNameLookup::m_pathDataMap.erase( found );
	}    
	::LeaveCriticalSection( &( WaMetascanNameLookup::m_tCriticalSection ) );

	return s_path;
}

void WaMetascanNameLookup::insert( std::wstring& path, std::wstring& data_id )
{
	::EnterCriticalSection( &( WaMetascanNameLookup::m_tCriticalSection ) );
	WaMetascanNameLookup::m_pathDataMap.insert( std::make_pair( data_id, path ) );
	::LeaveCriticalSection( &( WaMetascanNameLookup::m_tCriticalSection ) );
}