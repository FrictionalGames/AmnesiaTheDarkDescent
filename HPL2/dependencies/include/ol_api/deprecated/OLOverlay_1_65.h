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
// OLOverlay.h $Revision: 55965 $
//

/// \file OLOverlay.h

#ifndef OLOVERLAY_H
#define OLOVERLAY_H

#include "OLAPI.h"

namespace olapi
{
	/*! \ingroup OVERLAY_API_INIT_DESTROY 
		\brief the default version of the OnLive OLOverlay API to use in \ref olapi::OLGetOverlay.
	*/
	#define OVERLAY_API_VERSION (OLAPI_VERSION)

	/// \addtogroup OVERLAY_MESSAGEBOX
	/// 

	#define OL_MAX_MESSAGE_BOX_TITLE_SIZE		128	///< Maximum string length of a messageBox's title (doesn't include the nul char).
	#define OL_MAX_MESSAGE_BOX_BODY_SIZE		256	///< Maximum string length of a messageBox's body text (doesn't include the nul char).
	#define OL_MAX_MESSAGE_BOX_BUTTON_SIZE		128	///< Maximum string length of a messageBox's button specifier text (doesn't include the nul char).

	/// 

	/// \addtogroup OVERLAY_ALERTBOX
	/// 

	#define OL_MAX_ALERT_TITLE_SIZE			32		///< Maximum string length of an Alert Box's title (doesn't include the nul char).
	#define OL_MAX_ALERT_SUBTITLE_SIZE		64		///< Maximum string length of an Alert Box's subtitle (doesn't include the nul char).
	#define OL_MAX_ALERT_BODY_SIZE			256		///< Maximum string length of an Alert Box's body text (doesn't include the nul char).

	// 


	///
	/// \ingroup OVERLAY_STATUS_EVENT
	/// \brief This enum is used as a bitset representing the types of Overlay UI elements
	///			that are currently visible.  See \ref olapi::OLOverlayEventHandler::Status.
	///
	enum OLOverlayStatus
	{
		OL_OVERLAY_DOWN						= 0,	///< No overlay elements are visible.
		OL_OVERLAY_MENU						= 1,	///< An overlay menu is visible (see \ref olapi::OLOverlay::DisplayMenu).
		OL_OVERLAY_ALERT					= 2,	///< An alert notification is visible (see \ref olapi::OLOverlay::DisplayAlert).
		OL_OVERLAY_KEYBOARD					= 4,	///< The virtual keyboard is visible( see \ref olapi::OLOverlay::GetInputFromKeyboard).
		OL_OVERLAY_MESSAGE_BOX				= 8,	///< Message Box on screen (see \ref olapi::OLOverlay::DisplayMessageBox).
		OL_OVERLAY_BUSY						= 16	///< The Spinner Dialog is on screen (see \ref olapi::OLOverlay::ShowSpinnerDialog).
	} ;

	///
	/// \ingroup OVERLAY_MENU_CONTROL
	/// \brief Used in \ref olapi::OLOverlay::DisplayMenu to show a specific Overlay Menu Screen
	///			or close the overlay.
	/// 
	enum OLOverlayMenus
	{
		OL_OVERLAYMENU_NONE,						///< Close the overlay menu
		OL_OVERLAYMENU_GAMES,						///< Main games menu, user can switch to another game here.
		OL_OVERLAYMENU_SETTINGS,					///< User settings menu.
		OL_OVERLAYMENU_INBOX,						///< Friends Inbox.
		OL_OVERLAYMENU_SERVICE,						///< Service menu.
		OL_OVERLAYMENU_ABOUT,						///< About Box.
		OL_OVERLAYMENU_MAX,					///< max enum value
		OL_OVERLAYMENU_DEFAULT = OL_OVERLAYMENU_GAMES	///< The default menu brought up when a user opens the overlay
	};

