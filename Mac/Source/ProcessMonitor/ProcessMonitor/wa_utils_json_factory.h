//
//  wa_utils_json_factory.h
//  SelfYajl
//
//  Created by NTViet-MAC on 3/26/14.
//  Copyright (c) 2014 test-MAC. All rights reserved.
//

#ifndef SelfYajl_wa_utils_json_factory_h
#define SelfYajl_wa_utils_json_factory_h

#include "wa_utils_json.h"
#include "wa_api_error_codes.h"

/**
 *	@brief Will create @ref WaJson instances from @JSON formatted strings
 *	@ingroup waapi_internal_json
 */

struct TElement
{
    wa_wstring* pKey;
    WaJson*     pValue;
    TElement*   pNext;
    
    TElement( WaJson* p_in )
    {
        pKey = NULL;
        pValue = p_in;
        pNext = NULL;
    }
};

class WaJsonFactory
{
public:
    
    /**
	 *	@brief Constructor
	 */
    WaJsonFactory();
    
    /**
	 *	@brief Destructor
	 */
    ~WaJsonFactory();
    
    /**
	 *	@brief Will create a @ref WaJson instance for the given @JSON string
	 *	@param[in] json_in The @JSON formatted string to create our class instance from. If @c NULL is passed a @JSON value of type @ref WAAPI_JSON_NULL is returned
	 *	@param[out] json_value The class instance that represents the JSON string
	 *	@retval WAAPI_OK On successful conversion
	 *	@retval WAAPI_ERROR_INVALID_JSON The @JSON formatted string is not valid
	 */
    wa_int create( wa_wstring json_in, WaJson & json_value );
    wa_int create( const wa_wchar * json_in, WaJson & json_value );
    
private:
    
    WaJsonFactory& operator=( const WaJsonFactory &assign_from ) { return *this; };
    WaJsonFactory( const WaJsonFactory &assign_from ) {};
    
    void            setUpStack();
    void            freeStack();
    
    void            push( WaJson* p_value );
    WaJson*         pop();
    wa_int   addValue( WaJson* p_new );
    
    static int      handleString( void *ctx, const unsigned char *stream, size_t string_length );
    static int      handleNumber( void *ctx, const char *stream, size_t string_length );
    static int      handleStartObject( void *ctx );
    static int      handleEndObject( void *ctx );
    static int      handleStartArray( void *ctx );
    static int      handleEndArray( void *ctx );
    static int      handleBoolean( void *ctx, int boolean_value );
    static int      handleNull( void *ctx );
    
private:
    
    TElement* m_pStack;
};

#endif
