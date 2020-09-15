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

#ifndef ED_SCN_WORLD_H
#define ED_SCN_WORLD_H

//----------------------------------------------------------

#include "../Common/EdWorld.h"
#include "../Common/EdClipPlane.h"
#include "../Common/EdScnObject.h"

using namespace hpl;

class cEdScnObjCategory;
class iEdScnObject;

class cEdSurfacePicker;

//----------------------------------------------------------

enum eFogBoolProp
{
	eFogBoolProp_Active,
	eFogBoolProp_Culling,
};

enum eFogFloatProp
{
	eFogFloatProp_Start,
	eFogFloatProp_End,
	eFogFloatProp_FalloffExp
};

//----------------------------------------------------------

class iEdScnObjCategoryDataHandler
{
public:
	iEdScnObjCategoryDataHandler(bool abBypassLoadObjects) { mbBypassLoadObjects=abBypassLoadObjects; }

	bool GetBypassLoadObjects() { return mbBypassLoadObjects; }

	virtual void Save(cXmlElement*){}
	virtual void Load(cXmlElement*){}

protected:
	bool mbBypassLoadObjects;
};

class cEdScnObjCategory
{
public:
	cEdScnObjCategory(const tString& asName);
	~cEdScnObjCategory();

	void SetDataHandler(iEdScnObjCategoryDataHandler*, bool);
	void ClearDataHandler();

	const tString& GetName() { return msName ; }

	bool RegisterType(iEdScnWorld* apWorld, const tWString& asTypeName);

	void PreObjectsSave(cXmlElement* apParentElement);
	void PostObjectsSave();

	void PreObjectsLoad(cXmlElement* apParentElement);
	void Load(iEdScnWorld* apWorld, tScnObjList& alstLoadedObjects);
	void PostObjectsLoad();

	cXmlElement* GetElement() { return mpElement; }

	void SetBypassLoadObjects(bool abX) { mbBypassLoadObjects = abX; }

protected:
	tString msName;
	cXmlElement* mpElement;

	bool mbBypassLoadObjects;

	bool mbDestroyHandler;
	iEdScnObjCategoryDataHandler* mpDataHandler;
};

typedef std::vector<cEdScnObjCategory*> tScnObjCatVec;

//----------------------------------------------------------

class iEdScnWorld : public iEdWorld
{
public:
	iEdScnWorld(iEditor*, const tWString&, const tWString&, const tString&);
	virtual ~iEdScnWorld();

	iEdScnObject* GetScnObject(int);

	cEdSurfacePicker* GetSurfacePicker() { return mpSurfacePicker; }

	bool GetFogActive()				{ return mbFogActive; }
	bool GetFogCulling();
	const cColor& GetFogColor();
	float GetFogStart();
	float GetFogEnd();
	float GetFogFalloffExp();

	void SetShowFog(bool);

	void SetFogActive(bool);
	void SetFogCulling(bool);
	void SetFogColor(const cColor&);
	void SetFogStart(float);
	void SetFogEnd(float);
	void SetFogFalloffExp(float);

	bool GetSkyboxActive()				{ return mbSkyboxActive; }
	const cColor& GetSkyboxColor()		{ return mSkyboxColor; }
	const tString& GetSkyboxTexture()	{ return msSkyboxTexture; }

	void SetShowSkybox(bool);

	void SetSkyboxActive(bool);
	void SetSkyboxColor(const cColor&);
	void SetSkyboxTexture(const tString&);

	////////////////////////////////////////////////
	// Clip Plane management
	int GetFreeClipPlaneID();
	cEdClipPlane* AddClipPlane(int);
	void RemoveClipPlane(int);

	const tClipPlaneMap& GetClipPlanes();
	cEdClipPlane* GetClipPlane(int);
	//cEdClipPlane* CreateClipPlane();

	bool ClipPlaneNumChanged() { return mbNumClipPlanesChanged; }
	void SetClipPlanesUpdated();


	iEdAction* CreateSetFogBoolAction(eFogBoolProp, bool);
	iEdAction* CreateSetFogFloatAction(eFogFloatProp, float);
	iEdAction* CreateSetFogColorAction(const cColor&);

	iEdAction* CreateSetSkyboxActiveAction(bool);
	iEdAction* CreateSetSkyboxColorAction(const cColor&);
	iEdAction* CreateSetSkyboxTextureAction(const tString&);

	////////////////////////////////////////////////
	// Object Category 
	iEdAction* CreateAddClipPlaneAction();
	iEdAction* CreateRemoveClipPlaneAction(int);

	cEdScnObjCategory* CreateCategory(const tString&);
	cEdScnObjCategory* GetCategory(const tString&);

	bool LoadObjects(cXmlElement*, tScnObjList&);
	bool SaveObjects(cXmlElement*, const tScnObjList&);

	void CreateImportDialog();
	void CreateExportDialog();

	void ImportObjects(const tWString&, tScnObjList&);
	void ExportObjects(const tWString&, const tScnObjList&);

	bool ImportObjectsCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ImportObjectsCallback);

	bool ExportObjectsCallback(iWidget*, const cGuiMessageData&);
	kGuiCallbackDeclarationEnd(ExportObjectsCallback);

protected:

	void OnReset();
	void OnUpdate();

	void OnDraw(const cModuleDrawData& aData);

	void OnViewportMouseMove();

	void UpdateSkybox();
	
	bool mbFogUpdated;
	bool mbShowFog;

	bool mbFogActive;

	bool mbSkyboxUpdated;
	bool mbShowSkybox;

	bool mbSkyboxActive;
	cColor mSkyboxColor;
	tString msSkyboxTexture;

	tClipPlaneMap	mmapClipPlanes;
	int mlClipPlaneCounter;
	bool mbNumClipPlanesChanged;

	tScnObjCatVec mvCategories;

	cEdSurfacePicker* mpSurfacePicker;

	tWString msExportFilename;
};

//----------------------------------------------------------

#endif // ED_SCN_WORLD_H