	///
	/// \ingroup OVERLAY_ALERTBOX
	/// \brief The 'type' of an icon; one of four OnLive system icons, no icon, or a custom image.
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
	enum OLOverlayIconTypes
	{
		OL_OVERLAYICON_NONE,						///< No Icon
		OL_OVERLAYICON_CHEERS,						///< Thumbs Up Icon
		OL_OVERLAYICON_JEERS,						///< Thumbs Down Icon
		OL_OVERLAYICON_FRIEND,						///< Friend Icon
		OL_OVERLAYICON_ADD_FRIEND,					///< Add Friend Icon
		OL_OVERLAYICON_CUSTOM = 1000				///< Custom Icon
	};

	class OLOverlayEventHandler;
	class OLOverlayEventWaitingCallback;

	///
	/// \ingroup OVERLAY 
	/// \class OLOverlay
	/// \brief The OnLive Overlay is a graphical interface superimposed over your application.  The OLOverlay API provides
	///			virtual keyboard support, modal dialog boxes, transient alert notifications, as well as programmatic
	///			ways to control the overlay menu system (as if the end user pressed Alt+O or the home button).
	///
	/// \par API Initialization
	///		The OLOverlay singleton is created/retrieved with \ref olapi::OLGetOverlay() and destroyed 
	///		with \ref olapi::OLStopOverlay().\n\n
	///		NOTE: OLOverlay depends on the OLService API and should be initialized after it. \n
	///		See \ref OVERLAY_API_INIT_DESTROY and \ref SDK_LIFECYCLE_PAGE for general info about 
	///		initializing the OnLive SDK APIs.
	///
	/// \par API Event and Callback Handling
	///		See \ref SDK_EVENTS_PAGE for an overview or \ref OVERLAY_EVENT_HANDLING for specifics.
	///
	/// \par Overlay Status Flag Events
	///		The overlay sends \ref OLOverlayEventHandler::Status events as it changes state (and as the various overlay UI
	///		elements are shown/hidden).  See \ref OVERLAY_STATUS_EVENT.
	///
	/// \par Controlling the Overlay Menu System
	///		The overlay menu screens are what the end user brings up by pressing the home button on their controller or
	///		Alt+O on their keyboard.  You can show the overlay menu system programmatically with \ref OLOverlay::DisplayMenu
	///		or disable the overlay menu system entirely with \ref OLOverlay::DisableMenu.
	///
	/// \par Overlay UI Elements
	///		The overlay also provides 4 UI elements that you application can use:
	///		<dl>
	///			<dt> \ref OVERLAY_VIRTUAL_KEYBOARD
	///				<dd> A modal dialog prompting the user to enter a text using a virtual keyboard UI.
	///					 See \ref OLOverlay::GetInputFromKeyboard.
	///			<dt> \ref OVERLAY_MESSAGEBOX
	///				<dd> A messageBox is a modal dialog with an icon, title & body text, and prompts the user
	///					 with an arbitrary set of buttons.  See \ref OLOverlay::DisplayMessageBox.
	///			<dt> \ref OVERLAY_SPINNER
	///				<dd> A modal dialog indicating that the app is busy (displays a spinning icon and a short message).
	///					 Users cannot interact with the spinner dialog; it must be shown/hidden programmatically.
	///					 See \ref OLOverlay::ShowSpinnerDialog.
	///			<dt> \ref OVERLAY_ALERTBOX
	///				<dd> An alertBox is a non-modal notification dialog showing a title, subTitle, message body, and
	///					a pair of icons (these fields are optional).  User cannot interact with alert boxes and
	///					alert boxes are automatically hidden after being displayed for a specified amount of time.
	///					See \ref OLOverlay::DisplayAlert.
	///		</dl>
	///
	/// \par Overlay Input Focus and Pausing Gameplay
	///		With the exception of alert boxes, visible overlay UI elements consume all input from the user until the
	///		element is dismissed.  Because of this, your game needs to pause gameplay while the overlay is up.\n\n
	///
	///		For user-initiated overlay interactions (such as hiding/showing the overlay with the controller or keyboard),
	///		the OSP sends \ref olapi::OLUserSessionEventHandler::Pause and \ref olapi::OLUserSessionEventHandler::Resume events to your app.\n\n
	///		
	///		If you are programmatically displaying a modal overlay dialog or menu, however, you are responsible
	///		for maintaining the correct gameplay state and the OSP will not send the Pause/Resume events.
	///
	class OLOverlay : public OLEventBase
	{
		public:
		/// \addtogroup OVERLAY_EVENT_HANDLING
		/// 
		/// \name OLUserSession Callbacks & Event Handling
		/// 

