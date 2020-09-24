/*!  \file kfbxblendshape.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXBLENDSHAPE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXBLENDSHAPE_H

/**************************************************************************************

 Copyright (C) 2010 Autodesk, Inc. and/or its licensors.
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

#include <fbxfilesdk/kfbxplugins/kfbxdeformer.h>
#include <fbxfilesdk/kfbxmath/kfbxmatrix.h>
#include <fbxfilesdk/kfbxplugins/kfbxgroupname.h>

#include <fbxfilesdk/components/kbaselib/klib/kerror.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxSdkManager;
class KFbxGeometry;
class KFbxBlendShapeChannel;
class KFbxNode;

/** FBX SDK blend shape deformer class
  * \nosubgrouping
  */

class KFBX_DLL KFbxBlendShape : public KFbxDeformer
{
    KFBXOBJECT_DECLARE(KFbxBlendShape,KFbxDeformer);

public:
    /** Set the geometry affected by this blend shape deformer.
      * \param pGeometry               Pointer to the geometry object to set.
      * \return                        \c true on success, \c false otherwise.
	  * \remarks                       One blend shape deformer can only be used
	  * on one base geometry. So when SetGeometry is called, pGeometry will replace
	  * the last one. 
      */
    bool SetGeometry(KFbxGeometry* pGeometry);

    /** Get the geometry affected by this blend shape deformer.
      * \return                        a pointer to the geometry if set or NULL.
      */
    KFbxGeometry* GetGeometry();

    /** Add a blend shape channel.
      * \param pBlendShapeChannel      Pointer to the blend shape channel object to add.
      * \return                        \c true on success, \c false otherwise.
      */
    bool AddBlendShapeChannel(KFbxBlendShapeChannel* pBlendShapeChannel);

    /** Remove the given blend shape.
      * \param pBlendShapeChannel      Pointer to the blend shape channel to remove from this blend shape deformer.
      * \return                        Pointer to the blend shape channel or \c NULL if pBlendShapeChannel is not owned by this blend shape deformer.
      */
    KFbxBlendShapeChannel* RemoveBlendShapeChannel(KFbxBlendShapeChannel* pBlendShapeChannel);

    /** Get the number of blend shape channels.
      * \return                        Number of blend shape channels that have been added to this object.
      */
    int GetBlendShapeChannelCount() const;

    /** Get blend shape channel at given index.
      * \param pIndex                  Index of the blend shape channel.
      * \return                        Pointer to the blend shape channel or \c NULL if index is out of range.
      */
    KFbxBlendShapeChannel* GetBlendShapeChannel(int pIndex);

    /** Get the blend shape channel at given index.
      * \param pIndex                  Index of the blend shape channel.
      * \return                        Pointer to the blend shape channel or \c NULL if index is out of range.
      */
    KFbxBlendShapeChannel const* GetBlendShapeChannel(int pIndex) const;

    /** Get the type of the deformer.
      * \return                        Deformer type identifier.
      */
    EDeformerType GetDeformerType()  const {return eBLENDSHAPE; };

	/** Restore the blend shape deformer to its initial state.
	* Calling this function will do the following:
	* \li Clear the pointer to base geometry.
	* \li Remove all the blend shape channels.
	*/
	void Reset();

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//  Anything beyond these lines may not be documented accurately and is
//  subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS

    virtual KFbxObject& Copy(const KFbxObject& pObject);

protected:
    KFbxBlendShape(KFbxSdkManager& pManager, char const* pName);

    virtual KStringList GetTypeFlags() const;

    friend class KFbxScene;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXBLENDSHAPE_H

