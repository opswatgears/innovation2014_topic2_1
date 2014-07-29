#define _WAAPI_EXPORTS_

#include <assert.h>
#include "wa_utils_json.h"
#include "wa_api_error_codes.h"

// internal "constructor" used by all constructors
void WaJson::_init()
{
	// initialize our object with "none" values
	m_Value.json_string		= NULL;
	m_Value.json_array		= NULL;
	m_Value.json_object		= NULL;
	m_Value.json_number		= 0;
	m_Value.json_boolean	= false;
    
	// by default, we are null
	m_Type = WAAPI_JSON_NULL;
}

// start of constructors
WaJson::WaJson()
{
	_init();
}

WaJson::WaJson( const WaJson & json )
{
	_init();
	switch( json.m_Type )
	{
        case WAAPI_JSON_STRING:
            _set( json.m_Value.json_string->c_str() );
            break;
        case WAAPI_JSON_ARRAY:
            _set( *json.m_Value.json_array );
            break;
        case WAAPI_JSON_OBJECT:
            _set( *json.m_Value.json_object );
            break;
        case WAAPI_JSON_NUMBER:
            _set( json.m_Value.json_number );
            break;
        case WAAPI_JSON_BOOL:
            _set( json.m_Value.json_boolean );
            break;
        case WAAPI_JSON_NULL:
            break;
	}
}

WaJson::WaJson( const  wa_wchar * json_string )
{
	_init();
	_set( json_string );
}

WaJson::WaJson( const wa_int & json_number )
{
	_init();
	_set( json_number );
}

WaJson::WaJson( bool json_boolean )
{
	_init();
	_set( json_boolean );
}

WaJson::WaJson( const wa_json_array & json_array )
{
	_init();
	_set( json_array );
}

WaJson::WaJson( const wa_json_object & json_object )
{
	_init();
	_set( json_object );
}

WaJson::WaJson( const wa_wstring & key, const WaJson & value )
{
	_init();
	put(key, value);
}

WaJson::WaJson( WaJsonType json_type )
{
	_init();
	m_Type = json_type;
	switch( json_type )
	{
        case WAAPI_JSON_STRING:
            m_Value.json_string = new wa_wstring();
            break;
        case WAAPI_JSON_ARRAY:
            m_Value.json_array = new wa_json_array();
            break;
        case WAAPI_JSON_OBJECT:
            m_Value.json_object = new wa_json_object();
            break;
        case WAAPI_JSON_NUMBER:
        case WAAPI_JSON_BOOL:
        case WAAPI_JSON_NULL:
            break;
	}
}

// destructor
WaJson::~WaJson()
{
	clear();
}

WaJson & WaJson::operator+=( const WaJson &add_from )
{
	if( add_from.m_Type == this->m_Type  )
	{
		switch( this->m_Type )
		{
            case WAAPI_JSON_NUMBER:
                _set( this->m_Value.json_number + add_from.m_Value.json_number );
                break;
            case WAAPI_JSON_STRING:
                _set( ( *(this->m_Value.json_string) + *(add_from.m_Value.json_string) ).c_str() );
                break;
            case WAAPI_JSON_OBJECT:
                this->m_Value.json_object->insert( add_from.m_Value.json_object->begin(), add_from.m_Value.json_object->end() );
                break;
            case WAAPI_JSON_ARRAY:
                this->m_Value.json_array->insert( this->m_Value.json_array->end(), add_from.m_Value.json_array->begin(),add_from.m_Value.json_array->end() );
                break;
            default:
                break;
		}
	}
	return (*this);
}

WaJson & WaJson::operator=(const WaJson &assign_from )
{
	// disallow self assignment
	if( this == &assign_from )
		return (*this);
    
	// do set calls
	switch( assign_from.m_Type )
	{
        case WAAPI_JSON_NUMBER:
            _set( assign_from.m_Value.json_number );
            break;
        case WAAPI_JSON_BOOL:
            _set( assign_from.m_Value.json_boolean );
            break;
        case WAAPI_JSON_STRING:
            _set( assign_from.m_Value.json_string->c_str() );
            break;
        case WAAPI_JSON_ARRAY:
            _set( *assign_from.m_Value.json_array );
            break;
        case WAAPI_JSON_OBJECT:
            _set( *assign_from.m_Value.json_object );
            break;
        case WAAPI_JSON_NULL:
        default:
            clear();
            break;
	}
	return (*this);
}