		/// 
		/// \brief Registers your callback handler object with the OLOverlay API.
		///
		/// If you intend to handle callbacks for OLOverlay, you should set
		/// a callback object before you start the OnLive Services with \ref olapi::OLStartServices.
		/// Once services are started, callbacks can be dispatched by the SDK at any time.
		///
		/// OLOverlay stores a single callback object pointer; calling this method
		/// multiple times will replace the previous pointer.  There's no way to 'unregister'
		/// the pointer (you can't set it to NULL).
		///
		///	See \ref EVENTS_PAGE_REGISTRATION for details.
		///
		///		\param[in]	callbackHandler	Your implementation of the \ref OLOverlayEventWaitingCallback interface.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS Normal operation; callbacks can be dispatched on the supplied obj while
		///							the services are running.
		///		\retval OL_INVALID_PARAM Error - the supplied callback obj pointer was NULL (or invalid);
		///							the callback obj has not been set.
		///
		virtual OLStatus SetEventWaitingCallback(OLOverlayEventWaitingCallback* callbackHandler) = 0;

		///
		/// \brief Registers your event handler object with the OLOverlay API.
		///
		/// Once you've set the OLOverlay API's eventHandler, calling one of the handle event methods
		///		may dispatch events on your object (assuming events are available to process).
		///		See \ref EVENTS_PAGE_HANDLING_OVERVIEW.
		///
		/// OLOverlay stores a single eventHandler pointer; calling this method
		/// multiple times will replace the previous pointer.  There's no way to 'unregister'
		/// the pointer (you can't set it to NULL).  
		///
		///	See \ref EVENTS_PAGE_REGISTRATION for details.
		///
		///		\param	eventHandler	Your implementation of the OLOverlayEventHandler interface.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS Normal operation; events will be dispatched on the supplied obj.
		///		\retval OL_INVALID_PARAM Error - the supplied eventHandler pointer was NULL (or invalid);
		///							the eventHandler has not been set.
		///
		virtual OLStatus SetEventHandler(OLOverlayEventHandler* eventHandler) = 0;

		///
		/// \brief Sets this API's EventHandler and attempts to handle a single event. NOTE: prefer to use the simpler
		///  overload \ref HandleEvent(unsigned int).
		///
		/// This method acts as a combination of \ref SetEventHandler and \ref HandleEvent(unsigned int).
		/// If the supplied eventHandler isn't NULL, we set the event handler to the supplied value then try to handle a single event.
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
		///		\retval OL_INVALID_PARAM	The supplied handler was NULL, but the API doesn't have an event handler set.
		///		\retval OL_EVENT_TIMEOUT	No events were available for the duration of the wait
		///
		virtual OLStatus HandleEvent(OLOverlayEventHandler* eventHandler, unsigned int waitTimeMs) = 0;

		///
		/// \brief Attempts to dispatch a single event on this API's registered event handler object.
		///
		/// You must register an event handler object with /ref OLOverlay::SetEventHandler before
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
		///		\retval OL_INVALID_PARAM	The API doesn't have an event handler set (see \ref OLOverlay::SetEventHandler).
		///		\retval OL_EVENT_TIMEOUT	No events were available for the duration of the wait.
		///
		virtual OLStatus HandleEvent(unsigned int waitTimeMs) = 0;

		// 
		// 

