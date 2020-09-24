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
// olvoice.h $Revision: 54252 $
//

/// \file olvoice.h
/// \addtogroup VOICE_API
/// \brief The OLVoice interface provides communication between a running title and the OLService gateway.
///
/// The OLVoice interface provides communication between a running title and the OLService
/// gateway

#ifndef OLVOICE_H
#define OLVOICE_H

#include <ol_api/olapi.h>
//#include <ol_api/olservice.h>

namespace olapi
{
	/// \addtogroup VOICE_API
	/// 

	#define VOICE_API_VERSION (OLAPI_VERSION)

	///
	/// \brief Enumeration describing voice capabilities of a user's headset
	enum VoiceCapability
	{
		OL_VOICE_NO_VOICE = 0,           ///< user is not capable of voice
		OL_VOICE_LISTEN_ONLY = 1,        ///< user is not capable of transmitting voice but can receive
		OL_VOICE_LOW_QUALITY_VOICE = 1,  ///< user is capable of sending voice using low quality codecs
		OL_VOICE_HIGH_QUALITY_VOICE = 2, ///< user is capable of sending voice using high quality codecs
		OL_VOICE_CAPABILITY_MAX_TYPES
	};
	
	///
	/// \brief Key enumeration for accessing the VoiceStatus container
	///
	enum VoiceContainerKeys
	{
		OLKEY_RO_VoiceName,					///< string, Name of Container
		OLKEY_RO_VoiceContainerListId,      ///< string, ID of parent container list if any
		OLKEY_RO_VoiceChannelURI,			///< string, URI of created channel
		OLKEY_RO_VoiceChannelOwner,         ///< OLID, identity of channel owner
		OLKEY_RO_VoiceChannelDescription,   ///< string, human-readable description of channel
		OLKEY_RO_VoiceParticipantURI,       ///< string, SIP URI of a user
		OLKEY_RO_VoiceParticipantTag,       ///< string, tag of a user
		OLKEY_RO_VoiceParticipantUserID,    ///< OLID, identity of user
		OLKEY_RO_VoiceCapabilityHeadset1,   ///< enum VoiceCapability, capabilities of voice headset #1
		OLKEY_RO_VoiceCapabilityHeadset2,   ///< enum VoiceCapability, capabilities of voice headset #2
		OLKEY_RO_VoiceCapabilityHeadset3,   ///< enum VoiceCapability, capabilities of voice headset #3
		OLKEY_RO_VoiceCapabilityHeadset4,   ///< enum VoiceCapability, capabilities of voice headset #4
		OLKEY_RO_VoiceKeysMax
	};
	
	#define VOICECONTAINERNAME	"VoiceContainer"	///< Game Catalog Item Container Name 
	
	///
	/// \brief Key strings in the VoiceStatus container
	///
	#ifndef NO_OLAPI_DLL_LINKAGE
	extern __declspec(dllimport) char *VoiceContainerKeyNames[olapi::OLKEY_RO_VoiceKeysMax];
	#endif

	enum OLVoiceParticipantEventType
	{
		OL_VOICE_PARTICIPANT_ENTERED = 1,
		OL_VOICE_PARTICIPANT_EXITED,
		OL_VOICE_PARTICIPANT_SPEAKING,
		OL_VOICE_PARTICIPANT_IDLE,
		OL_VOICE_PARTICIPANT_MUTE,
		OL_VOICE_PARTICIPANT_DISABLED,
		OL_VOICE_PARTICIPANT_ENABLED,
		OL_VOICE_PARTICIPANT_LISTEN_ONLY,
		OL_VOICE_PARTICIPANT_MIC_ACTIVE,
	};

	class OLVoiceEventHandler;
	class OLVoiceCallbacks;

	///
	/// \class OLVoice
	/// \brief Voice Interface Class
	///
	/// Voice Interface Class
	///
	class OLVoice : public OLEventBase
	{
		public:

		/// \brief Allows client to be notified that an event is available.
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
		virtual OLStatus SetEventWaitingCallback(OLVoiceCallbacks* callback) = 0;

		///
		/// \brief Sets handler to call when an application event is processed.
		///
		/// Handler used by application to process events.
		///
		///		\param	handler			Pointer to OLVoiceEventHandler interface
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM invalid or NULL parameter handler.
		///
		virtual OLStatus SetEventHandler(OLVoiceEventHandler* handler) = 0;

