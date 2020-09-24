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
// OLService.h $Revision: 74023 $
//

/// \file OLService.h

#ifndef OLSERVICE_H
#define OLSERVICE_H

#include <ol_api/OLAPI.h>
#include <windows.h>
#include <time.h>

namespace olapi
{

	/// \ingroup OLSERVICE_API_SDKINFO_EVENT
	/// \brief The name of the SDKInfo container; see \ref OLSERVICE_API_SDKINFO_EVENT.
	#define SDKINFO_CONTAINER_NAME "SDKInfo"

	///
	/// \ingroup OLSERVICE_API_SDKINFO_EVENT
	/// \brief Used to index into the \ref olapi::SDKInfoContainerKeyNames array.
	///
	/// See \ref OLSERVICE_API_SDKINFO_EVENT and \ref CONTAINERS_PAGE_USING_KEYS.
	///
	enum SDKInfoContainerKeys
	{
		OLKEY_RO_SDKInfoContainerName,				///< string, read only, name of container.
		OLKEY_RO_OLAPIVersion,						///< string, read only, version of OLAPI in use by OSP.
		OLKEY_RO_SDKInfoKeysMax				///< indicates the array size; not an index.
	};

	///
	/// \ingroup OLSERVICE_API_SDKINFO_EVENT
	/// \brief Array of keyName strings for the SDKInfo container, indexed by the \ref SDKInfoContainerKeys enum.
	///
	/// See \ref OLSERVICE_API_SDKINFO_EVENT and \ref CONTAINERS_PAGE_USING_KEYS.
	///
#ifndef NO_OLAPI_DLL_LINKAGE
	extern __declspec(dllimport) char *SDKInfoContainerKeyNames[olapi::OLKEY_RO_SDKInfoKeysMax];
#endif


	class OLServiceEventHandler;
	class OLServiceEventWaitingCallback;

	/// \class OLService
	/// \ingroup SERVICE
	/// \brief  The OLService API provides access to important functionality that all the other APIs depend on, such as
	/// the OnLive SDK Container system, Logging system, and the starting and stopping of service to the OSP. 
	/// Also see \ref SDK_CONTAINERS_PAGE.
	///
	/// \par API Initialization
	///		The OLService singleton is created and retrieved with \ref olapi::OLGetService() and destroyed 
	///		with \ref olapi::OLStopService().\n\n
	///		OLService should be the first OnLive API that a user initializes (since the other APIs depend on OLService). \n
	///		See \ref OLSERVICE_API_CREATE_AND_DESTROY and \ref SDK_SESSION_FLOW_PAGE for general info about initializing 
	///		the OnLive SDK APIs.
	/// 
	/// \par Logging
	///		OLService has hooks into the OSP's logging system (see \ref OLSERVICE_API_LOGGING).
	/// 
	/// \par Containers
	///		OLService provides functionality to manipulate containers (see \ref OLSERVICE_API_CONTAINERS).
	class OLService : public OLEventBase
	{
		public:

		/// \addtogroup OLSERVICE_API_EVENT_HANDLING
		/// 
		/// \name OLService Callbacks & Event Handling
		/// 

		/// 
		/// \brief Registers your callback handler object with the OLService API.
		///
		/// If you intend to handle callbacks for OLService, you should set
		/// a callback object before you start the OnLive Services with \ref olapi::OLStartServices.
		/// Once services are started, callbacks can be dispatched by the SDK at any time.
		///
		/// OLService stores a single callback object pointer; calling this method
		/// multiple times will replace the previous pointer.  There is no way to 'unregister'
		/// the pointer (you cannot set it to NULL).
		///
		///	See \ref EVENTS_PAGE_REGISTRATION for details.
		///
		///		\param[in]	callbackHandler	Your implementation of the OLServiceEventWaitingCallback interface.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			Normal operation; callbacks can be dispatched on the supplied obj while
		///										the services are running.
		///		\retval OL_INVALID_PARAM	Error - the supplied callback obj pointer was NULL (or invalid);
		///										the callback obj has not been set.
		///
		virtual OLStatus SetEventWaitingCallback(OLServiceEventWaitingCallback* callbackHandler) = 0;

		///
		/// \brief Registers your event handler object with the OLService API.
		///
		/// Once you have set the OLService API's eventHandler, calling one of the handle event methods
		///		may dispatch events on your object (assuming events are available to process).
		///		See \ref EVENTS_PAGE_HANDLING_OVERVIEW.
		///
		/// OLService stores a single eventHandler pointer; calling this method
		/// multiple times will replace the previous pointer.  There is no way to 'unregister'
		/// the pointer (you cannot set it to NULL).  
		///
		///	See \ref EVENTS_PAGE_REGISTRATION for details.
		///
		///		\param	eventHandler	Your implementation of the OLServiceEventHandler interface.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			Normal operation; events will be dispatched on the supplied obj.
		///		\retval OL_INVALID_PARAM	Error - the supplied eventHandler pointer was NULL (or invalid);
		///										the eventHandler has not been set.
		///
		virtual OLStatus SetEventHandler(OLServiceEventHandler* eventHandler) = 0;

		///
		/// \brief Sets this API's EventHandler and attempts to handle a single event.  NOTE: Prefer to use the simpler
		///  inherited method \ref olapi::OLEventBase::HandleEvent.
		///
		/// This method acts as a combination of \ref SetEventHandler and \ref olapi::OLEventBase::HandleEvent.
		/// If the supplied eventHandler is not NULL, we set the event handler to the supplied value then try to handle a single event.
		/// If the supplied eventHandler is NULL, we try to handle a single event using the previously set handler.
		///
		/// See \ref SDK_EVENTS_PAGE for details.
		///
		///		\param[in]	eventHandler	If NULL, we use the last set eventHandler.
		///									Otherwise, the API's event handler is set to this handler.
		///
		///		\param[in]	waitTimeMs	The max amount of time to wait for an event in milliseconds.
		///										Use 0 to return immediately if there is no event waiting (non-blocking).
		///										Otherwise, this call may block for up to waitTimeMs if no event is available.
		///										You can use \ref OL_INFINITE to wait forever for the next event.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			An event was dispatched.
		///		\retval OL_INVALID_PARAM	The supplied handler was NULL, but the API does not have an event handler set.
		///		\retval OL_EVENT_TIMEOUT	No events were available for the duration of the wait
		///
		virtual OLStatus HandleEvent(OLServiceEventHandler* eventHandler, unsigned int waitTimeMs) = 0;

		// 
		// 
		/// \addtogroup OLSERVICE_API_CONTAINERS
		/// 
		/// \name OLService Container Helpers: Reading Container Values
		/// 

