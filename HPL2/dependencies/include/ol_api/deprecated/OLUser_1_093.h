/*************************************************************************************
			Copyright(c)   2009 - 2010 OnLive, Inc.  All Rights Reserved.

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
// OLUser.h $Revision: 63117 $
//

/// \file OLUser.h

#ifndef OLUSER_H
#define OLUSER_H

#include "OLAPI.h"

namespace olapi
{

	/// \addtogroup OLUSER_API_PRIVACY_STATUS
	/// 

	/*! \brief Returns true if the supplied game privacy flag is set.

		\param[in] olGameStatus		The status flagset from the \ref olapi::OLUserSessionEventHandler::GamePrivacyStatusResult
										event (requested with \ref olapi::OLUserSession::GetGamePrivacyStatus).
		\param[in] flag				An \ref olapi::OLGameStatus enum flag to test against the status value.
		\return true if the flag is set in the supplied olGameStatus.
	*/
	#define IsGamePrivacyFlagSet(olGameStatus, flag) ((olGameStatus) & (flag))

	///
	/// \brief These GameStatus privacy flags allow runtime control over Brag Clips and Game Spectating.
	///
	///	See \ref OLUserSession::GetGamePrivacyStatus, \ref OLUserSessionEventHandler::GamePrivacyStatusResult,
	///		and \ref OLUserSession::SetGamePrivacyStatus.
	///
	enum OLGameStatus
	{
		OL_GAME_PUBLIC						= 0,	///< public games allow anyone to spectate and take Brag Clips.
		OL_GAME_SPECTATING_PRIVATE			= 1,	///< disable spectating entirely.
		OL_GAME_SPECTATING_PRIVATE_AUDIO	= 2,	///< disable spectating audio (video is available).
		OL_GAME_BRAG_CLIP_PRIVATE			= 4,	///< disable Brag Clip captures entirely.
		OL_GAME_BRAG_CLIP_PRIVATE_AUDIO		= 8,	///< disable Brag Clip audio capture (video is available).

		OL_GAME_STATUS_MASK = OL_GAME_SPECTATING_PRIVATE | OL_GAME_SPECTATING_PRIVATE_AUDIO | OL_GAME_BRAG_CLIP_PRIVATE | OL_GAME_BRAG_CLIP_PRIVATE_AUDIO,
	} ;

	/// 

	/// \addtogroup OLUSER_API_SESSION_STATUS_CONTAINER
	/// 

	/*! \brief The name of the UserSessionStatus container; see \ref OLUSER_API_SESSION_STATUS_CONTAINER. */
	#define USERSESSIONSTATUSCONTAINERNAME	"UserSessionStatus"

	///
	/// \brief Identifies how the title was launched (as a timed demo, game rental, or purchased game).
	///
	/// See \ref OLUSER_API_SESSION_STATUS_CONTAINER
	///
	enum OLLicenseType
	{
		OL_LICENSE_TYPE_UNKNOWN = -1,				///< Unknown.
		OL_LICENSE_TYPE_GAME,						///< This game is owned by the user, it is a full game license.
		OL_LICENSE_TYPE_DEMO,						///< This game is a demo.
		OL_LICENSE_TYPE_RENTAL,						///< This game is rented by the user, it is a full game license.
	};

	///
	/// \brief Used to index into the \ref olapi::UserSessionStatusContainerKeyNames array.
	///
	/// Note: you should not use the deprecated keys with containers; the container values may not be legitimate.
	///
	/// See \ref OLUSER_API_SESSION_STATUS_CONTAINER and \ref CONTAINERS_PAGE_USING_KEYS.
	///
	enum UserSessionStatusContainerKeys
	{
		OLKEY_RO_UserSessionStatusName,				///< string, read only, Name of the container (see \#define \ref USERSESSIONSTATUSCONTAINERNAME).
		OLKEY_RO_MasterSessionID,					///< OLID,   read only, Session ID for the user when connected to OnLive.
		OLKEY_RO_Tagname,							///< string, read only, The user's tag name (OnLive user name).  See \ref ONLIVE_TAG_PAGE.
		OLKEY_RO_UserID,							///< OLID,   read only, User ID of the current user.
		OLKEY_RO_Latency,							///< int,    read only, DEPRECATED - Latency between end user and the OnLiveGameServer machine in milliseconds.
		OLKEY_RO_Bandwidth,							///< int,    read only, DEPRECATED - Approximate bandwidth of user's connection to OnLive.
		// SDK Version 1.011 additions
		OLKEY_RO_UserSessionID,						///< OLID,   read only, Session ID of the currently bound user; same as \ref OLUserSession::GetSessionID.
		// SDK Version 1.063 additions
		OLKEY_RO_LicenseType,						///< int, cast to \ref olapi::OLLicenseType enum; indicates how the game was launched (as a demo, rental, or purchased title)
		OLKEY_RO_UserSessionStatusKeysMax			///< indicates the array size; not an index.
	};

	///
	/// \brief Array of keyName strings for the UserSessionStatus container, indexed by the \ref UserSessionStatusContainerKeys enum.
	///
	/// See \ref OLAPPSESSION_API_SESSION_STATUS_CONTAINER and \ref CONTAINERS_PAGE_USING_KEYS.
	///
	#ifndef NO_OLAPI_DLL_LINKAGE
	extern __declspec(dllimport) char *UserSessionStatusContainerKeyNames[OLKEY_RO_UserSessionStatusKeysMax];
	#endif

	// 

	/// \addtogroup OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER
	/// 

	///
	/// \brief The platform the end user is running OnLive from.
	///
	/// See the OLKEY_RO_Platform value in the UserSystemSettings Container
	/// and \ref OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER.
	///
	enum OLClientPlatform
	{
		OL_CLIENT_MICROCONSOLE = 0,					///< Client is OnLive MicroConsole.
		OL_CLIENT_WINDOWS,							///< Client is Windows.
		OL_CLIENT_MAC,								///< Client is Mac.
		// SDK Version 1.088 additions
		OL_CLIENT_IOS,								///< Client is IOS (iPad, etc.)
		OL_CLIENT_MAX
	};

	///
	/// \brief The number of audio channels and end user supports.
	///
	/// See the OLKEY_RO_AudioConfiguration value in the UserSystemSettings Container
	/// and \ref OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER.
	///
	enum OLAudio
	{
		OL_AUDIO_2_0 = 0,					///< Audio channel 2.0
		OL_AUDIO_5_1,						///< Audio channel 5.1
		OL_AUDIO_7_1,						///< Audio channel 7.1
		OL_AUDIO_MAX
	};


	#define LANGUAGE_ENGLISH	"english"			///< \brief OLKEY_RO_Langauge value for English (US) - see \ref OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER
	#define LANGUAGE_FRENCH		"french"			///< \brief OLKEY_RO_Langauge value for French (Standard) - see \ref OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER
	#define LANGUAGE_ITALIAN	"italian"			///< \brief OLKEY_RO_Langauge value for Italian (Standard) - see \ref OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER
	#define LANGUAGE_GERMAN		"german"			///< \brief OLKEY_RO_Langauge value for German (Standard) - see \ref OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER
	#define LANGUAGE_SPANISH	"spanish"			///< \brief OLKEY_RO_Langauge value for Spanish (Spain) - see \ref OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER
	#define LANGUAGE_JAPANESE	"japanese"			///< \brief OLKEY_RO_Langauge value for Japanese - see \ref OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER


	/*! \brief The name of the UserSystemSettings Container; see \ref OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER */
	#define USERSYSTEMSETTINGSCONTAINERNAME	"UserSystemSettings"


	///
	/// \brief Used to index into the \ref olapi::UserSystemSettingsContainerKeyNames array.
	///
	/// Note: you should not use the deprecated keys with containers; the container values may not be legitimate.
	///
	/// See \ref OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER and \ref CONTAINERS_PAGE_USING_KEYS.
	///
	enum UserSystemSettingsContainerKeys
	{
		OLKEY_RO_UserSystemSettingsName,			///< string, read only,  container name; see \#define \ref USERSYSTEMSETTINGSCONTAINERNAME.
		OLKEY_RO_TVOutputMode,						///< string, read only,  DEPRECATED - Current resolution of the console or PC. - Do not use.
		OLKEY_RO_Language,							///< string, read only,  user's preferred language; see the language \#defines in OLUser.h.
		OLKEY_RW_AudioVolume,						///< float,  read write, 0.0 to 1.0; the master volume level.
		OLKEY_RO_Region,							///< string, read only,  DEPRECATED - Current region in which the user is located (sku region, not country code).
		// SDK Version 1.009 additions
		OLKEY_RO_Platform,							///< int,    read only,  the end user's platform.  Cast the int to an \ref OLClientPlatform enum.
		OLKEY_RO_Keyboard,							///< int,    read only,  cast to bool, true means Keyboard is connected.
		OLKEY_RO_Mouse,								///< int,    read only,  cast to bool, true means Mouse is connected.
		OLKEY_RO_Gamepad,							///< int,    read only,  number of gamepads connected.
		// SDK Version 1.023 additions
		OLKEY_RO_AudioConfiguration,				///< int,	 read only,  cast to an \ref OLAudio enum; represents the end user's audio channel suport.
		// SDK Version 1.041 additions
		OLKEY_RO_VibrationSupported,				///< int,    read only,  cast to bool, true means vibration is supported in Gamepad driver.
		// SDK Version 1.088 additions
		OLKEY_RO_Touchpad,							///< int,    read only,  cast to bool, true means Mouse is connected.

		OLKEY_RO_UserSystemSettingsKeysMax
	};

	///
	/// \brief Array of keyName strings for the UserSystemSettings container, indexed by the \ref UserSystemSettingsContainerKeys enum.
	///
	/// See \ref OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER and \ref CONTAINERS_PAGE_USING_KEYS.
	///
	#ifndef NO_OLAPI_DLL_LINKAGE
	extern __declspec(dllimport) char *UserSystemSettingsContainerKeyNames[OLKEY_RO_UserSystemSettingsKeysMax];
	#endif


	// 


	///
	/// \ingroup OLUSER_API_SAVE_LOAD_EVENTS
	/// \brief DEPRECATED - Additional context info for a OLUserSessionEventHandler::LoadGameRequest or a
	///			OLUserSessionEventHandler::SaveGameRequest event .
	///
	/// Load and save game requests are deprecated and should be avoided by new titles.
	/// See \ref OLUSER_API_SAVE_LOAD_EVENTS.
	///
	enum OLUIRequestType
	{
		OL_UIREQUEST_SUSPEND,						///< This request is hidden, and used during shutdown after a SuspendSession responds OL_NOT_IMPLEMENTED
		OL_UIREQUEST_HIDDEN,						///< DEPRECATED - The request is hidden and selection is automated.
		OL_UIREQUEST_AUTOMATIC,						///< DEPRECATED - Status of operation is shown, selection is automated.
	};


	class OLUserSessionEventHandler;
	class OLUserSessionEventWaitingCallback;

	///
	/// \ingroup USER
	/// \class OLUserSession
	/// \brief OLUserSession API gives you access to the bound user's session status, system settings, and privacy settings.
	///			It also allows you to handle user events such as pause/resume and confirmExitGame (and control inactivity timeouts).
	///
	/// \par API Initialization
	///		The OLUserSession singleton is created/retrieved with \ref olapi::OLGetUser() and destroyed 
	///		with \ref olapi::OLStopUser().\n\n
	///		NOTE: OLUserSession depends on the OLService API and should be initialized after it. \n
	///		See \ref OLUSER_API_CREATE_AND_DESTROY and \ref SDK_SESSION_FLOW_PAGE for general info about 
	///		initializing the OnLive SDK APIs.
	///
	/// \par API Event and Callback Handling
	///		See \ref SDK_EVENTS_PAGE for an overview or \ref OLUSER_API_EVENT_HANDLING for specifics.
	///
	/// \par Getting Information about the User or UserSession
	///		The UserSessionStatus container holds info about the user's session (user tag, userId,
	///		etc).  See \ref GetStatus and \ref OLUSER_API_SESSION_STATUS_CONTAINER.\n\n
	/// 
	///     The UserSystemSettings container has info about the user's preferences and hardware
	///		(language/locale, audio channels and volume, availability of mouse/keyboard/controller, etc).
	///		See \ref GetUserSystemSettings and \ref OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER.\n\n
	///
	/// \par Game Privacy Status Flags
	///		The Game Privacy Status Flags give you runtime control over users spectating your title
	///		or creating Brag Clips.  For example, if a 3rd party online system uses an email address
	///		as the username, you should disable Brag Clips and spectating while the email address is
	///		visible.  See \ref SetGamePrivacyStatus and \ref OLUSER_API_PRIVACY_STATUS.
	///
	///	\par Inactivity Timeouts
	///		The OSP has two inactivity checks: a 5+ minute user inactivity timeout and a 15 second 
	///		video stall detection timeout. Each can be disabled temporarily if your title expects to be inactive. 
	///		See \ref SuspendInputTimeout / \ref SuspendVideoStallDetection and \ref OLUSER_API_INACTIVITY_TIMEOUTS.
	///
	class OLUserSession : public OLEventBase
	{
		public:

		/// \addtogroup OLUSER_API_EVENT_HANDLING
		/// 
		/// \name OLUserSession Callbacks & Event Handling
		/// 

		/// 
		/// \brief Registers your callback handler object with the OLUserSession API.
		///
		/// If you intend to handle callbacks for OLUserSession, you should set
		/// a callback object before you start the OnLive Services with \ref olapi::OLStartServices.
		/// Once services are started, callbacks can be dispatched by the SDK at any time.
		///
		/// OLUserSession stores a single callback object pointer; calling this method
		/// multiple times will replace the previous pointer.  There is no way to 'unregister'
		/// the pointer (you cannot set it to NULL).
		///
		///	See \ref EVENTS_PAGE_REGISTRATION for details.
		///
		///		\param[in]	callbackHandler	Your implementation of the \ref OLUserSessionEventWaitingCallback interface.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			Normal operation; callbacks can be dispatched on the supplied obj while
		///										the services are running.
		///		\retval OL_INVALID_PARAM	Error - the supplied callback obj pointer was NULL (or invalid);
		///										the callback obj has not been set.
		///
		virtual OLStatus SetEventWaitingCallback(OLUserSessionEventWaitingCallback* callbackHandler) = 0;

		///
		/// \brief Registers your event handler object with the OLUserSession API.
		///
		/// Once you have set the OLUserSession API's eventHandler, calling one of the handle event methods
		///		may dispatch events on your object (assuming events are available to process).
		///		See \ref EVENTS_PAGE_HANDLING_OVERVIEW.
		///
		/// OLUserSession stores a single eventHandler pointer; calling this method
		/// multiple times will replace the previous pointer.  There is no way to 'unregister'
		/// the pointer (you cannot set it to NULL).  
		///
		///	See \ref EVENTS_PAGE_REGISTRATION for details.
		///
		///		\param	eventHandler	Your implementation of the OLUserSessionEventHandler interface.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			Normal operation; events will be dispatched on the supplied obj.
		///		\retval OL_INVALID_PARAM	Error - the supplied eventHandler pointer was NULL (or invalid);
		///										the eventHandler has not been set.
		///
		virtual OLStatus SetEventHandler(OLUserSessionEventHandler* eventHandler) = 0;

		///
		/// \brief Sets this API's EventHandler and attempts to handle a single event. NOTE: prefer to use the simpler
		///  overload \ref HandleEvent(unsigned int).
		///
		/// This method acts as a combination of \ref SetEventHandler and \ref HandleEvent(unsigned int).
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
		///										Otherwise this call may block for up to waitTimeMs if no event is available.
		///										You can use \ref OL_INFINITE to wait forever for the next event.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			An event was dispatched.
		///		\retval OL_INVALID_PARAM	The supplied handler was NULL, but the API does not have an event handler set.
		///		\retval OL_EVENT_TIMEOUT	No events were available for the duration of the wait.
		///
		virtual OLStatus HandleEvent(OLUserSessionEventHandler* eventHandler, unsigned int waitTimeMs) = 0;

		///
		/// \brief Attempts to dispatch a single event on this API's registered event handler object.
		///
		/// You must register an event handler object with \ref OLUserSession::SetEventHandler before
		///		calling HandleEvent.
		///
		/// See \ref SDK_EVENTS_PAGE for details.
		///
		///		\param[in]	waitTimeMs	The max amount of time to wait for an event in milliseconds.
		///										Use 0 to return immediately if there is no event waiting (non-blocking).
		///										Otherwise this call may block for up to waitTimeMs if no event is available.
		///										You can use \ref OL_INFINITE to wait forever for the next event.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			An event was dispatched.
		///		\retval OL_INVALID_PARAM	The API does not have an event handler set (see \ref OLUserSession::SetEventHandler).
		///		\retval OL_EVENT_TIMEOUT	No events were available for the duration of the wait.
		///
		virtual OLStatus HandleEvent(unsigned int waitTimeMs) = 0;

		// 
		// 

		/// \addtogroup OLUSER_API_INACTIVITY_TIMEOUTS
		/// 
		/// \name Controlling Inactivity Timeouts
		/// 

		///
		/// \brief Temporarily disable the OSP's user inactivity timeout for up to 5 minutes at a time.
		///
		/// If an end user is inactive (no keyboard/mouse/controller input) for more than 5 minutes,
		/// the OSP may disconnect the user and start the \ref SDK_SESSION_FLOW_SECTION_UNGRACEFUL_EXIT.
		/// The inactivity timeout is configurable within the OSP, but will never be less than 5 minutes.
		///
		/// If you expect the end user to be inactive for more than 5 minutes (ex: during 
		///	end credits or a long cut-scene), use this method to disable the inactivity timeout
		/// for up to 5 minutes at a time.  When the user is expected to be active again, 
		/// call \ref ResumeInputTimeout() to re-enable the inactivity timeout.
		///
		/// \responseEventList
		///		The OSP automatically enables the timer after 5 minutes and sends an unsolicited StatusResult 
		///		event letting you know:
		///
		///		\ospEvent OLUserSessionEventHandler::StatusResult
		///			\eventDesc	An unsolicited event with olStatus \ref OL_INPUTTIMER_REACTIVATED
		///						means the OSP has re-enabled the inactivity timer.  If you still
		///						expect the user to be inactive, call SuspendInputTimeout again.
		///						This cycle can be repeated indefinitely.
		///
		///						This unsolicited event has a single status code value:
		///							\statusResult OL_INPUTTIMER_REACTIVATED
		///
		///
		///	\responseEventListEnd
		///
		/// \note The actual inactivity timeout used by OnLive may be longer than 5 minutes, 
		///		but we guarantee that it will never be less than 5 minutes.  Assume that it is 5 minutes
		///		and your users will not see OnLive's UI warning that they will be disconnected due to inactivity.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success, the input timeout is disabled.
		///		\retval OL_FAIL						Unable to perform the request. (e.g. OSP's user inactivity timeout is already disabled).
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus SuspendInputTimeout() = 0;

		///
		/// \brief Re-enable the OSP's 5+ minute user inactivity timeout.
		///
		/// If an end user is inactive (no keyboard/mouse/controller input) for more than 5+ minutes,
		/// the OSP will disconnect the user and start the \ref SDK_SESSION_FLOW_SECTION_UNGRACEFUL_EXIT.
		///
		/// See \ref SuspendInputTimeout().
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success, the input timeout is re-enabled.
		///		\retval OL_FAIL						Unable to perform the request. (e.g. OSP's user inactivity timeout is not disabled).
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus ResumeInputTimeout() = 0;

		///
		/// \brief Temporarily disable the OSP's 15 second video stall detection.
		///
		/// The OSP expects your title to render frames while a user is bound to your title.
		/// If the OSP does not detect a frame render for 15 seconds, it may 
		/// disconnect the user and start the \ref SDK_SESSION_FLOW_SECTION_UNGRACEFUL_EXIT.
		///
		/// If you expect your title to stop rendering, you should disable video stall detection
		/// with this method and re-enable it once you are rendering again (see \ref ResumeVideoStallDetection).
		///
		/// Currently, the OSP detects stalls by tracking buffer swaps (ie: the d3d Present/PresentEx 
		/// call, or equivalent). In the future, however, the OSP will be able to track framebuffer differences.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success, the video stall detection is disabled.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus SuspendVideoStallDetection() = 0;

		///
		/// \brief Re-enable the OSP's 15 second video stall detection.
		///
		/// The OSP expects your title to render frames while a user is bound to your title.
		/// If the OSP does not detect a frame render for 15 seconds, it may 
		/// disconnect the user and start the \ref SDK_SESSION_FLOW_SECTION_UNGRACEFUL_EXIT.
		///
		/// See \ref SuspendVideoStallDetection().
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			Success, the input timeout is re-enabled.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus ResumeVideoStallDetection() = 0;

		// 
		// 


		///
		/// \ingroup OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER
		/// \brief Request the UserSystemSettings container from the OSP.  The OSP responds
		///		with either a ContainerIdResult event on success or a StatusResult event on failure.
		///
		/// The UserSystemSettings container holds a user's settings and preferences; for example:
		///		language and volume, available input hardware (mouse/keyboard/controller), etc.
		///		See \ref olapi::UserSystemSettingsContainerKeys.
		///
		/// \responseEventList
		///		The OSP responds with one of the following events if this request is successfully sent: 
		/// 
		///		\ospEvent OLUserSessionEventHandler::ContainerIDResult
		///			\eventDesc	Returns the ID of the UserSystemSettings Container.
		///						Note: The SDKTestHarness does not populate this container
		///						(you will get a container with empty or default values).
		///
		///		\ospEvent OLUserSessionEventHandler::StatusResult
		///			\eventDesc	 The StatusResult event indicates one of the following errors:
		///
		///				\statusResult OL_INTERNAL_ERROR	Generic OSP failure.
		///				\statusResult OL_OUT_OF_MEMORY		Failed to allocate space for the container.
		///
		/// \responseEventListEnd
		///
		///		\param[in]	context		Used to pair this request with one of the two event responses (see \ref olapi::OLGetNextContext).
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Request sent; expect a ContainerIDResult or StatusResult event matching the supplied context.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus GetUserSystemSettings(OLContext context) = 0;

		///
		/// \ingroup OLUSER_API_USER_SESSION_ID
		/// \brief Request the current UserSession's ID from the OSP.  The OSP responds
		///		with either an IDListResult event on success or a StatusResult event on failure.
		///
		/// The UserSession ID is not normally needed by SDK users, but it is used internally for tracking session metrics.
		/// This is the same value as in the UserSessionStatus container (see \ref olapi::OLKEY_RO_UserSessionID).
		///
		/// \responseEventList
		///		The SDK responds with one of the following events if this request is successfully sent: 
		/// 
		///		\ospEvent OLUserSessionEventHandler::IDListResult
		///			\eventDesc	Returns the ID of the UserSessionStatus Container.
		///						If the SessionID is already cached on the client, the GetSessionID call can 
		///						trigger the event dispatch immediately as a side effect.
		///
		///		\ospEvent OLUserSessionEventHandler::StatusResult
		///			\eventDesc	 The StatusResult event indicates that the OSP could not fulfill this request
		///						 due to one of the following errors:
		///
		///				\statusResult OL_SESSION_NOT_STARTED	The UserSession was invalid by the time the OSP got the request.
		///
		/// \responseEventListEnd
		/// 
		///		\param[in]	context		Used to pair this request with one of the two event responses (see \ref olapi::OLGetNextContext).
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Request sent; expect an IDListResult or StatusResult Event response matching the supplied context.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus GetSessionID(OLContext context) = 0;

		///
		/// \ingroup OLUSER_API_SESSION_STATUS_CONTAINER
		/// \brief Request the current UserSessionStatus container from the OSP.  The OSP responds
		///		with either a ContainerIdResult event on success or a StatusResult event on failure.
		///
		/// The UserSessionStatus container provides information about the currently bound user.
		/// For example, the user's tag (username), ID, and their license type for this title.
		/// See \ref olapi::UserSessionStatusContainerKeys.
		///
		///	NOTE: this is the same container you receive in the \ref olapi::OLAppSessionEventHandler::BindUser
		///	event.
		///
		/// \responseEventList
		///		The SDK responds with one of the following events if this request is successfully sent: 
		/// 
		///		\ospEvent OLUserSessionEventHandler::ContainerIDResult
		///			\eventDesc	Provides the ContainerID of the UserSessionStatus container.
		///						See \ref OLUSER_API_SESSION_STATUS_CONTAINER.
		///
		///		\ospEvent OLUserSessionEventHandler::StatusResult
		///			\eventDesc	 The StatusResult event indicates that the OSP could not fulfill this request
		///						 due to one of the following errors:
		///
		///				\statusResult OL_INTERNAL_ERROR	Generic OSP failure.
		///				\statusResult OL_OUT_OF_MEMORY		Failed to allocate space for the container.
		///
		/// \responseEventListEnd
		///
		///		\param[in]	context		Used to pair this request with one of the two event responses (see \ref olapi::OLGetNextContext).
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Request sent; expect a ContainerIDResult or StatusResult event matching the supplied context.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus GetStatus(OLContext context) = 0;



		/// \name Game Privacy Status
		/// 

		///
		/// \ingroup OLUSER_API_PRIVACY_STATUS
		/// \brief Set the title's privacy status flags (control the ability of users to spectate
		///		or anyone to capture Brag Clips of the title).
		///
		/// There are 3 tiers of privacy settings:\n
		///   Title Settings (specified in your game's submission form)\n
		///   User Settings (the end user's preferences, edited in the OnLive Overlay Menu)\n
		///   Runtime Settings (calling this method)
		///   
		/// The overall privacy values are these three settings or'd together.  This means your game
		/// can become more private than default at runtime, but can never override either the title-wide
		/// settings or the bound user's privacy preferences.
		///
		/// A title's default runtime privacy status is \ref OL_GAME_PUBLIC, allowing anyone to spectate
		/// or capture a Brag Clip.  You can only change the privacy flags while a user is bound to your title.
		/// When a user unbinds from your title, the privacy flags are reset back to OL_GAME_PUBLIC.
		///
		/// Note: the OLGameStatus value should be treated as an integer flagset (it will contain the set flags or'd together,
		///		rather than a discrete value from the OLGameStatus enum).
		///
		/// See \ref OLUSER_API_PRIVACY_STATUS.
		///
		///		\param[in]	privacyStatus	The new privacy status flag(s).  Set multiple flags by with bitwise-or
		///									then cast the result back to an OLGameStatus.
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					New privacy settings sent to the OSP.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus SetGamePrivacyStatus(OLGameStatus privacyStatus) = 0;

		// 


		///
		/// \ingroup OLUSER_API_CONFIRM_EXIT_GAME_EVENT
		/// \brief Send a status result back to the OSP to answer a previous request from the OSP.
		///			For example, if you handle the ConfirmExitGame event by displaying custom UI,
		///			you must report the user's choice back to the OSP.
		///
		///
		///		\param[in] statusResult		The result to report back to the OSP;  valid values depend
		///										on which request you are responding to.
		///										
		///		\param[in] requestContext	The context supplied in the OSP's request event.
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; result sent to OSP.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus SetRequestResult( olapi::OLStatus statusResult, olapi::OLContext requestContext ) = 0;

		/// \name Game Privacy Status
		/// 

		///
		/// \ingroup OLUSER_API_PRIVACY_STATUS
		/// \brief Retrieve the title's current runtime privacy status flags from the OSP; the result is
		///		returned in an \ref OLUserSessionEventHandler::GamePrivacyStatusResult Event.
		///
		///	You can only retrieve the title's privacy status while a user is bound.  These bits reflect only the
		/// title's runtime privacy settings; the OSP sets the total privacy settings by merging the title-wide
		/// privacy bits specified for your title with the bound user's privacy settings and the game's
		/// runtime settings.  See \ref SetGamePrivacyStatus.
		///
		/// See \ref OLUSER_API_PRIVACY_STATUS.
		///
		///		\param[in]	context		Used to pair this request with the response event (see \ref olapi::OLGetNextContext).
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Request sent to the OSP.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus GetGamePrivacyStatus(OLContext context) = 0;

		// 

	};

	///
	/// \ingroup USER OLUSER_API_EVENT_HANDLING
	/// \class OLUserSessionEventHandler
	/// \brief This is the OLUserSession API's event interface; your OLUserSession event handler must implement this interface.
	///		Each method represents an asynchronous event returned by the SDK.
	///
	/// See \ref EVENTS_PAGE_CONCEPTS for an overview of event handling.
	///
	/// \par Pause/Resume Events
	///		Gameplay must be paused while the OnLive Overlay is displayed; the \ref OLUserSessionEventHandler::Pause
	///		and \ref OLUserSessionEventHandler::Resume events let you know when to do so.
	///
	/// \par ConfirmGameExit Event
	///		When a user quits a game using the OnLive Overlay UI, the \ref OLUserSessionEventHandler::ConfirmExitGame
	///		lets your title (optionally) prompt the user with custom UI (prompting to save before quitting, for example).
	///
	class OLUserSessionEventHandler
	{
		public:

		///
		/// \brief Event returning an OLStatus code result; this typically means an error was
		///		encountered while the OSP was processing an async request.
		///
		/// For example, if the OSP ends a user session at the same time as a request 
		/// for the user session's status is made, the SDK will return OL_SUCCESS (because it is unaware of the OSP 
		/// ending the user session). However, by the time the request for the user session's status gets 
		/// to the OSP, the user session has ended, and the request is invalid. The OSP dispatches a status 
		/// result event with OL_SESSION_NOT_STARTED.
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
		///
		virtual OLStatus StatusResult(OLContext context, OLStatus status) = 0;

		///
		/// \brief Event returning a list of OnLive IDs.
		///
		/// The actual IDs in the list will depend on the type of request you made.
		/// For example, if this event is in response to a \ref olapi::OLUserSession::GetSessionID request,
		/// the list will contain a single application session ID.
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

		///
		/// \brief Event returning a containerID result; indicates that a request successfuly resulted
		///		in a container.
		///
		/// \note If you return OL_SUCCESS, the container will have its normal
		///		session flow (which varies by container type).  Any other return value causes
		///		the SDK to destroy the container immediately after this method returns.
		///
		/// See \ref CONTAINERS_PAGE_USING_CONTAINERS.
		///
		///		\param[in] context		The matching context you gave to the request for this result (see \ref EVENTS_PAGE_ASYNC_COMM).
		///
		///		\param[in] containerID	The requested container's ID.  The container's type will depend on what you requested.
		///
		///	\return Returns an OLStatus code to the SDK.
		///		\retval OL_SUCCESS				The event was handled; the container is valid until destroyed.
		///		\retval OL_NOT_IMPLEMENTED		The event was ignored (and the container will be destroyed immediately).
		///
		virtual OLStatus ContainerIDResult(OLContext context, OLID containerID) = 0;


		///
		/// \ingroup OLUSER_API_PAUSE_RESUME_EVENTS
		/// \brief Event sent when the game should pause gameplay (when the end user brings up the
		///			OnLive Overlay or is having significant network problems).
		///
		///	Pause events must affect gameplay, but should not halt rendering.  The OSP will send a \ref Resume 
		///	Event when gameplay should resume.  Your title will not able to receive input from the user
		/// while it is paused.
		///
		/// If the freeze flag is false, the game should continue animating any UI or relevant in-game objects
		/// (for example, swaying grass or other environmental animations that do not affect gameplay).
		/// If the freeze flag is true, however, the game should render the current frame over and over
		/// until the game is unpaused (see \ref Resume). If your game does not support freeze, you should just ignore
		/// the freeze flag and pause the gameplay.
		///
		/// It is not critical that your title respect the freeze argument; the rationale is to allow
		/// the OSP to "idle" a game by issuing a pause with freeze.  Since the game is just displaying
		/// the same framebuffer over & over, the power consumption of the graphics card decreases.
		///
		/// Note: It is very important that multiple player games handle pause properly - a pause event
		///	should not interrupt the game's network processing or disconnect game members.  Also, you may
		/// need to save the game's pre-pause status so you can restore it upon resume.  For example, if
		/// the user's game is already paused when he shows the OnLive Menu, dismissing
		/// the overlay should return the game to its previous (paused) state.
		///
		///		\param[in]	freeze		If true, render the current frame over & over (without any animations).
		///								If false, render UI & animations that do not affect gameplay.
		///
		///	\return Returns an OLStatus code to the SDK.
		///		\retval OL_SUCCESS				The event was handled; gameplay is paused.
		///		\retval OL_FAIL					The event was ignored; gameplay is already paused.  The OSP will not 
		///											shutdown the game.
		///
		virtual OLStatus Pause(bool freeze) = 0;

		///
		/// \ingroup OLUSER_API_PAUSE_RESUME_EVENTS
		/// \brief Event sent when the game should restore gameplay to its previous state (the state the game was
		///		in when it received the \ref Pause event).
		///
		/// This is typically sent when a user closes the OnLive Overlay or when a user with network connectivity
		/// problems recovers.
		///
		///	\return Returns an OLStatus code to the SDK.
		///		\retval OL_SUCCESS				The event was handled; the gameplay state is restored.
		///		\retval OL_FAIL					The event was ignored. gameplay is not paused. The OSP will not 
		///											shutdown the game.
		///
		virtual OLStatus Resume() = 0;


		///
		/// \ingroup OLUSER_API_SAVE_LOAD_EVENTS
		/// \brief DEPRECATED - This event is never sent by the OSP and will be removed in a future version
		///			of the OnLive SDK; return OL_NOT_IMPLEMENTED until then.
		///
		/// \deprecated This event is never sent by the OSP and will be removed in a future version
		///			of the OnLive SDK.  Return OL_NOT_IMPLEMENTED until then.
		/// 
		virtual OLStatus SaveGameRequest( olapi::OLUIRequestType type, olapi::OLContext context ) = 0;

		///
		/// \ingroup OLUSER_API_SAVE_LOAD_EVENTS
		/// \brief DEPRECATED - This event is never sent by the OSP and will be removed in a future version
		///			of the OnLive SDK; return OL_NOT_IMPLEMENTED until then.
		///
		/// \deprecated This event is never sent by the OSP and will be removed in a future version
		///			of the OnLive SDK.  Return OL_NOT_IMPLEMENTED until then.
		///
		virtual OLStatus LoadGameRequest( olapi::OLUIRequestType type, olapi::OLContext context ) = 0;

		///
		/// \ingroup OLUSER_API_CONFIRM_EXIT_GAME_EVENT
		/// \brief ConfirmExitGame lets your title display custom UI in response to the 
		///			user's overlay exit request.
		///
		/// When a user requests to exit your game from the OnLive Overlay, the ConfirmExitGame
		/// event is dispatched.  This lets your game reject the exit request or (optionally)
		/// display custom UI (asking the user if they are sure they want to exit and/or prompting
		/// them to save before they quit).
		///
		/// \par Rejecting the exit request
		///		If your game is not in a state where the user can exit (ex: in the middle of a game
		///		save), return OL_BUSY and the user's exit is rejected. OnLive Overlay will retry this 
		///		operation by sending \ref OLUserSessionEventHandler::ConfirmExitGame again every 1 second until a 
		///		OL_SUCCESS or OL_NOT_IMPLEMENTED is returned.
		///
		/// \par Rely on the Overlay's generic UI confirmation
		///		You can return OL_NOT_IMPLEMENTED and the Overlay will prompt the user with a
		///		generic "are you sure you want to quit?" dialog.
		///
		/// \par Show custom UI
		///		Show your custom UI, and tell the OSP if the user still wants to exit by calling
		///		\ref OLUserSession::SetRequestResult with the context supplied to this event and
		///		either OL_SUCCESS (the user confirmed their exit) or OL_FAIL (the user decided
		///		not to quit).
		///
		/// \par Handling the exit confirmation
		///		If the user confirms their exit (either with the generic overlay dialog or your
		///		custom UI that calls SetRequestResult), the OSP sends an \ref OLAppSessionEventHandler::UnbindUser 
		///		event and the user follows the unbinding process.
		///
		/// Note: You will never get overlapping ConfirmExitGame requests, but a user may be suspended at any time
		/// (including while you are showing the exit confirmation UI).
		///
		/// See the \ref SDK_SESSION_FLOW_SECTION_OVERLAY_EXIT.
		///
		///		\param[in] context		Use this context with \ref OLUserSession::SetRequestResult 
		///									to notify the OSP of the results of this UI operation.
		///
		///	\return Returns an OLStatus code to the SDK.
		///		\retval OL_SUCCESS				The event was handled; you will report the user's confirmation
		///											by calling \ref OLUserSession::SetRequestResult.
		///		\retval OL_NOT_IMPLEMENTED		The event was ignored; the overlay will display a generic
		///											"are you sure" dialog to the user.
		///		\retval OL_BUSY					Event handled, but the exit request was rejected because
		///											the title is not able to exit at this time. The OSP will resend the event in 1 second.
		///		\retval OL_FAIL					Deprecated return value for this event; it is treated
		///											the same as OL_BUSY.
		///
		virtual OLStatus ConfirmExitGame( olapi::OLContext context ) = 0;

		///
		/// \ingroup OLUSER_API_PRIVACY_STATUS
		/// \brief Event containing the title's current privacy status flags; (triggered by \ref OLUserSession::GetGamePrivacyStatus).
		///
		/// Note: the OLGameStatus value should be treated as an integer flagset (it will contain the set flags or'd together,
		///		rather than a discrete value from the OLGameStatus enum).
		///
		/// Event from the OSP containing the results of a title's request for its own privacy
		/// status flags. Privacy status is the five <I>OLGameStatus</I> bit flags, or'ed together.
		///
		///		\param[in] context		The matching context you gave to the request for this result (see \ref EVENTS_PAGE_ASYNC_COMM).
		///
		///		\param	privacyStatus	The current game privacy status flagset.
		///
		///	\return	Returns an OLStatus code.
		///		\retval OL_SUCCESS					Event successfully handled.
		///		\retval OL_NOT_IMPLEMENTED			Event ignored intentionally.
		///		\retval OL_FAIL						The event was not handled due to an unexpected error.
		///
		virtual OLStatus GamePrivacyStatusResult( OLContext context, OLGameStatus privacyStatus) = 0;
	};

	///
	/// \ingroup OLUSER_API_EVENT_HANDLING USER
	/// \class OLUserSessionEventWaitingCallback
	/// \brief This is the OLUserSession API's callback interface; your title's OLUserSession callback handler must implement this interface.
	///
	/// Each method in this class represents an OLUserSession callback.  If you choose to handle OLUserSession callbacks,
	/// register your callback handler using \ref OLUserSession::SetEventWaitingCallback.  Callback methods
	/// may be invoked at any time once the OnLive Services are running (see \ref OLSERVICE_API_CONNECTION_DISCONNECTION).
	///
	/// \note Callback methods are always invoked from an internal OnLive SDK thread, so all callback implementations must be thread safe.
	///
	/// See \ref EVENTS_PAGE_CONCEPTS for details.
	///
	class OLUserSessionEventWaitingCallback
	{
		public:

		///
		/// \brief Callback dispatched when an OLUserSession event is waiting to be handled.
		///
		/// If your title handles events from callbacks, call \ref OLUserSession::HandleEvent(unsigned int) to dispatch the waiting event.
		///	If you handle events by polling, you can ignore this event.
		///
		/// \note The EventWaiting callback must be thread safe and must complete in less than two milliseconds.
		///
		/// See \ref EVENTS_PAGE_HANDLING_OVERVIEW for more info.
		///
		virtual void EventWaiting() = 0;
	};

	/// \addtogroup OLUSER_API_CREATE_AND_DESTROY
	/// 

	///
	/// \brief Get the OLUserSession API singleton (the instance is created if necessary).
	///
	///	The OLUserSession pointer can be stored until the API is destroyed with \ref olapi::OLStopUser().
	///
	/// See \ref OLUSER_API_CREATE_AND_DESTROY.
	///
	///		\param[in] version		The API version string (for class compatibility), use \ref OLAPI_VERSION.
	///
	///		\param[out] olUserSession	The supplied pointer is set to the OLUserSession instance on success.
	///									The pointer is not modified on failure.
	///
	///	\return An OLStatus code.
	///		\retval	OL_SUCCESS			Success; the olUserSession pointer is set to the singleton.
	///		\retval OL_INVALID_PARAM	The supplied version string or olUserSession pointer is NULL.
	///		\retval OL_INVALID_VERSION	The API version requested is not supported. This is a fatal 
	///										error and your title should exit.
	///		\retval OL_INTERNAL_ERROR	A general internal or system error.
	///		\retval OL_OUT_OF_MEMORY	The SDK could not alloc memory for the singleton.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLGetUser(const char* version, OLUserSession** olUserSession);

	///
	/// \brief Destroy the OLUserSession API singleton.
	///
	/// The OnLive Services should be stopped before you destroy an API instance; see \ref olapi::OLStopServices().
	///
	/// See \ref OLUSER_API_CREATE_AND_DESTROY.
	///
	///		\param[in] olUserSession		Pointer to the OLUserSession instance to destroy.
	///
	///	\return Returns an OLStatus code.
	///		\retval	OL_SUCCESS			Success; the olUserSession was destroyed.
	///		\retval OL_INVALID_PARAM	The supplied olUserSession pointer was NULL.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLStopUser(OLUserSession* olUserSession);

	/// 

