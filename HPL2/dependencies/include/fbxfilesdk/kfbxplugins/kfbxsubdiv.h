#ifndef FBXFILESDK_KFBXPLUGINS_KFBXSUBDIV_H
#define FBXFILESDK_KFBXPLUGINS_KFBXSUBDIV_H

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

#include <fbxfilesdk/kfbxplugins/kfbxgeometry.h>

#include <fbxfilesdk/kfbxmath/kfbxvector4.h>
#include <fbxfilesdk/kfbxmath/kfbxvector2.h>

#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxMesh;
class KFbxSdkManager;

class KFBX_DLL KFbxSubdiv : public KFbxGeometry
{
    KFBXOBJECT_DECLARE(KFbxSubdiv,KFbxGeometry);

public:

    enum SubdivScheme
    {
        CATMULL_CLARK,  //Catmull¨CClark subdivision surface
        DOO_SABIN,      //Doo¨CSabin subdivision surface
        LOOP,           //Loop subdivision surface
        LINEAR,         //Linear subdivision surface 
    };

    enum TessPattern
    {
        ODD_FRACTIONAL,
        EVEN_FRACTIONAL,
        INTEGER,
        POWER2,      //Max, Maya use this one
    };

	enum SubdivDisplaySmoothness
	{
		HULL,
		ROUGH,
		MEDIUM,
		FINE,
	};

    /** InitSubdivLevel Initialize the subdiv
      * \param pLevelCount  number of levels
      * \param pScheme      subdivision scheme
      * \param pPattern     Tessellation pattern
      */
    void InitSubdivLevel(int pLevelCount, 
        SubdivScheme pScheme = CATMULL_CLARK, 
        TessPattern pPattern = POWER2);

    KFbxSubdiv(KFbxSdkManager& pManager, char const* pName);

    virtual void Destruct(bool pRecursive, bool pDependents);

    virtual KFbxNodeAttribute::EAttributeType GetAttributeType() const;


    //max subdivision level number
    static const int MAX_SUBDIV_LEVEL = 16;

    //subdiv levels in subdivision, including the base mesh and each subdiv levels
    KArrayTemplate<KFbxMesh*> mSubDivLevel;

    //Get the base mesh
    KFbxMesh* GetBaseMesh() const; 

    //Get the mesh from finest level
    KFbxMesh* GetFinestMesh() const;

    //Set the finest mesh
    bool SetFinestMesh(KFbxMesh* pMesh);

    //Set the finest mesh
    bool SetBaseMesh(KFbxMesh* pMesh);

    //Get the mesh from specific level
    KFbxMesh* GetMesh(int pLevel) const;

    /** SetSubdivLevelMesh Set certain subdivision mesh
      * \param pLevel   working level
      * \param pMesh    new level mesh. pLevel = 0 means base mesh, 
      pLevel = MaxLevel -1 means finest mesh
      */
    void SetSubdivLevelMesh(int pLevel, KFbxMesh* pMesh);

    int GetLevelCount() const;
    void SetLevelCount(int pLevelCount);

    int GetCurrentLevel() const;
    void SetCurrentLevel(int pCurrentLevel);

	KFbxMesh* GetCurrentMesh() const;

    KFbxSubdiv::SubdivScheme GetSubdivScheme() const;

    KFbxSubdiv::TessPattern GetTessPattern() const;

    void SetSubdivScheme(KFbxSubdiv::SubdivScheme pScheme);

    void SetTessPattern(KFbxSubdiv::TessPattern pPattern);

	KFbxSubdiv::SubdivDisplaySmoothness GetDisplaySmoothness() const;

	void SetDisplaySmoothness(KFbxSubdiv::SubdivDisplaySmoothness pSmoothness);

private:

    //base geometry mesh for subdivision
    KFbxMesh* mBaseMesh;

    //finest geometry mesh for subdivision
    KFbxMesh* mFinestMesh;

    //current operating subdivision level
    int mCurrLevel;

    //number of subdiv level
    int mLevelCount;

    //scheme of subdiv
    SubdivScheme mScheme;

    //pattern of subdiv
    TessPattern mPattern;

	//display smoothness of subdiv
	SubdivDisplaySmoothness mSmoothness;

	


};

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXSUBDIV_H
