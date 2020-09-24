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
// OLMedia.h $Revision: 57033 $
//

/// \file OLMedia.h
/// \addtogroup MEDIA_API
/// \brief The OLMedia interface allows players to use and manage media features including Brag Clips,
/// spectating, and raising cheers and jeers in response to other players.
///
/// The OLMedia interface allows players to use and manage media features including Brag Clips,
/// spectating, and raising cheers and jeers in response to other players.
/// Players on your title can save and share video clips of their game, called Brag Clips.
/// Users can watch others play games and even cheer the player on or jeer to voice their disapproval.
///
/// Your title has access to information related to these features, such as whether or not a
/// particular user has viewed a specified brag clip. Users or titles can determine
/// who is allow to watch game play and which brag clips to save or remove.
///
/// The OLMediaStream interface provides your application with access to media streams, for fixed assets
/// such as images and trailers as well as user-generated assets such as brag clips and spectating streams.
/// The API manages video frames in YUV420 format and audio frames as interleaved, little-endian PCM
/// samples.

#ifndef OLMEDIA_H
#define OLMEDIA_H

#include <ol_api/OLAPI.h>
//#include <ol_api/OLService.h>

namespace olapi
{
	/// \ingroup OLMEDIA_API_CREATE_AND_DESTROY
	/// \brief The default version of the OnLive OLMedia API to use in \ref olapi::OLGetMedia and \ref olapi::OLGetMediaStream.
	#define MEDIA_API_VERSION (OLAPI_VERSION)

	#define OL_MAX_MEDIA_COUNT		(1500)	///< Maximum number of media clips or id's to retrieve at once

	///
	/// \brief Media Sort Types
	///
	typedef enum OLMediaSortType
	{
		OL_MEDIA_SORT_NONE = -1,
		OL_MEDIA_SORT_MOST_RECENT,
		OL_MEDIA_SORT_ALPHABETIC,
		OL_MEDIA_SORT_RATING,
		OL_MEDIA_SORT_MOST_VIEWED,
		OL_MEDIA_SORT_MAX_TYPES
	} OLMediaSortType;

	/// 
	/// \brief An identifier used to control loaded streams.
	///
	typedef unsigned __int64 OLStreamId;

	/// \addtogroup OLMEDIA_API_BRAGCLIP_HANDLING
	///
	/// \brief Key enumeration for accessing the BragClip container
	///
	enum BragClipContainerKeys
	{
		OLKEY_RO_BragClipContainerName,						///< string, Name of Container
		OLKEY_RO_BragClipContainerListId,						///< string, ID of parent container list if any
		OLKEY_RO_BragClipId,									///< OLID, brag clip ID
		OLKEY_RO_BragClipName, 								///< string, Name of Brag Clip
		OLKEY_RO_BragClipDescription,							///< string, Description of Brag Clip
		OLKEY_RO_BragClipUrl,									///< string, Stream URL of Brag Clip
		OLKEY_RO_BragClipUserId,								///< OLID, brag clip creating user ID
		OLKEY_RO_BragClipAppId,								///< OLID, title Brag Clip was created in
		OLKEY_RO_BragClipCreationDate,						 	///< time_t, creation date in seconds from the UNIX epoch of Brag Clip
		OLKEY_RO_BragClipViews,								///< int, view count of Brag Clip
		OLKEY_RO_BragClipRating,								///< 0..1 float
		OLKEY_RO_BragClipRatingCount,						// number of times rated...
		OLKEY_RO_BragClipEventType,							///< int, when monitoring Brag Clips defines what type of event occurred OLBragClipEventType
		OLKEY_RO_BragClipOwnerPrivacySetting,					///< int, when monitoring Brag Clips defines what the owner's privacy setting is for the clip OLMediaOwnerPrivacyType
		OLKEY_RO_BragClipContainerKeysMax
	};

	/// \addtogroup OLMEDIA_API_SPECTATING_HANDLING
	///
	/// \brief Key enumeration for accessing the SpectatingStream container
	///
	enum SpectatingStreamContainerKeys
	{
		OLKEY_RO_SpectatingStreamContainerName,						///< string, Name of Container
		OLKEY_RO_SpectatingStreamContainerListId,						///< string, ID of parent container list if any
		OLKEY_RO_SpectatingStreamId,									///< OLID, spectating stream ID
		OLKEY_RO_SpectatingStreamUrl,									///< string, Stream URL of Spectating Stream
		OLKEY_RO_SpectatingStreamUserId,								///< OLID, brag clip creating user ID
		OLKEY_RO_SpectatingStreamAppId,								///< OLID, title Brag Clip was created in
		OLKEY_RO_SpectatingStreamCheers,								///< number of cheers for this stream
		OLKEY_RO_SpectatingStreamJeers,								///< number of jeers for this stream
		OLKEY_RO_SpectatingStreamPrivacyStatus,					 ///< Current state of the transient privacy status of the stream, see OLMediaStreamPrivacyStatus
		OLKEY_RO_SpectatingStreamEventType,							///< int, when monitoring Spectating Streams defines what type of event occurred OLSpectatingStreamEventType
		OLKEY_RO_SpectatingStreamOwnerPrivacySetting,					///< int, when monitoring Spectating Streams defines what the owner's privacy setting is for the stream OLMediaOwnerPrivacyType
		OLKEY_RO_SpectatingStreamNotificationLocation,					///< string, defines the rectangular location of any visible notification in the form ULX,ULY,LRX,LRY
		OLKEY_RO_SpectatingStreamViewerCount,
		OLKEY_RO_SpectatingStreamCreationTime,
		OLKEY_RO_SpectatingStreamContainerKeysMax
	};

	/// \addtogroup OLMEDIA_API_BRAGCLIP_HANDLING
	#define BRAGCLIPCONTAINERNAME	"BragClipContainer"					///< Brag Clip Container Name
	/// \addtogroup OLMEDIA_API_SPECTATING_HANDLING
	#define SPECTATINGSTREAMCONTAINERNAME	"SpectatingStreamContainer"	///< Spectating Stream Container Name

	///
	/// \brief Key strings in the BragClip container
	///
	#ifndef NO_OLAPI_DLL_LINKAGE
	extern __declspec(dllimport) char *BragClipContainerKeyNames[olapi::OLKEY_RO_BragClipContainerKeysMax];
	#endif

	///
	/// \brief Key strings in the SpectatingStream container
	///
	#ifndef NO_OLAPI_DLL_LINKAGE
	extern __declspec(dllimport) char *SpectatingStreamContainerKeyNames[olapi::OLKEY_RO_SpectatingStreamContainerKeysMax];
	#endif

