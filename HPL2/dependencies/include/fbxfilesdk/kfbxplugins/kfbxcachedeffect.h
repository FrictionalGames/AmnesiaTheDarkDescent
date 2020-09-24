/*!  \file kfbxcachedeffect.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXCACHEDEFFECT_H 
#define FBXFILESDK_KFBXPLUGINS_KFBXCACHEDEFFECT_H

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

#include <fbxfilesdk/kfbxplugins/kfbxnodeattribute.h>
#include <fbxfilesdk/kfbxplugins/kfbxcache.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/** \brief 
 * \nosubgrouping
 */
class KFBX_DLL KFbxCachedEffect : public KFbxNodeAttribute
{
	KFBXOBJECT_DECLARE(KFbxCachedEffect,KFbxNodeAttribute);

public:
    //! Returns the EAttributeType::eCACHED_EFFECT attribute type.
    virtual EAttributeType GetAttributeType() const;
	
     /** \enum ECategory  Effect attribute category. This is for identification purpose and has 
       * no influence inside the FBX SDK. However, applications may use this to filter KFbxCachedEffect
       * objects.
       * \remarks Vertex caches for deforming geometries are not handled by the KFbxCachedEffect object. These
       *          caches are connected with the KFbxVertexCacheDeformer object.
       * \remarks If an object of this class is used as the default NodeAttribute for a KFbxNode and the scene is
       *          saved to an FBX v6 and earlier versions, the CachedEffect attribute is not saved and the KFbxNode will
       *          be processed as a KFbxNull node with default values for the attribute.
      */
    typedef enum {
        ePARTICLES,             /*!<	This effect handles a particle cache. */
        eFLUIDS,                /*!<    This effect handles a fluid cache. */
        eHAIR,                  /*!<    This effect handles an hair cache */
        eGENERIC                /*!<    This effect handles a cache other than particles, fluids or hair. */
    } ECategory;

    //! Return the specialization category of this effect attribute as stored in the . (eGENERIC).
    ECategory GetCategory() const;

	/** Assign a cache object to be used by this attribute.
	  * \param pCache               The cache object.
      * \param pCategory            The "purpose" of this cached effect.
      * \remarks The cache referenced by the \b pCache pointer can freely be shared among
      *          multiple KFbxCachedEffects (and even the KFbxVertexCacheDeformer) therefore
      *          \b pCategory identifier should really only used has a hint of what tis KFbxCachedEffect
      *          represents but it should not be taken for granted that the content of the cache really
      *          matches the category. Applications should always check the cache files to ensure that
      *          they are manipulating the desired information.
      *          
	  */ 
	void SetCache( KFbxCache* pCache, ECategory pCategory = eGENERIC);

	/** Get the cache object used by this deformer.
	  * \return     A pointer to the cache object used by this deformer, or \c NULL if no cache object is assigned.
	  */
	KFbxCache* GetCache() const;

protected:


///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DOXYGEN_SHOULD_SKIP_THIS
public:
	virtual KFbxObject& Copy(const KFbxObject& pObject);

protected:
	KFbxCachedEffect(KFbxSdkManager& pManager, char const* pName);

    virtual void Construct(const KFbxCachedEffect* pFrom);
	virtual bool ConstructProperties(bool pForceSet);

public:
	virtual const char* GetTypeName() const;
	virtual KStringList GetTypeFlags() const;

private:
	void ClearCacheConnections();
    KFbxTypedProperty<ECategory> Category;

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

typedef KFbxCachedEffect* HKFbxCachedEffect;

inline EFbxType FbxTypeOf( KFbxCachedEffect::ECategory const &pItem )           { return eENUM; }

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXEFFECTATTRIBUTE_H

