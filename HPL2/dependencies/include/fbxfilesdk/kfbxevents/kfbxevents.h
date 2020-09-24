/*!  \file kfbxevents.h
 */

#ifndef FBXFILESDK_KFBXEVENTS_KFBXEVENTS_H
#define FBXFILESDK_KFBXEVENTS_KFBXEVENTS_H

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

#include <fbxfilesdk/kfbxplugins/kfbxtypes.h>
#include <fbxfilesdk/kfbxplugins/kfbxdatatypes.h>
#include <fbxfilesdk/kfbxmp/kfbxmutex.h>

// FBX namespace begin
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>
namespace kfbxevents
{
    /** FBX SDK event base class. 
      * An event is something that is emitted by an emitter, with the goal of being filled by the listener that listen to it. 
      * You can see that like a form that you send to some people. If those people know how to fill the form, they fill it and return
      * it to you with the right information in it. FBX object could be used as emitter, since KFbxObject is derived from KFbxEmitter.
      * Meanwhile, plug-in could be used as listener, since KFbxPlugin is derived from KFbxListener.
      * The derived class of KFbxEventBase contains a type ID to distinguish different types of events.
      * FBX object can emit different types of FBX events at different conditions. 
      * \par The whole process of event is:
      * \li 1. Create an emitter and a listener, then bind them together via the same event handler.
      * \li 2. Emitter can emit an event at certain conditions. The event could be handled by event handler.
      * \li 3. Once an event is emitted, the listener to this event will receive a signal. 
      * \li 4. And then the listener could process the event data according to the types of event, by calling event handler.
      * \note The event data is process by the callback function of event handler.
      * For example, if a certain property of a FBX object is changed, the FBX object(emitter) can emit an event which type is KFbxObjectPropertyChanged.
      * The plug-in(listener) who are listening to KFbxObjectPropertyChanged, will receive a signal and take action to process the event data. 
      * \nosubgrouping
      * \see KFbxEvent KFbxEventHandler KFbxListener KFbxEmitter
	  */
    class KFBX_DLL KFbxEventBase
    {
      public:
		 /**
		   * \name Constructor and Destructor
		   */
	     //@{
		 //!Destructor
         virtual ~KFbxEventBase();
		 //@}

		 /** Retrieve the event type ID
		   * \return            type id
		   */
         virtual int GetTypeId() const = 0;

		 /** Force events to give us a name
		   * \return            event name 
		   */
         virtual const char* GetEventName() const = 0;   

		protected:
         static int GetStaticTypeId(char const*);

        private:
         static kfbxmp::KFbxMutex smMutex;
    };

