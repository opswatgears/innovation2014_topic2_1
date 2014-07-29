#include "wa_utils_string.h"
#include "wa_utils_unicode.h"	

#include <algorithm>

using std::string;
using std::wstring;
using std::stringstream;
using std::wstringstream;

namespace WaStringUtils
{

wa_int  strToIntW( const wa_wstring & str )
{
	return _wtol( str.c_str() );
}

wa_int  strToInt( const std::string & str )
{
	return atol( str.c_str() );
}

wa_wstring  intToStrW( const wa_int & num )
{
	wa_wchar buff[12];
	_ltow(num, buff, 10);
	return (wa_wstring)buff;
}

std::string  intToStr( const wa_int & num )
{
	char buff[12];
	_ltoa( num, buff, 10 );
	return (std::string)buff;
}

std::string  wstrToStr( const wa_wstring & str )
{
	// conversion from Unicode project, see wa_utils_unicode.h
	size_t widesize = str.length();
	size_t utf8size = 3 * widesize + 1;
	char* utf8stringnative = new char[utf8size];
	const UTF16* sourcestart = reinterpret_cast<const UTF16*>(str.c_str());
	const UTF16* sourceend = sourcestart + widesize;
	UTF8* targetstart = reinterpret_cast<UTF8*>(utf8stringnative);
	UTF8* targetend = targetstart + utf8size;
	/*ConversionResult res = */ ConvertUTF16toUTF8(&sourcestart, sourceend, &targetstart, targetend, strictConversion);
	
	*targetstart = 0;
	std::string ret_str(utf8stringnative);
	delete [] utf8stringnative;
	return ret_str;
}

wa_wstring  strToWstr( const std::string & str )
{
	// conversion from Unicode project, see wa_utils_unicode.h
	size_t widesize = str.length();
	wa_wchar* widestringnative = new wa_wchar[widesize+1];
	const UTF8* sourcestart = reinterpret_cast<const UTF8*>(str.c_str());
	const UTF8* sourceend = sourcestart + widesize;
	UTF16* targetstart = reinterpret_cast<UTF16*>(widestringnative);
	UTF16* targetend = targetstart + widesize+1;
	ConversionResult res = ConvertUTF8toUTF16(&sourcestart, sourceend, &targetstart, targetend, strictConversion);
	if(res == sourceIllegal)
	{
		delete [] widestringnative;
		return mbcsToWstr(str);
	}
	*targetstart = 0;
	wa_wstring ret_str(widestringnative);
	delete [] widestringnative;
	return ret_str;
}

wa_wstring  mbcsToWstr(const std::string & str )
{
	wa_wchar * wideChar; //Var to hold UTF-16 chars
	int t = (int)str.size()+1; //Var to hold size needed for "final"
	wa_wstring ret_str;

	//Initialize wide char place holder
	wideChar = new wa_wchar[t];
	memset(wideChar, 0, sizeof(wchar_t)*t);

	//Convert MBCS to UTF-16 using system default code page
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str.c_str(), -1, wideChar, t);

	//Return result
	ret_str = wideChar;
	delete [] wideChar;

	return ret_str;
}

void  replaceAll( wa_wstring & str, const wa_wstring & to_replace, const wa_wstring & replace_with )
{
	// if to replace is empty, this infinite loops, so clearly disallow it
	if( !to_replace.empty() )
	{
		wa_wstring::size_type pos;
		pos = str.find( to_replace );
		while( pos != wa_wstring::npos )
		{
			str.replace( pos, to_replace.length(), replace_with );
			pos = str.find( to_replace, pos + replace_with.length() );
		}
	}
}

void  replaceChars(wa_wstring & str, const wa_wstring& strCharSet, wa_wchar replacement)
{
	//assert(!str.empty() && !strCharSet.empty());
	if ( !str.empty() && !strCharSet.empty())
	{
		for ( wa_wchar *p = (wa_wchar*)str.c_str(); p = wcspbrk(p,strCharSet.c_str()); ++p)
		{
			*p = replacement;
		}
	}
}

void  toLower( std::string & str )
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
}
void  toLower( wa_wstring & str )
{
	std::transform(str.begin(), str.end(), str.begin(), tolower);
}

bool  isNumber( const std::string & str )
{
	// validate if its empty
	if( str.empty() )
	{
		return false;
	}

	// check for positive and negative numbers

	for( size_t i = 0; i < str.length(); ++i )
	{
		if( str[i] == '-' )	// only allow - as the first character
		{
			if( i != 0 )
			{
				return false;
			}
		}
		else if( !isdigit( str[i] ) )	// check for digits for the rest
		{
			return false;
		}
	}
	return true;
}

void  ensurePathEnding( wa_wstring & str )
{
	if( !str.empty() )
	{
#ifdef _WAAPI_WINDOWS_
		if( str[ str.length() - 1 ] != L'\\' )
			str += L'\\';
#endif
	}
}

wa_wstring  bytesToWString( BYTE * bytes, DWORD dwSize )
{
	if( !bytes )
	{
		return L"";
	}

	if( dwSize >= 2 )
	{
		// UTF-16 LE
		if( ((char)(*bytes)) == '\xFF' && ((char)*( bytes + 1 )) == '\xFE' )
		{
			wa_wstring tmp( reinterpret_cast<const wa_wchar*>(bytes + 2) , dwSize - 2 );
			size_t pos = tmp.find( L'\x0000' );
			if( pos != wa_wstring::npos )
			{
				tmp = tmp.substr(0, pos );
			}
			return tmp;
		}
	}

	if( dwSize >= 3 )
	{
		// UTF-8
		if( ((char)(*bytes)) == '\xEF'  && ((char)*( bytes + 1)) == '\xBB' &&  ((char)*( bytes + 2)) == '\xBF' )
		{
			std::string tmp( reinterpret_cast< const char *>( bytes + 3), dwSize - 3 );
			return strToWstr( tmp );
		}
	}

	// ANSI
	std::string tmp( reinterpret_cast< const char *>(bytes), dwSize );
	return strToWstr( tmp );
}

