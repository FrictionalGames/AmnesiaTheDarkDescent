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
// OLContent.h $Revision: 60760 $
//

/// \file OLContent.h

#ifndef OLCONTENT_H
#define OLCONTENT_H

#include <ol_api/OLAPI.h>

namespace olapi
{
	/*! \ingroup OLCONTENT_API_CREATE_AND_DESTROY 
		\brief The default version of the OnLive OLContent API to use in \ref olapi::OLGetContent.
	*/
	#define CONTENT_API_VERSION (OLAPI_VERSION)

	/// \addtogroup OLCONTENT_ADDON_INFO_CONTAINER
	/// 

	/*! \brief The name of the ContentAddonInfo Container; see \ref OLCONTENT_ADDON_INFO_CONTAINER */
	#define CONTENTADDONINFOCONTAINERNAME	"ContentAddonInfo"


	///
	/// \brief Used to index into the \ref olapi::ContentAddonInfoContainerKeyNames array.
	///
	/// Note: The deprecated keys should not be used with containers; the container values may not be legitimate.
	///
	///	Also see:
	/// <ul>
	///		<li> \ref OLCONTENT_ADDON_INFO_CONTAINER
	///		<li> \ref CONTAINERS_PAGE_USING_KEYS.
	/// </ul>
	enum ContentAddonInfoContainerKeys
	{
		OLKEY_RO_CA_ContentAddonInfoName,				///< string, read only,  container name; see \#define \ref CONTENTADDONINFOCONTAINERNAME.
		OLKEY_RO_CA_Content_ID,							///< OLID, read only,  content ID of the Content Add-on.
		OLKEY_RO_CA_Content_Title,						///< string, read only,  title of the content.
		OLKEY_RO_CA_Content_Description,				///< string, read only,  description of the content; the maximum allowable size is \#define \ref OL_MAX_VALUE_DESCRIPTION_SIZE.
		OLKEY_RO_CA_Content_Summary,					///< string, read only,  short description of the content.
		OLKEY_RO_CA_ContentAddonInfoKeysMax
	};


	///
	/// \brief Array of keyName strings for the ContentAddonInfo container, indexed by the \ref ContentAddonInfoContainerKeys enum.
	///
	///Also see:
	/// <ul>
	///		<li> \ref OLCONTENT_ADDON_INFO_CONTAINER
	///		<li> \ref CONTAINERS_PAGE_USING_KEYS.
	/// </ul>
	///
	#ifndef NO_OLAPI_DLL_LINKAGE
	extern __declspec(dllimport) char *ContentAddonInfoContainerKeyNames[OLKEY_RO_CA_ContentAddonInfoKeysMax];
	#endif

	/// 

	class OLContentEventHandler;
	class OLContentEventWaitingCallback;


	///
	/// \ingroup OLCONTENT_API 
	/// \class OLContent
	/// \brief  The OLContent API gives your title access to user purchasable content. The content is accessed using a Content Add-on ID to 
	///			identify your title's content from other parts of the OnLive system while the Add-on Info Container holds the information 
	///			about the Content Add-on, such as the title, description, summary, and more. 
	///
	/// \par API Initialization
	///		The OLContent singleton is created and retrieved with \ref olapi::OLGetContent() and destroyed 
	///		with \ref olapi::OLStopContent().\n\n
	///		NOTE: OLContent depends on the OLService API and should be initialized after it. \n
	///		See \ref OLCONTENT_API_CREATE_AND_DESTROY and \ref SDK_SESSION_FLOW_PAGE for general info about 
	///		initializing the OnLive SDK APIs.
	///
	/// \par API Event and Callback Handling
	///		See \ref SDK_EVENTS_PAGE for an overview or \ref OLCONTENT_API_EVENT_HANDLING for specifics.
	///
	///
	class OLContent : public OLEventBase
	{
		public:
		/// \addtogroup OLCONTENT_API_EVENT_HANDLING
		/// 
		/// \name OLContent Callbacks & Event Handling
		/// 


		/// 
		/// \brief Registers your callback handler object with the OLContent API.
		///
		/// If you intend to handle callbacks for OLContent, you should set
		/// a callback handler object before you start the OnLive Services with \ref olapi::OLStartServices.
		/// Once services are started, callbacks can be dispatched by the SDK at any time.
		///
		/// OLContent stores a single callback object pointer; calling this method
		/// multiple times will replace the previous pointer.  There is no way to 'unregister'
		/// the pointer (you cannot set it to NULL).
		///
		///	See \ref EVENTS_PAGE_REGISTRATION for details.
		///
		///		\param[in]	callbackHandler	Your implementation of the \ref OLContentEventWaitingCallback interface.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			Normal operation; callbacks can be dispatched on the supplied object while
		///										the services are running.
		///		\retval OL_INVALID_PARAM	Error - The supplied callback object pointer was NULL (or invalid);
		///										the callback obj has not been set.
		///
		virtual OLStatus SetEventWaitingCallback(OLContentEventWaitingCallback* callbackHandler) = 0;