		///
		/// \brief Copies a value from a container into a provided string buffer.
		///
		/// Containers natively store all values as strings, but there are methods to parse the strings into
		/// other types (see \ref CONTAINERS_PAGE_NON_STRING_VALUES).
		///
		/// Use \ref GetContainerValueSize or \ref OL_MAX_VALUE_STRING_SIZE when allocating the buffer
		/// to hold the value.  If your buffer is too small for the value, the value is truncated, and \ref OL_RETURN_VALUE_TRUNCATED 
		/// is returned.
		///
		/// See \ref SDK_CONTAINERS_PAGE for information about containers.
		///
		///		\param[in] containerID		ID of the container to read from.
		///
		///		\param[in] keyName			The string keyName of the value to retrieve (see \ref CONTAINERS_PAGE_USING_KEYS).
		///
		///		\param[in] valueBufferSize	The size of the supplied valueBuffer in bytes.
		///
		///		\param[out] valueBuffer		The value string is copied into this buffer.
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the buffer now has a copy of the specified container value.
		///		\retval OL_RETURN_VALUE_TRUNCATED	The size of valueBuffer is not large enough to hold the value; the value was truncated.
		///		\retval OL_INVALID_PARAM			A supplied param was invalid (the containerId is the wrong type, the keyName is NULL, the valueBufferSize is less or equal to zero, or the valueBuffer is NULL).
		///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
		///		\retval OL_IDENTIFIER_NOT_FOUND		The keyName was not found in the container.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_ERROR			A generic internal SDK/OSP error.
		///
		virtual OLStatus GetContainerValue(OLID containerID, const char *keyName, int valueBufferSize, char *valueBuffer) = 0;

		///
		/// \brief Returns the size of a specific container value string in bytes.  Note: The size is returned as an OLStatus type and should be
		///		cast to an int to determine failure (a negative value) or success (a positive buffer size).
		///
		/// You should always cast this method's return value to an int.
		///		If the integer is negative, the method had an error and you should use the returned OLStatus type.
		///		If the integer is positive, the method succeeded, and the int is the value's buffer size in bytes.
		///
		/// See \ref SDK_CONTAINERS_PAGE for information about containers.
		///
		///		\param[in] containerID	ID of the container to read from.
		///
		///		\param[in] keyName		The string keyName of the value to retrieve (see \ref CONTAINERS_PAGE_USING_KEYS).
		///
		///	\return	Returns an OLStatus code that doubles as the value's size.  If the enum value is positive, treat it as the size.  
		///				Otherwise, treat it as an OLStatus error code.
		///		\retval <positive_value>			Success, cast the enum to an int to get the value string's size.
		///		\retval OL_INVALID_PARAM			A supplied param was invalid (containerId was the wrong type or keyName was NULL).
		///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
		///		\retval OL_IDENTIFIER_NOT_FOUND		The keyName was not found in the container.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_ERROR			A generic internal SDK/OSP error.
		///
		virtual OLStatus GetContainerValueSize(OLID containerID, const char *keyName) = 0;

		///
		/// \brief Read a container value string and parse it into the supplied integer.
		/// 
		/// You should only use this method on integer container values (ex: \ref olapi::OLKEY_RO_Gamepad).
		/// This method will happily convert any string into an integer (much like atoi will); there is no
		/// specific error if the underlying string is not an integer.
		///
		/// See \ref SDK_CONTAINERS_PAGE for information about containers.
		///
		///		\param[in] containerID	ID of the container to read from.
		///
		///		\param[in] keyName		The string keyName of the value to retrieve (see \ref CONTAINERS_PAGE_USING_KEYS).
		///
		///		\param[out] value		Resulting integer value (using pointer to pass by reference).
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the specified container value was copied into the value arg.
		///		\retval OL_INVALID_PARAM			A supplied param was invalid (the containerId is the wrong type or the value pointer is NULL).
		///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
		///		\retval OL_IDENTIFIER_NOT_FOUND		The keyName was not found in the container.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_ERROR			A generic internal SDK/OSP error.
		///
		virtual OLStatus GetContainerValueAsInt(OLID containerID, const char *keyName, int *value) = 0;

		///
		/// \brief Read a container value string and parse it into the supplied double.
		///
		/// You should only use this method on float container values (ex: \ref olapi::OLKEY_RW_AudioVolume).
		/// This method will happily convert any string into a double (much like atof will); there is no
		/// error if the underlying string is not a floating point number.
		///
		/// See \ref SDK_CONTAINERS_PAGE for information about containers.
		///
		///		\param[in] containerID	ID of the container to read from.
		///
		///		\param[in] keyName		The string keyName of the value to retrieve (see \ref CONTAINERS_PAGE_USING_KEYS).
		///
		///		\param[out] value		Resulting double value (using a pointer to pass by reference).
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the specified container value was copied into the value arg.
		///		\retval OL_INVALID_PARAM			A supplied param was invalid (the containerId is the wrong type or the value pointer is NULL).
		///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
		///		\retval OL_IDENTIFIER_NOT_FOUND		The keyName was not found in the container.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_ERROR			A generic internal SDK/OSP error.
		///
		virtual OLStatus GetContainerValueAsFloat(OLID containerID, const char *keyName, double *value) = 0;

		///
		/// \brief Read a container value string and parse it into the supplied OLID (you must supply the OLID's type).
		///
		/// You should only use this method on OLID container values (ex: \ref olapi::OLKEY_RO_UserSessionID).
		/// You also need to provide the correct type for the OLID (so for the UserSessionId, you would supply \ref olapi::OL_SESSION_ID).
		///
		/// See \ref SDK_CONTAINERS_PAGE for information about containers.
		///
		///		\param[in] containerID	ID of the container to read from.
		///
		///		\param[in] keyName		The string keyName of the value to retrieve (see \ref CONTAINERS_PAGE_USING_KEYS).
		///
		///		\param[out] value		Resulting OLID value (the id is set from the container and the type is what you supplied).
		///
		///		\param[in] idType		Type of ID to create.
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the specified container value was copied into the value arg.
		///		\retval OL_INVALID_PARAM			A supplied param was invalid (the containerId is the wrong type or the value pointer is NULL).
		///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
		///		\retval OL_IDENTIFIER_NOT_FOUND		The keyName was not found in the container.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_ERROR			A generic internal SDK/OSP error.
		///
		virtual OLStatus GetContainerValueAsID(OLID containerID, const char *keyName, OLID *value, OLIDTypes idType) = 0;

		// 
		/// \name OLService Container Helpers: Writing Container Values
		/// 


