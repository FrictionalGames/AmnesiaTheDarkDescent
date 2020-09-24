/****************************************************************************************

Copyright (C) 2010 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

/*!  \file kfbxpropertydef.h
 */

#ifndef FBXFILESDK_FBXCORE_KFBXPROPERTYDEF_H
#define FBXFILESDK_FBXCORE_KFBXPROPERTYDEF_H

#include <fbxfilesdk/fbxfilesdk_def.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

	typedef int					kFbxPropertyId;
	const   kFbxPropertyId		kFbxProperyIdNull = -1;
	const   kFbxPropertyId		kFbxProperyIdRoot = 0;

	enum	KFbxInheritType		{ eFbxOverride=0,eFbxInherit=1,eFbxDeleted=2 } ;

    enum kFbxConnectionType
    { 
	    eFbxConnectionNone				= 0,

		// System or user
		eFbxConnectionSystem			= 1 << 0,
	    eFbxConnectionUser				= 1 << 1,
	    eFbxConnectionSystemOrUser		= eFbxConnectionUser | eFbxConnectionSystem,

		// Type of Link
	    eFbxConnectionReference			= 1 << 2,
	    eFbxConnectionContains			= 1 << 3,
	    eFbxConnectionData				= 1 << 4,
	    eFbxConnectionLinkType			= eFbxConnectionReference | eFbxConnectionContains | eFbxConnectionData,

	    eFbxConnectionDefault			= eFbxConnectionUser | eFbxConnectionReference,


	    eFbxConnectionUnidirectional    = 1 << 7
    };

    class FbxPropertyFlags
    {
    public:
        /** Flags of FBX property.
          */
        enum eFbxPropertyFlags
        {
            eNO_FLAG    	= 0,     //!< No flag.
            eANIMATABLE 	= 1,     //!< This property can be animated.
            eUSER       	= 1<<1,  //!< This property is user-defined.
            eTEMPORARY  	= 1<<2,  //!< System property.
            ePUBLISHED		= 1<<3,  //!< This property has been connected to an animation curve node.
            ePSTATIC		= 1<<4,

            eNOT_SAVABLE	= 1<<5,  //!< This property will not be written to FBX file.
            eHIDDEN     	= 1<<6,  //!< This property is hidden in UI.

            eUI_DISABLED	= 1<<7,  //!< for dynamic UI
            eUI_GROUP       = 1<<8,  //!< for dynamic UI
            eUI_BOOLGROUP   = 1<<9,  //!< for dynamic UI
            eUI_EXPANDED    = 1<<10, //!< for dynamic UI
            eUI_NOCAPTION   = 1<<11, //!< for dynamic UI
            eUI_PANEL       = 1<<12, //!< for dynamic UI
            eUI_LEFTLABEL   = 1<<13, //!< for dynamic UI
            eUI_HIDDEN      = 1<<14  //!< for dynamic UI
        };

        /** Query the count of flags.
          * \return The count.
          */
        inline static int GetFlagCount() { return 16; }

        /** Get a flag which is the result of logical OR operator of all the flags.
          * \return The result flag.
          */
        inline static eFbxPropertyFlags AllFlags()
        {
            eFbxPropertyFlags lAllFlags = eNO_FLAG;

            for( int i = 0; i < GetFlagCount()-1; ++i )
                lAllFlags = (eFbxPropertyFlags) ( (lAllFlags << 1) | 1 );

            return lAllFlags;
        }
    };

	/**************************************************************************
    * Filter management
    **************************************************************************/
	class	KFbxConnectionPoint;
	typedef int kFbxFilterId;

    /** Class to manage ConnectFilter.
      * \nosubgrouping
      */
	class KFBX_DLL KFbxConnectionPointFilter
	{
		// CONSTRUCTOR/DESTRUCTOR
		/**
		* \name Constructor and Destructor
		*/
		//@{
	public: 
		//! Constructor
		KFbxConnectionPointFilter() { }
		//! Destructor
		virtual ~KFbxConnectionPointFilter();
		//@}
	public:
		/**
		* \name ConnectFilter management
		*/
		//@{

		//! Return reference ConnectionPoint filter.
		virtual KFbxConnectionPointFilter*		Ref();
		//! Cancel reference
		virtual void							Unref();

		//! Get unique filter ID
		virtual kFbxFilterId					GetUniqueId() const { return 0; }

		/** Judge if the given Connection Point is valid
		  * \param pConnect The given Connection Point.
		  * \return \c True if valid, \c false if not valid.
		  */
		virtual bool							IsValid				(KFbxConnectionPoint*	pConnect) const;
		/** Judge if the given Connection Point is a valid connection
		  * \param pConnect The given Connection Point.
		  * \param pType Connection type.
		  * \return \c True if valid, \c false if not valid.
		  */
		virtual bool							IsValidConnection	(KFbxConnectionPoint*	pConnect,kFbxConnectionType pType) const;
		/** Judge if it is equal with the given  ConnectionPoint filter.
		  * \param pConnectFilter The given  ConnectionPoint filter.
		  * \return \c True if equal, \c false if unequal.
		  */
		virtual bool							IsEqual				(KFbxConnectionPointFilter*	pConnectFilter)	const;

		//@}
	};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_FBXCORE_KFBXPROPERTYDEF_H

