/*!  \file kfbxeventhandler.h
 */

#ifndef FBXFILESDK_KFBXEVENTS_KFBXEVENTHANDLER_H
#define FBXFILESDK_KFBXEVENTS_KFBXEVENTHANDLER_H

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
#include <fbxfilesdk/kfbxevents/kfbxevents.h>

// FBX include
#include <fbxfilesdk/components/kbaselib/klib/kintrusivelist.h>

// FBX namespace begin
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>
namespace kfbxevents
{
    class KFbxListener;

    //-----------------------------------------------------------------
    /** FBX event handler class contains a listener and a callback function.
    * Event handler is used to bind emitter and listener together. Its callback function could process event data.
    * To generate a valid event handler, you can create an event emitter and event listener first and then call KFbxListener::Bind().
    * It will create an event handler automatically and bind the handler to the listener and the created emitter. 
    * After that, the emitter and listener are bound together via event handler.
    * \remarks An object(emitter) can emit a certain type of event, the plug-in(listener) who are listening to that type of event, 
    * will receive a signal and take action to process the event data. 
    * \par The whole process of event is:
    * \li 1. Create an emitter and a listener, then bind them together via the same event handler.
    * \li 2. Emitter can emit an event at certain conditions. The event could be handled by event handler.
    * \li 3. Once an event is emitted, the listener to this event will receive a signal. 
    * \li 4. And then the listener could process the event data according to the types of event, by calling event handler.
    * \note The event data is process by the callback function of event handler.
    * \nosubgrouping
    * \see KFbxListener KFbxEventBase KFbxEvent KFbxEmitter
    */
    class KFbxEventHandler
    {
    public:
        enum
        {
            eNODE_LISTENER = 0,
            eNODE_EMITTER,
            eNODE_COUNT
        };

        /** Constructor.
        */
        KFbxEventHandler(){}

        /** Destructor.
        */
        virtual ~KFbxEventHandler(){}

        // Handler handles a certain type of event.
        /** Get event type of current handler.
        * \return the type ID of event.
        */
        virtual int GetHandlerEventType() = 0;

        /** call callback function to process event data.
        * \param pEvent specify the event type. pEvent could be a specific class which derived from KFbxEventBase.
        * \see KFbxEventBase
        */
        virtual void FunctionCall(const KFbxEventBase& pEvent) = 0;

        /** Get listener of current handler.
        * \return listener
        */
        virtual KFbxListener* GetListener() = 0;

        KFBX_LISTNODE(KFbxEventHandler, eNODE_COUNT);
    };

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    //-----------------------------------------------------------------
    template <typename EventType, typename ListenerType>
    class KFbxMemberFuncEventHandler : public KFbxEventHandler
    {
    // VC6Note: There's no reason why the callback is a (const EventType*) it is obvious that it should be
    //           a (const EventType&) but because of a VC6 template limitation, we put  a pointer. 
    typedef void (ListenerType::*CBFunction)(const EventType*);

    public:
        KFbxMemberFuncEventHandler(ListenerType* pListenerInstance, CBFunction pFunc) :
            mListener(pListenerInstance),
            mFunc(pFunc)
        {
        }

        // From KFbxEventHandler
        virtual int GetHandlerEventType(){ return EventType::GetStaticTypeId(); }  
        virtual void FunctionCall(const KFbxEventBase& pEvent){ (*mListener.*mFunc)(reinterpret_cast<const EventType*>(&pEvent)); } 
        virtual KFbxListener* GetListener(){ return mListener;}
    private:
        ListenerType* mListener;

        // The callback function
        CBFunction mFunc;
    };

    //-----------------------------------------------------------------
    template <typename EventType, typename ListenerType>
    class KFbxConstMemberFuncEventHandler : public KFbxEventHandler
    {

    // VC6Note: There's no reason why the callback is a (const EventType*) it is obvious that it should be
    //           a (const EventType&) but because of a VC6 template limitation, we put  a pointer. 
    typedef void (ListenerType::*CBFunction)(const EventType*)const;

    public:
        KFbxConstMemberFuncEventHandler(ListenerType* pListenerInstance, CBFunction pFunc) :
            mListener(pListenerInstance),
            mFunc(pFunc)
        {
        }

        // From KFbxEventHandler
        virtual int GetHandlerEventType(){ return EventType::GetStaticTypeId(); }    
        virtual void FunctionCall(const KFbxEventBase& pEvent){ (*mListener.*mFunc)(reinterpret_cast<const EventType*>(&pEvent)); }
        virtual KFbxListener* GetListener(){ return mListener;}
    private:
        ListenerType* mListener;

        // The callback function
        CBFunction mFunc;
    };

    //-----------------------------------------------------------------
    template <typename EventType>
    class KFbxFuncEventHandler : public KFbxEventHandler
    {
    // VC6Note: There's no reason why the callback is a (const EventType*,KFbxListener*) it is obvious that it should be
    //           a (const EventType&,KFbxListener*) but because of a VC6 template limitation, we put  a pointer.
    typedef void (*CBFunction)(const EventType*,KFbxListener*);

    public:
        KFbxFuncEventHandler(KFbxListener* pListener, CBFunction pFunc) :
            mListener(pListener),
            mFunc(pFunc)
        {
        }

        // From KFbxEventHandler
        virtual int GetHandlerEventType(){ return EventType::GetStaticTypeId(); }   
        virtual void FunctionCall(const KFbxEventBase& pEvent){ (*mFunc)(reinterpret_cast<const EventType*>(&pEvent),mListener); }
        virtual KFbxListener* GetListener(){ return mListener; }
    private:
        KFbxListener* mListener;

        // The callback function
        CBFunction mFunc;
    };
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
}
// FBX namespace end
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXEVENTS_KFBXEVENTHANDLER_H