		///
		/// \brief Set a container value to a string (writable values only).
		///
		/// You can only write to values that are marked as read-write (OLKEY_RW_*).
		/// Updates are not published to the OSP until you flush or destroy the container.
		/// See \ref CONTAINERS_PAGE_WRITING_VALUES.
		///
		///		\param[in] containerID	ID of the container to update.
		///
		///		\param[in] keyName		The string keyName of the value to update (see \ref CONTAINERS_PAGE_USING_KEYS).
		///
		///		\param[in] value		The new string value; must be < \ref OL_MAX_VALUE_STRING_SIZE bytes long.
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the specified container value was updated locally (but still needs to be flushed to the OSP).
		///		\retval OL_INVALID_PERMISSION		The specified container key is not writable.
		///		\retval OL_INVALID_PARAM			A supplied param was invalid (the containerId is the wrong type or a key/value pointer was NULL).
		///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
		///		\retval OL_IDENTIFIER_NOT_FOUND		The keyName was not found in the container.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_ERROR			A generic internal SDK/OSP error.
		///		\retval OL_OUT_OF_MEMEORY			A memory allocation failed.
		///
		virtual OLStatus SetContainerValue(OLID containerID, char *keyName, char *value) = 0;

		///
		/// \brief Set a container value to an int (writable values only).
		///
		/// You can only write to values that are marked as read-write (OLKEY_RW_*).
		/// Updates are not published to the OSP until you flush or destroy the container.
		/// See \ref CONTAINERS_PAGE_WRITING_VALUES.
		///
		///		\param[in] containerID	ID of the container to update.
		///
		///		\param[in] keyName		The string keyName of the value to update (see \ref CONTAINERS_PAGE_USING_KEYS).
		///
		///		\param[in] value		The new integer value.
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the specified container value was updated locally (but still needs to be flushed to the OSP).
		///		\retval OL_INVALID_PERMISSION		The specified container key is not writable.
		///		\retval OL_INVALID_PARAM			A supplied param was invalid (the containerId is the wrong type or the key was NULL).
		///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
		///		\retval OL_IDENTIFIER_NOT_FOUND		The keyName was not found in the container.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_ERROR			A generic internal SDK/OSP error.
		///		\retval OL_OUT_OF_MEMEORY			A memory allocation failed.
		///
		virtual OLStatus SetContainerValueFromInt(OLID containerID, const char *keyName, int value) = 0;

		///
		/// \brief Set a container value to a double (writable values only).
		///
		/// You can only write to values that are marked as read-write (OLKEY_RW_*).
		/// Updates are not published to the OSP until you flush or destroy the container.
		/// See \ref CONTAINERS_PAGE_WRITING_VALUES.
		///
		///		\param[in] containerID	ID of the container to update.
		///
		///		\param[in] keyName		The string keyName of the value to update (see \ref CONTAINERS_PAGE_USING_KEYS).
		///
		///		\param[in] value		The new floating point value
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the specified container value was updated locally (but still needs to be flushed to the OSP).
		///		\retval OL_INVALID_PERMISSION		The specified container key is not writable.
		///		\retval OL_INVALID_PARAM			A supplied param was invalid (the containerId is the wrong type or the key was NULL).
		///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
		///		\retval OL_IDENTIFIER_NOT_FOUND		The keyName was not found in the container.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_ERROR			A generic internal SDK/OSP error.
		///		\retval OL_OUT_OF_MEMEORY			A memory allocation failed.
		///
		virtual OLStatus SetContainerValueFromFloat(OLID containerID, const char *keyName, double value) = 0;

		///
		/// \brief Set a container value to an OLID (writable values only).
		///
		/// You can only write to values that are marked as read-write (OLKEY_RW_*).
		/// Updates are not published to the OSP until you flush or destroy the container.
		/// See \ref CONTAINERS_PAGE_WRITING_VALUES.
		///
		///		\param[in] containerID	ID of the container to update.
		///
		///		\param[in] keyName		The string keyName of the value to update (see \ref CONTAINERS_PAGE_USING_KEYS).
		///
		///		\param[in] value		The new OnLive ID value (Note: The value is not altered)
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the specified container value was updated locally (but still needs to be flushed to the OSP).
		///		\retval OL_INVALID_PERMISSION		The specified container key is not writable.
		///		\retval OL_INVALID_PARAM			A supplied param was invalid (the containerId is the wrong type or the key was NULL).
		///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
		///		\retval OL_IDENTIFIER_NOT_FOUND		The keyName was not found in the container.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_ERROR			A generic internal SDK/OSP error.
		///		\retval OL_OUT_OF_MEMEORY			A memory allocation failed.
		///
		virtual OLStatus SetContainerValueFromID(OLID containerID, const char *keyName, OLID &value) = 0;

		// 
		/// \name OLService Container Helpers: Destroying Containers
		/// 

		///
		/// \brief Destroys a container (after flushing any modified values to the OSP).
		///
		/// When done using the information in a container, use this function to destroy it.
		/// If the container has been updated (values were set), destroying the container will
		/// first flush its contents to the OSP then destroy the container.
		///
		/// If this was the last container in a list the list will automaticaly be destructed.
		///
		/// See \ref CONTAINERS_PAGE_SESSION_FLOW
		///
		///		\param[in]	containerID		ID of container to destroy.
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the specified container was destroyed and the ID is now invalid.
		///		\retval OL_INVALID_PARAM			The containerId is the wrong type.
		///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///
		virtual OLStatus DestroyContainer(OLID containerID) = 0;

		///
		/// \brief Calls DestroyContainer on all containers associated with the specified OnLive session ID (user or application session).
		///
		/// Destroys all of the application session's or user session's containers.  Any updated values in the containers will be 
		/// flushed to the OSP before destruction.  This method can destroy containers that are in a containerId 
		/// list (removing the container ID from the list).  If a list's last container is destroyed, the list itself
		/// is destroyed as well.
		///
		/// SDK Users do not typically need to call this method.  The SDK will call this to destroy all of a user's
		///  containers after they have unbound.
		///
		/// Note: If the sessionId is invalid (or not found), this method returns OL_SUCCESS.
		///
		/// See \ref DestroySessionContainerLists and \ref CONTAINERS_PAGE_SESSION_FLOW
		///
		///		\param[in]	sessionID		ID of session to destroy containers for (can be an application or user session).
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; all containers associated with the session have been destroyed.
		///		\retval OL_INVALID_PARAM			The containerId is the wrong type.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///
		virtual OLStatus DestroySessionContainers(OLID sessionID) = 0;

