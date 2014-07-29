//
//  wa_worker.cpp
//  TryLoader
//
//  Created by NTViet-MAC on 7/23/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#include "wa_worker.h"

#include "wa_utils_json_factory.h"
#include "wa_utils_string.h"


WaWorker::WaWorker()
: m_cfSource( NULL )
, m_pWorker( NULL )
, m_nWorker( 0 )
{
    
}

WaWorker::~WaWorker()
{
    if ( this->m_cfSource != NULL )
    {
        CFRelease( this->m_cfSource );
    }
    
    delete[] this->m_pWorker;
}




void WaWorker::stop()
{
    CFRunLoopStop( this->m_cfRunner );
    CFRunLoopRemoveSource( this->m_cfRunner, this->m_cfSource, kCFRunLoopCommonModes );
    // When RunLoop Object is stopped, the control is back to next line in the routine of the thread
}

void WaWorker::signal( void* p_data )
{
    this->_processSignal( p_data );
    
    CFRunLoopSourceSignal( this->m_cfSource );
    CFRunLoopWakeUp( this->m_cfRunner );
}

void WaWorker::_init( CFRunLoopSourceContext* context )
{
    context->schedule = &( WaWorker::_schedule );
    this->m_cfSource = CFRunLoopSourceCreate( kCFAllocatorDefault, 0, context );
    CFRunLoopRef temp = CFRunLoopGetCurrent();
    CFRunLoopAddSource( temp, this->m_cfSource, kCFRunLoopCommonModes);
    
    CFRunLoopRun();
}

void WaWorker::_schedule( void *info, CFRunLoopRef cf_runner, CFStringRef cf_mode )
{
    WaWorker* p_ower = ( WaWorker* )info;
    p_ower->m_cfRunner = cf_runner;
}

void WaWorker::setWorker(WaWorker** p_worker, int n_worker)
{
    this->m_nWorker = n_worker;
    this->m_pWorker = new WaWorker* [this->m_nWorker];
    for ( int i = 0 ; i < n_worker ; i++ )
    {
        this->m_pWorker[i] = p_worker[i];
    }
}

WaJson* WaWorker::_toJson( std::string& s_data )
{
    WaJsonFactory factory;
    WaJson* p_json = new WaJson();
    factory.create( WaStringUtils::_T( ( char* )s_data.c_str() ), *p_json );
    
    return p_json;
}