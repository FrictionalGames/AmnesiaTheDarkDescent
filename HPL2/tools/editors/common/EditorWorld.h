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

#ifndef HPLEDITOR_EDITOR_WORLD_H
#define HPLEDITOR_EDITOR_WORLD_H

#include "../common/StdAfx.h"

using namespace hpl;

#include "EditorTypes.h"

//----------------------------------------------------------------------

class iEditorBase;
class iEntityWrapperType;
class iEntityWrapperData;
class iEntityWrapper;


class cEntityPicker;
class cSurfacePicker;

//----------------------------------------------------------------------

class iEditorWorld
{
public:
	iEditorWorld(iEditorBase* apEditor, const tString& asElementName);
	virtual ~iEditorWorld();

	virtual void OnEditorUpdate();
	virtual void Reset();

	void SetName(const tString& asName);
	tString& GetName() { return msName; }

	iEditorBase* GetEditor() { return mpEditor; }
	cWorld* GetWorld() { return mpWorld; }

	cEntityPicker* GetPicker() { return mpPicker; }
	cSurfacePicker* GetSurfacePicker() { return mpSurfacePicker; }

	////////////////////////////////////////////////////////
	// ID management
	bool IsIDInUse(int alID);
	int GetFreeID () { return mlIDCounter++; }
	int AddIDRedirection(int alOldID);

	////////////////////////////////////////////////////////
	// Entity management
	virtual bool AddObject(iEntityWrapper* apObject);
	virtual void RemoveObject(iEntityWrapper* apObject);

	iEntityWrapper* GetEntity(int alID);
	iEntityWrapper* GetEntityByName(const tString& asName);
	tEntityWrapperMap& GetEntities() { return mmapEntities; }

	bool HasEntity(iEntityWrapper* apObject);

	bool IsClearingEntities() { return mbIsClearingEntities; }
	void ClearEntities();

	////////////////////////////////////////////////////////
	// Loading / Saving
	virtual bool Load(iXmlDocument* apXmlDoc);
	virtual cXmlElement* GetWorldDataElement(iXmlDocument* apXmlDoc)=0;
	virtual void LoadWorldData(cXmlElement* apWorldDataElement);
	virtual cXmlElement* GetWorldObjectsElement(cXmlElement* apWorldDataElement)=0;
	virtual void LoadWorldObjects(cXmlElement* apWorldObjectsElement);
	virtual bool CustomCategoryLoader(cXmlElement* apWorldObjectsElement, cXmlElement* apCategoryElement) { return false; }

	void SetShowLoadErrorPopUp() { mbShowLoadErrorPopUp = true; }
	void SetLoadErrorMessage(const tWString& asX) { msLoadErrorMsg = asX; }

	virtual bool Save(iXmlDocument* apXmlDoc);
	virtual cXmlElement* CreateWorldDataElement(iXmlDocument* apXmlDoc)=0;
	virtual void SaveWorldData(cXmlElement* apWorldDataElement);
	virtual cXmlElement* CreateWorldObjectsElement(cXmlElement* apWorldDataElement)=0;
	virtual void SaveWorldObjects(cXmlElement* apWorldObjectsElement, tEntityWrapperList& alstEntities);
	virtual bool CustomCategorySaver(cXmlElement* apWorldObjectsElement) { return false; }


	virtual void ImportObjects(const tString& asX, tIntList& alstImportedIDs);
	virtual void ExportObjects(const tString& asX, tEntityWrapperList& alstEntsToExport);


	bool IsModified() { return mlNumModifications!=mlLastSavedModification; }
	unsigned int GetNumModifications() { return mlNumModifications; }
	unsigned int GetLastSavedModification() { return mlLastSavedModification; }
	void UpdateSavedModifications() { mlLastSavedModification = mlNumModifications; }
	void IncModifications();
	void DecModifications();

	///////////////////////////////////////////////////////
	// Object Naming
    bool IsNameAvailable(const tString& asName);

