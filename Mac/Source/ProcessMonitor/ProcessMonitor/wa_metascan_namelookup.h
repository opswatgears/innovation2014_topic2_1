//
//  wa_metascan_namelookup.h
//  ProcessMonitor
//
//  Created by NTViet-MAC on 7/26/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef ProcessMonitor_wa_metascan_namelookup_h
#define ProcessMonitor_wa_metascan_namelookup_h

#include <pthread.h>
#include <string>
#include <map>

class WaMetascanNameLookup
{
public:
    
    static void initialize();
    static void destroy();
    
    static std::string  lookup( std::string& data_id );
    static void         insert( std::string& path, std::string& data_id );
    
private:
    
    WaMetascanNameLookup() {};
    ~WaMetascanNameLookup() {};
    
    static std::map< std::string, std::string> m_pathDataMap;       // data_id <-> process_path
    static pthread_mutex_t m_tMutex;
};

#endif