	/// \addtogroup OLMEDIA_API_BRAGCLIP_HANDLING
	///
	/// \brief Event types, sent in containers when brag clip notifications are enabled via OLMedia::MonitorBragClips.
	///
	enum OLBragClipEventType
	{
		OL_BRAG_CLIP_CREATED,				///< A new brag clip was created
		OL_BRAG_CLIP_UPDATED,				///< A brag clip was updated
		OL_BRAG_CLIP_DELETED,				///< An existing brag clip was deleted
		OL_BRAG_CLIP_EVENT_MAX
	};

	/// \addtogroup OLMEDIA_API_SPECTATING_HANDLING
	///
	/// \brief Event types, sent in containers when brag clip notifications are enabled via OLMedia::MonitorSpectatingStreams.
	///
	enum OLSpectatingStreamEventType
	{
		OL_SPECTATING_STREAM_STARTED,		///< A new spectating stream has started
		OL_SPECTATING_STREAM_UPDATED,		///< A spectating stream was updated
		OL_SPECTATING_STREAM_HAD_FEEDBACK,	///< A spectating stream had feedback
		OL_SPECTATING_STREAM_ENDED,			///< An existing spectating stream ended
		OL_SPECTATING_STREAM_EVENT_MAX
	};

	///
	/// \brief Privacy types, provided in notifications about brag clips and spectating streams when enabled via 
	///		OLMedia::MonitorBragClips or OLMedia::MonitorSpectatingStreams.
	///
	enum OLMediaOwnerPrivacyType
	{
		OL_MEDIA_OWNER_PRIVACY_TYPE_ANYBODY,		///< Anybody can see the brag clip or spectating stream
		OL_MEDIA_OWNER_PRIVACY_TYPE_FRIENDS_ONLY,	///< Only friends can see the brag clip or spectating stream
		OL_MEDIA_OWNER_PRIVACY_TYPE_NO_ONE,	        ///< No one can see the brag clip or spectating stream
		OL_MEDIA_OWNER_PRIVACY_TYPE_MAX
	};

	///
	/// \brief Transient privacy status for a stream. During gameplay, the service can 
	/// notify your application that a spectating stream that it is watching has gone to
	/// a temporary privacy mode, in which user-private information is being blocked by
	/// the service in either the video, audio, or notification areas. Your application
	/// is notified so that the UI can indicate the situation accordingly.
	///
	enum OLMediaStreamPrivacyStatus
	{
		OL_MEDIA_STREAM_PRIVACY_STATUS_NO_RESTRICTION,  ///< The stream is unrestricted
		OL_MEDIA_STREAM_PRIVACY_STATUS_BLOCK_VIDEO,	 ///< The stream's video has been temporarily suspended by the service
		OL_MEDIA_STREAM_PRIVACY_STATUS_BLOCK_AUDIO,	 ///< The stream's audio has been temporarily suspended by the service
		OL_MEDIA_STREAM_PRIVACY_STATUS_BLOCK_VIDEO_AND_AUDIO, ///< The stream's video and audio have been temporarily suspended by the service
		OL_MEDIA_STREAM_PRIVACY_STATUS_BLOCK_NOTIFICATION_AREA, ///< The stream is unrestricted, but spectators are required to block the notification area
		OL_MEDIA_STREAM_PRIVACY_STATUS_BLOCK_VIDEO_AND_AUDIO_AND_NOTIFICATION_AREA, ///< The stream is suspended, and the notification area must be blocked.
		OL_MEDIA_STREAM_PRIVACY_STATUS_BLOCK_AUDIO_AND_NOTIFICATION_AREA, ///< The stream's audio is suspended, and spectators are required to block the notification area
		OL_MEDIA_STREAM_PRIVACY_STATUS_MAX
	};


	///
	/// \brief Brag clip events as strings
	///
	#ifndef NO_OLAPI_DLL_LINKAGE
	extern __declspec(dllimport) char *BragClipEventNames[OL_BRAG_CLIP_EVENT_MAX];
	#endif

	///
	/// \brief Spectating stream events as strings
	///
	#ifndef NO_OLAPI_DLL_LINKAGE
	extern __declspec(dllimport) char *SpectatingStreamEventNames[OL_SPECTATING_STREAM_EVENT_MAX];
	#endif

	/// \addtogroup OLMEDIA_API_SPECTATING_HANDLING
	///
	/// \brief Spectator Feedback Type
	///

	enum OLSpectatorFeedbackType
	{
		OL_SPECTATOR_FEEDBACK_CHEER,	///< The spectator enjoyed what he or she was seeing
		OL_SPECTATOR_FEEDBACK_JEER		///< The spectator hated what he or she was seeing
	};

	///
	/// \brief Media Types
	///
	enum OLMediaType
	{
		OL_MEDIA_TYPE_NONE = -1,		///< Unknown type of stream
		OL_MEDIA_TYPE_BRAG_CLIP,		///< A Brag Clip
		OL_MEDIA_TYPE_STREAM,			///< A Spectating Stream
		OL_MEDIA_TYPE_OLMC,				///< An Onlive Media Container
		OL_MEDIA_TYPE_MAX
	};
	
	class OLMediaEventHandler;
	class OLMediaCallbacks;
	class OLMediaStreamEventHandler;
	class OLMediaStreamCallbacks;

	///
	/// \class OLMedia
	/// \brief Media Interface Class
	///
	/// The OLMedia interface allows players to use and manage media features including Brag Clips,
	/// spectating, and raising cheers and jeers in response to other players.
	/// Players on your title can save and share video clips of their game, called Brag Clips.
	/// Users can watch others play games and even cheer the player on or jeer to voice their disapproval.
	///
	/// Your title has access to information related to these features, such as whether or not a
	/// particular user has viewed a specified brag clip. Users or titles can determine
	/// who is allow to watch game play and which brag clips to save or remove.
	///

	class OLMedia : public OLEventBase
	{
		public:

		/// \addtogroup OLMEDIA_API_EVENT_HANDLING
		/// 
		/// \name OLMedia Callbacks & Event Handling
		/// 

		/// \brief Allows client to be notified that
		/// an event is available.
		///
		/// Sets up a callback class object to be notified when an event is waiting to be handled. Event
		/// notification is achieved by calling the callback class object's EventWaiting() function.\n\n
		/// The application can use this to optimize Event Handling.
		///
		///		\param	callback		Pointer to OLAppEventWaitingCallback to call when an event is available
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM invalid or NULL parameter callback.
		///
		virtual OLStatus SetEventWaitingCallback(OLMediaCallbacks* callback) = 0;

		///
		/// \brief Sets handler to call when an application event is processed.
		///
		/// Handler used by application to process events.
		///
		///		\param	handler			Pointer to OLMediaEventHandler interface
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM invalid or NULL parameter handler.
		///
		virtual OLStatus SetEventHandler(OLMediaEventHandler* handler) = 0;

