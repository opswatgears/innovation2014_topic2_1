#include "wa_utils_json_factory.h"
#include "wa_api_error_codes.h"

// utilities
#include "wa_utils_string.h"

WaJsonFactory::WaJsonFactory()
{

}

WaJsonFactory::~WaJsonFactory()
{

}

WaJsonFactory & WaJsonFactory::operator=( const WaJsonFactory &assign_from )
{
	// nothing to do here
	return (*this);
}

WAUTIL_RETURN WaJsonFactory::create( const wa_wchar * json_in, WaJson & json_value )
{
	if( json_in == NULL )
	{
		json_value.clear();
		return WAAPI_OK;
	}

	return create( wa_wstring( json_in ), json_value );
}

WAUTIL_RETURN WaJsonFactory::create( wa_wstring json_in, WaJson & json_value )
{
	// validate against null
	if( json_in.empty() )
	{
		return WAAPI_ERROR_INVALID_JSON;
	}

	// clear all new lines so we are working with 1 constant string
	//WaStringUtils::replaceAll( json_in, L"\n", L" " );
	WaStringUtils::replaceChars( json_in, L"\n\r\t", L' ');

	// pass to recursive creator
	std::size_t pos = 0;
	wa_int rc =  _create( json_in, pos, json_value );
	if( WAAPI_FAILED( rc ) )
	{
		// if failed, clear values
		json_value.clear();
		return rc;
	} 
	else
	{
		// clear any left behind trailing whitespace
		_ignoreWhiteSpace( json_in, pos );
	}
	// check for any additional trailing content
	if( pos < json_in.length() )
	{
		json_value.clear();
		return WAAPI_ERROR_INVALID_JSON;
	}

	return WAAPI_OK;
}

WAUTIL_RETURN WaJsonFactory::_create( const wa_wstring & str, size_t & pos, WaJson & json_value )
{
	// ignore all white space and non-ascii we dislike
	_ignoreWhiteSpace( str, pos );

	// handle special wrapping case
	if( str[ pos ] == L'(' )
	{
		pos++;
	}

	// holder for converting strings
	wa_wstring json_string;

	switch( str[ pos ] )
	{
	case L'{' :
		return _createObject( str, ++pos, json_value );
	case L'[':
		return _createArray( str, ++pos, json_value );
	case L'"':
		if( WAAPI_FAILED( _createString( str, ++pos, json_string ) ) )
		{
			return WAAPI_ERROR_INVALID_JSON;
		}
		json_value = json_string.c_str();
		break;
	// handle the special cases for true, false and null
	case L't':
	case L'f':
	case L'n':
		return  _createSpecialCase( str, ++pos, json_value );
	default:
		return _createNumber( str, pos, json_value );
	}
	return WAAPI_OK;

}

void WaJsonFactory::_ignoreWhiteSpace( const wa_wstring & str, size_t & pos )
{
	// we do not want any white space or special characters (e.g. new line feeds, carriage returns, etc. )
	// take a look at an ASCII chart http://www.asciitable.com/
	while( pos < str.length() && /*pos != wa_wstring::npos &&*/ str[ pos ] <= 0x20 )
	{
		pos++;
	}
}

WAUTIL_RETURN WaJsonFactory::_createObject( const wa_wstring & str, size_t & pos, WaJson & json_object )
{
	// make the passed in value an empty object
	json_object.convert( WAAPI_JSON_OBJECT );

	// while we have not hit the end of our string continue to grab object data
	while( pos != wa_wstring::npos )
	{
		wa_wstring key;
		WaJson json_value;

		// skip white space
		_ignoreWhiteSpace( str, pos );

		// found the end of our map so we can return now
		if( str[ pos ] == L'}' && pos++ )
		{
			return WAAPI_OK;
		}

		if( str[ pos ] == L'"' && pos++ )
		{
			// parse out the key for this map entry
			if( WAAPI_FAILED( _createString( str, pos, key ) ) )
			{
				return WAAPI_ERROR_INVALID_JSON;
			}
		}
		else
		{
			return WAAPI_ERROR_INVALID_JSON;
		}

		// skip white space
		_ignoreWhiteSpace( str, pos );

		// we must have the value now for our map which is separated by ':'
		// if this is not the next character in the string then we have failed
		if( str[ pos ] != L':' )
		{
			return WAAPI_ERROR_INVALID_JSON;
		}

		// move past the ':'
		++pos;

		// retrieve the value for the map key
		if( WAAPI_FAILED( _create( str, pos, json_value ) ) )
		{
			return WAAPI_ERROR_INVALID_JSON;
		}

		// update our JSON instance with the mapped value
		json_object.put( key, json_value );

		// skip white space
		_ignoreWhiteSpace( str, pos );

		// check if we have a comma to check for, meaning we have multiple keys
		if( str[ pos ] == L',' && pos++ )
		{
			// check if our comma is a trailing comma, meaning it does not split an object which defines improper JSON format
			// if that is the case then we have failed
			_ignoreWhiteSpace( str, pos );
			if( str[ pos ] == L'}' )
			{
				return WAAPI_ERROR_INVALID_JSON;
			}
		}
	}

	return WAAPI_OK;
}

