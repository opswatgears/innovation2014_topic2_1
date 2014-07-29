//
//  wa_metascan_errchecker.h
//  TryLoader
//
//  Created by NTViet-MAC on 7/24/14.
//  Copyright (c) 2014 VietNguyen. All rights reserved.
//

#ifndef TryLoader_wa_metascan_errchecker_h
#define TryLoader_wa_metascan_errchecker_h

#include <string>
#include "wa_utils_json.h"
#include "wa_worker.h"

class WaMetascanErrChecker
{
public:
    static void pushInternalErr( WaWorker* p_des_worker, std::wstring err_msg );
    static void pushServerErr( WaWorker* p_des_worker, WaJson* p_err );
    static void pushServerErr( WaWorker* p_des_worker, std::wstring& content );
};

#endif