		///
		/// \brief Dispatches the next media api event.
		///
		/// Dispatches the next media api event, if available, to the handler indicated in SetEventHandler.  Implements an
		/// OLMediaEventHandler() interface in the application to process events from each of the APIs.
		///
		///		\param	handler			Pointer to OLMediaEventHandler interface
		///
		///		\param	time			Time to wait in milliseconds for next event, OL_INFINITE will wait until next event
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM invalid or NULL parameter handler and handler not already set, \n
		///								OL_EVENT_TIMEOUT if no events available.
		///
		virtual OLStatus HandleEvent(OLMediaEventHandler* handler, unsigned int time) = 0;

		///
		/// \brief Retrieves next event, if available.
		///
		/// Call upon event callback or per frame to handle (dispatch) the next event.
		/// The event handler must be previous setup with a SetEventHandler call in each API.
		///
		///		\param	time			Time to wait for next event in milliseconds or\n
		///								OL_INFINITE to wait for next event.
		///
		///		\return	OLStatus		OL_SUCCESS Normal operation.\n
		///								OL_INVALID_PARAM Invalid or NULL parameter handler and handler not already set. \n
		///								OL_EVENT_TIMEOUT If no events available.
		///
		virtual OLStatus HandleEvent(unsigned int time) = 0;

		/// 
		/// 

		/// \addtogroup OLMEDIA_API_BRAGCLIP_HANDLING
		///
		/// \brief Get metadata for available brag clips.
		///
		///   Retrieve the metadata for brag clips visible to the requesting User ID matching the
		///   constraints given in the mixed ID list. Generates an IDListResult event with container IDs,
		///   one for each matching brag clip.
		///
		///   The mixed ID list is a set of IDs, of type OL_IDENTITY_ID, OL_TITLE_ID, and OL_BRAG_CLIP_ID. For
		///   each ID type, if one is given, the returned clip must match one of them. The returned clips
		///   then must match for all types that are given. So a query with OL_IDENTITY_IDs given for Fred and
		///   Bob, with OL_TITLE_IDs given for AwesomeGame and CoolGame, would match any brag clips made by
		///   either Fred or Bob which were also made in either AwesomeGame and CoolGame.
		///
		///	  \param	context		Used to pair up async operations
		///	  \param	requestingUserID  		The user ID requesting the brag clips.
		///	  \param	mixedIDs	A list of IDs to match, see above.
		///
		///   \return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus GetBragClips(OLContext context, OLID requestingUserID, OLIDList *mixedIDs) = 0;

		/// Retrieve the metadata for media clips visible to the requesting User ID matching the
		/// constraints given in the mixed ID list. Generates an IDListResult event with media IDs,
		/// one for each matching brag clip or spectating stream. These IDs can then be passed to GetBragClips
		/// or GetSpectatingStreams to get the full metadata for those media.
		///
		/// The mixed ID list is a set of IDs, of type OL_IDENTITY_ID, OL_TITLE_ID, and OL_BRAG_CLIP_ID. For
		/// each ID type, if one is given, the returned clip must match one of them. The returned clips
		/// then must match for all types that are given. So a query with OL_IDENTITY_IDs given for Fred and 
		/// Bob, with OL_TITLE_IDs given for AwesomeGame and CoolGame, would match any brag clips made by
		/// either Fred or Bob which were also made in either AwesomeGame and CoolGame.
		///
		/// Params
		///		\param	context				Used to pair up async operations
		///	  	\param	requestingUserID  	The user ID requesting the brag clips.
		///	  	\param	mixedIDs			A list of IDs to match, see above.
		///	  	\param	media_type			The media type requested, typically OL_MEDIA_TYPE_BRAG_CLIP or OL_MEDIA_TYPE_STREAM
		///		\param	sort_type			The order to sort the results in, see OLTypes.h
		///		\param	max_count			The maximum number of results allowed.
		///
		/// Returns
		///   \return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus GetMediaIDs(OLContext context, OLID requestingUserID, OLIDList *mixedIDs, OLMediaType media_type, OLMediaSortType sort_type, int max_count) = 0;

		/// \addtogroup OLMEDIA_API_SPECTATING_HANDLING
		///
		/// \brief Get metadata for available spectating streams.
		///
		///	Retrieve the metadata for spectating streams visible to the requesting user ID matching
		///	the constraints given in the mixed ID list. Generates an IDListResult event with container IDs,
		///	one for each matching spectating stream.
		///
		///	The mixed ID list is a set of IDs, of type OL_IDENTITY_ID, OL_TITLE_ID, and OL_STREAM_ID. For
		///	each ID type, if one is given, the returned stream must match one of them. The returned streams
		///	then must match for all types that are given. So a query with OL_IDENTITY_IDs given for Fred and
		///	Bob, with OL_TITLE_IDs given for AwesomeGame and CoolGame, would match any spectating streams
		///	available from either Fred or Bob which were also in either AwesomeGame and CoolGame.
		///
		///		\param	context				Used to pair up async operations
		///		\param	requestingUserID	The user ID requesting the spectating streams.
		///		\param	mixedIDs			A list of IDs to match, see above.
		///
		///		\return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus GetSpectatingStreams(OLContext context, OLID requestingUserID, OLIDList *mixedIDs) = 0;

		/// \addtogroup OLMEDIA_API_BRAGCLIP_HANDLING
		///
		/// \brief Get media ratings that a user has provided for clip IDs.
		///
		///   Gets the ratings that a user has given for the set of brag clips provided.
		///   Causes an IDListResult event with container IDs, one for each brag clip requested. The containers
		///   define the OLKEY_RO_BragClipId key, and the OLKEY_RO_BragClipRatings key, but no others.
		///
		///	  	\param	context		Used to pair up async operations
		///	  	\param	userID		The user ID to find ratings for
		///	  	\param	bragClipIDs	The brag clip IDs to get ratings for
		///
		///		\return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus GetMediaRatingsByUserForIDs(OLContext context, OLID userID, OLIDList *bragClipIDs) = 0;

		/// \addtogroup OLMEDIA_API_BRAGCLIP_HANDLING
		/// \brief Start monitoring creation, update, and deletion of brag clips.
		///
		///   Tells the media service to monitor create, update and delete events for brag clips that match 
		///   the appID and userID supplied.  Title will be notified of matching events via the
		///   event handler as a ContainerIDResult event.
		///
		///   *Note*  the context provided to this function must be a Reserved Context. See /ref CONTEXT_PAGE_RESERVED_CONTEXT
		///
		///	  	\param	context		A reserved context used to pair up async operations
		///   	\param	appID		The application ID to retrieve brag clips for, or OLID_INVALID for all.
		///   	\param	userID		The user ID to retrieve brag clips for, or OLID_INVALID for all.
		///
		///		\return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus MonitorBragClips(OLContext context, OLID appID, OLID userID) = 0;