		///
		/// \ingroup OVERLAY_VIRTUAL_KEYBOARD
		/// \brief Display OnLive's virtual keyboard and prompts the user to enter text.  Acts as a modal dialog.
		///
		/// If the user cancels the input, the OSP sends an \ref OLOverlayEventHandler::StatusResult
		///		event with status code OL_CANCEL_BUTTON.
		///
		///	If the user submits input with the "Done" button, the OSP sends an
		///		\ref OLOverlayEventHandler::DataList Event.  The DataList has a single DataItem
		///		containing the nul terminated string.
		///
		///		Note that the text must be copied since the dataList is destroyed after the event is dispatched.
		///		For example:
		///
		/// \code
		///			const char *keyboardInput = NULL;
		///			olapi::OLDataItem *dataItem = (olapi::OLDataItem *)dataList->list;
		///			if (dataItem != NULL)
		///			{
		///				keyboardInput = strdup(dataItem->data);
		///				// alternatively, you can alloc a buffer of dataItem->size chars and use it.
		///			}
		///	\endcode
		///
		/// \par Limitations
		///		The virtual keyboard doesn't allow a custom prompt or an initial text value at this time.
		///		The virtual keyboard, message box, and spinner dialogs are mutually exclusive; only 1
		///		of these UI elements can be displayed by the overlay at a time.
		///
		///		\param[in]	context			Used to pair this request with the response event (see \ref olapi::OLGetNextContext).
		///
		///		\param[in]	olStringType	This arg is not used currently; but in the future it will control
		///										the keyboard's initial mode (text vs numeric) and the input format allowed.
		///
		///		\param[in]	maxInputLength	The max length of the input string allowed in the UI.
		///										The UI doesn't clamp the value you supply, so request a reasonable length.
		///
		///		\param[in]	validCharacterString	If NULL, all characters are available for input; otherwise only
		///												characters in this string are available for input.
		///												For example, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" would only allow
		///												uppercase letters in the input string (other virtual keyboard
		///												buttons are disabled).
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the virtual keyboard has been displayed.
		///		\retval OL_INVALID_PARAM			Invalid argument (ex: negative input length or an empty character filter string).
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLAPI_SERVICES.
		///		\retval OL_OVERLAY_IN_USE			The overlay is already displaying a modal dialog of some type.
		///
		virtual OLStatus GetInputFromKeyboard(OLContext context, OLStringType olStringType, unsigned int maxInputLength, 
												const char *validCharacterString = NULL) = 0;

		///
		/// \ingroup OVERLAY_MESSAGEBOX
		/// \brief Display a modal dialog box with a message, icon, and a configurable set of buttons; returns
		///			the user's selection in \ref OLOverlayEventHandler::MessageBoxResult or sends
		///			\ref OLOverlayEventHandler::StatusResult upon failure.
		///
		/// DisplayMessageBox will fail if another modal dialog is already visible (ie: another messageBox,
		///	virtual keyboard, or spinner dialog).
		///
		/// Here's an example messagebox with the cheers icon:
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
		///		begin with (images must be <= 128x128 pixels in all cases).
		///
		/// \par Handling User Input
		///		The end user can select one of the buttons or hit escape to cancel the dialog.  In either case,
		///		the OSP returns an \ref OLOverlayEventHandler::MessageBoxResult with the index of the selected button.
		///		If the dialog was canceled, -1 is returned.
		///
		/// Note: the text size defines are UI limits and do not include a nul character.
		///
		///		\param[in]	context			Used to pair this request with the response event (see \ref olapi::OLGetNextContext).
		///
		///		\param[in]	title			Title text, up to \ref OL_MAX_MESSAGE_BOX_TITLE_SIZE chars in length.
		///									You may use "" if you don't want a title.
		///
		///		\param[in]	body			Message body text, up to \ref OL_MAX_MESSAGE_BOX_BODY_SIZE chars in length.
		///									You may use "" if you don't want a body.
		///
		///		\param[in] buttonFormatString	Defines the buttons present in the dialog as well as the default button.
		///										Button labels are separated with '|' and '&' signifies the default selection.
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
		///		\retval OL_SUCCESS					Request sent; wait for the user's selection (\ref OLOverlayEventHandler::MessageBoxResult)
		///												or \ref OLOverlayEventHandler::StatusResult if the UI failed to display.
		///		\retval OL_INVALID_PARAM			One of the text fields is NULL or a problem with a supplied png file buffer.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLAPI_SERVICES.
		///		\retval OL_OVERLAY_IN_USE			The overlay is already displaying a modal dialog of some type.
		///
		virtual OLStatus DisplayMessageBox(OLContext context, char* title, char* body, char* buttonFormatString,
											OLOverlayIconTypes iconType = OL_OVERLAYICON_NONE, void* PNGFile = NULL, int PNGFileSize = 0) = 0;

