//
//  wa_metascan_hashloader.h
//  TryLoader
//
//  Created by NTViet-MAC on 7/23/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef TryLoader_wa_metascan_hashloader_h
#define TryLoader_wa_metascan_hashloader_h

#include "wa_metascan_talker.h"
#include "SHA1.h"
#include <map>

#define WAMETASCAN_HASHLOADER_UPLOADER     0
#define WAMETASCAN_HASHLOADER_CHECKER      1
#define WAMETASCAN_HASHLOADER_REPORTER     2

class WaMetascanHashloader: public WaMetascanTalker
{
public:
    
    WaMetascanHashloader( std::string& key, WaMetascanQueue< std::string >* p_file_queue );
    virtual ~WaMetascanHashloader();
    
    virtual void start();
    
private:
    
    static void*     _startWorking( void* p_arg );
    void             _setup();
    static void      _perform( void *info );
    bool             _upload( WaJson& hash_json );
    CFHTTPMessageRef _prepareData( std::string& url, WaJson& hash_json );
    virtual void     _processResponse( std::string& s_data );
    virtual void     _processSignal( void* p_data );
    bool             _getHash( std::string& file_path, std::string& hash_code );
    
private:
    
    WaMetascanQueue< std::string >* m_pFileQueue;    
    CSHA1                           m_hasher;
    map< std::string, std::string > m_cache;    // hash code <-> process path
};

#endif
