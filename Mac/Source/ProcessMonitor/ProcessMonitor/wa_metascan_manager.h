//
//  wa_metascan_manager.h
//  TryLoader
//
//  Created by NTViet-MAC on 7/18/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef TryLoader_wa_metascan_manager_h
#define TryLoader_wa_metascan_manager_h

#include "wa_worker.h"
#include "wa_metascan_queue.h"
#include "wa_metascan_cache.h"

enum WAMETASCAN_WORKER
{
    WAMETASCAN_REPORTER = 0,
    WAMETASCAN_CHECKER,
    WAMETASCAN_UPLOADER,
    WAMETASCAN_HASHLOADER,
    WAMETASCAN_TOTAL
};

class WaMetascanManager
{
public:
    
    WaMetascanManager( std::string& key );
    ~WaMetascanManager();
    
    TScanDetail* scan( std::string& path );
    void stop();
    
private:
    
    WaMetascanQueue< std::string >*     m_pFileQueue;
    WaMetascanQueue< std::string >*     m_pUpContentQueue;
    WaMetascanQueue< WaJson* >*         m_pProgressQueue;
    WaMetascanQueue< WaJson* >*         m_pResultQueue;
    
    WaWorker*                           m_pWorker[WAMETASCAN_TOTAL];
    WaMetascanCache*                    m_pCache;
};

#endif
