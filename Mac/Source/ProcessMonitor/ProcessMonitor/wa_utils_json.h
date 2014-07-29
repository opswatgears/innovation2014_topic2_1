#pragma once
/**
 *	@file wa_utils_json.h
 *	@brief Defines the class that represents a @JSON value
 *	@defgroup waapi_internal_json JSON types, classes, and utilities
 *	@ingroup waapi_internal
 *	@brief Represents all of our internal structures for handling @JSON content
 */


#include <unordered_map>
#include <set>
#include "wa_utils_types.h"


// NOTE: You cannot use the wa_allocator with memory debugging on. See wa_utils_debug.h
//#if defined( DEBUG_MEMORY )
//	#define WAAPI_ALLOCATAOR std::allocator
//#else
	#define WAAPI_ALLOCATAOR wa_allocator
//#endif
/**
 *	@brief Defines the enum type for @JSON values
 *	@ingroup waapi_internal_json
 */
enum WaJsonType
{
	/** @brief Represents a @JSON string value */
	WAAPI_JSON_STRING,		
	/** @brief Represents a @JSON number value */
	WAAPI_JSON_NUMBER,		
	/** @brief Represents a @JSON object value */
	WAAPI_JSON_OBJECT,		
	/** @brief Represents a @JSON array value */
	WAAPI_JSON_ARRAY,		
	/** @brief Represents a @JSON boolean value */
	WAAPI_JSON_BOOL,		
	/** @brief Represents a @JSON null value */
	WAAPI_JSON_NULL		
};

// declare for typedefs
class WaJson;

/**
 *	@typedef std::vector< WaJson > wa_json_array
 *	@brief Represents a @JSON array, which we internally use as a vector
 *	@ingroup waapi_internal_json
 */
typedef std::vector< WaJson, std::allocator< WaJson > > wa_json_array;

/**
 *	@typedef std::set< wa_wstring > wa_json_object_keys
 *	@brief Represents the keys in an @JSON object, which we internally use as a set of strings
 *	@ingroup waapi_internal_json
 */
typedef std::set< wa_wstring > wa_json_object_keys;
struct wa_map_hasher {

	size_t operator()(const wa_wstring& s) const
	{	
		size_t ret = 0;
		size_t pos = 0;
		while( pos < s.length() /* && pos < 8 */)
		{
			ret = ret * 101 + s[pos++];
		}
		return ret;
	}
};

struct wa_map_equal_to {
	bool operator()(const wa_wstring&  s1, const wa_wstring&  s2) const
	{
		return ( s1.compare( s2 ) == 0 );
	}
};

/**
 *	@typedef std::hash_map< wa_wstring, WaJson > wa_json_object
 *	@brief Represents a @JSON object, which we internally use as a hash_map
 *	@ingroup waapi_internal_json
 */
typedef std::unordered_map< const wa_wstring, WaJson, wa_map_hasher, wa_map_equal_to, std::allocator< std::pair< const wa_wstring, WaJson> > > wa_json_object;


/**
 *	@brief Represents a @JSON value
 *	@ingroup waapi_internal_json
 */
class WaJson
{

public:

	/**
	 *	@brief Constructor. Creates a new @JSON value and is set to null
	 */
	WaJson();

	/**
	 *	@brief Constructor. Creates a new @JSON value with the given @JSON instance
	 *	@param[in] json The instance to create a new instance from
	 */
	WaJson( const WaJson & json );

	/**
	 *	@brief Constructor. Creates a new @JSON value of type @ref WAAPI_JSON_STRING with the given character pointer
	 *	@param[in] json_string The string to create a new instance of type @ref WAAPI_JSON_STRING
	 */
	WaJson( const  wa_wchar * json_string );

	/**
	 *	@brief Constructor. Creates a new @JSON value of type @ref WAAPI_JSON_NUMBER with the given integer value
	 *	@param[in] json_number The integer to create a new instance of type @ref WAAPI_JSON_NUMBER
	 */
	WaJson( const wa_int & json_number );

	/**
	 *	@brief Constructor. Creates a new @JSON value of type @ref WAAPI_JSON_BOOL with the given boolean value
	 *	@param[in] json_boolean The boolean to create a new instance of type @ref WAAPI_JSON_BOOL
	 */
	WaJson( bool json_boolean );

