//
//  wa_metascan_manager.cpp
//  TryLoader
//
//  Created by NTViet-MAC on 7/18/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#include "wa_metascan_manager.h"
#include "wa_metascan_hashloader.h"
#include "wa_metascan_uploader.h"
#include "wa_metascan_checker.h"
#include "wa_metascan_reporter.h"
#include "wa_metascan_namelookup.h"

WaMetascanManager::WaMetascanManager( std::string& key )
{
    WaMetascanNameLookup::initialize();
    
    this->m_pCache = new WaMetascanCache();
    this->m_pFileQueue = new WaMetascanQueue< std::string >();
    this->m_pUpContentQueue = new WaMetascanQueue< std::string >();
    this->m_pProgressQueue = new WaMetascanQueue< WaJson* >();
    this->m_pResultQueue = new WaMetascanQueue< WaJson* >();
    
    this->m_pWorker[WAMETASCAN_REPORTER] = new WaMetascanReporter( this->m_pResultQueue, this->m_pCache );
    this->m_pWorker[WAMETASCAN_CHECKER] = new WaMetascanChecker( key, this->m_pProgressQueue  );
    this->m_pWorker[WAMETASCAN_UPLOADER] = new WaMetascanUploader( key, this->m_pUpContentQueue );
    this->m_pWorker[WAMETASCAN_HASHLOADER] = new WaMetascanHashloader( key, this->m_pFileQueue );
    
    
    this->m_pWorker[WAMETASCAN_CHECKER]->setWorker(  &( this->m_pWorker[WAMETASCAN_REPORTER] ),1 );
    
    WaWorker* p_temp1[] = { this->m_pWorker[WAMETASCAN_CHECKER], this->m_pWorker[WAMETASCAN_REPORTER] };
    this->m_pWorker[WAMETASCAN_UPLOADER]->setWorker( p_temp1, 2 );
    
    WaWorker* p_temp2[] = { this->m_pWorker[WAMETASCAN_UPLOADER], this->m_pWorker[WAMETASCAN_CHECKER], this->m_pWorker[WAMETASCAN_REPORTER] };
    this->m_pWorker[WAMETASCAN_HASHLOADER]->setWorker( p_temp2, 3 );
    
    this->m_pWorker[WAMETASCAN_REPORTER]->start();
    this->m_pWorker[WAMETASCAN_CHECKER]->start();
    this->m_pWorker[WAMETASCAN_UPLOADER]->start();
    this->m_pWorker[WAMETASCAN_HASHLOADER]->start();
    
    sleep( 3 );
}

WaMetascanManager::~WaMetascanManager()
{
    for ( int i = 0 ; i < 4 ; i++ )
    {
        if ( this->m_pWorker[i] != nullptr )
        {
            this->m_pWorker[i]->stop();
            usleep( 20 );
            
            delete this->m_pWorker[i];
            this->m_pWorker[i] = nullptr;
        }
    }
    
    delete this->m_pFileQueue;
    delete this->m_pUpContentQueue;
    delete this->m_pProgressQueue;
    delete this->m_pResultQueue;
    delete this->m_pCache;
    
    WaMetascanNameLookup::destroy();
}

TScanDetail* WaMetascanManager::scan( std::string& path )
{
    bool do_scan = false;
    TScanDetail* p_detail = this->m_pCache->get( path, do_scan );
    if ( do_scan )
    {
        this->m_pWorker[WAMETASCAN_HASHLOADER]->signal( ( void* )&path );
    }
    return p_detail;
}

void WaMetascanManager::stop()
{
    
}
