//
//  wa_metascan_cache.cpp
//  ProcessMonitor
//
//  Created by NTViet-MAC on 7/26/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#include "wa_metascan_cache.h"
#include "wa_utils_string.h"

WaMetascanCache::WaMetascanCache()
{
    pthread_mutex_init( &( this->m_tMutex ), NULL );
}

WaMetascanCache::~WaMetascanCache()
{
    pthread_mutex_lock( &( this->m_tMutex ) );
    for ( auto iter = this->m_cachMap.begin() ; iter != this->m_cachMap.end() ; iter++ )
    {
        TScanDetail* p_detail = iter->second;
        if ( p_detail != nullptr )
        {
            delete p_detail->p_result;
            delete p_detail;
        }
    }
    pthread_mutex_unlock( &( this->m_tMutex ) );
    pthread_mutex_destroy( &( this->m_tMutex ) );
}

TScanDetail* WaMetascanCache::get( std::string &process_path, bool& do_scan )
{
    TScanDetail* p_detail = nullptr;
    
    pthread_mutex_lock( &( this->m_tMutex ) );
    auto found = this->m_cachMap.find( process_path );
    if ( found != this->m_cachMap.end() )
    {
        p_detail = found->second;
        do_scan = false;
    }
    else
    {
        p_detail = new TScanDetail;
        this->m_cachMap.insert( std::make_pair( process_path, p_detail ) );
        do_scan = true;
    }
    pthread_mutex_unlock( &( this->m_tMutex ) );
    
    return p_detail;
}

void WaMetascanCache::insert(std::string &process_path, WaJson *detail_json )
{
    pthread_mutex_lock( &( this->m_tMutex ) );
    auto found = this->m_cachMap.find( process_path );
    if ( found != this->m_cachMap.end() )
    {
        WaJson result_json;
        detail_json->get( L"scan_results", result_json );
        
        wa_wstring s_result;
        result_json.get( L"scan_all_result_a", s_result );
        
        found->second->scan_status = WaStringUtils::wstrToStr( s_result );
        result_json.get( L"total_avs", found->second->n_engine );
        
        found->second->p_result = detail_json;
    }
    pthread_mutex_unlock( &( this->m_tMutex ) );
}