	/**
	 *	@brief Constructor. Creates a new @JSON value of type @ref WAAPI_JSON_ARRAY with the given vector value
	 *	@param[in] json_array The vector to create a new instance of type @ref WAAPI_JSON_ARRAY
	 */
	WaJson( const wa_json_array & json_array );

	/**
	 *	@brief Constructor. Creates a new @JSON value of type @ref WAAPI_JSON_OBJECT with the given hash map value
	 *	@param[in] json_object The hash map to create a new instance of type @ref WAAPI_JSON_OBJECT
	 */
	WaJson( const wa_json_object & json_object );

	/**
	 *	@brief Constructor. Creates a new @JSON value of type @ref WAAPI_JSON_OBJECT with the key-value pair
	 *	@param[in] key String key of the single element in this object
	 *	@param[in] value WaJson value of the single element in this object
	 */
	WaJson( const wa_wstring & key, const WaJson & value);
	
	/**
	 *	@brief will create a @JSON instance for the given type. This can be used to create an empty array or object if needed.
	 *
	 *	@param[in] json_type The type to create an empty instance of this instance. This will create empty objects based on the type passed in:
	 *  <br/><br/>@ref WAAPI_JSON_STRING creates an empty string
	 *	<br/>@ref WAAPI_JSON_NUMBER creates a integer of value 0
	 *	<br/>@ref WAAPI_JSON_OBJECT creates a empty hash map
	 *	<br/>@ref WAAPI_JSON_ARRAY creates a empty vector
	 *	<br/>@ref WAAPI_JSON_BOOL creates a boolean of value false
	 *	<br/>@ref WAAPI_JSON_NULL creates a null value
	 */
	WaJson( WaJsonType json_type );

	/**
	 *	@brief Destructor. Destroys a @JSON value
	 */
	virtual ~WaJson();

	/**
	 *	@brief Overloaded addition operator to allow combinations of 2 json instances of the same type. If a different type is given
	 *	the object is not changed. Has no affect of type WAAPI_JSON_BOOL and WAAPI_JSON_NULL
	 */
	virtual WaJson & operator+=( const WaJson &add_from );

	/**
	 *	@brief Overloaded assignment operator to set the values of this instance with another @JSON instance
	 *	@param[in] assign_from The JSON instance to assign the values from
	 */
	virtual WaJson & operator=( const WaJson &assign_from );

	/**
	 *	@brief Overloaded assignment operator to set the value of this instance with the given character pointer
	 *	@param[in] assign_from The char pointer to set this instance of as a @ref WAAPI_JSON_STRING
	 */
	virtual WaJson & operator=( const  wa_wchar * assign_from );

	/**
	 *	@brief Overloaded assignment operator to set the value of this instance with the given integer
	 *	@param[in] assign_from The integer to set this instance of as a @ref WAAPI_JSON_NUMBER
	 */
	virtual WaJson & operator=( const wa_int &assign_from );

	/**
	 *	@brief Overloaded assignment operator to set the value of this instance with the given boolean
	 *	@param[in] assign_from The bool to set this instance of as a @ref WAAPI_JSON_BOOL
	 */
	virtual WaJson & operator=( bool assign_from );

	/**
	 *	@brief Overloaded assignment operator to set the value of this instance with the given vector
	 *	@param[in] assign_from The vector to set this instance of as a @ref WAAPI_JSON_ARRAY
	 */
	virtual WaJson & operator=( const wa_json_array &assign_from );

	/**
	 *	@brief Overloaded assignment operator to set the value of this instance with the given map
	 *	@param[in] assign_from The hash map to set this instance of as a @ref WAAPI_JSON_OBJECT
	 */
	virtual WaJson & operator=( const wa_json_object &assign_from );

	/**
	 *	@brief Overload [] operator for accessing items in the instance when it is of type @ref WAAPI_JSON_ARRAY
	 *	@details If the index is out-of-bounds, or the instance is not of type @ref WAAPI_JSON_ARRAY then this will throw an assert() error
	 *	@param[in] index The position in the vector to retrieve the item
	 *	@return The @JSON instance at the given index, or an assert() error if it does not exist
	 */
	virtual WaJson & operator[]( size_t index );

