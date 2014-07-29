//
//  wa_process_lister.cpp
//  ProcessMonitorCMD
//
//  Created by NTViet-MAC on 7/24/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#include "wa_process_lister.h"
#include <pthread.h>
#include <sys/sysctl.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <libproc.h>
#include <string>

WaProcessLister::WaProcessLister()
: m_cfTimer( NULL )
, m_cfRunner( NULL )
{
    std::string key = "f409330ead0c59cec0a07675a5f39fe8";
    this->m_pScanManager = new WaMetascanManager( key );
}

WaProcessLister::~WaProcessLister()
{
    if ( this->m_cfTimer != NULL )
    {
        CFRelease( this->m_cfTimer );
    }
    
    if ( this->m_pScanManager != nullptr )
    {
        delete this->m_pScanManager;
        this->m_pScanManager = nullptr;
    }
    
    this->_clearCache();
}

void WaProcessLister::start( TSignal* p_signal )
{
    this->m_pSignal = p_signal;
    
    pthread_attr_t t_thread_att;
    pthread_attr_init( &t_thread_att );
    pthread_attr_setdetachstate( &t_thread_att, PTHREAD_CREATE_DETACHED );
    pthread_t thread_id;
    
    if ( pthread_create( &( thread_id ), &t_thread_att, &( WaProcessLister::_startWorking ), ( void* )this ) == 0 )
    {
        pthread_attr_destroy( &t_thread_att );
    }
}

void WaProcessLister::stop()
{
    CFRunLoopStop( this->m_cfRunner );
    CFRunLoopRemoveTimer( this->m_cfRunner, this->m_cfTimer, kCFRunLoopCommonModes );
}

void* WaProcessLister::_startWorking( void *p_arg )
{
    WaProcessLister* p_list = ( WaProcessLister* )p_arg;
    
    p_list->_setup();
    
    printf( "End lister\n" );
    
    return NULL;
}

void WaProcessLister::_setup()
{
    CFRunLoopTimerContext context = { 0, ( void* )this, NULL, NULL, NULL };
    this->m_cfTimer = CFRunLoopTimerCreate( kCFAllocatorDefault, 0.2, 10, 0, 0, &( WaProcessLister::_eventFired ), &context );
    //this->m_cfTimer = CFRunLoopTimerCreate( kCFAllocatorDefault, 0.2, 0, 0, 0, &( WaProcessLister::_eventFired ), &context );
    this->m_cfRunner = CFRunLoopGetCurrent();
    CFRunLoopAddTimer( this->m_cfRunner, this->m_cfTimer, kCFRunLoopCommonModes );
    
    // CALL CFRunLoopRun to start the run loop, this function is hold until CFRunLoopStop is called; otherwise, this function returns immediately and thread will stop
    CFRunLoopRun();
}

void WaProcessLister::_eventFired( CFRunLoopTimerRef timer, void *info )
{
    WaProcessLister* p_lister = ( WaProcessLister* )info;
    wa_process_list temp_list;
    
    //printf( "\nEvent fired: %d ------------------------------------ \n", p_lister->counter );
    
    kinfo_proc *        p_process_list = nullptr;
    int                 name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
    size_t              length = 0;
    
    if ( sysctl( name, 4, NULL, &length, NULL, 0) != 0 )
        return ;
    
    p_process_list = (kinfo_proc *)malloc( length );
    if ( sysctl( name, 4, p_process_list, &length, NULL, 0) == 0 )
    {
        char process_path_buff[PROC_PIDPATHINFO_MAXSIZE];
        size_t i_count = ( size_t )( length / sizeof( kinfo_proc ) );
        
        for ( size_t i = 0 ; i < i_count ; i++ )
        {
            struct extern_proc process_info = p_process_list[i].kp_proc;
            
            std::string process_name( process_info.p_comm );
            
            std::string process_path;
            if ( proc_pidpath( ( int )process_info.p_pid, process_path_buff, PROC_PIDPATHINFO_MAXSIZE ) > 0 )
                process_path = process_path_buff;
            
            if ( process_path.empty() )
                continue;
            
            auto found = p_lister->m_cache.find( process_info.p_pid );
            TScanDetail* p_detail;
            
            if ( found != p_lister->m_cache.end() )
            {
                found->second->should_clear = false;
                p_detail = found->second->p_detail;
            }
            else
            {
                // new process
                p_detail = p_lister->m_pScanManager->scan( process_path );
                // update local cache
                TCacheEntry* p_entry = new TCacheEntry( p_detail );
                p_lister->m_cache.insert( std::make_pair( process_info.p_pid, p_entry ) );
            }
            TProcessInfo process = { process_info.p_pid, process_name, process_path, p_detail };
            temp_list.push_back( process );
        }
    }
    free( p_process_list );
    
    p_lister->_refreshCache();
    
    // better if we just copy, need critical section here
    p_lister->m_pSignal->processList = temp_list;
    p_lister->m_pSignal->signal();
    
    printf( "Event is fired\n" );
}

void WaProcessLister::_refreshCache()
{
    vector< wa_process_cache::iterator > need_clear;
    for ( auto iter = this->m_cache.begin() ; iter != this->m_cache.end() ; iter++ )
    {
        if ( iter->second->should_clear == true )
            need_clear.push_back( iter );
        else
            iter->second->should_clear = true;
    }
    
    for ( int i = 0 ; i < ( int )need_clear.size() ; i++ )
    {
        this->m_cache.erase( need_clear[i] );
    }
}

void WaProcessLister::_clearCache()
{
    for ( auto iter = this->m_cache.begin() ; iter != this->m_cache.end() ; iter ++ )
    {
        if ( iter->second != nullptr )
            delete iter->second;
    }
    this->m_cache.clear();
}