WAUTIL_RETURN WaJsonFactory::_createString( const wa_wstring & str, size_t & pos, wa_wstring & json_string )
{
	json_string = L"";
	do 
	{
		// we found the end of the string
		if( str[ pos ] == L'"' )
		{
			// increment and success
			pos++;
			return WAAPI_OK;
		}
		else if( str[ pos ] == L'\\' ) // escaped characters
		{
			wa_wchar next = str[ ++pos ];
			switch( next )
			{
			case L'\\':
			case L'/':
			case L'"':
				break;
			case L'b':   
				next = L'\b';  
				break;
			case L'n':   
				next = L'\n';  
				break;
			case L'r':   
				next = L'\r';  
				break;
			case L't':   
				next = L'\t';  
				break;
			case L'f':   
				next = L'\f';  
				break; 
			case L'u':
				// must escape in place
				wa_int rc;
				if( WAAPI_FAILED( rc = _decodeCharacter( str, next, ++pos ) ) )
				{
					return rc;
				}
				break;
			}
			json_string += next;
			pos++;
		}
		else if( str[pos ] < 0x20 ) // un-escaped character! means invalid JSON string
		{
			return WAAPI_ERROR_INVALID_JSON;
		}
		else
		{
			json_string += str[ pos++ ];
		}
	} 
	while ( pos != wa_wstring::npos );

	// failed to find the end of the string, therefore invalid JSON
	return  WAAPI_ERROR_INVALID_JSON;
}

WAUTIL_RETURN WaJsonFactory::_decodeCharacter( const wa_wstring & json_string, wa_wchar & json_decoded_character, size_t & pos )
{
	if( pos + 4 > json_string.length() )
	{
		return  WAAPI_ERROR_INVALID_JSON;
	}

	if( WAAPI_FAILED( _convertToHexQuad( json_string.substr( pos, 4 ), json_decoded_character ) ) )
	{
		return WAAPI_ERROR_INVALID_JSON;
	}

	pos += 4;

// wchar_t is 2 bytes on windows, while its 4 bytes on mac and linux. 
// windows cannot represent a surrogate pair like mac and linux, thus windows leaves these as pairs
#ifndef _WAAPI_WINDOWS_

	// high surrogate char?
	if( json_decoded_character >= 0xd800 )
	{
		// yes - expect a low char
		if( json_decoded_character < 0xdc00 )
		{
			wa_wchar low_character;
			if( !( json_string[ pos ] == L'\\' && ++pos && json_string[ pos ] == L'u' && ++pos && WAAPI_SUCCESS( _convertToHexQuad( json_string.substr( pos, 4 ), low_character ) ) ) )
			{
				// missing low character in surrogate pair
				return WAAPI_ERROR_INVALID_JSON;
			}

			pos += 4;

			if( low_character < 0xdc00 || low_character >= 0xdfff )
			{
				// invalid low surrogate pair
				return WAAPI_ERROR_INVALID_JSON;
			}

			json_decoded_character = (json_decoded_character - 0xd800 ) * 0x400 + (low_character - 0xdc00 ) + 0x10000;
		}
		else if( json_decoded_character < 0xe000 )
		{
			// invalid high character in surrogate pair
			return  WAAPI_ERROR_INVALID_JSON;
		}
	}

#endif

	// hacky fix to check string iteration in check, see createString()
	pos--;
	return  WAAPI_OK;
}


