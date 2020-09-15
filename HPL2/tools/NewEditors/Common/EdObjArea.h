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

#ifndef ED_OBJECT_AREA_H
#define ED_OBJECT_AREA_H

#include "../Common/EdScnObject.h"

class cEdWindowViewport;

//---------------------------------------------------------------------

#define AreaPropIdStart 10

//---------------------------------------------------------------------

enum eAreaStr
{
	eAreaStr_Mesh = AreaPropIdStart,
	eAreaStr_Type,

	eAreaStr_LastEnum
};

//---------------------------------------------------------------------

class cTypeRootArea : public iUserTypeRoot
{
public:
	cTypeRootArea();

	iEdObjectType* GetTypeByXmlElement(cXmlElement*);

protected:
	iUserTypeNode* CreateSpecificUserTypeNode(iEdClass*);

	iEdObjectData* CreateTypeSpecificData() { return NULL; }
};

class cTypeArea : public iUserTypeNode
{
public:
	cTypeArea(iUserTypeRoot*, iEdClass*);

	const cColor& GetColor() { return mColor; }
	bool GetShowOrientation() { return mbShowOrientation; }
	bool GetDrawAsSphere() { return mbDrawAsSphere; }
	const cVector3f& GetDefaultSize() { return mvDefaultSize; }
	const tString& GetIcon() { return msIcon; }

protected:
	iEdObjectData* CreateTypeSpecificData();

	cColor mColor;
	bool mbShowOrientation;
	bool mbDrawAsSphere;
	cVector3f mvDefaultSize;
	tString msIcon;
};

//---------------------------------------------------------------------

class cEdObjAreaData : public iUserObjData
{
public:
	cEdObjAreaData(iEdObjectType*);

	bool SaveSpecificData(cXmlElement*);
	
protected:
	iEdObject* CreateTypeSpecificObject();
};

//---------------------------------------------------------------------

//---------------------------------------------------------------------

class cEdObjArea : public iUserObject
{
public:
	cEdObjArea(iEdObjectData*);
	virtual ~cEdObjArea();

	bool SetProperty(int, const tString&);

	bool GetProperty(int, tString&);

	void UpdateEntity();

	const tString& GetAreaType();
	void SetAreaType(const tString& asX);

	const tString& GetMeshFile() { return msMeshFile; }
	void SetMeshFile(const tString& asX);

	//void Draw(iEdViewport* apViewport, cRendererCallbackFunctions* apFunctions, iEdEditMode* apEditMode, bool abIsSelected, const cColor& aHighlightCol=cColor(1,1), const cColor& aDisabledCol=cColor(1,0,0,1));

	iEdEditPane* CreateEditPane();
protected:
	void OnDraw(const cModuleDrawData&);

	bool OnUpdate(bool);

	cObjectIcon* CreateIcon();
	void OnSetVar(const tWString& asName, const tWString& asValue) {}

	//////////////////////
	// Data
	tString msMeshFile;
	cMeshEntity* mpMesh;
	cMaterial*	mpMat;
};

//---------------------------------------------------------------------

#endif // ED_OBJECT_AREA_H
