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
// OLOverlay.h $Revision: 60760 $
//

/// \file OLOverlay.h

#ifndef OLOVERLAY_H
#define OLOVERLAY_H

#include <ol_api/OLAPI.h>

namespace olapi
{
	/*! \ingroup OLOVERLAY_API_CREATE_AND_DESTROY 
		\brief The default version of the OnLive OLOverlay API to use in \ref olapi::OLGetOverlay.
	*/
	#define OVERLAY_API_VERSION (OLAPI_VERSION)	
	/// \ingroup OLOVERLAY_API_MESSAGEBOX
	/// \brief Maximum string length of a Message Box's title (does not include the nul char).
	#define OL_MAX_MESSAGE_BOX_TITLE_SIZE				128	
	/// \ingroup OLOVERLAY_API_MESSAGEBOX
	/// \brief Maximum string length of a Message Box's body text (does not include the nul char).
	#define OL_MAX_MESSAGE_BOX_BODY_SIZE				256	
	/// \ingroup OLOVERLAY_API_MESSAGEBOX
	/// \brief Maximum string length of a Message Box's button specifier text (does not include the nul char).	
	#define OL_MAX_MESSAGE_BOX_BUTTON_SIZE				128		
	/// \ingroup OLOVERLAY_API_ALERTBOX
	/// \brief Maximum string length of an Alert Box's title (does not include the nul char).
	#define OL_MAX_ALERT_TITLE_SIZE						32		
	/// \ingroup OLOVERLAY_API_ALERTBOX
	///\brief Maximum string length of an Alert Box's subtitle (does not include the nul char).
	#define OL_MAX_ALERT_SUBTITLE_SIZE					64	
	/// \ingroup OLOVERLAY_API_ALERTBOX
	///\brief Maximum string length of an Alert Box's body text (does not include the nul char).
	#define OL_MAX_ALERT_BODY_SIZE						256			
	/// \ingroup OLOVERLAY_API_ALERTBOX
	///\brief Minimum duration the alert can be displayed on the screen (in seconds).
	#define	OL_MIN_ALERT_DURATION						3	
	/// \ingroup OLOVERLAY_API_ALERTBOX
	///\brief Maximum duration the alert can be displayed on the screen (in seconds).
	#define OL_MAX_ALERT_DURATION						10			 
	/// \ingroup OLOVERLAY_API_SPINNER_DIALOG
	/// \brief Maximum string length of a spinner dialog's message (does not include the nul char).
	#define OL_MAX_SPINNER_MESSAGE_SIZE					32
	/// \ingroup OLOVERLAY_API_VIRTUAL_KEYBOARD
	/// \brief The max string length the virtual keyboard can return (does not include the nul char).
	#define OL_MAX_VIRTUAL_KEYBOARD_INPUT_LENGTH		256	
	/// \ingroup OLOVERLAY_API_VIRTUAL_KEYBOARD
	/// \brief Maximum string length of a keyboard's title (does not include the nul char).
	#define OL_MAX_VIRTUAL_KEYBOARD_TITLE_SIZE			18			
	/// \ingroup OLOVERLAY_API_VIRTUAL_KEYBOARD
	/// \brief Maximum string length of a keyboard's description (does not include the nul char).
	#define OL_MAX_VIRTUAL_KEYBOARD_DESCRIPTION_SIZE	52			
	/// \ingroup OLOVERLAY_API_VIRTUAL_KEYBOARD
	/// \brief Maximum string length of a keyboard's allowed symbol (does not include the nul char).
	#define OL_MAX_VIRTUAL_KEYBOARD_SYMBOL_SIZE			64			 
	/// \ingroup OLOVERLAY_API_VIRTUAL_KEYBOARD
	/// \brief Maximum string length of a keyboard's allowed format (does not include the nul char).
	#define OL_MAX_VIRTUAL_KEYBOARD_FORMAT_SIZE			32			



	///
	/// \brief This enum is used as a bitset defining the behavior of the Overlay Virtual Keyboard.
	///			See \ref olapi::OLOverlay::GetInputFromKeyboard.
	///
	enum OLVirtualKeyboardControl
	{
		OL_VIRTUAL_KEYBOARD_DEFAULT					= 0x0,		///< No Flags, default String input. This flag should not be used with other keyboard input control flags.
		OL_VIRTUAL_KEYBOARD_FLOAT					= 0x1,		///< Input is a float; keyboard only enables digit and '.' keys (and validates that at most one '.' occurs). This flag overrides the validCharacterString.
		OL_VIRTUAL_KEYBOARD_INT						= 0x2,		///< Input is an integer; keyboard only enables digit keys. This flag overrides the validCharacterString.
		OL_VIRTUAL_KEYBOARD_NO_SYMBOLS				= 0x4,		///< Disable the keyboard's symbol keys (non alphanumeric). Only takes effect if validCharacterString is empty.
		OL_VIRTUAL_KEYBOARD_NO_LOWER				= 0x8,		///< Disable the keyboard's lowercase letter keys.  Only takes effect if validCharacterString is empty or using '~' in the formatString.
		OL_VIRTUAL_KEYBOARD_NO_NUMBERS				= 0x10,		///< Disable the keyboard's number keys. This flag cannot be used with OL_VIRTUAL_KEYBOARD_FLOAT or OL_VIRTUAL_KEYBOARD_INTEGER. Only takes effect if validCharacterString is empty.
		OL_VIRTUAL_KEYBOARD_NO_UPPER				= 0x20,		///< Disable the keyboard's uppercase letter keys. Only takes effect if validCharacterString is empty or using '~' in the formatString.
		OL_VIRTUAL_KEYBOARD_MULTI_LINE				= 0x40,		///< Text input becomes three eighteen column rows. Allow newline character in the return string. Only takes effect if input is not formatted.
		OL_VIRTUAL_KEYBOARD_IS_PRIVATE				= 0x80,		///< Do not echo text input back to the screen; only display asterisks (like a password entry dialog).
		OL_VIRTUAL_KEYBOARD_RETURN_WITHOUT_FORMAT	= 0x100,	///< If the input is formatted, the return string will not contain any format.
		OL_VIRTUAL_KEYBOARD_NO_CANCEL				= 0x200,	///< No Cancel button or Escape key. User cannot close the keyboard until a valid input is entered.
	};

