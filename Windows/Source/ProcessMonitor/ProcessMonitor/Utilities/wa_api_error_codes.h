/****** THIS FILE IS AUTOMATICALLY GENERATED. DO NOT EDIT OR YOUR CHANGES WILL BE OVERWRITTEN! ******/
#pragma once
/**	@file wa_api_error_codes.h
 *	@brief Defines success and error codes returned from API calls
 *	@defgroup waapi_errorcodes Error codes
 */

#include "wa_api_data_types.h"

/**
 *	@def WAAPI_SUCCESS(code)
 *	@brief Checks an error @a code for success. Equates to true if successful error code, false otherwise
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_SUCCESS(code) (((wa_int)(code))>=0)

/**
 *	@def WAAPI_FAILED(code)
 *	@brief Checks an error @a code for failure. Equates to true if failure error code, false otherwise
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_FAILED(code) (((wa_int)(code))<0)

/**
 *	@def WAAPI_SUCCESS_SPECIAL(code)
 *	@brief Checks an error @a code for a special success case. Equates to true if the error code is a method specific success code, false otherwise.
 *		   NOTE: this will return false for other normal success cases, such as WAAPI_OK.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_SUCCESS_SPECIAL(code) (((wa_int)(code))>=1000)

/**
 *	@brief Defines a success.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_OK 0

/**
 *	@brief Defines a success and value of TRUE.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_OK_TRUE 1

/**
 *	@brief Defines a success and value of FALSE.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_OK_FALSE 2

/**
 *	@brief Defines a partial success (for multiple operations, some succeeded and some failed).
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_PARTIAL_SUCCESS 3

/**
 *	@brief Defines a successful call, but that a product has never, or does not report that it has ever, run a Backup.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_OK_NO_BACKUP_REPORTED 2000

/**
 *	@brief Defines a successful call, but that a product's user interface may not reflect any changes.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_OK_NOT_SHOWN_IN_GUI 5000

/**
 *	@brief Defines a successful call, but that a product has never, or does not report that it has ever, run a scan.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_OK_NO_SCAN_REPORTED 5001

/**
 *	@brief Defines a successful call, but the product cannot perform the operation because of an expired license.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_OK_PRODUCT_LICENSE_EXPIRED 5002

/**
 *	@brief Defines a successful call, but the product may not be able to perform the operation without some interaction from the user in the product UI.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_OK_USER_INTERACTION 5003

/**
 *	@brief Defines a successful call, but the product may open a user interface in order to perform the operation.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_OK_GUI_DISPLAYED 5004

/**
 *	@brief Defines a general failure.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_GENERAL -1

/**
 *	@brief Defines an error when the license file is missing.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_LICENSE_MISSING -2

/**
 *	@brief Defines an error when the license provided does not match the license file.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_LICENSE_MISMATCH -3

/**
 *	@brief Defines an error when the license key has expired, not be to be confused with component licenses.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_LICENSE_EXPIRED -4

/**
 *	@brief Defines an error when the API has not been initialized, yet a call requiring initialization was invoked.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_NOT_INITIALIZED -5

/**
 *	@brief Defines an error when initialization is called when the API is already initialized.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_ALREADY_INITIALIZED -6

/**
 *	@brief Defines an error when a call is made to a component that is not licensed.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_COMPONENT_NOT_LICENSED -7

/**
 *	@brief Defines an error when a call is made to a component whose license has expired.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_COMPONENT_LICENSE_EXPIRED -8

/**
 *	@brief Defines an error when a call is made to a component that is licensed, but is not deployed.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_COMPONENT_NOT_DEPLOYED -9

/**
 *	@brief Defines an error when the component module from disk has been tampered with, therefore unloaded by the api.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_COMPONENT_TAMPERED -10

/**
 *	@brief Defines an error when a method call was made on a component that does not support it.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_COMPONENT_METHOD_NOT_SUPPORTED -11

/**
 *	@brief Defines an error when a method call was made on a component that does not implement it.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_COMPONENT_METHOD_NOT_IMPLEMENTED -12

/**
 *	@brief Defines an error when the database is missing.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_DATABASE_MISSING -13

/**
 *	@brief Defines an error when the database exists but is corrupted.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_DATABASE_CORRUPTED -14

/**
 *	@brief Defines an error when a lookup in the database is missing. Perhaps the database is out of date?
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_DATABASE_ENTRY_MISSING -15

/**
 *	@brief Defines an error when the database is queried when it has yet to be initialized.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_DATABASE_NOT_INITIALIZED -16

/**
 *	@brief Defines an error where there is no connection when one is expected.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_NO_CONNECTION -17

/**
 *	@brief Defines an error when the provided computer name or IP to a remote call is not found in the network.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_IP_NOT_FOUND -18

/**
 *	@brief Defines an error when the provided user, password, or domain information is invalid.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_INVALID_CREDENTIALS -19

/**
 *	@brief Defines an error when the input arguments provided do not match the methods documented input requirements. Not to be confused with @ref WAAPI_ERROR_INVALID_JSON
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_INVALID_INPUT_ARGS -20

/**
 *	@brief Defines an error when the input arguments provided are not in proper @JSON format. Not to be confused with @ref WAAPI_ERROR_INVALID_INPUT_ARGS
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_INVALID_JSON -21

/**
 *	@brief Defines an error when the access is denied for the invocation call requested.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_ACCESS_DENIED -22

/**
 *	@brief Defines an error when the system or product is in an invalid state to perform the invocation call requested.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_INVALID_STATE -23

/**
 *	@brief Defines an error when working with Component Object Model (COM) on the Windows platform.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_COM -25

/**
 *	@brief Defines an error when a product is required to be running in order to perform the invocation call requested, yet the product is no longer running.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_NOT_RUNNING -24

/**
 *	@brief Defines an error when a request is dispatched and an exception is thrown.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_DISPATCH_EXCEPTION -26

/**
 *	@brief Defines an error in a native system API.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_NATIVE_API -27

/**
 *	@brief Defines an error when an object is not found. This can be objects held internally, or objects on the endpoint, such as files, backup times, etc.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_NOT_FOUND -28

/**
 *	@brief Defines an error when an object is not found in the Windows Registry. Windows only.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_REGISTRY_NOT_FOUND -29

/**
 *	@brief Defines an error when an operating is made to the Windows Registry and the operation is denied access. Windows only.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_REGISTRY_ACCESS_DENIED -30

/**
 *	@brief Defines an error when a query operating is made to the Windows Registry and the data type read is not currently supported by the API. Windows only.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_REGISTRY_READ_TYPE_NOT_SUPPORTED -31

/**
 *	@brief Defines an error that occurs in the Windows Registry but cannot be specified by a more specific registry error code. Windows only.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_REGISTRY_GENERAL -32

/**
 *	@brief Defines an error that occurs with our encryption/decryption security module.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_CRYPTO -33

/**
 *	@brief Defines an error when a cache entry is missing.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_NO_CACHE_ENTRY -34

/**
 *	@brief Defines an error when a cache entry exists, but it is out of date.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_CACHE_OUT_OF_DATE -35

/**
 *	@brief Defines an error when the configuration value for a valid key is of the incorrect type.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_INVALID_CONFIG_VALUE -36

/**
 *	@brief Defines an error when the configuration key is not a valid configuration option.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_INVALID_CONFIG_KEY -37

/**
 *	@brief Defines an error when an expression is in an invalid format.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_INVALID_SYNTAX -38

/**
 *	@brief Defines an error when a dynamic function call is invalid.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_NOT_A_FUNCTION -39

/**
 *	@brief Defines a timeout for uninstallation.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_UNINSTALL_TIMEOUT -40

/**
 *	@brief Defines an error internal to the scripting engine.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_SCRIPTING_ENGINE -41

/**
 *	@brief Defines a general error within a script.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_SCRIPTING_GENERAL -42

/**
 *	@brief Defines a general error during a WMI call.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_WMI -43

/**
 *	@brief Defines an error that occurs when an attempt to compile a script.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_SCRIPT_COMPILE_FAILURE -44

/**
 *	@brief Defines an error when an expression is found to be invalid or corrupt.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_MALFORMED_EXPRESSION -45

/**
 *	@brief Defines an error when an expression is found to be invalid or corrupt.
 *	@ingroup waapi_errorcodes
 */
#define WAAPI_ERROR_MALFORMED_EXPRESSION -45

#define WAAPI_ERROR_LOCAL_CACHE	-46
