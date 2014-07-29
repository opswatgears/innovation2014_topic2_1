//
//  wa_metascan_uploader.cpp
//  TryLoader
//
//  Created by NTViet-MAC on 7/17/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#include "wa_metascan_uploader.h"
#include "wa_utils_json_factory.h"
#include "wa_utils_string.h"
#include "wa_api_error_codes.h"
#include "wa_metascan_errchecker.h"
#include <libgen.h>

WaMetascanUploader::WaMetascanUploader( std::string& key, WaMetascanQueue< std::string >* p_file_queue )
: WaMetascanTalker( key )
, m_pFileQueue( p_file_queue )
{
    
}

WaMetascanUploader::~WaMetascanUploader()
{

}

void WaMetascanUploader::start()
{
    pthread_attr_t t_thread_att;
    pthread_attr_init( &t_thread_att );
    pthread_attr_setdetachstate( &t_thread_att, PTHREAD_CREATE_DETACHED );
    pthread_t thread_id;
    
    if ( pthread_create( &( thread_id ), &t_thread_att, &( WaMetascanUploader::_startWorking ), ( void* )this ) == 0 )
    {
        pthread_attr_destroy( &t_thread_att );
    }
}

void* WaMetascanUploader::_startWorking( void* p_arg )
{
    WaMetascanUploader* p_worker = ( WaMetascanUploader* )p_arg;
    
    // After init, control belongs to CFRunLoop Object
    p_worker->_setup();
    
    // When stop is called, CFRunLoop Object will be stopped, the control is back to this function, do some deinitlization here
    printf( "End uploader\n" );
    
    return NULL;
}

void WaMetascanUploader::_setup()
{
    CFRunLoopSourceContext    context = {0, ( void* )this, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &( WaMetascanUploader::_perform ) };
    this->_init( &context );
}

void WaMetascanUploader::_perform( void *info )
{
    WaMetascanUploader* p_uploader = ( WaMetascanUploader* )info;
    while ( p_uploader->m_pFileQueue->size() > 0 )
    {
        std::string path = p_uploader->m_pFileQueue->get();
        if ( !path.empty() )
        {
            if ( p_uploader->_upload( path ) == false )
                WaMetascanErrChecker::pushInternalErr( p_uploader->m_pWorker[WAMETASCAN_UPLOADER_REPORTER], "can't upload file" );
        }
        else
        {
            WaMetascanErrChecker::pushInternalErr( p_uploader->m_pWorker[WAMETASCAN_UPLOADER_REPORTER], "empty file path" );
        }
    }
}

bool WaMetascanUploader::_upload( std::string& path )
{
    if ( this->m_cfKey == NULL )
        return false;
    
    std::string s_url = "https://scan.metascan-online.com/v2/file";
    CFHTTPMessageRef cf_message = NULL;
    
    if ( ( cf_message = this->_prepareFileUploaded( s_url, path ) ) != NULL )
    {
        this->_launchMessage( cf_message, this );
        return true;
    }
    
    return false;
}

CFHTTPMessageRef WaMetascanUploader::_prepareFileUploaded( std::string& url, std::string& path )
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
        
        CFStringRef cf_filename = CFStringCreateWithCString( NULL, path.c_str() , CFStringGetSystemEncoding() );
        if ( cf_filename == NULL )
            break;
        
        CFHTTPMessageSetHeaderFieldValue( cf_message, CFSTR( "filename" ), cf_filename );
        CFRelease( cf_filename );
        
        CFHTTPMessageSetHeaderFieldValue( cf_message, CFSTR( "apikey" ), this->m_cfKey );
        
        std::string s_content;
        if ( this->_readContent( path, s_content ) == false )
        {
            //printf( "Error in reading file\n" );
            break;
        }

        CFDataRef cf_body = CFDataCreate( kCFAllocatorDefault, ( const UInt8* )s_content.c_str(), ( CFIndex )s_content.size() );
        
        // temporal disable here
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

bool WaMetascanUploader::_readContent( std::string& file_path, std::string& content )
{
    FILE* fp = fopen( file_path.c_str(), "rb" );
    if ( fp == NULL )
        return false;
    
    char temp[1024];
    while ( !feof( fp ) )
    {
        size_t n_read = fread( temp, sizeof( char ), 1024, fp );
        content.append( temp, n_read );
    }
    
    fclose( fp );
    
    return true;
}

void WaMetascanUploader::_processResponse( std::string& s_data )
{
    WaJson* p_json = this->_toJson( s_data );
    
    if ( p_json->isType( WAAPI_JSON_NULL ) )
    {
        delete p_json;
        WaMetascanErrChecker::pushServerErr( this->m_pWorker[WAMETASCAN_UPLOADER_REPORTER], s_data );
    }
    else
    {
        //printf( "Load successfully:\n%s\n\n", WaStringUtils::wstrToStr( p_json->toPrettyString() ).c_str() );
        
        this->m_pWorker[WAMETASCAN_UPLOADER_CHECKER]->signal( ( void* )p_json );
    }
}

void WaMetascanUploader::_processSignal( void *p_data )
{
    std::string* p_str = ( std::string* )p_data;
    this->m_pFileQueue->put( *p_str );
}