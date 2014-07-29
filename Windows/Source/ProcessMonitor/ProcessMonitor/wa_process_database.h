#pragma once
#include <string>
#include <vector>
#include <map>
#include "wa_utils_json.h"


struct TScanDetail
{	
	std::wstring	scan_status;
	int				n_engine;
	WaJson*			p_detail;

	TScanDetail()
	{		
		scan_status = L"scanning";
		n_engine = 0;
		p_detail = NULL;
	}
};

typedef std::map< std::wstring, TScanDetail* > wa_scan_map;		// exe path <-> TScanDetail

class WaProcessDatabase
{
public:

	void			insert( std::wstring& s_path, WaJson* p_detail );	
	TScanDetail*	get( std::wstring& s_path, bool& do_scan );

	WaProcessDatabase(void);
	~WaProcessDatabase(void);
	
private:

	CRITICAL_SECTION			m_tCriticalSection;
	wa_scan_map					m_pMap;
};

