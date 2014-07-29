//
//  wa_process_lister.h
//  ProcessMonitorCMD
//
//  Created by NTViet-MAC on 7/24/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef ProcessMonitorCMD_wa_process_lister_h
#define ProcessMonitorCMD_wa_process_lister_h

#include <CoreFoundation/CoreFoundation.h>
#include "wa_process_def.h"
#include "wa_metascan_manager.h"

class WaProcessLister
{
public:
    
    WaProcessLister();
    ~WaProcessLister();
    
    void start( TSignal* p_signal );
    void stop();
    
private:
    
    static void*    _startWorking( void* p_arg );
    void            _setup();
    static void     _eventFired( CFRunLoopTimerRef timer, void *info );
    void            _refreshCache();
    void            _clearCache();
    
private:
    
    CFRunLoopRef        m_cfRunner;
    CFRunLoopTimerRef   m_cfTimer;
    wa_process_cache    m_cache;
    
    WaMetascanManager*  m_pScanManager;
    TSignal*            m_pSignal;
   
};

#endif