	/// 

	///
	/// \ingroup OLOVERLAY_API_STATUS_EVENT
	/// \brief This enum is used as a bitset representing the types of Overlay UI elements
	///			that are currently visible.  See \ref olapi::OLOverlayEventHandler::Status.
	///
	enum OLOverlayStatus
	{
		OL_OVERLAY_DOWN						= 0,	///< No overlay elements are visible.
		OL_OVERLAY_MENU						= 1,	///< An overlay menu is visible (see \ref olapi::OLOverlay::DisplayMenu).
		OL_OVERLAY_ALERT					= 2,	///< An alert notification is visible (see \ref olapi::OLOverlay::DisplayAlert).
		OL_OVERLAY_KEYBOARD					= 4,	///< The virtual keyboard is visible (see \ref olapi::OLOverlay::GetInputFromKeyboard).
		OL_OVERLAY_MESSAGE_BOX				= 8,	///< Message Box or GameInfo dialog on screen (see \ref olapi::OLOverlay::DisplayMessageBox or \ref GAME_INFO_DIALOG_PAGE).
		OL_OVERLAY_BUSY						= 16	///< The Spinner Dialog is on screen (see \ref olapi::OLOverlay::ShowSpinnerDialog).
	} ;

	///
	/// \ingroup OLOVERLAY_API_MENU_CONTROL
	/// \brief Used in \ref olapi::OLOverlay::DisplayMenu to show a specific Overlay Menu Screen
	///			or close the overlay.
	/// 
	enum OLOverlayMenus
	{
		OL_OVERLAYMENU_NONE,							///< Close the overlay menu.
		OL_OVERLAYMENU_GAMES,							///< Main games menu, user can switch to another game here.
		OL_OVERLAYMENU_SETTINGS,						///< User settings menu.
		OL_OVERLAYMENU_INBOX,							///< Friends Inbox.
		OL_OVERLAYMENU_SERVICE,							///< Service menu.
		OL_OVERLAYMENU_ABOUT,							///< About Box.
		OL_OVERLAYMENU_VOICE,							///< Voice Chat menu, user can manage channels and view the roster here.
		OL_OVERLAYMENU_ACHIEVEMENT,						///< Achievement menu, user can look up his/her achievements here.
		OL_OVERLAYMENU_MAX,								///< Max enum value.
		OL_OVERLAYMENU_DEFAULT = OL_OVERLAYMENU_GAMES	///< The default menu brought up when a user opens the overlay.
	};

	///
	/// \ingroup OLOVERLAY_API_ALERTBOX
	/// \brief The 'type' of an icon; one of five OnLive system icons, no icon, or a custom image.
	///			See \ref olapi::OLOverlay::DisplayAlert and \ref olapi::OLOverlay::DisplayMessageBox.
	///
	/// \par OL_OVERLAYICON_CHEERS icon
	///		\image html overlay_ui_icon_cheers.png
	///
	/// \par OL_OVERLAYICON_JEERS icon
	///		\image html overlay_ui_icon_jeers.png
	///
	/// \par OL_OVERLAYICON_FRIEND icon
	///		\image html overlay_ui_icon_friend.png
	///
	/// \par OL_OVERLAYICON_ADD_FRIEND icon
	///		\image html overlay_ui_icon_add_friend.png
	///
	/// \par OL_OVERLAYICON_ACHIEVEMENT icon
	///		\image html overlay_ui_icon_achievement.png
	///
	enum OLOverlayIconTypes
	{
		OL_OVERLAYICON_NONE,						///< No icon.
		OL_OVERLAYICON_CHEERS,						///< Thumbs Up icon.
		OL_OVERLAYICON_JEERS,						///< Thumbs Down icon.
		OL_OVERLAYICON_FRIEND,						///< Friend icon.
		OL_OVERLAYICON_ADD_FRIEND,					///< Add Friend icon.
		OL_OVERLAYICON_ACHIEVEMENT,					///< Trophy icon.
		OL_OVERLAYICON_CUSTOM = 1000,				///< Custom icon.
		OL_OVERLAYICON_MEDIASERVER_URI = 2000,		///< Load an icon from OnLive mediaServer
	};

	class OLOverlayEventHandler;
	class OLOverlayEventWaitingCallback;