		///
		/// \brief Destroys a containerList and all the containers in the list (after flushing any modified values to the OSP).
		///
		/// See \ref CONTAINERS_PAGE_SESSION_FLOW
		///
		///		\param[in]	containerListID ID of the containerList to destroy.
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the specified container list was destroyed and the ID is now invalid.
		///		\retval OL_INVALID_PARAM			The containerId is the wrong type.
		///		\retval OL_CONTAINER_LIST_NOT_FOUND The container list was not found (wrong containerId).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///
		virtual olapi::OLStatus DestroyContainerList(olapi::OLID containerListID) = 0;

		///
		/// \brief Destroys all the ContainerLists associated with the supplied user or application session ID.
		///
		/// Calls DestroyContainerList on all containerLists associated with the supplied user or application session ID.
		/// Any updated values in the containers will be flushed to the OSP before destruction.
		///
		/// SDK Users do not typically need to call this method.  The SDK will call this to destroy all of a user's
		///  container lists after they have unbound.
		///
		/// Note: If the sessionId is invalid (or not found), this method returns OL_SUCCESS.
		///
		/// See \ref DestroySessionContainers and \ref CONTAINERS_PAGE_SESSION_FLOW
		///
		///		\param[in]	sessionID		ID of session to destroy container lists for (can be an application or user session).
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; all containers associated with the session have been destroyed.
		///		\retval OL_INVALID_PARAM			The containerId is the wrong type.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///
		virtual olapi::OLStatus DestroySessionContainerLists(olapi::OLID sessionID) = 0;

		///
		/// \brief Writes all updated container values to the OSP.
		///
		/// Writing container values only modifies the locally cached container; the values
		/// are not written to the OSP until the container is flushed (or destroyed).
		///
		/// See \ref CONTAINERS_PAGE_WRITING_VALUES.
		///
		///		\param[in]	containerID		ID of container to flush.
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the specified container's modified values were written to the OSP.
		///		\retval OL_INVALID_PARAM			The containerId is the wrong type.
		///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (writable
		///												containers are only stored on users).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///
		virtual OLStatus FlushContainer(OLID containerID) = 0;

		// 

		/// \name OLService Container Helpers: Reading Container Values
		/// 

		///
		/// \brief Copy a container's name into the supplied string buffer.
		///
		/// Use \ref GetContainerValueSize or \ref OL_MAX_VALUE_STRING_SIZE when allocating the buffer
		/// to hold the name.  If your buffer is too small for the name, the name is truncated to
		/// fit in the buffer and the method returns OL_RETURN_VALUE_TRUNCATED.
		///
		/// Note: This is analogous to calling GetContainerValue with the container's name key.
		///
		///		\param[in] containerID		ID of the container to read from.
		///
		///		\param[in] nameBufferSize	Size of the supplied nameBuffer in bytes.
		///
		///		\param[out] nameBuffer		The container name is copied into this buffer.
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the buffer now has a copy of the container's name.
		///		\retval OL_RETURN_VALUE_TRUNCATED	The size of valueBuffer is not large enough to hold the value; the value was truncated.
		///		\retval OL_INVALID_PARAM			A supplied param was invalid (the containerId is the wrong type, the nameBufferSize is equal or less than zero, or the nameBuffer is NULL).
		///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///
		virtual OLStatus GetContainerName(OLID containerID, int nameBufferSize, char *nameBuffer) = 0;


		// 
		// 
		/// \name OLService Logging Methods
		///	
		/// \addtogroup	OLSERVICE_API_LOGGING
		/// 

		///
		/// \brief Set the current log level for the SDK's log.  The OSP will only log messages
		///   with a logLevel >= the SDK's current log level.
		///
		///	The SDK log level affects the \ref Log method as well as the \ref OLServiceEventHandler::dbg_SDKLogReceived event.
		///
		/// \note The OSP can override the client's log level when configured to do so.  If this method is not working as expected
		///    on an OnLive dev/test site, the site is probably overriding your setting and you should contact your OnLive producer.
		///
		/// See \ref OLSERVICE_API_LOGGING.
		///
		/// \warning Use this function for debugging only.  You must remove any calls to dbg_SetSDKLogLevel
		/// before submitting your title for OnLive certification. 
		///
		///		\param[in]	olLogLevel		This sets the verbosity of the log retrieved by the API.
		///
		///	\return	Always returns OLStatus OL_SUCCESS.
		///
		virtual OLStatus dbg_SetSDKLogLevel(OLLogLevel olLogLevel) = 0;

		///
		/// \brief Control reception of log messages from the OSP (via the \ref OLServiceEventHandler::dbg_SDKLogReceived event)
		///
		/// Log reception is disabled by default (and is always disabled in the production OnLive environment).
		///
		/// \warning Use this function for development/debugging only.  Your title will fail OnLive Certification if it
		///		calls dbg_EnableSDKLogReception.
		///
		/// See \ref OLSERVICE_API_LOGGING.
		///
		///		\param[in]	value	Enable or disable dbg_SDKLogReceived events.
		///
		///	\return	Always returns OLStatus OL_SUCCESS.
		///
		virtual OLStatus dbg_EnableSDKLogReception(bool value) = 0;

		///
		/// \brief Write a message to the OSP's log at a particular logLevel.  Uses printf formatting and truncates to ~1024 chars.
		///
		/// \par Log Rate limits
		///		Do not exceed an average of 1 log per second for OLLOG_DEBUG messages.\n
		///		Do not exceed an average of 1 log per minute for other log levels.\n
		///		Titles will be rejected by OnLive certification for logging abuse.
		///
		///	OnLive recommends that you remove all but the most important logging calls from
		///		your title's final submission builds.
		///
		/// The SDK ignores log messages that are below the SDK's current log level (see \ref dbg_SetSDKLogLevel).
		///
		/// \note If you have enabled SDK log reception, any messages you have successfully logged to the SDK log will be echoed back 
		///  via the \ref OLServiceEventHandler::dbg_SDKLogReceived event.
		///
		/// See \ref OLSERVICE_API_LOGGING.
		///
		///		\param[in]	logLevel	The log level for this message.
		///		\param[in]	format		A printf compatible format string.
		///		\param[in]	...			Variable arguments for your printf format string.
		///
		virtual void Log(OLLogLevel logLevel, char* format, ...) = 0;

		//
		//

		///
		/// \brief INTERNAL USE ONLY: Sends an unsolicited heartbeat to the OSP (for debugging the OSP)
		///
		/// The OLService handles OSP heartbeats automatically while services are running.
		///   This method allows you to send an 'extra' heartbeat response.
		///
		/// \warning Use this function for debugging only. You must remove any calls to dbg_SendHeartbeat
		/// before submitting your title for OnLive Compliance. 
		///
		///		\param[in]	context			The OLContext of the heartbeat request you are replying to.
		///
		///	\return An OLStatus code.
		///		\retval OL_SUCCESS					Success; a heartbeat has been sent to the OSP.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus dbg_SendHeartbeat(olapi::OLContext context) = 0;
	};