		///
		/// \brief Dispatches the next voice api event.
		///
		/// Dispatches the next voice api event, if available, to the handler indicated in SetEventHandler.  Implements an
		/// OLVoiceEventHandler() interface in the application to process events from each of the APIs.
		///
		///		\param	handler			Pointer to OLUserSessionEventHandler interface
		///
		///		\param	time			Time to wait in milliseconds for next event, OL_INFINITE will wait until next event
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INVALID_PARAM invalid or NULL parameter handler and handler not already set, \n
		///								OL_EVENT_TIMEOUT if no events available.
		///
		virtual OLStatus HandleEvent(OLVoiceEventHandler* handler, unsigned int time) = 0;

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
		/// \brief Get voice capabilities of the client
		///
		/// Retrieves a container ID for the current voice capabilities of the client. 
		/// This will trigger a ContainerID event. This call must be preceded by a call to InitChat.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus GetVoiceCapabilities(OLContext context) = 0;

		///
		/// \brief Initialize the chat subsystem
		///
		/// This initializes the chat subsystem for the user application session, creating any necessary connections to
		/// the voice servers and retrieving information from the connected client about its chat capabilities. This will
		/// trigger a StatusResult event.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus InitChat(OLContext context) = 0;

		///
		/// \brief Stop the chat subsystem
		///
		/// This shuts down the chat subsystem for the user application session, removing any necessary connections to
		/// the voice servers. This will trigger a StatusResult event.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus StopChat(OLContext context) = 0;

		///
		/// \brief Join a chat channel
		///
		/// This joins a chat channel. Channels are identified by SIP URIs, which are assigned upon channel creation.
		/// It is the application's responsibility to exchange channel URIs among multiple instances of the application,
		/// as would happen in a multiplayer chat scenario. On success, this will generate a ChannelJoined event followed by a
		/// stream of ParticipantEvent calls listing each user presently in the channel. Further ParticipantEvent events will
		/// generated as users interact. On failure, this will generate a StatusResult event.
		///
		/// The user may have voice chat disabled via the Dashboard API. In that situation, the Join Channel and Leave Channel
		/// API calls are available and will succeed, but the user's participant events will say that the user is disabled until
		/// the user enables themselves. The workflow is that the application joins and leaves channels even when the user is
		/// disabled in order to communicate to the user when these channels are available should they wish to join.
		///
		/// The special channel URI "#echo" can be used for debugging purposes. It will repeat back what you say to it.
		///
		///		\param	context				Used to pair up asynchronous requests and messages.
		///		\param	channel_uri			The SIP URI of the channel to join.
		///		\param	channel_password	An optional password to enter the channel.
		///		\param	join_muted			If true, the user joins the channel in a muted state
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus JoinChannel(OLContext context, const char *channel_uri, const char *channel_password, bool join_muted) = 0;

		///
		/// \brief Leave a chat channel
		///
		/// This leaves a previously joined channel. This will generate a StatusResult event.
		///
		/// The user may have voice chat disabled via the Dashboard API. In that situation, the Join Channel and Leave Channel
		/// API calls are available and will succeed, but the user's participant events will say that the user is disabled until
		/// the user enables themselves.
		///
		///		\param	context				Used to pair up asynchronous requests and messages.
		///		\param	channel_uri			The SIP URI of the channel to join.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus LeaveChannel(OLContext context, const char *channel_uri) = 0;

		///
		/// \brief Create a voice channel
		///
		/// This creates a new voice channel for others to join. The creator is not automatically joined to 
		/// the channel, and must use JoinChannel to enter it as any other user must. On failure, this will
		/// generate a StatusResult event, and on success it will generate a ContainerID event from which the
		/// OLKEY_RO_VoiceChannelURI member can be retrieved.
		///
		/// If the channel is not explicitly destroyed by the creator, it will be destroyed upon the termination
		/// of the application session of the creator. This will happen even if users are actively using the channel.
		///
		/// The name of the channel must only be unique to the creating user. That is, Alice can create only one 
		/// channel named "Stuff", but Bob can create a channel named "Stuff" even if Alice already has done so.
		/// Each channel will be assigned a unique SIP URI.
		///
		/// Users for whom voice chat is disabled are unable to create channels.
		///
		///		\param	context				Used to pair up asynchronous requests and messages.
		///		\param	channel_name		A human readable name for the channel.
		///		\param	channel_password	An optional password for the channel if the channel is to be password protected.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus CreateChannel(OLContext context, const char *channel_name, const char *channel_password) = 0;