		/// \addtogroup OLMEDIA_API_BRAGCLIP_HANDLING
		/// \brief Stop monitoring brag clip updates.
		///
		///   Tells the media service to stop monitoring brag clip create/delete events.
		///
		///	  	\param	context		Used to pair up async operations, use the same context used for MonitorBragClips
		///
		///		\return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus StopMonitoringBragClips(OLContext context) = 0;

		/// \addtogroup OLMEDIA_API_SPECTATING_HANDLING
		///
		/// \brief Monitor the start, updates, and end of spectating streams.
		///
		///   Tells the media service to monitor create, update and delete events for spectating streams that match 
		///   the appID and userID supplied.  Title will be notified of matching events via the
		///   event handler as a ContainerIDResult event.
		///
		///   *Note*  the context provided to this function must be a Reserved Context. See /ref CONTEXT_PAGE_RESERVED_CONTEXT
		///
		///	  	\param	context		A reserved context used to pair up async operations
		///   	\param	appID		The application ID to retrieve spectating streams for, or OLID_INVALID for all.
		///   	\param	userID		The user ID to retrieve spectating streams for, or OLID_INVALID for all.
		///
		///		\return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus MonitorSpectatingStreams(OLContext context, OLID appID, OLID userID) = 0;

		/// \addtogroup OLMEDIA_API_SPECTATING_HANDLING
		///
		/// \brief Stop monitoring spectating stream updates.
		///
		///   Tells the media service to stop monitoring spectating stream start/end events.
		///
		///	  	\param	context		Used to pair up async operations, use the same context you used for
		///			  				MonitorSpectatingStreams
		///
		///		\return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus StopMonitoringSpectatingStreams(OLContext context) = 0;

		/// \addtogroup OLMEDIA_API_BRAGCLIP_HANDLING
		/// \brief Set the user's rating for a brag clip.
		///
		///	Tells the media service what the user's rating is for the given brag clip.
		///
		///	  	\param	context		Used to pair up async operations
		///	  	\param	bragClipID	The Brag Clip ID to rate
		///   	\param	userID	  The user ID of the rating user
		///	  	\param	rating		A number from 0 to 4, with 4 being better
		///
		///		\return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus RateBragClip(OLContext context, OLID bragClipID, OLID userID, double rating) = 0;

		/// \addtogroup OLMEDIA_API_SPECTATING_HANDLING
		///
		/// \brief Provide feedback for a spectating stream.
		///
		///	Tells the media service that the user has CHEER'd or JEER'd the given spectating stream.
		///
		///	  	\param	context				Used to pair up async operations
		///	  	\param	spectatingStreamID	The spectating stream ID to provide feedback on.
		///	  	\param	userID			  The user ID of the rating user
		///	  	\param	type				The type of feedback provided, CHEER or JEER
		///
		///		\return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus AudienceFeedbackForSpectatingStream(OLContext context, OLID spectatingStreamID, OLID userID, OLSpectatorFeedbackType type) = 0;

		/// \addtogroup OLMEDIA_API_BRAGCLIP_HANDLING
		/// \brief Notify that a brag clip was viewed.
		///
		///	Notifies the media service that the user has viewed a brag clip.
		///
		///	  	\param	context		Used to pair up async operations
		///	  	\param	bragClipID	The brag clip that a user viewed.
		///	  	\param	appsessionID The application session of the viewer
		///
		///		\return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus BragClipViewed(OLContext context, OLID bragClipID, OLID appsessionID) = 0;

		/// \addtogroup OLMEDIA_API_SPECTATING_HANDLING
		///
		/// \brief Notify that a viewer started viewing a stream 
		///
		///	The Title can notify the media service that the user has started viewing a spectating stream.
		///
		///	  	\param	context				Used to pair up async operations
		///	  	\param	spectatingStreamID	The Spectating Stream the user began to view
		///	  	\param	appsessionID		The application session of the spectator
		///
		///		\return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus ViewerBeganSpectating(OLContext context, OLID spectatingStreamID, OLID appsessionID) = 0;

		/// \addtogroup OLMEDIA_API_SPECTATING_HANDLING
		///
		/// \brief Notify that a viewer has stopped viewing a stream.
		///
		///	Notifies the media service that the user has finished viewing a spectating stream.
		///
		///	  	\param	context				Used to pair up async operations
		///	  	\param	spectatingStreamID	The Spectating Stream the user began to view
		///	  	\param	appsessionID		The application session of the spectator
		///
		///		\return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus ViewerEndedSpectating(OLContext context, OLID spectatingStreamID, OLID appsessionID) = 0;

		/// \addtogroup OLMEDIA_API_BRAGCLIP_HANDLING
		/// \brief Remove a brag clip permanently.
		///
		///		Tells the media service to permanently remove a brag clip.
		///
		///		\param	context		Used to pair up async operations
		///		\param	bragClipID	The Brag Clip ID to rate
		///
		///		\return	OLStatus	OL_SUCCESS if the request was successfully sent,\n
		///						 	OL_INVALID_PARAM if an invalid parameter was provided,\n
		///							OL_INTERNAL_ERROR if the request could not be sent.
		///
		virtual OLStatus RemoveBragClip(OLContext context, OLID bragClipID) = 0;
	};

	///
	/// \class OLMediaStream
	/// \brief Media Stream Interface Class
	///
	/// The OLMediaStream interface provides your application with access to media streams, for fixed assets
	/// such as images and trailers as well as user-generated assets such as brag clips and spectating streams.
	/// The API manages video frames in YUV420 format and audio frames as interleaved, little-endian PCM
	/// samples. 
	///
	/// The API also provides jitter buffering, such that the application is notified when video or audio needs
	/// to be updated.
	///
	class OLMediaStream
	{
	public:

		/// \addtogroup OLMEDIA_API_EVENT_HANDLING
		/// 

		/// \brief Allows client to be notified that
		/// a stream event is available.
		///
		/// Sets up a callback class object to be notified when a stream event is waiting to be handled. Event
		/// notification is achieved by calling the callback class object's StreamEventWaiting() function.\n\n
		/// The application can use this to optimize Event Handling.
		///
		///		\param	callback		Pointer to OLAppEventWaitingCallback to call when an event is available
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM invalid or NULL parameter callback.
		///
		virtual OLStatus SetStreamEventWaitingCallback(OLMediaStreamCallbacks* callback) = 0;