		///
		/// \brief Registers your eventHandler object with the OLContent API.
		///
		/// Once you have set the OLContent API's event handler, calling one of the OLContentEventWaitingCallback methods
		///		may dispatch events on your object (assuming events are available to process).
		///		See \ref EVENTS_PAGE_HANDLING_OVERVIEW.
		///
		/// OLContent stores a single event handler pointer; calling this method
		/// multiple times will replace the previous pointer.  There is no way to 'unregister'
		/// the pointer (you cannot set it to NULL).  
		///
		///	See \ref EVENTS_PAGE_REGISTRATION for details.
		///
		///		\param	eventHandler		Your implementation of the OLContentEventHandler interface.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			Normal operation; events will be dispatched on the supplied obj.
		///		\retval OL_INVALID_PARAM	Error - The supplied event handler pointer was NULL (or invalid);
		///										the event handler has not been set.
		///
		virtual OLStatus SetEventHandler(OLContentEventHandler* eventHandler) = 0;


		///
		/// \brief Sets this API's event handler and attempts to handle a single event. NOTE: Prefer to use the simpler
		///  overload \ref HandleEvent(unsigned int).
		///
		/// This method acts as a combination of \ref SetEventHandler() and \ref HandleEvent(unsigned int).
		/// If the supplied event handler is not NULL, we set the event handler to the supplied value, then try to handle a single event.
		/// If the supplied event handler is NULL, we try to handle a single event using the previously set handler.
		///
		/// See \ref SDK_EVENTS_PAGE for details.
		///
		///		\param[in]	eventHandler	If NULL, we use the last set event handler.
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
		///		\retval OL_EVENT_TIMEOUT	No events were available for the duration of the wait.
		///
		virtual OLStatus HandleEvent(OLContentEventHandler* eventHandler, unsigned int waitTimeMs) = 0;


		///
		/// \brief Attempts to dispatch a single event on this API's registered OLContentEventHandler object.
		///
		/// You must set an OLContentEventHandler instance with \ref OLContent::SetEventHandler() before
		///		calling HandleEvent().
		///
		/// See \ref SDK_EVENTS_PAGE for details.
		///
		///		\param[in]	waitTimeMs	The max amount of time to wait for an event in milliseconds.
		///										Use 0 to return immediately if there is no event waiting (non-blocking).
		///										Otherwise, this call may block for up to waitTimeMs if no event is available.
		///										You can use \ref OL_INFINITE to wait forever for the next event.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			An event was dispatched.
		///		\retval OL_INVALID_PARAM	The API does not have an event handler set (see \ref OLContent::SetEventHandler).
		///		\retval OL_EVENT_TIMEOUT	No events were available for the duration of the wait.
		///
		virtual OLStatus HandleEvent(unsigned int waitTimeMs) = 0;

		/// 

		/// 
	
		/// \ingroup OLCONTENT_API_ADDON_ID
		/// \brief Requests user purchased Content Add-on IDs from the OSP. The OSP will respond with either an IDListResult 
		///	event on success or return an error status on failure.	
		///
		///	The Content Add-on ID will be given by your OnLive Account Manager prior to your title submission; your title 
		///	should map this ID to the additional content your title is going to support.
		///
		/// Your title must call this function at the begining of the user session and load/enable all
		/// required content for the current user; specifically, after receiving a BindUser() event
		/// and before your title advances the login state to OL_LOGIN_READY_FOR_CLIENT. 
		/// See \ref SDK_SESSION_FLOW_SECTION_USER_BINDING.
		///
		/// \responseEventList
		///		This function triggers the event dispatch immediately.
		///		The SDK responds with the following event if this request is successfully made: 
		/// 
		///		\ospEvent OLContentEventHandler::IDListResult
		///			\eventDesc	Returns a list of user purchased Content Add-on IDs.
		///	
		///
		/// \responseEventListEnd
		///
		///		\param[in]	context			Used to pair this request with one of the two event responses (see \ref olapi::OLGetNextContext).
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					An event was dispatched.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///		\retval OL_OUT_OF_MEMORY			Failed to allocate space for the ID list.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///
		virtual OLStatus GetUserPurchasedContentAddonIDs( OLContext context ) = 0;