		///
		/// \ingroup OVERLAY_MENU_CONTROL
		/// \brief Display a specific overlay menu screen or closes the overlay menu.  Acts as if the end
		///			user had opened the overlay to the specified menu screen.
		///
		/// For example, you may want to add a button to your title's settings menu
		/// that will open the overlay's settings menu screen.  Opening the menu system will not
		///	affect any open overlay modal dialogs (the menu system will open behind the modal dialog and won't
		///	gain focus until the dialog is closed).
		///
		/// The OSP will send an \ref OLOverlayEventHandler::StatusResult event indicating
		///		success or failure.  Changes to the overlay menu status may also generate
		///		\ref OLOverlayEventHandler::Status event(s).
		///
		/// \warning We do not recommend modifying the overlay menus while they are visible, as you may
		///		interrupt whatever action the user is taking.  In other words, don't use DisplayMenu
		///		to change the currently opened overlay menu screen or close the menu.
		///
		///		\param[in]	context		Used to pair this request with the response event (see \ref olapi::OLGetNextContext).
		///
		///		\param[in]	menu		The menu screen to open.  Use \ref OL_OVERLAYMENU_NONE to close any shown menu.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the menu is being displayed.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLAPI_SERVICES.
		///		\retval OL_OVERLAY_IN_USE			The overlay is already displaying a modal dialog of some type.
		///
		virtual OLStatus DisplayMenu(OLContext context, OLOverlayMenus menu) = 0;

		///
		/// \ingroup OVERLAY_ALERTBOX
		/// \brief Display an alert notification to the currently bound user.  The alert appears in 
		/// the same location as the OnLive system alert notifications, but contains custom icons and text.
		///
		/// An Alert is transient UI notification that displays a pair of icons and a short message.
		/// Users cannot interact with an alert, they animate in/out automatically. Here's an example
		/// alert dialog using the cheers & jeers icons:
		/// \image html overlay_ui_alertNotification.png
		///
		/// If multiple alerts are sent at once, they are queued up for display (FIFO) and shown one after another.
		/// While displaying queued alerts, the OSP does not send \ref OLOverlayEventHandler::Status events 
		/// in between the alerts (the alert status bit remains set until the alert queue is drained and no alerts
		/// are visible).
		///
		/// \par Using Icons
		///		The alert dialog allows you to optionally display 2 icons: a 64x64 pixel icon to the left of the message,
		///		and an 84x84 pixel icon to the right of the message.  You can choose from a set of OnLive system
		///		icons, a blank icon, or a custom icon (see \ref OLOverlayIconTypes).  If using a custom icon,
		///		you must supply it as a void* to a png file buffer (loaded into memory) and the size of the buffer.  The png
		///		buffer must be <= 128k in size.  Images are scaled to the icon size if they are not correctly sized to
		///		begin with (images must be <= 128x128 pixels in all cases).
		///
		/// Note: the text size defines are UI limits and do not include a nul character.
		///
		///		\param[in]	title			Title text, up to \ref OL_MAX_ALERT_TITLE_SIZE chars in length.
		///									You may use "" if you don't want a title.
		///
		///		\param[in]	subTitle		Subtitle text, up to \ref OL_MAX_ALERT_SUBTITLE_SIZE chars in length.
		///									You may use "" if you don't want a subtitle.
		///
		///		\param[in]	body			Message body text, up to \ref OL_MAX_ALERT_BODY_SIZE chars in length.
		///									You may use "" if you don't want a message body.
		///
		///		\param[in]  durationSec		The amount of time the alert will be displayed (in seconds).
		///
		///		\param[in]  leftIconType	The type of 32x32 pixel icon to display to the left of the message.
		///
		///		\param[in]  leftPNGFile		A pointer to a custom icon png file in memory.  The maximum image size is 128x128 
		///									pixels and will be scaled to the icon size (32x32).  Only used if leftIconType is
		///									OL_OVERLAYICON_CUSTOM (use NULL for other icon types).
		///									
		///		\param[in]  leftPNGFileSize	The size of the leftPNGFile buffer in bytes.  Must be <= 128k.
		///									Set to zero if leftPNGFile is NULL.
		///
		///		\param[in]  rightIconType	The type of 48x48 pixel icon to display to the right of the message.
		///
		///		\param[in]  rightPNGFile	A pointer to a custom icon png file in memory.  The maximum image size is 128x128 
		///									pixels and will be scaled to the icon size (48x48).  Only used if rightIconType is
		///									OL_OVERLAYICON_CUSTOM.  Use NULL for other icon types.
		///
		///		\param[in]  rightPNGFileSize	The size of the rightPNGFile buffer in bytes.  Must be <= 128k.
		///										Set to zero if rightPNGFile is NULL.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success; the alert is being displayed (or queued for display).
		///		\retval OL_INVALID_PARAM			One of the text fields is NULL or a problem with a supplied png file buffer.
		///		\retval OL_SESSION_NOT_STARTED		Calling the method outside of a user session (user not bound).
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLAPI_SERVICES.
		///
		virtual OLStatus DisplayAlert(const char* title, const char* subTitle, const char *body, int durationSec = 3,
										OLOverlayIconTypes leftIconType = OL_OVERLAYICON_NONE, void* leftPNGFile = NULL, int leftPNGFileSize = 0,
										OLOverlayIconTypes rightIconType = OL_OVERLAYICON_NONE, void* rightPNGFile = NULL, int rightPNGFileSize = 0) = 0;

