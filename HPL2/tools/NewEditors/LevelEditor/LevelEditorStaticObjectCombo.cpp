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

#include "../LevelEditor/LevelEditorStaticObjectCombo.h"
#include "../LevelEditor/HPLMap.h"
#include "../LevelEditor/HPLMapActions.h"

#include "../Common/EdObjStaticObject.h"

#include <algorithm>

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLevelEditorStaticObjectCombo::cLevelEditorStaticObjectCombo(cHplMap* apWorld, int alComboID)
{
	mpWorld = apWorld;
	mlComboID = alComboID;

	cParserVarContainer vars;
	//mpDrawProg = mpWorld->GetEditor()->GetEngine()->GetGraphics()->CreateGpuProgramFromShaders("ComboDrawProg", "flat_color_vtx.glsl", "flat_color_frag.glsl", &vars);
	//mpDrawProg->GetVariableId("gvColor");

	SetColor(cMath::RandRectColor(cColor(0,1), cColor(1,1)));
}

cLevelEditorStaticObjectCombo::~cLevelEditorStaticObjectCombo()
{
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLevelEditorStaticObjectCombo::AddObject(iEdScnObject* apObj)
{
	if(IsValidObject(apObj)==false || HasObject(apObj))
		return false;

	mlstObjects.push_back(apObj);

	cMapObjExtension* pData = static_cast<cMapObjExtension*>(apObj->GetExtension());
	//pData->SetCombo(this, true);

	return true;
}

bool cLevelEditorStaticObjectCombo::RemoveObject(iEdScnObject* apObj)
{
	if(IsValidObject(apObj)==false || HasObject(apObj)==false)
		return false;

	mlstObjects.remove(apObj);

	cMapObjExtension* pData = static_cast<cMapObjExtension*>(apObj->GetExtension());
	//pData->SetCombo(NULL, false);

	return true;
}

//-----------------------------------------------------------------------

bool cLevelEditorStaticObjectCombo::HasObject(iEdScnObject* apObj)
{
	if(IsValidObject(apObj)==false)
		return false;

	return find(mlstObjects.begin(), mlstObjects.end(), apObj) != mlstObjects.end();
}

//-----------------------------------------------------------------------

void cLevelEditorStaticObjectCombo::SetColor(const cColor& aCol)
{
	if(mColor==aCol)
		return;

	mColor = aCol;
}

//-----------------------------------------------------------------------

void cLevelEditorStaticObjectCombo::OnDraw(const cModuleDrawData& aData)
{
	if(aData.mStep==eEdDrawStep_PostTrans) return;

	cColor col = mColor;
	col.a = 0.5f;
	tScnObjListIt it = mlstObjects.begin();
	for(;it!=mlstObjects.end();++it)
	{
		iEdScnObject* pObj = *it;

		pObj->DrawObject(aData, true, col);
	}
}

//-----------------------------------------------------------------------

bool cLevelEditorStaticObjectCombo::Load(cXmlElement* apElement)
{
	mlComboID = apElement->GetAttributeInt("ID", mlComboID);
	mColor = apElement->GetAttributeColor("Color", mColor);

	//////////////////////////////////////////
	// Load combined object ids
	tString sObjIds = apElement->GetAttributeString("ObjIds");
	tIntVec vObjIds;
	cString::GetIntVec(sObjIds, vObjIds);

	for(size_t i=0;i<vObjIds.size();++i)
	{
		int lID = vObjIds[i];
		iEdScnObject* pObj = static_cast<iEdScnObject*>(mpWorld->GetObject(lID));

		AddObject(pObj);
	}
	
	return true;
}

bool cLevelEditorStaticObjectCombo::Save(cXmlElement* apElement)
{
	cXmlElement* pData = apElement->CreateChildElement("Combo");
	pData->SetAttributeInt("ID", mlComboID);
	pData->SetAttributeColor("Color", mColor);

	//////////////////////////////////////////
	// Save combined object ids
	tString sObjIds;
	tScnObjList::const_iterator itObjs = mlstObjects.begin();
	for(;itObjs!=mlstObjects.end();++itObjs)
	{
		iEdScnObject* pObj= *itObjs;
		sObjIds += cString::ToString(pObj->GetID()) + " ";
	}
	// Cut space at the end of the string
	sObjIds = cString::Sub(sObjIds, 0, (int)sObjIds.length()-1);

	pData->SetAttributeString("ObjIds", sObjIds);

	return true;
}

//-----------------------------------------------------------------------

iEdAction* cLevelEditorStaticObjectCombo::CreateActionAddObject(iEdScnObject* apObj)
{
	if(HasObject(apObj))
		return NULL;


	iEdAction* pAction = hplNew(cEdActionComboAddObject, (this, apObj));

	return pAction;
}

//-----------------------------------------------------------------------

iEdAction* cLevelEditorStaticObjectCombo::CreateActionRemoveObject(iEdScnObject* apObj)
{
	if(IsValidObject(apObj)==false || 
		HasObject(apObj)==false)
		return NULL;


	iEdAction* pAction = hplNew(cEdActionComboRemoveObject, (this, apObj));

	return pAction;
}

//-----------------------------------------------------------------------

iEdAction* cLevelEditorStaticObjectCombo::CreateActionSetColor(const cColor& aCol)
{
	return hplNew(cEdActionComboSetColor, (this, aCol));
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLevelEditorStaticObjectCombo::IsValidObject(iEdScnObject* apObj)
{
	return apObj!=NULL;// &&
			//(apObj->GetTypeID()==eEditorEntityType_StaticObject ||
			//apObj->GetTypeID()==eEditorEntityType_Primitive);
}


//-----------------------------------------------------------------------


