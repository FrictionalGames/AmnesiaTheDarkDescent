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
// olachievement.h $Revision: 54252 $
//

/// \file olachievement.h
/// \brief The OLAchievement interface provides communication between a running title and the OLService gateway.
///

#ifndef OLACHIEVEMENT_H
#define OLACHIEVEMENT_H

#include <ol_api/olapi.h> ///<
//#include <ol_api/olservice.h>

namespace olapi
{
	/*! \ingroup OLACHIEVEMENT_API_CREATE_AND_DESTROY 
		\brief The default version of the OnLive OLAchievement API to use in \ref olapi::OLGetAchievement.
	*/
	#define ACHIEVEMENT_API_VERSION (OLAPI_VERSION)


	///
	/// \ingroup OLACHIEVEMENT_RETRIEVE_DEFINITIONS
	/// \brief Key enumeration for accessing the AchievementDefinition container
	///
	enum AchievementDefinitionContainerKeys
	{
		OLKEY_RO_AchievementDefContainerName,			///< string, Name of Container
		OLKEY_RO_AchievementDefContainerListID,			///< Internal ID of Container List
		OLKEY_RO_AchievementDefID,						///< Unique ID for this Achievement
		OLKEY_RO_AchievementAddOnID,					///< AddOn ID for this Achievement, only valid if this achievement is part of a Content AddOn
		OLKEY_RO_AchievementDefName,					///< The short name of this Achievement
		OLKEY_RO_AchievementDefSequenceNum,				///< The sequence number of this Achievement (used for sorting)
		OLKEY_RO_AchievementDefDescription,				///< The long description of this Achievement
		OLKEY_RO_AchievementDefPoints,					///< The OnLive Points awarded for this Achievement
		OLKEY_RO_AchievementDefType,					///< The type of this Achievement (uses Int to specify enum AchievementType)
		OLKEY_RO_AchievementDef_Int_GoalValue,			///< Goal value for this Achievement (Note: For Int Achievement type only)
		OLKEY_RO_AchievementDef_Int_AccumulationType,	///< How to accumulate points (uses Int to specify enum AchievementAccumulationType);(Note: For Int Achievement type only)
		OLKEY_RO_AchievementDef_Int_DisplayType,		///< How to display this Achievement (uses Int to specify enum AchievementDisplayType); (Note: For Int Achievement type only)
		OLKEY_RO_AchievementDef_OneOfSet_NumItemsInSet,			///< Number of items in this set (Note: For OneOfSet Achievement type only)
		OLKEY_RO_AchievementDef_OneOfSetItem_ID_Vector,			///< ID for item; a vector that needs to be unpacked (Note: For OneOfSet Achievement type only)
		OLKEY_RO_AchievementDef_OneOfSetItem_Name_Vector,		///< Short name for item; a vector that needs to be unpacked (Note: For OneOfSet Achievement type only)
		OLKEY_RO_AchievementDef_OneOfSetItem_Points_Vector,		///< Point value of item; a vector that needs to be unpacked (Note: For OneOfSet Achievement type only)
		// Added in SDK Version 1.111.11.11
		OLKEY_RO_AchievementDef_Flags,							///< Flags that specify extra functionality; a vector that needs to be unpacked (Note: For OneOfSet Achievement type only) 
		OLKEY_RO_AchievementDefKeysMax
	};
	

	///
	/// \ingroup OLACHIEVEMENT_SET_AND_GET_ACHIEVEMENTS
	/// \brief Key enumeration for accessing the Achievement Container
	///
	enum AchievementContainerKeys
	{
		OLKEY_RO_AchievementName,					///< string, Name of Container
		OLKEY_RO_AchievementContainerListID,		///< Internal ID of Container List
		OLKEY_RO_AchievementID,						///< ID of Achievement
		OLKEY_RO_AchievementType,					///< The type of this Achievement (uses Int to specify enum AchievementType)
		OLKEY_RO_AchievementBoolValue,				///< Value of Achievement if type is Bool
		OLKEY_RO_AchievementIntValue,				///< Value of Achievement if type is Int
		OLKEY_RO_AchievementOneOfSetValue,			///< Value of Achievement if type is OneOfSet
		// Added in SDK version 1.138.15.15
		OLKEY_RO_AchievementMediaClipID,			///< The ID of the media clip (brag clip), if any, that is associated with this achievement 
		OLKEY_RO_AchievementUpdatedAt,				///< The date/time of the last update to this achievement (format is "%Y-%m-%d %H:%M:%S " using strftime() )
		OLKEY_RO_AchievementKeysMax
	};
	