    // Force events to declare a name by using an abstract method, and force them to use 
    // the proper name by making the call from KFbxEvent<> go through the private static
    // method.
    #define KFBXEVENT_DECLARE(Class)                                                    \
      public: virtual const char* GetEventName() const { return FbxEventName(); }       \
      private: static const char* FbxEventName() { return #Class; }                     \
      friend class KFbxEvent<Class>;

    //
    // Similar to above, but to be used when you've got an event template, and the
    // type is something know to FBX
    //
    #define KFBXEVENT_DECLARE_FBXTYPE(Class, FBXType)                                  \
      public: virtual const char* GetEventName() const { return FbxEventName(); }      \
      private:                                                                         \
         static const char* FbxEventName() {                                           \
         static KString lEventName = KString(#Class) + KString("<") +                  \
         GetFbxDataType(FbxTypeOf(*((FBXType const*)0))).GetName() + ">";               \
                                                                                       \
         return lEventName.Buffer();                                                   \
      }                                                                                \
      friend class KFbxEvent< Class<FBXType> >;



    //This is for templates classes that will uses non fbxtypes in their templates
    //We force the the creation of an UNIQUE string for each types so that we can
    //retrieve the event within multiple DLLs

    //to be able to use this, the char EventName[] = "uniqueEventName"; must be declared
    //globally.

    #define KFBXEVENT_TEMPLATE_HEADER_NOT_FBXTYPE(ClassName, TemplateName)\
    template < class TemplateName, const char* T > \
    class ClassName: public kfbxevents:: KFbxEvent< ClassName <TemplateName,T> >\
    {\
        public: virtual const char* GetEventName() const {return FbxEventName();}\
        private: static const char* FbxEventName() {\
        static KString lEventName = (KString(#ClassName) +"<"+ KString(T) +">");\
        return lEventName.Buffer();\
        }\
        friend class KFbxEvent< ClassName<TemplateName, T> >;


    //This is the footer macro, to put at the end to close the template class
    //created by KFBXEVENT_TEMPLATE_HEADER_NOT_FBXTYPE
    #define KFBXEVENT_TEMPLATE_FOOTER_NOT_FBXTYPE()\
    };

    /** FBX event class, derived from KFbxEventBase, and it contains a type ID for event. 
    * It's a template class. You can derive your own types of even. Such as:
    * \code class KFbxEventCustom : public kfbxevents::KFbxEvent<KFbxEventCustom> \endcode
    * \see KFbxObjectPropertyChanged KFbxEventReferencedDocument KFbxEventPostExport
    * \see KFbxEventPostImport KFbxEventPreExport KFbxEventPreImport KFbxEventPopulateSystemLibrary
    * \nosubgrouping
    * \remarks A FBX event is something that is emitted by an emitter, with the goal of being filled by the listener that listen to it. 
    * An object(emitter) can emit a certain type of event, the plug-in(listener) who are listening to that type of event, 
    * will receive a signal and take action to process the event data. 
    * \par The whole process of event is:
    * \li 1. Create an emitter and a listener, then bind them together via the same event handler.
    * \li 2. Emitter can emit an event at certain conditions. The event could be handled by event handler.
    * \li 3. Once an event is emitted, the listener to this event will receive a signal. 
    * \li 4. And then the listener could process the event data according to the types of event, by calling event handler.
    * \note The event data is process by the callback function of event handler.
    * \see KFbxEventBase KFbxEventHandler KFbxListener KFbxEmitter
    */
    //---------------------------------------------------
    // EventT : We use the curiously recurring template pattern
    //          to initialize the typeId of each event type
    template<typename EventT>
    class KFbxEvent : public KFbxEventBase
    {
    public:
        //!Destructor
        virtual ~KFbxEvent(){}

        /** Update the type ID of current event with the given type ID.
        * \param pTypeId     the new type ID.
        */
        static void ForceTypeId(int pTypeId)
        {
            kfbxmp::KFbxMutexHelper lLock( smMutex );

            // This is to handle specific cases where the type ID must be hard coded
            // It is useful for shared event across DLL. We can then guarantee that
            // The ID of a certain type will always have the same ID
            smTypeId = pTypeId;
        }

        /** Retrieve the event type ID
        * \note This may be called from multiple threads.
        * \return            type id
        */
        virtual int GetTypeId() const 
        {
			return GetStaticTypeId();
        }

        /** Retrieve the event type ID
        * \return            type id
        */
        static int GetStaticTypeId() 
        {
            if( !smTypeId )
            {
                kfbxmp::KFbxMutexHelper lLock( smMutex );

                if( !smTypeId )
                {
                    // If this does not compile, you need to add 
                    // KFBXEVENT_DECLARE(YourEventClassName) to your class declaration
                    smTypeId  = KFbxEventBase::GetStaticTypeId(EventT::FbxEventName());
                }
            }

           return smTypeId;
        }

    private:
        //! The type ID of event
        static int smTypeId;
        static kfbxmp::KFbxMutex smMutex;
    };

    // Static members implementation
    template<typename EventT>
    int KFbxEvent<EventT>::smTypeId = 0;
    template<typename EventT>
    kfbxmp::KFbxMutex KFbxEvent<EventT>::smMutex;
}
using namespace kfbxevents;


// FBX namespace end
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXEVENTS_KFBXEVENTS_H

