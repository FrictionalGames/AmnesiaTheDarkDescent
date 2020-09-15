/*
 * Copyright © 2009-2020 Frictional Games
 * 
 * This file is part of Amnesia: The Dark Descent.
 * 
 * Amnesia: The Dark Descent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 

 * Amnesia: The Dark Descent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Amnesia: The Dark Descent.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HPLEDITOR_ENTITY_WRAPPER_DECAL_H
#define HPLEDITOR_ENTITY_WRAPPER_DECAL_H

#include "EntityWrapper.h"

//---------------------------------------------------------------

#define DecalPropIdStart 30

enum eDecalInt
{
	eDecalInt_FileIndex = DecalPropIdStart,
	eDecalInt_CurrentSubDiv,
	eDecalInt_MaxTris,

	eDecalInt_LastEnum
};

enum eDecalFloat
{
	eDecalFloat_Offset = DecalPropIdStart,

	eDecalFloat_LastEnum,
};

enum eDecalBool
{
	eDecalBool_AffectStatic = DecalPropIdStart,
	eDecalBool_AffectPrimitive,
	eDecalBool_AffectEntity,

	eDecalBool_LastEnum,
};

enum eDecalStr
{
	eDecalStr_Material = DecalPropIdStart,

	eDecalStr_LastEnum,
};

enum eDecalVec2f
{
	eDecalVec2f_SubDivs = DecalPropIdStart,

	eDecalVec2f_LastEnum,
};

enum eDecalCol
{
	eDecalCol_Color = DecalPropIdStart,

	eDecalCol_LastEnum,
};

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// Type Decal
//  Holds properties specific to Decals and is responsible of creating
//  Decal data objects
class cEntityWrapperTypeDecal : public iEntityWrapperType
{
public:
	cEntityWrapperTypeDecal(); 

	bool AllowNullEngineEntity() { return true; }

	static float GetDecalOffsetMin() { return mfDecalOffsetMin; }
	static float GetDecalOffsetMax() { return mfDecalOffsetMax; }

	static int GetGlobalMaxTriangles() { return mlGlobalMaxTriangles; }
	static void SetGlobalMaxTriangles(int alX) { mlGlobalMaxTriangles = alX; }

	static void SetForcingUpdate(bool abX) { mbForcingUpdate = abX; }
	static bool IsForcingUpdate() { return mbForcingUpdate; }
protected:
    iEntityWrapperData* CreateSpecificData();

	static float mfDecalOffsetMin;
	static float mfDecalOffsetMax;
	static int mlGlobalMaxTriangles;

	static bool mbForcingUpdate;
};

//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// Decal Data
//  Responsible of loading and saving Decal data, also responsible of creating
//  Decal entity wrappers
class cEntityWrapperDataDecal : public iEntityWrapperData
{
public:
	cEntityWrapperDataDecal(iEntityWrapperType*);

	void CopyFromEntity(iEntityWrapper* apEntity);
	void CopyToEntity(iEntityWrapper* apEntity, int alCopyFlags);
	bool Load(cXmlElement*);
	bool SaveSpecific(cXmlElement*);

	cMesh* GetMesh() { return mpMesh; }

protected:
	iEntityWrapper* CreateSpecificEntity();

	/**
	 * Helper to save stuff in vertex arrays
	 * \param asOutput 
	 * \param alNumElements 
	 * \param apData 
	 */
	void SaveGeometryDataToString(tString& asOutput, int alNumElements, float* apData);

	iEntityWrapper* mpDecal;
	cMesh* mpMesh;
};

//---------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// Decal Entity Wrapper
//  Represents a decal object in a map. Has means of controlling 
//  the mesh output by the engine Decal Creator.
class cEntityWrapperDecal : public iEntityWrapper
{
public:
	cEntityWrapperDecal(iEntityWrapperData*);
	~cEntityWrapperDecal();

