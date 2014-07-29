#pragma once
/**
 *	@file wa_utils_json_factory.h
 *	@brief Will create @ref WaJson instance from a @JSON formatted string
 */

#include "wa_utils_json.h"

/**
 *	@brief Will create @ref WaJson instances from @JSON formatted strings
 *	@ingroup waapi_internal_json
 */
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

	WaJsonFactory & operator=( const WaJsonFactory &assign_from );

	/**
	 *	@brief Will create a @ref WaJson instance for the given @JSON string
	 *	@param[in] json_in The @JSON formatted string to create our class instance from. If @c NULL is passed a @JSON value of type @ref WAAPI_JSON_NULL is returned
	 *	@param[out] json_value The class instance that represents the JSON string
	 *	@retval WAAPI_OK On successful conversion
	 *	@retval WAAPI_ERROR_INVALID_JSON The @JSON formatted string is not valid
	 */
	WAUTIL_RETURN create( wa_wstring json_in, WaJson & json_value );
	WAUTIL_RETURN create( const wa_wchar * json_in, WaJson & json_value );

private:

	/**
	 *	@brief Will recursively be called to create our instances
	 *	@param str[in] The formatted @JSON string
	 *	@param pos[in,out] The current position in the @JSON string contents in which we are converting the data of
	 *	@param json_value[out] The converted value of the @JSON data relative to the position in the content
	 *	@retval WAAPI_OK On successful conversion
	 *	@retval WAAPI_ERROR_INVALID_JSON The @JSON formatted string is not valid
	 */
	WAUTIL_RETURN _create( const wa_wstring & str, size_t & pos, WaJson & json_value );

	/**
	 *	@brief Will ignore whitespace and other data in which we do not wish to treat as valid @JSON input for converting
	 *	@param[in] str The @JSON formatted string
	 *	@param[in,out] pos The position in the @JSON formatted string we are currently at
	 */
	void _ignoreWhiteSpace( const wa_wstring & str, size_t & pos );

	/**
	 *	@brief Will create a @JSON object
	 */
	WAUTIL_RETURN _createObject( const wa_wstring & str, size_t & pos, WaJson & json_object );

	/**
	 *	@brief Will create a @JSON string
	 */
	WAUTIL_RETURN _createString( const wa_wstring & str, size_t & pos, wa_wstring & json_string );

	/**
	 *	@brief Will decode a @JSON string character into actual value
	 */
	WAUTIL_RETURN _decodeCharacter( const wa_wstring & json_string, wa_wchar & json_decoded_character, size_t & pos );

	/**
	 *	@brief Used for converting @JSON string unicode representations to actual characters
	 */
	WAUTIL_RETURN _convertToHexQuad( const wa_wstring & str, wa_wchar & converted );

	/**
	 *	@brief Will create a @JSON array
	 */
	WAUTIL_RETURN _createArray( const wa_wstring & str, size_t & pos, WaJson & json_array );

	/**
	 *	@brief Will create @JSON special cases: null, true or false values
	 */
	WAUTIL_RETURN _createSpecialCase( const wa_wstring & str, size_t & pos, WaJson & json_special );

	/**
	 *	@brief Will create a @JSON number. Note: Will only handle positive and negative integers. Floats and exponentials are ignored.
	 */
	WAUTIL_RETURN _createNumber( const wa_wstring & str, size_t & pos, WaJson & json_number );
};