	/**
	 *	@brief Overload const [] operator for accessing items in the instance when it is of type @ref WAAPI_JSON_ARRAY
	 *	@details If the index is out-of-bounds, or the instance is not of type @ref WAAPI_JSON_ARRAY then this will throw an assert() error
	 *	@param[in] index The position in the vector to retrieve the item
	 *	@return The const @JSON instance at the given index, or an assert() error if it does not exist
	 */
	virtual const WaJson & operator[]( size_t index ) const;

	/**
	 *	@brief Overload [] operator for accessing items in the instance when it is of type @ref WAAPI_JSON_OBJECT
	 *	@details If the key is not found, or the instance is not of type @ref WAAPI_JSON_OBJECT then this will throw an assert() error
	 *	@param[in] key The key lookup for the @JSON instance in the object
	 *	@returns The @JSON instance for the given key, or an assert() error if it does not exist
	 */
	virtual WaJson & operator[]( const  wa_wchar * key );

	/**
	 *	@brief Overload const [] operator for accessing items in the instance when it is of type @ref WAAPI_JSON_OBJECT
	 *	@details If the key is not found, or the instance is not of type @ref WAAPI_JSON_OBJECT then this will throw an assert() error
	 *	@param[in] key The key lookup for the @JSON instance in the object
	 *	@returns The const @JSON instance for the given key, or an assert() error if it does not exist
	 */
	virtual const WaJson & operator[]( const  wa_wchar * key ) const;

	/**
	 *	@brief Wrapper for keys of type string for operator[]( const  wa_char * key )
	 *	@see operator[]( const  wa_char * key )
	 */
	virtual WaJson & operator[]( const wa_wstring & key );
	/**
	 *	@brief Const wrapper for keys of type string for operator[]( const  wa_char * key ) const
	 *	@see operator[]( const  wa_char * key ) const
	 */
	virtual const WaJson & operator[]( const wa_wstring & key ) const;

	/**
	 *	@brief Compares 2 WaJson objects.  returns false if not the same type, or values differ
	 */
	virtual bool operator==( const WaJson & _rhs ) const;

	/**
	 *	@brief Compares 2 WaJson objects.  returns true if not the same type, or values differ
	 */
	virtual bool operator!=( const WaJson & _rhs ) const;

	virtual bool operator<( const WaJson & _rhs ) const;



	/**
	 *	@brief Will convert the @JSON value to the given type. This will internally call @ref WaJson::clear()
	 *	@details
	 *	This is the same as calling the constructor with the @JSON type. We will create default and empty internal structures
	 *	based on the type passed in.
	 *
	 *	@param[in] type The type to convert this instance to:
	 *	<br/><br/>@ref WAAPI_JSON_STRING converts to an empty string
	 *	<br/>@ref WAAPI_JSON_NUMBER converts to  a integer of value 0
	 *	<br/>@ref WAAPI_JSON_OBJECT converts to  a empty hash map
	 *	<br/>@ref WAAPI_JSON_ARRAY converts to  a empty vector
	 *	<br/>@ref WAAPI_JSON_BOOL converts to  a boolean of value false
	 *	<br/>@ref WAAPI_JSON_NULL converts to  a null value
	 */
	virtual void convert( const WaJsonType & json_type );

	/**
	 *	@brief Retrieve the type of @JSON value for this instance
	 *	@returns The @JSON type of this instance
	 */
	virtual WaJsonType getType() const;

	/**
	 *	@brief Will check if the @JSON value is of type. This is just another way to perform if( this.getType() == type )
	 *	@param[in] type The @JSON type to check if this instance is of
	 *	@returns true if the instance is of the provided type
	 */
	virtual bool isType( const WaJsonType & json_type ) const;

	/**
	 *	@brief Will validate the @JSON value type for the given key and return true if the key-value is of the given type
	 *	@param[in] key The key to check its value type
	 *	@param[in] json_type The type to check the value against
	 *	@returns true if the key-value matches the given type, false for any other reason
	 */
	virtual bool isObjKeyType( const  wa_wchar * key, const WaJsonType & json_type ) const;

	/**
	 *	@brief Will clear the internal structure and make this @JSON value @ref WAAPI_JSON_NULL "nu"
	 *	@details This will handle all cleanup of allocated memory held by this @JSON value
	 */
	virtual void clear();

