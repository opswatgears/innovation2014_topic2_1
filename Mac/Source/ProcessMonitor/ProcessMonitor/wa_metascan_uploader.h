//
//  wa_metascan_uploader.h
//  TryLoader
//
//  Created by NTViet-MAC on 7/17/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef TryLoader_wa_metascan_uploader_h
#define TryLoader_wa_metascan_uploader_h

#include "wa_metascan_talker.h"

#define WAMETASCAN_UPLOADER_CHECKER     0
#define WAMETASCAN_UPLOADER_REPORTER    1

class WaMetascanUploader: public WaMetascanTalker
{
public:
    
    WaMetascanUploader( std::string& key, WaMetascanQueue< std::string >* p_file_queue );
    virtual ~WaMetascanUploader();
    
    virtual void start();
    
private:

    static void*     _startWorking( void* p_arg );
    void             _setup();
    bool             _upload( std::string& path );
    bool             _readContent( std::string& file_path, std::string& content );
    virtual void     _processResponse( std::string& s_data );
    virtual void     _processSignal( void* p_data );
    CFHTTPMessageRef _prepareFileUploaded( std::string& url, std::string& path );
    static void      _perform( void *info );
    
private:
    
    WaMetascanQueue< std::string >* m_pFileQueue;
};

#endif
