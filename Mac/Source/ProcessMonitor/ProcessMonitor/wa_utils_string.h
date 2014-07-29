#pragma once
/**
 *	@file wa_utils_string.h
 *	@brief Defines our utility functions for string operations
 *
 *	@defgroup waapi_internal_strings String utilities
 *	@ingroup waapi_internal
 *	@brief Utility code working with strings
 */

#include "wa_utils_types.h"
#include "wa_api_error_codes.h"			// defines our error codes

#include <sstream>
#include <locale>
#include <codecvt>

/**
 *	@brief Defines our class for working with string utility functions
 *	@ingroup waapi_internal_strings
 */
namespace WaStringUtils
{
    /**
	 *	@brief Will convert to wide string
	 *	@param[in] pstr: pointer to char string
	 *	@returns The wide string
	 */
    inline std::wstring _T( char* pstr )
    {
        std::wstring s_result;
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convertor;
        try
        {
            s_result = convertor.from_bytes( pstr );
        }
        catch ( ... )
        {
            size_t n_length = strlen( pstr );
            s_result.resize( n_length + 1 );
            
            size_t i = 0;
            for( ; i < n_length ; i++ )
            {
                s_result[i] = ( wchar_t )pstr[i];
            }
            s_result[i] = '\0';
        }
        return s_result;
    }
    
    /**
	 *	@brief Will match a pattern inside a text
	 *	@param[in] text The string to match
     *	@param[in] start The position to start matching
     *	@param[in] pattern The pattern to match in text
	 *	@returns The first matching postion
	 */
	wa_int  matchKMP( const char* text, int start, const char* pattern );
    
    ///////////////////
    void  separate( const wa_wstring& str, const wa_wstring& sep, wa_wstring_vector& token );
    ////////////////////
    
    
    /**
	 *	@brief Will convert a wide string integer to an integer
	 *	@param[in] str The wide string integer to convert
	 *	@returns The converted integer
	 */
	wa_int  strToIntW( const wa_wstring & str );
    
	/**
	 *	@brief Will convert a string integer to an integer
	 *	@param[in] str The string integer to convert
	 *	@returns The converted integer
	 */
	wa_int  strToInt( const std::string & str );

    /**
	 *	@brief Will convert a integer to a wide string integer
	 *	@param[in] num The integer to convert
	 *	@returns The converted wide string
	 */
	wa_wstring  intToStrW( const wa_int & num );
    
	/**
	 *	@brief Will convert a integer to a string integer
	 *	@param[in] num The integer to convert
	 *	@returns The converted string
	 */
	std::string  intToStr( const wa_int & num );

    /**
	 *	@brief Will convert a wide string to a string
	 *	@param[in] str The wide string to convert
	 *	@return The converted string
	 */
	std::string  wstrToStr( const wa_wstring & str );
    
	/**
	 *	@brief Will convert a string to a wide string
	 *	@param[in] str The string to convert
	 *	@return The converted string
	 */
	wa_wstring  strToWstr( const std::string & str );
    

	/**
	 *	@brief Will replace all occurrences of @c to_replace with @c replace_with on @c str
	 *	@param[in,out] str The string to replace all occurrences on
	 *	@param[in] to_replace The occurrence in the string to replace
	 *	@param[in] replace_with The occurrence to substitute with the @c to_replace
	 */
	void  replaceAll( wa_wstring & str, const wa_wstring & to_replace, const wa_wstring & replace_with );

    /**
     *	@brief Will replace all occurrences of characters in 'strCharSetToReplace' with 'replacement' character
     *	@param[in,out] str The string to replace all occurrences on
     *	@param[in] strCharSetToReplace The charset - any character in the set will be replaced by 'replacement' character.
     *	@param[in] replacement The occurrence to substitute with the 'replacement' character
     */
	void  replaceChars(wa_wstring & str, const wa_wstring& strCharSetToReplace, wa_wchar replacement);
    
    /**
	 *	@brief Will make the input string lowercase
	 *	@param[in] str The string to make lowercase
	 */
	void  toLower( std::string & str );
    
	/**
	 *	@brief Will make the input string lowercase
	 *	@param[in] str The string to make lowercase
	 */
	void  toLower( wa_wstring & str );

	/**
	 *	@brief Will make the input wide string lowercase
	 *	@param[in] str The wide string to make lowercase
	 */
	void  toLower( wa_wstring & str );

	/**
	 *	@brief Will check if the given string is a number
	 *	@param[in] The string to check if it is a number
	 *	@returns true if number, false otherwise
	 */
	bool  isNumber( const wa_wstring & str );

	/**
	 *	@brief Will ensure the path has a file separator at the end of it, if it does not, it will add one
	 *	@param[in] str The path to check for a file separator and add one if it doesn't exist
	 */
	void  ensurePathEnding( wa_wstring & str );

    /**
	 *	@brief Convert bytes to a wide string. Handles ANSI, UTF-8, UTF-16 LE encoded string into a usable string
	 *
	 *	UTF-8 = first 3 bytes 0xEF 0xBB 0xBF
	 *	UTF-16 LE = first 2 bytes 0xFF 0xFE
	 *
	 *	@param[in] bytes Pointer to location in memory where bytes start to convert
	 *	@param[in] dwSize Size of our buffer to convert to wide string
	 *
	 *	@retval the decoded string. If the given string is not encoded, it will be considered ANSI
	 */
	wa_wstring  bytesToWString( unsigned char * bytes, unsigned long dwSize );
    