WaJson & WaJson::operator=( const  wa_wchar * assign_from )
{
	_set( assign_from );
	return (*this);
}

WaJson & WaJson::operator=( const wa_int &assign_from )
{
	_set( assign_from );
	return (*this);
}

WaJson & WaJson::operator=( bool assign_from )
{
	_set( assign_from );
	return (*this);
}

WaJson & WaJson::operator=( const wa_json_array &assign_from )
{
	_set( assign_from );
	return (*this);
}

WaJson & WaJson::operator=( const wa_json_object &assign_from )
{
	_set( assign_from );
	return (*this);
}

WaJson & WaJson::operator[]( size_t index )
{
	// only allow if type is array
	assert( m_Type == WAAPI_JSON_ARRAY );
    
	// validate the size and index position
	assert( index >= 0 && (size_t)index < m_Value.json_array->size() );
	
	// return object
	return m_Value.json_array->at( index );
}

const WaJson & WaJson::operator[]( size_t index ) const
{
	// only allow if type is array
	assert( m_Type == WAAPI_JSON_ARRAY );
    
	// validate the size and index position
	assert( index >= 0 && (size_t)index < m_Value.json_array->size() );
    
	// return object
	return m_Value.json_array->at( index );
}

WaJson & WaJson::operator[]( const wa_wchar * key )
{
	//only allow if type is object
	assert( m_Type == WAAPI_JSON_OBJECT );
    
	// find value
	wa_json_object::iterator fIter = m_Value.json_object->find( key );
	
	// make sure it exists
	assert( fIter != m_Value.json_object->end() );
    
	// return object
	return fIter->second;
}

const WaJson & WaJson::operator[]( const wa_wchar * key ) const
{
	//only allow if type is object
	assert( m_Type == WAAPI_JSON_OBJECT );
    
	// find value
	wa_json_object::iterator fIter = m_Value.json_object->find( key );
    
	// make sure it exists
	assert( fIter != m_Value.json_object->end() );
    
	// return object
	return fIter->second;
}

WaJson & WaJson::operator[]( const wa_wstring & key )
{
	return (*this)[ key.c_str() ];
}

const WaJson & WaJson::operator[]( const wa_wstring & key ) const
{
	return (*this)[ key.c_str() ];
}

bool WaJson::operator!=( const WaJson & _rhs ) const
{
	return !((*this) == _rhs);
}

bool WaJson::operator==( const WaJson & _rhs ) const
{
	return !((*this) < _rhs) && !(_rhs < (*this));
}

bool WaJson::operator<( const WaJson & _rhs ) const
{
	if(getType() != _rhs.getType())
		return getType() < _rhs.getType();
    
	switch( m_Type )
	{
        case WAAPI_JSON_STRING:
            return *(m_Value.json_string) < *(_rhs.m_Value.json_string);
        case WAAPI_JSON_ARRAY:
        {
            if (size() != _rhs.size())
                return size() < _rhs.size();
            
            for (size_t i = 0; i < size(); i++)
            {
                if ((*this)[i] < _rhs[i])
                    return true;
            }
            
            return false;
        }
        case WAAPI_JSON_OBJECT:
        {
            if (size() != _rhs.size())
                return size() < _rhs.size();
            
            auto objKeys = keys();
            auto rhsKeys = _rhs.keys();
            auto itr = objKeys.begin();
            auto rhsItr = rhsKeys.begin();
            for (; itr != objKeys.end(); itr++, rhsItr++)
            {
                if (*itr != *rhsItr)
                    return *itr < *rhsItr;
                
                if ((*this)[*itr] != _rhs[*rhsItr])
                    return (*this)[*itr] < _rhs[*rhsItr];
            }
            
            return false;
        }
        case WAAPI_JSON_NUMBER:
            return m_Value.json_number < _rhs.m_Value.json_number;
        case WAAPI_JSON_BOOL:
            return m_Value.json_boolean < _rhs.m_Value.json_boolean;
        case WAAPI_JSON_NULL:
            return false;
	}
    
	assert( false ); // invalid type
	return false;
}