		///
		/// \brief Destroy a voice channel
		///
		/// This destroys a voice channel previously created by the caller. This generates a StatusResult event.
		///
		/// If the channel is not explicitly destroyed by the creator, it will be destroyed upon the termination
		/// of the application session of the creator. This will happen even if users are actively using the channel.
		/// Channels may be destroyed even if they are in active use.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///		\param	channel_uri		The URI of the channel, returned in a prior ChannelID event.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus DestroyChannel(OLContext context, const char *channel_uri) = 0;

		///
		/// \brief Start Voice Audio Monitoring
		///
		/// This begins feeding the application information about the voice detected from the microphone. The voice 
		/// subsystem will indicate if the user is speaking, the sonic energy of the audio, and the smoothed energy 
		/// of the audio. The events of type VoiceMonitorUpdate will continue until StopVoiceMonitor is called. This
		/// request generates a StatusResult event.
		///
		/// If the joinEchoChannel parameter is true, the user will be joined to a special channel that takes their
		/// microphone output and plays it back through their speakers after a brief delay.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///		\param	joinEchoChannel	Indicates that the user should be joined to the echo channel when the monitor is enabled.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus StartVoiceMonitor(OLContext context, bool joinEchoChannel) = 0;

		///
		/// \brief Stop Voice Audio Monitoring
		///
		/// Previously enabled Voice Monitor events are cancelled, and the user is removed from the echo
		/// channel. If the user was previously in a channel, they will rejoin that channel.
		/// This request generates a StatusResult event.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus StopVoiceMonitor(OLContext context) = 0;

		///
		/// \brief Set Participant Playback Level (0-100)
		///
		/// The playback level of the participant is adjusted to the given level, with 0 being silence and 100
		/// being maximum volume. This call affects only the playback in the speakers of the caller, not the playback
		/// level of all listeners in the channel. This request generates a StatusResult event.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///		\param	participantId	The User ID of the participant to manipulate.
		///		\param	level			The volume level from 0-100 of the playback.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus SetParticipantPlaybackLevel(OLContext context, const char *channel_uri, olapi::OLID participantId, int level) = 0;

		///
		/// \brief Set Participant Playback Mute Status
		///
		/// The given participant is either muted (set to playback volume level 0), or returned to their prior playback
		/// level. This call affects only the playback in the speakers of the caller, not the playback
		/// level of all listeners in the channel. This request generates a StatusResult event.
		///
		/// If this call is made with participantId being the current user, the mute will be applied to the user's
		/// microphone and no participant will hear from the current user again until they unmute themselves.
		///
		///		\param	context			Used to pair up asynchronous requests and messages.
		///		\param	participantId	The User ID of the participant to manipulate.
		///		\param	mute			Whether or not to mute the participant.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus SetParticipantPlaybackMuteStatus(OLContext context, const char *channel_uri, olapi::OLID participantId, bool mute) = 0;

		///
		/// \brief Set Overlay's Speaking Indicator Status
		///
		/// Turns on and off the overlay's default speaking indicator.  The speaking indicator will only
		/// turn on if voice is enabled in the Title.
		///
		///		\param	visible			Whether or not to show the speaking indicator.
		///
		///		\return	OLStatus		OL_SUCCESS normal operation,\n
		///								OL_INTERNAL_IO_ERROR communication with OSP could not complete,
		///									this is a fatal error.
		///
		virtual OLStatus SetSpeakingIndicatorStatus(bool visible) = 0;
	};


	///
	/// \class OLVoiceEventHandler
	///
	/// \brief Handles events from OnLive Service Platform (OSP).
	///
	/// Event handler for the OLVoice API.  Used to receive asynchronous events.	///
	///
	class OLVoiceEventHandler
	{
		public:

		///
		/// \brief Provides the ID for a container.
		///
		/// Provides the ID for the data container requested by an olapi::OLUserSession function.
		/// Use the container ID with GetVoiceContainer.
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
		/// \brief Provides a Result Code for an operation
		///
		/// Provides the status result code for a given request.
		///
		///		\param	context			Pairs asynchronous operations.
		///		\param	status			The status result code of the operation.
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		virtual OLStatus StatusResult(OLContext context, OLStatus status) = 0;

		///
		/// \brief A voice chat channel was joined.
		///
		/// The user has been joined to a voice chat channel. The owner of the channel
		/// as well as the URI and name of the channel are provided.
		///
		/// The user may have voice chat disabled via the Dashboard API. In that situation, the Join Channel and Leave Channel
		/// API calls are available and will succeed, but the user's participant events will say that the user is disabled until
		/// the user enables themselves.
		///
		///		\param	context			Pairs asynchronous operations.
		///		\param	channelOwner	The creator of the channel, also its owner.
		///		\param	channelURI		The URI of the channel
		///		\param	channelName		The human readable name of the channel.
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		virtual OLStatus ChannelJoinedEvent(OLContext context, OLID channelOwner, const char *channelURI, 
			const char *channelName) = 0;

