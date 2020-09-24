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
// OLAPI.h $Revision: 73887 $
//

/// \file OLAPI.h
/// \brief General Onlive SDK includes.
///
/// General Onlive SDK includes.

#ifndef OLAPI_H
#define OLAPI_H

#include <ol_api/OLAPIVersion.h>
#include <ol_api/OLID.h>
#include <ol_api/OLTypes.h>

namespace olapi
{
	///
	/// \class OLEventBase
	///
	/// \brief The OLEventBase class provides access to API event handling from the SDK
	/// OLHandleEvent() function.
	///
	/// As new APIs are added to the OnLive SDK, 
	/// their corresponding DLLs can handle events through OLAPI.dll.
	///
	class OLEventBase
	{
		public:

		///
		/// \brief Attempts to dispatch a single event on this API's registered event handler object.
		///
		/// See \ref SDK_EVENTS_PAGE for details.
		///
		///		\param[in]	waitTimeMs	The max amount of time to wait for an event in milliseconds.
		///								Use 0 to return immediately if there is no event waiting (non-blocking).
		///								Otherwise, this call may block for up to waitTimeMs if no event is available.
		///								You can use \ref OL_INFINITE to wait forever for the next event.
		///
		///		\return Returns an OLStatus code.
		///		\retval OL_SUCCESS			An event was dispatched.
		///		\retval OL_INVALID_PARAM	The API does not have an event handler set.
		///		\retval OL_EVENT_TIMEOUT	No events were available for the duration of the wait
		///
		virtual OLStatus HandleEvent(unsigned int waitTimeMs) = 0;

	};
}  // namespace olapi

#endif // OLAPI_H
 
