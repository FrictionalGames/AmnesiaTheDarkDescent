/*!  \file kfbxlistener.h
 */

#ifndef FBXFILESDK_KFBXEVENTS_KFBXLISTENER_H
#define FBXFILESDK_KFBXEVENTS_KFBXLISTENER_H

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
#include <fbxfilesdk/kfbxevents/kfbxemitter.h>
#include <fbxfilesdk/kfbxevents/kfbxeventhandler.h>

// FBX include
#include <fbxfilesdk/components/kbaselib/klib/kintrusivelist.h>
#include <fbxfilesdk/fbxfilesdk_memory.h>

// FBX namespace begin
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

namespace kfbxevents
{
    /**FBX SDK listener class. Once an event is emitted by an emitter, a listener should be created to listen to the event. 
      * The listener could receive a signal and take action to process the event data. 
      * \note The data will be process by the callback function of KFbxListener::Bind().
      * Plug-in could be used as listener, since KFbxPlugin is derived from KFbxListener.
      * To emit event, you could create an emitter and a listener, and then bind them together via event handler.
      * To listen to an event which is emitted by an emitter, you should bind current listener to the emitter by calling KFbxListener::Bind(). 
      * Event listener contains a list of event handlers.
      * \remarks An object(emitter) can emit a certain type of event, the plug-in(listener) who are listening to that type of event, 
      * will receive a signal and take action to process the event data. 
      * \par The whole process of event is:
      * \li 1. Create an emitter and a listener, then bind them together via the same event handler.
      * \li 2. Emitter can emit an event at certain conditions. The event could be handled by event handler.
      * \li 3. Once an event is emitted, the listener to this event will receive a signal. 
      * \li 4. And then the listener could process the event data according to the types of event, by calling event handler.
      * \note The event data is process by the callback function of event handler.
      * \see KFbxEmitter KFbxEventHandler KFbxEvent KFbxEventBase
	  */
    class KFBX_DLL KFbxListener
    {
    public:
		/**
		  * \name Constructor and Destructor
		  */
		//@{
		//!Destructor.
        ~KFbxListener();
		//!Constructor.
        KFbxListener(){}
		//@}
       
		////////////////////////////////////////////////////////////////////////////////////////
        /**
        * \name Bind and unbind methods
        */
        //@{

        /**Bind current listener and the specified emitter together via an automatically created event handler. 
         * An event handler will be created automatically and added to the handlers list of current listener and the specified emitter.
         * After that, the listener can listen to the event which is emitted by the specified emitter.
		 * \param pEmitter          Event emitter to bind. Current listener can listen to the event which is emitted by pEmitter.
		 * \param pFunc             The callback function to process event date.
		 * \return                  The automatically created event handler.
		 */
        template <typename EventType,typename ListenerType>
        KFbxEventHandler* Bind(KFbxEmitter& pEmitter, void (ListenerType::*pFunc)(const EventType*))

        {
            KFbxMemberFuncEventHandler<EventType,ListenerType>* eventHandler = 
                FbxSdkNew< KFbxMemberFuncEventHandler<EventType,ListenerType> >(static_cast<ListenerType*>(this),pFunc);
            pEmitter.AddListener(*eventHandler);
            mEventHandler.PushBack(*eventHandler);
            return eventHandler;
        }

        /**Bind current listener and the specified emitter together via an automatically created event handler. 
        * An event handler will be created automatically and added to the handlers list of current listener and the specified emitter.
        * After that, the listener can listen to the event which is emitted by the specified emitter.
        * \param pEmitter          Event emitter to bind. Current listener can listen to the event which is emitted by pEmitter.
        * \param pFunc             The callback function to process event date.
        * \return                  The automatically created event handler.
        */
        template <typename EventType,typename ListenerType>
        KFbxEventHandler* Bind(KFbxEmitter& pEmitter, void (ListenerType::*pFunc)(const EventType*)const)
        {
            KFbxConstMemberFuncEventHandler<EventType,ListenerType>* eventHandler = 
                        FbxSdkNew< KFbxConstMemberFuncEventHandler<EventType,ListenerType> >(static_cast<ListenerType*>(this),pFunc);
            pEmitter.AddListener(*eventHandler);
            mEventHandler.PushBack(*eventHandler);
            return eventHandler;
        }

        /**Bind current listener and the specified emitter together via an automatically created event handler. 
        * An event handler will be created automatically and added to the handlers list of current listener and the specified emitter.
        * After that, the listener can listen to the event which is emitted by the specified emitter.
        * \param pEmitter          Event emitter to bind. Current listener can listen to the event which is emitted by pEmitter.
        * \param pFunc             The callback function to process event date.
        * \return                  The automatically created event handler.
        */
        template <typename EventType>
        KFbxEventHandler* Bind(KFbxEmitter& pEmitter, void (*pFunc)(const EventType*,KFbxListener*))
        {
            KFbxFuncEventHandler<EventType>* eventHandler = 
                            FbxSdkNew< KFbxFuncEventHandler<EventType> >(this, pFunc);
            pEmitter.AddListener(*eventHandler);
            mEventHandler.PushBack(*eventHandler);
            return eventHandler;
        }
        
		/**Unbind an event handler. The specified event handler will be removed from the handlers list of current listener. 
		  * \param aBindId       The event handler to unbind.
		  */
        void Unbind(const KFbxEventHandler* aBindId);
		//@}

    private:
        typedef KIntrusiveList<KFbxEventHandler, KFbxEventHandler::eNODE_LISTENER> EventHandlerList;
        EventHandlerList mEventHandler;
    };
}

// FBX namespace end
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXEVENTS_KFBXLISTENER_H