	/**
	 *	@brief Convert bytes to a hex wide string. Example. the bytes: 0xFF 0xFE will give the string L"FFFE"
	 *
	 *	@param[in] bytes Pointer to location in memory where bytes start to convert
	 *	@param[in] dwSize Size of our buffer to convert to wide string
	 *
	 *	@retval the string representation of the bytes as hexadecimal.
	 */
	wa_wstring  bytesToHexWString( unsigned char * bytes, unsigned long buf_size );
    
	/**
	 *	@brief Convert a hex wide string to bytes. Example. the string L"FFFE" will give the bytes: 0xFF 0xFE
	 *		   **CALLER MUST CALL free() ON THE RETURNED BYTE ARRAY WHEN DONE!!
	 *
	 *	@param[in] str_bytes The wide string representation of the bytes
	 *	@param[in] bytes Pointer to location in memory where bytes will be stored
	 *	@param[in] wchar_format If true, the returned bytes will have null bytes (0x00 0x00) stripped and an ending null will be enforced.
	 *							This is a helper for converting arbitrary data to a string representation.
	 *
	 *	@retval the size of bytes.
	 */
	unsigned long  hexWStringToBytes( wa_wstring str_bytes, unsigned char ** bytes, bool wchar_format );
    
	/**
	 *	@brief Determines if an input string begins with a particular substring.
	 *	@param[in] stringIn The whole string to check against
	 *	@param[in] stringToMatch The substring to match against the beginning of stringIn
	 *	@param[in] ignoreCase if true the comparison will be case insensitive
	 *	@returns true if stringIn begins with stringToMatch
	 */
	bool  beginsWith( const wa_wstring & stringIn, const wa_wstring & stringToMatch, const bool ignoreCase );
    
	/**
	 *	@brief Determines if an input string ends with a particular substring.
	 *	@param[in] stringIn The whole string to check against
	 *	@param[in] stringToMatch The substring to match against the end of stringIn
	 *	@param[in] ignoreCase if true the comparison will be case insensitive
	 *	@returns true if stringIn ends with stringToMatch
	 */
	bool  endsWith( const wa_wstring & stringIn, const wa_wstring & stringToMatch, const bool ignoreCase );
    
	// minPrecision only works for values <= 4
	template <typename numericType>
	std::string file_size_pretty(numericType _sizeInBytes, unsigned int minPrecision = 2)
	{
		static const wchar_t * suffix[] = {L"B", L"KB", L"MB", L"GB", L"TB", L"PB", L"EB", L"ZB", L"YB"};
        
		int pow = 0;
		while (_sizeInBytes > 1024*1024)
		{
			_sizeInBytes /= 1024;
			pow++;
            
			if (pow + 2 >= (sizeof(suffix) / sizeof(suffix[0])))
			{
				break;	// Unlikely to happen for the next couple decades
			}
		}
        
		std::wstring decimalPortion;
		if (_sizeInBytes > 1024)
		{
			if (to_wstr(_sizeInBytes/1024).length() < minPrecision)
			{
				decimalPortion += L".";
				decimalPortion += to_wstr(_sizeInBytes % 1024);
				decimalPortion = decimalPortion.substr(0, minPrecision + 1 - to_wstr(_sizeInBytes/1024).length());
			}
            
			_sizeInBytes /= 1024;
			pow++;
		}
        
		return to_wstr(_sizeInBytes) + decimalPortion + suffix[pow];
	}
    
    //-- Group: from_wstr
    //
    template <typename dstT>
	dstT from_wstr(const std::wstring & _rhs)
	{
		dstT ret;
		std::wstringstream ss(_rhs);
		if(ss >> ret)
			return ret;
        
		return dstT();
	}
    
	template <>
	inline std::wstring from_wstr<std::wstring>(const std::wstring & _rhs)
	{
		return _rhs;
	}
    
	template <>
	inline std::string from_wstr(const std::wstring & _rhs)
	{
		return wstrToStr( _rhs );
	}
    
    
    //-- Group: to_str
    //
	template <typename T>
	std::string to_str(const T & _rhs)
	{
		return std::to_string( _rhs );
	}
    
	// specializations
	// Careful with pointers though - we don't want 'to_str(0)' to enter a function that takes a char *
	std::string  to_str(const std::string & _rhs); // just for optimization
    
    
    //-- Group: to_wstr
    //
	template <typename T>
	std::wstring to_wstr( T & _rhs )
	{
		return std::to_wstring( _rhs );
	}
    
	// specializations
	// Careful with pointers though - we don't want 'to_wstr(0)' to enter a function that takes a wchar_t *
	std::wstring  to_wstr(const std::wstring & _rhs); // just for optimization
	std::wstring  to_wstr(const std::string & _rhs);
	std::wstring  to_wstr(const char * _rhs);
	std::wstring  to_wstr(char * _rhs);
    
	template <typename dstT, typename srcT>
	dstT lex_cast(srcT _rhs)
	{
		return from_wstr<dstT>(to_wstr(_rhs));
	}
}