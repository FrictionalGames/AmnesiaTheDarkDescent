/****************************************************************************************
 
   Copyright (C) 2013 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxvertexcachedeformer.h
#ifndef _FBXSDK_SCENE_GEOMETRY_VERTEX_CACHE_DEFORMER_H_
#define _FBXSDK_SCENE_GEOMETRY_VERTEX_CACHE_DEFORMER_H_

#include <fbxsdk/fbxsdk_def.h>

#include <fbxsdk/scene/geometry/fbxdeformer.h>
#include <fbxsdk/scene/geometry/fbxcache.h>

#include <fbxsdk/fbxsdk_nsbegin.h>

/** \brief This class deforms control points of a geometry using control point positions
 * stored in the associated cache object.
 * \nosubgrouping
 */
class FBXSDK_DLL FbxVertexCacheDeformer : public FbxDeformer
{
	FBXSDK_OBJECT_DECLARE(FbxVertexCacheDeformer, FbxDeformer);

public:
	
	/** Assign a cache object to be used by this deformer.
	  * \param pCache     The cache object.
	  */ 
	void SetCache( FbxCache* pCache );

	/** Get the cache object used by this deformer.
	  * \return     A pointer to the cache object used by this deformer, or \c NULL if no cache object is assigned.
	  */
	FbxCache* GetCache() const;

	/** Select the cache channel by name.
	  * \param pName     The name of channel to use within the cache object.
	  */
	void SetCacheChannel( const char* pName );

	/** Get the name of the selected channel.
      * \return     The name of the selected channel within the cache object.
	  */
	FbxString GetCacheChannel() const;

	/** Activate the deformer.
	  * \param pValue     Set to \c true to enable the deformer.
	  */
	void SetActive( bool pValue );

	/** Indicate if the deformer is active or not.
	  * \return     The current state of the deformer.
	  */
	bool IsActive() const;

	/** Get the deformer type.
	  * \return     Deformer type identifier.
	  */
	virtual EDeformerType GetDeformerType() const { return FbxDeformer::eVertexCache; }

protected:
	/**
	 * \name Properties
	 */
	//@{
		FbxPropertyT<FbxBool>		Active;
		FbxPropertyT<FbxString>	Channel;
        FbxPropertyT<FbxString>    CacheSet;
	//@}

/*****************************************************************************************************************************
** WARNING! Anything beyond these lines is for internal use, may not be documented and is subject to change without notice! **
*****************************************************************************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
public:
    void SetCacheSetName(const char*  pCacheSetName);
    FbxString GetCacheSetName() const;

	virtual FbxObject& Copy(const FbxObject& pObject);
    
	static const char* sChannelPropertyName;
	static const char* sActivePropertyName;
    static const char* sCacheSetPropertyName;
    
protected:
	virtual void ConstructProperties(bool pForceSet);

	virtual FbxStringList	GetTypeFlags() const;

private:
	void ClearCacheConnections();
#endif /* !DOXYGEN_SHOULD_SKIP_THIS *****************************************************************************************/
};

#include <fbxsdk/fbxsdk_nsend.h>

#endif /* _FBXSDK_SCENE_GEOMETRY_VERTEX_CACHE_DEFORMER_H_ */