		///
		/// \brief Sets handler to call when an application stream event is processed.
		///
		/// Handler used by application to process events.
		///
		///		\param	handler			Pointer to OLMediaStreamEventHandler interface
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM invalid or NULL parameter handler.
		///
		///
		virtual OLStatus SetStreamEventHandler(OLMediaStreamEventHandler* handler) = 0;

		///
		/// \brief Dispatches the next media stream event.
		///
		/// Dispatches the next media stream api event, if available, to the handler indicated in SetEventHandler.  Implements an
		/// OLMediaEventHandler() interface in the application to process events from each of the APIs. Unlike other event handler
		/// APIs in the OSP, this event handler does not support waiting for an event, the application is required to use its own
		/// mechanisms if such behavior is desired.
		///
		///		\param	handler			Pointer to OLMediaStreamEventHandler interface, or NULL to use the one 
		///							 	previously set with SetStreamEventHandler
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM invalid or NULL parameter handler and handler not already set.
		///
		virtual OLStatus HandleStreamEvent(OLMediaStreamEventHandler* handler) = 0;
		/// 

		///
		/// \brief Determines if a URL is for a local file or remote stream.
		///
		///   Examines the URL provided and returns true if the URL refers to a file on the local filesystem.
		///   Remove stream URLs tend to have the form onlive://mediaserver/somestream.mov, while local files
		///   tend to start with file:/// or c:
		/// 
		///		\param	url The url to examine
		///
		///		\return	true if url refers to a local file, false otherwise.
		///
		virtual bool LocationIsFile(const char *url) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Load a video/audio stream from a given URL.
		///
		///   Initiates a background load of the stream specified by the given URL. The file will be loaded either
		///   on a mediaserver, or on the local computer, and its metadata will be parsed. An event of type
		///   StreamLoaded or StreamLoadFailed will be generated as a result of this request.
		///
		///   *Note*: It is an error to call any functions that relate to the stream between calling LoadStream 
		///			  and your title recieving the StreamLoaded callback.
		/// 
		///   	\param	url				Specifies the stream to load, typically of the form onlive://mediaserver/some-movie.mov
		///
		///		\return	OLStreamId		A valid stream ID if start of stream load was successful
		///								OLMediaStream::INVALID_STREAM_ID if the stream failed to load
		///
		virtual OLStreamId LoadStream(const char *url) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Unload a previously loaded stream
		///
		///   Initiates a background unload of the stream specified by the given ID. Further callback events
		///   for the given stream ID may continue to be generated for a brief time, though stream updates should
		///   terminate immediately. The stream will stop playback and its associated resources will be freed.
		///   An event of type StreamUnloaded will be generated as a result of this call.
		/// 
		///   	\param	streamId 	A previously returned stream ID from the /ref LoadStream or /ref LoadStreamImmedate 
		///
		///   	\return	OLStatus 	OL_SUCCESS the stream was found and the operation was enqueued,\n
		///							OL_INVALID_PARAM the stream was not found
		///
		virtual OLStatus UnloadStream(OLStreamId streamId) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Load a stream such that it is immediately ready for use.
		///
		///   Loads the stream specified by the given URL, immediately. Blocks the calling thread until the stream
		///   has been loaded. A description of the stream is immediately available via the streamDescription()
		///   method. No event is generated as a result of this method.
		/// 
		///		\param	url				Specifies the stream to load, typically of the form onlive://mediaserver/some-movie.mov
		///		\param	streamIsLive	Informs the application if the stream is from a live session or prerecorded
		///		\param	duration_us		Gives the duration of the stream, if it is not live
		///		\param	width			Gives the width of the video stream in pixels
		///		\param	height			Gives the height of the video stream in pixels
		///		\param	sampleRate		Gives the audio sample rate, if the stream has audio, or 0 if not
		///		\param	channelCount	Gives the number of channels in the audio if the stream has audio, 0 if not
		///		\param	bitsPerSample	Gives the number of bits in each audio sample if the stream has audio, 0 if not
		/// 
		///		\return	OLStreamId		A valid stream ID if load of stream was successful
		///								OLMediaStream::INVALID_STREAM_ID if the stream failed to load 
		///
		virtual OLStreamId LoadStreamImmediate(const char *url, bool *streamIsLive, 
											   unsigned __int64 *duration_us, 
											   unsigned int *width, unsigned int *height,
											   unsigned int *sampleRate, unsigned int *channelCount, 
											   unsigned int *bitsPerSample) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Unload a stream immediately.
		///
		///   Immediately stops the stream, unloads it, and frees its resources. No event is generated as a
		///   result of this method.
		///
		///   	\param	streamId		A previously returned stream ID from the /ref LoadStream or /ref LoadStreamImmediate 
		///
		///   	\return	OLStatus 	OL_SUCCESS the stream was found and the operation was enqueued,\n
		///							OL_INVALID_PARAM the stream was not found
		///
		virtual OLStatus UnloadStreamImmediate(OLStreamId streamId) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Sets the texture size that the application will copy video data into.
		///
		///   Specifies the texture size for the YUV420 data that will be provided to the application for the
		///   given stream ID. It is safe to set this to the video width and height specified when the stream
		///   was loaded. 
		///
		///	  *Note*: It is an error to play a stream whose texture size has not been specified.
		///
		///   	\param	streamId		Is a previously returned stream ID from the /ref LoadStream or /ref LoadStreamImmediate 
		///   	\param	textureWidth	Specifies the width in pixels of the texture displaying the video
		///   	\param	textureHeight	Specifies the height in pixels of the texture displaying the video
		///
		///   	\return	OLStatus 	OL_SUCCESS the stream was found and the operation was enqueued,\n
		///							OL_INVALID_PARAM the stream was not found
		///
		virtual OLStatus SetTextureSize(OLStreamId streamId, unsigned int textureWidth,
										unsigned int textureHeight) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		/// 
		/// \brief Start playback of a stream.
		///
		///   Begins playback of a previously loaded stream for which a texture size was also set via
		///   the setTextureSize() method. 
		///
		///   	\param	streamId		A previously returned stream ID from the /ref LoadStream or /ref LoadStreamImmedate 
		///
		///   	\return	OLStatus 	OL_SUCCESS the stream was found and the operation was enqueued,\n
		///							OL_INVALID_PARAM the stream was not found
		///
		virtual OLStatus PlayStream(OLStreamId streamId) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Suspend playback of a stream
		///
		///   Suspends playback of a previously loaded stream for which a texture size was also set via
		///   the setTextureSize() method. Calling this method before a stream has finished loading is 
		///   allowed.
		///
		///   	\param	streamId	A previously returned stream ID from the LoadStream or LoadStreamImmedate 
		///
		///   	\return	OLStatus 	OL_SUCCESS the stream was found and the operation was enqueued,\n
		///							OL_INVALID_PARAM the stream was not found
		///
		virtual OLStatus PauseStream(OLStreamId streamId) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		/// 
		/// \brief Seek within a stream
		///
		///   Moves the playback position to the given offset within a stream previously loaded via the loadStream
		///   or loadStreamImmediate methods. offset is a floating point value in the range of 0.0-1.0, with 0.0 
		///   corresponding to the beginning of the stream and 1.0 corresponding to the end. 
		///
		///   	\param	streamId	A previously returned stream ID from the LoadStream or LoadStreamImmedate 
		///   	\param	offset		A floating point value in the range 0.0-1.0, with 0.0 
		///	 						corresponding to the beginning of the stream and 1.0 corresponding to the end. 
		///
		///   	\return	OLStatus 	OL_SUCCESS the stream was found and the operation was enqueued,\n
		///							OL_INVALID_PARAM the stream was not found
		///
		virtual OLStatus SeekStream(OLStreamId streamId, double offset) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Set a stream to looping playback
		///
		///   Specifies whether a stream previously loaded via the LoadStream or LoadStreamImmediate methods
		///   should loop or not. A non-looping stream will pause at the end of playback, whereas a looping
		///   stream will continue from the beginning of the stream continuously and linearly. If a stream is
		///   looping, the StreamLastFrame callback will be initiated only *once*.
		///
		///   	\param	streamId	A previously returned stream ID from the LoadStream or LoadStreamImmedate 
		///   	\param	looping		Specifies whether playback should loop or not.
		///
		///   	\return	OLStatus 	OL_SUCCESS the stream was found and the operation was enqueued,\n
		///							OL_INVALID_PARAM the stream was not found
		///
		virtual OLStatus SetLooping(OLStreamId streamId, bool looping) = 0;

