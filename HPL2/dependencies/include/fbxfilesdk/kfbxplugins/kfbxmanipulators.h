/*!  \file kfbxmanipulators.h
 */
#ifndef FBXFILESDK_KFBXPLUGINS_KFBXMANIPULATORS_H
#define FBXFILESDK_KFBXPLUGINS_KFBXMANIPULATORS_H

/**************************************************************************************

 Copyright (C) 2008 - 2010 Autodesk, Inc. and/or its licensors.
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

#include <fbxfilesdk/kfbxplugins/kfbxobject.h>
#include <fbxfilesdk/kfbxplugins/kfbxcamera.h>
#include <fbxfilesdk/kfbxmath/kfbxvector2.h>
#include <fbxfilesdk/kfbxmath/kfbxvector4.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxCamManip_state;

/** This class can be used to provide basic camera manipulation in any program using this library.
  * \nosubgrouping
  */
class KFBX_DLL KFbxCameraManipulator : public KFbxObject
{
	KFBXOBJECT_DECLARE(KFbxCameraManipulator, KFbxObject);

public:
	enum eAlignment {eNoAlignment, eCameraToLookAt, eLookAtToCamera};
	enum eAction {eNoAction, eOrbit, eDolly, ePan};

    /** Set the camera used for the manipulation.
	*	\param	pCamera		Camera that will be used for the manipulation.
	*	\param	pCamAlign	How the camera should be aligned during camera manipulator initialization. */
	void Initialize(const KFbxCamera& pCamera, eAlignment pCamAlign);

    /** Begin manipulation of the camera.
	*	\param	pAction	The action performed for this manipulation scope.
	*	\param	pMouseX	Horizontal position of the mouse cursor.
	*	\param	pMouseY	Vertical position of the mouse cursor.
	*	\return			If TRUE, manipulation successfully initialized. */
	bool ManipBegin(eAction pAction, int pMouseX, int pMouseY);

    /** Notify manipulation of latest input.
	*	\param	pMouseX	Horizontal position of the mouse cursor.
	*	\param	pMouseY	Vertical position of the mouse cursor. */
	bool ManipNotify(int pMouseX, int pMouseY);

    /** End manipulation. */
	void ManipEnd();

	/** Retrieve current manipulation action. */
	eAction GetCurrentAction() const;

    /** Change camera position and look at to frame all objects.
	*	\remarks	The camera must be in a scene for this function to work properly. */
	void FrameAll();

    /** Change camera position and look at to frame all selected objects.
	*	\remarks	The camera must be in a scene for this function to work properly. */
	void FrameSelected();

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
	virtual KFbxObject& Copy(const KFbxObject& pObject);

private:
	KFbxCameraManipulator(KFbxSdkManager& pManager, char const* pName);
    virtual void Destruct(bool pRecursive, bool pDependents);

	KFbxCamManip_state* mState;
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXMANIPULATORS_H