void WaJson::convert( const WaJsonType & json_type )
{
	// cleanup first
	clear();
    
	// update with empty types
	m_Type = json_type;
	switch( json_type )
	{
        case WAAPI_JSON_STRING:
            m_Value.json_string = new wa_wstring();
            break;
        case WAAPI_JSON_ARRAY:
            m_Value.json_array = new wa_json_array();
            break;
        case WAAPI_JSON_OBJECT:
            m_Value.json_object = new wa_json_object();
            break;
        case WAAPI_JSON_NUMBER:
        case WAAPI_JSON_BOOL:
        case WAAPI_JSON_NULL:
            break;
	}
}

WaJsonType WaJson::getType() const
{
	return m_Type;
}

bool WaJson::isType( const WaJsonType & json_type ) const
{
	return ( m_Type == json_type );
}

bool WaJson::isObjKeyType( const  wa_wchar * key, const WaJsonType & json_type ) const
{
	// The commented out code here is not as efficent as the code below
	// return ( this->contains( key ) && this->get( key ).isType( json_type ) );
	if( m_Type == WAAPI_JSON_OBJECT )
	{
		wa_json_object::iterator fIter = m_Value.json_object->find( key );
		if( fIter != m_Value.json_object->end() )
		{
			return fIter->second.isType( json_type );
		}
	}
	return false;
}

void WaJson::clear()
{
	switch( m_Type )
	{
        case WAAPI_JSON_STRING:
            if( m_Value.json_string != NULL )
            {
                delete (m_Value.json_string );
                m_Value.json_string = NULL;
            }
            break;
        case WAAPI_JSON_ARRAY:
            if( m_Value.json_array != NULL )
            {
                delete (m_Value.json_array );
                m_Value.json_array = NULL;
            }
            break;
        case WAAPI_JSON_OBJECT:
            if( m_Value.json_object != NULL )
            {
                delete (m_Value.json_object);
                m_Value.json_object = NULL;
            }
            break;
        default:
            break;
	}
    
	// null out
	_init();
}

size_t WaJson::size() const
{
	if( m_Type == WAAPI_JSON_ARRAY )
	{
		return m_Value.json_array->size();
	}
	else if( m_Type == WAAPI_JSON_OBJECT )
	{
		return m_Value.json_object->size();
	}
	return 0;
}

void WaJson::add( const WaJson & json_value )
{
	if( m_Type != WAAPI_JSON_ARRAY )
	{
		clear();
		m_Value.json_array = new wa_json_array();
		m_Type = WAAPI_JSON_ARRAY;
	}
	m_Value.json_array->push_back( json_value );
}

WaJson WaJson::at( size_t index ) const
{
	return (*this)[ index ];
}

void WaJson::put( const  wa_wchar * key, const WaJson & json_value )
{
	put( wa_wstring( key ), json_value );
}

void WaJson::put( const wa_wstring & key, const WaJson & json_value  )
{
	if( m_Type != WAAPI_JSON_OBJECT )
	{
		clear();
		m_Value.json_object= new wa_json_object();
		m_Type = WAAPI_JSON_OBJECT;
	}
	else
	{
		// remove if the previous object has this key-value pair already
		remove( key );
	}
	m_Value.json_object->insert( std::make_pair( key, json_value ) );
}

WaJson WaJson::get( const  wa_wchar * key ) const
{
	return (*this)[ key ];
}

WaJson WaJson::get( const wa_wstring & key ) const
{
	return get( key.c_str() );
}

wa_int WaJson::get( const  wa_wchar * key, wa_wstring & val ) const
{
	if( m_Type == WAAPI_JSON_OBJECT )
	{
		wa_json_object::iterator fIter = m_Value.json_object->find( key );
		if( fIter == m_Value.json_object->end() )
		{
			return WAAPI_ERROR_NOT_FOUND;
		}
		if( fIter->second.getType() != WAAPI_JSON_STRING )
		{
			return WAAPI_ERROR_INVALID_STATE;
		}
		// get value as string
		fIter->second.val( val );
		return WAAPI_OK;
	}
	return WAAPI_ERROR_INVALID_STATE;
}

