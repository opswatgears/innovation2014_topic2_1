//
//  wa_metascan_hashloader.cpp
//  TryLoader
//
//  Created by NTViet-MAC on 7/23/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#include "wa_metascan_hashloader.h"
#include "wa_utils_string.h"
#include "wa_metascan_errchecker.h"
#include "wa_metascan_namelookup.h"

WaMetascanHashloader::WaMetascanHashloader( std::string& key, WaMetascanQueue< std::string >* p_file_queue )
: WaMetascanTalker( key )
, m_pFileQueue( p_file_queue )
{
    
}

WaMetascanHashloader::~WaMetascanHashloader()
{
    
}

void WaMetascanHashloader::start()
{
    pthread_attr_t t_thread_att;
    pthread_attr_init( &t_thread_att );
    pthread_attr_setdetachstate( &t_thread_att, PTHREAD_CREATE_DETACHED );
    pthread_t thread_id;
    
    if ( pthread_create( &( thread_id ), &t_thread_att, &( WaMetascanHashloader::_startWorking ), ( void* )this ) == 0 )
    {
        pthread_attr_destroy( &t_thread_att );
    }
}

void* WaMetascanHashloader::_startWorking( void* p_arg )
{
    WaMetascanHashloader* p_worker = ( WaMetascanHashloader* )p_arg;
    
    // After init, control belongs to CFRunLoop Object
    p_worker->_setup();
    
    // When stop is called, CFRunLoop Object will be stopped, the control is back to this function, do some deinitlization here
    printf( "End hashloader\n" );
    
    return NULL;
}

void WaMetascanHashloader::_setup()
{
    CFRunLoopSourceContext    context = {0, ( void* )this, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &( WaMetascanHashloader::_perform ) };
    this->_init( &context );
}

void WaMetascanHashloader::_perform( void *info )
{
    WaMetascanHashloader* p_hashloader = ( WaMetascanHashloader* )info;
    while ( p_hashloader->m_pFileQueue->size() > 0 )
    {
        WaJson hash_array( WAAPI_JSON_ARRAY );
        
        int counter = 0;
        while ( p_hashloader->m_pFileQueue->size() > 0 && counter < 50 )
        {
            std::string path = p_hashloader->m_pFileQueue->get();
            std::string hash_code;
            if ( p_hashloader->_getHash( path, hash_code ) == false )
            {
                WaMetascanErrChecker::pushInternalErr( p_hashloader->m_pWorker[WAMETASCAN_HASHLOADER_REPORTER], "can't create hash code" );
            }
            else
            {
                hash_array.add( WaStringUtils::_T( ( char* )hash_code.c_str() ).c_str() );
                p_hashloader->m_cache.insert( std::make_pair( hash_code, path ) );
            }
            
            counter ++;
        }
        
        WaJson hash_object( WAAPI_JSON_OBJECT );
        wa_wstring s_key = L"hash";
        hash_object.put( s_key , hash_array );
        
        if ( p_hashloader->_upload( hash_object ) == false )
            WaMetascanErrChecker::pushInternalErr( p_hashloader->m_pWorker[WAMETASCAN_HASHLOADER_REPORTER], "can't send hash list" );
    }
}

bool WaMetascanHashloader::_getHash( std::string& file_path, std::string& hash_code )
{
    if ( this->m_hasher.HashFile( file_path.c_str() ) == false )
        return false;
    
    this->m_hasher.Final();
    this->m_hasher.ReportHashStl( hash_code, CSHA1::REPORT_HEX_SHORT );
    this->m_hasher.Reset();
    
    return true;
}

bool WaMetascanHashloader::_upload( WaJson& hash_json )
{
    if ( this->m_cfKey == NULL )
        return false;
    
    std::string s_url = "https://hashlookup.metascan-online.com/v2/hash";
    
    CFHTTPMessageRef cf_message = NULL;
    
    if ( ( cf_message = this->_prepareData( s_url, hash_json ) ) != NULL )
    {
        this->_launchMessage( cf_message, this );
        return true;
    }
    
    return false;
}

CFHTTPMessageRef WaMetascanHashloader::_prepareData( std::string& url, WaJson& hash_json )
{
    CFHTTPMessageRef cf_message = NULL;
    bool b_failed = true;
    
    do
    {
        CFURLRef cf_url = this->_genURL( url );
        if ( cf_url == NULL )
            break;
        
        cf_message = CFHTTPMessageCreateRequest( kCFAllocatorDefault, CFSTR( "POST" ), cf_url, kCFHTTPVersion1_1 );
        CFRelease( cf_url );
        
        if ( cf_message == NULL )
            break;
        
        CFHTTPMessageSetHeaderFieldValue( cf_message, CFSTR( "apikey" ), this->m_cfKey );
        
        std::string s_content = WaStringUtils::wstrToStr( hash_json.toString() );
        CFDataRef cf_body = CFDataCreate( kCFAllocatorDefault, ( const UInt8* )s_content.c_str(), ( CFIndex )s_content.size() );
        
        CFHTTPMessageSetBody( cf_message, cf_body );
        CFRelease( cf_body );
        
        b_failed = false;
        
    } while ( false );
    
    if ( ( b_failed == true ) && ( cf_message != NULL ) )
    {
        CFRelease( cf_message );
        cf_message = NULL;
    }
    return  cf_message;
}

void WaMetascanHashloader::_processResponse( std::string& s_data )
{
    WaJson* p_json = this->_toJson( s_data );

    if ( p_json->isType( WAAPI_JSON_NULL ) )
    {
        WaMetascanErrChecker::pushServerErr( this->m_pWorker[WAMETASCAN_HASHLOADER_REPORTER], s_data );
        return;
    }
    
    if ( !p_json->isType( WAAPI_JSON_ARRAY ) )
    {
        WaMetascanErrChecker::pushServerErr( this->m_pWorker[WAMETASCAN_HASHLOADER_REPORTER], p_json );
        return;
    }
    
    //printf( "Hash:\n%s\n", WaStringUtils::wstrToStr( p_json->toPrettyString() ).c_str() );
    
    for ( size_t i = 0 ; i < p_json->size() ; i++ )
    {
        WaJson entry_json = p_json->at( i );
        
        wa_wstring hash_code;
        entry_json.get( L"hash", hash_code );
        
        auto found = this->m_cache.find( WaStringUtils::wstrToStr( hash_code ) );
        wa_wstring data_id;
        if ( WAAPI_FAILED( entry_json.get( L"data_id", data_id ) ) )
        {
            this->m_pWorker[WAMETASCAN_HASHLOADER_UPLOADER]->signal( ( void* )&( found->second ));
        }
        else
        {
            std::string id = WaStringUtils::wstrToStr( data_id );
            WaMetascanNameLookup::insert( found->second, id );
            
            WaJson* p_detail_json = new WaJson( WAAPI_JSON_OBJECT );
            
            p_detail_json->put( L"rest_ip", L"scan.metascan-online.com/v2" );
            p_detail_json->put( L"data_id", data_id.c_str() );
            
            this->m_pWorker[WAMETASCAN_HASHLOADER_CHECKER]->signal( ( void* )p_detail_json );
        }
        this->m_cache.erase( found );
    }
    delete p_json;        
}

void WaMetascanHashloader::_processSignal( void *p_data )
{
    std::string* p_str_data = ( std::string* )p_data;
    this->m_pFileQueue->put( *p_str_data );
}