wa_wstring  bytesToHexWString( BYTE * bytes, DWORD buf_size )
{
	if( !bytes || !buf_size )
	{
		return L"";
	}

	char hexStr[]= "0123456789abcdef";
	char* temp;

	temp = (char *)malloc( buf_size * 2 + 1 );
	temp[buf_size * 2] = 0;

	for( unsigned int i = 0; i < buf_size; ++i )
	{
		temp[ i * 2 + 0 ] = hexStr[ bytes[i] >> 4  ];
		temp[ i * 2 + 1 ] = hexStr[ bytes[i] & 0x0F ];
	}

	wa_wstring str = to_wstr( temp );
	free( temp );

	return str;
}

DWORD  hexWStringToBytes( wa_wstring str_bytes, BYTE ** bytes, bool wchar_format )
{
	DWORD len = str_bytes.size();
	// since the bytes are hex we must have exactly 2 chars representing each byte value
	if( str_bytes.empty() || (len % 2) )
	{
		*bytes = NULL;
		return 0;
	}
	
	*bytes = (BYTE *)malloc( (len / 2) + 1 ); // add 1 just in case we need it for wchar_format

	// get the char array
	const wa_wchar* str = str_bytes.c_str();

	DWORD pos = 0;
	for( DWORD i = 0; i < len; i+=2 )
	{
		// take each 2 chars and interpret as base 16 (hex)
		wa_wchar t[] = { str[i], str[i+1], L'\0' };

		// skip nulls for wchar formatting
		if( wchar_format && t[0] == L'0' && t[1] == L'0' )
			continue;

		(*bytes)[pos++] = (BYTE)wcstol( t, NULL, 16 );
	}

	if( wchar_format )
		(*bytes)[pos] = L'\0';

	return pos;
}

wstring  to_wstr(const string & _rhs, UINT _codePage)
{
	unsigned int iSizeOfStr = MultiByteToWideChar(_codePage, 0, _rhs.c_str(), (unsigned int)_rhs.size(), NULL, 0);  
	wchar_t* wszTgt = new wchar_t[iSizeOfStr];  
	if(!wszTgt)   
		return L"";

	MultiByteToWideChar(_codePage, 0, _rhs.c_str(), (unsigned int)_rhs.size(), wszTgt, iSizeOfStr);  
	wstring sUTF16(wszTgt, iSizeOfStr);
	delete [] wszTgt;  
	return sUTF16;
}

wstring  to_wstr(const string & _rhs)
{
	return to_wstr(_rhs, CP_UTF8);
}

wstring  to_wstr(const char * _rhs)
{
	return to_wstr(_rhs, CP_UTF8);
}

wstring  to_wstr(char * _rhs)
{
	return to_wstr(_rhs, CP_UTF8);
}

// Some common variant types
wstring  to_wstr(VARIANT _rhs)
{
	switch (_rhs.vt)
	{
	case VT_BSTR:
		return to_wstr(_rhs.bstrVal);
	case VT_UI1:
		return to_wstr(_rhs.bVal);
	case VT_UI2:
		return to_wstr(_rhs.uiVal);
	case VT_UI4:
		return to_wstr(_rhs.ulVal);
	case VT_I1:
		return to_wstr(_rhs.cVal);
	case VT_I2:
		return to_wstr(_rhs.iVal);
	case VT_I4:
		return to_wstr(_rhs.lVal);
	case VT_DATE:
		return to_wstr(_rhs.date);
	case VT_R4:
		return to_wstr(_rhs.fltVal);
	case VT_R8:
		return to_wstr(_rhs.dblVal);
	default:
		return L"";
	}
}

//wstring  to_wstr(const _variant_t & _rhs)
//{	
//	return to_wstr((VARIANT)_rhs);
//}

wstring  to_wstr(const wstring & _rhs)
{
	return _rhs;
}

string  from_wstr(const wstring & _rhs, UINT _codePage)
{
	int iSizeOfStr = WideCharToMultiByte(_codePage, 0, _rhs.c_str(), (unsigned int)_rhs.size(), NULL, 0, NULL, NULL);  
	char* szTgt = new char[iSizeOfStr];
	if(!szTgt)  
		return "";

	WideCharToMultiByte(_codePage, 0, _rhs.c_str(), (unsigned int)_rhs.size(), szTgt, iSizeOfStr, NULL, NULL);  
	string ret(szTgt, iSizeOfStr);
	delete [] szTgt;  

	return ret;
}

bool  beginsWith( const wa_wstring & stringIn, const wa_wstring & stringToMatch, const bool ignoreCase )
{
	wa_wstring analyze = stringIn, substring = stringToMatch;
	if( ignoreCase == TRUE )
	{
		toLower( analyze );
		toLower( substring );
	}

	if( analyze.find( substring.c_str(), 0, substring.length() ) == 0 )
		return TRUE;

	return FALSE;
}

bool  endsWith( const wa_wstring & stringIn, const wa_wstring & stringToMatch, const bool ignoreCase )
{
	wa_wstring analyze = stringIn, substring = stringToMatch;
	if(ignoreCase == TRUE)
	{
		toLower(analyze);
		toLower(substring);
	}

	if( analyze.find( substring.c_str(), stringIn.length()-substring.length() ) != wa_wstring::npos )
		return TRUE;

	return FALSE;
}


} // end namespace WaStringUtils