wa_int WaJson::get( const wa_wstring & key, wa_wstring & val ) const
{
	return get( key.c_str(), val );
}

wa_int WaJson::get( const  wa_wchar * key, wa_int & val ) const
{
	if( m_Type == WAAPI_JSON_OBJECT )
	{
		wa_json_object::iterator fIter = m_Value.json_object->find( key );
		if( fIter == m_Value.json_object->end() )
		{
			return WAAPI_ERROR_NOT_FOUND;
		}
		if( fIter->second.getType() != WAAPI_JSON_NUMBER )
		{
			return WAAPI_ERROR_INVALID_STATE;
		}
		// get value as int
		fIter->second.val( val );
		return WAAPI_OK;
	}
	return WAAPI_ERROR_INVALID_STATE;
}

wa_int WaJson::get( const wa_wstring & key, bool & val ) const
{
	return get( key.c_str(), val );
}

wa_int WaJson::get( const  wa_wchar * key, bool & val ) const
{
	if( m_Type == WAAPI_JSON_OBJECT )
	{
		wa_json_object::iterator fIter = m_Value.json_object->find( key );
		if( fIter == m_Value.json_object->end() )
		{
			return WAAPI_ERROR_NOT_FOUND;
		}
		if( fIter->second.getType() != WAAPI_JSON_BOOL )
		{
			return WAAPI_ERROR_INVALID_STATE;
		}
		// get value as bool
		fIter->second.val( val );
		return WAAPI_OK;
	}
	return WAAPI_ERROR_INVALID_STATE;
}

wa_int WaJson::get( const wa_wstring & key, wa_int & val ) const
{
	return get( key.c_str(), val );
}

wa_int WaJson::get( const  wa_wchar * key, WaJson & val ) const
{
	if( m_Type == WAAPI_JSON_OBJECT )
	{
		wa_json_object::iterator fIter = m_Value.json_object->find( key );
		if( fIter == m_Value.json_object->end() )
		{
			return WAAPI_ERROR_NOT_FOUND;
		}
		val = fIter->second;
		return WAAPI_OK;
	}
	return WAAPI_ERROR_INVALID_STATE;
}

wa_int WaJson::get( const wa_wstring & key, WaJson & val ) const
{
	return get( key.c_str(), val );
}

void WaJson::remove( const  wa_wchar * key )
{
	if( m_Type == WAAPI_JSON_OBJECT )
	{
		wa_json_object::iterator fIter = m_Value.json_object->find( key );
		if( fIter != m_Value.json_object->end() )
		{
			m_Value.json_object->erase( fIter );
		}
	}
}

void WaJson::remove( const wa_wstring & key )
{
	remove( key.c_str() );
}

bool WaJson::contains( const  wa_wchar * key ) const
{
	if( m_Type == WAAPI_JSON_OBJECT )
	{
		return ( m_Value.json_object->find( key ) != m_Value.json_object->end() ) ? true : false;
	}
	return false;
}

wa_json_object_keys WaJson::keys() const
{
	wa_json_object_keys keys;
	if(  m_Type == WAAPI_JSON_OBJECT )
	{
		// populate our keys
		for( wa_json_object::iterator iter = m_Value.json_object->begin(); iter != m_Value.json_object->end(); ++iter )
		{
			keys.insert( iter->first );
		}
	}
	return keys;
}

void WaJson::_set( const wa_int & json_number )
{
	clear();
	m_Value.json_number = json_number;
	m_Type = WAAPI_JSON_NUMBER;
}

void WaJson::_set( bool json_boolean )
{
	clear();
	m_Value.json_boolean = json_boolean;
	m_Type = WAAPI_JSON_BOOL;
}

void WaJson::_set( const  wa_wchar * json_string )
{
	clear();
	m_Value.json_string = new wa_wstring( json_string );
	m_Type = WAAPI_JSON_STRING;
}

void WaJson::_set( const wa_json_array & json_array )
{
	clear();
	m_Value.json_array = new wa_json_array( json_array );
	m_Type = WAAPI_JSON_ARRAY;
}