		///
		/// \brief A voice chat channel was exited.
		///
		/// The user has exited a voice chat channel. The owner of the channel
		/// as well as the URI and name of the channel are provided.
		///
		/// The user may have voice chat disabled via the Dashboard API. In that situation, the Join Channel and Leave Channel
		/// API calls are available and will succeed, but the user's participant events will say that the user is disabled until
		/// the user enables themselves.
		///
		///		\param	context			Pairs asynchronous operations.
		///		\param	channelOwner	The creator of the channel, also its owner.
		///		\param	channelURI		The URI of the channel
		///		\param	channelName		The human readable name of the channel.
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		virtual OLStatus ChannelExitedEvent(OLContext context, OLID channelOwner, const char *channelURI, 
			const char *channelName) = 0;

		///
		/// \brief Updates the status of a channel participant
		///
		/// After joining a channel, events are generated describing the state of its participants, whether
		/// speaking, not speaking, muted, or unmuted. Upon joining a channel, the applicatin will receive
		/// ParticipantEvent callbacks for each user in the channel. Their state should be set to IDLE unless
		/// a followup event describes them as SPEAKING or MUTE.
		///
		///		\param	context			Pairs asynchronous operations.
		///		\param	participantID	The User ID of the participant being updated
		///		\param	particpantTag	The Tag of the participant being updated
		///		\param	type			The new state of the participant
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		virtual OLStatus ParticipantEvent(OLContext context, OLID participantID, const char *participantTag,
			OLVoiceParticipantEventType type) = 0;

		///
		/// \brief Voice Monitor Data Update
		///
		/// On requesting voice data monitor information, provides information about the analyzed voice signal.
		///
		///		\param	context			Pairs asynchronous operations.
		///		\param	voiceDetected	Describes whether or not voice was detected in the audio frame.
		///		\param	energy			Describes the audio energy in the audio frame.
		///		\param	smoothedEnergy	Describes the smoothed energy contained in the audio frame.
		///
		///		\return	OLStatus		OL_SUCCESS event handled, \n
		///								OL_INVALID_PARAM event not handled due to invalid parameter, \n
		///								OL_NOT_IMPLEMENTED event not handled, \n
		///								OL_FAIL event not handled, event unexpected, invalid context, or other.
		///
		virtual OLStatus VoiceMonitorUpdate(OLContext context, bool voiceDetected, double energy, int smoothedEnergy) = 0;
	};

	///
	/// \class OLVoiceCallbacks
	///
	/// \brief Callback interface to receive asynchronous notification that events are waiting.
	///
	/// Notifies the application that an event is waiting to be handled. Handled with OLVoice::HandleEvent().
	///
	class OLVoiceCallbacks
	{
		public:

		///
		/// An event is waiting. Call OLVoice::HandleEvent() to receive the event
		///
		/// \note Callbacks must take less than 2 milliseconds.
		///
		virtual void EventWaiting() = 0;	
	};

	///
	/// \brief DLL Export
	///
	/// DLL Export: Start the user session API and retrieve the user API interface class pointer.
	///
	///		\param	version			API version, use VOICE_API_VERSION
	///
	///		\param	service			Pointer to a OLVoice class pointer.
	///
	///		\return	OLStatus		OL_SUCCESS normal operation, \n
	///								OL_INVALID_PARAM invalid parameter, version or service or both are NULL, \n
	///								OL_INVALID_VERSION version requested not supported, this is fatal, \n
	///								OL_INTERNAL_ERROR could not communicate with OSP, this is fatal, \n
	///								OL_OUT_OF_MEMORY trying to start service, If memory can not be freed, 
	///									this error is fatal, the app should start the exit process.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLGetVoice(const char* version, OLVoice** service);

	///
	/// \brief DLL Export
	///
	/// DLL Export: Stop the user session API.
	///
	///		\param	service			Pointer to a OLVoice class.
	///
	///		\return	OLStatus		OL_SUCCESS normal operation, \n
	///								OL_INVALID_PARAM invalid parameter, service is NULL.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLStopVoice(OLVoice* service);

	/// 

}; // namespace olapi

#endif // OLVOICE_H
