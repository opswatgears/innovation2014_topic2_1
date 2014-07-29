//
//  wa_metascan_cache.h
//  ProcessMonitor
//
//  Created by NTViet-MAC on 7/26/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef ProcessMonitor_wa_metascan_cache_h
#define ProcessMonitor_wa_metascan_cache_h

#include <map>
#include <string>
#include <pthread.h>
#include "wa_metascan_def.h"

class WaMetascanCache
{
public:
    
    WaMetascanCache();
    ~WaMetascanCache();
    
    TScanDetail*    get( std::string& process_path, bool& do_scan );
    void            insert( std::string& process_path, WaJson* detail_json );
    
private:
    
    std::map< std::string, TScanDetail* >    m_cachMap;
    pthread_mutex_t m_tMutex;
};

#endif
