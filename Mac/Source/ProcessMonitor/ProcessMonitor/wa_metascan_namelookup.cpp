//
//  wa_metascan_namelookup.cpp
//  ProcessMonitor
//
//  Created by NTViet-MAC on 7/26/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#include "wa_metascan_namelookup.h"

std::map< std::string, std::string> WaMetascanNameLookup::m_pathDataMap;
pthread_mutex_t WaMetascanNameLookup::m_tMutex;

void WaMetascanNameLookup::initialize()
{
    pthread_mutex_init( &( WaMetascanNameLookup::m_tMutex ), NULL );
}

void WaMetascanNameLookup::destroy()
{
    pthread_mutex_destroy( &( WaMetascanNameLookup::m_tMutex ) );
}

std::string  WaMetascanNameLookup::lookup( std::string& data_id )
{
    std::string s_path("");
    
    pthread_mutex_lock( &( WaMetascanNameLookup::m_tMutex ) );
    auto found = WaMetascanNameLookup::m_pathDataMap.find( data_id );
    if ( found != WaMetascanNameLookup::m_pathDataMap.end() )
    {
        s_path = found->second;
        WaMetascanNameLookup::m_pathDataMap.erase( found );
    }    
    pthread_mutex_unlock( &( WaMetascanNameLookup::m_tMutex ) );
    
    return s_path;
}

void WaMetascanNameLookup::insert( std::string& path, std::string& data_id )
{
    pthread_mutex_lock( &( WaMetascanNameLookup::m_tMutex ) );
    WaMetascanNameLookup::m_pathDataMap.insert( std::make_pair( data_id, path ) );
    pthread_mutex_unlock( &( WaMetascanNameLookup::m_tMutex ) );
}