		/// \addtogroup OLMEDIA_API_STILL_IMAGES
		/// 
		/// \brief Start an image load operation in the background.
		///
		///   Initiates a load of an image file in the background from the local disk or a remote mediaserver.
		///   Local URLs have the form file:///some-file.jpg and mediaserver URLs have the form 
		///   onlive://mediaserver/foo.jpg
		///   The image file is returned as a blob of bytes via the ImageLoaded event, or an error is specified
		///   by the ImageLoadFailed event.
		///
		///   	\param	url			The URL of the image to load, of the form onlive://mediaserver/some-file.jpg or as a local file
		/// 
		///   	\return	OLStatus 	OL_SUCCESS the URL was valid and the operation was enqueued,\n
		///							OL_INVALID_PARAM the stream was not found
		///
		virtual OLStatus LoadImage(const char *url) = 0;

		/// \addtogroup OLMEDIA_API_STILL_IMAGES
		///
		/// \brief Load an image immediately.
		///
		///   Immediately loads an image file from the given URL, either from the local disk or a remote mediaserver.
		///   Local URLs have the form file:///some-file.jpg and mediaserver URLs have the form 
		///   onlive://mediaserver/foo.jpg
		/// 
		///   	\param	url				The URL of the image to load, of the form onlive://mediaserver/some-file.jpg or as a local file
		///   	\param	data			Will be filled with the bytes from the file. 
		///   	\param	maxByteCount	Specifies the maximum number of bytes that can be copied into data.
		///   	\param	actualByteCount	Will be filled with the number of bytes copied into the data buffer.
		/// 
		///   	\return	OLStatus 	OL_SUCCESS the URL was valid and the operation was enqueued,\n
		///							OL_INVALID_PARAM the image was not found
		///
		virtual OLStatus LoadImageImmediate(const char *url, unsigned char *data, unsigned int maxByteCount, 
											unsigned int *actualByteCount) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Update the video and/or audio for a stream
		///
		///   Checks the given stream ID for updates, and if any are present, invokes the VideoUpdate or
		///   AudioUpdate callbacks. The application can call this method whenever it gets an 
		///   StreamNeedsUpdate callback, or it can poll this method once per frame to check for updates.
		///
		///   	\param	streamId		is a previously returned stream ID from the LoadStream or LoadStreamImmedate 
		///		\param	handler			Pointer to OLMediaStreamEventHandler interface, or NULL to use the one 
		///							 previously set with SetStreamEventHandler
		/// 
		///   	\return	OLStatus 	OL_SUCCESS the stream was found and the operation was enqueued,\n
		///							OL_INVALID_PARAM the stream was not found
		///
		virtual OLStatus UpdateStream(OLStreamId streamId, OLMediaStreamEventHandler* handler) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Sets the requested presentation offsets for video and audio frames for all streams.
		///
		///   Each frame in a stream has a presentation offset, which is the time at which the frame should be
		///   visible or audible by the user. By default, a stream will get a video update at exactly the presentation
		///   time for the frame, and an audio update 20ms before the audio is to be presented. The Media SDK allows the
		///   application to adjust these offsets. The offset can be positive, in which case a frame will be given to
		///   the application before it is to be presented by the given number of milliseconds, or negative, in which case
		///   the frame will be held for that many milliseconds. It is recommended that the application set these numbers
		///   before loading any streams for the most predictable results.
		///
		///   This setting has no effect on live streams.
		///
		///   \param videoOffset_ms is the number of milliseconds before presentation time that video frames  
		///				are given to the title (default 0).
		///   \param audioOffset_ms is the number of milliseconds before presentation time that audio frames 
		///				are given to the title (default 20ms).
		///
		virtual void SetStreamDisplayOffsets(int videoOffset_ms, int audioOffset_ms) = 0;

		///
		/// \brief   A constant defining an invalid stream. This is used to signal errors.
		///
		static const OLStreamId INVALID_STREAM_ID = 0;
	};


	///
	/// \class OLMediaEventHandler
	///
	/// \brief Handles events from OnLive Service Platform (OSP).
	///
	/// Event handler for the OLMedia API.  Used to receive asynchronous events.	///
	///
	class OLMediaEventHandler
	{
		public:

		/// \addtogroup OLMEDIA_API_EVENT_HANDLING
		/// 
		///
		/// \brief Provides the ID for a container.
		///
		/// Provides the ID for the data container requested by an olapi::OLMedia function.
		/// This is used to receive updates when using MonitorBragClips and MonitorSpectatingStreams.
		///
		///		\param	context			Pairs asynchronous operations.
		///		\param	id				Identifies container that has the requested data.
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		virtual OLStatus ContainerIDResult(OLContext context, OLID id) = 0;

		/// \brief Receive the ID or IDs requested.
		///
		/// 	GetBragClips: Returns a set of container IDs with metadata for each matching brag clip
		/// 	GetSpectatingStreams: Returns a set of container IDs with metadata for each matching spectatingStream
		/// 	GetMediaRatingsByUserForIDs: Returns a set of Container IDs with info for each requested brag clip rating by that user
		/// 	GetMediaIDs: Returns a set of media IDs, which can be passed to GetBragClips or GetSpectatingStreams respectively
		/// 
		/// 	\param	context		Used to pair up async operations
		/// 	\param	idlist		pointer to null terminated list of IDs
		/// 	\param	containerlistid	pointer to ID of the list holding the container id's.
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