		///
		/// \ingroup OVERLAY_MENU_CONTROL
		/// \brief Disable/enable access to the overlay menu (affects both end user hotkeys and
		///			programmatic access such as \ref DisplayMenu).
		///
		/// Should only be used when the overlay menu would significantly disrupt application operation.
		/// You are responsible for re-enabling the menu system if a user leaves your app while the overlay is disabled.
		///
		/// Calling DisableMenu while the overlay menu system is shown will close the menus and disable them.
		///
		///		\param[in]	disableMenuAccess		If true, the user cannot open the overlay; if false, the user can.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Success.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLAPI_SERVICES.
		///
		virtual OLStatus DisableMenu(bool disableMenuAccess) = 0;

		///
		/// \ingroup OVERLAY_SPINNER
		/// \brief Display a modal dialog box with a spinning icon and text message (to indicate the game is busy).
		///			The user cannot interact with this dialog; it must be hidden programatically.  Acts as a modal dialog.
		///
		/// \image html overlay_ui_spinner.png
		///
		///	The spinner dialog will replace any visible Overlay Menu; menus are not restored when the
		/// spinner is dismissed.  ShowSpinnerDialog will fail if another modal dialog is being displayed (another spinner,
		/// a virtual keyboard, or a message box).
		///
		///		\param[in]	context		Used to pair this request with the response event (see \ref olapi::OLGetNextContext).
		///
		///		\param[in]	showDialog	If true, the spinner dialog is shown; otherwise the dialog is hidden
		///
		///		\param[in]	dimScreen	If true, the portion of the screen behind the spinner is dimmed to indicate that it's
		///									inactive.  Only applies when you're showing the spinner.
		///
		///		\param[in]	message		Text message centered below the spinning icon in the dialog box.
		///									Use NULL for no message.
		///
		///	\return Returns an OLStatus code.
		///		\retval OL_SUCCESS					Request sent; expect an \ref OLOverlayEventHandler::StatusResult event
		///												with the supplied context.
		///		\retval OL_SERVICES_NOT_STARTED		The OnLive Services are not running; see \ref OLAPI_SERVICES.
		///		\retval OL_OVERLAY_IN_USE			The overlay is already displaying a modal dialog of some type.
		///
		virtual OLStatus ShowSpinnerDialog(OLContext context, bool showDialog, bool dimScreen = true, const char* message = NULL) = 0;

	};