	///
	/// \ingroup OLSERVICE_API_EVENT_HANDLING SERVICE
	/// \class	OLServiceEventHandler
	///
	/// \brief This is the OLService API's Event interface; your OLService event handler must implement this interface.
	///		Each method represents an asynchronous event returned by the SDK.
	///
	/// See \ref EVENTS_PAGE_CONCEPTS for information about callbacks and events.
	///
	class OLServiceEventHandler
	{
		public:

		///
		/// \ingroup OLSERVICE_API_LOGGING
		/// \brief Event sent when the SDK writes to the OSP log, allowing you to echo the OSP log message into your title's log.
		///
		/// \warning Use this event for development/debugging only.  Your title will fail OnLive Certification if it
		///		calls \ref OLService::dbg_EnableSDKLogReception to enable the dbg_SDKLogReceived event.
		///
		///  This event is only triggered if SDK Log Reception is enabled with \ref OLService::dbg_EnableSDKLogReception.  Once enabled,
		///  any OSP log messages that are >= the current SDKLogLevel (see \ref OLService::dbg_SetSDKLogLevel)
		///  will trigger a dbg_SDKLogReceived event.
		///
		/// \warning Do not call \ref OLService::Log from within dbg_SDKLogReceived or you can start an infinite log 
		/// 	message spam loop.  OLService::Log can trigger a \ref OLServiceEventHandler::dbg_SDKLogReceived event
		/// 	(if the event is enabled and the message passes the current log level).
		///
		/// See \ref OLSERVICE_API_LOGGING.
		///
		///		\param[in]	logMessage		The OSP message string. This string is freed when this method returns;
		///									do not save the pointer.
		///
		///	\return	Returns an OLStatus code to the SDK.
		///		\retval OL_SUCCESS			You have handled the event.
		///		\retval OL_NOT_IMPLEMENTED	You have ignored the event (this has no effect on OLService).
		///
		virtual OLStatus dbg_SDKLogReceived(char *logMessage) = 0;

		///
		/// \ingroup OLSERVICE_API_DBG_TEST_HARNESS_EVENT
		/// \brief The dbg_IsTestHarness event is sent if the title is running in the SDK Test Harness; use this event to put 
		///		your title in 'development mode' (ex: windowed, allow debug console, etc). 
		///
		///		The function described below is used for the dbg_IsTestHarness event.
		///  
		///		The dbg_IsTestHarness is sent automatically when you connect to the SDK Test Harness's emulated OSP environment. The event 
		///		is never sent if your title is running on an OnLive site. You must remove any code that handles an dbg_IsTestHarness 
		///		event before submitting your title for OnLive Compliance. 
		///
		virtual void dbg_IsTestHarness() = 0;

		///
		/// \ingroup OLSERVICE_API_SDKINFO_EVENT
		/// \brief This event tells you the runtime version of the OnLive SDK.  Note: SDKInfo is automatically retrieved and dispatched
		///		when you connect to the OSP. 
		///
		///		The define directive, enum, function, and array  described below are used for the SdkInfo event.
		/// 
		///  Each time your title is launched on an OnLive site, the OSP may update the OnLive SDK DLL to pickup bugfixes in 
		///  the SDK itself.  The SDKInfo event tells you the version of the DLL you are running against (as opposed to 
		///  \ref OLAPI_VERSION, which is the version you compiled against).
		///
		///  Get container key names by using the \ref SDKInfoContainerKeys enum to index into the \ref SDKInfoContainerKeyNames string array.
		///
		/// See \ref CONTAINERS_PAGE_USING_CONTAINERS for an overview of containers.
		///
		///		\param[in] containerID		The SDKInfo container's id.
		///
		///	\return	Returns an OLStatus code to the SDK.
		///		\retval OL_SUCCESS			You have handled the event; the containerId is valid until explicitly destroyed.
		///		\retval OL_NOT_IMPLEMENTED	You have ignored the event; the container is automatically destroyed.
		///		\retval OL_FAIL				You have had a fatal error; the container is automatically destroyed.
		///
		virtual OLStatus SDKInfoContainerIDReceived(olapi::OLID containerID) = 0;
	};


	///
	/// \ingroup OLSERVICE_API_EVENT_HANDLING SERVICE
	/// \class OLServiceEventWaitingCallback
	/// \brief This is the OLService API's callback interface; your title's OLService callback handler must implement this interface.
	///
	/// Each method in this class represents an OLService callback.  If you choose to handle OLService callbacks,
	/// register your callback handler using \ref olapi::OLService::SetEventWaitingCallback.  Callback methods
	/// may be invoked at any time once the OnLive Services are running (see \ref OLSERVICE_API_CONNECTION_DISCONNECTION).
	///
	/// \note Callback methods are always invoked from an internal OnLive SDK thread, so all callback implementations must be thread-safe.
	///
	/// See \ref EVENTS_PAGE_CONCEPTS for information about callbacks and events.
	///
	class OLServiceEventWaitingCallback
	{
		public:

		///
		/// \brief Callback dispatched when an OLService event is waiting to be handled.
		///
		/// If your title handles events from callbacks, call \ref OLService::HandleEvent to dispatch the waiting event.
		///	If you handle events by polling, you can ignore this event.
		///
		/// \note The EventWaiting callback must be thread-safe and must complete in less than 2 milliseconds.
		///
		/// See \ref EVENTS_PAGE_HANDLING_OVERVIEW for more information.
		///
		virtual void EventWaiting() = 0;
	};

	/// \addtogroup OLSERVICE_API_CREATE_AND_DESTROY
	/// 
	
	///
	/// \brief Get the OLService API singleton (the instance is created if necessary).
	///
	///	The OLService pointer can be stored until the API is destroyed with \ref olapi::OLStopService().
	///
	/// See \ref OLSERVICE_API_CREATE_AND_DESTROY.
	///
	///		\param[in] version		The API version string (for class compatibility), use \ref OLAPI_VERSION.
	///
	///		\param[out] olService	The supplied pointer is set to the OLService instance on success.
	///								The pointer is not modified on failure.
	///
	///	\return An OLStatus code.
	///		\retval	OL_SUCCESS			Success; the service pointer is set to the singleton.
	///		\retval OL_INVALID_PARAM	The supplied version string or service pointer is NULL.
	///		\retval OL_INVALID_VERSION	The API version requested is not supported. This is a fatal 
	///										error, and your title should exit.
	///		\retval OL_INTERNAL_ERROR	A general internal or system error.
	///		\retval OL_OUT_OF_MEMORY	The SDK could not alloc memory for the singleton.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLGetService(const char* version, OLService** olService);