// doxygen group comments
/*!

\addtogroup USER
	\brief The OLUserSession API gives you access to the bound user's session status, system settings,
	and privacy settings.  It also allows you to control input timeouts and handle user
	events such as pause/resume and confirmExitGame.

\addtogroup OLUSER_API_SESSION_STATUS_CONTAINER
	\brief The UserSessionStatus container holds information about the currently bound user,
	including the username, userID, and userSessionId, and more.

\addtogroup OLUSER_API_USER_SESSION_ID
	\brief The ApplicationSession Id is not normally needed by SDK users; it is used internally for tracking session metrics.

\addtogroup OLUSER_API_USER_SYSTEM_SETTINGS_CONTAINER
	\brief The UserSystemSettings container holds a user's settings and preferences, such as: language/locale info, audio
		hardware and volume, available input hardware (mouse/keyboard/controller, controller vibration), etc.

\addtogroup OLUSER_API_CONFIRM_EXIT_GAME_EVENT
	\brief When an end user chooses to exit a game using OnLive's Overlay, the SDK notifies your title
		with the \ref OLUserSessionEventHandler::ConfirmExitGame Event.  This allows the game to display 
		an "are you sure you want to exit" dialog, confirming that the user wants to exit.

	The entire process of a user exiting a game via the overlay is captured in the \ref SDK_SESSION_FLOW_SECTION_OVERLAY_EXIT.
		

\addtogroup OLUSER_API_SAVE_LOAD_EVENTS
	\brief The \ref OLUserSessionEventHandler::LoadGameRequest "LoadGameRequest" and 
			\ref OLUserSessionEventHandler::SaveGameRequest "SaveGameRequest" events are deprecated.

\addtogroup OLUSER_API_PAUSE_RESUME_EVENTS
	\brief The OSP sends Pause and Resume events when gameplay should stop and be restored.  Typically,
		this is when a user shows/hides the OnLive Overlay MenuSystem, or if a user is having severe
		network connectivity problems.

\addtogroup OLUSER_API_PRIVACY_STATUS
	\brief A game's privacy status flags allows runtime control over spectating and bragclips.

	A title may need to suppress spectating or Brag Clips at certain times; for example:
	<ul>
		<li> user privacy - suppress spectating & Brag Clips while the user logs into a 3rd party
			online system (with their email address onscreen).
		<li> legal issues - some titles need to suppress audio while Spectating or capturing Brag Clips
			due to music licensing issues.
		<li> gameplay - you may want to suppress Brag Clips of your end sequence or at other times.
	</ul>

	Note: The title's privacy status can be overridden by either the title's configuration (the settings
	specified when you submitted your game to certification), or by the bound user's privacy settings.


\addtogroup OLUSER_API_INACTIVITY_TIMEOUTS
	\brief The OSP has two inactivity checks: a 5+ minute user inactivity timeout and a 15 second video stall
		detection timeout.  Each can be disabled temporarily if your title expects to be inactive.

	The actual user inactivity timeout duration may vary, but it will always be >= 5 minute.  If your title
	expects the user to be inactive (no input) for 5 minutes or longer, you should disable the inactivity timeout.

	If either timeout expires, the OSP will disconnect the title's bound user and start the 
	\ref SDK_SESSION_FLOW_SECTION_UNGRACEFUL_EXIT.

\addtogroup OLUSER_API_CREATE_AND_DESTROY
	\brief The OLUserSession singleton is created/retrieved with \ref olapi::OLGetUser() and destroyed with \ref olapi::OLStopUser().

	Do not destroy any of the OnLive APIs (or their handlers) until after you have stopped the OnLive Services.
	See \ref SDK_SESSION_FLOW_PAGE and \ref OLSERVICE_API_CONNECTION_DISCONNECTION.

\addtogroup OLUSER_API_EVENT_HANDLING
	\brief Items related to processing OLUserSession callbacks and events.

	See \ref SDK_EVENTS_PAGE for an overview.

*/

}; // namespace olapi

#endif // OLUSERSESSION_H