	///
	/// \ingroup OLACHIEVEMENT_RETRIEVE_ICONS_AND_VIDEO
	/// \brief Key enumeration for accessing the Achievement Media container
	///
	enum AchievementMediaContainerKeys
	{
		OLKEY_RO_AchievementMediaName,					///< string, Name of Container
		OLKEY_RO_AchievementMediaContainerListID,		///< Internal ID of Container List
		OLKEY_RO_AchievementMediaSequenceNum,			///< Sequence Number of this asset
		OLKEY_RO_AchievementMediaURI_1280x720,			///< Media URI for resolution 1280x720
		OLKEY_RO_AchievementMediaURI_384x216,			///< Media URI for resolution 384x216
		OLKEY_RO_AchievementMediaURI_196x110,			///< Media URI for resolution 196x110
		OLKEY_RO_AchievementMediaURI_160x90,			///< Media URI for resolution 160x90
		OLKEY_RO_AchievementMediaURI_128x128,			///< Media URI for resolution 128x128
		OLKEY_RO_AchievementMediaURI_576x720,			///< Media URI for resolution 576x720
		OLKEY_RO_AchievementMediaURI_140x176,			///< Media URI for resolution 140x176
		OLKEY_RO_AchievementMediaURI_96x120,			///< Media URI for resolution 96x120	
		// Added in SDK Version 1.131.11.11
		OLKEY_RO_AchievementMediaSortOrder,				///< Sort Order of this asset, 0 = locked, 1 = unlocked for bool or int, [1, n] = unlocked for one of set
		OLKEY_RO_AchievementMediaKeysMax
	};


	///\brief The name of AchievementDefContainer container name.
    #define ACHIEVEMENTDEFCONTAINERNAME					"AchievementDefContainer"				
	///\brief The name of the AchievementContainer container name.
    #define ACHIEVEMENTCONTAINERNAME					"AchievementContainer"	
	///\brief The name of the AchievementMediaContainer container name.
    #define ACHIEVEMENTMEDIACONTAINERNAME				"AchievementMediaContainer"				
	

	///
	/// \ingroup OLACHIEVEMENT_RETRIEVE_DEFINITIONS
	/// \brief Enumeration for specifying the Achievement type 
	///
	enum AchievementType
	{
		OL_AchievementType_Unknown,								///< Unknown Achievement type
		OL_AchievementType_Bool,								///< Bool Achievement type (on/off)
		OL_AchievementType_Int,									///< Int Achievement type (x/y)
		OL_AchievementType_OneOfSet,							///< OneOfSet Achievement type (1 item selected from a set)

		OL_AchievementType_Max
	};


	///
	/// \ingroup OLACHIEVEMENT_RETRIEVE_DEFINITIONS
	/// Enumeration for specifying how an Int Achievement accumulates points
	///
	enum AchievementAccumulationType
	{
		OL_AchievementAccumulationType_Unknown,		///< Unknown accumulation type
		OL_AchievementAccumulationType_Partial,		///< Accumulate partial points
		OL_AchievementAccumulationType_Full,		///< Only accumulate points when full
		OL_AchievementAccumulationType_Max
	};


	///
	/// \ingroup OLACHIEVEMENT_RETRIEVE_DEFINITIONS
	/// Enumeration for specifying how an Int Achievement displays the point accumulation
	///
	enum AchievementDisplayType
	{
		OL_AchievementDisplayType_Unknown,		///< Unknown display type
		OL_AchievementDisplayType_Bar,			///< Display a filled bar
		OL_AchievementDisplayType_XofY,			///< Display “X of Y”
		OL_AchievementDisplayType_Float,		///< Display 0.0 to 1.0
		OL_AchievementDisplayType_Percent,		///< Display “XX%”
		OL_AchievementDisplayType_IconBar,		///< Display icon filled with bar
		OL_AchievementDisplayType_Max
	};
	/// \addtogroup OLACHIEVEMENT_RETRIEVE_DEFINITIONS
	/// 