	///
	/// \ingroup OLOVERLAY_API 
	/// \class OLOverlay
	/// \brief The OnLive Overlay is a graphical interface produced by the OSP and superimposed over your title.  
	///			The OLOverlay API provides virtual keyboard support, modal dialog boxes, transient alert notifications, 
	///			and programmatic ways to control the overlay menu system (as if the user pressed Alt+O or the 
	///			home button). The Overlay API notifies titles of events that require a display in the OnLive Overlay, 
	///			conveys input from the Overlay to titles, and allows titles to use some of the Overlay features. 
	///			Note that "Overlay" is the term used in the API and "Dashboard" is the term used for the UI.
	///
	/// \par API Initialization
	///		The OLOverlay singleton is created and retrieved with \ref olapi::OLGetOverlay() and destroyed 
	///		with \ref olapi::OLStopOverlay().\n\n
	///		NOTE: OLOverlay depends on the OLService API and should be initialized after it. \n
	///		See \ref OLOVERLAY_API_CREATE_AND_DESTROY and \ref SDK_SESSION_FLOW_PAGE for general info about 
	///		initializing the OnLive SDK APIs.
	///
	/// \par API Event and Callback Handling
	///		See \ref SDK_EVENTS_PAGE for an overview or \ref OLOVERLAY_API_EVENT_HANDLING for specifics.
	///
	/// \par Overlay Status Flag Events
	///		The overlay sends \ref OLOverlayEventHandler::Status events as it changes state (and as the various overlay UI
	///		elements are shown/hidden).  See \ref OLOVERLAY_API_STATUS_EVENT.
	///
	/// \par Controlling the Overlay Menu System
	///		The overlay menu screens are what the user brings up by pressing the home button on their controller or
	///		Alt+O on their keyboard.  You can show the overlay menu system programmatically with \ref OLOverlay::DisplayMenu
	///		or disable the overlay menu system entirely with \ref OLOverlay::DisableMenu.
	///
	/// \par Overlay UI Elements
	///		The overlay also provides 4 UI elements that your title can use:
	///		<dl>
	///			<dt> \ref OLOVERLAY_API_VIRTUAL_KEYBOARD
	///				<dd> A modal dialog prompting the user to enter text using a virtual keyboard UI.
	///					 See \ref OLOverlay::GetInputFromKeyboard.
	///			<dt> \ref OLOVERLAY_API_MESSAGEBOX
	///				<dd> A messageBox is a modal dialog with an icon, title & body text, and prompts the user
	///					 with an arbitrary set of buttons.  See \ref OLOverlay::DisplayMessageBox.
	///			<dt> \ref OLOVERLAY_API_SPINNER_DIALOG
	///				<dd> A modal dialog indicating that the title is busy (displays a spinning icon and a short message).
	///					 Users cannot interact with the spinner dialog; it must be shown/hidden programmatically.
	///					 See \ref OLOverlay::ShowSpinnerDialog.
	///			<dt> \ref OLOVERLAY_API_ALERTBOX
	///				<dd> An alertBox is a non-modal notification dialog showing a title, subTitle, message body, and
	///					a pair of icons (these fields are optional).  User cannot interact with alert boxes, and
	///					alert boxes are automatically hidden after being displayed for a specified amount of time.
	///					See \ref OLOverlay::DisplayAlert.
	///		</dl>
	///
	/// \par Overlay Input Focus and Pausing Gameplay
	///		With the exception of alert boxes, visible overlay UI elements consume all input from the user until the
	///		element is dismissed.  Because of this, your title needs to pause gameplay while the overlay is up.\n\n
	///
	///		For user-initiated overlay interactions (such as hiding/showing the overlay with the controller or keyboard),
	///		the OSP sends \ref olapi::OLUserSessionEventHandler::Pause and \ref olapi::OLUserSessionEventHandler::Resume events to your title.\n\n
	///		
	///		If you are programmatically displaying a modal overlay dialog or menu, however, you are responsible
	///		for maintaining the correct gameplay state, and the OSP will not send the Pause/Resume events.
	///
	class OLOverlay : public OLEventBase
	{
		public:

		/// \addtogroup OLOVERLAY_API_EVENT_HANDLING
		/// 
		/// \name OLUserSession Callbacks & Event Handling
		/// 

		/// 
		/// \brief Registers your callback handler object with the OLOverlay API.
		///
		/// If you intend to handle callbacks for OLOverlay, you should set
		/// a callback handler object before you start the OnLive Services with \ref olapi::OLStartServices.
		/// Once services are started, callbacks can be dispatched by the SDK at any time.
		///
		/// OLOverlay stores a single callback object pointer; calling this method
		/// multiple times will replace the previous pointer.  There is no way to 'unregister'
		/// the pointer (you cannot set it to NULL).
		///
		///	See \ref EVENTS_PAGE_REGISTRATION for details.
		///
		///		\param[in]	callbackHandler	Your implementation of the \ref OLOverlayEventWaitingCallback interface.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			Normal operation; callbacks can be dispatched on the supplied obj while
		///										the services are running.
		///		\retval OL_INVALID_PARAM	Error - the supplied callback obj pointer was NULL (or invalid);
		///										the callback obj has not been set.
		///
		virtual OLStatus SetEventWaitingCallback(OLOverlayEventWaitingCallback* callbackHandler) = 0;



		///
		/// \brief Registers your event handler object with the OLOverlay API.
		///
		/// Once you have set the OLOverlay API's event handler, calling one of the OLOverlayEventWaitingCallback methods
		///		may dispatch events on your object (assuming events are available to process).
		///		See \ref EVENTS_PAGE_HANDLING_OVERVIEW.
		///
		/// OLOverlay stores a single event handler pointer; calling this method
		/// multiple times will replace the previous pointer.  There is no way to 'unregister'
		/// the pointer (you cannot set it to NULL).  
		///
		///	See \ref EVENTS_PAGE_REGISTRATION for details.
		///
		///		\param	eventHandler	Your implementation of the OLOverlayEventHandler() interface.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			Normal operation; events will be dispatched on the supplied obj.
		///		\retval OL_INVALID_PARAM	Error - the supplied event handler pointer was NULL (or invalid);
		///										the event handler has not been set.
		///
		virtual OLStatus SetEventHandler(OLOverlayEventHandler* eventHandler) = 0;



		///
		/// \brief The API sets the passed in event handler and attempts to handle a single event. NOTE: Prefer to use the simpler
		///  overload \ref HandleEvent(unsigned int).
		///
		/// This method acts as a combination of \ref SetEventHandler() and \ref HandleEvent(unsigned int).
		/// If the supplied event handler is not NULL, we store the event handler to the supplied value, then try to handle a single event.
		/// If the supplied event handler is NULL, we try to handle a single event using the previously stored handler.
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
		virtual OLStatus HandleEvent(OLOverlayEventHandler* eventHandler, unsigned int waitTimeMs) = 0;



		///
		/// \brief Attempts to dispatch a single event on this API's set event handler object.
		///
		/// You must register an OLOverlayEventHandler instance with \ref OLOverlay::SetEventHandler() before
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
		///		\retval OL_INVALID_PARAM	The API does not have an event handler set (see \ref OLOverlay::SetEventHandler).
		///		\retval OL_EVENT_TIMEOUT	No events were available for the duration of the wait.
		///
		virtual OLStatus HandleEvent(unsigned int waitTimeMs) = 0;


		/// 

		/// 

