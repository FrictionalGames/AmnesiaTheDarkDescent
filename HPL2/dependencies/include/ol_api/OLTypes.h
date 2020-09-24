/*************************************************************************************
			Copyright(c)   2009 - 2011 OnLive, Inc.  All Rights Reserved.

NOTICE TO USER:

This code contains confidential material proprietary to OnLive, Inc. Your access to
and use of these confidential materials is subject to the terms and conditions of
your confidentiality and SDK license agreements with OnLive. This document and
information and ideas herein may not be disclosed, copied, reproduced or distributed
to anyone outside your company without the prior written consent of OnLive.

You may not modify, reverse engineer, or otherwise attempt to use this code for
purposes not specified in your SDK license agreement with OnLive. This material,
including but not limited to documentation and related code, is protected by U.S.
and international copyright laws and other intellectual property laws worldwide
including, but not limited to, U.S. and international patents and patents pending.
OnLive, MicroConsole, Brag Clips, Mova, Contour, the OnLive logo and the Mova logo
are trademarks or registered trademarks of OnLive, Inc. The OnLive logo and the Mova
logo are copyrights or registered copyrights of OnLive, Inc. All other trademarks
and other intellectual property assets are the property of their respective owners
and are used by permission. The furnishing of these materials does not give you any
license to said intellectual property.

THIS SOFTWARE IS PROVIDED BY ONLIVE "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*************************************************************************************/
//
// OnLive Service Platform API
// OLTypes.h $Revision: 78145 $
//

/// \file OLTypes.h
/// \brief Data types available to all APIs and functions within the OnLive SDK.
///
/// This module is a collection of common defines, structs, enums, and types for the entire OnLive SDK.

#ifndef	OLTYPES_H
#define	OLTYPES_H

namespace olapi
{
	/// \ingroup OLSERVICE_API_CONTAINERS
	/// \brief The max string length of a container key (does not include the nul char).
	#define OL_MAX_KEY_STRING_SIZE			256
	/// \ingroup OLSERVICE_API_CONTAINERS
	/// \brief The maximum string length of a container value (does not include the nul char).
	#define OL_MAX_VALUE_STRING_SIZE		256
	/// \ingroup OLSERVICE_API_CONTAINERS
	/// \brief The max string length of a container's description value; does not include the nul char.
	#define OL_MAX_VALUE_DESCRIPTION_SIZE	1024
	/// \ingroup OLLAUNCHER_API_LAUNCHING_PRODUCTS
	/// \brief The max string length of a product key value; see \ref olapi::OLLauncher::StartProduct().
	#define OL_MAX_VALUE_PRODUCT_KEY_SIZE	20
	/// \ingroup OLLAUNCHER_API_LAUNCHING_PRODUCTS
	/// \brief The max string length of a launched product's arguments string; See \ref olapi::OLLauncher::StartProduct().
	#define OL_MAX_LAUNCHED_ARG_SIZE		1024

	/// \addtogroup OLSERVICE_API_CONTEXT
	/// 

	/// \brief Unsolicited events are dispatched with an OLContext of OL_UNSOLICITED_CONTEXT.
	///
	/// For example, the OSP only allows you to suspend the user inactivity timer temporarily.
	/// If you leave the timer suspended for too long, the OSP will automatically enable the timer
	/// and dispatch an unsolicited StatusResult event with OL_UNSOLICITED_CONTEXT and
	/// a specific status code: \ref olapi::OL_INPUTTIMER_REACTIVATED.
	/// See \ref olapi::OLUserSession::SuspendInputTimeout for details about this example.
	#define OL_UNSOLICITED_CONTEXT			(0)


	/// \brief OL_INVALID_CONTEXT is returned when no valid context can be returned.  This denotes an error.
	#define OL_INVALID_CONTEXT				(0xffffffff)


	/// \brief OL_CONTEXT_MAX is the maximum context that will be returned by OLGetNextContext or OLReserveNextContext
	#define OL_CONTEXT_MAX					(0x7fffffff)

	/// \brief Returns True if a context is an error.  All error contexts will be above context max.
	#define IS_ERROR_CONTEXT(context) ((context) > OL_CONTEXT_MAX)

	/// \brief Returns True if a context is valid.  All error contexts will be above context max.
	#define IS_VALID_CONTEXT(context) ((context) <= OL_CONTEXT_MAX)

	/// \brief Returns True if a context is valid and not OL_UNSOLICITED_CONTEXT.  All error contexts will be above context max.
	#define IS_SOLICITED_CONTEXT(context) (IS_VALID_CONTEXT(context) && (context) != OL_UNSOLICITED_CONTEXT)

	/// \brief Returns True if a context is UNSOLICITED.
	#define IS_UNSOLICITED_CONTEXT(context) ((context) == OL_UNSOLICITED_CONTEXT)