		/// \brief Notify that an information retrieval error occurred.
		///
		/// Notifies the application that a prior information query, e.g. for details of a brag clip or
		/// spectating stream, was unable to complete.
		///
		///	  	\param	context		Used to pair up async operations
		///	  	\param	error		The error code describing why the information was unavailable
		///	  	\param	id			If available, the first ID passed in the request that caused the error.
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		virtual OLStatus ErrorEvent(OLContext context, OLStatus error, OLID id) = 0;
		/// 
	};


	///
	/// \class OLMediaStreamEventHandler
	///
	/// \brief Handles events from OnLive Service Platform (OSP).
	///
	/// Event handler for the OLMediaStream API.  Used to receive asynchronous stream events.
	///
	class OLMediaStreamEventHandler
	{
		public:

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		/// 
		/// \brief Notifies the application that a stream is loaded.
		///
		///	This callback is called when a stream previously requested via LoadStream has successfully 
		///	completed loading, and the application has not explicitly called UnloadStream() on it. If a 
		///	stream was loaded in immediate mode, this method is not called.
		///
		///   \param	streamId		A previously returned stream ID from OLMedia's LoadStream method.
		///   \param	url				The URL provided to the loadStream method that returned this stream ID.
		///   \param	streamIsLive 	Informs the application if the stream is from a live session or prerecorded
		///   \param	duration_us 	Gives the duration of the stream in microseconds, if it is not live
		///   \param	width 			Gives the width of the video stream in pixels
		///   \param	height 			Gives the height of the video stream in pixels
		///   \param	sampleRate		Gives the audio sample rate, if the stream has audio, or 0 if not
		///   \param	channelCount	Gives the number of channels in the audio if the stream has audio, 0 if not
		///   \param	bitsPerSample	Gives the number of bits in each audio sample if the stream has audio, 0 if not
		///
		virtual void StreamLoaded(OLStreamId streamId, const char *url, bool streamIsLive, unsigned __int64 duration_us, 
								  unsigned int width, unsigned int height, unsigned int sampleRate, 
								  unsigned int channelCount, unsigned int bitsPerSample) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		/// 
		/// \brief A prior LoadStream call has failed to complete.
		///
		///   This callback is called when a stream previously requested via LoadStream() has failed to load,
		///   and the application has not explicitly called UnloadStream() on it. If a stream was loaded in immedate
		///   mode via LoadStreamImmediate(), this method is not called.
		///
		///   \param	streamId		A previously returned stream ID from the OLMedia's LoadStream method.
		///   \param	url				The URL provided to the loadStream method that returned this stream ID.
		///   \param	error			A string describing the error as accurately as possible.
		///
		virtual void StreamLoadFailed(OLStreamId streamId, const char *url, const char *error) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief A stream encountered a playback error.
		///
		///		This callback is called when a previously loaded stream has encountered an error that prevents
		///		it from playing further. Such errors are considered fatal and it is recommended that the stream 
		///		be unloaded at that point.
		/// 
		///		\param	streamId		A previously returned stream ID from OLMediaStream::LoadStream or 
		///								OLMediaStream::LoadStreamImmedate methods.
		///		\param	url				The URL provided to the loadStream or loadStreamImmedate methods that returned 
		///								this stream ID.
		///		\param	error			A string describing the error as accurately as possible.
		///
		virtual void StreamEncounteredError(OLStreamId streamId, const char *url, const char *error) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Notifies the application that a stream has completed unloading.
		///
		///   This callback is called when a previously loaded stream is finished unloading after a
		///   Media SDK unloadStream() request. When this callback is invoked, the resources associated
		///   with the given stream ID have been fully released.
		///
		///  \param	 streamId A previously returned stream ID from OLMediaStream::LoadStream or OLMediaStream::LoadStreamImmediate.
		///
		virtual void StreamUnloaded(OLStreamId streamId) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Notifies the application that the stream has reached its last frame.
		///
		///   This callback is called when a stream that has been played has delivered the last displayable 
		///   video frame in the stream. If the stream is set to looping via OLMedia's SetStreamLooping
		///   method, the stream will continue playback from the beginning. Otherwise, playback will pause.
		///   This callback is only invoked on the first loop of a looping stream.
		///
		///		\param	 streamId A previously returned stream ID from OLMediaStream::LoadStream or OLMediaStream::LoadStreamImmediate.
		///
		virtual void StreamLastVideoFrame(OLStreamId streamId) = 0;

		/// \addtogroup OLMEDIA_API_STILL_IMAGES
		///
		/// \brief A previously requested image has finished loading.
		///
		///   This callback is called when an image file, previously requested via the OLMediaStream::LoadImage
		///   method, is ready for use. 
		///
		///		*Note*: The bytes associated with imgfile *must* be copied into the application's local storage,
		///				as the memory will be freed or reused when the callback returns.
		/// 
		/// \param	url		The URL provided to OLMedia's LoadImage method.
		/// \param	data	The set of compressed bytes found in the image file. This data is freed when the callback
		///					returns. The application is responsible for copying the data.
		/// \param	byteCount	The number of bytes in the data.
		/// 
		virtual void ImageLoaded(const char *url, const unsigned char *data, unsigned int byteCount) = 0;

