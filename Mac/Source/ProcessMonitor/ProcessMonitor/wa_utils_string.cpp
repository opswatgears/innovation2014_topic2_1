#define _WAAPI_EXPORTS_

#include "wa_utils_string.h"
#include <algorithm>
#include <string>

using std::string;
using std::string;
using std::stringstream;
using std::stringstream;

namespace WaStringUtils
{

    wa_int  matchKMP( const char* text, int start, const char* pattern )
    {
        int *T;
        int i, j;
        int result = -1;
        
        if (pattern[0] == '\0')
            return result;
        
        /* Construct the lookup table */
        T = new int[strlen( pattern ) + 1];
        T[0] = -1;
        for (i=0; pattern[i] != '\0'; i++)
        {
            T[i+1] = T[i] + 1;
            while (T[i+1] > 0 && pattern[i] != pattern[T[i+1]-1])
                T[i+1] = T[T[i+1]-1] + 1;
        }
        
        /* Perform the search */
        for (i = start, j=0; text[i] != '\0'; )
        {
            if (j < 0 || text[i] == pattern[j])
            {
                ++i, ++j;
                if (pattern[j] == '\0')
                {
                    result = i - j;
                    break;
                }
            }
            else j = T[j];
        }
        
        delete[] T;
        
        return result;
    }
    
    void  separate( const wa_wstring& str, const wa_wstring& sep, wa_wstring_vector& token )
    {
        const char* p_sep =  WaStringUtils::wstrToStr( sep ).c_str();
        char* p_token = strtok( ( char* )WaStringUtils::wstrToStr( str ).c_str(), p_sep );
        
        while ( p_token != NULL )
        {
            token.push_back( WaStringUtils::_T( p_token ) );
            p_token = strtok( NULL, p_sep );
        }
    }
    
    wa_int  strToIntW( const wa_wstring & str )
    {
        std::string s_temp = WaStringUtils::wstrToStr( str );
        return std::atoi( s_temp.c_str() );
    }
    
    wa_int  strToInt( const std::string & str )
    {
        return ( wa_int )std::atol( str.c_str() );
    }

    wa_wstring  intToStrW( const wa_int & num )
    {
        return std::to_wstring( num );
    }
    
    std::string  intToStr( const wa_int & num )
    {
        return std::to_string( num );
    }
    
    std::string  wstrToStr( const wa_wstring & str )
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convertor;
        return convertor.to_bytes( str );
    }

    wa_wstring  strToWstr( const std::string & str )
    {
        std::wstring s_result;
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convertor;
        try
        {
            s_result = convertor.from_bytes( str );
        }
        catch ( ... )
        {
            size_t n_length = str.size();
            const char* _p_temp = str.c_str();
            s_result.resize( n_length + 1 );
            
            size_t i = 0;
            for( ; i < n_length ; i++ )
            {
                s_result[i] = ( wchar_t )_p_temp[i];
            }
            s_result[i] = '\0';
        }
        return s_result;
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
            for ( wa_wchar *p = ( wa_wchar* )str.c_str(); ( ( p = wcspbrk( p, strCharSet.c_str() ) ) != NULL ) ; ++p )
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
    
    bool  isNumber( const wa_wstring & str )
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
            if( str[ str.length() - 1 ] != L'/' )
                str += L'/';
        }
    }

    wa_wstring  bytesToWString( unsigned char * bytes, unsigned long dwSize )
    {
        if( !bytes )
        {
            return L"";
        }
        
        /*if( dwSize >= 2 )
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
        }*/
        
        // ANSI
        for ( unsigned long i = 0 ; i < dwSize ; i++ )
        {
            if ( ( ( char )bytes[i] ) < 32 )
                bytes[i] = 32;  // white space;
        }
        
        std::string tmp( reinterpret_cast< const char *>(bytes), dwSize );
        return strToWstr( tmp );
    }
    
    std::string  to_str(const std::string & _rhs)
    {
        return _rhs;
    }
    
    wa_wstring  bytesToHexWString( unsigned char * bytes, unsigned long buf_size )
    {
        if( !bytes || !buf_size )
        {
            return L"";
        }
        
        char hexStr[]= "0123456789abcdef";
        char* p_hex = new char [buf_size * 2 + 1];
        char* p_temp = p_hex;
        
        for( unsigned int i = 0 ; i < buf_size; ++i, p_temp += 2 )
        {
            p_temp[0] = hexStr[ bytes[i] >> 4  ];
            p_temp[1] = hexStr[ bytes[i] & 0x0F ];
        }
        p_temp[0] = '\0';
        
        wa_wstring str = WaStringUtils::to_wstr( p_hex );
        delete[] p_hex;
        
        return str;
    }
    
    unsigned long  hexWStringToBytes( wa_wstring str_bytes, unsigned char ** bytes, bool wchar_format )
    {
        size_t len = str_bytes.size();
        // since the bytes are hex we must have exactly 2 chars representing each byte value
        if( str_bytes.empty() || ( len % 2 ) )
        {
            *bytes = NULL;
            return 0;
        }
        
        *bytes = new unsigned char[ ( len / 2 ) + 1 ]; // add 1 just in case we need it for wchar_format
        
        // get the char array
        const wa_wchar* str = str_bytes.c_str();
        
        size_t pos = 0;
        for( size_t i = 0; i < len; i += 2 )
        {
            // take each 2 chars and interpret as base 16 (hex)
            wa_wchar t[] = { str[i], str[i+1], L'\0' };
            
            // skip nulls for wchar formatting
            if( wchar_format && t[0] == L'0' && t[1] == L'0' )
                continue;
            
            (*bytes)[pos++] = ( unsigned char )wcstol( t, NULL, 16 );
        }
        
        if( wchar_format )
            (*bytes)[pos] = L'\0';
        
        return pos;
    }
    
    bool  beginsWith( const wa_wstring & stringIn, const wa_wstring & stringToMatch, const bool ignoreCase )
    {
        wa_wstring analyze = stringIn, substring = stringToMatch;
        if( ignoreCase == true )
        {
            toLower( analyze );
            toLower( substring );
        }
        
        if( analyze.find( substring.c_str(), 0, substring.length() ) == 0 )
            return true;
        
        return false;
    }
    
    bool  endsWith( const wa_wstring & stringIn, const wa_wstring & stringToMatch, const bool ignoreCase )
    {
        wa_wstring analyze = stringIn, substring = stringToMatch;
        if(ignoreCase == true)
        {
            toLower(analyze);
            toLower(substring);
        }
        
        if( analyze.find( substring.c_str(), stringIn.length()-substring.length() ) != wa_wstring::npos )
            return true;
        
        return false;
    }
    
    std::wstring  to_wstr(const std::wstring & _rhs)
    {
        return  _rhs;
    }
    
	std::wstring  to_wstr(const std::string & _rhs)
    {
        return WaStringUtils::strToWstr( _rhs );
    }
    
	std::wstring  to_wstr(const char * _rhs)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convertor;
        return convertor.from_bytes( _rhs );
    }
    
	std::wstring  to_wstr(char * _rhs)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convertor;
        return convertor.from_bytes( _rhs );
    }
} // end namespace WaStringUtils