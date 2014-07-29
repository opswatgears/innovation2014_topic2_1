//
//  wa_metascan_checker.cpp
//  TryLoader
//
//  Created by NTViet-MAC on 7/18/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#include "wa_metascan_checker.h"
#include "wa_api_error_codes.h"
#include "wa_metascan_errchecker.h"
#include "wa_utils_string.h"

WaMetascanChecker::WaMetascanChecker( std::string& key, WaMetascanQueue< WaJson* >* p_wait_queue )
: WaMetascanTalker( key )
, m_pWaitQueue( p_wait_queue )
{
    
}

WaMetascanChecker::~WaMetascanChecker()
{
    
}

void WaMetascanChecker::start()
{
    pthread_attr_t t_thread_att;
    pthread_attr_init( &t_thread_att );
    pthread_attr_setdetachstate( &t_thread_att, PTHREAD_CREATE_DETACHED );
    pthread_t thread_id;
    
    if ( pthread_create( &( thread_id ), &t_thread_att, &( WaMetascanChecker::_startWorking ), ( void* )this ) == 0 )
    {
        pthread_attr_destroy( &t_thread_att );
    }
}

void* WaMetascanChecker::_startWorking( void* p_arg )
{
    WaMetascanChecker* p_worker = ( WaMetascanChecker* )p_arg;
    
    // After init, control belongs to CFRunLoop Object
    p_worker->_setup();
    
    printf( "End checker\n" );
    
    // When stop is called, CFRunLoop Object will be stopped, the control is back to this function, do some deinitlization here
    return NULL;
}

void WaMetascanChecker::_setup()
{
    CFRunLoopSourceContext    context = {0, ( void* )this, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &( WaMetascanChecker::_perform ) };
    this->_init( &context );
}

void WaMetascanChecker::_perform( void *info )
{
    WaMetascanChecker* p_checker = ( WaMetascanChecker* )info;
    
    while ( p_checker->m_pWaitQueue->size() > 0 )
    {
        WaJson* p_json = p_checker->m_pWaitQueue->get();
        
        //printf( " - checker checks status\n" );
        
        wa_wstring data_id;
        p_json->get( L"data_id", data_id );
        
        wa_wstring rest_ip;
        p_json->get( L"rest_ip", rest_ip );
        
        delete p_json;
        
        if ( p_checker->_checkScanStatus( data_id, rest_ip ) == true )
            p_checker->m_cache.insert( std::make_pair( data_id, rest_ip ) );
    }
}

bool WaMetascanChecker::_checkScanStatus( wa_wstring& data_id, wa_wstring& rest_ip )
{
    std::string url = "https://" + WaStringUtils::wstrToStr( rest_ip ) + "/file/" + WaStringUtils::wstrToStr( data_id );
    CFHTTPMessageRef cf_message = NULL;
    
    if ( ( cf_message = this->_prepareQueryStatus( url ) ) != NULL )
    {
        //printf( " - checker: ready launch message\n" );
        this->_launchMessage( cf_message, this );
        //printf( " - checker: launches message successfully\n" );
        
        return true;
    }
    
    WaMetascanErrChecker::pushInternalErr( this->m_pWorker[WAMETASCAN_CHECKER_REPORTER], "can't not send data" );
    //printf( " - checker: error in launching message\n" );
    return false;
}

void WaMetascanChecker::_processResponse( std::string& s_data )
{
    WaJson* p_json = this->_toJson( s_data );
    if ( p_json->isType( WAAPI_JSON_NULL ) )
    {
        delete p_json;
        WaMetascanErrChecker::pushServerErr( this->m_pWorker[WAMETASCAN_CHECKER_REPORTER], s_data );
    }
    else
    {
        WaJson result_json;
        int percentage;
        
        if ( ( WAAPI_SUCCESS( p_json->get( L"scan_results", result_json ) )  &&
              ( WAAPI_SUCCESS( result_json.get( L"progress_percentage", percentage ) ) ) ) )
        {
            if ( percentage < 100 )
            {                
                printf( " - Progress: %d\n", percentage );
                
                wa_wstring data_id;
                
                if ( WAAPI_SUCCESS( p_json->get( L"data_id", data_id ) ) )
                {
                    auto found = this->m_cache.find( data_id );
                    if ( found != this->m_cache.end() )
                    {
                        this->_checkScanStatus( data_id, found->second );
                    }
                }
                delete p_json;
            }
            else
            {
                wa_wstring data_id;
                p_json->get( L"data_id", data_id );
                this->m_cache.erase( data_id );
                
                this->m_pWorker[WAMETASCAN_CHECKER_REPORTER]->signal( ( void* )p_json );
            }
        }
        else
        {
            WaMetascanErrChecker::pushInternalErr( this->m_pWorker[WAMETASCAN_CHECKER_REPORTER], "can't get value from json" );
        }
    }
}

void WaMetascanChecker::_processSignal( void *p_data )
{
    WaJson* p_json_data = ( WaJson* )p_data;
    this->m_pWaitQueue->put( p_json_data );
}

CFHTTPMessageRef WaMetascanChecker::_prepareQueryStatus( std::string& url )
{
    CFHTTPMessageRef cf_message = NULL;
    bool b_failed = true;
    
    do
    {
        CFURLRef cf_url = this->_genURL( url );
        if ( cf_url == NULL )
            break;
        
        cf_message = CFHTTPMessageCreateRequest( kCFAllocatorDefault, CFSTR( "GET" ), cf_url, kCFHTTPVersion1_1 );
        if ( cf_message == NULL )
            break;
        
        CFHTTPMessageSetHeaderFieldValue( cf_message, CFSTR( "apikey" ), this->m_cfKey );
        
        b_failed = false;
        
    } while ( false );
    
    if ( ( b_failed == true ) && ( cf_message != NULL ) )
    {
        CFRelease( cf_message );
        cf_message = NULL;
    }
    return  cf_message;
}