	/// \brief An OLContext is used pair a SDK request to its async response event.
	///			Use \ref olapi::OLGetNextContext() to generate a new context for a request.
	typedef	unsigned int OLContext;


	///
	///	\brief OLAPI enumeration for duration of reserved contexts
	///
	/// See \ref OLSERVICE_API_CONTEXT for more information.
	///
	enum OLContextDuration
	{
		OL_DURATION_NONE,			///< No duration is set
		OL_DURATION_AUTO,			///< Automatically determine duration
		OL_DURATION_USER_SESSION,	///< Context will expire at end of this user session
		OL_DURATION_APP_SESSION,	///< Context will expire at end of this app session
		OL_DURATION_MAX
	};

	/// 


	/// \brief This is the max string length of a user's tag in our backend database.
	#define OL_MAX_USERNAME_SIZE			22


	/// \brief Constant used to mean infinite time
	#define	OL_INFINITE	(0xffffffff)


	/// \brief Used internally to suppress unused parameter warnings (for reserved/deprecated args).
	#define UNUSED_PARAM(param) (param)


	/// \brief Used by \ref olapi::OLWaitServicesCommunicating; 10 seconds in milliseconds.
	#define OL_MINIMUM_WAIT_SERVICES_TIME	(10 * 1000)


	///
	/// \ingroup OLOVERLAY_API_VIRTUAL_KEYBOARD
	///	\brief DEPRECATED in 1.66.4.4 - Identifies the type of virtual keyboard input expected; controls the allowed input characters.
	///
	enum OLStringType
	{
		OL_STRING =	0,								///< Keyboard input is ascii string.
		OL_FLOAT,									///< Keyboard input is a float.
		OL_INT,										///< Keyboard input is a 32bit integer.
	};

	/// disabling warning c4200 allows a nonstandard extension: zero-sized array in struct/union
	#pragma warning(push)


	#pragma	warning(disable: 4200)


	///
	/// \ingroup OLSERVICE_API_CONTAINERS
	///	\brief Represents a generic blob of data returned by the OSP.  See \ref olapi::OLDataList.
	///
	struct OLDataItem
	{
		int	size;									///< Size of data in bytes.
		char data[0];								///< The data's actual type depends on the OSP request that returned the data.
	};


	///
	/// \ingroup OLSERVICE_API_CONTAINERS
	///	\brief The OLDataList struct contains a NULL terminated list of \ref olapi::OLDataItem objects (representing blobs).
	///			The dataList is used as a generic mechanism for the OSP to return data from specific OSP requests.
	///
	/// The expected size of the list and the contents of each dataItem blob is defined by the OSP
	///		request that creates the list.
	///
	///	See \ref olapi::OLOverlay::GetInputFromKeyboard as an example of a request that returns a data list.
	///
	struct OLDataList
	{
		char list[0];								///< Null terminated array of OLDataItems, cast as (\ref olapi::OLDataItem *).
	};


	///
	/// \ingroup OLID
	///	\brief A wrapper struct representing an array of OLID objects (terminated by an invalid OLID).
	///
	/// You can find an invalid OLID by treating it as a bool (using the ! operator or comparing against 0); for example:
	/// \code
	///		olapi::OLIDList *idList; // assume this has been intialized elsewhere and is not NULL.
	///		
	///		// check the first element to see if the list is empty
	///		bool isEmptyList = !idList->idlist[0];
	///		
	///		// iterate over the list
	///		for ( int i=0; idList->idlist[i] != 0; ++i )
	///		{ /* do stuff */ }
	/// \endcode
	///
	struct OLIDList
	{
		OLID idlist[0];								///< Null terminated array of OLID's.
	};


	#pragma warning(pop)