	/////////////////////////////////////////////////////////////////////////////////////
	// Property interface
	bool SetProperty(int, const int&);
	bool SetProperty(int, const float&);
	bool SetProperty(int, const bool&);
	bool SetProperty(int, const tString&);
	bool SetProperty(int, const cVector2f&);
	bool SetProperty(int, const cColor&);

	bool GetProperty(int, int&);
	bool GetProperty(int, float&);
	bool GetProperty(int, bool&);
	bool GetProperty(int, tString&);
	bool GetProperty(int, cVector2f&);
	bool GetProperty(int, cColor&);

	void OnPostDeployAll(bool);

	void Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions,iEditorEditMode* apEditMode,
						bool abIsSelected, const cColor& aHighlightCol, const cColor& aDisabledCol);

	cEditorWindowEntityEditBox* CreateEditBox(cEditorEditModeSelect* apEditMode);

	void SetAffectStatic(bool abX);
	void SetAffectPrimitive(bool abX);
	void SetAffectEntity(bool abX);

	const tIntVec& GetAffectedEntityIDs() { return mvAffectedEntityIDs; }
	void SetAffectedEntityIDs(const tIntVec& avEntityIDs) { mvAffectedEntityIDs = avEntityIDs; }

	int GetMaxTriangles() { return mlMaxTriangles; }
	void SetMaxTriangles(int alX);

	const tString& GetMaterial() { return msMaterial; }
	void SetMaterial(const tString& asX);

	float GetOffset() { return mfOffset; }
	void SetOffset(float afX);

	const cColor& GetColor() { return mColor; }
	void SetColor(const cColor& aX);

	const cVector2l& GetUVSubDivisions() { return mvSubDivisions; }
	void SetUVSubDivisions(const cVector2l& avX);

	int GetCurrentSubDiv() { return mlCurrentSubDiv; }
	void SetCurrentSubDiv(int alX);

	int GetFileIndex() { return mlFileIndex; }
	void SetFileIndex(int alIdx) { mlFileIndex = alIdx; }
	
	/**
	 * Recreates geometry if needed
	 */
	void UpdateDecal();

	void SetAbsPosition(const cVector3f& avPosition);
	void SetAbsRotation(const cVector3f& avRotation);
	void SetAbsScale(const cVector3f& avScale, int alAxis=-1);
	void UpdateEntity();

	/**
	 * Sets up the engine decal creator state for creation and returns a non null value if a decal vertex buffer is output
	 * \return 
	 */
	static iVertexBuffer* BuildDecalVertexBuffer(cWorld* apWorld, cDecalCreator* apCreator,
											const cVector3f& avPos, const cVector3f& avSize, float afOffset, 
											const cVector3f& avRight, const cVector3f& avUp, const cVector3f& avFwd,
											const tString& asMaterial, const cColor& aCol,
											const cVector2l& avSubDivs,int alSubDiv, int alMaxTris,
											bool abAffectStaticObject, bool abAffectPrimitive, bool abAffectEntity);

protected:
	cEntityIcon* CreateIcon();
    iEngineEntity* CreateSpecificEngineEntity();

	/**
	 * Helper func to create actual mesh
	 * \return 
	 */
	cMesh* CreateDecalMesh();

	/**
	 * Helper func to cast out unwanted geometry
	 */
	static void IterateRenderableNode(iRenderableContainerNode *apNode, cDecalCreator* apCreator, 
										bool abAffectStaticObject, bool abAffectPrimitive, bool abAffectEntity);

	tString msMaterial;
	float mfOffset;
	cVector2l mvSubDivisions;
	int mlCurrentSubDiv;
	cColor mColor;

	int mlMaxTriangles;
	bool mbAffectsStaticObjects;
	bool mbAffectsEntities;
	bool mbAffectsPrimitives;

	tIntVec mvAffectedEntityIDs;

	int mlFileIndex;

	bool mbDecalUpdated;
	bool mbGeometryUpdated;
	bool mbMaterialUpdated;

	bool mbDeployed;
};

//---------------------------------------------------------------

#endif // HPLEDITOR_ENTITY_WRAPPER_DECAL_H
