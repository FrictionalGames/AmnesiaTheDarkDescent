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

#include "LevelEditorStaticObjectCombo.h"
#include "LevelEditorActions.h"
#include "LevelEditorWorld.h"

#include "../common/EntityWrapperStaticObject.h"

#include <algorithm>

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLevelEditorStaticObjectCombo::cLevelEditorStaticObjectCombo(cLevelEditorWorld* apWorld, int alComboID)
{
	mpWorld = apWorld;
	mlComboID = alComboID;

	cParserVarContainer vars;
	mpDrawProg = mpWorld->GetEditor()->GetEngine()->GetGraphics()->CreateGpuProgramFromShaders("ComboDrawProg", "flat_color_vtx.glsl", "flat_color_frag.glsl", &vars);
	mpDrawProg->GetVariableId("gvColor");

	SetColor(cMath::RandRectColor(cColor(0,1), cColor(1,1)));
}

cLevelEditorStaticObjectCombo::~cLevelEditorStaticObjectCombo()
{
	mpWorld->GetEditor()->GetEngine()->GetGraphics()->DestroyGpuProgram(mpDrawProg);
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLevelEditorStaticObjectCombo::AddObject(iEntityWrapper* apObj)
{
	if(IsValidObject(apObj)==false || HasObject(apObj))
		return false;

	mlstEntities.push_back(apObj);

	cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)apObj->GetEntityExtData();
	pData->mlComboID = GetID();

	return true;
}

bool cLevelEditorStaticObjectCombo::RemoveObject(iEntityWrapper* apObj)
{
	if(IsValidObject(apObj)==false || HasObject(apObj)==false)
		return false;

	mlstEntities.remove(apObj);

	cLevelEditorEntityExtData* pData = (cLevelEditorEntityExtData*)apObj->GetEntityExtData();
	pData->mlComboID = -1;

	return true;
}

//-----------------------------------------------------------------------

bool cLevelEditorStaticObjectCombo::HasObject(iEntityWrapper* apObj)
{
	if(IsValidObject(apObj)==false)
		return false;

	return find(mlstEntities.begin(), mlstEntities.end(), apObj) != mlstEntities.end();
}

//-----------------------------------------------------------------------

void cLevelEditorStaticObjectCombo::SetColor(const cColor& aCol)
{
	if(mColor==aCol)
		return;

	mColor = aCol;
	mpDrawProg->SetColor4f(0, mColor);
	mpDrawProg->UnBind();
}

//-----------------------------------------------------------------------

void cLevelEditorStaticObjectCombo::Draw(cEditorWindowViewport* apViewport, cRendererCallbackFunctions* apFunctions)
{
	tEntityWrapperListIt it = mlstEntities.begin();
	for(;it!=mlstEntities.end();++it)
	{
		iEntityWrapper* pEnt = *it;
		pEnt->DrawProgram(apViewport, apFunctions, mpDrawProg, mColor);
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

	for(int i=0;i<(int)vObjIds.size();++i)
	{
		int lID = vObjIds[i];
		iEntityWrapper* pObj = mpWorld->GetEntity(lID);

		AddObject((cEntityWrapperStaticObject*)pObj);
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
	tEntityWrapperList::const_iterator itObjs = mlstEntities.begin();
	for(;itObjs!=mlstEntities.end();++itObjs)
	{
		iEntityWrapper* pEnt = *itObjs;
		sObjIds += cString::ToString(pEnt->GetID()) + " ";
	}
	// Cut space at the end of the string
	sObjIds = cString::Sub(sObjIds, 0, (int)sObjIds.length()-1);

	pData->SetAttributeString("ObjIds", sObjIds);

	return true;
}

//-----------------------------------------------------------------------

iEditorAction* cLevelEditorStaticObjectCombo::CreateActionAddObject(iEntityWrapper* apObj)
{
	if(IsValidObject(apObj)==false ||
		HasObject((cEntityWrapperStaticObject*)apObj))
		return NULL;


	iEditorAction* pAction = hplNew(cLevelEditorActionAddObjectToCombo, (mpWorld, this,(cEntityWrapperStaticObject*)  apObj));

	if(pAction->IsValidAction()==false)
	{
		hplDelete(pAction);
		pAction = NULL;
	}

	return pAction;
}

//-----------------------------------------------------------------------

iEditorAction* cLevelEditorStaticObjectCombo::CreateActionRemoveObject(iEntityWrapper* apObj)
{
	if(IsValidObject(apObj)==false || 
		HasObject(apObj)==false)
		return NULL;


	iEditorAction* pAction = hplNew(cLevelEditorActionRemoveObjectFromCombo, (mpWorld, this, (cEntityWrapperStaticObject*) apObj));

	if(pAction->IsValidAction()==false)
	{
		hplDelete(pAction);
		pAction = NULL;
	}

	return pAction;
}

//-----------------------------------------------------------------------

iEditorAction* cLevelEditorStaticObjectCombo::CreateActionSetColor(const cColor& aCol)
{
    iEditorAction* pAction = hplNew(cLevelEditorActionComboSetColor, (mpWorld, mlComboID, aCol));

	if(pAction->IsValidAction()==false)
	{
		hplDelete(pAction);
		pAction = NULL;
	}

	return pAction;
}

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLevelEditorStaticObjectCombo::IsValidObject(iEntityWrapper* apObj)
{
	return apObj!=NULL &&
			(apObj->GetTypeID()==eEditorEntityType_StaticObject ||
			apObj->GetTypeID()==eEditorEntityType_Primitive);
}


//-----------------------------------------------------------------------