	///
	/// \ingroup OLAPI_STATUS_CODE
	///	\brief Basic OLAPI enumerations for	reporting game errors.
	///
	/// See \ref olapi::OLStatusToString as well.
	///
	enum OLStatus
	{
		OL_SUCCESS = 1,								///< Return without an error.
		OL_SHARED_BASE				= -0x00001000,	///< Shared error codes
		OL_INTERNAL_ERROR,							///< FATAL: Internal error.  This error is fatal, the application should start the exit process.
		OL_INTERNAL_IO_ERROR,						///< FATAL: Internal IO error.  This error is fatal, the title should start the exit process.
		OL_OUT_OF_MEMORY,							///< FATAL: Insufficient memory to perform operation.  If memory can not be freed, this error is fatal.  The application should start the exit process.
		OL_INVALID_VERSION,							///< FATAL: Invalid DLL version.  This error is fatal.  The application should start the exit process.
		OL_INVALID_REG_KEY,							///< Internal error, registry is used for initial startup.
		OL_INVALID_REG_VALUE,						///< Internal error, registry is used for initial startup.
		OL_INVALID_REG_VALUE_NOT_FOUND,				///< Internal error, registry is used for initial startup.
		OL_FAIL,									///< Failed, but no error occured.
		OL_NOT_IMPLEMENTED,							///< Not implemented.
		OL_INVALID_PARAM,							///< One or more bad function parameters.
		OL_INVALID_PERMISSION,						///< Insufficient permission to	access data.
		OL_EVENT_TIMEOUT,							///< A HandleEvent was called, but timed out.
		OL_NO_ITEMS_IN_LIST,						///< No items exist for requested list.
		OL_SERVICES_NOT_STARTED,					///< Trying to use API without starting services.
		OL_SESSION_NOT_STARTED,						///< Trying to use API without a proper session in progress.
		OL_HOST_NOT_AVAILABLE,						///< internal error, host not available.
		OL_INVALID_SESSION_ID,						///< The session ID used is invalid.
		OL_OVERLAY_IN_USE,							///< There are other overlay UIs already on the screen.
		OL_BUSY,									///< The operation or event could not complete because it was busy.
		OL_SERVICE_BASE				= -0x00004000,	///< OLService specific error codes.
		OL_CONTAINER_NOT_FOUND,						///< Not found.
		OL_CONTAINER_LIST_NOT_FOUND,				///< Not found.
		OL_CONTAINER_LOCKED,						///< Locked.
		OL_IDENTIFIER_NOT_FOUND,					///< Not found.
		OL_IDENTIFIER_ALREADY_EXISTS,				///< Already exists.
		OL_RETURN_VALUE_TRUNCATED,					///< The return value is truncated due to insufficient buffer size.
		OL_APPLICATION_BASE			= -0x00005000,	///< OLApplication specific error codes.
		OL_ALREADY_STARTED,							///< Start command receive after already started.
		OL_ALREADY_BOUND,							///< A user is already bound.
		OL_RESUME_SUCCESFUL,						///< DEPRECATED; use OL_SUCCESS instead - Resume was requested in BindUser and was successful.
		OL_USER_BASE				= -0x00006000,	///< OLUser specific error codes
		OL_RESOLUTION_INVALID,						///< Used to tell the OSP that resolution requested is not supported.
		OL_INPUTTIMER_REACTIVATED,					///< The suspended input timer has been released.
		OL_IDENTITY_BASE			= -0x00007000,	///< OLIdentity specific error codes.
		OL_INVALID_IDENTITY_ID,						///< One or more of the supplied OLID's was invalid.
		OL_TITLE_BASE				= -0x00008000,	///< OLTitle specific error codes.
		OL_FRIEND_BASE				= -0x00009000,	///< OLFriends specific error codes.
		OL_OVERLAY_BASE				= -0x0000A000,	///< OLOverlay specific error codes.
		OL_CANCEL_BUTTON,							///< User select cancel on Overlay UI that requires user input.
		OL_MENU_NOT_ENABLED,						///< The requested menu is not enabled by the OSP.
		OL_ACHIEVEMENT_BASE			= -0x0000B000,	///< OLAchievement specific error codes.
		OL_NO_ACHIEVEMENT_DEFINITIONS,				///< No Achievement Definitions for this title.
		OL_NO_ACHIEVEMENTS,							///< No Achievements have been earned or unlocked for this user for this title.
		OL_MEDIA_BASE				= -0x0000D000,	///< OLMedia specific error codes.
		OL_MEDIA_ID_NOT_FOUND,						///< The media ID given was not found, either due to error, deletion, or stream end.
		OL_MEDIA_CONTEXT_NOT_FOUND,					///< The context given to StopMonitoringBragClips or StopMonitoringSpectatingStreams did not match any prior context given to the corresponding start call.
		OL_PARTIAL_LIST_RECEIVED,					///< MAX COUNT was exceeded, only a partial list was returned.

	};


	///
	/// \ingroup OLAPI_STATUS_CODE
	/// \brief Returns the supplied OLStatus value as a string (for logging).
	///
	///		\param[in] olStatus		The status to convert into a string.
	///
	///		\return Returns status code's name string (ex: OL_SUCCESS returns "OL_SUCCESS").
	///
	extern "C" __declspec(dllexport) const char* __cdecl OLStatusToString( OLStatus olStatus );


	///
	/// \ingroup OLSERVICE_API_LOGGING
	///	\brief The SDK log will ignore messages with a log level below the SDK's current log level.
	///
	/// See \ref OLSERVICE_API_LOGGING.
	enum OLLogLevel
	{
		OLLOG_DEBUG = 0,							///< Log everything.
		OLLOG_INFO,									///< Log information, warnings and errors.
		OLLOG_WARNING,								///< Log warning and errors.
		OLLOG_ERROR,								///< Log errors.
		OLLOG_FATAL,								///< Log fatal errors.
		OLLOG_MAX									///< enum max; used for validation
	};

}

#endif // OLTYPES_H