	/** 
	 *	@brief Will retrieve the size of the @JSON value for type @ref WAAPI_JSON_ARRAY or @ref WAAPI_JSON_OBJECT
	 *	@returns The size of the @JSON value of type @ref WAAPI_JSON_ARRAY or @ref WAAPI_JSON_OBJECT. If called on any other type size returned will be 0
	 */
	virtual size_t size() const;

	/**
	 *	@brief Will add a @JSON value of type @ref WAAPI_JSON_ARRAY.
	 *	@details If the @JSON instance is not of type @ref WAAPI_JSON_ARRAY, then the internal structure will be 
	 *	cleared and created into a @ref WAAPI_JSON_ARRAY instance
	 *
	 *	@param[in] json_value A @JSON value to add to the array.
	 */
	virtual void add( const WaJson & json_value );

	/**
	 *	@brief Will retrieve the @JSON value of the type @ref WAAPI_JSON_ARRAY
	 *	@details If the object is not of type @ref WAAPI_JSON_ARRAY or the index provided is out of range, this 
	 *	will throw an asserr() error
	 
	 *	@param[in] index The index in the @JSON array to retrieve the stored @JSON value
	 *	@returns The @JSON instance at the given index, or an assert() error is thrown
	 */
	virtual WaJson at( size_t index ) const;

	/**
	 *	@brief Will put a @JSON value into the type @ref WAAPI_JSON_OBJECT for the given key
	 *	@details If the @JSON instance is not of type @ref WAAPI_JSON_OBJECT, then the internal structure will be cleared and created into a @ref WAAPI_JSON_OBJECT instance
	 *	@param[in] key The key lookup that is to reference the @c json_value held by the @JSON object
	 *	@param[in] json_value The value to store, referenced by @c key in the @JSON object
	 */
	virtual void put( const  wa_wchar * key, const WaJson & json_value );

	/**
	 *	@brief Will put a @JSON value into the type @ref WAAPI_JSON_OBJECT for the given key. This is the string wrapper on the const  wa_char * version
	 *	@see put( const  wa_char * key, const WaJson & json_value )
	 */
	virtual void put( const wa_wstring & key, const WaJson & json_value );

	/**
	 *	@brief Will get a @JSON value from the type @ref WAAPI_JSON_OBJECT instance for the given key
	 *	@details If the instance is not of type @ref WAAPI_JSON_OBJECT or the key does not exist, then this will throw an assert() error
	 *	@param[in] key Will retrieve the @JSON value for the given key that references it
	 *	@returns The @JSON value for the given key, or an assert() error is thrown
	 */
	virtual WaJson get( const  wa_wchar * key ) const;

	/**
	 *	@brief Will get a @JSON value from the type @ref WAAPI_JSON_OBJECT instance for the given key. This is the string wrapper on the const  wa_char * version
	 *	@see get( const  wa_char * key )
	 */
	virtual WaJson get( const wa_wstring & key ) const;

	/**
	 *	@brief will retrieve the string value for the given key if it exists. Useful for known queries
	 *	@param[in] key The key to lookup the string value
	 *	@param[out] val The string value referenced by the key
	 *	@retval WAAPI_OK If successfully retrieves the string value for the given key
	 *	@retval WAAPI_ERROR_NOT_FOUND If the key does not exist
	 *	@retval WAAPI_ERROR_INVALID_STATE The key exists, but the mapped value is not of type WAAPI_JSON_STRING
	 */
	virtual wa_int get( const  wa_wchar * key, wa_wstring & val ) const;

	/**
	 *	@brief will retrieve the string value for the given key if it exists. Useful for known queries. This is a string wrapper on the const  wa_char * version
	 *	@see get( const  wa_char * key, wa_wstring & val )
	 */
	virtual wa_int get( const wa_wstring & key, wa_wstring & val ) const;

	/**
	 *	@brief will retrieve the int value for the given key if it exists. Useful for known queries
	 *	@param[in] key The key to lookup the string value
	 *	@param[out] val The int value referenced by the key
	 *	@retval WAAPI_OK If successfully retrieves the int value for the given key
	 *	@retval WAAPI_ERROR_NOT_FOUND If the key does not exist
	 *	@retval WAAPI_ERROR_INVALID_STATE The key exists, but the mapped value is not of type WAAPI_JSON_NUMBER
	 */
	virtual wa_int get( const  wa_wchar * key, wa_int & val ) const;