void WaJson::_set( const wa_json_object & json_object )
{
	clear();
	m_Value.json_object = new wa_json_object( json_object );
	m_Type = WAAPI_JSON_OBJECT;
}

wa_int WaJson::val( wa_int & json_value ) const
{
	if( m_Type == WAAPI_JSON_NUMBER )
	{
		json_value = m_Value.json_number;
		return WAAPI_OK;
	}
	return WAAPI_ERROR_INVALID_JSON;
}

wa_int WaJson::val( wa_wstring & json_value ) const
{
	if( m_Type == WAAPI_JSON_STRING )
	{
		json_value = *m_Value.json_string;
		return WAAPI_OK;
	}
	return WAAPI_ERROR_INVALID_JSON;
}

wa_int WaJson::val( bool & json_value ) const
{
	if( m_Type == WAAPI_JSON_BOOL )
	{
		json_value = m_Value.json_boolean;
		return WAAPI_OK;
	}
	return WAAPI_ERROR_INVALID_JSON;
}

wa_wstring WaJson::toString() const
{
	wa_wstring sRet = L"";
    
	switch( m_Type )
	{
        case WAAPI_JSON_STRING:
            sRet = L"\"" + ( ( m_Value.json_string != NULL ) ? _encodeString( *(m_Value.json_string) ) : L"" ) + L"\"";
            break;
            
        case WAAPI_JSON_BOOL:
            sRet = ( m_Value.json_boolean ) ? L"true" : L"false";
            break;
            
        case WAAPI_JSON_NUMBER:
            sRet = std::to_wstring( m_Value.json_number );
            break;
            
        case WAAPI_JSON_ARRAY:
            sRet = L"[";
            if( m_Value.json_array != NULL )
            {
                for( wa_json_array::iterator iter = (m_Value.json_array)->begin();
                    iter != (m_Value.json_array)->end();
                    ++iter )
                {
                    // add a comma between each object
                    if( iter != (m_Value.json_array)->begin() )
                    {
                        sRet += L",";
                    }
                    sRet += iter->toString();
                }
            }
            sRet += L"]";
            break;
            
        case WAAPI_JSON_OBJECT:
            sRet = L"{";
            if( m_Value.json_object != NULL )
            {
                for( wa_json_object::iterator iter = (m_Value.json_object)->begin();
                    iter != (m_Value.json_object)->end();
                    ++iter )
                {
                    if( iter != (m_Value.json_object)->begin() )
                    {
                        sRet += L",";
                    }
                    sRet += L"\"" + _encodeString( iter->first ) + L"\":" + iter->second.toString();
                }
            }
            sRet += L"}";
            break;
            
        case WAAPI_JSON_NULL:
        default:
            sRet = L"null";
            break;
	}
	return sRet;
}

wa_wstring WaJson::toPrettyString() const
{
	return _toPrettyString( 0 );
}

WaJson WaJson::first()
{
	WaJson ret;
    
	if (size() == 0)
		return ret;
    
	switch( m_Type )
	{
        case WAAPI_JSON_ARRAY:
            ret = (*this)[(size_t)0];
            break;
            
        case WAAPI_JSON_OBJECT:
            ret = m_Value.json_object->begin()->second;
            break;
        default:
            break;
	}
    
	return ret;
}

void WaJson::toCharPointer(  wa_wchar ** json_out ) const
{
    // get json to string format
	wa_wstring json_tmp = toString();
    
	// allocate space for buffer
	*json_out = new  wa_wchar[ json_tmp.length() + 1 ];
    
	// copy result back to the user
	wcscpy( *json_out, json_tmp.c_str() );
}

void WaJson::toPrettyCharPointer(  wa_wchar ** json_out ) const
{
	// get json to string format
	wa_wstring json_tmp = toPrettyString();
    
	// allocate space for buffer
	*json_out = new  wa_wchar[ json_tmp.length() + 1 ];
    
	// copy result back to the user
	wcscpy( *json_out, json_tmp.c_str() );
}

