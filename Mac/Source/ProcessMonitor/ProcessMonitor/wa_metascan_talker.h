//
//  wa_metascan_talker.h
//  TryLoader
//
//  Created by NTViet-MAC on 7/18/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef TryLoader_wa_metascan_talker_h
#define TryLoader_wa_metascan_talker_h

#include <CFNetwork/CFNetwork.h>
#include "wa_worker.h"
#include "wa_metascan_queue.h"

class WaMetascanTalker: public WaWorker
{
public:
    
    WaMetascanTalker( std::string& key );
    virtual ~WaMetascanTalker();
    
protected:

    void            _closeStream( CFReadStreamRef cf_reader );
    bool            _launchMessage( CFHTTPMessageRef cf_message, WaMetascanTalker* p_object );
    virtual void    _processResponse( std::string& data ) {};
    static void     _recieveData( CFReadStreamRef stream, CFStreamEventType type, void *info );
    CFURLRef        _genURL( std::string& url );
    
protected:
    
    CFStringRef             m_cfKey;
};

#endif