	#define IsAchievementDefinitionFlagSet(olAchievementDefinitionFlags, flag) ((olAchievementDefinitionFlags) & (flag))
	///
	enum AchievementDefinitionFlags
	{
		OL_DISPLAY_ACHIEVEMENT_ALERT							= 0x0001,		///< OLAchievement should display an alert when this Achievement is unlocked
		OL_TRIGGER_BRAGCLIP_ON_UNLOCK							= 0x0002,		///< When this Achievement is unlocked, trigger a bragclip
		OL_IS_RANK												= 0x0004,		///< This Achievement is designated as a "rank" for display purposes
		OL_IS_HIDDEN											= 0x0008,		///< This Achievement is hidden (won't be shown until unlocked)
		OL_DISPLAY_ACHIEVEMENT_ALERT_INT_10_PERCENT				= 0x0010,		///< OLAchievement should display an alert for an Int Achievement every 10%; Note: This flag is for an Int Achievement and will do nothing if used with other Achievement types
		OL_DISPLAY_ACHIEVEMENT_ALERT_INT_25_PERCENT				= 0x0020,		///< OLAchievement should display an alert for an Int Achievement every 25%; Note: This flag is for an Int Achievement and will do nothing if used with other Achievement types
		OL_DISPLAY_ACHIEVEMENT_ALERT_INT_EVERY_UNIT				= 0x0040,		///< OLAchievement should display an alert for an Int Achievement on every single update; Note: This flag is for an Int Achievement and will do nothing if used with other Achievement types
	};


	///
	/// \brief Key strings in the AchievementStatus container
	///
	#ifndef NO_OLAPI_DLL_LINKAGE
	/// \ingroup OLACHIEVEMENT_RETRIEVE_DEFINITIONS
	extern __declspec(dllimport) char *AchievementDefinitionContainerKeyNames[olapi::OLKEY_RO_AchievementDefKeysMax];
	/// \ingroup OLACHIEVEMENT_RETRIEVE_ICONS_AND_VIDEO
	extern __declspec(dllimport) char *AchievementMarketingAssetContainerKeyNames[olapi::OLKEY_RO_AchievementMediaKeysMax];
	/// \ingroup OLACHIEVEMENT_SET_AND_GET_ACHIEVEMENTS
	extern __declspec(dllimport) char *AchievementContainerKeyNames[olapi::OLKEY_RO_AchievementKeysMax];
	/// \ingroup OLACHIEVEMENT_RETRIEVE_DEFINITIONS 
	extern __declspec(dllimport) char *AchievementTypeNames[olapi::OL_AchievementType_Max];
	/// \ingroup OLACHIEVEMENT_RETRIEVE_DEFINITIONS
	extern __declspec(dllimport) char *AchievementAccumulationTypeNames[olapi::OL_AchievementAccumulationType_Max];
	/// \ingroup OLACHIEVEMENT_RETRIEVE_DEFINITIONS
	extern __declspec(dllimport) char *AchievementDisplayTypeNames[olapi::OL_AchievementDisplayType_Max];
	/// \ingroup OLACHIEVEMENT_RETRIVE_ICONS_AND_VIDEO
	extern __declspec(dllimport) char *AchievementMediaContainerKeyNames[olapi::OLKEY_RO_AchievementMediaKeysMax];
	#endif
		/// end addtogroup OLACHIEVEMENT_RETRIEVE_DEFINITIONS
		/// 

	class OLAchievementEventHandler;
	class OLAchievementCallbacks;


	///
	/// \ingroup OLACHIEVEMENT_API_EVENT_HANDLING OLACHIEVEMENTS_API
	/// \class OLAchievement
	/// \brief Achievement Interface Class
	///
	/// Achievement Interface Class
	///
	class OLAchievement : public OLEventBase
	{
		public:

		/// \addtogroup OLACHIEVEMENT_API_EVENT_HANDLING
		/// 
		/// \name OLAchievement Callbacks & Event Handling
		/// 

		/// \brief Allows client to be notified that an event is available.
		///
		/// Sets up a callback class object to be notified when an event is waiting to be handled. Event
		/// notification is achieved by calling the callback class object's EventWaiting() function.\n\n
		/// The application can use this to optimize Event Handling.
		///
		///		\param	callback		Pointer to OLAppEventWaitingCallback to call when an event is available.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM invalid or NULL parameter callback.
		///
		virtual OLStatus SetEventWaitingCallback(OLAchievementCallbacks* callback) = 0;