		/// \addtogroup OLMEDIA_API_STILL_IMAGES
		///
		/// \brief A previously requested image has failed to load.
		///
		///   This callback is called when an image previously requested via the OLMediaStream::LoadImage method
		///   has failed to load.
		/// 
		/// \param	url is the URL provided to OLMedia's LoadImage method.
		/// \param	error is a string describing the error as accurately as possible.
		/// 
		virtual void ImageLoadFailed(const char *url, const char *error) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Notifies the application that it must update the video of a stream.
		///
		///   This callback is called when a stream previously loaded by OLMediaStream::LoadStream or 
		///   OLMediaStream::LoadStreamImmediate, sized via the OLMediaStream::SetTextureSize() method, and played via the 
		///   OLMediaStream::PlayStream() method has a new video frame to display. The data is provided as YUV420
		///   data, with a line stride equal to the width of the texture specified by the application to 
		///   the OLMediaStream::SetTextureSize method. The YUV420 data size is equal to the texture width
		///   times the texture height times 3/2.
		///   This callback is called by the thread that invoked the OLMediaStream::HandleStreamEvent() method.
		/// 
		/// \param	streamId	A previously returned stream ID from OLMediaStream::LoadStream or OLMediaStream::LoadStreamImmediate
		/// \param	data 		The YUV data for this frame. It is reused or destroyed after this callback returns.
		/// \param	dataByteCount	Specifies the number of bytes in this update.
		/// \param	offset_us	Specifies the offset in microseconds from the beginning of the stream of this update.
		/// 
		virtual void VideoUpdate(OLStreamId streamId, unsigned char *data, unsigned int dataByteCount,
								 unsigned __int64 offset_us) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Notifies the application that audio is ready to be queued for playback.
		///
		///   This callback is invoked when a stream previously loaded by OLMedia's LoadStream or
		///   LoadStreamImmediate methods, sized via the SetTextureSize() method, and played via the PlayStream()
		///   method has new audio samples ready to be played back immediately. The data is provided as PCM
		///   samples, 16 bits wide little endian if the audio bits per sample specifies 16 bit audio, and
		///   interleaved if the audio channel count specifies stereo audio.
		///   This callback is called by the thread that invoked OLMedia's HandleStreamEvent() method.
		/// 
		///   \param	streamId 	A previously returned stream ID from OLMediaStream::LoadStream or OLMediaStream::LoadStreamImmediate
		///   \param	data		The PCM data for this frame. It is reused or destroyed after this callback returns.
		///   \param	dataByteCount	Specifies the number of bytes in this update.
		///   \param	offset_us	Specifies the offset in microseconds from the beginning of the stream of this update.
		/// 
		virtual void AudioUpdate(OLStreamId streamId, unsigned char *data, unsigned int dataByteCount,
								 unsigned __int64 offset_us) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Tells the application to flush any video display it might have.
		///
		///   This callback is invoked when the stream is about to start, or about to seek, and the 
		///   video's associated display should be cleared.
		/// 
		/// \param	streamId	A previously returned stream ID from OLMediaStream::LoadStream or OLMediaStream::LoadStreamImmediate
		/// 
		virtual void FlushVideo(OLStreamId streamId) = 0;

		/// \addtogroup OLMEDIA_API_VIDEO_STREAM_PLAYBACK
		///
		/// \brief Tells the application to flush any queued audio samples it might have.
		///
		///   This callback is invoked when the stream is about to start, or about to seek, and the 
		///   audio's associated playback buffers should be cleared.
		/// 
		/// \param	streamId	A previously returned stream ID from OLMediaStream::LoadStream or OLMediaStream::LoadStreamImmediate
		/// 
		virtual void FlushAudio(OLStreamId streamId) = 0;
	};


	///
	/// \class OLMediaCallbacks
	///
	/// \brief Callback interface to receive asynchronous notification that events are waiting.
	///
	/// Notifies the application that an event is waiting to be handled. Handled with OLMedia::HandleEvent().
	///
	class OLMediaCallbacks
	{
		public:

		/// \addtogroup OLMEDIA_API_EVENT_HANDLING
		///
		/// An event is waiting. Call OLMedia::HandleEvent() to receive the event
		///
		/// \note Callbacks must take less than 2 milliseconds.
		///
		virtual void EventWaiting() = 0;	
	};

	///
	/// \class OLMediaStreamCallbacks
	///
	/// \brief Callback interface to receive asynchronous notification that stream events are waiting.
	///
	/// Notifies the application that an event is waiting to be handled. Handled with OLMediaStream::HandleStreamEvent().
	///
	class OLMediaStreamCallbacks
	{
		public:

		/// \addtogroup OLMEDIA_API_EVENT_HANDLING
		///
		/// A stream event is waiting. Call OLMediaStream::HandleStreamEvent() to receive the event.
		/// 
		virtual void StreamEventWaiting() = 0;

		/// \addtogroup OLMEDIA_API_EVENT_HANDLING
		/// 
		/// A stream's video or audio data requires update. Call OLMediaStream::UpdateStream(id) to update the stream.
		/// \param id	The stream ID that needs update, previously returned from OLMediaStream::LoadStream or OLMediaStream::LoadStreamImmediate
		///
		virtual void StreamNeedsUpdate(OLStreamId id) = 0;
	};

	/// \addtogroup OLMEDIA_API_CREATE_AND_DESTROY
	/// 
	///
	/// \brief DLL Export
	///
	/// DLL Export: Start the media API and retrieve the media API interface class pointer.
	/// NOTE: The media service is a singleton, multiple calls to OLGetMedia will return the same pointer.
	///
	///		\param	version			API version, use MEDIA_API_VERSION
	///
	///		\param	service			Pointer to a OLMedia class pointer.
	///
	///		\return	OLStatus		OL_SUCCESS normal operation, \n
	///								OL_INVALID_PARAM invalid parameter, version or service or both are NULL, \n
	///								OL_INVALID_VERSION version requested not supported, this is fatal, \n
	///								OL_INTERNAL_ERROR could not communicate with OSP, this is fatal, \n
	///								OL_OUT_OF_MEMORY trying to start service, If memory can not be freed, 
	///									this error is fatal, the title should start the exit process.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLGetMedia(const char* version, OLMedia** service);

	///
	/// \brief DLL Export
	///
	/// DLL Export: Stop the media API.
	///
	///		\param	service			Pointer to a OLMedia class.
	///
	///		\return	OLStatus		OL_SUCCESS normal operation, \n
	///								OL_INVALID_PARAM invalid parameter, service is NULL.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLStopMedia(OLMedia* service);

	///
	/// \brief DLL Export
	///
	/// DLL Export: Start the media stream API and retrieve the media stream API interface class pointer.
	/// NOTE: The media stream API is *NOT* a singleton. You can instantiate multiple media stream services.
	///
	///		\param	version			API version, use MEDIA_API_VERSION
	///
	///		\param	service			Pointer to a OLMediaStream class pointer.
	///
	///		\return	OLStatus		OL_SUCCESS normal operation, \n
	///								OL_INVALID_PARAM invalid parameter, version or service or both are NULL, \n
	///								OL_INVALID_VERSION version requested not supported, this is fatal, \n
	///								OL_INTERNAL_ERROR could not communicate with OSP, this is fatal, \n
	///								OL_OUT_OF_MEMORY trying to start service, If memory can not be freed,
	///									this error is fatal, the title should start the exit process.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLGetMediaStream(const char* version, OLMediaStream** service);

	///
	/// \brief DLL Export
	///
	/// DLL Export: Stop the media stream API.
	///
	///		\param	service			Pointer to a OLMediaStream class.
	///
	///		\return	OLStatus		OL_SUCCESS normal operation, \n
	///								OL_INVALID_PARAM invalid parameter, service is NULL.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLStopMediaStream(OLMediaStream* service);
	/// 

}; // namespace olapi

#endif // OLMEDIA_H
