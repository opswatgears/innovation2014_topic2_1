#pragma once
/**
 *	@file wa_utils_string.h
 *	@brief Defines our utility functions for string operations
 *
 *	@defgroup waapi_internal_strings String utilities
 *	@ingroup waapi_internal
 *	@brief Utility code working with strings
 */

#include "wa_api_data_types.h"		// defines our data types
#include "wa_api_error_codes.h"			// defines our error codes
#include "wa_utils_types.h"	// defines our internal data types not exposed to the user

#include <sstream>
#include <assert.h>


/**
 *	@brief Defines our class for working with string utility functions
 *	@ingroup waapi_internal_strings
 */
namespace WaStringUtils
{
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
	 *	@brief Will convert a multi-byte string to a wide string
	 *	@param[in] str The multi-byte string to convert
	 *	@return The converted multi-byte string
	 */
	wa_wstring  mbcsToWstr(const std::string & str );

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
	 *	@brief Will make the input wide string lowercase
	 *	@param[in] str The wide string to make lowercase
	 */
	void  toLower( wa_wstring & str );

	/**
	 *	@brief Will check if the given string is a number
	 *	@param[in] The string to check if it is a number
	 *	@returns true if number, false otherwise
	 */
	bool  isNumber( const std::string & str );

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
	wa_wstring  bytesToWString( BYTE * bytes, DWORD dwSize );

	/**
	 *	@brief Convert bytes to a hex wide string. Example. the bytes: 0xFF 0xFE will give the string L"FFFE"
	 *
	 *	@param[in] bytes Pointer to location in memory where bytes start to convert
	 *	@param[in] dwSize Size of our buffer to convert to wide string
	 *
	 *	@retval the string representation of the bytes as hexadecimal.
	 */
	wa_wstring  bytesToHexWString( BYTE * bytes, DWORD buf_size );

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
	DWORD  hexWStringToBytes( wa_wstring str_bytes, BYTE ** bytes, bool wchar_format );

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
	std::wstring file_size_pretty(numericType _sizeInBytes, UINT minPrecision = 2)
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
	inline VARIANT from_wstr<VARIANT>(const std::wstring & _rhs)
	{
		VARIANT ret;
		ret.bstrVal = ::SysAllocString(_rhs.c_str());
		ret.vt = VT_BSTR;
		return ret;
	}

	/*template <>
	inline _variant_t from_wstr<_variant_t>(const std::wstring & _rhs)
	{
		_variant_t ret = from_wstr<VARIANT>(_rhs);
		return ret;
	}*/

	template <>
	inline std::wstring from_wstr<std::wstring>(const std::wstring & _rhs)
	{
		return _rhs;
	}

	std::string  from_wstr(const std::wstring & _rhs, UINT _codePage);

	template <>
	inline std::string from_wstr(const std::wstring & _rhs)
	{
		return from_wstr(_rhs, CP_UTF8);
	}

	template <typename T>
	std::string to_str(const T & _rhs)
	{
		return from_wstr<std::string>(to_wstr(_rhs));
	}

	// specializations
	// Careful with pointers though - we don't want 'to_str(0)' to enter a function that takes a char *
	std::string  to_str(const std::string & _rhs); // just for optimization

	template <typename T>
	std::wstring to_wstr(const T & _rhs)
	{
		std::wstringstream ss;
		ss << _rhs;
		return ss.str();
	}

	// specializations
	// Careful with pointers though - we don't want 'to_wstr(0)' to enter a function that takes a wchar_t *
	std::wstring  to_wstr(const std::wstring & _rhs); // just for optimization
	std::wstring  to_wstr(const std::string & _rhs, UINT _codePage);
	std::wstring  to_wstr(const std::string & _rhs);
	std::wstring  to_wstr(const char * _rhs);
	std::wstring  to_wstr(char * _rhs);

	std::wstring  to_wstr(VARIANT _rhs);
	//std::wstring to_wstr(const _variant_t & _rhs);

	template <typename dstT, typename srcT>
	dstT lex_cast(srcT _rhs)
	{
		return from_wstr<dstT>(to_wstr(_rhs));
	}

	/**
	 *	@brief Overwrite string contents in memory
	 *	@param[in/out] str string to be overwritten
	 *	@param[in] size string size (in bytes)
	 */
	template <class WA_CHAR> void zap_str(WA_CHAR *str, DWORD size)
	{
		if ( str && size)
		{
			SecureZeroMemory(str,size * sizeof(WA_CHAR));
		}
	}

	/**
	 *	@brief Overwrite string contents in memory
	 *	@param[in/out] str string to be overwritten
	 */
	template <class WA_CHAR> void zap_str(std::basic_string<WA_CHAR>& str)
	{
		if ( !str.empty() )
		{
			SecureZeroMemory((PVOID)str.c_str(), str.size() * sizeof(WA_CHAR));
		}
		str.clear();
	}

	/**
	 *	@brief Overwrite  contents in memory for all strings in std list.
	 *	@param[in/out] strings list of strings to be overwritten
	 */
	template <class WA_CHAR> void zap_str_list(std::list<std::basic_string<WA_CHAR>>& strings)
	{
		for ( auto iter = strings.begin(); iter != strings.end(); iter++ )
		{
			zap_str(*iter);
		}
		strings.clear();
	}

	/**
	 *	@brief Hide string contents in memory
	 *	@param[in/out] str string who's contents to be hidden
	 */
	template <class WA_CHAR> void obfuscate_str(std::basic_string<WA_CHAR>& str)
	{
		for ( auto iter = str.begin(); iter != str.end(); iter++ )
		{
			if ( sizeof(WA_CHAR) == 1 )
			{
				unsigned char& uc = (unsigned char&)(*iter);
				uc ^= 0x21;
			}
			else if ( sizeof(WA_CHAR) == 2 )
			{
				unsigned short& uc = (unsigned short&)(*iter);
				uc ^= 0x41;
			}
			else
			{
				assert(false);
			}
		}
	}

	/**
	 *	@brief Un-hide string contents in memory
	 *	@param[in/out] str string who's contents to be un-hidden
	 */
	template <class WA_CHAR> void unobfuscate_str(std::basic_string<WA_CHAR>& str)
	{
		obfuscate_str(str);
	}
}