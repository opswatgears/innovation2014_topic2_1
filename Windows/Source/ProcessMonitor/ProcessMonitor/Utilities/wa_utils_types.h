#pragma once
/**
 *	@file wa_utils_types.h
 *	@brief Defines the internal types used by all WANet functions. These are strictly internal and
 *	should NOT be exposed to the customer
 *	@defgroup waapi_internal [OPSWAT INTERNAL]
 *	@brief Represents ALL internal classes, structures, utilities, etc. that are used by our API but NOT exposed to customers!
 *
 *	@defgroup waapi_internal_types Data types
 *	@ingroup waapi_internal
 *	@brief Describes internal data types used throughout the back-end of the API
 */

// STL stuff, TODO: maybe remove and create custom C with only stuff we need?
#include <string>
#include <vector>
#include <set>
#include <hash_map>
#include <map>
#include <unordered_map>
#include <algorithm>

// windows stuff
#include "Windows.h"

// our data types
#include "wa_api_data_types.h"


/**
 *	@typedef wa_wstring
 *	@brief Defines our string data type
 *	@ingroup waapi_internal_types
 */
typedef std::basic_string< wa_wchar > wa_wstring;

/**
 *	@typedef wa_wstring_vector
 *	@brief Defines a vector of strings
 *	@ingroup waapi_internal_types
 */
typedef std::vector< wa_wstring > wa_wstring_vector;

/**
 *	@typedef wa_wstring_set
 *	@brief Defines a set of strings
 *	@ingroup waapi_internal_types
 */
typedef std::set< wa_wstring > wa_wstring_set;

#define WAUTIL_RETURN __int32
