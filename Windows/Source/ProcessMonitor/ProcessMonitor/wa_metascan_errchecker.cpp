//
//  wa_metascan_errchecker.cpp
//  TryLoader
//
//  Created by NTViet-MAC on 7/24/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#include "wa_metascan_errchecker.h"
#include "wa_utils_string.h"


void WaMetascanErrChecker::pushInternalErr( WaWorker* p_des_worker, std::wstring err_msg )
{
    WaJson* p_err_json = new WaJson( WAAPI_JSON_OBJECT );
    p_err_json->put( L"err", err_msg.c_str() );
    p_err_json->put( L"type", L"local" );
    p_des_worker->signal( ( void* )p_err_json );
}

void WaMetascanErrChecker::pushServerErr( WaWorker* p_des_worker, std::wstring& content )
{
    std::wstring err_msg;
    size_t i_start = content.find( L"<title>" );
    if ( i_start != std::string::npos )
    {
        i_start += 7;
        size_t i_end = content.find( L"</title>", i_start );
        err_msg = content.substr( i_start, i_end - i_start );
    }
    else
    {
        err_msg = content;
    }
    
    WaJson* p_err_json = new WaJson( WAAPI_JSON_OBJECT );
    p_err_json->put( L"err", err_msg.c_str() );
    p_err_json->put( L"type", L"server" );
    p_des_worker->signal( ( void* )p_err_json );
}

void WaMetascanErrChecker::pushServerErr( WaWorker* p_des_worker, WaJson* p_err )
{
    wa_wstring s_msg;
    if ( WAAPI_FAILED( p_err->get( L"err", s_msg ) ) )
    {
        p_err->clear();
        p_err->put( L"err", L"unknown error from server" );
    }
    p_err->put( L"type", L"server" );
    p_des_worker->signal( ( void* )p_err );
}