		/// \ingroup OLACHIEVEMENT_API_EVENT_HANDLING
		///
		/// \brief Sets handler to call when an application event is processed.
		///
		/// Handler used by application to process events.
		///
		///		\param	handler			Pointer to OLAchievementEventHandler interface.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM invalid or NULL parameter handler.
		///
		virtual OLStatus SetEventHandler(OLAchievementEventHandler* handler) = 0;


		/// \ingroup OLACHIEVEMENT_API_EVENT_HANDLING
		///
		/// \brief Dispatches the next Achievement API event.
		///
		/// Dispatches the next Achievement API event, if available, to the handler indicated in SetEventHandler.  Implements an
		/// OLAchievementEventHandler() interface in the application to process events from each of the APIs.
		///
		///		\param	handler			Pointer to OLAchievementEventHandler interface.
		///
		///		\param	time			Time to wait in milliseconds for next event, OL_INFINITE will wait until next event.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM invalid or NULL parameter handler and handler not already set, \n
		///								OL_EVENT_TIMEOUT if no events available.
		///
		virtual OLStatus HandleEvent(OLAchievementEventHandler* handler, unsigned int time) = 0;


		///
		/// \brief Retrieves next event, if available.
		///
		/// Call upon event callback or per frame to handle (dispatch) the next event.
		/// The eventHandler must be previous setup with a SetEventHandler() call in each API.
		///
		///		\param	time			Time to wait for next event in milliseconds or\n
		///								OL_INFINITE to wait for next event.
		///
		///		\return	OLStatus		OL_SUCCESS Normal operation.\n
		///								OL_INVALID_PARAM Invalid or NULL parameter handler and handler not already set. \n
		///								OL_EVENT_TIMEOUT If no events available.
		///
		virtual OLStatus HandleEvent(unsigned int time) = 0;

		/// end addtogroup OLACHIEVEMENT_API_EVENT_HANDLING
		/// 
		/// end name OLAchievement Callbacks & Event Handling
		/// 

		///
		/// \ingroup OLACHIEVEMENT_RETRIEVE_DEFINITIONS
		/// \brief Retrieves Achievement definitions for a title ID.
		///
		/// The Achievement Defintions are received as container lists, this will be an OLIDList of container IDs.
		/// The result will be returned through IDListResult() in OLAchievementEventHandler.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\param	titleID			The title ID to retrieve Achievement Definitions for.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM one of the params is invalid.
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus GetAchievementDefContainerIDList(OLContext context, OLID titleID) = 0;


		///
		/// \ingroup OLACHIEVEMENT_RETRIEVE_ICONS_AND_VIDEO
		/// \brief Retrieves media URIs for icons associated with Achievement Definitions for a title ID.
		///
		/// The media URIs are received as container lists, this will be an OLIDList of container IDs.
		/// The result will be returned through IDListResult() in OLAchievementEventHandler.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\param	titleID			The title ID to retrieve Achievement Definitions for.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM one of the params is invalid.
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus GetAchievementMediaIconContainerIDList(OLContext context, OLID titleID) = 0;


		///
		/// \ingroup OLACHIEVEMENT_RETRIEVE_ICONS_AND_VIDEO
		/// \brief Retrieves media for videos associated with Achievement Definitions for a title ID.
		///
		/// The media URIs are received as container lists, this will be an OLIDList of container IDs.
		/// The result will be returned through IDListResult() in OLAchievementEventHandler.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\param	titleID			The title ID to retrieve Achievement Definitions for.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM one of the params is invalid.
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus GetAchievementMediaVideoContainerIDList(OLContext context, OLID titleID) = 0;


		///
		/// \ingroup OLACHIEVEMENT_SET_AND_GET_ACHIEVEMENTS
		/// \brief Retrieves the values of Achievement for the user.
		///
		/// The achievements are received as container lists, this will be an OLIDList of container IDs.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\param	userID			The user ID to get Achievement for.
		///								  When called from a title, this must be the user's userID.
		///
		///		\param	titleID			The title ID to get the Achievement for.
		///								  When called from a title, this must be the title's titleID.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM one of the params is invalid.
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus GetAchievementsContainerIDList(OLContext context, OLID userID, OLID titleID) = 0;