		///
		/// \ingroup OLOVERLAY_API_VIRTUAL_KEYBOARD
		/// \brief Displays OnLive's virtual keyboard and prompts the user to enter a string.  Acts as a modal dialog.
		///
		///	Note: GetInputFromKeyboard will fail if another modal dialog is already visible (ie: another keyboard,
		///		messageBox, spinner dialog, or the GameInfo dialog).  
		///
		/// In this example, the keyboard's validCharacterString is set to "OnLive!", so all other characters
		///	are disabled:
		/// \image html overlay_ui_keyboard.png
		///
		/// \responseEventList
		///		The SDK responds with one of the following events if this request is successfully sent: 
		///
		///		\ospEvent OLOverlayEventHandler::DataList
		///			\eventDesc	The OSP sends a DataList event containing the string the user entered.
		///						The DataList has a single DataItem in it; the DataItem contains the 
		///						string's buffer size and the string itself.  For example:
		///
		///					\code
		///						// extracting the user's input from the DataList event
		///						// MyHandler::DataList(olapi::OLContext context, olapi::OLDataList *dataList)
		///						const char *keyboardInput = NULL;
		///						// the dataList should have a single element
		///						olapi::OLDataItem *dataItem = (olapi::OLDataItem *)dataList->list;
		///						if (dataItem != NULL)
		///						{
		///							// Note: The dataList is destroyed when the event handler returns, so we copy it.
		///							keyboardInput = strdup(dataItem->data);
		///							// alternatively, you can alloc a buffer of dataItem->size chars and copy into it.
		///						}
		///					\endcode
		///
		///		\ospEvent OLOverlayEventHandler::StatusResult
		///			\eventDesc	Indicates that the keyboard could not be displayed or that the keyboard dialog
		///						was canceled by the user.
		///
		///				\statusResult OL_CANCEL_BUTTON			The user canceled the keyboard input by pressing
		///															escape (or 'b' on their controller).
		///				\statusResult OL_OVERLAY_IN_USE			Another modal dialog is already being displayed by
		///															the overlay, so the keyboard cannot be shown.
		///				\statusResult OL_INTERNAL_ERROR			A generic OSP/Overlay error.
		///
		/// \responseEventListEnd
		///
		///
		///		\param[in]	context			Used to pair this request with the response event (see \ref olapi::OLGetNextContext).
		///
		///		\param[in]	title			Title text, up to \ref OL_MAX_VIRTUAL_KEYBOARD_TITLE_SIZE chars in length.
		///									OSP will use "Enter Text:" if title is "".
		///
		///		\param[in]	description		Description text, up to \ref OL_MAX_VIRTUAL_KEYBOARD_DESCRIPTION_SIZE chars in length.
		///									You may use "" for no description.
		///
		///		\param[in]	defaultString	Text to be shown as a default string in the text area.
		///									You may use "" for no defaultString. 
		///									This arg cannot be used with input formatting, \ref OL_VIRTUAL_KEYBOARD_FLOAT or \ref OL_VIRTUAL_KEYBOARD_INT.
		///									If defaultString's length is greater than maxInputLength, defaultString will be truncated.
		///
		///		\param[in]	controlFlags	Used to control the keyboard's behavior; for example, forcing text or numeric input.
		///									This is a bitset, or individual flags together and cast back to the enum type.
		///									See \ref OLVirtualKeyboardControl for details.
		///
		///		\param[in]	maxInputLength	The max length of the input string allowed in the UI; this is a string length, not
		///										a buffer size.  Must be > 0 and <= \ref OL_MAX_VIRTUAL_KEYBOARD_INPUT_LENGTH chars.
		///									This arg only takes effect if formatString is empty.
		///
		///		\param[in]	minInputLength	The min length of the input string allowed in the UI; this is a string length, not
		///										a buffer size. Must be >= 0 and <= maxInputLength.
		///									This arg only takes effect if formatString is empty.
		///
		///		\param[in]	validCharacterString	If NULL, all characters are available for input; otherwise, only
		///												characters in this string are available for input.
		///												For example, "AEIOUY" would disable all the virtual keyboard
		///												keys except for the upper case vowels.
		///												This arg will trump any control flags that disable keyboard keys.
		///												\ref OL_VIRTUAL_KEYBOARD_FLOAT or \ref OL_VIRTUAL_KEYBOARD_INT will overwrite this arg.
		///
		///		\param[in]	formatString	Used to format the input. Use '~' for alphabets, '#' for numbers, and '*' for any character.
		///										'*' can be used with character validation, and '~' can be used with \ref OL_VIRTUAL_KEYBOARD_NO_UPPER or \ref OL_VIRTUAL_KEYBOARD_NO_LOWER.
		///										For example, you can use "(###) ### - ####" for getting phone number from the user.
		///										Use \ref OL_VIRTUAL_KEYBOARD_RETURN_WITHOUT_FORMAT in controlFlags if you want the return value without format.
		///										The data will only return when the user fills out the formatted string.
		///										You may use "" for no format.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Request sent; expect a DataList or StatusResult event matching the supplied context.
		///		\retval OL_OVERLAY_IN_USE			The overlay is already displaying a modal dialog of some type.
		///		\retval OL_INVALID_PARAM			Invalid argument (ex: invalid input length or an empty character filter string).
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus GetInputFromKeyboard(OLContext context, const char* title, const char* description, 
											  const char* defaultString, OLVirtualKeyboardControl controlFlags, 
											  unsigned int minInputLength, unsigned int maxInputLength, 
											  const char *validCharacterString, const char* formatString) = 0;

