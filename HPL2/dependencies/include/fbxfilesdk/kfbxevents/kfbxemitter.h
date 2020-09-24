/*!  \file kfbxemitter.h
 */

#ifndef FBXFILESDK_KFBXEVENTS_KFBXEMITTER_H
#define FBXFILESDK_KFBXEVENTS_KFBXEMITTER_H

/**************************************************************************************

 Copyright (C) 2001 - 2010 Autodesk, Inc. and/or its licensors.
 All Rights Reserved.

 The coded instructions, statements, computer programs, and/or related material 
 (collectively the "Data") in these files contain unpublished information 
 proprietary to Autodesk, Inc. and/or its licensors, which is protected by 
 Canada and United States of America federal copyright law and by international 
 treaties. 
 
 The Data may not be disclosed or distributed to third parties, in whole or in
 part, without the prior written consent of Autodesk, Inc. ("Autodesk").

 THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
 ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
 WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
 BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE, 
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE. 
 WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
 OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE. 
 
 IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
 OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
 OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
 SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
 OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
 HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
 FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
 ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
 WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
 OR DAMAGE. 

**************************************************************************************/
#include <fbxfilesdk/fbxfilesdk_def.h>

// Local includes 
#include <fbxfilesdk/kfbxevents/kfbxeventhandler.h>

// FBX include
#include <fbxfilesdk/kfbxmp/kfbxmutex.h>
#include <fbxfilesdk/components/kbaselib/klib/kintrusivelist.h>

// FBX namespace begin
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

namespace kfbxevents
{
    class KFbxListener;

    /** Base class to emit event with the specified event type.
    * The event type could be a specific class which derived from KFbxEvent. Please read KFbxEmitter::Emit() for more details. 
    * Event emitter contains a list of event handlers.
    * FBX object could be used as emitter, since KFbxObject is derived from KFbxEmitter.
    * Before using emitter to emit an event, one or more event handlers must be added to the handlers list of current emitter.
    * In other words, it's "bind event handlers to emitter".
    * There are two ways to bind event handlers to emitter.
    * \li 1. If you already got an event handler and would like to bind it to current emitter, please call KFbxEmitter::AddListener().
    * \li 2. Or you can create an event listener first and then call KFbxListener::Bind().
    * It will create an event handler automatically and bind the handler to the specified emitter.
    * It's similar to unbind or remove an even handler. For more details, 
    * \see KFbxEmitter::RemoveListener()
    * \see KFbxListener::Unbind()
    * \note Only Emit() is thread-safe.
    * \remarks An object(emitter) can emit a certain type of event, the plug-in(listener) who are listening to that type of event, 
    * will receive a signal and take action to process the event data. 
    * \par The whole process of event is:
    * \li 1. Create an emitter and a listener, then bind them together via the same event handler.
    * \li 2. Emitter can emit an event at certain conditions. The event could be handled by event handler.
    * \li 3. Once an event is emitted, the listener to this event will receive a signal. 
    * \li 4. And then the listener could process the event data according to the types of event, by calling event handler.
    * \note The event data is process by the callback function of event handler.
    * \nosubgrouping
    * \see KFbxListener KFbxEventHandler KFbxEvent KFbxEventBase
    */
    class KFBX_DLL KFbxEmitter
    {
    public:

        //!Constructor
        KFbxEmitter();

        //!Destructor
        ~KFbxEmitter();

        /** The specified event handler will be added to the handlers list of current emitter.
        * Bind the specified event handler to current emitter actually.
        * \note This is not a thread-safe method!
        * \param pHandler the event handler will be added to the handlers list of current emitter.
        */
        void AddListener(KFbxEventHandler& pHandler);

        /** The specified event handler will be removed from the handlers list of current emitter.
        * \note This is not a thread-safe method!
        * \param pHandler the event handler will be removed from the handlers list of current emitter.
        */
        void RemoveListener(KFbxEventHandler& pHandler);

        ////////////////////////////////////////////////////////
        template <typename EventType>

        /** Emit an event with the specified the event type. One the event is emitted, the listener to this event will receive a signal.
         * \note This is a thread-safe method!
         * \param pEvent specify the event type to emit. pEvent could be a specific class which derived from KFbxEvent, such as KFbxObjectPropertyChanged.
         * \remarks For example:
         * \code KFbxObject::Emit(KFbxObjectPropertyChanged(*pProperty)); \endcode
         * \see KFbxEventBase KFbxObjectPropertyChanged KFbxEventReferencedDocument KFbxEventPostExport
         * \see KFbxEventPostImport KFbxEventPreExport KFbxEventPreImport KFbxEventPopulateSystemLibrary
         */
        void Emit(const EventType& pEvent) const
        {
            if ( mData )
            {
                kfbxmp::KFbxMutexHelper lLock( mData->mMutex );

                EventHandlerList::iterator itBegin = mData->mEventHandler.Begin();
                EventHandlerList::iterator itEnd = mData->mEventHandler.End();
                for (EventHandlerList::iterator it = itBegin; it!=itEnd; ++it)
                {
                    if ((*it).GetHandlerEventType() == pEvent.GetTypeId())
                        (*it).FunctionCall(pEvent);
                }
            }
        }

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

    protected:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
        typedef KIntrusiveList<KFbxEventHandler, KFbxEventHandler::eNODE_EMITTER> EventHandlerList;

        struct KFbxEmitterData
        {
            EventHandlerList mEventHandler;
            mutable kfbxmp::KFbxMutex mMutex;
        };

        KFbxEmitterData* mData;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
    };

}
// FBX namespace end
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXEVENTS_KFBXEMITTER_H