wa_wstring WaJson::_toPrettyString( int indent, bool indent_start /*= true*/ ) const
{
	wa_wstring sRet = L"";
    
	switch( m_Type )
	{
        case WAAPI_JSON_ARRAY:
            
            indent = indent + 4;
            
            for( int j = 0; indent_start && j < indent - 4; ++j )
            {
                sRet += L" ";
            }
            
            sRet += L"[";
            if( m_Value.json_array != NULL )
            {
                for( wa_json_array::iterator iter = (m_Value.json_array)->begin();
                    iter != (m_Value.json_array)->end();
                    ++iter )
                {
                    // add a comma between each object
                    if( iter != (m_Value.json_array)->begin() )
                    {
                        sRet += L",";
                    }
                    if( iter->getType() != WAAPI_JSON_OBJECT )
                    {
                        sRet += L"\n";
                    }
                    for( int i = 0; i < indent; ++i )
                    {
                        sRet += L" ";
                    }
                    // start arrays and objects on a new line
                    if( iter->getType() == WAAPI_JSON_ARRAY  )
                    {
                        sRet += L"\n" + iter->_toPrettyString( indent + 4 );
                    }
                    else if( iter->getType() == WAAPI_JSON_OBJECT )
                    {
                        sRet += L"\n" + iter->_toPrettyString( indent );
                    }
                    else
                    {
                        sRet += iter->toString();
                    }
                }
            }
            sRet += L"\n";
            
            for( int j = 0; j < indent - 4; ++j ) 
            {
                sRet += L" ";
            }
            
            sRet += L"]";
            break;
            
        case WAAPI_JSON_OBJECT:
            
            indent = indent + 4;
            
            for( int j = 0; indent_start && j < indent - 4; ++j ) 
            {
                sRet += L" ";
            }
            
            sRet += L"{";
            if( m_Value.json_object != NULL )
            {
                for( wa_json_object::iterator iter = (m_Value.json_object)->begin();
                    iter != (m_Value.json_object)->end();
                    ++iter )
                {
                    if( iter != (m_Value.json_object)->begin() )
                    {
                        sRet += L",";
                    }
                    sRet += L"\n";
                    
                    for( int i = 0; i < indent; ++i )
                    {
                        sRet += L" ";
                    }
                    
                    sRet += L"\"" + _encodeString( iter->first ) + L"\" : ";
                    
                    // start arrays and objects on a new line
                    if( iter->second.getType() == WAAPI_JSON_ARRAY || iter->second.getType() == WAAPI_JSON_OBJECT )
                    {
                        sRet += /*"\n" + */ iter->second._toPrettyString( indent + 4 , false );
                    }
                    else
                    {
                        sRet += iter->second.toString();
                    }
                }
            }
            sRet += L"\n";
            for( int j = 0; j < indent - 4; ++j ) 
            {
                sRet += L" ";
            }
            sRet += L"}";
            break;
            
        default:
            sRet = toString();
            break;
	}
	return sRet;
}

wa_wstring WaJson::_encodeString( const wa_wstring & to_encode ) const
{
	// store escapes to return
	wa_wstring ret = L"";
    
	for( size_t i = 0; i < to_encode.size(); ++i )
	{
		// pull character
        wa_wchar c = to_encode[i];
        
		// set it as if it is our append value already
		wa_wstring append_str = L"";
		append_str += c;
        
		switch( c )
		{
            case '"':
                append_str = L"\\\"";
                break;
            case '\\':
                append_str = L"\\\\";
                break;
            case '\t':
                append_str = L"\\t";
                break;
            case '\n':
                append_str = L"\\n";
                break;
            case '\r':
                append_str = L"\\r";
                break;
            case '\b':
                append_str = L"\\b";
                break;
            case '\f':
                append_str = L"\\f";
                break;
            default:
                #ifdef _WAAPI_WINDOWS_
                // anything less than a space and greater than DEL, escape as unicode
                if( !(c >= 0x20 && c < 0x7F) )
                {
                    wa_wchar buff[16];
                    swprintf( ( wchar_t* )buff, 16, L"\\u%04x", c );
                    append_str = buff;
                }
                #endif
                break;
		}
		ret += append_str;
	}
	return ret;
}