		///
		/// \ingroup OLOVERLAY_API_MESSAGEBOX
		/// \brief Display a modal dialog box with a message, icon, and a configurable set of buttons; returns
		///			the user's selection in \ref OLOverlayEventHandler::MessageBoxResult or sends
		///			\ref OLOverlayEventHandler::StatusResult upon failure or cancellation.
		///
		/// DisplayMessageBox will fail if another modal dialog is already visible (ie: another messageBox,
		///	virtual keyboard, spinner dialog, or the gameInfo dialog).
		///
		/// Here is an example messagebox with the cheers icon:
		///	\image html overlay_ui_messagebox.png
		///
		/// \par The Button Format String
		///		Use the buttonFormatString to specify the buttons available on your dialog box.
		///		The messageBox supports an arbitrary number of buttons; use '|' to separate button
		///		labels and use '&' as the first character of a button to set it as the default selection.\n
		///		For example, "button 1|&button 2|button 3" will give us 3 buttons and designate the center one as
		///		the default.  The dialog above uses this button format string.
		///
		/// \par Using Icons
		///		The messageBox dialog allows you to optionally display a 48x48 pixel icon.  You can choose from a set of OnLive system
		///		icons, a blank icon, or a custom icon (see \ref OLOverlayIconTypes).  If using a custom icon,
		///		you must supply it as a void* to a png file buffer (loaded into memory) and the size of the buffer.  The png
		///		buffer must be <= 128k in size.  Images are scaled to the icon size if they are not correctly sized to
		///		begin with (images must be <= 128x128 pixels in all cases). Overlay API currently does not support 
		///		mediaserver URI for this function call, using \ref OL_OVERLAYICON_MEDIASERVER_URI
		///		will cause the call to return \ref OL_INVALID_PARAM.
		///
		/// \responseEventList
		///		The SDK responds with one of the following events if this request is successfully sent: 
		///
		///		\ospEvent OLOverlayEventHandler::MessageBoxResult
		///			\eventDesc	The OSP sends the index of the button selected by the user.
		///						Indices start with zero, as expected.
		///
		///		\ospEvent OLOverlayEventHandler::StatusResult
		///			\eventDesc	Indicates that the messagebox could not be displayed or that the user canceled
		///						the dialog.
		///
		///				\statusResult OL_CANCEL_BUTTON			The user canceled the messagebox by pressing
		///																escape (or 'b' on their controller).
		///				\statusResult OL_OVERLAY_IN_USE		Another modal dialog is already being displayed by
		///																the overlay, so the keyboard cannot be shown.
		///
		/// \responseEventListEnd
		///
		/// Note: The messagebox text size \#defines are UI limits and do not include a nul character.
		///
		///		\param[in]	context			Used to pair this request with the response event (see \ref olapi::OLGetNextContext).
		///
		///		\param[in]	title			Title text, up to \ref OL_MAX_MESSAGE_BOX_TITLE_SIZE chars in length.
		///									You may use "" if you do not want a title.
		///
		///		\param[in]	body			Message body text, up to \ref OL_MAX_MESSAGE_BOX_BODY_SIZE chars in length.
		///									You may use "" if you do not want a body.
		///
		///		\param[in] buttonFormatString	Defines the buttons present in the dialog as well as the default button.
		///										Button labels are separated with '|'; the default selection is signified by '&'.
		///										For example, "Yes|No|&Cancel"
		///
		///		\param[in]  iconType		The type of 48x48 pixel icon to display in the msg box. 
		///
		///		\param[in]  PNGFile			A pointer to a custom icon png file in memory.  The maximum image size is 128x128 
		///									pixels and will be scaled to the icon size (48x48).  Only used if iconType is
		///									OL_OVERLAYICON_CUSTOM (use NULL for other icon types).
		///									
		///		\param[in]  PNGFileSize		The size of the PNGFile buffer in bytes.  Must be <= 128k.
		///									Set to zero if PNGFile is NULL.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Request sent; expect OSP to return with an \ref OLOverlayEventHandler::MessageBoxResult
		///												or \ref OLOverlayEventHandler::StatusResult event.
		///		\retval OL_OVERLAY_IN_USE			The overlay is already displaying a modal dialog of some type.
		///		\retval OL_INVALID_PARAM			One of the supplied params was invalid; see the log for details.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus DisplayMessageBox(OLContext context, char* title, char* body, char* buttonFormatString,
										   OLOverlayIconTypes iconType = OL_OVERLAYICON_NONE, 
										   void* PNGFile = 0, 
										   int PNGFileSize = 0) = 0;

		///
		/// \ingroup OLOVERLAY_API_MENU_CONTROL
		/// \brief Displays a specific overlay menu screen or closes the overlay menu.  Acts as if the end
		///			user had opened the overlay to the specified menu screen.  Responds with an 
		///			\ref OLOverlayEventHandler::StatusResult Event with OL_SUCCESS or a failure code.
		///
		/// For example, you may want to add a button to your title's settings menu
		/// that will open the overlay's settings menu screen.  Opening the menu system will not
		///	affect any open overlay modal dialogs (the menu system will open behind the modal dialog and will not
		///	gain focus until the dialog is closed).
		///
		/// \warning We do not recommend modifying the overlay menus while they are visible, as you may
		///		interrupt whatever action the user is taking.  In other words, do not use DisplayMenu
		///		to change the currently opened overlay menu screen or close the menu.
		///
		/// \image html overlay_ui_menuSystem.png
		///
		/// \responseEventList
		///		The SDK responds with one of the following events if this request is successfully sent: 
		///
		///		\ospEvent OLOverlayEventHandler::StatusResult
		///			\eventDesc	Indicates a success (the menu was successfully shown or hidden), or that the
		///						operation failed.
		///
		///				\statusResult OL_SUCCESS			The Overlay Menu System has displayed the specified menu
		///															or hidden the menu entirely.
		///				\statusResult OL_FAIL				The menu cannot be shown or hidden (example: trying to show
		///															a menu while menus are disabled or trying to hide
		///															the menu when no menu is visible).
		///				\statusResult OL_OVERLAY_IN_USE		Another modal dialog is already being displayed by
		///															the overlay, so the menu cannot be shown.
		///				\statusResult OL_MENU_NOT_ENABLED	The requested menu is invisible or disabled by the OSP. (example:
		///															trying to show the achievement menu when the title does not
		///															have achievement enabled.)
		///
		/// \responseEventListEnd
		///
		///		\param[in]	context		Used to pair this request with the response event (see \ref olapi::OLGetNextContext).
		///
		///		\param[in]	menu		The menu screen to open.  Use \ref OL_OVERLAYMENU_NONE to close any shown menu.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; request sent. A StatusResult event is upon success or failure.
		///		\retval OL_OVERLAY_IN_USE			The overlay is already displaying a modal dialog of some type.
		///		\retval OL_INVALID_PARAM			One of the supplied params was invalid; see the log for details.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus DisplayMenu(OLContext context, OLOverlayMenus menu) = 0;