		///
		/// \ingroup OLACHIEVEMENT_SET_AND_GET_ACHIEVEMENTS
		/// \brief Sets an Achievement for the user.
		///
		/// The Achievement is set for the user by a specified Achievement ID.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\param	achievementID	The ID of the Achievement to set.
		///
		///		\param	userID			The user ID to get the Achievement for.
		///								  When called from a title, this must be the user's userID.
		///
		///		\param	titleID			The title ID to set the Achievement for.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM one of the params is invalid.
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus SetAchievementBool(OLContext context, OLID achievementID, OLID userID, OLID titleID) = 0;


		///
		/// \ingroup OLACHIEVEMENT_SET_AND_GET_ACHIEVEMENTS
		/// \brief Sets an Achievement for the user.
		///
		/// The Achievement is set for the user by a specified Achievement ID.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\param	achievementID	The ID of the Achievement to set.
		///
		///		\param	value			The value to set the Achievement to.
		///
		///		\param	userID			The user ID to get Achievement for.
		///								  When called from a title, this must be the user's userID.
		///
		///		\param	titleID			The title ID to set the Achievement for.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM one of the params is invalid.
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus SetAchievementInt(OLContext context, OLID achievementID, unsigned int value, OLID userID, 
										   OLID titleID) = 0;

		///
		/// \ingroup OLACHIEVEMENT_SET_AND_GET_ACHIEVEMENTS
		/// \brief Sets an Achievement for the user.
		///
		/// The Achievement is set for the user by a specified Achievement ID.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\param	achievementID	The ID of the Achievement to set.
		///
		///		\param	value			The value to set the Achievement to.
		///								  This is the ID of the item in the OneOfSet
		///
		///		\param	userID			The user ID to get Achievement for.
		///								  When called from a title, this must be the user's userID.
		///
		///		\param	titleID			The title ID to set the Achievement for.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM one of the params is invalid.
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus SetAchievementOneOfSet(OLContext context, OLID achievementID, OLID value, OLID userID, 
												OLID titleID) = 0;

		///
		/// \ingroup OLACHIEVEMENT_RETRIEVE_ONLIVE_POINTS
		/// \brief Gets OnLive Points for the given title for the user.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\param	userID		The user ID to get OnLive Points for.
		///							  When called from a title,
		///							  this must be this user’s user ID.
		///
		///		\param	titleID		The title ID to get OnLive Points for.
		///							  When called from a title,
		///							  this must be this title’s title ID.
		///
		///		\return	OLStatus	OL_SUCCESS normal operation.
		///							OL_INVALID_PARAM one of the params is invalid.
		///							OL_INTERNAL_IO_ERROR communication with OSP
		///							  could not complete, this is a fatal error.
		///
		virtual OLStatus GetPointsForTitle(OLContext context, OLID userID, OLID titleID) = 0;


		///
		/// \ingroup OLACHIEVEMENT_RETRIEVE_ONLIVE_POINTS
		/// \brief Gets total OnLive Points for the user (for all titles).
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\param	userID		The user ID to get OnLive Points for.
		///							  When called from a title,
		///							  this must be this user’s user ID.
		///
		///		\return	OLStatus	OL_SUCCESS normal operation.
		///							OL_INVALID_PARAM one of the params is invalid.
		///							OL_INTERNAL_IO_ERROR communication with OSP
		///							  could not complete, this is a fatal error.
		///
		virtual OLStatus GetPointsTotal(OLContext context, OLID userID) = 0;
	};


	///
	/// \ingroup OLACHIEVEMENTS_API OLACHIEVEMENT_API_EVENT_HANDLING
	/// \class OLAchievementEventHandler
	///
	/// \brief Handles events from OnLive Service Platform (OSP).
	///
	/// Event handler for the OLAchievement API.  Used to receive asynchronous events.	///
	///
	class OLAchievementEventHandler
	{
		public:


		///
		/// \ingroup OLACHIEVEMENT_API_EVENT_HANDLING
		/// \brief Event returning an OLStatus code result.
		///
		///		\param[in] context		The matching context you gave to the request for this result.
		///
		///		\param[in] status		An OLStatus code indicating the result of your request (typically an error).
		///
		///	\return An OLStatus code indicating if you have handled the event.
		///		\retval OL_SUCCESS				The event was handled.
		///		\retval OL_NOT_IMPLEMENTED		The event was ignored.
		///
		virtual OLStatus StatusResult(OLContext context, OLStatus status) = 0;


		/// \ingroup OLACHIEVEMENT_API_EVENT_HANDLING
		/// \brief Provides the ID for a container.
		///
		/// Provides the ID for the data container requested by an olapi::OLUserSession function.
		/// Use the container ID with GetAchievementContainer.
		///
		///		\param	context			Pairs asynchronous operations.
		///		\param	id				Identifies container that has the requested data.
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		///								
		///								OL_SUCCESS.
		///
		virtual OLStatus ContainerIDResult(OLContext context, OLID id) = 0;


		///
		/// \ingroup OLACHIEVEMENT_API_EVENT_HANDLING
		/// \brief Contains a null terminated list of IDs the application requested using an OLAchievement function
		/// call.
		///
		/// Provides a null terminated list of IDs the application requested, using an OLAchievement function call
		/// such as OLAchievement::GetAchievementDefContainerIDList.
		///
		///		\param context			Matches the API call with the corresponding
		///								OLAppSession::HandleEvent. Pass a unique value.
		///								This operation is asynchronous.
		///
		///		\param	idlist			Pointer to null terminated list of IDs.
		///
		///		\param	containerlistid	ID of the list holding the container IDs.
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		///								
		///								OL_SUCCESS.
		///
		virtual OLStatus IDListResult(OLContext context, OLIDList *idlist, OLID *containerlistid) = 0;


		///
		/// \ingroup OLACHIEVEMENT_RETRIEVE_ONLIVE_POINTS
		/// \brief Contains the points value the application requested using an OLAchievement function call.
		///
		/// Provides the points value the application requested, using an OLAchievement function call
		/// such as OLAchievement::GetPointsForTitle() or OLAchievement::GetPointsTotal().
		///
		///		\param context			Matches the API call with the corresponding
		///								OLAppSession::HandleEvent. Pass a unique value.
		///								This operation is asynchronous.
		///
		///		\param	points			The points requested.
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		virtual OLStatus PointsResult(OLContext context, unsigned int points) = 0;


		///
		/// \ingroup OLACHIEVEMENT_SET_AND_GET_ACHIEVEMENTS
		/// \brief Update that informs the title that an Achievement was updated.
		///
		/// Whenever an Achievement is updated, this notification will be called.
		///   This is most likely a direct result of a SetAchievementBool() call.
		///   This notification is only called when the given achievement is set to true the first time.
		///
		///		\param context			Matches the API call with the corresponding
		///								OLAppSession::HandleEvent. Pass a unique value.
		///								This operation is asynchronous.
		///
		///		\param	achievementID	The ID of the Achievement that was updated.
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		virtual OLStatus AchievementUpdatedBool(OLContext context, OLID achievementID) = 0;


		///
		/// \ingroup OLACHIEVEMENT_SET_AND_GET_ACHIEVEMENTS
		/// \brief Update that informs the title that an Achievement was updated.
		///
		/// Whenever an Achievement is updated, this notification will be called.
		///   This is most likely a direct result of a SetAchievementInt() call.
		///   This notification is only called when the Achievement is set to a higher value
		///   than the previously set value.
		///
		///		\param context			Matches the API call with the corresponding
		///								OLAppSession::HandleEvent. Pass a unique value.
		///								This operation is asynchronous.
		///
		///		\param	achievementID	The ID of the Achievement that was updated.
		///
		///		\param	value			The value the Achievement was updated to.
		///
		///		\param	prevValue		The previous value the Achievement was updated from (-1 if no value was set).
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		virtual OLStatus AchievementUpdatedInt(OLContext context, OLID achievementID, unsigned int value, 
											   int prevValue) = 0;