	tString GenerateName(const tString& asBaseName);

	///////////////////////////////////////////////////////
	// Object Creation
	iEntityWrapper* CreateEntityWrapperFromData(iEntityWrapperData* apData);
	iEntityWrapper* CreateEntityWrapperFromXMLElement(cXmlElement* apElement);

	void SetUpEntityWrapper(iEntityWrapper* apEntity,
							const cVector3f& avPosition=0,
							const cVector3f& avRotation=0,
							const cVector3f& avScale=1);

	void DestroyEntityWrapper(iEntityWrapper* apEntity, bool abRemoveFromWorld=true);

	void AddEntityType(iEntityWrapperType* apType);
	iEntityWrapperType* GetEntityTypeByName(const tString&);
	iEntityWrapperType* GetEntityTypeByID(int alType);
	iEntityWrapperType* GetEntityTypeByElement(cXmlElement*);

	int GetEntityTypeNum();
	iEntityWrapperType* GetEntityType(int alX);

	////////////////////////////////////////////////////////////////
	// Data / Entity callbacks for a specific world

	virtual void CreateDataCallback(iEntityWrapperData* apData) {}
	virtual void CopyDataFromEntityCallback(iEntityWrapperData* apData, iEntityWrapper* apEnt) {}
	virtual void CopyDataToEntityCallback(iEntityWrapperData* apData, iEntityWrapper* apEnt, int alCopyStep) {}
	virtual void DestroyDataCallback(iEntityWrapperData* apData) {}

	virtual void SaveDataCallback(iEntityWrapperData* apData, cXmlElement* apElement) {}
	virtual void LoadDataCallback(iEntityWrapperData* apData, cXmlElement* apElement) {}

	virtual void DestroyEntityWrapperCallback(iEntityWrapper* apEnt) {}

	///////////////////////////////
	// Global Light stuff
	void SetGlobalAmbientLightEnabled(bool abX);
	void SetGlobalPointLightEnabled(bool abX);

	bool GetGlobalAmbientLightEnabled();
	bool GetGlobalPointLightEnabled();

	///////////////////////////////
	// Object Visibility
	void SetTypeVisibility(int alTypeID, bool abX);
	bool GetTypeVisibility(int alTypeID);

	void SetVisibilityUpdated() { mbVisibilityUpdated = true; }
	void UpdateVisibility();

	void SetTypeActive(int alTypeID, bool abX);
	bool GetTypeActive(int alTypeID);

	///////////////////////////////
	// Clip planes
	cEditorClipPlane* AddClipPlane();
	void RemoveClipPlane(int alIdx, bool abDestroy=true);
	void RemoveClipPlane(cEditorClipPlane* apPlane, bool abDestroy=true);
	void ClearClipPlanes(bool abDestroyPlanes=true);

	cEditorClipPlane* GetClipPlane(int alIdx) 
	{
		if(alIdx<0 || alIdx>(int)mvClipPlanes.size()) return NULL; 
		else return mvClipPlanes[alIdx]; 
	}
	tEditorClipPlaneVec& GetClipPlanes() { return mvClipPlanes; }
	void SetClipPlanes(const tEditorClipPlaneVec& avX);
	void SetClipPlanesUpdated() { mbClipPlanesUpdated=true; }

	bool ClipPlaneNumUpdated() { bool bUpdated = mbClipPlaneNumUpdated; mbClipPlaneNumUpdated = false; return bUpdated; }


	int AddEntityCategory(const tString& asX, int alType);
	void AddTypeToCategory(int alCatID, int alType);
	cXmlElement* GetParentElementForObject(std::vector<cXmlElement*>& avParentElements, iEntityWrapper* apEnt);

	///////////////////////////////
	// Filename indices
	void AddFileIndex(const tString& asName, int alTypeID);
	int AddFilenameToIndex(const tString& asIndexName, const tString& asFilename);
	int AddFilenameToIndex(tStringVec& avIndex, const tString& asFilename);
	const tString& GetFilenameFromIndex(const tString& asName, int alIndex);
	tString GetFileRelativePath(const tString& asX);