		///
		/// \ingroup OLOVERLAY_API_ALERTBOX
		/// \brief Display an alert notification to the currently bound user.  The alert appears in 
		/// the same location as the OnLive system alert notifications, but contains custom icons and text.
		///
		/// An Alert is a transient UI notification that displays a pair of icons and a short message.
		/// Users cannot interact with an alert, they animate in/out automatically. Here is an example
		/// alert dialog using the cheers & jeers icons:
		/// \image html overlay_ui_alertNotification.png
		///
		/// If multiple alerts are sent at once, they are queued up for display (FIFO) and shown one after another.
		/// While displaying queued alerts, the OSP does not send \ref OLOverlayEventHandler::Status events 
		/// in between the alerts (the alert status bit remains set until the alert queue is drained and no alerts
		/// are visible).
		///
		/// \par Using Icons
		///		The alert dialog allows you to optionally display two icons: a 64x64 pixel icon to the left of the message,
		///		and an 84x84 pixel icon to the right of the message.  You can choose from a set of OnLive system
		///		icons, a blank icon, or a custom icon (see \ref OLOverlayIconTypes).  If using a custom icon,
		///		you must supply it as a void* to a png file buffer (loaded into memory) and the size of the buffer.  The png
		///		buffer must be <= 128k in size.  Images are scaled to the icon size if they are not correctly sized to
		///		begin with (images must be <= 128x128 pixels in all cases).
		///
		/// Note: The alertbox text size \#defines are UI limits and do not include a nul character.
		///
		///		\param[in]	title				Title text, up to \ref OL_MAX_ALERT_TITLE_SIZE chars in length.
		///										You may use "" if you do not want a title.
		///
		///		\param[in]	subTitle			Subtitle text, up to \ref OL_MAX_ALERT_SUBTITLE_SIZE chars in length.
		///										You may use "" if you do not want a subtitle.
		///
		///		\param[in]	body				Message body text, up to \ref OL_MAX_ALERT_BODY_SIZE chars in length.
		///										You may use "" if you do not want a message body.
		///
		///		\param[in]  durationSec			The amount of time the alert will be displayed (in seconds). The range is from
		///										\ref OL_MIN_ALERT_DURATION to \ref OL_MAX_ALERT_DURATION. If the durationSec 
		///										is out of range, the OSP will use the closest acceptable value.
		///
		///		\param[in]  leftIconType		The type of 32x32 pixel icon to display to the left of the message.
		///
		///		\param[in]  leftIconData		If leftIconType is \ref OL_OVERLAYICON_CUSTOM, this is a pointer to a custom icon png file in memory.  
		///										The maximum image size is 128x128 pixels and will be scaled to the icon size (32x32). 
		///										If leftIconType is \ref OL_OVERLAYICON_MEDIASERVER_URI, 
		///										this is a pointer to the Mediaserver URI string. Otherwise, use NULL for other icon types.
		///									
		///		\param[in]  leftIconDataSize	The size of the leftIconData buffer in bytes.  Must be <= 128k.
		///										Set to zero if leftIconData is NULL or a Mediaserver URI.
		///
		///		\param[in]  rightIconType		The type of 48x48 pixel icon to display to the right of the message.
		///
		///		\param[in]  rightIconData		If rightIconType is \ref OL_OVERLAYICON_CUSTOM, this is a pointer to a custom icon png file in memory.  
		///										The maximum image size is 128x128 pixels and will be scaled to the icon size (32x32). 
		///										If rightIconType is \ref OL_OVERLAYICON_MEDIASERVER_URI, 
		///										this is a pointer to the Mediaserver URI string. Otherwise, use NULL for other icon types.
		///
		///		\param[in]  rightIconDataSize	The size of the rightIconData buffer in bytes.  Must be <= 128k.
		///										Set to zero if rightIconData is NULL or a Mediaserver URI.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the alert is being displayed (or queued for display).
		///		\retval OL_INVALID_PARAM			One of the supplied params was invalid; see the log for details.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus DisplayAlert(const char* title, const char* subTitle, const char *body, 
									  int durationSec = 5,
									  OLOverlayIconTypes leftIconType = OL_OVERLAYICON_NONE, 
									  void* leftIconData = 0, 
									  int leftIconDataSize = 0,
									  OLOverlayIconTypes rightIconType = OL_OVERLAYICON_NONE, 
									  void* rightIconData = 0, 
									  int rightIconDataSize = 0) = 0;

		///
		/// \ingroup OLOVERLAY_API_MENU_CONTROL
		/// \brief Disable/enable access to the overlay menu (affects both user hotkeys and
		///			programmatic access such as \ref DisplayMenu).
		///
		/// Should only be used when the overlay menu would significantly disrupt title operation.
		/// You are responsible for re-enabling the menu system if a user leaves your title while the overlay is disabled.
		///
		/// Calling DisableMenu while the overlay menu system is shown will close the menus and disable them.
		///
		///		\param[in]	disableMenuAccess		If true, the user cannot open the overlay; if false, the user can.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus DisableMenu(bool disableMenuAccess) = 0;



		///
		/// \ingroup OLOVERLAY_API_SPINNER_DIALOG
		/// \brief Display a modal dialog box with a spinning icon and text message (to indicate the game is busy).
		///			The user cannot interact with this dialog; it must be hidden programmatically.  Acts as a modal dialog.
		///			Responds with an \ref OLOverlayEventHandler::StatusResult event with OL_SUCCESS or a failure code.
		///
		/// \image html overlay_ui_spinner.png
		///
		///	ShowSpinnerDialog will fail if another modal dialog is being displayed (another spinner, a menu,
		/// virtual keyboard, message box, or the gameInfo dialog).
		///
		/// \responseEventList
		///		The SDK responds with a StatusResult event if this request is successfully sent: 
		///
		///		\ospEvent OLOverlayEventHandler::StatusResult
		///			\eventDesc	Indicates a success (the spinner was successfully shown or hidden), or that the
		///						operation failed.
		///
		///				\statusResult OL_SUCCESS			The spinner dialog was shown or hidden.
		///				\statusResult OL_FAIL				The spinner could not be shown or hidden (ex: trying to hide
		///															the spinner when it is not visible).
		///				\statusResult OL_OVERLAY_IN_USE		Another modal dialog is already being displayed by
		///															the overlay, so the spinner cannot be shown.
		///
		/// \responseEventListEnd
		///
		///		\param[in]	context		Used to pair this request with the response event (see \ref olapi::OLGetNextContext).
		///
		///		\param[in]	showDialog	If true, the spinner dialog is shown; otherwise, the dialog is hidden
		///
		///		\param[in]	dimScreen	If true, the portion of the screen behind the spinner is dimmed to indicate that it is
		///									inactive.  Only applies when you are showing the spinner.
		///
		///		\param[in]	message		Text message centered below the spinning icon in the dialog box, must be 
		///								<= \ref OL_MAX_SPINNER_MESSAGE_SIZE chars in length. Use NULL for no message.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; request sent. Expect a StatusResult event from the OSP matching the supplied context.
		///		\retval OL_OVERLAY_IN_USE			The overlay is already displaying a modal dialog of some type.
		///		\retval OL_INVALID_PARAM			One of the supplied params was invalid; see the log for details.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLSERVICE_API_CONNECTION_DISCONNECTION.
		///		\retval OL_INTERNAL_IO_ERROR		The SDK failed to send the request to the OSP.
		///
		virtual OLStatus ShowSpinnerDialog(OLContext context, bool showDialog, bool dimScreen = true, 
										   const char* message = 0) = 0;

	};

	///
	/// \ingroup OLOVERLAY_API_EVENT_HANDLING OLOVERLAY_API
	/// \class OLOverlayEventHandler
	/// \brief This is the OLOverlay API's Event interface; your OLOverlay event handler must implement this interface.
	///		Each method represents an asynchronous event returned by the SDK.
	///
	/// See \ref EVENTS_PAGE_CONCEPTS for an overview of event handling.
	///
	class OLOverlayEventHandler
	{
		public:


		///
		/// \ingroup OLOVERLAY_API_STATUS_EVENT
		/// \brief Event sent by the OSP when the Overlay status bitset changes (as menus, alert notifications,
		///				message boxes, etc are shown and hidden).
		///
		///		\param[in]  statusBits	Treat this enum as an integer bitset of
		///									the flags defined in \ref olapi::OLOverlayStatus.
		///
		///		\retval	OL_SUCCESS				The event was handled.
		///		\retval OL_NOT_IMPLEMENTED		The event was ignored.
		///
		virtual OLStatus Status(OLOverlayStatus statusBits) = 0;



		///
		/// \brief Event containing a successful set of data from a previous OSP request.  For example, 
		///			\ref OLOverlay::GetInputFromKeyboard returns the input text in this event.
		///
		/// This event is a generic way for the OSP to return data to your title.  The supplied
		///		\ref olapi::OLDataList struct contains a null terminated list of \ref olapi::OLDataItem objects.
		///		Each item has a size and a char array blob.
		///
		///	The actual type of the data in the dataList (and item blobs) varies according to the request that triggered
		///		this event.  For example, GetInputFromKeyboard returns a single item in the dataList,
		///		and that item's data blob is cast to the nul terminated string entered by the user.
		///
		///	Data formats are documented in the methods that trigger this event.
		///
		/// \warning The dataList supplied by this event is freed when the method returns; you must
		///		copy off any data you need to retain.
		///
		///		\param[in] context		The matching context you gave to the request for this result (see \ref EVENTS_PAGE_ASYNC_COMM).
		///
		///		\param[in] dataList		Data returned from a previous OSP request.  Data list length and underlying
		///									dataItem contents are defined by the OSP request this event is responding to.
		///
		///	\return An OLStatus code indicating if you have handled the event.
		///		\retval OL_SUCCESS				The event was handled.
		///		\retval OL_NOT_IMPLEMENTED		The event was ignored.
		///		\retval OL_FAIL					Event not handled.
		///
		virtual OLStatus DataList(OLContext context, OLDataList* dataList) = 0;



		///
		/// \brief Event returning the user's button selection (or cancellation) from a message box
		///		(see \ref OLOverlay::DisplayMessageBox).
		/// 
		///		\param[in] context		The matching context you gave to the DisplayMessageBox request for this result 
		///									(see \ref EVENTS_PAGE_ASYNC_COMM).
		///
		///		\param[in] buttonIndex	The button index (0 based) of the button selected by the user.
		///
		///	\return An OLStatus code indicating if you have handled the event.
		///		\retval OL_SUCCESS				The event was handled.
		///		\retval OL_NOT_IMPLEMENTED		The event was ignored.
		///		\retval OL_FAIL					Event not handled.
		///
		virtual OLStatus MessageBoxResult( OLContext context, int buttonIndex ) = 0;



		///
		/// \brief Event returning an OLStatus code result; this typically means an error was
		///		encountered while the OSP was processing an async request.
		///
		/// For example, if we request input from the user with the virtual keyboard 
		///	(see \ref OLOverlay::GetInputFromKeyboard) and the user cancels the input, 
		/// the OSP sends a StatusResult event with \ref OL_CANCEL_BUTTON as the status.
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
		///		\retval OL_FAIL					Event not handled.
		///
		virtual OLStatus StatusResult(OLContext context, OLStatus status) = 0;
	};

	///
	/// \ingroup OLOVERLAY_API_EVENT_HANDLING OLOVERLAY_API
	/// \class OLOverlayEventWaitingCallback
	/// \brief This is the OLOverlay API's callback interface; your title's OLOverlay callback handler must implement this interface.
	///
	/// Each method in this class represents an OLOverlay callback.  If you choose to handle OLOverlay callbacks,
	/// register your callback handler using \ref OLOverlay::SetEventWaitingCallback.  Callback methods
	/// may be invoked at any time once the OnLive Services are running (see \ref OLSERVICE_API_CONNECTION_DISCONNECTION).
	///
	/// \note Callback methods are always invoked from an internal OnLive SDK thread, so all callback implementations must be thread-safe.
	///
	/// See \ref EVENTS_PAGE_CONCEPTS for details.
	///
	class OLOverlayEventWaitingCallback
	{
		public:


		///
		/// \brief Callback dispatched when an OLOverlay event is waiting to be handled.
		///
		/// If your title handles events from callbacks, call \ref OLOverlay::HandleEvent(unsigned int) to dispatch the waiting event.
		///	If you handle events by polling, you can ignore this event.
		///
		/// \note The EventWaiting callback must be thread-safe and must complete in less than two milliseconds.
		///
		/// See \ref EVENTS_PAGE_HANDLING_OVERVIEW for more info.
		///
		virtual void EventWaiting() = 0;
	};

	/// \addtogroup OLOVERLAY_API_CREATE_AND_DESTROY
	/// 

	///
	/// \brief Get the OLOverlay API singleton (the instance is created if necessary).
	///
	///	The OLOverlay pointer can be stored until the API is destroyed with \ref olapi::OLStopOverlay().
	///
	/// See \ref OLOVERLAY_API_CREATE_AND_DESTROY.
	///
	///		\param[in] version		The API version string (for class compatibility), use \ref OVERLAY_API_VERSION.
	///
	///		\param[out] olOverlay	The supplied pointer is set to the OLOverlay instance on success.
	///									The pointer is not modified on failure.
	///
	///	\return An OLStatus code.
	///		\retval	OL_SUCCESS			Success; the olOverlay pointer is set to the singleton.
	///		\retval OL_INVALID_PARAM	The supplied version string or olOverlay pointer is NULL.
	///		\retval OL_INVALID_VERSION	The API version requested is not supported. This is a fatal 
	///										error and your title should exit.
	///		\retval OL_INTERNAL_ERROR	A general internal or system error.
	///		\retval OL_OUT_OF_MEMORY	The SDK could not alloc memory for the singleton.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLGetOverlay(const char* version, OLOverlay** olOverlay);

	///
	/// \brief Destroy the OLOverlay API singleton.
	///
	/// The OnLive Services should be stopped before you destroy an API instance; see \ref olapi::OLStopServices().
	///
	/// See \ref OLOVERLAY_API_CREATE_AND_DESTROY.
	///
	///		\param[in] olOverlay		Pointer to the OLOverlay instance to destroy.
	///
	///	\return Returns an OLStatus code.
	///		\retval	OL_SUCCESS			Success; the olOverlay was destroyed.
	///		\retval OL_INVALID_PARAM	The supplied olOverlay pointer was NULL.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLStopOverlay(OLOverlay* olOverlay);


	/// 