	///
	/// \brief Destroy the OLService API singleton.
	///
	/// The OnLive Services should be stopped before you destroy an API instance; see \ref olapi::OLStopServices().
	///
	/// See \ref OLSERVICE_API_CREATE_AND_DESTROY.
	///
	///		\param[in] olService		Pointer to the OLService instance to destroy.
	///
	///	\return Returns an OLStatus code.
	///		\retval	OL_SUCCESS			Success; the OLService instance was destroyed.
	///		\retval OL_INVALID_PARAM	The supplied OLService pointer was NULL.
	///
	extern "C" __declspec(dllexport) olapi::OLStatus __cdecl OLStopService(OLService* olService);

	//

	/// \addtogroup OLSERVICE_API_CONNECTION_DISCONNECTION
	///	

	///
	/// \brief Returns true if the OnLive Services are running.  Services are started/stopped with
	///			\ref olapi::OLStartServices and \ref olapi::OLStopServices.
	///
	///	\see OLSERVICE_API_CONNECTION_DISCONNECTION.
	///
	///  \return true if you are ready to connect to the OSP (between calls to OLStartServices and OLStopServices).
	///
	extern "C" __declspec(dllexport) bool __cdecl OLAreServicesRunning();

	///
	/// \brief Returns true if the OnLive Services are communicating (connected).  \ref olapi::OLStartServices
	///			begins running the services immediately and begins the connection process.
	///
	/// Services are started/stopped with \ref olapi::OLStartServices and \ref olapi::OLStopServices.
	///
	///	\see OLSERVICE_API_CONNECTION_DISCONNECTION and \ref olapi::OLWaitServicesCommunicating.
	///
	///  \return true if you are connected to the OSP (between calls to OLStartServices and OLStopServices).
	///
	extern "C" __declspec(dllexport) bool __cdecl OLAreServicesCommunicating();

	///
	/// \brief Blocks until the connection to the OSP is established (or timeout).  See \ref olapi::OLStartServices
	///		and \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
	///
	/// You do not have to wait for OSP communicating before issuing requests (since requests are queued), but
	/// blocking until you are connected can prevent confusion while developing.  For example, if you run the game
	/// without an instance of the test harness running, OLStartServices will succeed, but you will never start
	/// communicating.
	///
	/// You can also poll for an established connection with \ref olapi::OLAreServicesCommunicating.
	/// 
	/// \warning This method blocks the calling thread, but the OnLive SDK dispatches callbacks from
	///		its internal DLL thread.  You may receive a callback before this method returns.
	///
	///		\param[in] waitTimeMs		Maximum amount of time in milliseconds to wait for connection.
	///										The OSP will always wait at least \ref OL_MINIMUM_WAIT_SERVICES_TIME ms.
	///
	///	\return Returns An OLStatus code.
	///		\retval OL_SUCCESS	Success; you have connected to the OSP.
	///		\retval OL_FAIL		Connection timeout (failure); your title should exit.
	///
	extern "C" __declspec(dllexport) olapi::OLStatus __cdecl OLWaitServicesCommunicating(DWORD waitTimeMs = OL_MINIMUM_WAIT_SERVICES_TIME);

	///
	/// \brief Start running the OnLive Services and begin establishing communication with the OSP.
	///
	/// After initialization of individual OnLive APIs, call OLStartServices to begin connecting to the OSP.
	/// Upon success, the OnLive services are running, but may not be communicating (connected) yet.
	/// The SDK may begin dispatching callbacks at any point after OLStartServices returns.
	///
	/// You may call \ref olapi::OLWaitServicesCommunicating to block until the connection to the OSP is
	/// established and the services are communicating.
	///
	/// NOTE: If you call functions that make OSP requests before we are communicating with the OSP,
	///		the requests are queued.
	///
	/// If you fail to connect to the OSP, your title should destroy all OnLive APIs and terminate.
	///
	///	\see OLSERVICE_API_CONNECTION_DISCONNECTION.
	///
	///	\return Returns An OLStatus code.
	///		\retval OL_SUCCESS	Success; you have connected to the OSP.
	///		\retval OL_FAIL		Cannot connect to the OnLive Service Platform; your title should exit.
	///
	extern "C" __declspec(dllexport) olapi::OLStatus __cdecl OLStartServices();

	///
	/// \brief Stop communication with the OSP and stop the running services.
	///
	/// Once you have stopped the OnLive services, you will no longer receive callbacks
	/// and will not be able to process events.  This call will destroy any remaining
	/// containers.
	///
	///	\see OLSERVICE_API_CONNECTION_DISCONNECTION for information about connection and disconnection.
	///
	///	\return Returns an OLStatus code.
	///		\retval OL_SUCCESS	Success; you have disconnected from the OSP.
	///		\retval OL_FAIL		Cannot disconnect from the OSP (you were not connected to begin with)
	///
	extern "C" __declspec(dllexport) olapi::OLStatus __cdecl OLStopServices();

	// 

	///
	/// \ingroup OLSERVICE_API_CONTEXT
	/// \brief Returns the next available OLContext value to use in an async request.
	///
	/// See \ref EVENTS_PAGE_ASYNC_COMM.
	///
	///	\return Returns the next available OLContext value.
	///
	extern "C" __declspec(dllexport) olapi::OLContext __cdecl OLGetNextContext();

	///
	/// \ingroup OLSERVICE_API_CONTEXT
	/// \brief Reserves and returns the next available OLContext value to use in repeated async request.
	/// 
	/// OLReserveNextContext Is used to aquire and reserve an OLContext so that it can be used over a long period
	/// of time.  OLGetNextContext can reissue contexts, but will not issue a context that has been reserved.
	/// Context can either be freed with a call to OLReleaseContext, or it will be automatically cleaned when the 
	/// duration ends.
	///
	/// *NOTE*: this funtion will return OL_INVALID_CONTEXT if you call it with an invalid duration.
	///
	///		\param[in] duration		When the context can be automatically cleaned. 
	///								Allowed values: OL_DURATION_AUTO, OL_DURATION_APP_SESSION, OL_DURATION_USER_SESSION
	///
	/// See \ref EVENTS_PAGE_ASYNC_COMM.
	///
	///	\return Returns the next available OLContext value.  This function can return OL_INVALID_CONTEXT if an invalid
	///					duration is provided
	///
	extern "C" __declspec(dllexport) olapi::OLContext __cdecl OLReserveNextContext(olapi::OLContextDuration duration);

