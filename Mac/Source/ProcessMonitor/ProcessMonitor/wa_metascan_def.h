//
//  wa_metascan_def.h
//  ProcessMonitor
//
//  Created by NTViet-MAC on 7/26/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef ProcessMonitor_wa_metascan_def_h
#define ProcessMonitor_wa_metascan_def_h

#include "wa_utils_json.h"

struct TScanDetail
{
    std::string scan_status;
    int         n_engine;
    WaJson*     p_result;   // just reference, never delete, will receive from metascan manager
    
    TScanDetail()
    {
        scan_status = "scanning";
        n_engine = 0;
        p_result = nullptr;
    }
};

#endif
