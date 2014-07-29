//
//  wa_metascan_reporter.cpp
//  TryLoader
//
//  Created by NTViet-MAC on 7/19/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#include "wa_metascan_reporter.h"
#include "wa_utils_string.h"
#include "wa_metascan_namelookup.h"

WaMetascanReporter::WaMetascanReporter( WaMetascanQueue< WaJson* >* p_result_queue, WaMetascanCache* p_cache )
: WaWorker()
, m_pResultQueue( p_result_queue )
, m_pCache( p_cache )
{
    
}

WaMetascanReporter::~WaMetascanReporter()
{
    
}

void WaMetascanReporter::start()
{
    pthread_attr_t t_thread_att;
    pthread_attr_init( &t_thread_att );
    pthread_attr_setdetachstate( &t_thread_att, PTHREAD_CREATE_DETACHED );
    pthread_t thread_id;
    
    if ( pthread_create( &( thread_id ), &t_thread_att, &( WaMetascanReporter::_startWorking ), ( void* )this ) == 0 )
    {
        pthread_attr_destroy( &t_thread_att );
    }
}

void* WaMetascanReporter::_startWorking( void* p_arg )
{
    WaMetascanReporter* p_worker = ( WaMetascanReporter* )p_arg;
    
    // After init, control belongs to CFRunLoop Object
    p_worker->_setup();
    
    printf( "End reporter\n" );
    
    // When stop is called, CFRunLoop Object will be stopped, the control is back to this function, do some deinitlization here
    return NULL;
}

void WaMetascanReporter::_setup()
{
    CFRunLoopSourceContext    context = {0, ( void* )this, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &( WaMetascanReporter::_perform ) };
    this->_init( &context );
}

void WaMetascanReporter::_perform( void *info )
{
    WaMetascanReporter* p_reporter = ( WaMetascanReporter* )info;
    
    while ( p_reporter->m_pResultQueue->size() > 0 )
    {
        WaJson* p_json = p_reporter->m_pResultQueue->get();
        p_reporter->_report( p_json );       
    }    
}

void WaMetascanReporter::_report( WaJson* p_json )
{
    wa_wstring s_msg;
    if ( WAAPI_SUCCESS( p_json->get( L"err", s_msg ) ) )
    {
        printf( "Error: %s\n", WaStringUtils::wstrToStr( s_msg ).c_str() );
    }
    else
    {
        wa_wstring data_id;
        p_json->get( L"data_id", data_id );               
        
        std::string id = WaStringUtils::wstrToStr( data_id );
        std::string s_path = WaMetascanNameLookup::lookup( id );
        
        if ( s_path.empty() )
        {
            WaJson info_json;
            p_json->get( L"file_info", info_json );
            wa_wstring display_name;
            info_json.get( L"display_name", display_name );
            s_path = WaStringUtils::wstrToStr( display_name );
        }
        
        this->m_pCache->insert( s_path, p_json );
    }
}

void WaMetascanReporter::_processSignal( void *p_data )
{
    WaJson* p_json_data = ( WaJson* )p_data;
    this->m_pResultQueue->put( p_json_data );
}