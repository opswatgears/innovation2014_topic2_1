//
//  wa_process_info.h
//  ShowProcess
//
//  Created by NTViet-MAC on 7/25/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef ShowProcess_wa_process_info_h
#define ShowProcess_wa_process_info_h

#include <map>
#include <string>
#include <vector>
#include "wa_metascan_def.h"

struct TCacheEntry
{
    //int pid; use as lookup key
    bool            should_clear;
    TScanDetail*    p_detail;
    
    TCacheEntry( TScanDetail* detail )
    {
        should_clear = false;
        p_detail = detail;
    }
};

typedef std::map<int, TCacheEntry*> wa_process_cache;

struct TProcessInfo
{
    int pid;
    std::string process_name;
    std::string process_path;
    TScanDetail* scan_detail;   // just reference, never delete
    
    TProcessInfo( int i_pid, std::string& s_name, std::string& s_path, TScanDetail* p_detail )
    {
        pid = i_pid;
        process_name = s_name;
        process_path = s_path;
        scan_detail = p_detail;
    }
};

typedef std::vector< TProcessInfo >  wa_process_list;

struct TSignal
{
    CFRunLoopRef        cfRunner;
    CFRunLoopSourceRef  cfSource;
    wa_process_list     processList;
    
    void signal()
    {
        CFRunLoopSourceSignal( cfSource );
        CFRunLoopWakeUp( cfRunner );
    }
};

#endif