	///
	/// \ingroup OVERLAY_EVENT_HANDLING OVERLAY
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
		/// \ingroup OVERLAY_STATUS_EVENT
		/// \brief Event sent by OSP when the Overlay status bitset changes (as menus, alert notifications,
		///				message boxes, etc are shown and hidden).
		///
		///		\param[in]  statusBits	Treat this enum as an integer bitset of
		///									the flags defined in \ref olapi::OLOverlayStatus.
		///
		///		\return	OLStatus		OL_SUCCESS Event handled. \n
		///								OL_NOT_IMPLEMENTED Event not handled.
		///
		virtual OLStatus Status(OLOverlayStatus statusBits) = 0;

		///
		/// \brief Event containing a successful set of data from a previous OSP request.  For example, 
		///			\ref OLOverlay::GetInputFromKeyboard returns the input text in this event.
		///
		/// This event is a generic way for the OSP to return data to your application.  The supplied
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
		///	\return An OLStatus code indicating if you've handled the event.
		///		\retval OL_SUCCESS				The event was handled.
		///		\retval OL_NOT_IMPLEMENTED		The event was ignored.
		///		\retval OL_NOT_IMPLEMENTED		Event not handled due to invalid params (ie: dataList was NULL or an unexpected length)
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
		///									(or -1 if the dialog was canceled).
		///
		///	\return An OLStatus code indicating if you've handled the event.
		///		\retval OL_SUCCESS				The event was handled.
		///		\retval OL_NOT_IMPLEMENTED		The event was ignored.
		///		\retval OL_NOT_IMPLEMENTED		Event not handled due to invalid params (button index out of range)
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
		///	\return An OLStatus code indicating if you've handled the event.
		///		\retval OL_SUCCESS				The event was handled.
		///		\retval OL_NOT_IMPLEMENTED		The event was ignored.
		///		\retval OL_NOT_IMPLEMENTED		Event not handled due to invalid params (ie: dataList was NULL or an unexpected length)
		///		\retval OL_FAIL					Event not handled.
		///
		virtual OLStatus StatusResult(OLContext context, OLStatus status) = 0;
	};

	///
	/// \ingroup OVERLAY_EVENT_HANDLING OVERLAY
	/// \class OLOverlayEventWaitingCallback
	/// \brief This is the OLOverlay API's callback interface; your app's OLOverlay callback handler must implement this interface.
	///
	/// Each method in this class represents an OLOverlay callback.  If you choose to handle OLOverlay callbacks,
	/// register your callback handler using \ref OLOverlay::SetEventWaitingCallback.  Callback methods
	/// may be invoked at any time once the OnLive Services are running (see \ref OLAPI_SERVICES).
	///
	/// \note Callback methods are always invoked from an internal OnLive SDK thread, so all callback implementations must be thread safe.
	///
	/// See \ref EVENTS_PAGE_CONCEPTS for details.
	///
	class OLOverlayEventWaitingCallback
	{
		public:

		///
		/// \brief Callback dispatched when an OLOverlay event is waiting to be handled.
		///
		/// If your application handles events from callbacks, call \ref OLOverlay::HandleEvent(unsigned int) to dispatch the waiting event.
		///	If you handle events by polling, you can ignore this event.
		///
		/// \note The EventWaiting callback must be thread safe and must complete in less than 2 milliseconds.
		///
		/// See \ref EVENTS_PAGE_HANDLING_OVERVIEW for more info.
		///
		virtual void EventWaiting() = 0;
	};


	/// \addtogroup OVERLAY_API_INIT_DESTROY
	/// 

	///
	/// \brief Get the OLOverlay API singleton (the instance is created if necessary).
	///
	///	The OLOverlay pointer can be stored until the API is destroyed with \ref olapi::OLStopOverlay().
	///
	/// See \ref OVERLAY_API_INIT_DESTROY.
	///
	///		\param[in] version		The API version string (for class compatibility), use \ref OVERLAY_API_VERSION.
	///
	///		\param[out] olOverlay	The supplied pointer is set to the OLOverlay instance on success.
	///									The pointer is not modified on failure.
	///
	///	\return An OLStatus code.
	///		\retval	OL_SUCCESS			Success; the olOverlay pointer is set to the singleton.
	///		\retval OL_INVALID_PARAM	The supplied version string or olOverlay pointer is NULL.
	///		\retval OL_INVALID_VERSION	The API version requested isn't supported. This is a fatal 
	///										error and your app should exit.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLGetOverlay(const char* version, OLOverlay** olOverlay);

	///
	/// \brief Destroy the OLOverlay API singleton.
	///
	/// The OnLive Services should be stopped before you destroy an API instance; see \ref olapi::OLStopServices().
	///
	/// See \ref OVERLAY_API_INIT_DESTROY.
	///
	///		\param[in] olOverlay		Pointer to the OLOverlay instance to destroy.
	///
	///	\return Returns an OLStatus code.
	///		\retval	OL_SUCCESS			Success; the olOverlay was destroyed.
	///		\retval OL_INVALID_PARAM	The supplied olOverlay pointer was NULL.
	///
	extern "C" __declspec(dllexport) OLStatus __cdecl OLStopOverlay(OLOverlay* olOverlay);

	/// 


