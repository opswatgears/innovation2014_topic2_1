#pragma once
#include <string>
#include <vector>
#include "wa_process_database.h"

// ----------------- For listing processes and their modules
struct TModuleInfo
{
	std::wstring s_module;
	std::wstring s_exe;
	TScanDetail* p_scan;	// just reference, never deleted
};

struct TProcessInfo
{
	int pid;						
	int nthread;
	std::wstring process_name;

	std::vector< TModuleInfo > p_modules;

	TProcessInfo( int i_pid, int n_thread, std::wstring& s_name )
	{
		pid = i_pid;
		nthread = n_thread;
		process_name = s_name;		
	}
};

typedef std::vector< TProcessInfo >  wa_process_list;

//