		///
		/// \brief Requests the information of multiple Content Add-on's from the OSP.  The OSP will respond
		///		with either an IDListResult event on success, or a StatusResult event on failure.
		///
		///
		/// \responseEventList
		///		The OSP responds with one of the following events if this request is successfully sent: 
		/// 
		///		\ospEvent OLContentEventHandler::IDListResult
		///			\eventDesc	Returns the ID of the UserSystemSettings Container.
		///						Note: The SDK Test Harness does not populate this container
		///						(you will get a container with empty or default values).
		///
		///		\ospEvent OLContentEventHandler::StatusResult
		///			\eventDesc	 The StatusResult event indicates one of the following errors:
		///
		///				\statusResult OL_INTERNAL_ERROR		Generic OSP failure.
		///				\statusResult OL_BUSY				OSP is still processing the previous call.
		///				\statusResult OL_OUT_OF_MEMORY		Failed to allocate space for the container.
		///
		/// \responseEventListEnd
		///
		///		\param[in]	context		Used to pair this request with one of the two event responses (see \ref olapi::OLGetNextContext).
		///
		///		\param[in]	contentIDs	A list of Content Add-on IDs to retrieve information from. (see \ref olapi::ContentAddonInfoContainerKeys for available info for each Content Add-on).
		///
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					An event was dispatched.
		///		\retval OL_INVALID_PARAM			contentID is not a valid Content Add-on ID.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///
		virtual OLStatus GetContentAddonInfo( OLContext context, OLIDList *contentIDs ) = 0;
	};


	///
	/// \ingroup OLCONTENT_API_EVENT_HANDLING OLCONTENT_API
	/// \class OLContentEventHandler
	/// \brief This is the OLContent API's Event interface; your OLContent event handler must implement this interface.
	///		Each method represents an asynchronous event returned by the SDK.
	///
	/// See \ref EVENTS_PAGE_CONCEPTS for an overview of event handling.
	///
	class OLContentEventHandler
	{
		public:


		///
		/// \brief Event returning an OLStatus code result; this typically means an error was
		///		encountered while the OSP was processing an async request.
		///
		/// For example, if the title requests user purchased content IDs from the OSP
		///	(see \ref OLContent::GetUserPurchasedContentAddonIDs), and the OSP encounters an error, 
		/// the OSP sends a StatusResult event with \ref OL_INTERNAL_ERROR as the status.
		///
		/// See \ref EVENTS_PAGE_ASYNC_COMM.
		///
		///		\param[in] context		The matching context you gave to the request for this result (see \ref EVENTS_PAGE_ASYNC_COMM).
		///
		///		\param[in] status		An OLStatus code indicating the result of your request (typically an error).
		///
		///	\return An OLStatus code indicating if you have handled the event.
		///		\retval OL_SUCCESS				The event was handled.
		///		\retval OL_NOT_IMPLEMENTED		The event was ignored.
		///		\retval OL_FAIL					The event was not handled.
		///
		virtual OLStatus StatusResult(OLContext context, OLStatus status) = 0;


		///
		/// \brief Event returning a list of OnLive IDs.
		///
		/// The actual IDs in the list will depend on the type of request you made.
		/// For example, if this event is in response to a \ref olapi::OLContent::GetUserPurchasedContentAddonIDs request,
		/// the list will contain Content Add-on IDs ( \ref OL_CONTENT_ID ).
		///
		/// See \ref OLIDList for an example of iterating over an ID list.
		///
		///		\param[in] context		The matching context you gave to the request for this result (see \ref EVENTS_PAGE_ASYNC_COMM).
		///
		///		\param[in] idList		The list of IDs; the list is invalidated when this method returns, so save off any needed data.
		///
		///	\return Returns an OLStatus code to the SDK.
		///		\retval OL_SUCCESS				The event was handled.
		///		\retval OL_NOT_IMPLEMENTED		The event was ignored.
		///
		virtual OLStatus IDListResult(OLContext context, OLIDList *idList) = 0;
	};


	///
	/// \ingroup OLCONTENT_API_EVENT_HANDLING OLCONTENT_API
	/// \class OLContentEventWaitingCallback
	/// \brief This is the OLContent API's callback interface; your title's OLContent callback handler must implement this interface.
	///
	/// Each method in this class represents an OLContent callback.  If you choose to handle OLContent callbacks,
	/// register your callback handler using \ref OLContent::SetEventWaitingCallback.  Callback methods
	/// may be invoked at any time once the OnLive Services are running (see \ref OLSERVICE_API_CONNECTION_DISCONNECTION).
	///
	/// \note Callback methods are always invoked from an internal OnLive SDK thread, so all callback implementations must be thread-safe.
	///
	/// See \ref EVENTS_PAGE_CONCEPTS for details.
	///
	class OLContentEventWaitingCallback
	{
		public:


		///
		/// \brief Callback dispatched when an OLContent event is waiting to be handled.
		///
		/// If your application handles events from callbacks, call \ref OLContent::HandleEvent(unsigned int) to dispatch the waiting event.
		///	If you handle events by polling, you can ignore this event.
		///
		/// \note The EventWaiting callback must be thread-safe and must complete in less than 2 milliseconds.
		///
		/// See \ref EVENTS_PAGE_HANDLING_OVERVIEW for more info.
		///
		virtual void EventWaiting() = 0;
	};

	/// \addtogroup OLCONTENT_API_CREATE_AND_DESTROY
	/// 

	///
	/// \brief Get the OLContent API singleton (the instance is created if necessary).
	///
	///	The OLContent pointer can be stored until the API is destroyed with \ref olapi::OLStopContent().
	///
	/// See \ref OLCONTENT_API_CREATE_AND_DESTROY.
	///
	///		\param[in] version		The API version string (for class compatibility), use \ref CONTENT_API_VERSION.
	///
	///		\param[out] olContent	The supplied pointer is set to the OLContent instance on success.
	///									The pointer is not modified on failure.
	///
	///	\return An OLStatus code.
	///		\retval	OL_SUCCESS			Success; the olContent pointer is set to the singleton.
	///		\retval OL_INVALID_PARAM	The supplied version string or olContent pointer is NULL.
	///		\retval OL_INVALID_VERSION	The API version requested is not supported. This is a fatal 
	///										error and your title should exit.
	///		\retval OL_INTERNAL_ERROR	A general internal or system error.
	///		\retval OL_OUT_OF_MEMORY	The SDK could not alloc memory for the singleton.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLGetContent(const char* version, OLContent** olContent);


	///
	/// \brief Destroy the OLContent API singleton.
	///
	/// The OnLive Services should be stopped before you destroy an API instance; see \ref olapi::OLStopServices().
	///
	/// See \ref OLCONTENT_API_CREATE_AND_DESTROY.
	///
	///		\param[in] olContent		Pointer to the OLContent instance to destroy.
	///
	///	\return Returns an OLStatus code.
	///		\retval	OL_SUCCESS			Success; the olContent was destroyed.
	///		\retval OL_INVALID_PARAM	The supplied olContent pointer was NULL.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLStopContent(OLContent* olContent);

	/// 


/*!

\addtogroup OLCONTENT_API
	\brief \copybrief olapi::OLContent

\addtogroup OLCONTENT_API_CREATE_AND_DESTROY
	\brief The OLContent singleton is created and retrieved with \ref olapi::OLGetContent() and destroyed with \ref olapi::OLStopContent().	Do not 
	destroy any of the OnLive APIs (or their handlers) until after you have stopped the OnLive Services. 

	Also see:
	 <ul>
		<li> \ref SDK_SESSION_FLOW_PAGE for information about the session flow.
		<li> \ref OLSERVICE_API_CONNECTION_DISCONNECTION for information about OSP connection and disconnection.
	 </ul>
		
\addtogroup OLCONTENT_API_EVENT_HANDLING
	\brief Callbacks and events are used for asynchronous communication to and from the OSP. 

	See \ref SDK_EVENTS_PAGE for an overview of callback and events.

\addtogroup OLCONTENT_API_ADDON_ID
	\brief The OnLive Content Add-on ID is an OLID with type \ref olapi::OL_CONTENT_ID; 
	it is used to identify your title's content from other parts of the OnLive system.
	The ID will be provided by your OnLive Account Manager prior your title submission; your title 
	should map this ID to the additional content your title is going to support. 
	
	To retrieve content IDs that a user has purchased, the title should call \ref olapi::OLContent::GetUserPurchasedContentAddonIDs().
	The result will be returned to \ref olapi::OLContentEventHandler::IDListResult() as a list of Content Add-on IDs if the function call is
	successful; otherwise, an error code will be returned to \ref olapi::OLContentEventHandler::StatusResult().
	
\addtogroup OLCONTENT_ADDON_INFO_CONTAINER
	\brief The Content Add-On Info Container holds information about the content including the ID, title, description, summary, and more.
	To retrieve information that is associated with the content, a title should call \ref olapi::OLContent::GetContentAddonInfo(). 
	The result will be returned to \ref olapi::OLContentEventHandler::IDListResult() as a list of containers if the function call success; 
	otherwise, an error code will be returned to \ref olapi::OLContentEventHandler::StatusResult().
	The keys that are specified in each container are defined in \ref  olapi::ContentAddonInfoContainerKeyNames[], which is 
	accessed by \ref olapi::ContentAddonInfoContainerKeys. 

	Also, see \ref SDK_CONTAINERS_PAGE for an overview of OnLive SDK Containers.

*/

}; // namespace olapi

#endif // OLCONTENT_H
