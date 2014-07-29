//
//  wa_worker.h
//  TryLoader
//
//  Created by NTViet-MAC on 7/23/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef TryLoader_wa_worker_h
#define TryLoader_wa_worker_h

#include <CoreFoundation/CoreFoundation.h>
#include <string>
#include "wa_utils_json.h"

class WaWorker
{
public:
    
    WaWorker();
    virtual ~WaWorker();
    
    virtual void start() = 0;
    virtual void stop();
    virtual void signal( void* p_data );
    virtual void setWorker( WaWorker** p_worker, int n_worker );
    
protected:
    
    void            _init( CFRunLoopSourceContext* context );
    static void     _schedule( void *info, CFRunLoopRef cf_runner, CFStringRef cf_mode );
    virtual void    _processSignal( void* p_data ) = 0;
    WaJson*         _toJson( std::string& s_data );
    CFURLRef        _genURL( std::string& url );
    
protected:

    CFRunLoopSourceRef      m_cfSource;
    CFRunLoopRef            m_cfRunner;
    WaWorker**              m_pWorker;
    int                     m_nWorker;
};

#endif