WAUTIL_RETURN WaJsonFactory::_convertToHexQuad( const wa_wstring & str, wa_wchar & converted )
{
	// check if we have the proper input
	if( str.size() != 4 )
	{
		return WAAPI_ERROR_INVALID_JSON;
	}

	// initialize
	converted = 0;

	// iterate over each HEX value
	for( size_t pos = 0; pos < 4; ++pos )
	{
		wa_wchar c = str[ pos ];

		// Check algorithm for converting HEX to char taken fro SBJson (Objective-C) parser
		int check = ( c >= '0' && c <= '9' )
			? c - '0' : ( c >= 'a' && c <= 'f' )
			? ( c - 'a' + 10 ) : (c >= 'A' && c <= 'F' )
			? ( c - 'A' + 10 ) : -1;
		if( check == -1 )
		{
			return WAAPI_ERROR_INVALID_JSON;
		}
		converted *= 16;
		converted += check;
	}

	// converted just fine
	return WAAPI_OK;
}

WAUTIL_RETURN WaJsonFactory::_createArray( const wa_wstring & str, size_t & pos, WaJson & json_array )
{
	// convert to empty array
	json_array.convert( WAAPI_JSON_ARRAY );

	while( pos != wa_wstring::npos )
	{
		_ignoreWhiteSpace( str, pos );

		// check if we hit the end of our array
		if( str[ pos ] == L']' && pos++ )
		{
			return WAAPI_OK;
		}

		// convert JSON value held by array
		WaJson json_value;
		if( WAAPI_FAILED( _create( str, pos, json_value ) ) )
		{
			return WAAPI_ERROR_INVALID_JSON;
		}

		// update array
		json_array.add( json_value );

		_ignoreWhiteSpace( str, pos );

		// check for additional items
		if( str[ pos ] == L',' && pos++ )
		{
			_ignoreWhiteSpace( str, pos );
			if( str[ pos ] == L',' )
			{
				// nope! invalid trailing comma. Invalid JSON format
				return WAAPI_ERROR_INVALID_JSON;
			}
			if( str[pos] == L']' )
			{
				return WAAPI_ERROR_INVALID_JSON;
			}
		}
	}
	// the only way we get here is if we hit the end of the file which cannot happen without ending the array in valid JSON format
	return WAAPI_ERROR_INVALID_JSON;
}

WAUTIL_RETURN WaJsonFactory::_createSpecialCase( const wa_wstring & str, size_t & pos, WaJson & json_special )
{
	// check bounds
	if( pos + 3 > str.length() )
	{
		return WAAPI_ERROR_INVALID_JSON;
	}

	// substring out the space check
	wa_wstring tmp = str.substr( pos, 3 );

	// true case
	if( tmp == L"rue" )
	{
		json_special = true;
		pos += 3;
		return WAAPI_OK;
	}
	else if( tmp == L"ull" )
	{
		json_special.clear(); // make null
		pos += 3;
		return WAAPI_OK;
	}
	else
	{
		// check bounds
		if( pos + 4 > str.length() )
		{
			return WAAPI_ERROR_INVALID_JSON;
		}
		tmp = str.substr( pos , 4 );
		if( tmp == L"alse" )
		{
			json_special = false;
			pos += 4;
			return WAAPI_OK;
		}
	}
	// invalid special case
	return WAAPI_ERROR_INVALID_JSON;
}

WAUTIL_RETURN WaJsonFactory::_createNumber( const wa_wstring & str, size_t & pos, WaJson & json_number )
{
	size_t firstPos = pos;
	bool bCapture = true;

	wa_wstring tmp = L"";
	while( isdigit( str[ pos ] ) 
		|| str[ pos ] == L'-' 
		|| str[ pos ] == L'+' 
		|| str[ pos ] == L'.'
		|| str[ pos ] == L'e'
		|| str[ pos ] == L'E' )
	{
		// we only care about whole digits, negative and positive. We will ignore complex numbers
		if( str[ pos ] == L'.'
			|| str[ pos ] == L'e'
			|| str[ pos ] == L'E'  )
		{
			bCapture = false;
		}
		
		if( bCapture )
		{
			// we only care about whole digits, negative and positive. We will ignore complex numbers
			if( isdigit( str[pos ] ) || ( str[ pos] == L'-' && pos == firstPos ) )
			{
				tmp += str[ pos ];
			}
		}
		pos++;
	}

	// an empty number
	if( tmp.empty() || tmp == L"-" )
	{
		return WAAPI_ERROR_INVALID_JSON;
	}

	// conver to number
	json_number = WaStringUtils::strToIntW( tmp );
	return WAAPI_OK;
}