		///
		/// \ingroup OLACHIEVEMENT_SET_AND_GET_ACHIEVEMENTS
		/// \brief Update that informs the title that an Achievement was updated.
		///
		/// Whenever an Achievement is updated, this notification will be called.
		///   This is most likely a direct result of a SetAchievementOneOfSet() call.
		///   This notification is called whenever the given achievement is set to a new value.
		///
		///		\param context			Matches the API call with the corresponding
		///								OLAppSession::HandleEvent. Pass a unique value.
		///								This operation is asynchronous.
		///
		///		\param	achievementID	The ID of the Achievement that was updated.
		///
		///		\param	value			The value the Achievement was updated to.
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		virtual OLStatus AchievementUpdatedOneOfSet(OLContext context, OLID achievementID, OLID value) = 0;
	};


	///
	/// \ingroup OLACHIEVEMENT_API_EVENT_HANDLING OLACHIEVEMENTS_API
	/// \class OLAchievementCallbacks
	///
	/// \brief Callback interface to receive asynchronous notification that events are waiting.
	///
	/// Notifies the application that an event is waiting to be handled. Handled with OLAchievement::HandleEvent().
	///
	class OLAchievementCallbacks
	{
		public:

		///
		/// An event is waiting. Call OLAchievement::HandleEvent() to receive the event
		///
		/// \note Callbacks must take less than 2 milliseconds.
		///
		virtual void EventWaiting() = 0;	
	};


	/// \ingroup OLACHIEVEMENT_API_CREATE_AND_DESTROY
	///
	/// \brief DLL Export
	///
	/// DLL Export: Start the user session API and retrieve the user API interface class pointer.
	///
	///		\param	version			API version, use ACHIEVEMENT_API_VERSION
	///
	///		\param	service			Pointer to a OLAchievement class pointer.
	///
	///		\return	OLStatus		OL_SUCCESS normal operation, \n
	///								OL_INVALID_PARAM invalid parameter, version or service or both are NULL, \n
	///								OL_INVALID_VERSION version requested not supported, this is fatal, \n
	///								OL_INTERNAL_ERROR could not communicate with OSP, this is fatal, \n
	///								OL_OUT_OF_MEMORY trying to start service, If memory can not be freed, 
	///									this error is fatal, the app should start the exit process.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLGetAchievement(const char* version, OLAchievement** service);


