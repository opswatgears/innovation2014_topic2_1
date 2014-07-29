//
//  wa_metascan_reporter.h
//  TryLoader
//
//  Created by NTViet-MAC on 7/19/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef TryLoader_wa_metascan_reporter_h
#define TryLoader_wa_metascan_reporter_h

#include "wa_worker.h"
#include "wa_metascan_queue.h"
#include "wa_metascan_cache.h"
#include <string>
#include <vector>

class WaMetascanReporter: public WaWorker
{
public:
    
    WaMetascanReporter( WaMetascanQueue< WaJson* >* p_result_queue, WaMetascanCache* p_cache );
    virtual ~WaMetascanReporter();
    
    virtual void start();
    
private:
    
    static void*     _startWorking( void* p_arg );
    void             _setup();
    void             _report( WaJson* p_json );
    static void      _perform( void *info );
    virtual void     _processSignal( void* p_data );
    
private:
    
    WaMetascanQueue< WaJson* >*     m_pResultQueue;
    WaMetascanCache*                m_pCache;
};

#endif
