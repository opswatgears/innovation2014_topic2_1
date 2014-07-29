//
//  wa_utils_json_factory.cpp
//  SelfYajl
//
//  Created by NTViet-MAC on 3/26/14.
//  Copyright (c) 2014 test-MAC. All rights reserved.
//

#include "wa_utils_json_factory.h"
#include "wa_utils_string.h"
#include "yajl_parser.h"
#include <string>


#define STATUS_ABORT        0
#define STATUS_CONTINUE     1

WaJsonFactory::WaJsonFactory()
: m_pStack( NULL )
{
    
}

WaJsonFactory::~WaJsonFactory()
{
    this->freeStack();
}

void WaJsonFactory::setUpStack()
{
    this->m_pStack = NULL;
}

void WaJsonFactory::freeStack()
{
    if ( this->m_pStack != NULL )
    {
        TElement* p_temp = this->m_pStack;
        
        while ( p_temp != NULL )
        {
            TElement* p_del = p_temp;
            p_temp = p_temp->pNext;
            
            if ( p_del->pKey != NULL )
                delete p_del->pKey;
            
            if ( p_del->pValue != NULL )
                delete p_del->pValue;
            
            delete p_del;
        }
        this->m_pStack = NULL;
    }
}

wa_int WaJsonFactory::create( const wa_wchar * json_in, WaJson & json_value )
{
	if( json_in == NULL )
	{
		json_value.clear();
		return WAAPI_OK;
	}
    
	return this->create( wa_wstring( json_in ), json_value );
}

wa_int WaJsonFactory::create( wa_wstring json_in, WaJson& json_value )
{
    // validate against null
	if( json_in.empty() )
	{
		return WAAPI_ERROR_INVALID_JSON;	}
    
    this->setUpStack();
    
    static const yajl_callbacks p_callbacks =
    {
        /* null        = */ WaJsonFactory::handleNull,
        /* boolean     = */ WaJsonFactory::handleBoolean,
        /* integer     = */ NULL,
        /* double      = */ NULL,
        /* number      = */ WaJsonFactory::handleNumber,
        /* string      = */ WaJsonFactory::handleString,
        /* start map   = */ WaJsonFactory::handleStartObject,
        /* map key     = */ WaJsonFactory::handleString,
        /* end map     = */ WaJsonFactory::handleEndObject,
        /* start array = */ WaJsonFactory::handleStartArray,
        /* end array   = */ WaJsonFactory::handleEndArray
    };
    
    yajl_handle handle;
    yajl_status status;

    handle = yajl_alloc ( &p_callbacks, NULL, ( void* )this );
    yajl_config(handle, yajl_allow_comments, 1);
    
    std::string s_input = WaStringUtils::wstrToStr( json_in );
    
    handle->lexer = yajl_lex_alloc( &( handle->alloc ),
                                    ( handle->flags & yajl_allow_comments ),
                                   !( handle->flags & yajl_dont_validate_strings ) );
    
    do
    {
        status = yajl_do_parse( handle, ( const unsigned char* )s_input.c_str(), s_input.length() );
        if ( status != yajl_status_ok )
            break;
        
        status = yajl_do_finish( handle );
        
    } while (0);
    
    if ( status != yajl_status_ok )
    {
        this->freeStack();
        yajl_free (handle);
        
        // if failed, clear values
        json_value.clear();
        return WAAPI_ERROR_INVALID_JSON;
    }
    
    yajl_free (handle);
    WaJson* p_root = this->pop();
    json_value = *p_root;
    delete p_root;
    
    this->freeStack();
    
    return WAAPI_OK;
}

void WaJsonFactory::push( WaJson* p_value )
{
    TElement* p_ele = new TElement( p_value );
    if ( this->m_pStack != NULL )
    {
        p_ele->pNext = this->m_pStack;
    }
    this->m_pStack = p_ele;
}

WaJson* WaJsonFactory::pop()
{
    if ( this->m_pStack == NULL )
    {
        return NULL;      //RETURN_ERROR (ctx, NULL, "context_pop: Bottom of stack reached prematurely");
    }
    
    TElement* p_top = this->m_pStack;
    this->m_pStack = p_top->pNext;
    
    WaJson* p_ret = p_top->pValue;
    delete p_top;
    
    return p_ret;
}

/*
 * Add a value to the value on top of the stack or the "root" member in the
 * context if the end of the parsing process is reached.
 */