	///
	/// \ingroup OLSERVICE_API_CONTEXT
	/// \brief Checks if a context is reserved and returns the its duration if it is.
	///
	/// Checks if a context is reserved and returns the its duration if it is. Returns OL_DURATION_NONE if not reserved.
	///
	/// See \ref EVENTS_PAGE_ASYNC_COMM.
	///
	///		\param[in] context		The context to check.
	///
	///	\return Returns duration for given context.
	///
	extern "C" __declspec(dllexport) olapi::OLContextDuration __cdecl OLIsContextReserved(olapi::OLContext context);

	///
	/// \ingroup OLSERVICE_API_CONTEXT
	/// \brief Changed the auto destroy time for the reserved context.
	///
	/// Checks if a context is reserved and changes the duration of the reservation to the new duration.
	///
	/// See \ref EVENTS_PAGE_ASYNC_COMM.
	///
	///		\param[in] context		The context to change.
	///
	///		\param[in] duration		New duration for the context. 
	///								Only OL_DURATION_USER_SESSION and OL_DURATION_APP_SESSION are accepted.
	///
	///	\return Returns An OLStatus code.
	///		\retval OL_INVALID_PARAM			invalid param.
	///		\retval OL_IDENTIFIER_NOT_FOUND		Context is not currently reserved.
	///     \retval OL_SUCCESS					auto destroy time was updated.
	extern "C" __declspec(dllexport) olapi::OLStatus _cdecl OLChangeContextDuration(olapi::OLContext context, olapi::OLContextDuration duration);

	///
	/// \ingroup OLSERVICE_API_CONTEXT
	/// \brief Unreserves the given context.
	///
	/// See \ref EVENTS_PAGE_ASYNC_COMM.
	///
	///		\param[in] context		The context to free.
	///
	///	\return Returns An OLStatus code.
	///		\retval OL_SUCCESS	Successfuly released the context.
	///		\retval OL_INVALID_PARAM			invalid param.
	///		\retval OL_FAIL		Could not free the context because it was not reserved.
	///
	extern "C" __declspec(dllexport) olapi::OLStatus __cdecl OLReleaseContext(olapi::OLContext context);

	///
	/// \ingroup OLSERVICE_API_CONTEXT
	/// \brief Unreserves all contexts of a given duration.
	///
	/// See \ref EVENTS_PAGE_ASYNC_COMM.
	///
	///		\param[in] duration		The duration which has expired.
	///
	///
	extern "C" __declspec(dllexport) void __cdecl OLReleaseDurationContexts(olapi::OLContextDuration duration);

	///
	/// \brief Helper that checks if we are in an active application or user session.
	///
	/// We do not expect SDK users to call this function often; it is available, but mostly for
	/// internal use.
	///
	///	Note: It is not possible for a user session to exist outside of an application session.
	///
	///		\param[in] userSession	If true, we check for both an application and user session.\n
	///								If false, we only check for an application session.
	///
	///		\return Returns true if we are in the specified type of session.
	///
	extern "C" __declspec(dllexport) bool __cdecl OLCheckSession(bool userSession);



	/// \addtogroup OLSERVICE_API_POLLING
	/// 

	///
	/// \brief Polls each OnLive API and handles as many waiting events as possible within the specified time limits.
	///
	///	Only call this method if you plan to poll for events (see \ref EVENTS_PAGE_HANDLING_OVERVIEW and \ref EVENTS_PAGE_HANDLING_POLLING).
	///	Use \ref olapi::OLCancelHandleEvents if you need to bail out of a call to OLHandleEvents from within a handled event.
	///
	/// Note: The supplied time limits may not be strictly honored in all cases; once OLHandleEvents dispatches an event,
	///   it has to let the event handler method complete (even if it pushes us over the max_milliseconds time limit).
	///
	///		\param[in] min_milliseconds	The min amount of time this function blocks for (waiting for events to arrive).
	///										If zero, this function returns immediately when no events are waiting.
	///
	///		\param[in] max_milliseconds	The max amount of time this function will process events for.
	///
	///		\return 	true: Processed all available messages.\n
	///					false: A timeout or cancel occurred (possibly leaving available messages unprocessed).
	///
	extern "C" __declspec(dllexport) bool __cdecl OLHandleEvents( unsigned int min_milliseconds, unsigned int max_milliseconds );

	///
	/// \brief Stops \ref olapi::OLHandleEvents() from processing any more events.
	///
	/// OLHandleEvents usually processes as many available events within a given time limit.
	/// Call this function from within an event to force OLHandleEvents to stop after the current event.
	///
	///		\return  Returns true if more events are available to process.
	///
	extern "C" __declspec(dllexport) bool __cdecl OLCancelHandleEvents();

	//

	/// \addtogroup OLSERVICE_API_CONTAINERS
	///

	///
	/// \brief Parse an olapi-formatted timestamp into a time_t.
	///
	///		\param[in] timestamp	An OnLive time string value.
	///
	///		\return Returns the time_t version of the supplied time string.
	///
	extern "C" __declspec(dllexport) time_t __cdecl OLParseTimestamp(const char* timestamp);

	///
	/// \brief Function pointer for iterating over a container value that is a vector-of-strings.
	///			See \ref olapi::OLGetContainerVectorValue.  Not used by a public API yet.
	///
	typedef void (CALLBACK* PVECTORCALLBACK)(const char*, unsigned int context);

	///
	/// \brief Get a vector of strings from a container and iterate over it, 
	///		calling the supplied function for each string in the vector.  Not used by a public API yet.
	///
	/// If the vector contains objects instead of strings, they are considered as a single item in the vector.
	/// If a string vector element is too large (larger than the supplied size), it is skipped over silently.
	///
	///		\param[in] containerID		ID of the container to read from.
	///
	///		\param[in] keyName			The string keyName of the value to retrieve (see \ref CONTAINERS_PAGE_USING_KEYS).
	///
	///		\param[in] size				Strings in the vector larger than this size are skipped over.
	///
	///		\param[in] callback			Your function pointer that is called on the vector entries.
	///
	///		\param[in] context			A user defined 'context' to tie this function call to the function pointer invocations.
	///									(it is typically set to 'this' for c++ methods).
	///
	///	\return Returns an OLStatus code.
	///		\retval OL_SUCCESS					Success; the container's vector value has been iterated over.
	///		\retval OL_INVALID_PARAM			A supplied param was invalid (the containerId is the wrong type, the keyName is NULL, the size is less or equal to zero, or the buffer is NULL).
	///		\retval OL_CONTAINER_NOT_FOUND		The container itself was not found (wrong containerId).
	///		\retval OL_IDENTIFIER_NOT_FOUND		The keyName was not found in the container.
	///		\retval OL_INTERNAL_ERROR			The container value is not a properly formatted vector-of-strings.
	///		\retval OL_FAIL						An instance of OLService has not been initialized yet.
	///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
	///
	extern "C" __declspec(dllexport) olapi::OLStatus OLGetContainerVectorValue(olapi::OLID containerID, const char *keyName, int size, PVECTORCALLBACK callback, unsigned int context);

