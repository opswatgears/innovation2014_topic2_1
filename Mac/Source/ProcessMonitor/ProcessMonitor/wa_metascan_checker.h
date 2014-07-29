//
//  wa_metascan_checker.h
//  TryLoader
//
//  Created by NTViet-MAC on 7/18/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef TryLoader_wa_metascan_checker_h
#define TryLoader_wa_metascan_checker_h

#include "wa_metascan_talker.h"
#include <map>

#define WAMETASCAN_CHECKER_REPORTER    0

class WaMetascanChecker: public WaMetascanTalker
{
public:
    
    WaMetascanChecker( std::string& key, WaMetascanQueue< WaJson* >* p_wait_queue );
    virtual ~WaMetascanChecker();
    
    virtual void start();
    
private:
    
    static void*     _startWorking( void* p_arg );
    void             _setup();
    virtual void     _processResponse( std::string& s_data );
    virtual void     _processSignal( void* p_data );
    bool             _checkScanStatus( wa_wstring& data_id, wa_wstring& rest_ip );
    CFHTTPMessageRef _prepareQueryStatus( std::string& url );
    static void      _perform( void *info );
    
private:
    
    WaMetascanQueue< WaJson* >*  m_pWaitQueue;
    
    // cache
    std::map< wa_wstring, wa_wstring >    m_cache;
};

#endif