wa_int WaJsonFactory::addValue( WaJson* p_new )
{
    /*
     * There are three valid states in which this function may be called:
     *   - There is no value on the stack => This is the only value. This is the
     *     last step done when parsing a document. We assign the value to the
     *     "root" member and return.
     *   - The value on the stack is an object. In this case store the key on the
     *     stack or, if the key has already been read, add key and value to the
     *     object.
     *   - The value on the stack is an array. In this case simply add the value
     *     and return.
     */
    if ( this->m_pStack == NULL )
    {
        this->push( p_new );
        return WAAPI_OK;
    }
    
    int i_ret = WAAPI_ERROR_INVALID_JSON;
    TElement* p_top = this->m_pStack;
    
    if ( p_top->pValue->isType( WAAPI_JSON_OBJECT ) == true )
    {
        if ( p_top->pKey == NULL )
        {
            if ( p_new->isType( WAAPI_JSON_STRING ) == true )
            {
                p_top->pKey = new wa_wstring();
                p_new->val( *( p_top->pKey ) );
                i_ret = WAAPI_OK;
            }
            else
            {
                i_ret = WAAPI_ERROR_INVALID_JSON;
            }
            /*RETURN_ERROR (ctx, EINVAL, "context_add_value: "
             "Object key is not a string (%#04x)",
             v->type);*/
        }
        else /* if (ctx->key != NULL) */
        {
            p_top->pValue->put( *( p_top->pKey ), *p_new );
            delete p_top->pKey;
            p_top->pKey = NULL;
            
            i_ret = WAAPI_OK;
        }
    }
    else
    {
        if ( p_top->pValue->isType( WAAPI_JSON_ARRAY ) == true )
        {
            p_top->pValue->add( *p_new );
            i_ret = WAAPI_OK;
        }
        else
        {
            i_ret = WAAPI_ERROR_INVALID_JSON;
        }
        /*RETURN_ERROR (ctx, EINVAL, "context_add_value: Cannot add value to "
                      "a value of type %#04x (not a composite type)",*/
    }
    
    delete p_new;
    return i_ret;
}


int WaJsonFactory::handleString( void *ctx, const unsigned char *stream, size_t string_length )
{
    char* s_temp = new char[string_length + 1];
    memcpy( s_temp, stream, string_length);
    s_temp[string_length] = '\0';
    
    WaJson* p_value = new WaJson( WaStringUtils::_T( s_temp ).c_str() );
    delete[] s_temp;
    
    return ( ( ( ( WaJsonFactory* )ctx )->addValue( p_value ) == WAAPI_OK )? STATUS_CONTINUE : STATUS_ABORT );
}

int WaJsonFactory::handleNumber( void *ctx, const char *stream, size_t string_length )
{
    char* s_temp = new char[string_length + 1];
    memcpy( s_temp, stream, string_length);
    s_temp[string_length] = '\0';
    
    wa_int i_temp = WaStringUtils::strToInt( s_temp );
    delete[] s_temp;
    
    WaJson* p_value = new WaJson( i_temp );
    return ( ( ( ( WaJsonFactory* )ctx )->addValue( p_value ) == WAAPI_OK )? STATUS_CONTINUE : STATUS_ABORT );
}

int WaJsonFactory::handleStartObject( void *ctx )
{
    ( ( WaJsonFactory* )ctx )->push( new WaJson( WAAPI_JSON_OBJECT ) );
    return STATUS_CONTINUE;
}

int WaJsonFactory::handleEndObject( void *ctx )
{
    WaJsonFactory* p_factory = ( WaJsonFactory* )ctx;
    WaJson* p_object = p_factory->pop();
    
    if ( p_object != NULL )
    {
        return ( ( p_factory->addValue( p_object ) == WAAPI_OK )?  STATUS_CONTINUE : STATUS_ABORT );
    }
    
    return STATUS_ABORT;
}

int WaJsonFactory::handleStartArray( void *ctx )
{
    ( ( WaJsonFactory* )ctx )->push( new WaJson( WAAPI_JSON_ARRAY ) );
    return STATUS_CONTINUE;
}

int WaJsonFactory::handleEndArray( void *ctx )
{
    WaJsonFactory* p_factory = ( WaJsonFactory* )ctx;
    WaJson* p_array = p_factory->pop();
    
    if ( p_array != NULL )
    {
        return ( ( p_factory->addValue( p_array ) == WAAPI_OK )?  STATUS_CONTINUE : STATUS_ABORT );
    }
    
    return STATUS_ABORT;
}

int WaJsonFactory::handleBoolean( void *ctx, int boolean_value )
{
    bool b_value = ( boolean_value ? true: false );
    WaJson* p_value = new WaJson( b_value );
    
    return ( ( ( ( WaJsonFactory* )ctx )->addValue( p_value ) == WAAPI_OK )? STATUS_CONTINUE : STATUS_ABORT );
}

int WaJsonFactory::handleNull( void *ctx )
{
    WaJson* p_value = new WaJson( WAAPI_JSON_NULL );
    return ( ( ( ( WaJsonFactory* )ctx )->addValue( p_value ) == WAAPI_OK )? STATUS_CONTINUE : STATUS_ABORT );
}