	/**
	 *	@brief will retrieve the int value for the given key if it exists. Useful for known queries. This is a string wrapper on the const  wa_char * version
	 *	@see get( const  wa_char * key, wa_int & val )
	 */
	virtual wa_int get( const wa_wstring & key, wa_int & val ) const;

	/**
	 *	@brief will retrieve the bool value for the given key if it exists. Useful for known queries
	 *	@param[in] key The key to lookup the string value
	 *	@param[out] val The int value referenced by the key
	 *	@retval WAAPI_OK If successfully retrieves the int value for the given key
	 *	@retval WAAPI_ERROR_NOT_FOUND If the key does not exist
	 *	@retval WAAPI_ERROR_INVALID_STATE The key exists, but the mapped value is not of type WAAPI_JSON_BOOL
	 */
	virtual wa_int get( const  wa_wchar * key, bool & val ) const;

	/**
	 *	@brief will retrieve the bool value for the given key if it exists. Useful for known queries. This is a string wrapper on the const  wa_char * version
	 *	@see get( const  wa_char * key, bool & val )
	 */
	virtual wa_int get( const wa_wstring & key, bool & val ) const;

	/**
	 *	@brief will retrieve the json value for the given key if it exists. Useful for known queries
	 *	@param[in] key The key to lookup the string value
	 *	@param[out] val The json value referenced by the key
	 *	@retval WAAPI_OK If successfully retrieves the int value for the given key
	 *	@retval WAAPI_ERROR_NOT_FOUND If the key does not exist
	 *	@retval WAAPI_ERROR_INVALID_STATE The key exists
	 */
	virtual wa_int get( const  wa_wchar * key, WaJson & val ) const;

	/**
	 *	@brief will retrieve the json value for the given key if it exists. Useful for known queries. This is a string wrapper on the const  wa_char * version
	 *	@see get( const  wa_char * key, wa_int & val )
	 */
	virtual wa_int get( const wa_wstring & key, WaJson & val ) const;

	/**
	 *	@brief Will remove a @JSON value from the instance if it is of type @ref WAAPI_JSON_OBJECT for the given key.
	 *	@param[in] key The key to remove the @JSON value it has stored. If the key does not exist, nothing happens
	 */
	virtual void remove( const  wa_wchar * key );
	
	/**
	 *	@brief Will remove a @JSON value from the instance if it is of type @ref WAAPI_JSON_OBJECT for the given key. This is a string wrapper on the const  wa_char * version
	 *	@see remove( const  wa_char * key )
	 */
	virtual void remove( const wa_wstring & key );

	/**
	 *	@brief Will check if the @ref WAAPI_JSON_OBJECT instance has a value for the given key.
	 *	@details If the instance is not of type @ref WAAPI_JSON_OBJECT, this function will return false
	 *	@returns true if contains a value for the key, false otherwise
	 */
	virtual bool contains( const  wa_wchar * key ) const;

	/**
	 *	@brief Will retrieve the keys of the @ref WAAPI_JSON_OBJECT instance if the instance is of type @ref WAAPI_JSON_OBJECT otherwise an empty list is returned
	 *	@details To handle an error on this, the key list returned will be empty meaning instance is not a @JSON object or the @JSON object is empty. Easier to code with
	 *	rather than surrounded with error cases
	 */
	virtual wa_json_object_keys keys() const;

	/**
	 *	@brief Will retrieve the @JSON number value from this instance if the instance is of type @ref WAAPI_JSON_NUMBER
	 *	@param[out] json_value The @JSON number value
	 *	@retval WAAPI_OK On successful retrieval.
	 *	@retval WAAPI_ERROR_INVALID_JSON If type does not match this data access 
	 */
	virtual wa_int val( wa_int & json_value ) const;

	/**
	 *	@brief Will retrieve the @JSON string value from this instance if the instance is of type @ref WAAPI_JSON_STRING
	 *	@param[out] json_value The @JSON string value
	 *	@retval WAAPI_OK On successful retrieval.
	 *	@retval WAAPI_ERROR_INVALID_JSON If type does not match this data access 
	 */
	virtual wa_int val( wa_wstring & json_value ) const;