	void LoadFileIndices(cXmlElement* apWorldObjectsElement);
	void SaveFileIndices(cXmlElement* apWorldObjectsElement);

	void SanitizeFileIndex(tStringVec& avFileIndices, int alTypeID);
	void RelocateFileIndex(int alTypeID, int alOldIndex, int alNewIndex);

	void SetBGDefaultColor(const cColor& aX);
	const cColor& GetBGDefaultColor() { return mBGDefaultColor; }
	///////////////////////////////
	// Skybox
	void SetShowSkybox(bool abX);
	void SetSkyboxActive(bool abX);
	void SetSkyboxTexture(const tString& asX);
	void SetSkyboxColor(const cColor& aX);

	void UpdateSkybox();

	bool GetShowSkybox() { return mbShowSkybox; }
	bool GetSkyboxActive() { return mbSkyboxActive; }
	const tString& GetSkyboxTexture() { return msSkyboxTexture; }
	const cColor& GetSkyboxColor() { return mSkyboxColor; }

	///////////////////////////////
	// Fog
	void SetShowFog(bool abX);
	void SetFogActive(bool abX);
	void SetFogCulling(bool abX);
	void SetFogStart(float afX);
	void SetFogEnd(float afX);
	void SetFogFalloffExp(float afX);
	void SetFogColor(const cColor& aX);

	bool GetShowFog() { return mbShowFog; }
	bool GetFogActive() { return mbFogActive; }
	bool GetFogCulling();
	float GetFogStart();
	float GetFogEnd();
	float GetFogFalloffExp();
	const cColor& GetFogColor();

	const cVector3f& GetMinPosition() { return mvMinPos; }
	const cVector3f& GetMaxPosition() { return mvMaxPos; }
	void AddOutlierEntity(iEntityWrapper* apEnt);

protected:
	///////////////////////////////////////////////////////
	// Data
	iEditorBase* mpEditor;
	cWorld* mpWorld;
	tString msElementName;

	tString msName;

	bool mbIsClearingEntities;

	unsigned int mlNumModifications;
	unsigned int mlLastSavedModification;

	tEntityWrapperMap mmapEntities;

	int mlIDCounter;
	
	int mlDefaultCategory;
	std::vector<tString> mvEntityCategories;
	std::vector<tIntList> mvEntityCategoryTypes;

	tEntityTypeList mlstEntityTypes;

	std::map<int, int> mmapIDRedirectors;

	tStringVec mvFileIndexNames;
	tIntVec mvFileIndexTypeIDs;
		
	std::map<tString,tStringVec> mmapFileIndices;

	bool mbVisibilityUpdated;

	cEntityPicker* mpPicker;
	cSurfacePicker* mpSurfacePicker;

	////////////////////////////////
	// Global Lights
	cLightBox* mpGlobalAmbientLight;
	cLightPoint* mpGlobalPointLight;

	bool mbWorldLightsEnabled;
	bool mbWorldPSEnabled;

	////////////////////////////////
	// Skybox
	cColor mBGDefaultColor;

	bool mbShowSkybox;
	bool mbSkyboxActive;
	cColor mSkyboxColor;
    tString msSkyboxTexture;

	bool mbSkyboxTextureUpdated;

	////////////////////////////////
	// Fog
	bool mbShowFog;
	bool mbFogActive;

	////////////////////////////////
	// Clip Planes
	bool mbClipPlanesUpdated;
	bool mbClipPlaneNumUpdated;
	tEditorClipPlaneVec mvClipPlanes;

	cEditorClipPlane* mpClipPlane;

	cVector3f mvMinPos;
	cVector3f mvMaxPos;

	tEntityWrapperList mlstOutliers;

	bool mbShowLoadErrorPopUp;
	tWString msLoadErrorMsg;
};

//----------------------------------------------------------------------

#endif //HPLEDITOR_EDITOR_WORLD_H
