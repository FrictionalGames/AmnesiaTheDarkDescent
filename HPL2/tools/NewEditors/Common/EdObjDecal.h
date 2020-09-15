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

#ifndef ED_OBJ_DECAL_H
#define ED_OBJ_DECAL_H

#include "../Common/EdScnObject.h"
#include "../Common/EdUIPicker.h"

//---------------------------------------------------------------

class cEdSurfacePicker;

class cEdObjDecal;

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
class cTypeDecal : public iEdScnObjType
{
public:
	cTypeDecal();

	bool Init();

	bool AllowNullEngineObject() { return true; }

	static float GetDecalOffsetMin() { return mfDecalOffsetMin; }
	static float GetDecalOffsetMax() { return mfDecalOffsetMax; }

	static int GetGlobalMaxTriangles() { return mlGlobalMaxTriangles; }
	static void SetGlobalMaxTriangles(int alX) { mlGlobalMaxTriangles = alX; }

	static void SetForcingUpdate(bool abX) { mbForcingUpdate = abX; }
	static bool IsForcingUpdate() { return mbForcingUpdate; }

	static cFileIndex* mpIndex;

protected:
    iEdObjectData* CreateTypeSpecificData();

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
class cEdObjDecalData : public iEdScnObjData
{
public:
	cEdObjDecalData(iEdObjectType*);

	void CopyFromObject(iEdObject* apEntity);
	void CopyToObject(iEdObject* apEntity, ePropStep);

	bool LoadSpecificData(cXmlElement*);
	bool SaveSpecificData(cXmlElement*);

	cMesh* GetMesh() { return mpMesh; }

	void SetTypeFilter(const tTypeFilterMap& amapFilter) { mmapFilter = amapFilter; }

protected:
	iEdObject* CreateTypeSpecificObject();

	/**
	 * Helper to save stuff in vertex arrays
	 * \param asOutput 
	 * \param alNumElements 
	 * \param apData 
	 */
	void SaveGeometryDataToString(tString& asOutput, int alNumElements, float* apData);

	cEdObjDecal* mpDecal;
	cMesh* mpMesh;
	tTypeFilterMap mmapFilter;
};

//---------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// Decal Entity Wrapper
//  Represents a decal object in a map. Has means of controlling 
//  the mesh output by the engine Decal Creator.
class cEdObjDecal : public iEdScnObject
{
public:
	cEdObjDecal(iEdObjectData*);
	~cEdObjDecal();

	/////////////////////////////////////////////////////////////////////////////////////
	// Property interface
	bool SetProperty(int, const int);
	bool SetProperty(int, const float);
	bool SetProperty(int, const bool);
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

	void OnDraw(const cModuleDrawData&);

	iEdEditPane* CreateEditPane();

	void SetAffectStatic(bool abX);
	void SetAffectPrimitive(bool abX);
	void SetAffectEntity(bool abX);

	void SetAffectedTypes(const tTypeFilterMap& amapFilter) { mmapAffectedTypes = amapFilter; }
	const tTypeFilterMap& GetAffectedTypes() { return mmapAffectedTypes; }

	const tIntVec& GetAffectedEntityIDs() { return mvAffectedEntityIDs; }
	void SetAffectedEntityIDs(const tIntVec& avEntityIDs) { mvAffectedEntityIDs = avEntityIDs; }

	int GetMaxTriangles() { return mlMaxTriangles; }
	void SetMaxTriangles(int alX);

	void SetMatFile(cIndexedFile*);
	cIndexedFile* GetMatFile() { return mpMatFile; }
	tString GetMaterial() { return mpMatFile?mpMatFile->msRelativePath:""; }
	void SetMaterial(const tString& asX);

	float GetOffset() { return mfOffset; }
	void SetOffset(float afX);

	const cColor& GetColor() { return mColor; }
	void SetColor(const cColor& aX);

	const cVector2l& GetUVSubDivisions() { return mvSubDivisions; }
	void SetUVSubDivisions(const cVector2l& avX);

	int GetCurrentSubDiv() { return mlCurrentSubDiv; }
	void SetCurrentSubDiv(int alX);
	
	/**
	 * Recreates geometry if needed
	 */
	void UpdateDecal();

	void SetTranslation(const cVector3f& avPosition);
	void SetRotation(const cVector3f& avRotation);
	void SetScale(const cVector3f& avScale);
	bool OnUpdate(bool);

	/**
	 * Sets up the engine decal creator state for creation and returns a non null value if a decal vertex buffer is output
	 * \return 
	 */
	static iVertexBuffer* BuildDecalVertexBuffer(cWorld* apWorld, cDecalCreator* apCreator,
											const cVector3f& avPos, const cVector3f& avSize, float afOffset, 
											const cVector3f& avRight, const cVector3f& avUp, const cVector3f& avFwd,
											const tString& asMaterial, const cColor& aCol,
											const cVector2l& avSubDivs,int alSubDiv, int alMaxTris,
											cEdSurfacePicker* apPicker, const tTypeFilterMap& amapAffectedTypes);

protected:
	cObjectIcon* CreateIcon();
    iEngineObject* CreateEngineObject();

	/**
	 * Helper func to create actual mesh
	 * \return 
	 */
	cMesh* CreateDecalMesh();

	/**
	 * Helper func to cast out unwanted geometry
	 */
	static void IterateRenderableNode(iRenderableContainerNode *apNode, cDecalCreator* apCreator,
										cEdSurfacePicker* apPicker);

	tString msMaterial;
	float mfOffset;
	cVector2l mvSubDivisions;
	int mlCurrentSubDiv;
	cColor mColor;

	int mlMaxTriangles;
	/*
	bool mbAffectsStaticObjects;
	bool mbAffectsEntities;
	bool mbAffectsPrimitives;
	*/
	tTypeFilterMap mmapAffectedTypes;

	tIntVec mvAffectedEntityIDs;

	cIndexedFile* mpMatFile;

	bool mbDecalUpdated;
	bool mbGeometryUpdated;
	bool mbMaterialUpdated;

	bool mbDeployed;
};

//---------------------------------------------------------------

#endif // ED_OBJ_DECAL_H