	/**
	 *	@brief Will retrieve the @JSON boolean value from this instance if the instance is of type @ref WAAPI_JSON_BOOL
	 *	@param[out] json_value The @JSON boolean value
	 *	@retval WAAPI_OK On successful retrieval.
	 *	@retval WAAPI_ERROR_INVALID_JSON If type does not match this data access 
	 */
	virtual wa_int val( bool & json_value ) const;

	/**
	 *	@brief Will return the object as a @JSON formatted string
	 *	@returns The @JSON string representation of this instance
	 */
	virtual wa_wstring toString() const;

	/**
	 *	@brief Will return the object as a @JSON formatted string with indents and newlines
	 *	@returns The @JSON string representation of this instance with indents and newlines
	 */
	virtual wa_wstring toPrettyString() const;

	/**
	 *	@brief Will allocate the @c json_out parameter with the toString() version of this @JSON value
	 *	@attention It is the callers responsibility to call wa_api_free on the @c json_out returned!!! THIS IS VERY IMPORTANT!!!
	 */
	virtual void toCharPointer(  wa_wchar ** json_out ) const;

	/**
	 *	@brief Will allocate the @c json_out parameter with the toPrettyString() version of this @JSON value
	 *	@attention It is the callers responsibility to call wa_api_free on the @c json_out returned!!! THIS IS VERY IMPORTANT!!!
	 */
	virtual void toPrettyCharPointer(  wa_wchar ** json_out ) const;

	/**
	 *	@brief Returns the first element for type @ref WAAPI_JSON_ARRAY, and first value for type @ref WAAPI_JSON_OBJECT  
	 */
	virtual WaJson first();

protected:

	/**
	 *	@brief Stores the type of the @JSON value
	 */
	WaJsonType m_Type;

	// TODO: why the pointers? if going across binary boundaries, I don't think our constructors can take STL containers. If not,
	// we should be able to just use non-pointer objects
	/**
	 *	@brief Stores the actual value of the @JSON value
	 */
	union{
		wa_wstring		*	json_string;	/* Holds a @JSON string value */
		wa_int				json_number;	/* Holds a @JSON number value, NOTE: no need to support floating point numbers */
		bool					json_boolean;	/* Holds a @JSON boolean value */
		wa_json_array		*	json_array;		/* Holds a @JSON array value */
		wa_json_object	*	json_object;	/* Holds a @JSON object value */
	} m_Value;

private:
	/**
	 *	@brief Wrapper function to handle all "basic" initialization, e.g. set pointers to NULL, that each constructor will leverage so we have consistent instances
	 */
	virtual void _init();

	/**
	 *	@brief Will cleanup current internal memory and set the internal memory to represent this instance as a @ref WAAPI_JSON_NUMBER
	 *	@param[in] json_number The integer to set this instance to
	 */
	virtual void _set( const wa_int & json_number );

	/**
	 *	@brief Will cleanup current internal memory and set the internal memory to represent this instance as a @ref WAAPI_JSON_BOOL
	 *	@param[in] json_boolean The boolean to set this instance to
	 */
	virtual void _set( bool json_boolean );

	/**
	 *	@brief Will cleanup current internal memory and set the internal memory to represent this instance as a @ref WAAPI_JSON_STRING
	 *	@param[in] json_string The string to set this instance to
	 */
	virtual void _set( const  wa_wchar * json_string );

	/**
	 *	@brief Will cleanup current internal memory and set the internal memory to represent this instance as a @ref WAAPI_JSON_ARRAY
	 *	@param[in] json_array The vector to set this instance to
	 */
	virtual void _set( const wa_json_array & json_array );

	/**
	 *	@brief Will cleanup current internal memory and set the internal memory to represent this instance as a @ref WAAPI_JSON_OBJECT
	 *	@param[in] json_object The hash map to set this instance to
	 */
	virtual void _set( const wa_json_object & json_object );

	/** 
	 *	@brief Internally called by @ref toPrettyString to handle growing indents for complex @JSON types
	 *	@param[in] indent The current amount of spaces to indent
	 *	@returns The @JSON representation of this instance with indentations
	 */
	virtual wa_wstring _toPrettyString( int indent, bool indent_start = true ) const;

    /**
	 *	@brief Will escape a string into @JSON string format.
	 *	@return The escaped string
	 */
	virtual wa_wstring _encodeString( const wa_wstring & to_encode ) const;
};
