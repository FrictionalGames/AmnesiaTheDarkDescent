/****************************************************************************************

   Copyright (C) 2010 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxqueryevent.h
 */

#ifndef FBXFILESDK_KFBXMODULES_KFBXQUERYEVENT_H
#define FBXFILESDK_KFBXMODULES_KFBXQUERYEVENT_H


#include <fbxfilesdk/fbxfilesdk_def.h>

// FBX includes
#include <fbxfilesdk/kfbxevents/kfbxevents.h>

// FBX begin namespace
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>


   /** A query event is something that is emitted by an entity, with the goal of being filled by someone that listen to it. 
    *  You can see that like a form that you send to some people. If those people know how to fill the form, they fill it and return
    *  it to you with the right information in it.  A query event is emitted, and plug-in who are listening to that type of query, 
    *  fill the data that can be accessed by the query emitter.
    */
    template <typename QueryT>
    class KFbxQueryEvent : public kfbxevents::KFbxEvent<KFbxQueryEvent<QueryT> >
    {
    public:
        /**
        *\name Public interface
        */
        //@{
        /** Constructor.
		  * \param pData The requested data.  
          */
        explicit KFbxQueryEvent(QueryT* pData):mData(pData){}

        /** Accessor to a mutable reference to the data. Event are usually const and can't be modified by listener. 
         * This special type of event can have is content modified via this accessor.
         * \return A mutable reference the requested data.
        */
        QueryT& GetData()const { return *mData; }
        //@}

    private:
        mutable QueryT* mData;

    private:
        virtual const char* GetEventName() const { K_ASSERT(false); return ""; }
        static const char* FbxEventName() { K_ASSERT(false); return ""; }
        friend class KFbxEvent< KFbxQueryEvent<QueryT> >;
    };
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMODULES_KFBXQUERYEVENT_H