	///
	/// \brief Function pointer for iterating over a container value that is a vector-of-objects.
	///			See \ref olapi::OLGetContainerObjectFromValue.  Not used by a public API yet.
	///
	typedef void (CALLBACK* POBJECTCALLBACK)(const char*, int index, unsigned int context);


	///
	/// \brief Iterate a vector-of-objects (retrieved from a container as a string), 
	///			and call the supplied function pointer for each object in the vector.  Not used by a public API yet.
	///
	/// If a serialized obj string (vector element) is too large (larger than the supplied size), it is skipped over silently.
	///
	///		\param[in] objectVectorString		A 'vector-of-objects' that was serialized into a container string.
	///
	///		\param[in] size				If the serialized object (in string form) is larger than this size, we skip it.
	///
	///		\param[in] callback			Your function pointer that is called on each vector object entry.
	///
	///		\param[in] context			A user defined 'context' to tie this function call to the function pointer invocations.
	///									(it is typically set to 'this' for c++ methods).
	///
	///	\return Returns an OLStatus code.
	///		\retval OL_SUCCESS					Success; the vector has been iterated over.
	///		\retval OL_INVALID_PARAM			A supplied param was invalid (the objectVectorString is NULL or the size is equal or less than zero).
	///		\retval OL_INTERNAL_ERROR			The container value is not a properly formatted vector-of-objects.
	///		\retval OL_FAIL						An instance of OLService has not been initialized yet.
	///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
	///
	extern "C" __declspec(dllexport) olapi::OLStatus OLGetContainerObjectFromValue(char *objectVectorString, int size, POBJECTCALLBACK callback, unsigned int context);


	///
	/// \brief Set the OSP test flag 
	///
	/// This function is for internal SDK usage only.
	///
	///		\param flag				
	///
	///		\retval OL_SUCCESS					Success;
	///		\retval OL_INVALID_PARAM			The supplied param was invalid (flag is NULL).
	///		\retval OL_FAIL						An instance of OLService has not been initialized yet.
	///
	extern "C" __declspec(dllexport) olapi::OLStatus __cdecl OLSetTestFlag(char* flag);

	///

}  // namespace olapi

// doxygen group comments
/*! 
\addtogroup SERVICE
	\brief \copybrief olapi::OLService

\addtogroup OLSERVICE_API_CREATE_AND_DESTROY
	\brief The OLService singleton is created and retrieved with \ref olapi::OLGetService() and destroyed 
			with \ref olapi::OLStopService(). Do not destroy any of the OnLive APIs (or their handlers) until 
			after you have stopped the OnLive Services. 
	
	See \ref SDK_SESSION_FLOW_PAGE for information about the session flow.

	See \ref OLSERVICE_API_CONNECTION_DISCONNECTION for information about OSP connection and disconnection.

\addtogroup OLSERVICE_API_LOGGING
	\brief The methods below allow your title to send messages to the OSP log (SDK log) and register to receive OSP log messages
	(which you can echo to your own logging system). 
	
	The enum, function, and logging methods described below are used for the OLService logging system.

	\par Log Rate limits
		Do not exceed an average of 1 log per second for OLLOG_DEBUG messages.\n
		Do not exceed an average of 1 log per minute for other log levels.\n
		Titles will be rejected by OnLive certification for logging abuse.

	OnLive recommends that you remove all but the most important logging calls from
		your title's final submission builds.

	\warning Do not call \ref OLService::Log from within \ref OLServiceEventHandler::dbg_SDKLogReceived or you can start an infinite log 
	message spam loop.  OLService::Log can trigger a dbg_SDKLogReceived event (if the event is enabled and the message passes the current log level).

	There are a handful of other useful logging methods, most of which convert an enum value into a string (these are examples, not an exhaustive list):
	<ul>
	<li> \ref olapi::OLStatusToString
	<li> \ref olapi::OLResolutionToString
	</ul>

\addtogroup OLSERVICE_API_CONTEXT
	\brief An \ref olapi::OLContext is a number that allows you to pair an SDK request to its async response event.  When you make 
	an async request, one of the arguments you must supply is the OLContext for the event.  The SDK stores the context for the 
	duration of the async request, echoing the value back to you in the eventual async response event. Use \ref olapi::OLGetNextContext() 
	to generate a new context for each request. Unsolicited events use \ref OL_UNSOLICITED_CONTEXT as the context value in an event dispatch.
	
	The define directive, typedef, and function described below are used for the OLContext system.
	

	See \ref EVENTS_PAGE_ASYNC_COMM for information about asynch communication.

\addtogroup OLSERVICE_API_CONNECTION_DISCONNECTION
	\brief Starting and stopping the "OnLive Services" is analogous to connecting and disconnecting to the OSP backend.
	The SDK only dispatches API callbacks while the OnLive Services are running.  Do not start the OnLive Services until you have created and initialized each of the OnLive APIs that you wish to use
	(getting their singleton instances and registering any callback and event handlers for the APIs). Once the OnLive Services are 
	running, you may begin issuing requests and handling events (requests are queued
	until the services are communicating). 
	
	The functions described below are used for communication verification, and the starting and stopping of the OnLive Services.


	See \ref SDK_SESSION_FLOW_PAGE for info about the application session flow sequence.

	

\addtogroup OLSERVICE_API_EVENT_HANDLING
	\brief Callbacks and events are used for asynchronous communication to and from the OSP.

		The classes and functions described below are used for registering and handling OLService callbacks and events.
	
	See \ref SDK_EVENTS_PAGE for an overview of callbacks and events.

\addtogroup OLSERVICE_API_CONTAINERS
	\brief A container is a collection of related key-value pairs. Many of the APIs use containers to return data. This API contains the functions 
	to access the data within the containers.
	 
	
	See \ref SDK_CONTAINERS_PAGE for information about containers.

\addtogroup OLSERVICE_API_SDKINFO_EVENT
	\brief \copybrief olapi::OLServiceEventHandler::SDKInfoContainerIDReceived()

	See \ref SDK_CONTAINERS_PAGE for information about containers.

\addtogroup OLSERVICE_API_DBG_TEST_HARNESS_EVENT
	\brief \copybrief olapi::OLServiceEventHandler::dbg_IsTestHarness()

*/

#endif // OLSERVICE_H

