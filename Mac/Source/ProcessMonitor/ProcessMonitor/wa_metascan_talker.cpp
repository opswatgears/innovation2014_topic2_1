//
//  wa_metascan_talker.cpp
//  TryLoader
//
//  Created by NTViet-MAC on 7/18/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#include "wa_metascan_talker.h"
#include "wa_utils_json_factory.h"
#include "wa_utils_string.h"

WaMetascanTalker::WaMetascanTalker( std::string& key )
: WaWorker()
{        
    this->m_cfKey = CFStringCreateWithCString( NULL, key.c_str(), CFStringGetSystemEncoding() );
}

WaMetascanTalker::~WaMetascanTalker()
{
    if ( this->m_cfKey != NULL )
        CFRelease( this->m_cfKey );
}


bool WaMetascanTalker::_launchMessage( CFHTTPMessageRef cf_message, WaMetascanTalker* p_object )
{
    CFReadStreamRef cf_reader = NULL;
    bool            b_failed = true;
    
    do
    {
        // Create the stream for the request.
        CFReadStreamRef cf_reader = CFReadStreamCreateForHTTPRequest( kCFAllocatorDefault, cf_message );
        if ( cf_reader == NULL )
            break;
        
        if ( CFReadStreamSetProperty( cf_reader, kCFStreamPropertyHTTPShouldAutoredirect, kCFBooleanTrue) == false )
            break;
        
        // Set the client notifier
        CFStreamClientContext       ctxt = { 0, ( void* )p_object, NULL, NULL, NULL };
        if ( CFReadStreamSetClient( cf_reader,
                                   kCFStreamEventOpenCompleted |
                                   kCFStreamEventHasBytesAvailable |
                                   kCFStreamEventEndEncountered |
                                   kCFStreamEventErrorOccurred, WaMetascanTalker::_recieveData, &ctxt ) == false )
            break;
        
        // Schedule the stream
        CFReadStreamScheduleWithRunLoop( cf_reader, CFRunLoopGetCurrent(), kCFRunLoopCommonModes );
        
        // Start the HTTP connection
        if ( CFReadStreamOpen( cf_reader ) == false )
            break;
        
        b_failed = false;
        
    } while ( false );
    
    CFRelease( cf_message );
    
    if ( b_failed == true )
    {
        this->_closeStream( cf_reader );
        return false;
    }
    
    return true;
}

void WaMetascanTalker::_closeStream( CFReadStreamRef cf_reader )
{
    if ( cf_reader != NULL )
    {
        CFReadStreamSetClient( cf_reader, kCFStreamEventNone, NULL, NULL );
        CFReadStreamUnscheduleFromRunLoop( cf_reader, CFRunLoopGetCurrent(), kCFRunLoopCommonModes );
        CFReadStreamClose( cf_reader );
        CFRelease( cf_reader );
    }
}

void WaMetascanTalker::_recieveData( CFReadStreamRef stream, CFStreamEventType type, void *info )
{
    WaMetascanTalker* p_ower = ( WaMetascanTalker* )info;
    
    switch ( type )
    {
        case kCFStreamEventHasBytesAvailable:
        {
            UInt8*      p_buffer = new UInt8[1024];
            CFIndex     bytesRead;
            std::string s_data;

            do
            {
                bytesRead = CFReadStreamRead( stream, p_buffer, 1024 );
                if ( bytesRead > 0 )
                    s_data.append( ( char* )p_buffer, bytesRead );
                
            } while ( bytesRead > 0 );
                        
            delete[] p_buffer;
            p_ower->_processResponse( s_data );
            
            break;
        }
        case kCFStreamEventEndEncountered:
        {           
            p_ower->_closeStream( stream );
            
            break;
        }
        case kCFStreamEventErrorOccurred:
        {
            CFErrorRef cf_error = CFReadStreamCopyError( stream );
            printf( "Error on recieving data\n" );
            CFRelease( cf_error );
            p_ower->_closeStream( stream );
            
            break;
        }
        default:
            break;
    }
}

CFURLRef WaMetascanTalker::_genURL( std::string& url )
{
    CFURLRef cf_url = NULL;
    do
    {
        CFStringRef cf_raw_str = CFStringCreateWithCString( NULL, url.c_str(), CFStringGetSystemEncoding() );
        if ( cf_raw_str == NULL )
            break;
        
        CFStringRef cf_normalized_str  = CFURLCreateStringByReplacingPercentEscapes( NULL, cf_raw_str, CFSTR("") );
        CFRelease( cf_raw_str );
        if ( cf_normalized_str == NULL )
            break;
        
        CFStringRef cf_escaped_str = CFURLCreateStringByAddingPercentEscapes( NULL, cf_normalized_str, NULL, NULL, kCFStringEncodingUTF8 );
        CFRelease( cf_normalized_str );
        
        if ( cf_escaped_str == NULL )
            break;
        
        cf_url = CFURLCreateWithString( kCFAllocatorDefault, cf_escaped_str, NULL );
        CFRelease( cf_escaped_str );
        
    } while ( false );
    
    return cf_url;
}