	/// \ingroup OLACHIEVEMENT_API_CREATE_AND_DESTROY
	/// \brief DLL Export
	///
	/// DLL Export: Stop the user session API.
	///
	///		\param	service			Pointer to a OLAchievement class.
	///
	///		\return	OLStatus		OL_SUCCESS normal operation, \n
	///								OL_INVALID_PARAM invalid parameter, service is NULL.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLStopAchievement(OLAchievement* service);
	

/*! 

\addtogroup OLACHIEVEMENTS_API
	\brief Achievements are in-game trophies that each user can earn during gameplay. When a user earns an Achievement, the user receives a 
		pop-up notification and a specified amount of \ref ONLIVE_POINTS. Users are able to view their Achivements and OnLive Points score to 
		compare with their friends. The OLAchievement API allows a title to set, retrieve, and define Achievement values; access icons and videos 
		associated with Achievements; and calculate a user's OnLive Points. Your title's Achievements are defined in an Achievement Definitions YAML file.

		For information on Achievements, Achievement types, and creating an Achievement Definitions YAML File, see \ref OLACHIEVEMENTS_API_OVERVIEW.

\addtogroup OLACHIEVEMENT_API_CREATE_AND_DESTROY 
	\brief The OLAchievement singleton is created and retrieved with \ref olapi::OLGetAchievement() and destroyed 
		with \ref olapi::OLStopAchievement(). Do not destroy any of the OnLive APIs (or their handlers) until 
		after you have stopped the OnLive Services. 

		Also see:

		<ul><li> \ref SDK_SESSION_FLOW_PAGE for information about the session flow.
			<li> \ref OLSERVICE_API_CONNECTION_DISCONNECTION for information about OSP connection and disconnection.
		</ul>		
	 

\addtogroup OLACHIEVEMENT_API_EVENT_HANDLING
	\brief Callbacks and events are used for asynchronous communication to and from the OSP. 

	Also see \ref SDK_EVENTS_PAGE for an overview of callback and events.

\addtogroup OLACHIEVEMENT_RETRIEVE_DEFINITIONS
	\brief The Achievement Definitions in the Achievement Definitions YAML File can be retrieved; this allows in-game display of the 
		Achievement Definitions directly from the Achievement Definitions YAML File.		

		To retrieve Achievement Definitions, a title should call \ref olapi::OLAchievement::GetAchievementDefContainerIDList().  The result will be returned 
		to \ref olapi::OLAchievementEventHandler::IDListResult() as a list of containers.  Each container holds the definition of one Achievement.  The keys that are specified
		in each container are defined in \ref olapi::AchievementDefinitionContainerKeyNames[], which is accessed by \ref olapi::AchievementDefinitionContainerKeys.

\addtogroup OLACHIEVEMENT_RETRIEVE_ICONS_AND_VIDEO
	\brief Achievements that can be earned, Achievements that have been earned, and Achievement alerts that notify users of an earned Achievement can be displayed 
		as a still image or video icon.
	
		To retrieve icon and video assets that are associated with Achievement Definitions, a title should call
		\ref olapi::OLAchievement::GetAchievementMediaIconContainerIDList() or \ref olapi::OLAchievement::GetAchievementMediaVideoContainerIDList().  The result will be returned
		to \ref olapi::OLAchievementEventHandler::IDListResult() as a list of containers.  Each container holds the definition of one icon or video asset.  The keys that are specified
		in each container are defined in \ref olapi::AchievementMediaContainerKeyNames[], which is accessed by \ref olapi::AchievementMediaContainerKeys.

\addtogroup OLACHIEVEMENT_SET_AND_GET_ACHIEVEMENTS
	\brief Three different Achievement types can be set and retrieved: Bool, Int, and OneOfSet.
	
	To retrieve Achievements, titles should call \ref olapi::OLAchievement::GetAchievementsContainerIDList(). A list of containers, 
	each holding the value of one Achievement, is returned from \ref olapi::OLAchievementEventHandler::IDListResult(). The list of containers 
	will only contain Achievement that have had a value set. If no values have been set, the title will instead receive a callback from
	\ref olapi::OLAchievementEventHandler::StatusResult() with a value of OL_NO_ACHIEVEMENTS.  The keys specified in each container 
	are defined in \ref olapi::AchievementContainerKeyNames[], which is accessed by \ref olapi::AchievementContainerKeys.

	Three different Achievement types can be set: bool, int, and OneOfSet. Titles set the achievement types as follows:
		
		<ul>
			<li><b>Bool</b>:The title should call \ref olapi::OLAchievement::SetAchievementBool(); if the call was valid and the operation 
							completes, the title will receive a callback from \ref olapi::OLAchievementEventHandler::AchievementUpdatedBool(). 
							Note that a Bool Achievement type can only be set to true and can only be set once. </li><br>

			<li><b>Int</b>: The title should call \ref olapi::OLAchievement::SetAchievementInt(); if the call was valid and the operation 
							completes, the title will receive a callback from \ref olapi::OLAchievementEventHandler::AchievementUpdatedInt(). 
							Note that a Int Achievement type can only be set to a higher value and cannot be set to a value higher than the max 
							value of the Achievement. </li><br>

			<li><b>OneOfSet</b>: The title should call \ref olapi::OLAchievement::SetAchievementOneOfSet(); if the call was 
							valid, and the operation completes , the title will receive a callback from 
							\ref olapi::OLAchievementEventHandler::AchievementUpdatedOneOfSet(). Note that a OneOfSet Achievement type can be set 
							to any value.</li>
		</ul>

	If the calls to set any of the achievement types was not valid, the title will receive a callback from \ref olapi::OLAchievementEventHandler::StatusResult().		
		

\addtogroup OLACHIEVEMENT_RETRIEVE_ONLIVE_POINTS
	\brief The OnLive Points score each user receives for an Achievement is based on the point values assigned to the Achievements defined in the 
	Achievement Definitions YAML File.
	
	To retrieve the user's current \ref ONLIVE_POINTS, a title can call \ref olapi::OLAchievement::GetPointsForTitle() or 
	\ref olapi::OLAchievement::GetPointsTotal() to retrieve either the user's OnLive Points
		for this title or total score for all titles.  Both calls trigger a callback to \ref olapi::OLAchievementEventHandler::PointsResult() 
		with the result.

*/
}; // namespace olapi

#endif // OLACHIEVEMENT_H