// doxygen group comments
/*!

\addtogroup OVERLAY
	\brief \copybrief olapi::OLOverlay

\addtogroup OVERLAY_API_INIT_DESTROY
	\brief The OLOverlay singleton is created/retrieved with \ref olapi::OLGetOverlay() and destroyed with \ref olapi::OLStopOverlay().

	Don't destroy any of the OnLive APIs (or their handlers) until after you've stopped the OnLive Services.
	See \ref SDK_LIFECYCLE_PAGE and \ref OLAPI_SERVICES.

\addtogroup OVERLAY_EVENT_HANDLING
	\brief Items related to processing OLOverlay callbacks and events.

	See \ref SDK_EVENTS_PAGE for an overview.

\addtogroup OVERLAY_VIRTUAL_KEYBOARD
	\brief The OnLive Overlay allows you to prompt for input from a virtual keyboard with \ref OLOverlay::GetInputFromKeyboard.
			Results are returned in either an \ref OLOverlayEventHandler::StatusResult even (failure or input canceled) or 
			an \ref OLOverlayEventHandler::DataList event (successful input).

\addtogroup OVERLAY_MENU_CONTROL
	\brief The Overlay "Menus" are the overlay screens the user sees when they bring up the overlay 
			(ex: Games, Settings, Inbox, Service, About).  These overlay menus can be shown, hidden, or 
			disabled.

\addtogroup OVERLAY_STATUS_EVENT
	\brief Whenever one of the Overlay UI elements is hidden or shown, an \ref OLOverlayEventHandler::Status event is sent to
			keep your app advised of the current status of the overlay UI.

\addtogroup OVERLAY_SPINNER
	\brief The Spinner Dialog is a modal dialog that displays a short text message and a spinning icon.  It indicates
			that the game is busy.  The user cannot interact with the dialog; it must be shown/hidden programatically.

	\image html overlay_ui_spinner.png

\addtogroup OVERLAY_ALERTBOX
	\brief An Alert is a transient UI notification that displays a short message and optionally a pair of icons.  The OSP automatically displays
		certain system alerts (ex: friend login/out, spectators joining, etc), but your title can also display custom alerts.  For example:

	\image html overlay_ui_alertNotification.png

\addtogroup OVERLAY_MESSAGEBOX
	\brief A message box is a modal dialog with an icon, title & body text, and an arbitrary set of buttons.

	For example:
	\image html overlay_ui_messagebox.png

*/

}; // namespace olapi

#endif // OLOVERLAY_H