/*!

\addtogroup OLOVERLAY_API
	\brief \copybrief olapi::OLOverlay

\addtogroup OLOVERLAY_API_CREATE_AND_DESTROY
	\brief The OLOverlay singleton is created and retrieved with \ref olapi::OLGetOverlay() and destroyed with \ref olapi::OLStopOverlay(). Do 
	not destroy any of the OnLive APIs (or their handlers) until after you have stopped the OnLive Services. 

		See \ref SDK_SESSION_FLOW_PAGE for information about the session flow.

	See \ref OLSERVICE_API_CONNECTION_DISCONNECTION for information about OSP connection and disconnection.

\addtogroup OLOVERLAY_API_EVENT_HANDLING
	\brief Callbacks and events are used for asynchronous communication to and from the OSP. 
	
	See \ref SDK_EVENTS_PAGE for an overview of callback and events.

\addtogroup OLOVERLAY_API_VIRTUAL_KEYBOARD
	\brief The OnLive Overlay allows you to prompt for input from a virtual keyboard with \ref OLOverlay::GetInputFromKeyboard.
			Results are returned in either an \ref OLOverlayEventHandler::StatusResult or 
			\ref OLOverlayEventHandler::DataList event. 
			
	In this example, the input has been restricted to the letters "OnLive!":
	\image html overlay_ui_keyboard.png

\addtogroup OLOVERLAY_API_MENU_CONTROL
	\brief The Overlay "Menus" are the overlay screens the user sees when they bring up the overlay 
			(ex: Games, Settings, Inbox, Service, About).  These overlay menus can be shown, hidden, or 
			disabled. 
			
	For example, this is the Overlay "service" menu screen:
	\image html overlay_ui_menuSystem.png


\addtogroup OLOVERLAY_API_STATUS_EVENT
	\brief Whenever one of the Overlay UI elements is hidden or shown, an \ref OLOverlayEventHandler::Status event is sent to
			keep your title advised of the current status of the overlay UI. 
			
\addtogroup OLOVERLAY_API_SPINNER_DIALOG
	\brief The Spinner Dialog is a modal dialog that displays a short text message and a spinning icon.  It indicates
			that the game is busy.  The user cannot interact with the dialog; it must be shown/hidden programmatically and
			responds with an \ref OLOverlayEventHandler::StatusResult event with OL_SUCCESS or a failure code. 
			
	For example:
	\image html overlay_ui_spinner.png

\addtogroup OLOVERLAY_API_ALERTBOX
	\brief An Alert is a transient UI notification that displays a short message and optionally a pair of icons.  The OSP automatically displays
		certain system alerts (ex: friend login/out, spectators joining, etc), but your title can also display custom alerts.


	For example:
	\image html overlay_ui_alertNotification.png

	

\addtogroup OLOVERLAY_API_MESSAGEBOX
	\brief A message box is a modal dialog with an icon, title & body text, and an arbitrary set of buttons. 
	

	For example:
	\image html overlay_ui_messagebox.png


	
*/

}; // namespace olapi

#endif // OLOVERLAY_H
