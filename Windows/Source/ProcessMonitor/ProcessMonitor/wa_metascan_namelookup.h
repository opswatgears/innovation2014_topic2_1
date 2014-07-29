#pragma once
#include <map>
#include <string>

class WaMetascanNameLookup
{
public:

	static void initialize();
	static void destroy();

	static std::wstring lookup( std::wstring& data_id );
	static void         insert( std::wstring& path, std::wstring& data_id );

private:

	WaMetascanNameLookup() {};
	~WaMetascanNameLookup() {};

	static std::map< std::wstring, std::wstring> m_pathDataMap;       // data_id <-> process_path
	static CRITICAL_SECTION	m_tCriticalSection;
};
